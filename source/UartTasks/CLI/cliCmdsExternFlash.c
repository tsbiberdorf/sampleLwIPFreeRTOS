/**
 * @file cliCmdsExternFlash.c
 * @details full implementation of interacting with cli commands related to
 *  external flash. This contains commands for not only generic interaction
 *  with external flash, but also interacting with fault log writing.
 */

#include <stdint.h>
#include <string.h>
#include "source/LED/LEDOperations.h"
#include "fsl_debug_console.h"


#include "UartTasks/DebugTask.h"
#include "Memory/externalFlash.h"
#include "Memory/FaultLogs.h"
#include "Cli.h"
/*******************************************************************************
 * Definitions ================================================================
 ******************************************************************************/
  /*****************************************************************************
   * Prototypes/externs ========================================================
   ****************************************************************************/
static int32_t extfHelpCmd(char *Param);
static int32_t extfReadCmd(char *Param);
static int32_t extfWriteEnableToggleCmd(char *Param);
static int32_t extfWritePatternCmd(char *Param);
static int32_t extfDumpCmd(char *Param);
static int32_t extfBulkEraseCmd(char *Param);
static int32_t extfWriteFullSector(char *Param);
static int32_t extPrintLogInfo(char *Param);
static int32_t extEraseSector(char *Param);
/*****************************************************************************
 * Variables =================================================================
 ****************************************************************************/

/**
 * @brief help string for command options related to external flash: extf
 */
const char *extfCmdText[] = {
		"extern spi flash help commands:\r\n",
		"-rd <register> :read register\r\n",
		"    9f RDID \r\n",
		"    05 RDSR \r\n",
		"    01 WRR \r\n",
		"    03 <start address>\r\n"
		"-wc <register>: write register \r\n",
		"    6 WREN write enable\r\n",
		"    4 WRDI write disable\r\n",
		"-se <sector 0..63>: erase sector \r\n",
		"-be bulk erase full flash \r\n",
		"-pp write a single patterned fault log into external flash \r\n",
		"-dump dump all logs\r\n",
		"-wfs : write full sector\r\n",
		"-fl : print where next fault log will be written\r\n",
		"\0"
};

/**
 * @brief CLI options supported with the command: extf
 */
s_cliCommandOptions_t extfOptions[]= {
		{"-rd", 3, extfReadCmd},
		{"-wc", 3, extfWriteEnableToggleCmd},
		{"-pp", 3, extfWritePatternCmd},
		{"-be", 3, extfBulkEraseCmd},
		{"-se", 3, extEraseSector},
		{"-dump", 5, extfDumpCmd},
		{"-wfs", 4, extfWriteFullSector},
		{"-fl", 3, extPrintLogInfo},
		{"-h",2,extfHelpCmd},{"-?",2,extfHelpCmd},{NULL,0,extfHelpCmd}
};

static const char *tl_extfMainHelpText = "\"extf\": external spi flash commands\r\n";
static s_cliCommands_t tl_extfMainCliCmd = {"extf", extfOptions};


/*******************************************************************************
 * Private Methods
 ******************************************************************************/

/**
 * @brief display menu options
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t extfHelpCmd(char *Param)
{
	uint16_t helpIdx=0;

	while(extfCmdText[helpIdx][0])
	{
		DebugTaskWrite(extfCmdText[helpIdx],strlen(extfCmdText[helpIdx]));
		helpIdx++;
	}
	return 0;
}

/**
 * @brief execute read command
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t extfReadCmd(char *Param)
{
	uint32_t value;
//	char Print_Str[64];

	//@todo handle incorrect entry with a print
	Param = (char *) ParseHex((uint8_t *)Param,&value); //get the register index to read

		switch(value)
		{
		case RDID:
			ExtfRDID();
			break;
		case READID:
			break;
		case RDSR:
			ExtfRDSR();
			break;
		case READ:
			//get the starting address to read from
			Param = (char *) ParseHex((uint8_t *)Param, &value);
			PRINTF("read address 0x%x\r\n", value);
			ExtfDumpPageData(value);

			break;
		default:
	//					context->printf_data_func("not supported read register\r\n");
			break;
		}
	return 0;
}

/**
 * @brief command to enable or disable writing to the spi flash
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t extfWriteEnableToggleCmd(char *Param)
{
	uint32_t selection;
	ParseHex((uint8_t *)&Param,&selection);
	//@todo handle missing proper argument
		switch(selection)
		{
		case WREN:
			ExtfWren();
			break;
		case WRDI:
			ExtfWrdi();
			break;
		default:
	//					context->printf_data_func("not supported write register\r\n");
		break;
		}
	return 0;
}

/**
 * @brief command handler to program a sector with a pattern in spi flash
 * @details address should be given in hex
 *
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t extfWritePatternCmd(char *Param)
{
//	uint32_t value, address;
//	ParseHex((uint8_t *)Param,&value); //get the register index to read
//	char Print_Str[64];

	//@todo handle the case of no argument being programmed
//		address = value;
//				context->printf_data_func("page program address 0x%x\r\n",address);
		//sprintf(Print_Str, "page program address 0x%x\r\n", address);
		//DebugTaskWrite(Print_Str,strlen(Print_Str));
		WritePatternLogToExtf();

	return 0;
}

/**
 * @brief dump display of all fault records
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t extfDumpCmd(char *Param)
{
	char Print_Str[25];

	sprintf(Print_Str, "dump all log records\r\n");
	DebugTaskWrite(Print_Str, strlen(Print_Str));
//	context->printf_data_func("dump all log records \r\n");
	DumpLogsFromExtf();
	return 0;
}

/**
 * @brief command to bulk erase all of external flash
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t extfBulkEraseCmd(char *Param)
{
	char Print_Str[26];

	sprintf(Print_Str, "erase all flash memory\r\n");
	DebugTaskWrite(Print_Str, strlen(Print_Str));
	////			context->printf_data_func("erase all flash memory \r\n");
	ExtfBE();
	return 0;
}

/**
 * @brief command to trigger writing a sectors worth of fault logs
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t extfWriteFullSector(char *Param){
	fillLogSector();
	return 0;
}

/**
 * @brief command to print where the next fault log will be written into flash
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t extPrintLogInfo(char *Param){
	sLogRecordLocation_t logvalue = getCurrentLogLocation();
	char Print_Str[25];
	sprintf(Print_Str, "Sector: %d\r\nindex: %d\r\n", logvalue.idxSector, logvalue.logIdxIntoSector);
	DebugTaskWrite(Print_Str, strlen(Print_Str));

	return 0;

}

/**
 * @brief command to erase a specified sector of external flash
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t extEraseSector(char *Param){

	uint32_t value;
	ParseHex((uint8_t *)Param,&value); //get the register index to read
	ExtfSectorErase(value);
	return 0;
}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/
/**
 * @brief initialize pointers to help menu and command execution.
 * @param CmdOpt pointer to the command option slot in main cli command array
 * @return C-String of title row for main menu's help display string
 */
const char* extfCliInit(s_cliCommands_t *CmdOpt)
{
	*CmdOpt = tl_extfMainCliCmd;
	return tl_extfMainHelpText;
}
