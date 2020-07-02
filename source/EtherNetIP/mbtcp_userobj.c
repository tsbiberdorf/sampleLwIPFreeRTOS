/*
 *            Copyright (c) 2002-2009 by Real Time Automation, Inc.
 *
 *  This software is copyrighted by and is the sole property of
 *  Real Time Automation Inc. (RTA).  All rights, title, ownership, 
 *  or other interests in the software remain the property of RTA.  
 *  This software may only be used in accordance with the corresponding
 *  license agreement.  Any unauthorized use, duplication, transmission,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MAY NOT be removed or modified without prior
 *  written consent of RTA.
 *
 *  RTA reserves the right to modify this software without notice.
 *
 *  Real Time Automation
 *  150 S. Sunny Slope Road             USA 262.439.4999
 *  Suite 130                           http://www.rtaautomation.com
 *  Brookfield, WI 53005                software@rtaautomation.com
 *
 *************************************************************************
 *
 *     Module Name: mbtcp_userobj.c
 *         Version: 1.03
 *    Version Date: 05/05/2009
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains register and coil data functions.
 *
 */

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "inc/mbtcp_rta_system.h"
#include "inc/mbtcp_system.h"
#include "Memory/CommonComm.h"

#include "NGRMRelay.h"

/* ---------------------------- */
/* LOCAL STRUCTURE DEFINITIONS  */
/* ---------------------------- */

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */
//uint16 input_registers[4][10];  // define four areas of 10 registers
//uint16 output_registers[4][10]; // define four areas of 10 registers
//
//uint8 input_coils[4][8];  // define four areas of 64 coils (8 coils per byte)
//uint8 output_coils[4][8]; // define four areas of 64 coils (8 coils per byte)

/**
 * @note due to limited memory available, all Modbus register reads will
 * share the same memory buffer, ConfigurationRegisters, RealTimeRegisters
 * and FaultRegisters.
 */
#define CONF_REG_START_ADDR (CFG_MEMMAP_FW_REV_ADDR)
#define CONF_REG_SIZE ((CFG_MEMMAP_TCRP_ADDR+CFG_MEMMAP_TCRP_SIZE))
uint16_t gModBusRegisters[CONF_REG_SIZE];

#define RT_REG_START_ADDR (CFG_MEMMAP_RTREG_ADDR)
#define RT_REG_SIZE ((CFG_MEMMAP_RTREG_END-CFG_MEMMAP_RTREG_ADDR))

#define FAULT_REG_START_ADDR (CFG_MEMMAP_FRI_ADDR)
#define FAULT_REG_SIZE ((CFG_MEMMAP_FAULRREG_END-CFG_MEMMAP_FRI_ADDR))

#define SETIP_REG_START_ADDR (CFG_MEMMAP_SETIP_ADDR)
#define SETIP_REG_SIZE ((CFG_MEMMAP_SETIP_END-CFG_MEMMAP_SETIP_ADDR))

#define RTCTIME_REG_START_ADDR (CFG_MEMMAP_RTCTIME_ADDR)
#define RTCTIME_REG_SIZE ((CFG_MEMMAP_RTCTIME_END-CFG_MEMMAP_RTCTIME_ADDR))

/* ---------------------------- */
/* LOCAL FUNCTIONS              */
/* ---------------------------- */

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */
#define MBTYPE_INPUT_REG    0x01
#define MBTYPE_HOLD_REG     0x02
typedef struct 
{
    uint8   mb_type;
    uint16  start_addr;
    uint16  length;
    uint16  *data_ptr;    
}MB_USER_REG_TABLE;

enum ModBusMemoryRange_e
{
	eMBMR_ConfigurationReg,
	eMBMR_RealTimeReg,
	eMBMR_FaultReg,
	eMBMR_SetIPReg,
	eMBMR_SetRTCReg,
	eMBMR_ManufacureTestReg,
};

//@todo update the reg table to reflect needed items instead of MP8000 items
const MB_USER_REG_TABLE MB_Reg_Lookup[] = 
{
    {(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), (uint16)CONF_REG_START_ADDR, (uint16)CONF_REG_SIZE, gModBusRegisters},
    {(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), (uint16)RT_REG_START_ADDR, (uint16)RT_REG_SIZE, gModBusRegisters},
    {(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), (uint16)FAULT_REG_START_ADDR, (uint16)FAULT_REG_SIZE, gModBusRegisters},
    {(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), (uint16)SETIP_REG_START_ADDR, (uint16)SETIP_REG_SIZE, gModBusRegisters},
    {(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), (uint16)RTCTIME_REG_START_ADDR, (uint16)RTCTIME_REG_SIZE, gModBusRegisters},
	{(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), (uint16)MFT_MEMMAP_START_ADDR, (uint16)MFT_MEMMAP_SIZE, gModBusRegisters},
};

//const MB_USER_REG_TABLE MB_Reg_Lookup[] =
//{
//    {(MBTYPE_INPUT_REG), 100,   10,   &input_registers[1][0]},
//    {(MBTYPE_INPUT_REG), 200,   10,   &input_registers[1][0]},
//    {(MBTYPE_INPUT_REG), 300,   10,   &input_registers[2][0]},
//    {(MBTYPE_INPUT_REG), 400,   10,   &input_registers[3][0]},
//
//    {(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), 1100,   10,  &output_registers[0][0]}, // overlay to Input registers
//    {(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), 1200,   10,  &output_registers[1][0]}, // overlay to Input registers
//    {(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), 1300,   10,  &output_registers[2][0]}, // overlay to Input registers
//    {(MBTYPE_INPUT_REG|MBTYPE_HOLD_REG), 1400,   10,  &output_registers[3][0]}, // overlay to Input registers
//};
#define MBREG_LOOKUP_SIZE   (sizeof(MB_Reg_Lookup)/sizeof(MB_USER_REG_TABLE))

#if FC01_RDCOILSTAT
#define MBTYPE_INPUT_COIL   0x04
#define MBTYPE_OUTPUT_COIL  0x08
typedef struct 
{
    uint8   mb_type;
    uint16  start_addr;
    uint16  length;
    uint8   *data_ptr;    
}MB_USER_COIL_TABLE;

const MB_USER_COIL_TABLE MB_Coil_Lookup[] = 
{
    {(MBTYPE_INPUT_COIL), 100,   64,   &input_coils[0][0]},
    {(MBTYPE_INPUT_COIL), 200,   64,   &input_coils[1][0]},
    {(MBTYPE_INPUT_COIL), 300,   64,   &input_coils[2][0]},
    {(MBTYPE_INPUT_COIL), 400,   64,   &input_coils[3][0]},

    {(MBTYPE_INPUT_COIL|MBTYPE_OUTPUT_COIL), 1100,   64,  &output_coils[0][0]}, // overlay to Input coils
    {(MBTYPE_INPUT_COIL|MBTYPE_OUTPUT_COIL), 1200,   64,  &output_coils[1][0]}, // overlay to Input coils
    {(MBTYPE_INPUT_COIL|MBTYPE_OUTPUT_COIL), 1300,   64,  &output_coils[2][0]}, // overlay to Input coils
    {(MBTYPE_INPUT_COIL|MBTYPE_OUTPUT_COIL), 1400,   64,  &output_coils[3][0]}, // overlay to Input coils
};
#define MBCOIL_LOOKUP_SIZE   (sizeof(MB_Coil_Lookup)/sizeof(MB_USER_COIL_TABLE))
#endif // #if FC01_RDCOILSTAT

/**/
/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
/* ====================================================================
Function:     mbtcp_userobj_init
Parameters:   N/A
Returns:      N/A

This function initialize all user object model variables.
======================================================================= */
void mbtcp_userobj_init (void)
{
	memset(gModBusRegisters, 0, sizeof(gModBusRegisters));
#if FC01_RDCOILSTAT
    memset(input_registers, 0, sizeof(input_registers));
    memset(output_registers, 0, sizeof(output_registers));
    memset(input_coils, 0, sizeof(input_coils));
    memset(output_coils, 0, sizeof(output_coils));
#endif
}

#if FC01_RDCOILSTAT
/* ====================================================================
Function:     mbtcp_userobj_getInputCoilPtr
Parameters:   requested size (in bits)
Returns:      pointer to Input Coil Data Table
              (NULL on error)

This function verifies enough data is allocated, then returns the 
pointer to the Input Coil data table based on the starting address. 
We also need to know the bit position within the data byte (passed
by reference).
======================================================================= */
uint8 *mbtcp_userobj_getInputCoilPtr (uint16 start_addr, uint16 size_in_bits, uint8 * bit_pos)
{
    uint16  ix;
    uint16  i;
    uint16  table_ix;

    // 0 data requested, return an error
    if((size_in_bits == 0) || (!bit_pos))
        return(NULL);

    // look to see if this data is in our supported range
    for(table_ix = 0xFFFF, i=0; i<MBCOIL_LOOKUP_SIZE && table_ix == 0xFFFF; i++)
    {
        /* 
            Make sure
                1. starting address is in range
                2. complete request is in range
                3. mb type is valid
        */
        if( (start_addr >= MB_Coil_Lookup[i].start_addr) &&
            ((start_addr+size_in_bits-1 ) <= (MB_Coil_Lookup[i].start_addr+MB_Coil_Lookup[i].length-1)) &&
            (MB_Coil_Lookup[i].mb_type & MBTYPE_INPUT_COIL))
        {
            table_ix = i;
        }
    }

    // not found, return an error
    if(table_ix == 0xFFFF)
    {
        return(NULL);
    }

    // adjust the index
    ix = (uint16)((start_addr-MB_Coil_Lookup[table_ix].start_addr)/8);

    // find the bit position
    *bit_pos = (uint8)((start_addr-MB_Coil_Lookup[table_ix].start_addr)%8);

    // success, return the pointer to the input coils
    return(&MB_Coil_Lookup[table_ix].data_ptr[ix]);
}
#endif

#if FC01_RDCOILSTAT
/* ====================================================================
Function:     mbtcp_userobj_getOutputCoilPtr
Parameters:   requested size (in bits)
Returns:      pointer to Output Coil Data Table
              (NULL on error)

This function verifies enough data is allocated, then returns the 
pointer to the Output Coil data table based on the starting address. 
We also need to know the bit position within the data byte (passed
by reference).
======================================================================= */
uint8 *mbtcp_userobj_getOutputCoilPtr (uint16 start_addr, uint16 size_in_bits, uint8 * bit_pos)
{
    uint16  ix;
    uint16  i;
    uint16  table_ix;

    // 0 data requested, return an error
    if((size_in_bits == 0) || (!bit_pos))
        return(NULL);

    // look to see if this data is in our supported range
    for(table_ix = 0xFFFF, i=0; i<MBCOIL_LOOKUP_SIZE && table_ix == 0xFFFF; i++)
    {
        /* 
            Make sure
                1. starting address is in range
                2. complete request is in range
                3. mb type is valid
        */
        if( (start_addr >= MB_Coil_Lookup[i].start_addr) &&
            ((start_addr+size_in_bits-1 ) <= (MB_Coil_Lookup[i].start_addr+MB_Coil_Lookup[i].length-1)) &&
            (MB_Coil_Lookup[i].mb_type & MBTYPE_OUTPUT_COIL))
        {
            table_ix = i;
        }
    }

    // not found, return an error
    if(table_ix == 0xFFFF)
    {
        return(NULL);
    }

    // adjust the index
    ix = (uint16)((start_addr-MB_Coil_Lookup[table_ix].start_addr)/8);

    // find the bit position
    *bit_pos = (uint8)((start_addr-MB_Coil_Lookup[table_ix].start_addr)%8);

    // success, return the pointer to the input coils
    return(&MB_Coil_Lookup[table_ix].data_ptr[ix]);
}
#endif

/* ====================================================================
Function:     mbtcp_userobj_getInputRegPtr
Parameters:   requested size (in words)
Returns:      pointer to Input Register Data Table
              (NULL on error)

This function verifies enough data is allocated, then returns the 
pointer to the Input Register data table based on the starting address. 
======================================================================= */
uint16 *mbtcp_userobj_getInputRegPtr (uint16 start_addr, uint16 size_in_words)
{
    uint16  ix;
    uint16  i;
    uint16  table_ix;

    // 0 data requested, return an error
    if(size_in_words == 0)
        return(NULL);

    mbtcp_user_dbprint2("%d getInputReg starting at %x.\n",size_in_words, start_addr);

    // look to see if this data is in our supported range
    for(table_ix = 0xFFFF, i=0; i<MBREG_LOOKUP_SIZE && table_ix == 0xFFFF; i++)
    {
        /* 
            Make sure
                1. starting address is in range
                2. complete request is in range
                3. mb type is valid
        */
        if( (start_addr >= MB_Reg_Lookup[i].start_addr) &&
            ((start_addr+size_in_words-1 ) <= (MB_Reg_Lookup[i].start_addr+MB_Reg_Lookup[i].length-1)) &&
            (MB_Reg_Lookup[i].mb_type & MBTYPE_INPUT_REG))
        {
            table_ix = i;
        }
    }

    // not found, return an error
    if(table_ix == 0xFFFF)
        return(NULL);

    /*
     * populate buffer with memory requested.
     */
    switch(table_ix)
    {
    case eMBMR_ManufacureTestReg:
		MemMapRead(MFT_MEMMAP_START_ADDR, MFT_MEMMAP_SIZE, gModBusRegisters);
    	break;
    case eMBMR_ConfigurationReg:
		MemMapRead(CONF_REG_START_ADDR, CONF_REG_SIZE, gModBusRegisters);
	    gModBusRegisters[0] = (MAJOR_VERSION*0x100) | MINOR_VERSION;
	    gModBusRegisters[1] = (VERSION_VERSION * 0x100) | REVISION_VERSION;

    	break;
    case eMBMR_RealTimeReg:
		MemMapRead(RT_REG_START_ADDR, RT_REG_SIZE, gModBusRegisters);
    	break;
    case eMBMR_FaultReg:
		MemMapRead(FAULT_REG_START_ADDR, FAULT_REG_SIZE, gModBusRegisters);
    	break;
    case eMBMR_SetIPReg:
		MemMapRead(SETIP_REG_START_ADDR, SETIP_REG_SIZE, gModBusRegisters);
    	break;
    default:
    	memset(gModBusRegisters, 0, sizeof(gModBusRegisters));
    }

    // adjust the index
    ix = (uint16)(start_addr-MB_Reg_Lookup[table_ix].start_addr);

    // success, return the pointer to the input coils
    return(&MB_Reg_Lookup[table_ix].data_ptr[ix]);
}

/* ====================================================================
Function:     mbtcp_userobj_getOutputRegPtr
Parameters:   requested size (in words)
Returns:      pointer to Output Register Data Table
              (NULL on error)

This function verifies enough data is allocated, then returns the 
pointer to the Output Register data table based on the starting address. 
======================================================================= */
uint16 *mbtcp_userobj_getOutputRegPtr (uint16 start_addr, uint16 size_in_words)
{
    uint16  ix;
    uint16  i;
    uint16  table_ix;

    mbtcp_user_dbprint2("%d getOutputReg starting at %x.\n",size_in_words, start_addr);

    // 0 data requested, return an error
    if(size_in_words == 0)
        return(NULL);

    // look to see if this data is in our supported range
    for(table_ix = 0xFFFF, i=0; i<MBREG_LOOKUP_SIZE && table_ix == 0xFFFF; i++)
    {
        /* 
            Make sure
                1. starting address is in range
                2. complete request is in range
                3. mb type is valid
        */
        if( (start_addr >= MB_Reg_Lookup[i].start_addr) &&
            ((start_addr+size_in_words-1 ) <= (MB_Reg_Lookup[i].start_addr+MB_Reg_Lookup[i].length-1)) &&
            (MB_Reg_Lookup[i].mb_type & MBTYPE_HOLD_REG))
        {
            table_ix = i;
        }
    }

    // not found, return an error
    if(table_ix == 0xFFFF)
        return(NULL);

    /*
     * populate buffer with memory requested.
     */
    switch(table_ix)
    {
    case eMBMR_ManufacureTestReg:
		MemMapRead(MFT_MEMMAP_START_ADDR, MFT_MEMMAP_SIZE, gModBusRegisters);
    	break;
    case eMBMR_ConfigurationReg:
    	//@todo fix why this is the only way that we get proper response back on a value read manufacture test will be using these regs
		MemMapRead(CONF_REG_START_ADDR, MFT_MEMMAP_SIZE, gModBusRegisters);
//	    gModBusRegisters[0] = (MAJOR_VERSION*0x100) | MINOR_VERSION;
//	    gModBusRegisters[1] = (VERSION_VERSION * 0x100) | REVISION_VERSION;

    	break;
    case eMBMR_RealTimeReg:
		MemMapRead(RT_REG_START_ADDR, RT_REG_SIZE, gModBusRegisters);
    	//@todo figure out why writing to address 600 triggers this instead of the eMBMR_ManufactureTestReg
    	break;
    case eMBMR_FaultReg:
		MemMapRead(FAULT_REG_START_ADDR, FAULT_REG_SIZE, gModBusRegisters);
    	break;
    case eMBMR_SetIPReg:
		MemMapRead(SETIP_REG_START_ADDR, SETIP_REG_SIZE, gModBusRegisters);
    	break;
    default:
    	memset(gModBusRegisters, 0, sizeof(gModBusRegisters));
    }

    // adjust the index
    ix = (uint16)(start_addr-MB_Reg_Lookup[table_ix].start_addr);

    // success, return the pointer to the read data
    return(&MB_Reg_Lookup[table_ix].data_ptr[ix]);
}

/* ====================================================================
Function:     mbtcp_userobj_coilsWritten
Parameters:   starting address (1-based)
              length in bits
Returns:      N/A

This function is called when coil data is written to the output coil
table.  The address and length of the coil data that was written is 
passed.
======================================================================= */
void mbtcp_userobj_coilsWritten (uint16 start_addr, uint16 length_in_bits)
{
    mbtcp_user_dbprint2("%d coils written starting at %x.\n",length_in_bits, start_addr);
}

/* ====================================================================
Function:     mbtcp_userobj_registersWritten
Parameters:   starting address (1-based)
              length in words
Returns:      N/A

This function is called when register data is written to the output 
register table.  The address and length of the register data that was 
written is passed.
======================================================================= */
void mbtcp_userobj_registersWritten (uint16 start_addr, uint16 length_in_words)
{
    uint16  i;
    uint16  table_ix;
    uint16 modBusRegisterBufferOffset;
    uint16 memMapAddress=0,memMapReadSize=0;

    mbtcp_user_dbprint2("%d registersWritten %x.\n",length_in_words, start_addr);

    /*
     * need to make a write to the memory map
     */
    for(table_ix = 0xFFFF, i=0; i<MBREG_LOOKUP_SIZE && table_ix == 0xFFFF; i++)
    {
        /*
            Make sure
                1. starting address is in range
                2. complete request is in range
                3. mb type is valid
        */
        if( (start_addr >= MB_Reg_Lookup[i].start_addr) &&
            ((start_addr+length_in_words-1 ) <= (MB_Reg_Lookup[i].start_addr+MB_Reg_Lookup[i].length-1)) &&
            (MB_Reg_Lookup[i].mb_type & MBTYPE_HOLD_REG))
        {
            table_ix = i;
        }
    }

    switch(table_ix)
    {
    case eMBMR_ManufacureTestReg:
    	modBusRegisterBufferOffset = MFT_MEMMAP_START_ADDR;
    	memMapReadSize = length_in_words*2;
    	memMapAddress = start_addr;
    	break;

    case eMBMR_ConfigurationReg:
    	modBusRegisterBufferOffset = 0;
//    	memMapAddress = start_addr;
        /**
         * Need to convert the register address to the physical memory map address
         *
         */
    	if(start_addr != 0x76)
    	{
    		memMapAddress = start_addr*2;
    	}
    	else
    	{
    		memMapAddress = start_addr;
    	}
    	memMapReadSize = length_in_words*2;
    	break;
    case eMBMR_RealTimeReg:
    	modBusRegisterBufferOffset = RT_REG_START_ADDR;
    	memMapReadSize = length_in_words*2;
    	memMapAddress = start_addr;
    	break;
    case eMBMR_FaultReg:
    	modBusRegisterBufferOffset = FAULT_REG_START_ADDR;
    	memMapReadSize = length_in_words*2;
    	memMapAddress = start_addr;
    	break;
    case eMBMR_SetIPReg:
    	modBusRegisterBufferOffset = SETIP_REG_START_ADDR;
    	memMapReadSize = length_in_words*2;
    	memMapAddress = start_addr;
    	break;
    case eMBMR_SetRTCReg:
    	modBusRegisterBufferOffset = RTCTIME_REG_START_ADDR;
    	memMapReadSize = length_in_words*2;
    	memMapAddress = start_addr;
    	/*
    	 * protection check to prevent changes to the time stamp from
    	 * an error write.
    	 */
#define RTC_UNLOCK_KEY (0x5A5A) /**< unlock key that must be set to allow the RTC time to be accepted */
#define RTC_UPDATE_SIZE (4) /**< override size of the RTC time stamp to use */
    	if( *(gModBusRegisters + (start_addr - modBusRegisterBufferOffset + 2)) == RTC_UNLOCK_KEY )
    	{
        	memMapReadSize = RTC_UPDATE_SIZE;
    	}
    	break;
    default:
    	modBusRegisterBufferOffset = 0;
    }

    MemMapWrite(memMapAddress, memMapReadSize, gModBusRegisters + (start_addr - modBusRegisterBufferOffset), 1);
}

/* ====================================================================
Function:     mbtcp_userobj_getExceptionStatus
Parameters:   N/A
Returns:      8-bit exception status

This function returns the exception status for the device.
======================================================================= */
uint8 mbtcp_userobj_getExceptionStatus (void)
{
   static uint8 exc_stat = 1;
   exc_stat++;
   return(exc_stat);
}

/**/
/* ******************************************************************** */
/*                      LOCAL FUNCTIONS                                 */
/* ******************************************************************** */

/* *********** */
/* END OF FILE */
/* *********** */

