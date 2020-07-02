/**
 * @file cliCmdsModbus.c
 * @details Implementation of API logic to interact with the Modbus memory Cli commands
 * @see cliCmdsModbus.h for Public implementation
 * @ingroup groupCliCmds
 *
 */

#include <stdint.h>
#include <string.h>
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "../DebugTask.h"
#include "Memory/LocalModbusMemory.h"
#include "Cli.h"
#include "cliCmdsModbus.h"

  /*****************************************************************************
   * Definitions ===============================================================
   ****************************************************************************/
  /*****************************************************************************
   * Prototypes/externs ========================================================
   ****************************************************************************/
static int32_t mbHelpCmd(char *Param);
static int32_t printMbReg(char *Param);
static int32_t setMbReg(char *Param);

extern uint16_t readSingleMbReg(int index); //this is used for printing only
/*****************************************************************************
 * Variables =================================================================
 ****************************************************************************/
/**
 * @brief Help menu C-String
 */
char const * const mbCmdTxt[] = {
		"modbus command help commands:\r\n",
		"-p A print value of production test section register A\r\n",
		"-s A B set register A of production test section to value B\r\n",
		"\0"
};

/**
 * @brief CLI options supported with the command: mb
 */
s_cliCommandOptions_t mbOptions[]= {
		{"-p", 2, printMbReg},
		{"-s", 2, setMbReg},

		{"-h",2,mbHelpCmd},{"-?",2,mbHelpCmd},
		{NULL,0,mbHelpCmd}
};
static char const * const tl_mbMainHelpText = "mb Commands\r\n";
static s_cliCommands_t tl_mbMainCliCmd = {"mb", mbOptions};
/*****************************************************************************
 * Private Methods ===========================================================
 ****************************************************************************/
/**
 * @brief command for printing a specific value in the stored production test values
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t printMbReg(char *Param)
{
	uint32_t index;

	char display_val[64];
	ParseDecimal((uint8_t *)Param,&index);

	//it is index -1 since the user is thinking in index start at 1 wheras the regs are stored where first index is 0
	index = index - 1;
	//@todo notify if improper format
		sprintf(display_val, "MFT Reg %d: %d", index+1, readSingleMbReg(index));
		DebugTaskWrite(display_val,strlen(display_val));

		return 0;
}

/**
 * @brief command for setting the value of an production test modbus register
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t setMbReg(char *Param)
{
	uint32_t value, index;
	uint8_t *nextParam;

	char display_val[64];
	nextParam = ParseDecimal((uint8_t *)Param,&index);
	if(nextParam!= NULL)
	{
		//@todo notify if improper format
		nextParam = ParseDecimal((uint8_t *)nextParam, &value);
		//it is index -1 since the user is thinking in index start at 1 wheras the regs are stored where first index is 0
		updatePTReg((index-1), value);
//		sprintf(display_val, "MFT Reg %d: %d", value, getMFTBufferValue(value));
	//	DebugTaskWrite(display_val,strlen(display_val));

	}else
	{
		sprintf(display_val,"Invalid format\r\n");
		DebugTaskWrite(display_val, strlen(display_val));
		return 1; //problem invalid format?
	}

	return 0;
}

/**
 * @brief print of usage for modbus cli commands
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t mbHelpCmd(char *Param)
{
	uint16_t helpIdx=0;

	while(mbCmdTxt[helpIdx][0])
	{
		DebugTaskWrite(mbCmdTxt[helpIdx],strlen(mbCmdTxt[helpIdx]));
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
const char* MbCliInit(s_cliCommands_t *CmdOpt)
{
	*CmdOpt = tl_mbMainCliCmd;
	return tl_mbMainHelpText;
}
