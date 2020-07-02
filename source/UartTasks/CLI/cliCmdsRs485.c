/**
 * @file cliCmdsRs485.c
 * @details Implementation of API for Interacting with the RS485 through CLI
 * @see cliCmdsRs485.h for public implementation
 * @todo update with how rs485 task now works and have Terry update function documentation
 * @ingroup groupCliCmds
 */
#include <stdint.h>
#include <string.h>
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "UartTasks/DebugTask.h"

#include "Peripheral/RS485/RS485HMI.h"
#include "Peripheral/RS485/RS485Sensor.h"
#include "Peripheral/RS485/RS485Common.h"
#include "Cli.h"
#include "cliCmdsRs485.h"

  /*****************************************************************************
   * Definitions ===============================================================
   ****************************************************************************/
  /*****************************************************************************
   * Prototypes/externs ========================================================
   ****************************************************************************/
static int32_t rs485SensorSelect(char *Param);
static int32_t rs485SensorTx(char *Param);
//static int32_t rs485SensorTest(char *Param);
static int32_t rs485HMISelect(char *Param);
static int32_t rs485HMITx(char *Param);
static int32_t rs485HelpCmd(char *Param);
static int32_t rs485SensorSend(char *Param);
static int32_t rs485HMISend(char *Param);
static int32_t rs485SensorLoopBack(char *Param);
static int32_t rs485HMILoopBack(char *Param);

/*****************************************************************************
 * Variables =================================================================
 ****************************************************************************/
/**
 * @brief help menu C-String
 */
char const * const rs485CmdText[] = {
		"RS485 help commands:\r\n",
		"-ssel [0/1] Sensor Tx/Rx select \r\n",
		"-strx [0/1] Sensor RS485 to tx or rx block of data\r\n",
		"-stx have sensor tx message to HMI\r\n",
		"-slb sensor to hmi loopback test",
		"-hsel [0/1] HMI Tx/Rx select \r\n",
		"-htrx [0/1] HMI RS485 to tx or rx block of data\r\n",
		"-htx have HMI tx message to Sensor\r\n",
		"-hlb hmi to sensor loopback test",
		"\0"
};

/**
 * CLI options supported with the command: rs485
 */
s_cliCommandOptions_t rs485Options[]= {
		{"-ssel",5,rs485SensorSelect},
		{"-strx",5,rs485SensorTx},
		{"-stx",4,rs485SensorSend},
		{"-slb",4,rs485SensorLoopBack},
		{"-hsel",5,rs485HMISelect},
		{"-htrx",5,rs485HMITx},
		{"-htx",4,rs485HMISend},
		{"-hlb",4,rs485HMILoopBack},

		{"-h",2,rs485HelpCmd},{"-?",2,rs485HelpCmd},{NULL,0,rs485HelpCmd}
};

static char const tl_rs485MainHelpText[] = "rs485 Commands\r\n";
static s_cliCommands_t tl_rs485MainCliCmd = {"rs485", rs485Options};

/*****************************************************************************
 * Private Methods ===========================================================
 ****************************************************************************/
/**
 * @details Change the Transmit/Receive bit for the Sensor RS485 port
 * @param[in] Paramater pointer, if zero set to receive, if non-zero to transmit
 * @return 0
 */
static int32_t rs485SensorSelect(char *Param)
{
	uint32_t value;
	PRINTF("Sensor RX485 ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("place in Tx Mode (write data)\r\n");
			xTaskNotify(GetSensorTaskHandle(),SENSOR_TX_MODE,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("place in Rx Mode (read data)\r\n");
			xTaskNotify(GetSensorTaskHandle(),SENSOR_RX_MODE,eSetBits);
			replyOK();
		}
	}
	else
	{
		PRINTF("\r\n");
	}
	return 0;
}

static int32_t rs485SensorTx(char *Param)
{
	uint32_t value;
	PRINTF("Sensor RX485 ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("read data\r\n");
			xTaskNotify(GetSensorTaskHandle(),SENSOR_RX_DATA,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("echo back data\r\n");
			xTaskNotify(GetSensorTaskHandle(),SENSOR_TX_DATA,eSetBits);
			replyOK();
		}
	}
	else
	{
		PRINTF("\r\n");
	}
	return 0;
}

/**
 * @detail Call TriggerHmiLoopbackCommand() API to trigger HMI to Sensor loop back
 * test in the same manner as the ModbusTCP command would
 *
 * @param Parm parameter passing into CLI command
 * @return 0
 */
static int32_t rs485HMILoopBack(char *Param)
{
	TriggerHmiLoopbackCommand();
	return 0;
}

/**
 * @detail Call TriggerHmiLoopbackCommand() API to trigger Sensor to HMI loop back
 * test in the same manner as the ModbusTCP command would
 *
 * @param Parm parameter passing into CLI command
 * @return 0
 */
static int32_t rs485SensorLoopBack(char *Param)
{
	TriggerSensorLoopbackCommand();
	return 0;
}

static int32_t rs485SensorSend(char *Param)
{
	sRS485Cmd_t sensorCmd;
	uint32_t value;
	PRINTF("Sensor RS485 Send Cmd");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value == 1)
		{
			sensorCmd.PortId = HMI_RS485_ID;
			sensorCmd.Cmd = value;
			sensorCmd.Payload = (uint8_t *)"test message";
			sensorCmd.uSize_t.size = strlen((char *)sensorCmd.Payload);
			SendSensorTaskCmd(&sensorCmd);
			replyOK();
		}
		if(value==2)
		{
			GetSensorLoopbackStatus();
			replyOK();
		}
	}
	PRINTF("\r\n");
	return 0;
}

/**
 * @brief create a RS485 command to send from the HMI port to the sensor port
 *
 * @detail this API can be expanded to allow for a larger variety of commands.  Currently
 * it is only structured to trigger a loopback command.
 *
 * @param Param
 * @return 0
 */
static int32_t rs485HMISend(char *Param)
{
	sRS485Cmd_t hmiCmd;
	uint32_t value;
	PRINTF("HMI RS485 Send Cmd");

	/*
	 * if needed more parsing can be added to allow for larger variety of commands
	 * to sent.
	 */
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value==1)
		{
			hmiCmd.PortId = SENSOR_RS485_ID;
			hmiCmd.Cmd = e485CmdLoopback;
			hmiCmd.Payload = (uint8_t *)"test message";
			hmiCmd.uSize_t.size = strlen((char *)hmiCmd.Payload);
			SendHmiTaskCmd(&hmiCmd);
			replyOK();
		}
		if(value==2)
		{
			GetHmiLoopbackStatus();
			replyOK();
		}
	}
	PRINTF("\r\n");
	return 0;
}


static int32_t rs485HMISelect(char *Param)
{
	uint32_t value;
	PRINTF("HMI RX485 ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("place in Tx Mode (write data)\r\n");
			xTaskNotify(GetHMITaskHandle(),HMI_TX_MODE,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("place in Rx Mode (read data)\r\n");
			xTaskNotify(GetHMITaskHandle(),HMI_RX_MODE,eSetBits);
			replyOK();
		}
	}
	else
	{
		PRINTF("\r\n");
	}
	return 0;
}

static int32_t rs485HMITx(char *Param)
{
	uint32_t value;
	PRINTF("HMI RX485 ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("read data\r\n");
			xTaskNotify(GetHMITaskHandle(),HMI_RX_DATA,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("echo back data\r\n");
			xTaskNotify(GetHMITaskHandle(),HMI_TX_DATA,eSetBits);
			replyOK();
		}
	}
	else
	{
		PRINTF("\r\n");
	}
	return 0;
}


/**
 * @brief display menu options
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t rs485HelpCmd(char *Param)
{
	uint16_t helpIdx=0;

	while(rs485CmdText[helpIdx][0])
	{
		DebugTaskWrite(rs485CmdText[helpIdx],strlen(rs485CmdText[helpIdx]));
		helpIdx++;
	}
	return 0;
}

/*****************************************************************************
 * Public Methods ============================================================
 ****************************************************************************/
/**
 * @brief initialize pointers to help menu and command execution.
 * @param CmdOpt pointer to the command option slot in main cli command array
 * @return C-String of title row for main menu's help display string
 */
const char* rs485CliInit(s_cliCommands_t *CmdOpt)
{
	*CmdOpt = tl_rs485MainCliCmd;
	return tl_rs485MainHelpText;
}
