/**
 * @file cliCmdsIP.c
 * @details API implementation for interacting with the IP address operations
 * only use the -connect instruction if the starting of ip task items isn't
 * triggered anywhere else in execution.
 *
 * @see cliCmdsIP.h for public facing functions
 * @ingroup groupCliCmds
 */
#include <stdint.h>
#include <string.h>
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "UartTasks/DebugTask.h"
#include "Cli.h"
#include "networkTasks/IpConnectHandler.h"
#include "cliCmdsIP.h"
  /*****************************************************************************
   * Definitions ===============================================================
   ****************************************************************************/
  /*****************************************************************************
   * Prototypes/externs ========================================================
   ****************************************************************************/
static int32_t printIpAddr(char *Param);
static int32_t setStaticIpAddr(char *Param);
static int32_t setGateway(char *Param);
static int32_t setSubnetMask(char *Param);
static int32_t startConnection(char *Param);
static int32_t ipHelpCmd(char *Param);

/*****************************************************************************
 * Variables =================================================================
 ****************************************************************************/
/**
 * @brief help menu string
 */
char const * const ipCmdText[] = {
		"ip address help commands:\r\n",
		"-p print current ip address\r\n",
		"-sip A B C D set static ip address\r\n",
		"-snm A B C D set network address\r\n",
		"-gwip A B C D set gateway address\r\n",
		"-connect connect with current ip settings\r\n",
		"\0"
};

/**
 * @brief CLI options supported with the command: ipaddr
 */
s_cliCommandOptions_t ipOptions[]= {
		{"-p", 2, printIpAddr},
		{"-sip", 4, setStaticIpAddr},
		{"-snm", 4, setSubnetMask},
		{"-gwip", 5, setGateway},
		{"-connect", 8, startConnection},

		{"-h",2,ipHelpCmd},{"-?",2,ipHelpCmd},
		{NULL,0,ipHelpCmd}
};
static char const * const tl_IpMainHelpText = "ipaddr - IP Commands\r\n";
static s_cliCommands_t tl_IpMainCliCmd = {"ipaddr", ipOptions};


/*****************************************************************************
 * Private Methods ===========================================================
 ****************************************************************************/


/**
 * @brief display device's current ip address.
 * @bug doesn't return fail if operation couldn't complete properly
 * @param Param C-String obtained from cli arguments
 * @return 0 after print
 */
static int32_t printIpAddr(char *Param)
{
	printIpValues();
    return 0;
}

/**
 * @brief Set the value of the static ip address.
 * @bug doesn't return fail if operation couldn't complete properly
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t setStaticIpAddr(char *Param)
{
//	changeToDHCP();
	uint32_t value;
	uint8_t *nextParam;
	uint8_t newAddress[] ={0,0,0,0};

//	char display_val[64];
	nextParam = ParseDecimal((uint8_t *)Param,&value);
	if(nextParam!= NULL)
	{
//		sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//		DebugTaskWrite(display_val,strlen(display_val));
		newAddress[0] = value;

		nextParam = ParseDecimal((uint8_t *)nextParam,&value);
		if(nextParam != NULL)
		{
			//if there is another param print it
//			sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//			DebugTaskWrite(display_val,strlen(display_val));
			newAddress[1] = value;

		}

		nextParam = ParseDecimal((uint8_t *)nextParam,&value);
		if(nextParam != NULL)
		{
			//if there is another param print it
//			sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//			DebugTaskWrite(display_val,strlen(display_val));
			newAddress[2] = value;

		}
		nextParam = ParseDecimal((uint8_t *)nextParam,&value);
		if(nextParam != NULL)
		{
			//if there is another param print it
//			sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//			DebugTaskWrite(display_val,strlen(display_val));
			newAddress[3] = value;
		}

		//@todo don't allow if there were too many parameters
		setCurrentIpv4Address(newAddress);
	}
	else
	{
		PRINTF("\r\n");
	}
	return 0;
}

/**
 * @brief Set the value of the gateway address.
 * @bug doesn't return fail if operation couldn't complete properly
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t setGateway(char *Param)
{
	uint32_t value;
	uint8_t *nextParam;
	uint8_t newAddress[] ={0,0,0,0};

//	char display_val[64];
	nextParam = ParseDecimal((uint8_t *)Param,&value);
	if(nextParam!= NULL)
	{
//		sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//		DebugTaskWrite(display_val,strlen(display_val));
		newAddress[0] = value;

		nextParam = ParseDecimal((uint8_t *)nextParam,&value);
		if(nextParam != NULL)
		{
			//if there is another param print it
//			sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//			DebugTaskWrite(display_val,strlen(display_val));
			newAddress[1] = value;

		}

		nextParam = ParseDecimal((uint8_t *)nextParam,&value);
		if(nextParam != NULL)
		{
			//if there is another param print it
//			sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//			DebugTaskWrite(display_val,strlen(display_val));
			newAddress[2] = value;

		}
		nextParam = ParseDecimal((uint8_t *)nextParam,&value);
		if(nextParam != NULL)
		{
			//if there is another param print it
//			sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//			DebugTaskWrite(display_val,strlen(display_val));
			newAddress[3] = value;
		}

		//@todo don't allow if there were too many parameters
		setCurrentIpv4Gateway(newAddress);
	}
	else
	{
		PRINTF("\r\n");
	}
	return 0;
}

/**
 * @brief Set the value of the subnet mask.
 * @bug doesn't return fail if operation couldn't complete properly
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t setSubnetMask(char *Param)
{
	uint32_t value;
	uint8_t *nextParam;
	uint8_t newAddress[] ={0,0,0,0};

//	char display_val[64];
	nextParam = ParseDecimal((uint8_t *)Param,&value);
	if(nextParam!= NULL)
	{
//		sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//		DebugTaskWrite(display_val,strlen(display_val));
		newAddress[0] = value;

		nextParam = ParseDecimal((uint8_t *)nextParam,&value);
		if(nextParam != NULL)
		{
			//if there is another param print it
//			sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//			DebugTaskWrite(display_val,strlen(display_val));
			newAddress[1] = value;

		}

		nextParam = ParseDecimal((uint8_t *)nextParam,&value);
		if(nextParam != NULL)
		{
			//if there is another param print it
//			sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//			DebugTaskWrite(display_val,strlen(display_val));
			newAddress[2] = value;

		}
		nextParam = ParseDecimal((uint8_t *)nextParam,&value);
		if(nextParam != NULL)
		{
			//if there is another param print it
//			sprintf(display_val,"%u", ((u8_t *)&value)[0]);
//			DebugTaskWrite(display_val,strlen(display_val));
			newAddress[3] = value;
		}

		//@todo don't allow if there were too many parameters
		setCurrentIpv4Netmask(newAddress);
	}
	else
	{
		PRINTF("\r\n");
	}
	return 0;
}

//void initIpAddrTasks(); /**< extern reference to starting the connection task */
/**
 * @brief Command to start the IP connection to the network.
 * @param Param C-String obtained from cli arguments
 * @bug doesn't return anything but success
 * @return 0 upon success
 */
static int32_t startConnection(char *Param)
{
//	StartConnectionTask();
	initIpAddrTasks();
	return 0;
}

/**
 * @brief display help menu for Ip command options.
 * @param Param C-String obtained from cli arguments
 * @return 0
 */
static int32_t ipHelpCmd(char *Param)
{
	uint16_t helpIdx=0;

	while(ipCmdText[helpIdx][0])
	{
		DebugTaskWrite(ipCmdText[helpIdx],strlen(ipCmdText[helpIdx]));
		helpIdx++;
	}
	return 0;
}
/*******************************************************************************
 * Public Methods
 ******************************************************************************/
/**
 * @brief initalize pointers to the help menu string and commands to execute.
 * @param CmdOpt pointer to the command option slot in main cli command array
 * @return C-String of title row for main menu's help display string
 */
const char* IPCliInit(s_cliCommands_t *CmdOpt)
{
	*CmdOpt = tl_IpMainCliCmd;
	return tl_IpMainHelpText;
}
