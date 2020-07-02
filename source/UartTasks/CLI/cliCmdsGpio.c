/**
 * @file cliCmdsGpio.c
 *
 * @details Implementation Code for the API related to processing GPIO Cli Commands
 * @see cliCmdsGpio.h for public facing functions
 * @ingroup groupCliCmds
 *
 */

#include <stdint.h>
#include <string.h>
#include "source/LED/LEDOperations.h"
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "UartTasks/DebugTask.h"
#include "Cli.h"
#include "cliCmdsGpio.h"
#include "Peripheral/GPIO/GpiocOperations.h"
  /*****************************************************************************
   * Definitions ===============================================================
   ****************************************************************************/
  /*****************************************************************************
   * Prototypes/externs ========================================================
   ****************************************************************************/
static int32_t resetPin(char *Param);
static int32_t tripRelay(char *Param);
static int32_t ngrRelay(char *Param);
static int32_t gndRelay(char *Param);
static int32_t auxRelay(char *Param);
static int32_t selfTestRelay(char *Param);
static int32_t gpioHelpCmd(char *Param);

/*****************************************************************************
 * Variables =================================================================
 ****************************************************************************/
/**
 * @brief help string for command options associated with GPIO.
 */
char const  * const gpioCmdText[] = {
		"gpio help commands:\r\n",
		"-trip [0/1] release/energize trip relay \r\n",
		"-ngr [0/1] release/energize ngr relay \r\n",
		"-gnd [0/1] release/energize gnd relay \r\n",
		"-aux [0/1] release/energize aux relay \r\n",
		"-test trigger relay self test \r\n",
		"-reset [0/1] set reset bit high/low \r\n",
		"\0"
};

/**
 * @brief CLI options supported with the command: gpio
 */
s_cliCommandOptions_t gpioOptions[]= {
		{"-trip",5,tripRelay},
		{"-ngr",4,ngrRelay},
		{"-gnd",4,gndRelay},
		{"-aux",4,auxRelay},
		{"-test",5,selfTestRelay},
		{"-reset",6,resetPin},

		{"-h",2,gpioHelpCmd},{"-?",2,gpioHelpCmd},{NULL,0,gpioHelpCmd}
};

static char const  *const  tl_gpioMainHelpText = "gpio commands\r\n";
static s_cliCommands_t tl_gpioMainCliCmd = {"gpio", gpioOptions};

/*****************************************************************************
 * Private/Protected Methods =================================================
 ****************************************************************************/

/**
 * @brief command to set reset pin high or low
 * @details if passed value of 0 turn off pin otherwise turn it on
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t resetPin(char *Param)
{
	uint32_t value;
	PRINTF("reset pin ");

	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("high\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RESET_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("low\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RESET_OFF,eSetBits);
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
 * @brief command to turn trip relay on or off
 * @details if passed 0 turn off relay, other wise turn it on
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t tripRelay(char *Param)
{
	uint32_t value;
	PRINTF("trip relay ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RELAY_TRIP_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RELAY_TRIP_OFF,eSetBits);
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
 * @brief command to turn ngr relay on or off
 * @details if passed 0 turn off relay, other wise turn it on
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t ngrRelay(char *Param)
{
	uint32_t value;
	PRINTF("ngr relay ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RELAY_NGR_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RELAY_NGR_OFF,eSetBits);
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
 * @brief command to turn ground relay on or off
 * @details if passed 0 turn off relay, other wise turn it on
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t gndRelay(char *Param)
{
	uint32_t value;
	PRINTF("gnd relay ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RELAY_GND_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RELAY_GND_OFF,eSetBits);
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
 * @brief command to turn aux relay on or off
 * @details if passed 0 turn off relay, other wise turn it on
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t auxRelay(char *Param)
{
	uint32_t value;
	PRINTF("aux relay ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RELAY_AUX_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetGPIOTaskHandle(),RELAY_AUX_OFF,eSetBits);
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
 * @brief command to initialize the self test operation of all relays
 * @see TriggerDigitalInputTest for details about the test
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t selfTestRelay(char *Param)
{
	uint32_t value;
	PRINTF("relay self test ");

	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("Start\r\n");
			xTaskNotify(GetGPIOTaskHandle(),PT_TEST_DIGITAL_IO,eSetBits);
			replyOK();
		}
		else
		{
			if( GpioSelfTestResults() == 0 )
			{
				replyOK();
			}
			PRINTF("Done\r\n");
			xTaskNotify(GetGPIOTaskHandle(),SELFTEST_DONE,eSetBits);
			replyOK();
		}
	}
	else
	{
		PRINTF("\r\n");
	}




	PRINTF("\r\n");
	return 0;
}


/**
 * @brief display options available for gpio commands
 * @param Param C-String obtained from cli arguments
 * @return 0 upon successful execution of command
 */
static int32_t gpioHelpCmd(char *Param)
{
	uint16_t helpIdx=0;

	while(gpioCmdText[helpIdx][0])
	{
		DebugTaskWrite(gpioCmdText[helpIdx],strlen(gpioCmdText[helpIdx]));
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
const char* GpioCliInit(s_cliCommands_t *CmdOpt)
{
	*CmdOpt = tl_gpioMainCliCmd;
	return tl_gpioMainHelpText;
}
