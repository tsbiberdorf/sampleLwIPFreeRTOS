/**
 * @details this was a file copied over from MP8000 that was in charge of getting the status of the unit?
 * It was originally needed for part of ethernet communication so it was copied over
 * EtherNetSupport.c
 *
 *  Created on: Oct 20, 2017
 *      Author: tbiberdorf
 */
#include <stdio.h>
#include <stdint.h>
#include "Memory/CommonComm.h"

#define CONFIGURED (1<<2)
#define MINOR_RECOVERABLE_FAULT (1<<8)
#define MINOR_UNRECOVERABLE_FAULT (1<<9)
#define MAJOR_RECOVERABLE_FAULT (1<<10)
#define MAJOR_UNRECOVERABLE_FAULT (1<<11)

/**
 * @brief determine what the Identity status (attribute 6) value
 *
 */
uint16_t GetIdentityStatus()
{
	uint32_t lvValue;
	uint32_t hvValue;
	uint16_t faultValue;
	uint16_t status;
	status = 0;

	MemMapRead(CFG_MEMMAP_LV_ADDR, CFG_MEMMAP_LV_SIZE, &lvValue);
	MemMapRead(CFG_MEMMAP_HV_ADDR, CFG_MEMMAP_HV_SIZE, &hvValue);
	if(hvValue > lvValue)
	{
		status |= CONFIGURED;
	}
	MemMapRead(CFG_MEMMAP_FAULT_STAT_ADDR, CFG_MEMMAP_FAULT_STAT_SIZE, &faultValue);
	if(faultValue)
	{
		status |= MAJOR_RECOVERABLE_FAULT;
	}
	return status;
}
