/**
 *@file cliCmdsProductionTest.c
 *
 * @details Full implementation of API to interact with Production Test operations through CLI
 * @see cliCmdsProductionTest.h for public implementation
 * @see ProductionTest.h for internal functions this interacts with
 * @ingroup groupCliCmds
 */
#include <stdint.h>
#include <string.h>
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "UartTasks/DebugTask.h"
#include "LED/LEDOperations.h"
#include "Peripheral/GPIO/GpiocOperations.h"
#include "Peripheral/RS485/RS485Sensor.h"
#include "ProductionTest/ProductionTest.h"
#include "Cli.h"
#include "cliCmdsProductionTest.h"
  /*****************************************************************************
   * Definitions ===============================================================
   ****************************************************************************/
  /*****************************************************************************
   * Prototypes/externs ========================================================
   ****************************************************************************/
static int32_t ptHelpCmd(char *Param);
static int32_t ptRelayCli(char *Param);
static int32_t pt485Cli(char *Param);
static int32_t ptLedCli(char *Param);
static int32_t pt3VCli(char *Param);
static int32_t ptDigitalInCli(char *Param);

/*****************************************************************************
 * Variables =================================================================
 ****************************************************************************/
/**
 * @brief help menu C-String
 */
char const *const ptCmdText[] = {
		"pt help commands:\r\n",
		"-led      :Production Test measures LED voltage \r\n",
		"-rly [0/1]:Production Test release/energize all relays \r\n",
		"-485 [0/1]:0=Sensor To HMI 1=HMI to Sensor \r\n",
		"-3v       :Production Test verify adc reading of 3.3v",
		"-di       :Production Test verify Digital inputs",
		"\0"
};

/**
 * @brief CLI options supported with the command: pt  (Production Test)
 */
s_cliCommandOptions_t ptOptions[]= {
		{"-rly",4, ptRelayCli},
		{"-led",4,ptLedCli},
		{"-485",4,pt485Cli},
		{"-3v", 3,pt3VCli},
		{"-di", 3,ptDigitalInCli},

		{"-h",2,ptHelpCmd},{"-?",2,ptHelpCmd},{NULL,0,ptHelpCmd}
};

static char const *const tl_PtMainHelpText = "pt - production test Commands\r\n";
static s_cliCommands_t tl_PtMainCliCmd = {"pt", ptOptions};
/*****************************************************************************
 * Private Methods ===========================================================
 ****************************************************************************/

/**
 * @brief trigger production test of digital inputs
 *
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ptDigitalInCli(char *Param)
{
	executePTCommand(eVerifyDigitalInputsCmd);
	return 0;
}

/**
 * @brief trigger production test of 3.3v line
 *
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t pt3VCli(char *Param)
{
	executePTCommand(eVerify3VCmd);
	return 0;
}

/**
 * @brief trigger production loopback test of 485 bus
 * @details if passed value of 0 it tests message sending from sensor to HMI,
 * otherwise it tests a message sending message from HMI to Sensor
 *
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t pt485Cli(char *Param)
{
	uint32_t value;
	ParseDecimal((uint8_t *)Param,&value);
	if(value)
	{
		executePTCommand(eVerifyRS485CmdHMI);
	}
	else
	{
		executePTCommand(eVerifyRS485CmdSensor);
	}

	return 0;
}
/**
 * @brief trigger production test led code
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ptLedCli(char *Param)
{
	PRINTF("Production Test LED ");
	executePTCommand(eVerifyLEDsCmd);
	return 0;
}

/**
 * @brief trigger production test of relays
 * @details if parameter passed is 0 turn off all relays, else turn on all relays
 *
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ptRelayCli(char *Param)
{
	uint32_t value;
	PRINTF("Production Test relay ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			executePTCommand(eEngergizeAllRelaysCmd);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			executePTCommand(eReleaseAllRelaysCmd);
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
static int32_t ptHelpCmd(char *Param)
{
	uint16_t helpIdx=0;

	while(ptCmdText[helpIdx][0])
	{
		DebugTaskWrite(ptCmdText[helpIdx],strlen(ptCmdText[helpIdx]));
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
const char* ProductionTestCliInit(s_cliCommands_t *CmdOpt)
{
	*CmdOpt = tl_PtMainCliCmd;
	return tl_PtMainHelpText;
}
