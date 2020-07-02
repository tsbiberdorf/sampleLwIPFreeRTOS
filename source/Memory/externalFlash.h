/**
 * @file externalFlash.h
 * @brief public API implementation to interact with external spi flash
 * @see externalFlash.c for full implementation
 */

#ifndef SOURCE_MEMORY_EXTERNALFLASH_H_
#define SOURCE_MEMORY_EXTERNALFLASH_H_

#include "FreeRTOS.h"
#include "task.h"


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

/**
 * ingroup groupExtfOp
 * @{
 */
#define SAMPLE_SIZE_500MS (360)
#define EXF_PRINTF PRINTF


#define WRR 		(0x01)
#define WREN 		(0x06) // enable write
#define WREN_SIZE 	(1)
#define WRDI 		(0x04) // disable write
#define WRDI_SIZE 	(1)
#define CLSR		(0x30) // clear status register
#define CLSR_SIZE	(1)

#define EXTF_COMMAND_COMPLETED (0xA5A5)

#define EXTF_COMMAND_SIZE (1)
#define EXTF_ADDRESS_SIZE (3)
#define EXTF_PAGE_SIZE (256)
#define EXTF_TRANSFER_SIZE (EXTF_COMMAND_SIZE+EXTF_ADDRESS_SIZE+EXTF_PAGE_SIZE+1)
#define DATA_LOG_BUFFER_SIZE (4385)
#define EXTF_SECTOR_SIZE (256*EXTF_PAGE_SIZE) // memory size of each sector (bytes)
#define NUMBER_EXTF_SECTORS (64) // external flash contains 64 sectors
#define EXT_FLASH_SIZE (EXTF_SECTOR_SIZE*NUMBER_EXTF_SECTORS)

/*
 * Below macro is used to install or uninstall all functions of external SPI flash.
 * All operations of external SPI flash will be hidden from the other files which use it.
 *
 * When macro is set to 1, all variables and logging functions will be compiled into final firmware.
 * When macro is set to 0, only empty logging functions will be compiled into final firmware because some file may use them,
 * unused variables will not be compiled.
 *
 * When macro is set to 1, the external SPI flash codes also check the availability of SPI flash every time it is powered up.
 * If SPI flash is not available (not populated or not working), all logging operations will be bypassed.
 *
 */


#define EXTERNAL_FLASH_SUPPORT (1)

typedef enum ExtfState_e
{
	extFlashIdle,
	extFlashBusy,
	extFlashDone,
	extFlashError
}eExtfState_t;

typedef enum ExternalFlashStates_e
{
	extfCommandCompleted,
	extfRDID,
	extfRDSR,
	extfWREN,
	extfWRDI,

	extfBE1,// rdsr
	extfBE2, // wren
	extfBE3, // rdsr
	extfBE4, // be
	extfBE5, // rdsr
	extfBE6, // done

	extfRD1, // rdsr
	extfRD2, // rd
	extfRD3, // done

	extfPP1, // rdsr
	extfPP2, // wren
	extfPP3, // rdsr
	extfPP4, // pp
	extfPP5, // rdsr
	extfPP6, // done

	extfSE1, // rdsr
	extfSE2, // wren
	extfSE3, // rdsr
	extfSE4, // se
	extfSE5, // rdsr
	extfSE6, // done
	extCommandError,
}eExternalFlashStates_t;

typedef enum ExternalFlashSectors_e{
	eExtfSector1  = 0x00000000,
	eExtfSector2  = eExtfSector1 + EXTF_SECTOR_SIZE,
	eExtfSector3  = eExtfSector2 + EXTF_SECTOR_SIZE,
	eExtfSector4  = eExtfSector3 + EXTF_SECTOR_SIZE,
	eExtfSector5  = eExtfSector4 + EXTF_SECTOR_SIZE,
	eExtfSector6  = eExtfSector5 + EXTF_SECTOR_SIZE,
	eExtfSector7  = eExtfSector6 + EXTF_SECTOR_SIZE,
	eExtfSector8  = eExtfSector7 + EXTF_SECTOR_SIZE,
	eExtfSector9  = eExtfSector8 + EXTF_SECTOR_SIZE,
	eExtfSector10 = eExtfSector9 + EXTF_SECTOR_SIZE,
	eExtfSector11 = eExtfSector10 + EXTF_SECTOR_SIZE,
	eExtfSector12 = eExtfSector11 + EXTF_SECTOR_SIZE,
	eExtfSector13 = eExtfSector12 + EXTF_SECTOR_SIZE,
	eExtfSector14 = eExtfSector13 + EXTF_SECTOR_SIZE,
	eExtfSector15 = eExtfSector14 + EXTF_SECTOR_SIZE,
	eExtfSector16 = eExtfSector15 + EXTF_SECTOR_SIZE,
	eExtfSector17 = eExtfSector16 + EXTF_SECTOR_SIZE,
	eExtfSector18 = eExtfSector17 + EXTF_SECTOR_SIZE,
	eExtfSector19 = eExtfSector18 + EXTF_SECTOR_SIZE,
	eExtfSector20 = eExtfSector19 + EXTF_SECTOR_SIZE,
	eExtfSector21 = eExtfSector20 + EXTF_SECTOR_SIZE,
	eExtfSector22 = eExtfSector21 + EXTF_SECTOR_SIZE,
	eExtfSector23 = eExtfSector22 + EXTF_SECTOR_SIZE,
	eExtfSector24 = eExtfSector23 + EXTF_SECTOR_SIZE,
	eExtfSector25 = eExtfSector24 + EXTF_SECTOR_SIZE,
	eExtfSector26 = eExtfSector25 + EXTF_SECTOR_SIZE,
	eExtfSector27 = eExtfSector26 + EXTF_SECTOR_SIZE,
	eExtfSector28 = eExtfSector27 + EXTF_SECTOR_SIZE,
	eExtfSector29 = eExtfSector28 + EXTF_SECTOR_SIZE,
	eExtfSector30 = eExtfSector29 + EXTF_SECTOR_SIZE,
	eExtfSector31 = eExtfSector30 + EXTF_SECTOR_SIZE,
	eExtfSector32 = eExtfSector31 + EXTF_SECTOR_SIZE,
	eExtfSector33 = eExtfSector32 + EXTF_SECTOR_SIZE,
	eExtfSector34 = eExtfSector33 + EXTF_SECTOR_SIZE,
	eExtfSector35 = eExtfSector34 + EXTF_SECTOR_SIZE,
	eExtfSector36 = eExtfSector35 + EXTF_SECTOR_SIZE,
	eExtfSector37 = eExtfSector36 + EXTF_SECTOR_SIZE,
	eExtfSector38 = eExtfSector37 + EXTF_SECTOR_SIZE,
	eExtfSector39 = eExtfSector38 + EXTF_SECTOR_SIZE,
	eExtfSector40 = eExtfSector39 + EXTF_SECTOR_SIZE,
	eExtfSector41 = eExtfSector40 + EXTF_SECTOR_SIZE,
	eExtfSector42 = eExtfSector41 + EXTF_SECTOR_SIZE,
	eExtfSector43 = eExtfSector42 + EXTF_SECTOR_SIZE,
	eExtfSector44 = eExtfSector43 + EXTF_SECTOR_SIZE,
	eExtfSector45 = eExtfSector44 + EXTF_SECTOR_SIZE,
	eExtfSector46 = eExtfSector45 + EXTF_SECTOR_SIZE,
	eExtfSector47 = eExtfSector46 + EXTF_SECTOR_SIZE,
	eExtfSector48 = eExtfSector47 + EXTF_SECTOR_SIZE,
	eExtfSector49 = eExtfSector48 + EXTF_SECTOR_SIZE,
	eExtfSector50 = eExtfSector49 + EXTF_SECTOR_SIZE,
	eExtfSector51 = eExtfSector50 + EXTF_SECTOR_SIZE,
	eExtfSector52 = eExtfSector51 + EXTF_SECTOR_SIZE,
	eExtfSector53 = eExtfSector52 + EXTF_SECTOR_SIZE,
	eExtfSector54 = eExtfSector53 + EXTF_SECTOR_SIZE,
	eExtfSector55 = eExtfSector54 + EXTF_SECTOR_SIZE,
	eExtfSector56 = eExtfSector55 + EXTF_SECTOR_SIZE,
	eExtfSector57 = eExtfSector56 + EXTF_SECTOR_SIZE,
	eExtfSector58 = eExtfSector57 + EXTF_SECTOR_SIZE,
	eExtfSector59 = eExtfSector58 + EXTF_SECTOR_SIZE,
	eExtfSector60 = eExtfSector59 + EXTF_SECTOR_SIZE,
	eExtfSector61 = eExtfSector60 + EXTF_SECTOR_SIZE,
	eExtfSector62 = eExtfSector61 + EXTF_SECTOR_SIZE,
	eExtfSector63 = eExtfSector62 + EXTF_SECTOR_SIZE,
	eExtfSector64 = eExtfSector63 + EXTF_SECTOR_SIZE, //current max is 0x003F0000 with page size 256 and sector size 256

	eExtfFaultSector1 = eExtfSector1,
	eExtfFaultSector2 = eExtfSector2,

}eExtfFlashSector_t;

/**@} */
uint16_t ResetLogEntries();
uint8_t ExternalFlashReady();

void InitExternalFlash();
void initExternalFlashThread();
TaskHandle_t GetExternalFlashCommandTaskId();

int8_t ExtfRDID();
int8_t ExtfRDSR();

int8_t ExtfWren();
uint8_t ExtfWrdi();
int8_t ExtfBE();
int8_t ExtfDumpPageData(uint32_t Address);
uint8_t setExtfReadyFlag(uint8_t value);
uint8_t GetExtfReadyFlag();
TaskHandle_t getExtfTaskHandle();
uint8_t * GetWriteDataBuffer();

void SetExtfAddress(uint32_t Address);
void SetExtfWriteSize(uint32_t NumberOfBytes);
int8_t ExtFlashPageRead(uint32_t Address,uint8_t *ptrDest);
int8_t ExtfSectorErase(uint32_t Address);
eExtfState_t GetExtfState();

#endif /* GC100_USERINTERFACE_EXTERNALFLASH_H_ */
