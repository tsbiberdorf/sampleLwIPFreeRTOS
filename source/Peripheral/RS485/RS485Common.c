/**
 * @ingroup groupRS485
 * {
 * @file RS485Common.c
 * @details Full implementation of RS485Common
 * @see RS485Common.h for public API
 *
 */

//#include <RTT/SEGGER_RTT.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "fsl_crc.h"
#include "fsl_debug_console.h"
#include "Peripheral/CRC/crcInterface.h"
#include "RS485Common.h"


/**
 * @brief perform a CRC check on a 485 payload.  will return zero if OK
 */
uint16_t VerifyCmdCRC(sRS485Cmd_t *verifyCRC)
{
	SemaphoreHandle_t crcSemaphore;

	uint16_t crcValue;
	crcSemaphore = InitCrc16();
	assert(crcSemaphore);

    crcWriteData((uint8_t *)&(verifyCRC->PortId), 1);
    crcWriteData((uint8_t *)&(verifyCRC->Cmd), 1);
    crcWriteData((uint8_t *)&(verifyCRC->uSize_t.size), 2);

    crcWriteData(verifyCRC->Payload, (verifyCRC->uSize_t.size) );

    crcValue = crcGet16bitResult();

	crcReleaseSemaphore(crcSemaphore);

    if( crcValue == verifyCRC->uCRC_t.CRC)
    {
		PRINTF("Verify Cmd:%x CRC:%x\r\n",verifyCRC->Cmd,verifyCRC->uCRC_t.CRC);
    	crcValue = 0;
    }

    return crcValue;
}

/**
 * @brief print rs485 information
 */
void seggerPrintRS485(sRS485Cmd_t *printCmd)
{
	uint16_t idx;
	PRINTF( "%d %x %x %d %x\n",printCmd->State,printCmd->PortId,printCmd->Cmd
			,printCmd->uSize_t.size,printCmd->uCRC_t.CRC);
	for(idx=0;idx<printCmd->uSize_t.size;idx++)
	{
		PRINTF( "%x ",printCmd->Payload[idx]);
		if(idx>0 &&(idx%8 == 0))
		{
			PRINTF( "\n");
		}
	}
	PRINTF( "\n\n");
}

/**}*/
