/**
 * @file FaultLogs.c
 * @details Implementation of Fault Log interaction API
 *
 * The first page of each external flash sector reserved for fault logs, is a header that
 * contains a binary count of how many fault logs have been written into that particular
 * sector. Since you can only clear bits, not set them without erasing the full sector,
 * a bit value of 0 is what indicates 1 fault log has been stored.
 *
 * See the \#defines within this file to get the sizes of each header and a single fault log entry.
 * See the \#defines in externalFlash.c and externalFlash.h to figure out the sizes of each sector
 * and page. The current understanding is no matter what an entry size is each page is of size 256 bytes
 * and there are 256 pages per sector.
 *
 * @see FaultLogs.h to see public API implementation
 * @todo modify this code so that a fault log can have its values updated by a different piece of code
 */


#include <assert.h>
#include "FreeRTOS.h"
#include "semphr.h"


#include "fsl_rtc.h"
#include "fsl_debug_console.h"
#include "Peripheral/RTC/rtcInterface.h"
#include "externalFlash.h"
#include "FaultLogs.h"

  /*****************************************************************************
   * Definitions ===============================================================
   ****************************************************************************/
#define FIRST_SECTOR_FAULT_LOG      (0)    /**< First sector number reserved for fault logs*/
#define SIZE_OF_HEADING				(256)  /**< Amount of uint8_t values within the sector heading */
#define LOC_OF_LAST_LOG_CNT         (0x1F) /**< Index within the array of uint8_t of the sector heading that will indicate how full a sector is */
#define HEADER_SECTOR_FULL_VALUE    (0x80) /**< Value within the indicating uint8_t that signifies the sector has been filled */
/*******************************************************************************
 * Prototypes/Structs/externs =================================================
 ******************************************************************************/
/**
 * @brief external flash sector "header" structure
 * @details extf entry that indicates how full the sector is. This gets written
 * size is currently 256 bytes
 * and updated into the first (0) index of each fault log sector.
 */
typedef struct _SectorHeading_s{
	uint8_t logsWritten[SIZE_OF_HEADING]; //1*256
}sSectorHeading_t;/*256*/

/*****************************************************************************
 * Variables =================================================================
 ****************************************************************************/
static sLogRecordLocation_t tl_LogStorageLocation;     /**< indicates which sector and inner sector index the fault log should be written into */
static sLogRecord_t *tl_ptrLogData = NULL;             /**< Current fault log that will be written when write is triggered*/
volatile uint8_t tl_RecordADCDataFlag = 1;             /**< flag to allow ADC storage */
static SemaphoreHandle_t tl_FaultLogNotifySem = NULL;  /**< semaphore in charge of protecting conflicting notifications to the task in charge of writing fault logs*/
static sSectorHeading_t *tl_ptrSectorHeading = NULL;   /**< value that holds the header that indicates how many logs have been written into a single sector */

/*****************************************************************************
 * Private Methods ===========================================================
 ****************************************************************************/

/**
 * @brief Find where in external flash to begin storing fault logs
 *
 * The first page within each sector contains variables that contain how many logs are currently
 * stored within that sector. A bit value of 0 indicates 1 fault log has been stored.
 *
 * For example a value of 0x3 indicates that 2 fault logs have been written into the particular
 * sector of flash.
 *
 * @param LogSize number of pages needed to store a log record to
 * @param LogsPerSector number of logs that can be stored in a sector
 * @return void
 */
static void FindWhereToStoreLog( sLogRecordLocation_t *Location,uint32_t PagesPerLogSize, uint32_t LogsPerSector)
{
	uint32_t sectorNumber=65;
	uint32_t logRecordCnt=0;
	uint32_t readAddress;
	uint16_t idx;
	uint8_t logCntToParse = 0;
	uint8_t data[EXTF_PAGE_SIZE+1];

	/*
	 * read the "header" log in each sector of memory until we find a sector
	 * that isn't full.
	 */
	for(idx = FIRST_SECTOR_FAULT_LOG; idx < NUM_FAULT_SECTORS; idx++){
		readAddress = idx *EXTF_SECTOR_SIZE;
		ExtFlashPageRead(readAddress, &data[0]);
		vTaskDelay(1);
		if(data[LOC_OF_LAST_LOG_CNT] != HEADER_SECTOR_FULL_VALUE)
		{
			sectorNumber = idx;
			break;
		}
		vTaskDelay(1);
	}

	/*
	 * If all fault log sectors are full, clear the first fault log sector and indicate this
	 * is where the faults should being to be written
	 */
	if(idx == NUM_FAULT_SECTORS)
	{
		//this means that all sectors are full, clear the first sector and start from there
		sectorNumber = FIRST_SECTOR_FAULT_LOG;
		ExtfSectorErase(sectorNumber);
		Location->idxSector = sectorNumber;
		Location->logIdxIntoSector = 1;
		return;
	}

	/*
	 * if you found a sector that is not completely full, determine how many
	 * uint8_t values within the header have been fully cleared.
	 *
	 * (8 logs per 1 uint8_t that is a value of 9)
	 */
	idx = 0;
	logRecordCnt = 0; //log record count starts at 1 because the first log is always taken by the "header"
	while(idx < Location->logsPerSector)
	{

		if(data[idx] == 0x0)
		{
			logRecordCnt+=8;
			idx++;
		}else{
			//we reached a uint value that indicates the sector isn't full
			//keep the index where it is and continue on
			break;
		}
	}

	//count single fault log writes from header value that is not fully cleared
	logCntToParse = ~(data[idx]);
	while( logCntToParse >  0x0)
	{
		logCntToParse = logCntToParse >> 1;
		logRecordCnt++;
	}

	//save the found sector number and inner sector index
	Location->idxSector = sectorNumber;
	Location->logIdxIntoSector = logRecordCnt + 1;//+1 to get to the index of the next fault log to write
}
/**
 * @brief notify the fault log task to write a log into external flash
 * @return semaphore take of fault log notify successful or not
 */
static int32_t FaultLogNotifyTaskCmd()
{
	int32_t status;

	status = xSemaphoreTake(tl_FaultLogNotifySem,portMAX_DELAY);
	if(status == pdTRUE )
	{
		xTaskNotify(getFaultLogTaskId(),0x1,eSetBits );
	}
	return status;
}

/**
 * @brief Store Log to SPI Flash
 * @return 0 upon success
 */
static uint16_t StoreLogToFlash(sLogRecord_t *ptrLogData,sLogRecordLocation_t *ptrLogStorageLocation)
{
	int16_t pageIdx;
	uint16_t readIdx,timeoutCnt,idx;
	uint32_t flashAddress;
	uint8_t *writeDataBuffer;
	uint8_t innerIdx = 0;

//	EXF_PRINTF("StoreLogToFlash %d %d %d\r\n"
//			,ptrLogStorageLocation->recordCnt
//			,ptrLogStorageLocation->idxSector
//			,ptrLogStorageLocation->logIdxIntoSector);

	tl_RecordADCDataFlag = 0; // clear flag until storage is done.

	/*
	 * populate fault record structure items that are not updated by an internal function
	 */
	ptrLogData->timestamp = RTC->TSR;
	ptrLogData->entryCount = ptrLogStorageLocation->recordCnt;
	memset(ptrLogData ->reserved, 0xFF, EXTF_LOG_RESERVED);

	/* *
	 * store log record to flash
	 * this will index from the starting page index into the sector and index up the number
	 * of pages needed to store a log
	 */
	writeDataBuffer = GetWriteDataBuffer();
	for( readIdx=0,pageIdx=ptrLogStorageLocation->logIdxIntoSector*ptrLogStorageLocation->pagesPerLog;
			pageIdx < ptrLogStorageLocation->logIdxIntoSector*ptrLogStorageLocation->pagesPerLog+ptrLogStorageLocation->pagesPerLog;
			pageIdx++)
	{
		/*
		 * (the address) =  (the sector index) * (size of sector) + (page index into the sector) * (size of page)
		 */
		flashAddress = ptrLogStorageLocation->idxSector * EXTF_SECTOR_SIZE + pageIdx*EXTF_PAGE_SIZE;
//		PRINTF("flash address: %x\r\n", flashAddress);
		//copy log into external flash write buffer
		for(idx=0;idx<EXTF_PAGE_SIZE;idx++)
		{
			writeDataBuffer[idx] = *( (uint8_t *)(ptrLogData) + readIdx++ );
		}

		//check if we have somehow reached the end of flash
		if( flashAddress > EXT_FLASH_SIZE)
		{
			/*
			 * flash is filled, just exit should never get here
			 */
			goto exitMethod;
		}

		//notify external flash task to write page into external flash
		SetExtfAddress(flashAddress);
		SetExtfWriteSize(EXTF_PAGE_SIZE);
		xTaskNotify(getExtfTaskHandle(),extfPP1,eSetValueWithOverwrite );

		//wait until last write is finished before continuing
		timeoutCnt = 100;
		vTaskDelay(25);
		while (timeoutCnt)
		{
			if( GetExtfState() == extFlashDone )
			{
//				EXF_PRINTF("%x \r\n",flashAddress);
				vTaskDelay(1);
				break;
			}
			vTaskDelay(25);
			timeoutCnt--;
		}
	}


	//update the sector "header" that another log was written into flash
	flashAddress = ptrLogStorageLocation->idxSector * EXTF_SECTOR_SIZE;
	SetExtfAddress(flashAddress); //set the header address to write to
	idx = (ptrLogStorageLocation->logIdxIntoSector - 1) / 8;      //get the index to update within the header array
	innerIdx = (ptrLogStorageLocation->logIdxIntoSector - 1) % 8; //get the bit number that needs to be cleared
	tl_ptrSectorHeading->logsWritten[idx] &= ~(1 << innerIdx);    //clear the bit within the proper idx o header's array
	ptrLogStorageLocation->logIdxIntoSector++;
	ptrLogStorageLocation->recordCnt++;

	//update the external flash write buffer with the header values
	for(idx=0;idx<EXTF_PAGE_SIZE;idx++)
	{
		writeDataBuffer[idx] = *( (uint8_t *)(tl_ptrSectorHeading) + idx );
	}

	//notify the external flash task to write the new values
	SetExtfWriteSize(EXTF_PAGE_SIZE);
	xTaskNotify(getExtfTaskHandle(),extfPP1,eSetValueWithOverwrite );

	timeoutCnt = 100;
	vTaskDelay(1);
	while (timeoutCnt)
	{
		if( GetExtfState() == extFlashDone )
		{
//			EXF_PRINTF("%x \r\n",flashAddress);
			vTaskDelay(1);
			break;
		}
		vTaskDelay(25);
		timeoutCnt--;
	}

	//check if we have reached the end of the current sector and handle it as needed
		if(ptrLogStorageLocation->logIdxIntoSector +1 > ptrLogStorageLocation->logsPerSector+1)
		{
			/*
			 * must increment to next sector for storage
			 */
			if(ptrLogStorageLocation->idxSector+1 >= NUM_FAULT_SECTORS)
			{
				ptrLogStorageLocation->idxSector = FIRST_SECTOR_FAULT_LOG; //first spot is taken up by fault log
			}
			else{
				ptrLogStorageLocation->idxSector++;
			}
			ptrLogStorageLocation->logIdxIntoSector = 1;//first spot is taken up by sector "header"
			memset(tl_ptrSectorHeading,0xFF,sizeof(sSectorHeading_t)); //clear log count in header

			/**
			 * must clear the sector so that writes can be done again
			 */
			ExtfSectorErase(ptrLogStorageLocation->idxSector*EXTF_SECTOR_SIZE);
		}



	exitMethod:
	tl_RecordADCDataFlag = 1; // allow storage again

	return 0;
}
/*****************************************************************************
 * Protected Methods =========================================================
 ****************************************************************************/
/*****************************************************************************
 * Public Methods ============================================================
 ****************************************************************************/

/**
 * @brief write 255 fault logs into external flash
 */
void fillLogSector()
{
	uint32_t i = 0;
	while( i < 255)
	{
		uint8_t timeoutCnt = 100;
		while (timeoutCnt)
		{
			if( GetExtfReadyFlag() && GetExtfState() == extFlashIdle )
			{
				i++;
				tl_ptrLogData->timestamp = RTC->TSR;

				tl_ptrLogData->entryCount = tl_LogStorageLocation.recordCnt;
				FaultLogNotifyTaskCmd();
				break;
			}
			vTaskDelay(1);
			timeoutCnt--;
		}
	}
}

/**
 * @brief return current location information if where the next fault log will be stored
 * @return task local variable that tracks the next write location in flash
 */
sLogRecordLocation_t getCurrentLogLocation()
{
	return tl_LogStorageLocation;
}

/**
 * @brief Program a hard-coded pattern to the external External SPI Flash
 *
 * @return 0 on success
 */
int8_t WritePatternLogToExtf()
{
	tl_ptrLogData->timestamp = RTC->TSR;

	tl_ptrLogData->entryCount = tl_LogStorageLocation.recordCnt;
	FaultLogNotifyTaskCmd();

	return 0;
}

/**
 * @brief Dump an entire section of SPI FLASH memory
 * @todo update this to ignore the "header" section in each sector
 * @bug will not properly dump all log entries in flash
 * @return 1 on success
 */
int8_t DumpLogsFromExtf()
{
	uint32_t logRecordSize = sizeof(sLogRecord_t);
	sLogRecordLocation_t dumpLogs;
	uint32_t readAddress;
	uint32_t pageIdx;
	int8_t status;
	eExtfState_t extfStatus= extFlashIdle;

	sLogRecord_t *ptrLogBuffer;
	dumpLogs.idxSector = 0;
	dumpLogs.logIdxIntoSector=0;
	dumpLogs.pagesPerLog = (logRecordSize / EXTF_PAGE_SIZE);
	dumpLogs.logsPerSector = EXTF_SECTOR_SIZE / (dumpLogs.pagesPerLog*EXTF_PAGE_SIZE)-1;
	tl_RecordADCDataFlag = 0; // stop ADC storage of data
	ptrLogBuffer = tl_ptrLogData; // place to store data read from FLASH

	//@todo if supervisor task exists may have to have it mass dump logs but excluding for now since not used in current build
	//xTaskNotify(GetSupervisorTaskId(),eSupEventDumpLogRecords,eSetBits );

	vTaskDelay(100); // allow supervisor task to get in Dump Log state

	/*
	 * read log record from FLASH
	 */
	readAddress = 0;
	pageIdx = 0;
	memset(ptrLogBuffer,0x0,dumpLogs.pagesPerLog*EXTF_PAGE_SIZE);

	while( readAddress < EXT_FLASH_SIZE )
	{
		for(pageIdx=0;pageIdx < dumpLogs.pagesPerLog;pageIdx++)
		{
			/*
			 * loop until all pages for a log record are read
			 */
			readAddress = (dumpLogs.idxSector*EXTF_SECTOR_SIZE) +
					(dumpLogs.logIdxIntoSector*dumpLogs.pagesPerLog*EXTF_PAGE_SIZE) +
					(pageIdx*EXTF_PAGE_SIZE);
			status = ExtFlashPageRead( readAddress,(uint8_t *)ptrLogBuffer+(pageIdx*EXTF_PAGE_SIZE));
			while(status < 0)
			{
				vTaskDelay(1);
				status = ExtFlashPageRead( readAddress,(uint8_t *)ptrLogBuffer+(pageIdx*EXTF_PAGE_SIZE));
			}
			assert(status);
		}
		dumpLogs.logIdxIntoSector++;
		if(dumpLogs.logIdxIntoSector == dumpLogs.logsPerSector)
		{
			dumpLogs.logIdxIntoSector = 0;
			dumpLogs.idxSector++;
		}

		if(ptrLogBuffer->timestamp == 0xFFFFFFFF)
		{
			/*
			 * last log in buffer
			 */
			goto exitMethod;
		}
		/*
		 * display log memory
		 */

		PRINTF("log count: %x, timestamp:%x\r\n", ptrLogBuffer->entryCount, ptrLogBuffer->timestamp);
	}

	exitMethod:
	return extfStatus;
}

/**
 * @brief Clear location information of fault log count
 * @return 0 upon success
 */
uint16_t ResetLogEntries()
{
	tl_LogStorageLocation.idxSector = FIRST_SECTOR_FAULT_LOG;
	tl_LogStorageLocation.logIdxIntoSector = 1;

	return 0;
}

/**
 * @brief provide Fault log writing to External Flash Task
 * @see externalFlash.c the init external flash task to see where this task gets initialized
 * @param pvParameters void ptr
 */
void externalFlashLogTask(void *pvParameters)
{
    uint32_t logRecordSize = 0;
    uint32_t pagesPerLog,logsPerSector;
    uint32_t externalFlashCommand;
    tl_ptrLogData = (sLogRecord_t *)pvPortMalloc(sizeof(sLogRecord_t)); //allocate space for a fault log holder
    assert(tl_ptrLogData); //assert if not enough space for log data

    //Initialize holder for what amount of logs are currently in the sector
    tl_ptrSectorHeading = (sSectorHeading_t*)pvPortMalloc(sizeof(sSectorHeading_t));
    assert(tl_ptrSectorHeading);
    memset(tl_ptrSectorHeading->logsWritten, 0xFF, SIZE_OF_HEADING);

    //initialize the values inside of the local fault log
    tl_ptrLogData->timestamp = RTC->TSR;
    logRecordSize = sizeof(sLogRecord_t);

    //initialize the values related to which sector and where the log should be written to
    pagesPerLog = logRecordSize / EXTF_PAGE_SIZE;
    logsPerSector = EXTF_SECTOR_SIZE / (pagesPerLog*EXTF_PAGE_SIZE)-1;

    tl_LogStorageLocation.logsPerSector = logsPerSector;
    tl_LogStorageLocation.pagesPerLog = pagesPerLog;
    FindWhereToStoreLog(&tl_LogStorageLocation,pagesPerLog,logsPerSector);
    tl_LogStorageLocation.recordCnt = tl_LogStorageLocation.recordCnt+1;
    tl_ptrLogData->entryCount = tl_LogStorageLocation.recordCnt +1; // increment record count

    //initialize semaphore to handle for writing a single log to flash
	if(tl_FaultLogNotifySem == NULL)
	{
		tl_FaultLogNotifySem = xSemaphoreCreateBinary();
		assert(tl_FaultLogNotifySem);
		xSemaphoreGive(tl_FaultLogNotifySem);
	}

    setExtfReadyFlag(1);

    EXF_PRINTF("externalFlashCommTask \r\n");
    EXF_PRINTF("%d %x %x\r\n",tl_LogStorageLocation.recordCnt,tl_LogStorageLocation.idxSector,tl_LogStorageLocation.logIdxIntoSector);

	while(1)
	{
		if( xTaskNotifyWait(0x00, 0xFFFFFFFF, (uint32_t *)(&externalFlashCommand), portMAX_DELAY) == pdTRUE )
		{
			if(GetExtfReadyFlag())
			{
				EXF_PRINTF("external Flash Command: %x\r\n",externalFlashCommand);

				switch(externalFlashCommand)
				{
					default:
//						tl_ptrLogData->faultCode = externalFlashCommand;
					case 1:
						/*
						 * store log record to flash then release sem to notify we can accept another notification
						 */
							StoreLogToFlash(tl_ptrLogData,&tl_LogStorageLocation);
							xSemaphoreGive(tl_FaultLogNotifySem);
						break;
				}
			}
			else
			{
				EXF_PRINTF("external Flash is not available: %x\r\n",externalFlashCommand);
			}
		}
	}
}
/** @}*/


