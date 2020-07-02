/**
 * @file    LEDOperations.c
 *
 * @brief   LED Operations Task.
 *
 */

//#include <RTT/SEGGER_RTT.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
//#include "MK60D10.h"
#include "fsl_uart.h"
#include "NGRMRelay.h"

#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "TaskParameters.h"
#include "source/ProductionTest/ProductionTest.h"
#include "Memory/LocalModbusMemory.h"
#include "drivers/fsl_adc16.h"
#include "LEDOperations.h"

/******************************************************************************
 * defines/macros
 *****************************************************************************/
#define PASS (0x003F)
#define DEBUG_UART UART0

#define LED_FLASH_DELAY_TIME (100) //* delay time for flashing LED, should be about 1 sec

#define GND_FLT (0)
#define NGR_FLT (1)
#define TRIPPED (2)
#define SENSOR (3)
#define POWER (4)
#define WIFI (5)
#define NUMBER_OF_LEDS (WIFI+1)

#define LED_SELFTEST_ADC0 (ADC0)
#define LED_SELFTEST_ADC1 (ADC1)
#define LED_SELFTEST_GROUP (0)

#define ADC0_DM0_CH (19) // pin 19
#define ADC0_DP1_CH (1) // pin 14
#define ADC0_DM1_CH (20) // pin 15

#define ADC1_DM0_CH (19) // pin 21
#define ADC1_DP0_CH (0)	// pin 20
#define ADC1_DP1_CH (1) // pin 16

#define ADC1_VREF_CH (18) // pin 26 VREF
#define ADC1_DAC_CH  (23) // pin 27 DAC0

#define GND_SELFTEST_ADC 		(LED_SELFTEST_ADC0)
#define GND_SELFTEST_GROUP 		(LED_SELFTEST_GROUP)
#define GND_SELFTEST_CH 		(ADC1_DM0_CH)

#define NGR_SELFTEST_ADC 		(LED_SELFTEST_ADC1)
#define NGR_SELFTEST_GROUP 		(LED_SELFTEST_GROUP)
#define NGR_SELFTEST_CH 		(ADC1_DP0_CH)

#define TRIPPED_SELFTEST_ADC 	(LED_SELFTEST_ADC0)
#define TRIPPED_SELFTEST_GROUP 	(LED_SELFTEST_GROUP)
#define TRIPPED_SELFTEST_CH 	(ADC0_DM0_CH)

#define SENSOR_SELFTEST_ADC 	(LED_SELFTEST_ADC0)
#define SENSOR_SELFTEST_GROUP 	(LED_SELFTEST_GROUP)
#define SENSOR_SELFTEST_CH 		(ADC0_DP1_CH)

#define POWER_SELFTEST_ADC 		(LED_SELFTEST_ADC1)
#define POWER_SELFTEST_GROUP 	(LED_SELFTEST_GROUP)
#define POWER_SELFTEST_CH 		(ADC1_DP1_CH)

#define WIFI_SELFTEST_ADC 		(LED_SELFTEST_ADC0)
#define WIFI_SELFTEST_GROUP 	(LED_SELFTEST_GROUP)
#define WIFI_SELFTEST_CH 		(ADC0_DM1_CH)

#define V24_ADC 				(LED_SELFTEST_ADC1)
#define V24_GROUP 				(LED_SELFTEST_GROUP)
#define V24_CH 					(ADC1_VREF_CH)

#define V33_ADC 				(LED_SELFTEST_ADC0)
#define V33_GROUP 				(LED_SELFTEST_GROUP)
#define V33_CH 					(ADC1_DAC_CH)

typedef struct LED_SelfTest_s {
	adc16_channel_config_t chConfig;
	ADC_Type * LedADC;

	uint32_t LedGroup;
	uint16_t adcRead;
}sLedSelftest_t;

/******************************************************************************
 * class variables
 *****************************************************************************/
TaskHandle_t  tl_LEDTaskHandlerId = NULL;
const char *LedTaskName = "LedTask";
uint32_t tl_LedSelfTestFlag = 0;

sLedSelftest_t tl_LedSelfTest[NUMBER_OF_LEDS];
adc16_channel_config_t tl_33vConfig;

/******************************************************************************
 * Private Methods
 *****************************************************************************/
/**
 * @brief LED Task
 *
 * @todo this code need to be debugged on NGR hardware.  When first testing, carefull stepping through the code
 * will need to be done to verify operation.
 * Tests to run would include turn on/off each LED, then perform the LED selftest and confirm that the high/low
 * ADC measurements are where they should be.
 */
static void LedTask(void *pvParameters)
{
	uint32_t notifyBits;
	uint16_t ledFlashDelay = 0;
	uint16_t interruptFlag = 0;
	uint16_t ledADCValue = 0;
	uint8_t modbusIdx;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 10 );
	BaseType_t xResult;
	//SEGGER_RTT_printf(0,"LED Task started\r\n");
	uint8_t selfTestLoop;
//	uint8_t ledSelfTestResults = 0;

    tl_LedSelfTest[GND_FLT].LedADC = GND_SELFTEST_ADC;
    tl_LedSelfTest[GND_FLT].LedGroup = GND_SELFTEST_GROUP;
    tl_LedSelfTest[GND_FLT].chConfig.channelNumber = GND_SELFTEST_CH;
    tl_LedSelfTest[GND_FLT].chConfig.enableInterruptOnConversionCompleted = false;
    tl_LedSelfTest[GND_FLT].chConfig.enableDifferentialConversion = false;

    tl_LedSelfTest[NGR_FLT].LedADC = NGR_SELFTEST_ADC;
    tl_LedSelfTest[NGR_FLT].LedGroup = NGR_SELFTEST_GROUP;
    tl_LedSelfTest[NGR_FLT].chConfig.channelNumber = NGR_SELFTEST_CH;
    tl_LedSelfTest[NGR_FLT].chConfig.enableInterruptOnConversionCompleted = false;
    tl_LedSelfTest[NGR_FLT].chConfig.enableDifferentialConversion = false;

    tl_LedSelfTest[TRIPPED].LedADC = TRIPPED_SELFTEST_ADC;
    tl_LedSelfTest[TRIPPED].LedGroup = TRIPPED_SELFTEST_GROUP;
    tl_LedSelfTest[TRIPPED].chConfig.channelNumber = TRIPPED_SELFTEST_CH;
    tl_LedSelfTest[TRIPPED].chConfig.enableInterruptOnConversionCompleted = false;
    tl_LedSelfTest[TRIPPED].chConfig.enableDifferentialConversion = false;

    tl_LedSelfTest[SENSOR].LedADC = SENSOR_SELFTEST_ADC;
    tl_LedSelfTest[SENSOR].LedGroup = SENSOR_SELFTEST_GROUP;
    tl_LedSelfTest[SENSOR].chConfig.channelNumber = SENSOR_SELFTEST_CH;
    tl_LedSelfTest[SENSOR].chConfig.enableInterruptOnConversionCompleted = false;
    tl_LedSelfTest[SENSOR].chConfig.enableDifferentialConversion = false;

    tl_LedSelfTest[POWER].LedADC = POWER_SELFTEST_ADC;
    tl_LedSelfTest[POWER].LedGroup = POWER_SELFTEST_GROUP;
    tl_LedSelfTest[POWER].chConfig.channelNumber = POWER_SELFTEST_CH;
    tl_LedSelfTest[POWER].chConfig.enableInterruptOnConversionCompleted = false;
    tl_LedSelfTest[POWER].chConfig.enableDifferentialConversion = false;

    tl_LedSelfTest[WIFI].LedADC = POWER_SELFTEST_ADC;
    tl_LedSelfTest[WIFI].LedGroup = POWER_SELFTEST_GROUP;
    tl_LedSelfTest[WIFI].chConfig.channelNumber = POWER_SELFTEST_CH;
    tl_LedSelfTest[WIFI].chConfig.enableInterruptOnConversionCompleted = false;
    tl_LedSelfTest[WIFI].chConfig.enableDifferentialConversion = false;

    tl_33vConfig.channelNumber = V33_CH;


    PRINTF("LED Task Started\r\n");
	while(1)
	{
//		tl_LEDTaskHandlerId = ledTaskHandlerId;
		xResult = xTaskNotifyWait( 0x00,    /* Don't clear bits on entry. */
				0xFFFFFFFF,        /* Clear all bits on exit. */
				&notifyBits, /* Stores the notified value. */
				xMaxBlockTime );

		if( xResult == pdPASS )
		{
			if( notifyBits & LED_NGR_ON)
			{
//				GPIO_SetPinsOutput(BOARD_RELAY_TRIP_GPIO,1<<BOARD_RELAY_TRIP_PIN);
				GPIO_PortSet(BOARD_NGR_FLT_LED_GPIO,1<<BOARD_NGR_FLT_LED_PIN);
			}
			if( notifyBits & LED_NGR_OFF)
			{
				GPIO_PortClear(BOARD_NGR_FLT_LED_GPIO,1<<BOARD_NGR_FLT_LED_PIN);
			}
			if( notifyBits & LED_GND_ON)
			{
				GPIO_PortSet(BOARD_GND_FLT_LED_GPIO,1<<BOARD_GND_FLT_LED_PIN);
			}
			if( notifyBits & LED_GND_OFF)
			{
				GPIO_PortClear(BOARD_GND_FLT_LED_GPIO,1<<BOARD_GND_FLT_LED_PIN);
			}
			if( notifyBits & LED_TRIPPED_ON)
			{
				GPIO_PortSet(BOARD_TRIPPED_LED_GPIO,1<<BOARD_TRIPPED_LED_PIN);
			}
			if( notifyBits & LED_TRIPPED_OFF)
			{
				GPIO_PortClear(BOARD_TRIPPED_LED_GPIO,1<<BOARD_TRIPPED_LED_PIN);
			}
			if( notifyBits & LED_SENSOR_ON)
			{
				GPIO_PortSet(BOARD_SENSOR_LED_GPIO,1<<BOARD_SENSOR_LED_PIN);
			}
			if( notifyBits & LED_SENSOR_OFF)
			{
				GPIO_PortClear(BOARD_SENSOR_LED_GPIO,1<<BOARD_SENSOR_LED_PIN);
			}
			if( notifyBits & LED_POWER_ON)
			{
				GPIO_PortSet(BOARD_POWER_LED_GPIO,1<<BOARD_POWER_LED_PIN);
			}
			if( notifyBits & LED_POWER_OFF)
			{
				GPIO_PortClear(BOARD_POWER_LED_GPIO,1<<BOARD_POWER_LED_PIN);
			}
			if( notifyBits & LED_WIFI_ON)
			{
				GPIO_PortSet(BOARD_WIFI_LED_GPIO,1<<BOARD_WIFI_LED_PIN);
			}
			if( notifyBits & LED_WIFI_OFF)
			{
				GPIO_PortClear(BOARD_WIFI_LED_GPIO,1<<BOARD_WIFI_LED_PIN);
			}
			if( notifyBits & V24_TEST_ON)
			{
				ADC16_SetChannelConfig(V24_ADC,V24_GROUP,V24_CH);
		        while (0U == (kADC16_ChannelConversionDoneFlag &
		                      ADC16_GetChannelStatusFlags(V24_ADC, V24_GROUP)))
		        {
		        }
		        PRINTF("23V ADC value %d\r\n", ADC16_GetChannelConversionValue(V24_ADC, V24_GROUP));

			}
			if( notifyBits & V33_TEST_ON)
			{

				ADC16_SetChannelConfig(V33_ADC,V33_GROUP,&tl_33vConfig);
		        while (0U == (kADC16_ChannelConversionDoneFlag &
		                      ADC16_GetChannelStatusFlags(V33_ADC, V33_GROUP)))
		        {
		        }
		        uint32_t convertedValue = ADC16_GetChannelConversionValue(V33_ADC, V33_GROUP);
		        PRINTF("33V ADC value %d\r\n", convertedValue);
				updatePTReg(ePtResultReg1, convertedValue);
				updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);
			}
			if( notifyBits & LED_TEST_ON)
			{
				// set all LEDs
				GPIO_PortSet(BOARD_NGR_FLT_LED_GPIO,1<<BOARD_NGR_FLT_LED_PIN);
				GPIO_PortSet(BOARD_GND_FLT_LED_GPIO,1<<BOARD_GND_FLT_LED_PIN);
				GPIO_PortSet(BOARD_TRIPPED_LED_GPIO,1<<BOARD_TRIPPED_LED_PIN);
				GPIO_PortSet(BOARD_SENSOR_LED_GPIO,1<<BOARD_SENSOR_LED_PIN);
				GPIO_PortSet(BOARD_POWER_LED_GPIO,1<<BOARD_POWER_LED_PIN);
				GPIO_PortSet(BOARD_WIFI_LED_GPIO,1<<BOARD_WIFI_LED_PIN);

		        // need to cycle through all 6 LEDs

#warning "Step through this code on hardware to verify operation"
		        for( selfTestLoop = 0, modbusIdx = ePtResultReg1; selfTestLoop < NUMBER_OF_LEDS; selfTestLoop++, modbusIdx++)
		        {
		        	//@bug only ends up being executed once per power cycle
			        ADC16_SetChannelConfig( tl_LedSelfTest[selfTestLoop].LedADC,
			        		tl_LedSelfTest[selfTestLoop].LedGroup,
							&tl_LedSelfTest[selfTestLoop].chConfig);
			        while (0U == (kADC16_ChannelConversionDoneFlag &
			                      ADC16_GetChannelStatusFlags(
			                    		  tl_LedSelfTest[selfTestLoop].LedADC,
										  tl_LedSelfTest[selfTestLoop].LedGroup)))
			        {
			        }
			        ledADCValue = ADC16_GetChannelConversionValue(
			        		tl_LedSelfTest[selfTestLoop].LedADC,
			        		tl_LedSelfTest[selfTestLoop].LedGroup);
			        PRINTF("ADC Value: %d\r\n", ledADCValue);
			        /**
			         *  @todo need to call MemoryMap storage and place the ADC value that the tester will have to analyze
			         */
			        updatePTReg(modbusIdx, ledADCValue );
		        }

				// clear all LEDs
		        GPIO_PortClear(BOARD_NGR_FLT_LED_GPIO,1<<BOARD_NGR_FLT_LED_PIN);
		        GPIO_PortClear(BOARD_GND_FLT_LED_GPIO,1<<BOARD_GND_FLT_LED_PIN);
		        GPIO_PortClear(BOARD_TRIPPED_LED_GPIO,1<<BOARD_TRIPPED_LED_PIN);
		        GPIO_PortClear(BOARD_SENSOR_LED_GPIO,1<<BOARD_SENSOR_LED_PIN);
		        GPIO_PortClear(BOARD_POWER_LED_GPIO,1<<BOARD_POWER_LED_PIN);
		        GPIO_PortClear(BOARD_WIFI_LED_GPIO,1<<BOARD_WIFI_LED_PIN);

				/**
				 * @todo need to call the memory map to indicate that the test has been completed.
				 */
				updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);
			}

//			SEGGER_RTT_printf(0,"led %d\r\n",notifyBits);
//			GPIO_TogglePinsOutput(BOARD_INITPINS_RedLED_GPIO,1<<BOARD_INITPINS_RedLED_PIN);
//			GPIO_TogglePinsOutput(BOARD_INITPINS_GreenLED_GPIO,1<<BOARD_INITPINS_GreenLED_PIN);
//			GPIO_TogglePinsOutput(BOARD_INITPINS_BlueLED_GPIO,1<<BOARD_INITPINS_BlueLED_PIN);
		}
		else
		{
		}

		if(interruptFlag)
		{
			if( interruptFlag++ > LED_FLASH_DELAY_TIME)
			{
				interruptFlag = 0;
				// flash LED to indicate still alive
//				GPIO_ClearPinsOutput(GPIOC,LED3);
			}
		}

		if(ledFlashDelay++ > LED_FLASH_DELAY_TIME)
		{
			ledFlashDelay = 0;
			// flash LED to indicate still alive
//			GPIO_TogglePinsOutput(GPIOC,LED2);
		}

	}
}

/**
 * return the LED self test results
 */
int32_t LedSelfTestResults()
{
	/*
	 * not sure what were going to do with the results yet
	 */
	tl_LedSelfTestFlag = 0;
	return 0;
}

TaskHandle_t GetLedTaskHandle()
{
	return tl_LEDTaskHandlerId;
}

void StartLedTask()
{
	xTaskCreate(LedTask, LedTaskName, LED_TASK_STACK_SIZE , NULL, LED_TASK_PRIORITY, &tl_LEDTaskHandlerId);

}
