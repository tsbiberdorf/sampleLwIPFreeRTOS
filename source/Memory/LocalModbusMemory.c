/*
 * @file LocalModbusMemory.c
 *
 * Full code implementation of the API that manages the memory of the local modbus
 * memory buffer.
 * @see LocalModbusMemory.h for public implementation
 *@todo protect the memory from issue of conflicting access
 *@todo change memory from statically declared to memalloc()
 */

#include "CommonComm.h"
#include "LocalModbusMemory.h"
/*******************************************************************************
* Variables
******************************************************************************/
//static uint16_t tl_MFTResultValue;
static uint16_t tl_PTResultBuffer[MFT_MEMMAP_SIZE];


/******************************************************************************
 * public methods (both through extern and #include header file)
 *****************************************************************************/

/**
 * @brief Initializes the local storage buffer for the production test results
 * @return pointer to the value stored within the local modbus memory buffer
 */
uint16_t* MemInitLocalPTBuffer()
{
	//@todo update this with the appropriate memory index handling
	tl_PTResultBuffer[0] = 256; //temp value to verify memory is being read properly
	tl_PTResultBuffer[1] = 257;
	tl_PTResultBuffer[2] = 258;
	return &tl_PTResultBuffer[0];
	//	tl_MFTResultValue = 69;
//	return &tl_MFTResultValue;
}

/**
 * @brief call that allows the unit to update a particular modbus memory buffer register.
 * @details this buffer is then read/written to when the appropriate requests are sent
 * via modbus.
 *
 * use of the specifically created enum is the preferred method to use when calling this method
 *@see eProductTestResultRegs_t to learn details about how this buffer is indexed through
 *@param index the register to write to
 *@param value value to be written into the modbus buffer register
 */
uint8_t updatePTReg(uint16_t index, uint16_t value)
{
	if(index > MFT_MEMMAP_SIZE)
	{
		return 1; //could not update invalid index
	}

	tl_PTResultBuffer[index] = value;
	return 0; //success
}

/**
 * @brief getter for a specific register of memory within the production test portion
 * @details this shouldn't need to be used apart from during debug. As such all references
 * to this should be done by calling it via extern.
 */
uint16_t readSingleMbReg(int index)
{
	return tl_PTResultBuffer[index];
}
