/**
 * @file cliCmdsVoltage.c
 * @details Implementation of API to interact with voltages through the CLI
 * @see cliCmdsVoltage.h for public implementation
 * @ingroup groupCliCmds
 */

#include <stdint.h>
#include <string.h>
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "UartTasks/DebugTask.h"

#include "source/LED/LEDOperations.h"
#include "Cli.h"
#include "cliCmdsRs485.h"

  /*****************************************************************************
   * Definitions ===============================================================
   ****************************************************************************/
  /*****************************************************************************
   * Prototypes/externs ========================================================
   ****************************************************************************/
static int32_t voltHelpCmd(char *Param);
static int32_t volt33(char *Param);
static int32_t volt24(char *Param);

/*****************************************************************************
 * Variables =================================================================
 ****************************************************************************/
/**
 * @todo update for NGR relay code
 * @brief command help C-String
 */
char const *const voltCmdText[] = {
		"volt help commands:\r\n",
		"-24 read 24V adc value\r\n",
		"-33 read 3.3V adc value\r\n",
		"\0"
};

/**
 * CLI options supported with the command: volt
 *
 * @todo update for ngr relay code
 */
s_cliCommandOptions_t voltOptions[]= {
		{"-24",3,volt24},
		{"-33",3,volt33},
		{"-h",2,voltHelpCmd},{"-?",2,voltHelpCmd},{NULL,0,voltHelpCmd}
};

static char const *const tl_VoltageMainHelpText = "volt Commands\r\n";
static s_cliCommands_t tl_VoltageMainCliCmd = {"volt", voltOptions};

/*****************************************************************************
 * Private Methods ===========================================================
 ****************************************************************************/
/**
 * @brief volt -24
 * @details get reading of 24v line
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t volt24(char *Param)
{
	PRINTF("volt 24V ");
	xTaskNotify(GetLedTaskHandle(),V24_TEST_ON,eSetBits);
	PRINTF("\r\n");

	return 0;
}

/**
 * @brief volt -33
 * @details get reading of 3.3v
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t volt33(char *Param)
{
	PRINTF("volt 3.3V ");
	xTaskNotify(GetLedTaskHandle(),V33_TEST_ON,eSetBits);
	PRINTF("\r\n");

	return 0;
}

/**
 * @brief print voltage help menu
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t voltHelpCmd(char *Param)
{
	uint16_t helpIdx=0;

	while(voltCmdText[helpIdx][0])
	{
		DebugTaskWrite(voltCmdText[helpIdx],strlen(voltCmdText[helpIdx]));
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
const char* VoltageCliInit(s_cliCommands_t *CmdOpt)
{
	*CmdOpt = tl_VoltageMainCliCmd;
	return tl_VoltageMainHelpText;
}
