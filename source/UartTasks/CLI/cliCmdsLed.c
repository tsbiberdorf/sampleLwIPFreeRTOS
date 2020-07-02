/**
 * @file cliCmdsLed.c
 * @details Implementation code related to API that interacts with the LED commands
 * @see cliCmdsLed.h for public definition of API
 * @ingroup groupCliCmds
 */


#include <stdint.h>
#include <string.h>
#include "source/LED/LEDOperations.h"
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "UartTasks/DebugTask.h"
#include "Cli.h"
#include "cliCmdsLed.h"

  /*****************************************************************************
   * Definitions ===============================================================
   ****************************************************************************/
  /*****************************************************************************
   * Prototypes/externs ========================================================
   ****************************************************************************/
static int32_t ledSelfTest(char *Param);
static int32_t ledTripped(char *Param);
static int32_t ledNgrFlt(char *Param);
static int32_t ledGndFlt(char *Param);
static int32_t ledSensor(char *Param);
static int32_t ledPower(char *Param);
static int32_t ledWifi(char *Param);
static int32_t ledHelpCmd(char *Param);

/*****************************************************************************
 * Variables =================================================================
 ****************************************************************************/
/**
 * @brief help menu C-string
 * @todo update for NGR relay code
 */
static char const * const ledCmdText[] = {
		"led help commands:\r\n",
		"-gndflt [0/1] turn gndflt LED on/off\r\n",
		"-ngrflt [0/1] turn ngrflt LED on/off\r\n",
		"-tripped [0/1] turn tripped LED on/off\r\n",
		"-sensor [0/1] turn sensor LED on/off\r\n",
		"-power [0/1] turn power LED on/off\r\n",
		"-wifi [0/1] turn wifi LED on/off\r\n",
		"-test LED Self test\r\n",
		"\0"
};

/**
 *@brief CLI options supported with the command: led
 *
 * @todo update for ngr relay code
 */
static s_cliCommandOptions_t ledOptions[]= {
		{"-test",5,ledSelfTest},
		{"-gndflt",7,ledGndFlt},
		{"-ngrflt",7,ledNgrFlt},
		{"-tripped",8,ledTripped},
		{"-sensor",7,ledSensor},
		{"-power",6,ledPower},
		{"-wifi",5,ledWifi},
		{"-h",2,ledHelpCmd},{"-?",2,ledHelpCmd},{NULL,0,ledHelpCmd}
};

static char const * const tl_ledMainHelpText = "led Commands\r\n";
static s_cliCommands_t tl_ledMainCliCmd = {"led", ledOptions};
/*****************************************************************************
 * Private Methods ===========================================================
 ****************************************************************************/
/**
 * @brief LED Self test request
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ledSelfTest(char *Param)
{
	uint32_t value;
	PRINTF("LED Self Test ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("run \r\n");
			xTaskNotify(GetLedTaskHandle(),LED_TEST_ON,eSetBits);
		}
		else
		{
			PRINTF("results %d\r\n",LedSelfTestResults());
		}
	}
	else
	{
		PRINTF("\r\n");
	}

	return 0;
}


/**
 * @brief Tripped LED
 * @details turn tripped led off if passed a 0, on otherwise
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ledTripped(char *Param)
{
	uint32_t value;
	PRINTF("led Tripped ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_TRIPPED_ON,eSetBits);
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_TRIPPED_OFF,eSetBits);
		}
	}
	else
	{
		PRINTF("\r\n");
	}
	return 0;
}


/**
 * @brief NGR LED
 * @details turn ngr led off if passed a 0, on otherwise
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ledNgrFlt(char *Param)
{
	uint32_t value;
	PRINTF("led NgrFlt ");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_NGR_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_NGR_OFF,eSetBits);
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
 * @brief Gnd LED
 * @details turn ground fault led off if passed a 0, on otherwise
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ledGndFlt(char *Param)
{
	uint32_t value;
	PRINTF("led ledGndFlt");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_GND_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_GND_OFF,eSetBits);
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
 * @brief Sensor LED
 * @details turn sensor led off if passed a 0, on otherwise
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ledSensor(char *Param)
{
	uint32_t value;
	PRINTF("led ledSensor");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_SENSOR_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_SENSOR_OFF,eSetBits);
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
 * @brief Power LED
 * @details turn power led off if passed a 0, on otherwise
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ledPower(char *Param)
{
	uint32_t value;
	PRINTF("led ledPower");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_POWER_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_POWER_OFF,eSetBits);
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
 * @brief Wifi LED
 * @details turn wifi led off if passed a 0, on otherwise
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ledWifi(char *Param)
{
	uint32_t value;
	PRINTF("led ledWifi");
	if( ParseDecimal((uint8_t *)Param,&value) )
	{
		if(value)
		{
			PRINTF("ON\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_WIFI_ON,eSetBits);
			replyOK();
		}
		else
		{
			PRINTF("OFF\r\n");
			xTaskNotify(GetLedTaskHandle(),LED_WIFI_OFF,eSetBits);
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
 * @brief LED help
 * @details display the help menu text to user
 * @param Param C-String obtained from cli arguments
 * @return 0 upon success
 */
static int32_t ledHelpCmd(char *Param)
{
	uint16_t helpIdx=0;

	while(ledCmdText[helpIdx][0])
	{
		DebugTaskWrite(ledCmdText[helpIdx],strlen(ledCmdText[helpIdx]));
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
const char* LedCliInit(s_cliCommands_t *CmdOpt)
{
	*CmdOpt = tl_ledMainCliCmd;
	return tl_ledMainHelpText;
}
