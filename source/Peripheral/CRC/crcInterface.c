/*
 * crcInterface.c
 *
 *  Created on: May 13, 2020
 *      Author: ThreeBoysTech
 */

#include <assert.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "fsl_common.h"
#include "fsl_crc.h"

#define NGR_CRC (CRC0)

//const static CRC_Type *tl_CRCbase = CRC0;
const static uint32_t tl_CRCseed = 0x0;
static SemaphoreHandle_t tl_CRCSem = NULL;


/*!
 * @brief Init for CRC-16/MAXIM.
 * @details Init CRC peripheral module for CRC-16/MAXIM protocol.
 *          width=16 poly=0x8005 init=0x0000 refin=true refout=true xorout=0xffff check=0x44c2 name="CRC-16/MAXIM"
 *          http://reveng.sourceforge.net/crc-catalogue/
 */
SemaphoreHandle_t InitCrc16()
{
    crc_config_t config;

	if(tl_CRCSem == NULL)
	{
		tl_CRCSem = xSemaphoreCreateMutex();
		assert(tl_CRCSem);
	}

	xSemaphoreTake(tl_CRCSem,portMAX_DELAY);

    config.polynomial = 0x8005;
    config.seed = tl_CRCseed;
    config.reflectIn = true;
    config.reflectOut = true;
    config.complementChecksum = true;
    config.crcBits = kCrcBits16;
    config.crcResult = kCrcFinalChecksum;

    CRC_Init(NGR_CRC, &config);

    return tl_CRCSem;
}

void crcReleaseSemaphore(SemaphoreHandle_t CRCSemaphore)
{
	xSemaphoreGive(CRCSemaphore);
}

void crcWriteData(const uint8_t *data, size_t dataSize)
{
	CRC_WriteData(NGR_CRC, data, dataSize);
}

uint16_t crcGet16bitResult()
{
	return CRC_Get16bitResult(NGR_CRC);
}

