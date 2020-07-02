/*
 * CommonComm.c
 *
 *  Created on: Jun 29, 2015
 *      Author: AMcguire
 */

#include <assert.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "NGRMRelay.h"
#include "CommonComm.h"
#include "LocalModbusMemory.h"
#include "ProductionTest/ProductionTest.h"

#define CONFIG_SECTOR_SIZE (4096)


//static uint16_t TestDummyValue = 17; //@todo remove once poc is complete
static uint8_t *tl_ptrPTMemMap = NULL;

//static uint8_t *tl_ptrConfigMemMap = NULL;
//static uint8_t *tl_ptrRealTimeMemMap = NULL;
//static uint8_t *tl_ptrFaultMemMap = NULL;
//static uint8_t *tl_ptrSetStaticIPMap = NULL;
//static uint8_t *tl_ptrRTCTimestampMap = NULL;
static SemaphoreHandle_t gMemMapSem = NULL;
//SemaphoreHandle_t ts_ManualRestartSem = NULL;

//SemaphoreHandle_t ts_PushButtonManualRestartSem = NULL;
//static int ts_RestartMode = MP8K_RESET_MODE_NORMAL;
//extern int32_t nvmViewLogDate(eSelectLog_t LogToView,int32_t ClockTime,int32_t Offest, Mp8kFaultEntry_t *ptrSaveAddress);

extern uint16_t readSingleMbReg(int index);
/**
 * @brief change all settings back to factory settings.
 */
int ApplyFactoryDefaultSettings()
{
//	uint32_t longValue;
//	uint16_t shortValue;
//
//	longValue = MAJOR_VERSION<<24 | MINOR_VERSION<<16 | VERSION_VERSION<<8 | REVISION_VERSION;
//	MemMapWrite(CFG_MEMMAP_FW_REV_ADDR,CFG_MEMMAP_FW_REV_SIZE,&longValue,0);
//
//	shortValue = CFG_MEMMAP_PC_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_PC_ADDR,CFG_MEMMAP_PC_SIZE,&shortValue,0);
//
//	shortValue = CFG_MEMMAP_MULT_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_MULT_ADDR,CFG_MEMMAP_MULT_SIZE,&shortValue,0);
//
//	shortValue = CFG_MEMMAP_PT_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_PT_ADDR,CFG_MEMMAP_PT_SIZE,&shortValue,0);
//
//	longValue = CFG_MEMMAP_LV_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_LV_ADDR,CFG_MEMMAP_LV_SIZE,&longValue,0);
//
//	longValue = CFG_MEMMAP_HV_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_HV_ADDR,CFG_MEMMAP_HV_SIZE,&longValue,0);
//
//	shortValue = CFG_MEMMAP_VUB_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_VUB_ADDR,CFG_MEMMAP_VUB_SIZE,&shortValue,0);
//
//	shortValue = CFG_MEMMAP_TC_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_TC_ADDR,CFG_MEMMAP_TC_SIZE,&shortValue,0);
//
//	longValue = CFG_MEMMAP_OC_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_OC_ADDR,CFG_MEMMAP_OC_SIZE,&longValue,0);
//
//	longValue = CFG_MEMMAP_UC_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_UC_ADDR,CFG_MEMMAP_UC_SIZE,&longValue,0);
//
//	shortValue = CFG_MEMMAP_UCTD_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_UCTD_ADDR,CFG_MEMMAP_UCTD_SIZE,&shortValue,0);
//
//	shortValue = CFG_MEMMAP_CUB_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_CUB_ADDR,CFG_MEMMAP_CUB_SIZE,&shortValue,0);
//
//	shortValue = CFG_MEMMAP_EIPTO_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_EIPTO_ADDR,CFG_MEMMAP_EIPTO_SIZE,&shortValue,0);
//
//	shortValue = CFG_MEMMAP_LIN_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_LIN_ADDR,CFG_MEMMAP_LIN_SIZE,&shortValue,0);
//
//	longValue = CFG_MEMMAP_RD0_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_RD0_ADDR,CFG_MEMMAP_RD0_SIZE,&longValue,0);
//
//	longValue = CFG_MEMMAP_RD1_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_RD1_ADDR,CFG_MEMMAP_RD1_SIZE,&longValue,0);
//
//	longValue = CFG_MEMMAP_RD2_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_RD2_ADDR,CFG_MEMMAP_RD2_SIZE,&longValue,0);
//
//	longValue = CFG_MEMMAP_RD3_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_RD3_ADDR,CFG_MEMMAP_RD3_SIZE,&longValue,0);
//
//	shortValue = CFG_MEMMAP_RF_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_RF_ADDR,CFG_MEMMAP_RF_SIZE,&shortValue,0);
//
//	shortValue = CFG_MEMMAP_RU_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_RU_ADDR,CFG_MEMMAP_RU_SIZE,&shortValue,0);
//
//	longValue = CFG_MEMMAP_GF_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_GF_ADDR,CFG_MEMMAP_GF_SIZE,&longValue,0);
//
//	shortValue = CFG_MEMMAP_GFTD_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_GFTD_ADDR,CFG_MEMMAP_GFTD_SIZE,&shortValue,0);
//
//	longValue = CFG_MEMMAP_LKW_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_LKW_ADDR,CFG_MEMMAP_LKW_SIZE,&longValue,0);
//
//	longValue = CFG_MEMMAP_HKW_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_HKW_ADDR,CFG_MEMMAP_HKW_SIZE,&longValue,0);
//
//	shortValue = CFG_MEMMAP_HPRTD_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_HPRTD_ADDR,CFG_MEMMAP_HPRTD_SIZE,&shortValue,0);
//
//	shortValue = CFG_MEMMAP_STLP_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_STLP_ADDR,CFG_MEMMAP_STLP_SIZE,&shortValue,0);
//
//	longValue = CFG_MEMMAP_SIP_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_SIP_ADDR,CFG_MEMMAP_SIP_SIZE,&longValue,0);
//
//	longValue = CFG_MEMMAP_CNFG_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_CNFG_ADDR,CFG_MEMMAP_CNFG_SIZE,&longValue,0);
//
//	longValue = CFG_MEMMAP_MRTNVM_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_MRTNVM_ADDR,CFG_MEMMAP_MRTNVM_SIZE,&longValue,0);
//
//	longValue = CFG_MEMMAP_MSTNVM_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_MSTNVM_ADDR,CFG_MEMMAP_MSTNVM_SIZE,&longValue,0);
//
//	shortValue = CFG_MEMMAP_SCNTNVM_DEFAULT;
//	MemMapWrite(CFG_MEMMAP_SCNTNVM_ADDR,CFG_MEMMAP_SCNTNVM_SIZE,&shortValue,1);
//
//	vTaskDelay(250); // allow flash code to complete the update
//
//	wdtForceReset("ApplyFactoryDefaultSettings");
//
	return 1;
}

/*!
 * Initializing MemMap. Read from NVM to populate the MemMap.
 * @return 0, assert on error
 */

extern uint8_t* MemInitLocalPTBuffer();

int MemMapInit()
{
	tl_ptrPTMemMap = MemInitLocalPTBuffer(); //@todo move this once a function has been decided to initialize all of the memory
//	tl_ptrMftMemMap = &TestDummyValue;
//	tl_ptrConfigMemMap = NvmGetConfigurationRamMemMap();
//	tl_ptrRealTimeMemMap = NvmGetRealTimeRamMemMap();
//	tl_ptrFaultMemMap = NvmGetFaultRamMemMap();
//	tl_ptrSetStaticIPMap = NvmGetSetStaticIpRamMemMap();
//	tl_ptrRTCTimestampMap = NvmGetRTCTimeRamMemMap();
//	assert(tl_ptrConfigMemMap);

//	gMemMapSem = xSemaphoreCreateBinary();
//	assert(gMemMapSem);

//	ts_ManualRestartSem = xSemaphoreCreateBinary();
//	assert(ts_ManualRestartSem);
//
//	ts_PushButtonManualRestartSem = xSemaphoreCreateBinary();
//	assert(ts_PushButtonManualRestartSem);

	return 0;
}

/*!
 * Reads data from Memory Map.
 * @param Address Start address of the read request
 * @param Size Size in bytes of data to read
 * @param Value Pointer into data structure this function will write to.
 * @return 0 if no error, -1 for address failure, -2 for size failure
 */
int MemMapRead(uint16_t Address, uint16_t Size, void *Value)
{
	enum _MemMapFailureCode_e
	{
		eMemMapFailure_NoFailure = 0,
		eMemMapFailure_Address = -1,
		eMemMapFailure_Size = -2
	};

	int error = eMemMapFailure_NoFailure;
	int16_t i,e,offset, memLocalIdx;

	//@todo reinclude semaphore once memory actually needs protected access
	//poc assigning local memory value
	//@todo special case returning the value to be different based on the address reading requested
	if(Address + Size < MFT_MEMMAP_SIZE+1)
		{
			//error = xSemaphoreTake(gMemMapSem, portMAX_DELAY);
//			if(error == pdPASS)
//			{
				error = 0;
//				Value = tl_ptrPTMemMap;
				for(i = 0;i<Size;i+=1)
				{
					*((uint16_t *)(Value+i)) = (uint16_t*) readSingleMbReg(i);
//					*((uint8_t *)(Value+i)) = *(tl_ptrMftMemMap);
//					*((uint8_t *)(Value+i)) = *(getMftStructPtr());
					if( (i) == (MFT_MEMMAP_END_ADDR - MFT_MEMMAP_CMD_ADDR))
//					if(tl_ptrPTMemMap+i)
					{
						/*
						 * verify that memory is not read past the
						 * real time memory size
						 */
						for(e=0;e<Size;e++)
						{
							/*
							 * zero return memory due to error found
							 */
							*((uint8_t *)(Value+e)) = 0x0;
						}
						error = eMemMapFailure_Size;
						break;
					}
				}

//				xSemaphoreGive(gMemMapSem);
//			}
		}
		else if((Address >= MFT_MEMMAP_START_ADDR) && (Address < MFT_MEMMAP_END_ADDR)
							&& ( (Address+Size) <= MFT_MEMMAP_END_ADDR))
		{
			for(i=0, memLocalIdx=0, offset = Address;i<Size;i+=2, memLocalIdx++)
			{
				*((uint16_t*)(Value+i)) =  (uint16_t*) readSingleMbReg(memLocalIdx);
				if( (offset+i) == MFT_MEMMAP_SIZE)
				{
					/*
					 * verify that memory is not read past the
					 * configuration memory size
					 */
					error = eMemMapFailure_Size;
					break;
				}
			}
		}
		else
		{
			error = eMemMapFailure_Address;
		}
//	if(Address + Size < COMM_MEMMAP_SIZE)
//	{
//		error = xSemaphoreTake(gMemMapSem, portMAX_DELAY);
//		if(error == pdPASS)
//		{
//			error = 0;
//			for(i=0,offset = Address;i<Size;i++)
//			{
//				*((uint8_t *)(Value+i)) = *(tl_ptrConfigMemMap + offset + i);
//				if( (offset+i) == COMM_MEMMAP_SIZE)
//				{
//					/*
//					 * verify that memory is not read past the
//					 * configuration memory size
//					 */
//					error = eMemMapFailure_Size;
//					break;
//				}
//			}
//
//			xSemaphoreGive(gMemMapSem);
//		}
//	}
//	else if( (Address >= CFG_MEMMAP_RTREG_ADDR) && (Address <CFG_MEMMAP_RTREG_END) )
//	{
//		/*
//		 * ==== Realtime Status Registers ====
//		 */
//		error = xSemaphoreTake(gMemMapSem, portMAX_DELAY);
//		if(error == pdPASS)
//		{
//			error = 0;
//			for(i = 0, offset = (Address-CFG_MEMMAP_RTREG_ADDR);i<Size;i++)
//			{
//				*((uint8_t *)(Value+i)) = *(tl_ptrRealTimeMemMap + offset + i);
//				if( (offset+i) == (CFG_MEMMAP_RTREG_END - CFG_MEMMAP_RTREG_ADDR))
//				{
//					/*
//					 * verify that memory is not read past the
//					 * real time memory size
//					 */
//					for(e=0;e<Size;e++)
//					{
//						/*
//						 * zero return memory due to error found
//						 */
//						*((uint8_t *)(Value+e)) = 0x0;
//					}
//					error = eMemMapFailure_Size;
//					break;
//				}
//			}
//
//			xSemaphoreGive(gMemMapSem);
//		}
//	}
//	else if( (Address >= CFG_MEMMAP_FAULRREG_ADDR) && (Address < CFG_MEMMAP_FAULRREG_END)
//			&& ( (Address+Size) <= CFG_MEMMAP_FAULRREG_END))
//	{
//		/*
//		 * ==== Fault Retrieval Registers ====
//		 */
//		error = xSemaphoreTake(gMemMapSem, portMAX_DELAY);
//		if(error == pdPASS)
//		{
//			error = 0;
//			for(i = 0, offset = (Address-CFG_MEMMAP_FAULRREG_ADDR);i<Size;i++)
//			{
//				*((uint8_t *)(Value+i)) = *(tl_ptrFaultMemMap + offset + i);
//				if( (offset+i) == (CFG_MEMMAP_FAULRREG_END - CFG_MEMMAP_FAULRREG_ADDR))
//				{
//					/*
//					 * verify that memory is not read past the
//					 * fault memory size
//					 */
//					for(e=0;e<Size;e++)
//					{
//						/*
//						 * zero return memory due to error found
//						 */
//						*((uint8_t *)(Value+e)) = 0x0;
//					}
//					error = eMemMapFailure_Size;
//					break;
//				}
//			}
//
//			xSemaphoreGive(gMemMapSem);
//		}
//	}
//	else if( (Address >= CFG_MEMMAP_SETIP_ADDR) && (Address < CFG_MEMMAP_SETIP_END)
//			&& ( (Address+Size) <= CFG_MEMMAP_SETIP_END))
//	{
//		/*
//		 * ==== Set Static IP Registers ====
//		 */
//		error = xSemaphoreTake(gMemMapSem, portMAX_DELAY);
//		if(error == pdPASS)
//		{
//			error = 0;
//			for(i = 0, offset = (Address-CFG_MEMMAP_SETIP_ADDR);i<Size;i++)
//			{
//				*((uint8_t *)(Value+i)) = *(tl_ptrSetStaticIPMap + offset + i);
//				if( (offset+i) == (CFG_MEMMAP_SETIP_END - CFG_MEMMAP_SETIP_ADDR))
//				{
//					/*
//					 * verify that memory is not read past the
//					 * fault memory size
//					 */
//					for(e=0;e<Size;e++)
//					{
//						/*
//						 * zero return memory due to error found
//						 */
//						*((uint8_t *)(Value+e)) = 0x80+i;
//					}
//					error = eMemMapFailure_Size;
//					break;
//				}
//			}
//
//			xSemaphoreGive(gMemMapSem);
//		}
//	}
//	else if( (Address >= CFG_MEMMAP_RTCTIME_ADDR) && (Address < CFG_MEMMAP_RTCTIME_END)
//			&& ( (Address+Size) <= CFG_MEMMAP_RTCTIME_END))
//	{
//		/* ==== Modbus RTC register ==== */
//		error = xSemaphoreTake(gMemMapSem, portMAX_DELAY);
//		if(error == pdPASS)
//		{
//			if( Size == 4)
//			{
//				*((uint32_t *)(Value)) = *((uint32_t *)tl_ptrRTCTimestampMap);
//			}
//			error = 0;
//			xSemaphoreGive(gMemMapSem);
//		}
//	}
//	else
//	{
//		error = eMemMapFailure_Address;
//	}
	return error;

}

/**
 * @details check sequence of configuration registers to determine if a static IP address should be
 * changed.  IPv4 address is formated as follows:  AA:BB:CC:DD
 * Sequence is as follows:
 * <ul>
 * 		<li> LV set to 567.0 Vrms
 * 		<li> HV set to 7.0 Vrms
 * 		<li> RD0 set IPv4 address: AA
 * 		<li> RD1 set IPv4 address: BB
 * 		<li> RD2 set IPv4 address: CC
 * 		<li> RD3 set IPv4 address: DD
 * 	</ul>
 * 	After RD3 has been set, all registers will be returned to their original values
 *
 * 	@return void
 */
//void CheckStaticIpChangeRequest(uint16_t Address, uint16_t Size, const void *ptrValue)
//{
//	enum _StaticIpChange_e
//	{
//		eSIPinit,
//		eSIPgotLV,
//		eSIPgotHV,
//		eSIPgotRD0,
//		eSIPgotRD1,
//		eSIPgotRD2,
//		eSIPgotRD3,
//	};
//	typedef struct _StaticIpChange_s
//	{
//		enum _StaticIpChange_e state;
//		uint32_t lv;
//		uint32_t hv;
//		uint32_t rd0;
//		uint32_t rd1;
//		uint32_t rd2;
//		uint32_t rd3;
//		uint32_t ipv4;
//	}sStaticIpChage_t;
//	static sStaticIpChage_t tl_StaticIpChange = {.state=eSIPinit};
//
//	if( (Address <= CFG_MEMMAP_LV_ADDR) && ((Address + Size)>CFG_MEMMAP_LV_ADDR ) )
//	{
//		tl_StaticIpChange.lv = *(uint32_t *)(ptrValue);
//		if( tl_StaticIpChange.lv == 56700) // 567.0 V
//		{
//			tl_StaticIpChange.state = eSIPgotLV;
//			/*
//			 * store LV value if to revert back after a static IP assignment
//			 */
//			tl_StaticIpChange.lv = *(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_LV_ADDR);
//		}
//		else
//		{
//			tl_StaticIpChange.state = eSIPinit;
//		}
//	}
//
//	switch(tl_StaticIpChange.state)
//	{
//	case eSIPgotLV:
//		if( (Address <= CFG_MEMMAP_HV_ADDR) && ((Address + Size)>CFG_MEMMAP_HV_ADDR ) )
//		{
//			/*
//			 * will only continue if the HV has been set to 7.0Vrms
//			 */
//			tl_StaticIpChange.hv = *(uint32_t *)(ptrValue);
//			if( tl_StaticIpChange.hv == 700) // 7.0 V
//			{
//				tl_StaticIpChange.state = eSIPgotHV;
//				/*
//				 * store LV value if to revert back after a static IP assignment
//				 */
//				tl_StaticIpChange.hv = *(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_HV_ADDR);
//			}
//			else
//			{
//				tl_StaticIpChange.state = eSIPinit;
//			}
//		}
//		break;
//
//	case eSIPgotHV:
//		if( (Address <= CFG_MEMMAP_RD0_ADDR) && ((Address + Size)>CFG_MEMMAP_RD0_SIZE ) )
//		{
//			/*
//			 * RD0 will become the AA of IPv4 address: AA:BB:CC:DD
//			 */
//			tl_StaticIpChange.ipv4 = ((*(uint32_t *)(ptrValue) ) & 0xFF); // save in Little Endian format
//			tl_StaticIpChange.rd0 = *(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_RD0_ADDR);
//			tl_StaticIpChange.state = eSIPgotRD0;
//		}
//		break;
//
//	case eSIPgotRD0:
//		if( (Address <= CFG_MEMMAP_RD1_ADDR) && ((Address + Size)>CFG_MEMMAP_RD1_SIZE ) )
//		{
//			/*
//			 * RD1 will become the BB of IPv4 address: AA:BB:CC:DD
//			 */
//			tl_StaticIpChange.ipv4 |= ((*(uint32_t *)(ptrValue) ) & 0xFF) << 8; // save in Little Endian format
//			tl_StaticIpChange.rd1 = *(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_RD1_ADDR);
//			tl_StaticIpChange.state = eSIPgotRD1;
//		}
//		break;
//
//	case eSIPgotRD1:
//		if( (Address <= CFG_MEMMAP_RD2_ADDR) && ((Address + Size)>CFG_MEMMAP_RD2_SIZE ) )
//		{
//			/*
//			 * RD2 will become the CC of IPv4 address: AA:BB:CC:DD
//			 */
//			tl_StaticIpChange.ipv4 |= ((*(uint32_t *)(ptrValue) ) & 0xFF) << 16; // save in Little Endian format
//			tl_StaticIpChange.rd2 = *(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_RD2_ADDR);
//			tl_StaticIpChange.state = eSIPgotRD2;
//		}
//		break;
//
//	case eSIPgotRD2:
//		if( (Address <= CFG_MEMMAP_RD3_ADDR) && ((Address + Size)>CFG_MEMMAP_RD3_SIZE ) )
//		{
//			/*
//			 * RD3 will become the DD of IPv4 address: AA:BB:CC:DD
//			 */
//			tl_StaticIpChange.ipv4 |= ((*(uint32_t *)(ptrValue) ) & 0xFF) << 24; // save in Little Endian format
//			tl_StaticIpChange.rd3 = *(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_RD3_ADDR);
//			tl_StaticIpChange.state = eSIPgotRD3;
//		}
//		break;
//	case eSIPinit:
//	case eSIPgotRD3:
//		break;
//	}
//
//	if( tl_StaticIpChange.state == eSIPgotRD3 )
//	{
//		/*
//		 * change the static IP address and restore all registers back to their original states
//		 */
//		*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_LV_ADDR) =  tl_StaticIpChange.lv;
//		*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_HV_ADDR) =  tl_StaticIpChange.hv;
//		*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_RD0_ADDR) = tl_StaticIpChange.rd0;
//		*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_RD1_ADDR) = tl_StaticIpChange.rd1;
//		*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_RD2_ADDR) = tl_StaticIpChange.rd2;
//		*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_RD3_ADDR) = tl_StaticIpChange.rd3;
//		*(uint32_t *)(ptrValue) = tl_StaticIpChange.rd3;
//		/*
//		 * update IP address storage location
//		 */
//		*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_SIP_ADDR) = tl_StaticIpChange.ipv4;
//		/**
//		 * @note when the phone app is updated to support changing the subnet and gateway values
//		 * this code will be replaced.  Until that happens, use the defaut settings
//		 * @note these values can be configured via the webpage only!!
//		 */
//		*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_SUBMASKIP_ADDR) = 0xFFFFFF; // change subnet mask to class C network
//		*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_GATEIP_ADDR) = 0x0; // allow any gateway address to be accepted
//		extern void IpAddrHandlerPhoneAppChangeIpAddr(uint32_t PhoneAppIpAddr,uint32_t SubNetMask, uint32_t GatewayIp);
//
//		IpAddrHandlerPhoneAppChangeIpAddr(tl_StaticIpChange.ipv4,*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_SUBMASKIP_ADDR),*(uint32_t *)(tl_ptrConfigMemMap + CFG_MEMMAP_GATEIP_ADDR));
//
//		tl_StaticIpChange.state = eSIPinit;
//	}
//
//	return;
//}

/*!
 * Writes data to Memory Map.
 * @param Address Start address of the read request
 * @param Size Size in bytes of data to read
 * @param ptrValue Pointer into data structure this function will read from.
 * @param Commit if true (1), this will trigger a write to NVM memory. Set this to zero when making
 * successive calls to this function to avoid excessive writing to flash memory.
 * @return 0 if no error, otherwise failure
 * @note If we just need to write out, then we can call this function with size == 0 and address == 0
 */
int MemMapWrite(uint16_t Address, uint16_t Size, const void *ptrValue, int Commit)
{
//	int error = 0;
	short int *ptrCmd;
//	static sBleEvent_t event;
//	int zeroValue;
//	uint16_t i,offset=0;
//	uint32_t currentTime;
//
	ptrCmd = (short int *) ptrValue;

	if(Address == 0x600)
	{
		//@todo remove hardcoded value of 1 for address write that triggers the manufacture command
		executePTCommand(*ptrCmd); //@todo special case this to only be called if the write was to the command address
	}

	//@todo add function call to execute


//		case CMD_RESET_MOTOR_SERVICE_TIME:
//			error = xSemaphoreTake(gMemMapSem, portMAX_DELAY);
//			if(error == pdPASS)
//			{
//				zeroValue = 0;
//				memcpy(tl_ptrConfigMemMap + CFG_MEMMAP_MSTNVM_ADDR, &zeroValue, CFG_MEMMAP_MST_SIZE);
//				memcpy(tl_ptrConfigMemMap + CFG_MEMMAP_MST_ADDR, &zeroValue, CFG_MEMMAP_MST_SIZE);
//				ConfigMemMapCommit();
//				zeroMST();
//				xSemaphoreGive(gMemMapSem);
//				error=0;
//			}
//			break;
//		default:
//			/*
//			 * not a supported command
//			 */
//			;
//		}
//	}
//	else if(Address >= CFG_MEMMAP_FRI_ADDR && Address <
//			(CFG_MEMMAP_FSDF_ADDR + CFG_MEMMAP_FSDF_SIZE) && Size <= CFG_MEMMAP_FSDF_SIZE)
//	{

//	return error;
	//@todo replace return with proper error returns based on processing
	return 0;
}





uint8_t *TakeConfigMemMapLock()
{
//	uint8_t *ptrConfigMemMapAddress = NULL;
//
//	if(xSemaphoreTake(gMemMapSem, portMAX_DELAY) == pdPASS)
//	{
//		ptrConfigMemMapAddress = tl_ptrConfigMemMap;
//	}
//	return ptrConfigMemMapAddress;
}

void GiveConfigMemMapLock()
{
//	xSemaphoreGive(gMemMapSem);
}



/*!
 * Request exclusive access to the memory map
 * @param block_time Amount of time to block before obtaining access.
 * @return
 */
uint8_t *MemmapAcquire(uint32_t block_time)
{
//	if(xSemaphoreTake(gMemMapSem, block_time) != pdTRUE)
//	{
//		return NULL;
//	}
//	else
//	{
//		return tl_ptrConfigMemMap;
//	}
return NULL;
}

/*!
 * Release exclusive access to the memory map.
 */
void MemmapRelease()
{
	//xSemaphoreGive(gMemMapSem);
}


/**
 * these are of the memory offsets into the CIP Assembly Configuration buffer where the configuration
 * variables will be stored.
 */
#define CIP_ASSEMBLYCNFG_OFFSET_MULT       (0)
#define CIP_ASSEMBLYCNFG_OFFSET_PT         (CIP_ASSEMBLYCNFG_OFFSET_MULT       + CFG_MEMMAP_MULT_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_LV         (CIP_ASSEMBLYCNFG_OFFSET_PT         + CFG_MEMMAP_PT_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_HV         (CIP_ASSEMBLYCNFG_OFFSET_LV         + CFG_MEMMAP_LV_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_VUB        (CIP_ASSEMBLYCNFG_OFFSET_HV         + CFG_MEMMAP_HV_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_TC         (CIP_ASSEMBLYCNFG_OFFSET_VUB        + CFG_MEMMAP_VUB_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_OC         (CIP_ASSEMBLYCNFG_OFFSET_TC         + CFG_MEMMAP_TC_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_UC         (CIP_ASSEMBLYCNFG_OFFSET_OC         + CFG_MEMMAP_OC_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_UCTD       (CIP_ASSEMBLYCNFG_OFFSET_UC         + CFG_MEMMAP_UC_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_CUB        (CIP_ASSEMBLYCNFG_OFFSET_UCTD       + CFG_MEMMAP_UCTD_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_RES1       (CIP_ASSEMBLYCNFG_OFFSET_CUB        + CFG_MEMMAP_CUB_SIZE) // orignally CUBTD, not not supported
#define CIP_ASSEMBLYCNFG_OFFSET_LIN        (CIP_ASSEMBLYCNFG_OFFSET_RES1       + CFG_MEMMAP_CUBTD_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_RD0        (CIP_ASSEMBLYCNFG_OFFSET_LIN        + CFG_MEMMAP_LIN_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_RD1        (CIP_ASSEMBLYCNFG_OFFSET_RD0        + CFG_MEMMAP_RD0_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_RD2        (CIP_ASSEMBLYCNFG_OFFSET_RD1        + CFG_MEMMAP_RD1_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_RD3        (CIP_ASSEMBLYCNFG_OFFSET_RD2        + CFG_MEMMAP_RD2_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_RU         (CIP_ASSEMBLYCNFG_OFFSET_RD3        + CFG_MEMMAP_RD3_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_RF         (CIP_ASSEMBLYCNFG_OFFSET_RU         + CFG_MEMMAP_RU_SIZE)


#define CIP_ASSEMBLYCNFG_OFFSET_GF         (CIP_ASSEMBLYCNFG_OFFSET_RF         + CFG_MEMMAP_RU_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_GFTD       (CIP_ASSEMBLYCNFG_OFFSET_GF         + CFG_MEMMAP_GF_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_GFID       (CIP_ASSEMBLYCNFG_OFFSET_GFTD       + CFG_MEMMAP_GFTD_SIZE) // not supported
#define CIP_ASSEMBLYCNFG_OFFSET_LKW        (CIP_ASSEMBLYCNFG_OFFSET_GFID       + CFG_MEMMAP_GFID_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_HKW        (CIP_ASSEMBLYCNFG_OFFSET_LKW        + CFG_MEMMAP_LKW_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_HPRTD      (CIP_ASSEMBLYCNFG_OFFSET_HKW        + CFG_MEMMAP_HKW_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_STLP       (CIP_ASSEMBLYCNFG_OFFSET_HPRTD      + CFG_MEMMAP_HPRTD_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_STLTD      (CIP_ASSEMBLYCNFG_OFFSET_STLP       + CFG_MEMMAP_STLP_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_STLID      (CIP_ASSEMBLYCNFG_OFFSET_STLTD      + CFG_MEMMAP_STLTD_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_STLID      (CIP_ASSEMBLYCNFG_OFFSET_STLTD      + CFG_MEMMAP_STLTD_SIZE)
//#define CIP_ASSEMBLYCNFG_OFFSET_ENDIS      (CIP_ASSEMBLYCNFG_OFFSET_STLID      + CFG_MEMMAP_STLID_SIZE)
#define CIP_ASSEMBLYCNFG_OFFSET_CNFG       (CIP_ASSEMBLYCNFG_OFFSET_STLID      + CFG_MEMMAP_STLID_SIZE)
//#define CIP_ASSEMBLYCNFG_OFFSET_NAME       (CIP_ASSEMBLYCNFG_OFFSET_CNFG       + CFG_MEMMAP_CNFG_SIZE)
