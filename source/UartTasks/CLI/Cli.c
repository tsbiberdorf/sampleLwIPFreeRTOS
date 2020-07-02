/**
 * @file Cli.c
 * This is the api implementation for interacting with the cli.
 *
 *@note
 * Adding a new Cli Option:
 * First each new cli command will need its own file that handles it's specific
 * options. See any of the cliCmds*.c files for examples on how to do this.
 * Then to add it into the main list for the user to interact with, you have to
 *  increment the value CLI_NUM_CMDS and add the initialization into CliInit.
 *
 * @see Cli.h for public facing options
 * @ingroup groupCliCmds
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#include "FreeRTOS.h"
#include "task.h"
#include "../DebugTask.h"
#include "NGRMRelay.h"
#include "fsl_debug_console.h"


#include "Cli.h"
#include "cliCmdsIP.h"
#include "cliCmdsLed.h"
#include "cliCmdsGpio.h"
#include "cliCmdsRs485.h"
#include "cliCmdsVoltage.h"
#include "cliCmdsProductionTest.h"
#include "cliCmdsExternFlash.h"
#include "UartTasks/CLI/cliCmdsModbus.h"

/*******************************************************************************
 * Definitions ================================================================
 ******************************************************************************/
#define CLI_BUFFER_SIZE (256)

/*******************************************************************************
 * Prototypes/externs =========================================================
 ******************************************************************************/


/*******************************************************************************
 * Variables ==================================================================
 ******************************************************************************/

/**
 * @brief    reference index for command text
 * @details  This enum holds indices related to where the line in the main command help menu,
 * the details for a command are to be stored.
 *
 * @see tl_helpCmdTxt for which array these indices refer to
 */
typedef enum MainCliMenuTxt_e{
	eMenuHeaderTxt, /**< Menu header, should always be at index 0 for proper print displays */
	eLedTxt,        /**< Txt line for led commands */
	eVoltageTxt,
	eGpioTxt,
	eRs485Txt,
	eProductionTestTxt,
	eIpAddressTxt,
	eModbusTxt,        //@todo remove and merge with product or make it specific to modbus
	eExternFlashTxt,

	eMenuEndTxt,       /**< end of the menu is empty so this should always be the largest index */
} eMainCliMenuTxt_t;


/**
 * @brief reference index for actual command ptr
 * @details This enum hold indicies related to where the particular ptr that
 * holds the information regarding command options is stored.
 *
 * @see tl_UserCmds for which array these items refer to
 */
typedef enum MainCliMenuCmd_e{
	eHelpCmd, /**< This is the indicator for the options available from the main cli operation*/
	eLedCmd,
	eVoltageCmd,
	eGpioCmd,
	eRs485Cmd,
	eProductionTestCmd,
	eIpAddressCmd,
	eModbusCmd,
	eExternFlashCmd,

	eNullCmd, /**< this should always be the largest value in the main cli group */
}e_MainCliMenuCmd_t;



static uint8_t tl_cliData[CLI_BUFFER_SIZE];
static uint16_t tl_cliIdx = 0;

static s_cliCommands_t tl_UserCmds[eNullCmd + 2]; /**!< List of all the Cli Commands available ot the user */
static const char *tl_helpCmdTxt[eMenuEndTxt+2];  /**!< string of the main help menu text */
														//add 2 so that the help text holds the header line and ending tag and 1 for null instance

/*******************************************************************************
 * Private Methods ============================================================
 ******************************************************************************/

/**
 * LoopOptions will loop through the specified options that can be associated
 * with each command.  This list is specific for each command.
 *
 * @param UserCmdOptions listed options for the specific command
 * @param PtrOption pointer to data recieved over the CLI port
 * @return zero on success
 */
int32_t LoopOptions(s_cliCommands_t *UserCmdOptions,uint8_t *PtrOption)
{
#define MAX_OPTION_SIZE (9) // max size the option field can be
	char userOption[MAX_OPTION_SIZE];
	char *ptrUserOption;
	int8_t idx;
	int8_t optionSize=0;
	int32_t status = -1;
	int8_t userOptionSize=0;

	ptrUserOption = (char *)PtrOption;
	idx = 0;
	while( (ptrUserOption[idx] != ' ') && (ptrUserOption[idx] != 0))
	{
		userOption[idx] = ptrUserOption[idx];
		idx++;
		userOptionSize++;
		if(idx == MAX_OPTION_SIZE)
		{
			//SEGGER_RTT_printf(0,"unknown command\r\n");
			status = -1;
			goto exitMethod;
		}
		else
		{
			userOption[idx] = 0;
		}
	}
	for(idx=0; UserCmdOptions->option[idx].option != NULL;)
	{
		optionSize = UserCmdOptions->option[idx].optionSize; // str len of the option to compare against
//		userOptionSize = strlen(PtrOption); // str len of the option requested by the user
		if(optionSize == userOptionSize)
		{
			if( strncmp(userOption,UserCmdOptions->option[idx].option,optionSize) == 0)
			{
				/*
				 * we have a matching option and the pointer to where the parameter
				 * will be located.
				 */

				/*
				 * verify we have a method to call
				 */
				if( UserCmdOptions->option[idx].optionCallback != 0)
				{
					status = UserCmdOptions->option[idx].optionCallback((char *)(PtrOption+optionSize));
				}
			}
		}
		idx++;
	}

	/*
	 * for commands that have no options
	 */
	if( status < 0 )
	{
		/*
		 * verify we have a method to call
		 */
		if( UserCmdOptions->option[idx].optionCallback != 0)
		{
			status = UserCmdOptions->option[idx].optionCallback((char *)(PtrOption+optionSize));
		}
	}

	exitMethod:
	return status;
}

/**
 * If a command is detected, this API will determine if the command matches
 * a command listed in the tl_UserCmds array.
 *
 *  @param EnteredCmd pointer to the command received over the CLI port
 *  @return zero on success
 */
int32_t LoopCmds(uint8_t *EnteredCmd)
{
	int32_t status = -1;
	int32_t cmdStringLength;
	uint8_t *ptrOption = NULL;
	s_cliCommands_t *ptrUserCmds = &tl_UserCmds[0];
	while( ptrUserCmds->cmdString != NULL)
	{
		cmdStringLength = strlen(ptrUserCmds->cmdString);
		if( strncmp((char *)EnteredCmd,ptrUserCmds->cmdString,cmdStringLength) == 0)
		{
			if(*(EnteredCmd+cmdStringLength) == '+')
			{
				ptrOption = EnteredCmd+cmdStringLength;
			}
			else
			{
				ptrOption = EnteredCmd+cmdStringLength+1;
			}
			status = LoopOptions(ptrUserCmds,ptrOption);
		}
		ptrUserCmds++;
	}

	if(status < 0)
	{
		//SEGGER_RTT_printf(0,"unknown command\r\n");
	}

	return status;
}


/*******************************************************************************
 * Public Methods =============================================================
 ******************************************************************************/


/**
 * parse InputStr for decimal value.
 *
 *@param InputStr string to be parsed
 *@param PtrDecimal variable to save the read decimal value into
 * @return ptrEnd will be null if no more data to parse
 */
uint8_t * ParseDecimal(uint8_t *InputStr, uint32_t *PtrDecimal)
{
	int16_t idx;
	uint8_t *ptrStart;
	uint8_t *ptrEnd = NULL;
	uint32_t value;
	ptrStart = InputStr;
	idx = 0;

	/*
	 * first look for the first digit of the decimal number
	 */
	while(*(ptrStart+idx) == ' ')
	{
		idx++;
		/*
		 * @todo
		 * may want to put a check here, to make sure the idx does not
		 * get too big
		 */
	}
	ptrStart = ptrStart+idx;
	value = strtoul((char*)ptrStart,(char **)&ptrEnd,10);
	*PtrDecimal = value;

	if(ptrEnd == NULL)
	{
		/*
		 * no more data to parse
		 */
	}
	return ptrEnd;
}


/**
 * parse InputStr for hex value.
 *
 *@param InputStr C-String to be parsed
 *@param PtrHex variable to save the read decimal value into
 * @return will be null if no more data to parse or pointer to string where the parse left off
 */
uint8_t * ParseHex(uint8_t *InputStr, uint32_t *PtrHex)
{
	int16_t idx;
	uint8_t *ptrStart;
	uint8_t *ptrEnd = NULL;
	uint32_t value;
	ptrStart = InputStr;
	idx = 0;

	/*
	 * first look for the first digit of the decimal number
	 */
	while(*(ptrStart+idx) == ' ')
	{
		idx++;
		/*
		 * @todo
		 * may want to put a check here, to make sure the idx does not
		 * get too big
		 */
	}
	ptrStart = ptrStart+idx;
	value = strtoul((char*)ptrStart,(char **)&ptrEnd,16);
	*PtrHex = value;

	if(ptrEnd == NULL)
	{
		/*
		 * no more data to parse
		 */
	}
	return ptrEnd;
}

/**
 * provide 'OK' message from CLI command
 *
 */
void replyOK()
{
	const char *OKMsg = "OK\r\n";

	DebugTaskWrite(OKMsg,strlen(OKMsg));
}

/**
 * provide 'ERROR' message from CLI command
 */
void replyERROR()
{
	const char *ERRORMsg = "ERROR\r\n";

	DebugTaskWrite(ERRORMsg,strlen(ERRORMsg));
}



/******************************************************************************
 * main menu NGR Relay CLI support ============================================
 ******************************************************************************/

/**
 * display version of build and the main help menu of all available main commands.
 *
 * @param Param C-String of values passed from the command line
 * @return 0 upon completion
 */
static int32_t helpCmd(char *Param)
{
	uint16_t helpIdx=0;
	char version[64];

	sprintf(version,"\r\nVersion %d.%d.%d.%d\r\n",MAJOR_VERSION,MINOR_VERSION,VERSION_VERSION,REVISION_VERSION);
	DebugTaskWrite(version,strlen(version));

	while(tl_helpCmdTxt[helpIdx][0])
	{
		DebugTaskWrite(tl_helpCmdTxt[helpIdx],strlen((const char *)tl_helpCmdTxt[helpIdx]));
		helpIdx++;
	}
	return 0;
}

s_cliCommandOptions_t helpOptions[]= {
		{"-h",2,helpCmd},{"-?",2,helpCmd},{NULL,0,helpCmd}
}; /**< Array of pointers to different commands that would trigger displaying the help menu */


/**
 * This function initializes the connection of specific cli commands into the main menu
 * of options so that they can be executed by the user.
 */
void CliInit()
{

	//properly link the specific commands their options and their main menu text
	tl_helpCmdTxt[eMenuHeaderTxt] = "NGR help commands:\r\n";
	tl_helpCmdTxt[eLedTxt] = LedCliInit(&tl_UserCmds[eLedCmd]);
	tl_helpCmdTxt[eVoltageTxt] = VoltageCliInit(&tl_UserCmds[eVoltageCmd]);
	tl_helpCmdTxt[eGpioTxt] = GpioCliInit(&tl_UserCmds[eGpioCmd]);
	tl_helpCmdTxt[eRs485Txt] = rs485CliInit(&tl_UserCmds[eRs485Cmd]);
	tl_helpCmdTxt[eProductionTestTxt] = ProductionTestCliInit(&tl_UserCmds[eProductionTestCmd]);
	tl_helpCmdTxt[eIpAddressTxt] = IPCliInit(&tl_UserCmds[eIpAddressCmd]);
	tl_helpCmdTxt[eModbusTxt] = MbCliInit(&tl_UserCmds[eModbusCmd]);
	tl_helpCmdTxt[eExternFlashTxt] = extfCliInit(&tl_UserCmds[eExternFlashCmd]);
	tl_helpCmdTxt[eMenuEndTxt] = "\0";

	//add the help list used for the main menu of options available
	tl_UserCmds[eHelpCmd].cmdString = "help";
	tl_UserCmds[eHelpCmd].option = helpOptions;

	//make sure that the last item in the commands is null so we dont run into infinite loop issues
	tl_UserCmds[eNullCmd].cmdString = NULL;
	tl_UserCmds[eNullCmd].option = NULL;

}


/**
 * Place each character received over the debug CLI port into a buffer.
 * This buffer will perform a check if a CLI command is present based on if
 * a CR/LF is detected.
 *
 * @param InputKey character received over the debug UART port
 * @return void
 */
void cli(uint8_t InputKey)
{
	tl_cliData[tl_cliIdx++] = InputKey;

	if( (tl_cliData[tl_cliIdx-1] == '\n') || (tl_cliData[tl_cliIdx-1] == '\r'))
	{
		tl_cliData[tl_cliIdx-1] = 0;
		if( (tl_cliData[tl_cliIdx-2] == '\r') || (tl_cliData[tl_cliIdx-2] == '\n') )
		{
			tl_cliData[tl_cliIdx-2] = 0;
		}

		tl_cliIdx = 0;
		if(tl_cliData[tl_cliIdx]  != 0)
			LoopCmds(tl_cliData);
	}

	if(tl_cliIdx == CLI_BUFFER_SIZE)
	{
		tl_cliData[CLI_BUFFER_SIZE-1] = 0x0;
		tl_cliIdx = 0;
		LoopCmds(tl_cliData);
	}
}
