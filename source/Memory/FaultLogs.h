/**
 * @file FaultLogs.h
 *
 * @brief Public API for writing fault log into external flash
 * @see FaultLogs.c for full implementation
 */

#ifndef MEMORY_FAULTLOGS_H_
#define MEMORY_FAULTLOGS_H_

#define EXTF_LOG_RESERVED (256-8) /**< number of uint8_t values needed to get the fault log to be the proper size*/
#define NUM_FAULT_SECTORS (2)     /**< number of external flash sectors reserved for fault logging */

/**
 * @brief holds information about a log record
 * @details This is meant so that it is easy to update and organize which sections within the log record
 * are where within the full amount of data.
 * @see tl_ptrLogData for an example of how this is used
 */
typedef struct _LogRecord_s
{
	uint32_t entryCount; //4
	uint32_t timestamp;  //4
	uint8_t reserved[EXTF_LOG_RESERVED]; //248
	//@todo add crc uint32_t
}sLogRecord_t; //4+4+248 = 256

/**
 * @brief hold information about where next log record should be stored.
 * @see tl_LogStorageLocation for an example of how this is used
 */
typedef struct _LogRecordLocation_s
{
	uint32_t logIdxIntoSector; /**< how logs into the sector the log should be stored in */
	uint32_t idxSector;        /**< external flash sector number */
	uint32_t recordCnt; 	   /**< total number of log records written */
	uint32_t logsPerSector;    /**< number of log records that fit into a sector */
	uint32_t pagesPerLog;      /**< number of pages needed to hold one log entry */
}sLogRecordLocation_t;


void fillLogSector();
sLogRecordLocation_t getCurrentLogLocation();
int8_t DumpLogsFromExtf();
int8_t WritePatternLogToExtf();
void externalFlashLogTask(void *pvParameters);
#endif /* MEMORY_FAULTLOGS_FAULTLOGS_H_ */
