/**
 * @file GpiocOperations.c
 *
 * @details full implementation of handler for controlling the relays and a voltage readings over gpio lines
 * @see GpiocOperations.h for public API implementation
 * @todo for the energize all relays and release all relays it may become necessary to special case the Production test update if we end up freeing the memory allocated to the production test modbus buffer
 * @todo clean up remaining header and other necessary documentation
 */

#include <stdio.h>
#include <assert.h>
//#include <RTT/SEGGER_RTT.h>
#include "FreeRTOS.h"
#include "task.h"
//#include "MK60D10.h"
#include "fsl_uart.h"
#include "TaskParameters.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LED/LEDOperations.h"
#include "UartTasks/DebugTask.h"
//#include "source/ProductionTest/ProductionTest.h"
//#include "Memory/LocalModbusMemory.h"
#include "GpiocOperations.h"

/******************************************************************************
 * Define/Macros
 *****************************************************************************/
#define DEBUG_UART UART0

#define BOARD_INPUT_IRQ_HANDLER GPIO_1_IRQHANDLER

#define BOARD_TestC1_GPIO GPIOC
#define BOARD_TestC1_PORT PORTC
#define BOARD_TestC1_PIN 1
#define BOARD_SW_IRQ BOARD_SW1_IRQ
#define BOARD_SW_NAME BOARD_SW1_NAME

#define ALL_RELAYS_ENERGIZED (302)
#define ALL_RELAYS_RELEASED  (303)

static volatile uint16_t ts_Pin1TestCount = 0;
static uint32_t tl_ButtonPressed = false;
static uint32_t tl_ChangeConfigurationFlag = 0;
const char *GpioTaskName = "GpioTask";

//volatile uint32_t ts_IrqCount = 0;

typedef enum _PinSelection_e
{
	ePinSelectedNone,
	ePinSelectedOne,
}ePinSelection_t;

typedef enum _PullUpPower_e
{
	ePullUpPowerLow,
	ePullUpPowerHigh,
}ePullUpPower_t;

typedef enum _PassiveFilter_e
{
	ePassiveFilterOff,
	ePassiveFilterOn,
}ePassiveFilter_t;

typedef enum _ActiveFilter_e
{
	eActiveFilterOff,
	eActiveFilterOn,
}eActiveFilter_t;

typedef struct _InputPinOperation_s
{
	ePinSelection_t PinSelection;
	ePullUpPower_t PullUpPower;
	ePassiveFilter_t PassiveFilter;
	ePassiveFilter_t ActiveFilter;
}sInputPinOperation_t;

/******************************************************************************
 * Private/Class Variables
 *****************************************************************************/
TaskHandle_t  tl_GPIOTaskHandlerId = NULL;
eGPIOSelfTest_t tl_GPIOSelfTestInOperationFlag = eGpioSelfTestNotTriggered;

sInputPinOperation_t tl_CurrentInputPin = {
		.PinSelection = ePinSelectedOne,
		.PullUpPower=ePullUpPowerHigh,
		.PassiveFilter=ePassiveFilterOn,
		.ActiveFilter=eActiveFilterOff};

/*******************************************************************************
 * Private/Protected Methods
 ******************************************************************************/
/*!
 * @brief Interrupt service fuction of switch.
 *
 * This function toggles the LED
 */
void PORTC_IRQHandler(void)
{
	uint32_t irqPinState;

	irqPinState = GPIO_PortGetInterruptFlags(BOARD_TestC1_GPIO);

	if( irqPinState & (1U << BOARD_TestC1_PIN))
	{
//		ts_IrqCount++;
    /* Clear external interrupt flag. */
		GPIO_PortClearInterruptFlags(BOARD_TestC1_GPIO, 1U << BOARD_TestC1_PIN);
		ts_Pin1TestCount++;
	}
    /* Change state of button. */
    tl_ButtonPressed = true;
}


/**
 * @brief set the input pin
 */
uint32_t setInputPin(uint32_t Pin)
{

	tl_CurrentInputPin.PinSelection = ePinSelectedOne;
	tl_ChangeConfigurationFlag = 1;
	return tl_CurrentInputPin.PinSelection = ePinSelectedOne;
}

/**
 * @brief modify the Pullup power selection
 */
uint32_t setPullupPowerLevel(uint32_t PowerLevel)
{
	uint32_t powerLevel = 0;
	if(PowerLevel)
	{
		tl_CurrentInputPin.PullUpPower = ePullUpPowerHigh;
		powerLevel = 1;
	}
	else
	{
		tl_CurrentInputPin.PullUpPower = ePullUpPowerLow;
	}

	tl_ChangeConfigurationFlag = 1;
	return powerLevel;
}

/**
 * @brief modify the passive filter selection
 */
uint32_t setPassiveFilter(uint32_t PassiveFilter)
{
	uint32_t passiveFilter= 0;
	if(PassiveFilter)
	{
		tl_CurrentInputPin.PassiveFilter = ePassiveFilterOn;
		passiveFilter = 1;
	}
	else
	{
		tl_CurrentInputPin.PassiveFilter = ePassiveFilterOff;
	}

	tl_ChangeConfigurationFlag = 1;
	return passiveFilter;
}

/**
 * @brief modify the active filter selection
 */
uint32_t setActiveFilter(uint32_t ActiveFilter)
{
	uint32_t activeFilter= 0;
	if(ActiveFilter)
	{
		tl_CurrentInputPin.ActiveFilter = eActiveFilterOn;
		activeFilter = 1;
	}
	else
	{
		tl_CurrentInputPin.ActiveFilter = eActiveFilterOff;
	}

	tl_ChangeConfigurationFlag = 1;
	return activeFilter;
}

/**
 * @brief display current input pin selection settings
 */
void getInputPinConfiguration(char *CurrentConfigurationPtr,size_t *Size)
{
	const char *pinConfigStr1 ="Input Pin: ";
	const char *pinConfigStr2 ="one\r\n";
	const char *pinConfigStr3 ="none\r\n";
	const char *pinConfigStr4 ="Pullup Pwr: ";
	const char *pinConfigStr5 ="high\r\n";
	const char *pinConfigStr6 ="low\r\n";
	const char *pinConfigStr7 ="Passive Filter: ";
	const char *pinConfigStr8 ="on\r\n";
	const char *pinConfigStr9 ="off\r\n";
	const char *pinConfigStr10 ="Active Filter: ";
	const char *pinConfigStr11 ="on\r\n";
	const char *pinConfigStr12 ="off\r\n";
	const char *pinConfigStr13 ="test Count: ";

//	GPIO_TogglePinsOutput(GPIOC,LED2);
//	GPIO_SetPinsOutput(GPIOC,LED1);
	uint16_t currentPin1TestCount = 0;

//	uint16_t pin1TestCount;
	size_t sentSize = 0;
	size_t length;
	currentPin1TestCount = ts_Pin1TestCount;
	char *ptrDetails = CurrentConfigurationPtr;
	length = strlen(pinConfigStr1);
	memcpy(ptrDetails,pinConfigStr1,length);
	ptrDetails += length;
	sentSize += length;
	switch (tl_CurrentInputPin.PinSelection)
	{
	case ePinSelectedOne:
		length = strlen(pinConfigStr2);
		memcpy(ptrDetails,pinConfigStr2,length);
		ptrDetails += length;
		sentSize += length;
		break;
	case ePinSelectedNone:
		length = strlen(pinConfigStr3);
		memcpy(ptrDetails,pinConfigStr3,length);
		ptrDetails += length;
		sentSize += length;
		break;
	}


	// display pullup strength
	length = strlen(pinConfigStr4);
	memcpy(ptrDetails,pinConfigStr4,length);
	ptrDetails += length;
	sentSize += length;
	switch (tl_CurrentInputPin.PullUpPower)
	{
	case ePullUpPowerHigh:
		length = strlen(pinConfigStr5);
		memcpy(ptrDetails,pinConfigStr5,length);
		ptrDetails += length;
		sentSize += length;
		break;
	case ePullUpPowerLow:
		length = strlen(pinConfigStr6);
		memcpy(ptrDetails,pinConfigStr6,length);
		ptrDetails += length;
		sentSize += length;
		break;
	}

	// display passive filter settings
	length = strlen(pinConfigStr7);
	memcpy(ptrDetails,pinConfigStr7,length);
	ptrDetails += length;
	sentSize += length;
	switch (tl_CurrentInputPin.PassiveFilter)
	{
	case ePassiveFilterOn:
		length = strlen(pinConfigStr8);
		memcpy(ptrDetails,pinConfigStr8,length);
		ptrDetails += length;
		sentSize += length;
		break;
	case ePassiveFilterOff:
		length = strlen(pinConfigStr9);
		memcpy(ptrDetails,pinConfigStr9,length);
		ptrDetails += length;
		sentSize += length;
		break;
	}


	// display Active filter settings
	length = strlen(pinConfigStr10);
	memcpy(ptrDetails,pinConfigStr10,length);
	ptrDetails += length;
	sentSize += length;
	switch (tl_CurrentInputPin.ActiveFilter)
	{
	case eActiveFilterOn:
		length = strlen(pinConfigStr11);
		memcpy(ptrDetails,pinConfigStr11,length);
		ptrDetails += length;
		sentSize += length;
		break;
	case eActiveFilterOff:
		length = strlen(pinConfigStr12);
		memcpy(ptrDetails,pinConfigStr12,length);
		ptrDetails += length;
		sentSize += length;
		break;
	}

	// display test count settings
	length = strlen(pinConfigStr13);
	memcpy(ptrDetails,pinConfigStr13,length);
	ptrDetails += length;
	sentSize += length;

	length = sprintf(ptrDetails,"%d\r\n",currentPin1TestCount);
	ptrDetails += length;
	sentSize += length;

	*Size = sentSize;
}

/**
 * @brief clear the interrupt test count
 */
void gpioClearTestCount()
{
	//PRINTF("clear ts_Pin1TestCount");
	ts_Pin1TestCount = 0;
}

/**
 * Energize the Trip Relay
 */
inline static void EnergizeTripRelay()
{
	GPIO_PortSet(BOARD_RELAY_TRIP_GPIO,1<<BOARD_RELAY_TRIP_PIN);
}

/**
 * Release the Trip Relay
 */
inline static void ReleaseTripRelay()
{
	GPIO_PortClear(BOARD_RELAY_TRIP_GPIO,1<<BOARD_RELAY_TRIP_PIN);
}

/**
 * Energize the NGR Relay
 */
inline static void EnergizeNGRRelay()
{
	GPIO_PortSet(BOARD_RELAY_NGR_GPIO,1<<BOARD_RELAY_NGR_PIN);
}

/**
 * Release the NGR Relay
 */
inline static void ReleaseNGRRelay()
{
	GPIO_PortClear(BOARD_RELAY_NGR_GPIO,1<<BOARD_RELAY_NGR_PIN);
}

/**
 * Energize the GND Relay
 */
inline static void EnergizeGNDRelay()
{
	GPIO_PortSet(BOARD_RELAY_GND_GPIO,1<<BOARD_RELAY_GND_PIN);
}

/**
 * Release the GND Relay
 */
inline static void ReleaseGNDRelay()
{
	GPIO_PortClear(BOARD_RELAY_GND_GPIO,1<<BOARD_RELAY_GND_PIN);
}

inline static void EnergizeAUXRelay()
{
	GPIO_PortSet(BOARD_RELAY_AUX_GPIO,1<<BOARD_RELAY_AUX_PIN);
}

inline static void ReleaseAUXRelay()
{
	GPIO_PortClear(BOARD_RELAY_AUX_GPIO,1<<BOARD_RELAY_AUX_PIN);
}

static void GpioTask(void *pvParameters)
{
	uint32_t notifyBits;
	BaseType_t xResult;
	const TickType_t xMaxBlockTime = portMAX_DELAY ;
	PRINTF("Gpio Task started\r\n");
//    PRINTF("Gpio Task Started\r\n");
	char relayTestResultsMessage[64];
	int32_t strLength;

	while(1)
	{
		xResult = xTaskNotifyWait( 0x00,    /* Don't clear bits on entry. */
				0xFFFFFFFF,        /* Clear all bits on exit. */
				&notifyBits, /* Stores the notified value. */
				xMaxBlockTime );

		if( xResult == pdPASS )
		{
			if(notifyBits & PT_RELAYS_ON)
			{
//				ptEnergizeAllRelays();
				EnergizeTripRelay();
				EnergizeAUXRelay();
				EnergizeGNDRelay();
				EnergizeNGRRelay();


				PRINTF("All relays energized\n");
				strLength = sprintf(relayTestResultsMessage,"All relays energized\r\n");
				UART_WriteBlocking(DEBUG_UART, (uint8_t *)relayTestResultsMessage, strLength);
//				updatePTReg(ePtResultReg1, ALL_RELAYS_ENERGIZED);
//				updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);
			}
			if(notifyBits & PT_RELAYS_OFF)
			{
//				ptReleaseAllRelays();
				ReleaseTripRelay();
				ReleaseAUXRelay();
				ReleaseGNDRelay();
				ReleaseNGRRelay();
				PRINTF("All relays released\n");
				strLength = sprintf(relayTestResultsMessage,"All relays released\r\n");
				UART_WriteBlocking(DEBUG_UART, (uint8_t *)relayTestResultsMessage, strLength);
//				updatePTReg(ePtResultReg1, ALL_RELAYS_ENERGIZED);
//				updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);
			}
			if(notifyBits & PT_TEST_DIGITAL_IO)
			{
//				updatePTReg(ePtResultReg1,GPIO_ReadPinInput(BOARD_RESET_P_GPIO,BOARD_RESET_P_PIN) );
//				updatePTReg(ePtResultReg2,GPIO_ReadPinInput(BOARD_TEST_P_GPIO,BOARD_TEST_P_PIN) );
//				updatePTReg(ePtResultReg3,GPIO_ReadPinInput(BOARD_WIFI_ENABLE_GPIO,BOARD_WIFI_ENABLE_PIN));
//				updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);   // update test register to indicate operation complete
			}
			if(notifyBits & RELAY_TRIP_ON)
			{
				PRINTF("Trip Relay ON\n");
				EnergizeTripRelay();
			}
			if(notifyBits & RELAY_TRIP_OFF)
			{
				PRINTF("Trip Relay OFF\n");
				ReleaseTripRelay();
			}
			if(notifyBits & RELAY_NGR_ON)
			{
				PRINTF("NGR Relay ON\n");
				EnergizeNGRRelay();
			}
			if(notifyBits & RELAY_NGR_OFF)
			{
				PRINTF("NGR Relay OFF\n");
				ReleaseNGRRelay();
			}
			if(notifyBits & RELAY_GND_ON)
			{
				PRINTF("GND Relay ON\n");
				EnergizeGNDRelay();
			}
			if(notifyBits & RELAY_GND_OFF)
			{
				PRINTF("GND Relay OFF\n");
				ReleaseGNDRelay();
			}
			if(notifyBits & RELAY_AUX_ON)
			{
				PRINTF("AUX Relay ON\n");
				EnergizeAUXRelay();
			}
			if(notifyBits & RELAY_AUX_OFF)
			{
				PRINTF("AUX Relay OFF\n");
				ReleaseAUXRelay();
			}
			/**
			 * @todo these tests are only for bringup testing and the code can be removed after that
			 * need to verify that the reset and test pins are writing and reading
			 * These pin will then be used by the self test as soon as they are known to be working
			 */
			if(notifyBits & RESET_ON)
			{
				PRINTF("reset high\n");
				//ConfigureGpioSelfTest(1);
				GPIO_PortSet(BOARD_RESET_P_GPIO,BOARD_RESET_P_PIN);
				vTaskDelay(1); // short delay to allow pin to settle, should not be needed
				if( GPIO_PinRead(BOARD_RESET_P_GPIO,BOARD_RESET_P_PIN) )
				{
					PRINTF("test pin high\n");
				}
				else
				{
					PRINTF("test pin low\n");
				}
				if( GPIO_PinRead(BOARD_WIFI_ENABLE_GPIO,BOARD_WIFI_ENABLE_PIN) )
				{
					PRINTF("wifi pin high\n");
				}
				else
				{
					PRINTF("wifi pin low\n");
				}
				//ConfigureGpioSelfTest(0);
			}
			if(notifyBits & RESET_OFF)
			{
				PRINTF("reset high\n");
				//ConfigureGpioSelfTest(1);

				GPIO_PortSet(BOARD_RESET_P_GPIO,BOARD_RESET_P_PIN);
				vTaskDelay(1); // short delay to allow pin to settle, should not be needed
				if( GPIO_PinRead(BOARD_RESET_P_GPIO,BOARD_RESET_P_PIN) )
				{
					PRINTF("test pin high\n");
				}
				else
				{
					PRINTF("test pin low\n");
				}
				if( GPIO_PinRead(BOARD_WIFI_ENABLE_GPIO,BOARD_WIFI_ENABLE_PIN) )
				{
					PRINTF("wifi pin high\n");
				}
				else
				{
					PRINTF("wifi pin low\n");
				}
				//ConfigureGpioSelfTest(0);
			}
		}
	}
}


/*******************************************************************************
 * Public Methods
 ******************************************************************************/

TaskHandle_t GetGPIOTaskHandle()
{
	return tl_GPIOTaskHandlerId;
}

/**
 * Return the GPIO Self Test results
 */
eGPIOSelfTest_t GpioSelfTestResults()
{
	eGPIOSelfTest_t results;
	/*
	 * place check here to determine if a selftest has been triggered.  if so
	 * determine if the test has completed, if so release the task for normal
	 * operation
	 */
	if(	tl_GPIOSelfTestInOperationFlag == eGpioSelfTestTestPending )
	{
		tl_GPIOSelfTestInOperationFlag = eGpioSelfTestCompleted;
		results = tl_GPIOSelfTestInOperationFlag;
	}
	else
	{
		/*
		 * @todo test is in operation, will need to place timeout condition
		 * here to fault is the test does not complete
		 */
		results = -1;
	}

	return results;
}

void triggerReleaseAllRelays()
{
	xTaskNotify(GetGPIOTaskHandle(),PT_RELAYS_OFF,eSetBits);
}

void triggerEnergizeAllRelays()
{
	xTaskNotify(GetGPIOTaskHandle(),PT_RELAYS_ON,eSetBits);
}

void StartGpioTask()
{
	xTaskCreate(GpioTask, GpioTaskName, GPIO_TASK_STACK_SIZE , NULL, GPIO_TASK_PRIORITY, &tl_GPIOTaskHandlerId);

}


