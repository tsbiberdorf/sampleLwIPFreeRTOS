/**
 * @file RS485Common.h
 *
 * @brief Public API for the common functions between the two HMI Directions
 * @see RS485Common.c for full implementation
 */

#ifndef RS485_RS485COMMON_H_
#define RS485_RS485COMMON_H_
#include <stdint.h>

/*******************************************************************************
 * Enum/Struct  ===============================================================
 ******************************************************************************/

/**
 * @brief types of command oeprations related to RS485 Operations
 */
typedef enum _RS485Cmds_e
{
	e485CmdLoopback = 1,
	e485CmdLoopbackAck,
	e485CmdRead,
	e485CmdReadAck,
	e485CmdWrite,
	e485CmdWriteAck,
}e485Cmds_t;

/**
 * @brief parsing RS485 commands
 */
typedef enum _RS485State_e
{
	e485Idle = 0, /**< no command in state of processing */
	e485Cmd, /**< port command has been received */
	e485Size0, /**< receiving the size of the command */
	e485Size1, /**< receiving the size of the command */
	e485Data, /**< receiving the data of the command */
	e485CRC0, /**< receiving the CRC of the command */
	e485CRC1, /**< receiving the CRC of the command */
}e485State_t;

/**
 * @details Loopback test format sequence.
 */
typedef enum _RS485LoopBackTest_e
{
	e485LBNotStarted = 0, /**< no loopback command has been sent */
	e485LBSent, /**< a loopback command has been sent */
	e485LBAck, /**< a loopback ACK has been recieved but not verified */
	e485LBVerified, /**< the loopback ACK has been verified  */
}eRS485LoopBackTest_t;

/**
 * protocol format for RS485 commands
 */
typedef struct _RS485Cmd_s
{
	e485State_t State;
	uint8_t PortId;
	e485Cmds_t Cmd;
	union _Size_u
	{
		uint8_t sizeInBytes[2];
		uint16_t size;
	}uSize_t;
	union _CRC_u
	{
		uint8_t crcInBytes[2];
		uint16_t CRC;
	}uCRC_t;
	uint8_t *Payload;
}sRS485Cmd_t;

/*******************************************************************************
 * Definitions   ==============================================================
 ******************************************************************************/
#define HMI_RS485_ID (1)
#define SENSOR_RS485_ID (2)

#define RS485_TIMEOUT_CNT (150) // ticks delay waiting for ACK
uint16_t VerifyCmdCRC(sRS485Cmd_t *verifyCRC);
void seggerPrintRS485(sRS485Cmd_t *printCmd);

#endif /* RS485_RS485COMMON_H_ */
