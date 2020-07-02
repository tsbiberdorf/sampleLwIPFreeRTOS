/**
 * @file externalFlash.c
 * @details Full Implementation of handling communication with external SPI Flash
 *
 * The external FLASH storage communication will happen through the Kinetis SPI2 peripheral
 * the SPI2 will be connected to the following pins:
 * - 66-PTB20 SPI2_PSC0
 * - 67-PTB21 SPI2_SCK
 * - 68-PTB22 SPI2_SOUT
 * - 69-PTB23 SPI2_SIN
 *
 * @see externalFlash.h for public definition
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_dspi.h"
#include "fsl_rtc.h"
#include "Peripheral/RTC/rtcInterface.h"
#include "FaultLogs.h"
#include "TaskParameters.h"
#include "externalFlash.h"

/******************************************************************************
 * MACROS
 *****************************************************************************/

//---------------- steps to perform various operations ------------------------
/**
 * bulk erase steps:
 * - BE1 0x05 RDSR must be zero
 * - BE2 0x06 WREN must set write enable
 * - BE3 0x05 RDSR verify that write enable is set
 * - BE4 0x60 BE perform bulk erase operation
 * - BE5 0x05 RDSR poll until erase completed
 *
 * @defgroup groupBE bulk erase SPI Flash
 * @ingroup groupExtfOp
 * @{
 */
#define BE1			(RDSR)       /**< bulk erase step 1 */
#define BE1_SIZE 	(RDSR_SIZE)
#define BE2			(WREN)		 /**< bulk erase step 2 */
#define BE2_SIZE 	(WREN_SIZE)
#define BE3			(RDSR)        /**< bulk erase step 3 */
#define BE3_SIZE 	(RDSR_SIZE)
#define BE4			(BE)          /**< bulk erase step 4 */
#define BE4_SIZE 	(BE_SIZE)
#define BE5			(RDSR)		  /**< bulk erase step 5*/
#define BE5_SIZE 	(RDSR_SIZE)

#define BE 			(0x60) // Bulk erase
#define BE_SIZE 	(1)
/** @}*/


/**
 * sector erase steps:
 * - SE1 0x05 RDSR must be zero
 * - SE2 0x06 WREN must set write enable
 * - SE3 RDSR verify that write enable is set
 * - SE4 0xd8 + ADDRESS sector erase operation
 * - PP5 RDSR poll until sector erase complete
 *
 * @defgroup groupSE erase sector of SPI Flash
 * @ingroup groupExtfOp
 * @{
 */
#define SE1			(RDSR)      /**< sector erase step 1 */
#define SE1_SIZE 	(RDSR_SIZE)
#define SE2			(WREN)      /**< sector erase step 2 */
#define SE2_SIZE 	(WREN_SIZE)
#define SE3			(RDSR)      /**< sector erase step 3 */
#define SE3_SIZE 	(RDSR_SIZE)
#define SE4			(SE)		/**< sector erase step 4 */
#define SE4_SIZE 	(SE_SIZE)
#define SE5			(RDSR) 		/**< sector erase step 5 */
#define SE5_SIZE 	(RDSR_SIZE)

#define SE			(0xd8) // sector erase
#define SE_SIZE		(0x04)
/**@} */

/**
 * page program steps:
 * - PP1 0x05 RDSR must be zero
 * - PP2 0x06 WREN must set write enable
 * - PP3 RDSR verify that write enable is set
 * - PP4 0x04 + ADDRESS + DATA perform data write command
 * - PP5 RDSR poll until write complete
 *
 * @defgroup groupPP Program Page of SPI Flash
 * @ingroup groupExtfOp
 * @{
 */
#define PP1			(RDSR)
#define PP1_SIZE 	(RDSR_SIZE)
#define PP2			(WREN)
#define PP2_SIZE 	(WREN_SIZE)
#define PP3			(RDSR)
#define PP3_SIZE 	(RDSR_SIZE)
#define PP4			(PP)
#define PP4_SIZE 	(PP_SIZE)
#define PP5			(RDSR)
#define PP5_SIZE 	(RDSR_SIZE)

#define PP			(0x02) // page program
#define PP_SIZE		(0x04)
/**@}*/
/**
 * @defgroup groupRDOP Read External Flash Operations
 * @ingroup groupExtfOp
 * @{
 */
#define RDID 		(0x9F) // read identification
#define RDID_SIZE 	(5)
#define READID 		(0x90)
#define READID_SIZE (2)
#define RDSR_SIZE 	(2)
#define RDSR 		(0x05) // read status register
#define READ		(0x03) // read data bytes
#define READ_SIZE	(0x04)
/** @}*/
//---------------- board details for communication -----------------------------
#define EXTFLASH_DSPI_MASTER_BASEADDR SPI2
#define EXTFLASH_DSPI_MASTER_CLK_SRC DSPI2_CLK_SRC
#define EXTFLASH_DSPI_MASTER_CLK_FREQ CLOCK_GetFreq(BUS_CLK)
#define EXTFLASH_DSPI_MASTER_PCS_FOR_INIT kDSPI_Pcs0
#define EXTFLASH_DSPI_MASTER_PCS_FOR_TRANSFER kDSPI_MasterPcs0
#define TRANSFER_SIZE 64U         /*! Transfer dataSize */
#define TRANSFER_BAUDRATE 2000000U /*! Transfer baudrate - 2M */

/******************************************************************************
 * static Variables
 *****************************************************************************/



#if (EXTERNAL_FLASH_SUPPORT==1)

static TaskHandle_t tl_ExternalFlashTaskId = 0;
static TaskHandle_t tl_FaultLogTaskId = 0;
static dspi_master_handle_t tl_ExternalFlashHandle;
static volatile uint8_t tl_ExtFTransferCompleted = 0;
static volatile eExtfState_t tl_ExternalFlashState = extFlashIdle;


uint8_t tl_ExtfRxData[EXTF_TRANSFER_SIZE] = {0U};
uint8_t tl_ExtfTxData[EXTF_TRANSFER_SIZE] = {0U};
uint8_t *tl_PtrWriteData = &tl_ExtfTxData[PP4_SIZE]; // write data is part of the External Flash Tx Data buffer
uint8_t tl_ExtRdSize;
uint32_t tl_ExtFlashReadSize = 0;
uint32_t tl_ExtFlashReadAddress = 0;
uint32_t tl_ExtFlashWriteSize = 0;
volatile uint8_t tl_ExternalFlashReadyFlag = 0; // flag to signal that external flash is ready for use


static uint8_t tl_ExternalFlashPresentFlag = 0;


/******************************************************************************
 * Private Methods
 *****************************************************************************/

/**
 * @ingroup groupExtfOp
 * @{
 */
/**
 * @brief spi flash master callback
 */
void EXTFLASH_DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        __NOP();
    }

    tl_ExtFTransferCompleted = 1;
}

/**
 * @brief set size of data memory read operation
 */
static void SetExtfReadSize(uint32_t NumberOfBytes)
{
	tl_ExtFlashReadSize = NumberOfBytes;
}

/**
 * @brief set read memory address
 * @param Address addess to change the writing address to
 */
void SetExtfAddress(uint32_t Address)
{
	tl_ExtFlashReadAddress = Address;
}

/**
 * @brief set size of value to write
 *@param NumberOfBytes set the number of bytes to write
 */
void SetExtfWriteSize(uint32_t NumberOfBytes)
{
	tl_ExtFlashWriteSize = NumberOfBytes;
}

/**
 * @brief get current state of external flash
 * @return state of current flash
 */
eExtfState_t GetExtfState()
{

	if( tl_ExternalFlashState == extFlashDone)
	{
		xTaskNotify(tl_ExternalFlashTaskId,extfCommandCompleted,eSetValueWithOverwrite );
	}

	return tl_ExternalFlashState;
}

/**
 * @brief read the RDID register from the SPi Flash.  This is to verify that that the SPI flash
 * is present and responding.
 *
 * @return 1 on success
 * @ingroup groupRDOP
 */
int8_t ExtfRDID()
{
	uint8_t timeoutCnt;
	eExtfState_t extfStatus;
//	char Print_Str[64];
	extfStatus =  GetExtfState();
	if( extfStatus == extFlashIdle )
	{
		xTaskNotify(tl_ExternalFlashTaskId,extfRDID,eSetValueWithOverwrite );

		timeoutCnt = 100;
		while (timeoutCnt)
		{
			if( GetExtfState() == extFlashDone )
			{
				PRINTF("%x %x %x %x \r\n",tl_ExtfRxData[1],tl_ExtfRxData[2],tl_ExtfRxData[3],tl_ExtfRxData[4]);
				//context->printf_data_func("%x %x %x %x \r\n",tl_ExtfRxData[1],tl_ExtfRxData[2],tl_ExtfRxData[3],tl_ExtfRxData[4]);
				extfStatus = extFlashIdle;
				break;
			}
			vTaskDelay(25);
			timeoutCnt--;
		}
		if( timeoutCnt )
		{
			PRINTF("OK\r\n");
			//context->printf_data_func("OK\r\n");
		}
		else
		{
			PRINTF("timeout ERROR\r\n");
			//context->printf_data_func("timeout ERROR\r\n");
		}
	}
	return extfStatus;
}


/**
 * @brief Sector Erase External SPI FLASH memory.
 *
 * @param Address address location of sector to erase
 * @return 1 on success
 * @ingroup groupSE
 */
int8_t ExtfSectorErase(uint32_t Address)
{
	uint8_t timeoutCnt;
	eExtfState_t extfStatus;

	extfStatus =  GetExtfState();
	if( extfStatus == extFlashIdle )
	{
		SetExtfAddress(Address);

		xTaskNotify(tl_ExternalFlashTaskId,extfSE1,eSetValueWithOverwrite );

		timeoutCnt = 100;
		while (timeoutCnt)
		{
			if( GetExtfState() == extFlashDone )
			{
				//context->printf_data_func("%x \r\n",tl_ExtfRxData[1]);
				extfStatus = extFlashIdle;
				break;
			}
			vTaskDelay(25);
			timeoutCnt--;
		}
		if( timeoutCnt )
		{
			//context->printf_data_func("OK\r\n");
		}
		else
		{
			//context->printf_data_func("timeout ERROR\r\n");
		}
	}
	return extfStatus;
}

/**
 * @brief Bulk Erase entire Extern SPI FLASH memory.
 *
 * @return 1 on success
 * @ingroup groupBE
 */
int8_t ExtfBE()
{
	eExtfState_t extfStatus;
	uint16_t sectorCnt;
	uint32_t sectorAddress;

	for(sectorCnt=0;sectorCnt<NUMBER_EXTF_SECTORS;sectorCnt++)
	{
		sectorAddress = sectorCnt*EXTF_SECTOR_SIZE;
		//context->printf_data_func("erase sector %d\r\n",sectorAddress);
		extfStatus = ExtfSectorErase(sectorAddress);
	}

	return extfStatus;
}


/**
 * @brief Perform page read
 * @details this reads a page from the flash and stores the results into a
 * new location.
 *
 * @param Address address to begin reading at
 * @param ptrDest pointer to where the read data will be copied
 * @return timeoutCnt
 * @ingroup groupRDOP
 */
int8_t ExtFlashPageRead(uint32_t Address,uint8_t *ptrDest)
{
#define TIMEOUT_CNT (100)
	int8_t timeoutCnt = -1;
	int16_t idx;
	eExtfState_t extfStatus;

	extfStatus =  GetExtfState();
	if( extfStatus == extFlashIdle )
	{
		SetExtfAddress(Address);
		SetExtfReadSize(256);
		xTaskNotify(tl_ExternalFlashTaskId,extfRD1,eSetValueWithOverwrite );

		timeoutCnt = TIMEOUT_CNT;
		while (timeoutCnt)
		{
			if( GetExtfState() == extFlashDone )
			{
				extfStatus = extFlashIdle;
				break;
			}
			vTaskDelay(25);
			timeoutCnt--;
		}
		if(timeoutCnt)
		{
			for(idx=0;idx<EXTF_PAGE_SIZE;idx++)
			{
				/*
				 * copy page read to the destination address block
				 */
				*(ptrDest+idx) =  *(tl_ExtfRxData+4+idx);
			}
		}
	}
	return timeoutCnt;
}



/**
 * @brief read the RDSR register from the SPi Flash.
 * @details This is to verify that that the SPI flash is present and responding.
 *
 * @return 1 on success
 * @ingroup groupRDOP
 */
int8_t ExtfRDSR()
{
	uint8_t timeoutCnt;
	eExtfState_t extfStatus;

	extfStatus =  GetExtfState();
	if( extfStatus == extFlashIdle )
	{
		xTaskNotify(tl_ExternalFlashTaskId,extfRDSR,eSetValueWithOverwrite );

		timeoutCnt = 100;
		while (timeoutCnt)
		{
			if( GetExtfState() == extFlashDone )
			{
//				context->printf_data_func("%x \r\n",tl_ExtfRxData[1]);
				extfStatus = extFlashIdle;
				break;
			}
			vTaskDelay(25);
			timeoutCnt--;
		}
		if( timeoutCnt )
		{
//			context->printf_data_func("OK\r\n");
		}
		else
		{
//			context->printf_data_func("timeout ERROR\r\n");
		}
	}
	return extfStatus;
}

/**
 * @brief execute the read data register from the SPi Flash.
 * @details this just dumps the results onto the display. It doesn't actually
 * save the data anywhere.
 *
 * @param Address location to begin reading data from flash
 * @return 1 on success
 * @ingroup groupRDOP
 */
int8_t ExtfDumpPageData(uint32_t Address)
{
#define TIMEOUT_CNT (100)
	uint8_t timeoutCnt;
	eExtfState_t extfStatus;
	uint16_t idx;
	uint8_t *ptrData;

	extfStatus =  GetExtfState();
	if( extfStatus == extFlashIdle )
	{
		SetExtfAddress(Address);
		SetExtfReadSize(EXTF_PAGE_SIZE);
		xTaskNotify(tl_ExternalFlashTaskId,extfRD1,eSetValueWithOverwrite );

		timeoutCnt = TIMEOUT_CNT;
		while (timeoutCnt)
		{
			if( GetExtfState() == extFlashDone )
			{
//				context->printf_data_func("%x \r\n",tl_ExtfRxData[1]);
				PRINTF("%x \r\n",tl_ExtfRxData[1]);
				extfStatus = extFlashIdle;
				break;
			}
			vTaskDelay(25);
			timeoutCnt--;
		}
		if( timeoutCnt )
		{
//			context->printf_data_func("OK\r\n");
			PRINTF("OK\r\n");
			//			context->printf_data_func("%x\r\n",*(ptrData+idx));
			PRINTF("\r\n");

			/*
			 * dump raw data from page out
			 * The page data will be displayed in 0x10 bytes per line with
			 * its address starting each line.
			 */
			ptrData = tl_ExtfRxData+4;
			PRINTF("%04x:",Address);
			for(idx=0;idx<EXTF_PAGE_SIZE;idx++)
			{
				PRINTF("%02x ",ptrData[idx]);
				if( (idx+1) < EXTF_PAGE_SIZE)
				{
					if( ((idx+1) % 16) == 0 )
					{
						PRINTF("\r\n%04x:",Address+idx+1);
					}
				}
				else
				{
					PRINTF("\r\n");
				}
			}
		}
		else
		{
//			context->printf_data_func("timeout ERROR\r\n");
			PRINTF("timeout ERROR\r\n");
		}
	}
	return extfStatus;
}

/**
 * @brief place SPI Flash into Write enable
 * @return 1 on success
 * @ingroup groupExtfOp
 */
int8_t ExtfWren()
{
	uint8_t timeoutCnt;
	eExtfState_t extfStatus;

	extfStatus =  GetExtfState();
	if( extfStatus == extFlashIdle )
	{
		xTaskNotify(tl_ExternalFlashTaskId,extfWREN,eSetValueWithOverwrite );

		timeoutCnt = 100;
		while (timeoutCnt)
		{
			if( GetExtfState() == extFlashDone )
			{
				extfStatus = extFlashIdle;
				break;
			}
			vTaskDelay(25);
			timeoutCnt--;
		}
		if( timeoutCnt )
		{
//			context->printf_data_func("OK\r\n");
		}
		else
		{
//			context->printf_data_func("timeout ERROR\r\n");
		}
	}
	return extfStatus;
}

/**
 * @brief take SPI Flash out of Write enable
 * @return 1 on success
 * @ingroup groupExtfOp
 */
uint8_t ExtfWrdi()
{
	uint8_t timeoutCnt;
	eExtfState_t extfStatus;

	extfStatus =  GetExtfState();
	if( extfStatus == extFlashIdle )
	{
		xTaskNotify(tl_ExternalFlashTaskId,extfWRDI,eSetValueWithOverwrite );

		timeoutCnt = 100;
		while (timeoutCnt)
		{
			if( GetExtfState() == extFlashDone )
			{
				extfStatus = extFlashIdle;
				break;
			}
			vTaskDelay(25);
			timeoutCnt--;
		}
		if( timeoutCnt )
		{
//			context->printf_data_func("OK\r\n");
		}
		else
		{
//			context->printf_data_func("timeout ERROR\r\n");
		}
	}
	return extfStatus;
}


/**
 * @brief Read/verify the SPI Flash Read Status register
 * @details this makes sure that the flash state is what i is expected to be.
 * @returns 1 if state matches expected
 * @ingroup groupExtfOp
 */
static int32_t verifyReadStatus(char VerifyFLASHState)
{
	int32_t status = 0;

	if( tl_ExtfRxData[1] & 1<<7) // bit 7
	{
		 EXF_PRINTF("RDSR: Status Register Write Disable\r\n");
	}
	if( tl_ExtfRxData[1] & 1<<6)
	{
		 EXF_PRINTF("RDSR: Programming Error Occurred\r\n");
	}
	if( tl_ExtfRxData[1] & 1<<5)
	{
		 EXF_PRINTF("RDSR: Erase Error Occurred\r\n");
	}
	if( tl_ExtfRxData[1] & 1<<4)
	{
		 EXF_PRINTF("RDSR: BP2 Block Protected\r\n");
	}
	if( tl_ExtfRxData[1] & 1<<3)
	{
		 EXF_PRINTF("RDSR: BP1 Block Protected\r\n");
	}
	if( tl_ExtfRxData[1] & 1<<2)
	{
		 EXF_PRINTF("RDSR: BP0 Block Protected\r\n");
	}
	if( tl_ExtfRxData[1] & 1<<1)
	{
		 EXF_PRINTF("RDSR: Write Enable Latch\r\n");
	}
	if( tl_ExtfRxData[1] & 1<<0)
	{
		 EXF_PRINTF("RDSR: Write in Progress\r\n");
	}
	if( tl_ExtfRxData[1] == 0)
	{
		 EXF_PRINTF("RDSR: clear\r\n");
	}

	/*
	 * poll waiting for extfDone to indicate that the read status is ready to look at
	 */
	if( tl_ExtfRxData[1] == VerifyFLASHState)
	{
		status = 1; // read status matches the expected state
	}

	return status;
}

/**
 *@brief trigger start of master transfer
 *@ingroup groupExtfOp
 */
static void triggerMasterTransfer(uint8_t *PtrTxDataBuffer, uint8_t *PtrRxDataBuffer,uint32_t Size)
{
    dspi_transfer_t masterXfer;

    tl_ExtFTransferCompleted = 0; // clear flag, the IRQ method will signal when the command has completed
    tl_ExternalFlashState = extFlashBusy; // set state to indicate SPI bus is now busy running a command
    masterXfer.txData = PtrTxDataBuffer;
	masterXfer.rxData = PtrRxDataBuffer;
	masterXfer.dataSize = Size;
	masterXfer.configFlags = kDSPI_MasterCtar0 | EXTFLASH_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
	DSPI_MasterTransferNonBlocking(EXTFLASH_DSPI_MASTER_BASEADDR, &tl_ExternalFlashHandle, &masterXfer);
}

/**
 * @brief Read Data operations
 * @ingroup groupExtfOp
 */
static void ReadDataOpoerations(eExternalFlashStates_t RDFlashState)
{
#define ABLE_TO_READ (0x00)
    EXF_PRINTF("ReadDataOperations: %d\n",RDFlashState);

    switch(RDFlashState)
    {
    case extfRD1:
    	/*
    	 * before triggering the read operation, need to verify that the flash
    	 * is ready
    	 */
        EXF_PRINTF("extfRD1: %d\n",RDFlashState);
    	if( verifyReadStatus(ABLE_TO_READ) )
    	{
    		if(tl_ExtFlashReadSize < 256+1)
    		{
    			// issue read data command
    			xTaskNotify(tl_ExternalFlashTaskId,extfRD2,eSetValueWithOverwrite );
    		}
    	}
		else
		{
			xTaskNotify(tl_ExternalFlashTaskId,extCommandError,eSetValueWithOverwrite );
		}
    	break;
    case extfRD2:
        EXF_PRINTF("extfRD2: %d\n",RDFlashState);
        // signal data read done
		xTaskNotify(tl_ExternalFlashTaskId,extfRD3,eSetValueWithOverwrite );
    	break;
    case extfRD3:
    	break;
    default:
        EXF_PRINTF("ReadDataOperations default: %d\n",RDFlashState);

    }
}

/**
 * @brief Bulk Flash erase operation
 * <ol>
 * <li> extfBE1: RDSR -> verify status is clear and ready to set write enable bit
 * <li> extfBE2: WREN -> set write enable bit
 * <li> extfBE3: RDSR -> verify write enable bit set
 * <li> extfBE4: BE -> instruct flash to erase full flash memory
 * <li> extfBE5: RDSR -> verify status is clear this will indicate that the memory is clear
 * <li> extfCommandCompleted
 * </ol>
 * @ingroup groupBE
 */
static void BulkEraseOperations(eExternalFlashStates_t ExtfCmdState)
{
#define ABLE_TO_WRITE (0x00)
#define ABLE_WRITE_ENABLE_SET (0x02)
#define ERASE_COMPLETED (0x00)
	static uint16_t timeoutCnt = 200;
    EXF_PRINTF("BulkEraseOperations: %d\n",ExtfCmdState);

	switch(ExtfCmdState)
	{
	case extfBE1:
		/*
		 * to perform a bulk flash erase
		 * first need verify that the flash status is clear
		 */
		if( verifyReadStatus(ABLE_TO_WRITE) )
		{
			xTaskNotify(tl_ExternalFlashTaskId,extfBE2,eSetValueWithOverwrite );
		}
		else
		{
			xTaskNotify(tl_ExternalFlashTaskId,extCommandError,eSetValueWithOverwrite );
		}
		timeoutCnt = 200;
		break;
	case extfBE2:
		xTaskNotify(tl_ExternalFlashTaskId,extfBE3,eSetValueWithOverwrite );
		break;
	case extfBE3:
		/*
		 * verify that the wren bit has been set
		 */
		if( verifyReadStatus(ABLE_WRITE_ENABLE_SET) )
		{
			// issue the Bulk Erase cmd
			xTaskNotify(tl_ExternalFlashTaskId,extfBE4,eSetValueWithOverwrite );
		}
		else
		{
			xTaskNotify(tl_ExternalFlashTaskId,extCommandError,eSetValueWithOverwrite );
		}
		break;
	case extfBE4:
		/*
		 * issue RDSR to determine if the Bulk Erase has completed
		 */
		xTaskNotify(tl_ExternalFlashTaskId,extfBE5,eSetValueWithOverwrite );
		break;
	case extfBE5:
		/*
		 * verify that the wren bit has been set
		 */
		timeoutCnt--;
		if( timeoutCnt == 0 )
		{
			tl_ExternalFlashState = extFlashIdle;
			xTaskNotify(tl_ExternalFlashTaskId,extfWRDI,eSetValueWithOverwrite );
			break;
		}
		if( verifyReadStatus(ERASE_COMPLETED) )
		{
			// set notify to signal command done
			xTaskNotify(tl_ExternalFlashTaskId,extfBE6,eSetValueWithOverwrite );
		}
		else
		{
			/*
			 * command has not completed, short delay then poll again to determine if it is done
			 */
			vTaskDelay(200);
			xTaskNotify(tl_ExternalFlashTaskId,extfBE5,eSetValueWithOverwrite );

//			xTaskNotify(tl_ExternalFlashTaskId,extFlashError,eSetValueWithOverwrite );
		}
		break;
	default:
	    EXF_PRINTF("BulkEraseOperations default: %d\n",ExtfCmdState);

	}
	ResetLogEntries();
}

/**
 * @brief Page Program operation
 * <ol>
 * <li> extfPP1: RDSR -> verify status is clear and ready to set write enable bit
 * <li> extfPP2: WREN -> set write enable bit
 * <li> extfPP3: RDSR -> verify write enable bit set
 * <li> extfPP4: PP -> instruct flash to program specified page
 * <li> extfPP5: RDSR -> verify status is clear this will indicate that the page has been programmed
 * <li> extfCommandCompleted
 * </ol>
 * @ingroup groupPP
 */
static void PageProgramOperations(eExternalFlashStates_t ExtfCmdState)
{
#define ABLE_TO_WRITE (0x00)
#define ABLE_WRITE_ENABLE_SET (0x02)
#define PROGRAM_COMPLETED (0x00)

    //EXF_PRINTF("PageProgramOperation: %d\n",ExtfCmdState);

	switch(ExtfCmdState)
	{
	case extfPP1:
		/*
		 * to perform a bulk flash erase
		 * first need verify that the flash status is clear
		 */
		if( verifyReadStatus(ABLE_TO_WRITE) )
		{
			xTaskNotify(tl_ExternalFlashTaskId,extfPP2,eSetValueWithOverwrite );
		}
		else
		{
			xTaskNotify(tl_ExternalFlashTaskId,extCommandError,eSetValueWithOverwrite );
		}
		break;
	case extfPP2:
		xTaskNotify(tl_ExternalFlashTaskId,extfPP3,eSetValueWithOverwrite );
		break;
	case extfPP3:
		/*
		 * verify that the wren bit has been set
		 */
		if( verifyReadStatus(ABLE_WRITE_ENABLE_SET) )
		{
			// issue the Page Program
			xTaskNotify(tl_ExternalFlashTaskId,extfPP4,eSetValueWithOverwrite );
		}
		else
		{
			xTaskNotify(tl_ExternalFlashTaskId,extCommandError,eSetValueWithOverwrite );
		}
		break;
	case extfPP4:
		/*
		 * issue RDSR to determine if the Page Program has completed
		 */
		xTaskNotify(tl_ExternalFlashTaskId,extfPP5,eSetValueWithOverwrite );
		break;
	case extfPP5:
		/*
		 * verify that the wren bit has been cleared
		 */
		if( verifyReadStatus(PROGRAM_COMPLETED) )
		{
			// set notify to signal command done
			xTaskNotify(tl_ExternalFlashTaskId,extfPP6,eSetValueWithOverwrite );
		}
		else
		{
			/*
			 * command has not completed, short delay then poll again to determine if it is done
			 */
			vTaskDelay(200);
			xTaskNotify(tl_ExternalFlashTaskId,extfPP5,eSetValueWithOverwrite );
		}
		break;
	default:
	    EXF_PRINTF("PageProgramOperation default: %d\n",ExtfCmdState);

	}

}


/**
 * @brief Sector Erase operation
 * <ol>
 * <li> extfSE1: RDSR -> verify status is clear and ready to set write enable bit
 * <li> extfSE2: WREN -> set write enable bit
 * <li> extfSE3: RDSR -> verify write enable bit set
 * <li> extfSE4: SE -> instruct flash to program specified page
 * <li> extfSE5: RDSR -> verify status is clear this will indicate that the page has been programmed
 * <li> extfCommandCompleted
 * </ol>
 * @ingroup groupSE
 */
static void SectorEraseOperations(eExternalFlashStates_t ExtfCmdState)
{
#define ABLE_TO_WRITE (0x00)
#define ABLE_WRITE_ENABLE_SET (0x02)
#define PROGRAM_COMPLETED (0x00)

    EXF_PRINTF("SectorEraseOperation: %d\n",ExtfCmdState);

	switch(ExtfCmdState)
	{
	case extfSE1:
		/*
		 * to perform a bulk flash erase
		 * first need verify that the flash status is clear
		 */
		if( verifyReadStatus(ABLE_TO_WRITE) )
		{
			xTaskNotify(tl_ExternalFlashTaskId,extfSE2,eSetValueWithOverwrite );
		}
		else
		{
			xTaskNotify(tl_ExternalFlashTaskId,extCommandError,eSetValueWithOverwrite );
		}
		break;
	case extfSE2:
		xTaskNotify(tl_ExternalFlashTaskId,extfSE3,eSetValueWithOverwrite );
		break;
	case extfSE3:
		/*
		 * verify that the wren bit has been set
		 */
		if( verifyReadStatus(ABLE_WRITE_ENABLE_SET) )
		{
			// issue the Page Program
			xTaskNotify(tl_ExternalFlashTaskId,extfSE4,eSetValueWithOverwrite );
		}
		else
		{
			xTaskNotify(tl_ExternalFlashTaskId,extCommandError,eSetValueWithOverwrite );
		}
		break;
	case extfSE4:
		/*
		 * issue RDSR to determine if the Page Program has completed
		 */
		xTaskNotify(tl_ExternalFlashTaskId,extfSE5,eSetValueWithOverwrite );
		break;
	case extfSE5:
		/*
		 * verify that the wren bit has been cleared
		 */
		if( verifyReadStatus(PROGRAM_COMPLETED) )
		{
			// set notify to signal command done
			xTaskNotify(tl_ExternalFlashTaskId,extfSE6,eSetValueWithOverwrite );
		}
		else
		{
			/*
			 * command has not completed, short delay then poll again to determine if it is done
			 */
			vTaskDelay(200);
			xTaskNotify(tl_ExternalFlashTaskId,extfSE5,eSetValueWithOverwrite );
		}
		break;
	default:
	    EXF_PRINTF("SectorEraseOperation default: %d\n",ExtfCmdState);

	}

}


/**
 * @details Initialize SPI HW for external flash
 * @ingroup groupExtfOp
 */
void InitExternalFlash()
{
	dspi_master_config_t masterConfig;
	uint32_t srcClock_Hz;

	//InitSPI2();

	/* Master config */
	masterConfig.whichCtar = kDSPI_Ctar0;
	masterConfig.ctarConfig.baudRate = TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.bitsPerFrame = 8U;
	masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
	masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
	masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
	masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;

	masterConfig.whichPcs = EXTFLASH_DSPI_MASTER_PCS_FOR_INIT;
	masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

	masterConfig.enableContinuousSCK = false;
	masterConfig.enableRxFifoOverWrite = false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint = kDSPI_SckToSin0Clock;

	srcClock_Hz = EXTFLASH_DSPI_MASTER_CLK_FREQ;
	DSPI_MasterInit(EXTFLASH_DSPI_MASTER_BASEADDR, &masterConfig, srcClock_Hz);
	DSPI_MasterTransferCreateHandle(EXTFLASH_DSPI_MASTER_BASEADDR, &tl_ExternalFlashHandle, EXTFLASH_DSPI_MasterUserCallback, NULL);

	tl_ExtfTxData[0] = RDID;
	triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,RDID_SIZE);

	/*
	 * Wait for SPI transfer to complete
	 */
	while (!tl_ExtFTransferCompleted);

	/*
	 * check returned SPI flash IDs, S25FL032P0XMFI011: Manuf. ID=0x01, Device Id=0x0215, # Extended bytes=0x4D
	 */
	if((tl_ExtfRxData[1] == 0x01) && (tl_ExtfRxData[2] == 0x02) && (tl_ExtfRxData[3] == 0x15) && (tl_ExtfRxData[4] == 0x4D))
	{
		EXF_PRINTF("external SPI FLASH is present\n");
		tl_ExternalFlashPresentFlag = 1;
	}
	else
	{
		EXF_PRINTF("external SPI FLASH is not available\n");
		tl_ExternalFlashPresentFlag = 0;
	}
}


/**
 * @brief External FLASH Task
 *
 * @details externalFlash task is responsible to handling all external FLASH read/write API calls
 *
 * @param pvParameters input values to task
 * @return void
 * @ingroup groupExtfOp
 */
static void externalFlashTask(void *pvParameters)
{
    eExternalFlashStates_t extfCmdState = extfCommandCompleted;

    EXF_PRINTF("external SPI FLASH task started\n");

    tl_ExternalFlashState = extFlashIdle; // signal that the SPI flash is ready for a command
	while(1)
	{

		if( xTaskNotifyWait(0x00, 0xFFFFFFFF, (uint32_t *)(&extfCmdState), portMAX_DELAY) == pdTRUE )
		{
		    //EXF_PRINTF("extf %d\n",extfCmdState);

			switch(extfCmdState)
			{
			case extfCommandCompleted:
			    EXF_PRINTF("extfIdle\n");
				tl_ExternalFlashState = extFlashIdle; // signal that the SPI flash is ready for a command
				break;
			case extfRDID:
				assert( !tl_ExternalFlashState );
				tl_ExtfTxData[0] = RDID;
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,RDID_SIZE);
				break;
			case extfRDSR:
				assert( !tl_ExternalFlashState );
				tl_ExtfTxData[0] = RDSR;
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,RDSR_SIZE);
				break;
			case extfWREN:
				assert( !tl_ExternalFlashState );
				tl_ExtfTxData[0] = WREN;
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,WREN_SIZE);
				break;
			case extfWRDI:
				assert( !tl_ExternalFlashState );
				tl_ExtfTxData[0] = WRDI;
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,WRDI_SIZE);
				break;
			case extfBE1: // RDSR
				assert( !tl_ExternalFlashState );
				tl_ExtfTxData[0] = BE1;
//			    EXF_PRINTF("extfBE1 RDSR\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,BE1_SIZE);
				break;
			case extfBE2: // WREN
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = BE2;
//			    EXF_PRINTF("extfBE2 WREN\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,BE2_SIZE);
				break;
			case extfBE3: // RDSR
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = BE3;
//			    EXF_PRINTF("extfBE3 RDSR\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,BE3_SIZE);
				break;
			case extfBE4: // BE
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = BE4;
//			    EXF_PRINTF("extfBE4 BE\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,BE4_SIZE);
				break;
			case extfBE5: // RDSR
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = BE5;
//			    EXF_PRINTF("extfBE5 RDSR\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,BE5_SIZE);
				break;
			case extfBE6:
				tl_ExternalFlashState = extFlashDone;
				break;

			case extfRD1:
				assert( !tl_ExternalFlashState );
				tl_ExtfTxData[0] = RDSR;
//			    EXF_PRINTF("extfRD1 RDSR\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,RDSR_SIZE);
				break;
			case extfRD2:
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = READ;
				tl_ExtfTxData[1] = (tl_ExtFlashReadAddress>>16)&0xff; // address bits 23..16
				tl_ExtfTxData[2] = (tl_ExtFlashReadAddress>>8)&0xff; // address bits 15..8
				tl_ExtfTxData[3] = (tl_ExtFlashReadAddress)&0xff; // address bits 7..0

//			    EXF_PRINTF("extfRD1 RD\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,READ_SIZE+tl_ExtFlashReadSize);
				break;
			case extfRD3:
				tl_ExternalFlashState = extFlashDone;
				break;
			case extCommandError:
			    EXF_PRINTF("external SPI FLASH ERROR!!!\n");
				tl_ExternalFlashState = extFlashIdle; // signal that the SPI flash is ready for a command
				break;

			case extfPP1:
				assert( !tl_ExternalFlashState );
				tl_ExtfTxData[0] = PP1; //RDSR
//			    EXF_PRINTF("extfRD1 PP1\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,PP1_SIZE);
				break;
			case extfPP2:
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = PP2; // WREN
//			    EXF_PRINTF("extfPP2\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,PP2_SIZE);
				break;
			case extfPP3:
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = PP3; // RDSR
//			    EXF_PRINTF("extfPP3\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,PP3_SIZE);
				break;
			case extfPP4:
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = PP4; // PP
				tl_ExtfTxData[1] = (tl_ExtFlashReadAddress>>16)&0xff; // address bits 23..16
				tl_ExtfTxData[2] = (tl_ExtFlashReadAddress>>8)&0xff; // address bits 15..8
				tl_ExtfTxData[3] = (tl_ExtFlashReadAddress)&0xff; // address bits 7..0
	//		    EXF_PRINTF("extfPP4\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,PP4_SIZE+tl_ExtFlashWriteSize);
				break;
			case extfPP5:
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = PP5; // RDSR
		//	    EXF_PRINTF("extfPP3\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,PP5_SIZE);
				break;
			case extfPP6:
				tl_ExternalFlashState = extFlashDone;
				break;


			case extfSE1:
				assert( !tl_ExternalFlashState );
				tl_ExtfTxData[0] = SE1; //RDSR
//			    EXF_PRINTF("extfRD1 SE1\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,SE1_SIZE);
				break;
			case extfSE2:
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = SE2; // WREN
//			    EXF_PRINTF("extfSE2\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,SE2_SIZE);
				break;
			case extfSE3:
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = SE3; // RDSR
//			    EXF_PRINTF("extfSE3\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,SE3_SIZE);
				break;
			case extfSE4:
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = SE4; // SE
				tl_ExtfTxData[1] = (tl_ExtFlashReadAddress>>16)&0xff; // address bits 23..16
				tl_ExtfTxData[2] = (tl_ExtFlashReadAddress>>8)&0xff; // address bits 15..8
				tl_ExtfTxData[3] = (tl_ExtFlashReadAddress)&0xff; // address bits 7..0
//			    EXF_PRINTF("extfSE4\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,SE4_SIZE);
				break;
			case extfSE5:
				assert( tl_ExternalFlashState == extFlashBusy );
				tl_ExtfTxData[0] = SE5; // RDSR
//			    EXF_PRINTF("extfSE3\n");
				triggerMasterTransfer(tl_ExtfTxData,tl_ExtfRxData,SE5_SIZE);
				break;
			case extfSE6:
				tl_ExternalFlashState = extFlashDone;
				break;
}

			if( tl_ExternalFlashState == extFlashBusy )
			{
				/*
				 * if processing a SPI command, now wait until it
				 * has completed
				 */
				while (!tl_ExtFTransferCompleted)
				{
					vTaskDelay(1);
				}
				switch(extfCmdState)
				{
				case extfBE1:
				case extfBE2:
				case extfBE3:
				case extfBE4:
				case extfBE5:
					BulkEraseOperations(extfCmdState);
					break;
				case extfRD1:
				case extfRD2:
				case extfRD3:
					ReadDataOpoerations(extfCmdState);
					break;
				case extfPP1:
				case extfPP2:
				case extfPP3:
				case extfPP4:
				case extfPP5:
					PageProgramOperations(extfCmdState);
					break;
				case extfSE1:
				case extfSE2:
				case extfSE3:
				case extfSE4:
				case extfSE5:
					SectorEraseOperations(extfCmdState);
					break;
				default:
					tl_ExternalFlashState = extFlashDone;
				}
			}
		}
	}
}

/******************************************************************************
 * Public Methods
 *****************************************************************************/
/**
 *
 * @ingroup groupExtfOp
 * @{
 */
/**
 * @brief get flag that indicates if flash is ready
 * @return flashReadyFlag value
 */
uint8_t GetExtfReadyFlag()
{
	return tl_ExternalFlashReadyFlag;
}

/**
 * @brief set state of flash being ready
 * @return 0 upon completion
 */
uint8_t setExtfReadyFlag(uint8_t value)
{
	tl_ExternalFlashReadyFlag = value;
	return 0;
}
/**
 * @brief get external flash task handle
 * @return task handle for the external flash task
 */
TaskHandle_t getExtfTaskHandle()
{
	return tl_ExternalFlashTaskId;
}

/**
 * @brief get fault log task handler id
 * @return task handle for fault log
 */
TaskHandle_t getFaultLogTaskId()
{
	return tl_FaultLogTaskId;
}

/**
 * @brief Start External Flash Task And Fault log task operation
 * @return void
 */
void initExternalFlashThread()
{
	InitExternalFlash();

	/*
	 * if SPI Flash is available, start flashTask
	 */
	if(tl_ExternalFlashPresentFlag)
	{
		xTaskCreate(externalFlashTask, "flashTask", EXTFLASH_TASK_STACK_SIZE, NULL, EXTFLASH_TASK_PRIORITY, &tl_ExternalFlashTaskId);
	}

	xTaskCreate(externalFlashLogTask, "FaultLogTask", EXTFLASH_TASK_STACK_SIZE, NULL, EXTFLASH_TASK_PRIORITY, &tl_FaultLogTaskId);
}

/**
 * @brief get pointer to write data buffer
 * @return pointer to write data buffer
 */
uint8_t * GetWriteDataBuffer()
{
	return tl_PtrWriteData;
}
/**@}*/

#else

void initExternalFlashThread()
{

}

int32_t CmdExtfOperations( int32_t argc, char **argv)
{
//	context->printf_data_func("External flash functions are not installed \r\n");

	return 0;
}

/**
 * @details push a terminator resistance value to Log Terminator buffer.
 * This is a one time record not a circular queue.  The log will only be
 * filled till the buffer size.
 */
uint16_t LogTerminatorResistance(int32_t iResistance)
{
	return 1;
}


 */
TaskHandle_t GetExternalFlashCommandTaskId()
{
	return 0;
}
#endif
