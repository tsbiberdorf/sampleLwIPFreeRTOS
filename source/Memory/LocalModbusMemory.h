/*
 *@file LocalModbusMemory.h
 *
 *@brief Public API to interact with local modbus memory buffer
 *@see LocalModbusMemory.h for full implementation
 */

#ifndef MEMORY_LOCALMODBUSMEMORY_H_
#define MEMORY_LOCALMODBUSMEMORY_H_

#define MANUFACTURE_TEST_RESERVED 12

/**
 * @brief reference index for a register in the local modbus memory buffer
 * @details This enum hold indices related to where the particular ptr that
 * holds the information regarding command options is stored.
 *
 * From the tester's point of view these registers are read through ModbusTCP
 * by requesting values starting in register address 0x600.
 *
 * @see tl_PTResultBuffer to figure out what variable these indices are associated with
 */
typedef enum ProductTestResultRegs_e{
	ePtCommandReg    = 0,
	ePtStatusReg = 1,
	ePtResultReg1 = 2,
	ePtResultReg2 = 3,
	ePtResultReg3 = 4,
	ePtResultReg4 = 5,
	ePtResultReg5 = 6,
	ePtResultReg6 = 7,

} eProductTestResultRegs_t;

uint8_t updatePTReg(uint16_t index, uint16_t value);

#endif /* MEMORY_LOCALMODBUSMEMORY_H_ */
