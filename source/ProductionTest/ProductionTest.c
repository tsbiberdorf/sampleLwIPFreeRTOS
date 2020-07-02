/**
 * @file ProductionTest.c
 *
 * @brief internal logic for the API that executes Production Test commands.
 *
 * @details This logic is triggered by a tester when the unit is going through
 * the manufacturing floor. These will check various aspects of the board.
 *
 * All voltages and items that make sense to do so, will be implemented in such
 *  a way so that it is the testing unit's responsibility to pass or fail the
 * device on a particular test. The Relay unit will post value(s) into modbus
 * response registers for the tester to use to determine pass or fails.
 *
 * @see ProductionTest.h for the public facing API calls
 * @ingroup ProductionTestGroup
 */

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include "TaskParameters.h"
#include "fsl_gpio.h"
#include "fsl_uart.h"
#include "pin_mux.h"
#include "drivers/fsl_adc16.h"
#include "fsl_debug_console.h"
#include "Peripheral/RS485/RS485HMI.h"
#include "Peripheral/RS485/RS485Sensor.h"
#include "Memory/LocalModbusMemory.h"
#include "LED/LEDOperations.h"
#include "Peripheral/GPIO/GpiocOperations.h"
#include "ProductionTest.h"

/******************************************************************************
 * private defines/macros ====================================================
 *****************************************************************************/

#define PASS_WIFI            (0) /** !< value saved into modbus memory to indicate wifi test passed */
#define FAIL_WIFI            (1) /** !< value saved into modbus memory to indicate wifi test failed */

/******************************************************************************
 * private variables ==========================================================
 *****************************************************************************/

/******************************************************************************
 * Private Methods ============================================================
 ******************************************************************************/
/**
 * @brief Notify GPIO Task to start the digital input test
 */
static inline void TriggerDigitalInputTest(){
	xTaskNotify(GetGPIOTaskHandle(),PT_TEST_DIGITAL_IO,eSetBits);
}

/**
 * @brief Notify LED task to start the LED Test
 */
static inline void TriggerLEDProductionTest()
{
	xTaskNotify(GetLedTaskHandle(),LED_TEST_ON,eSetBits);
}

/**
 * @brief Notify the LED task to test the 3.3v adc value
 */
static inline void Trigger3vProductionTest()
{
	xTaskNotify(GetLedTaskHandle(),V33_TEST_ON,eSetBits);
}
/**
 * @brief Function to test the wifi module communication
 * @todo insert code needed to properly set up the wifi module for testing
 */
void TestWifiCmd()
{
	updatePTReg(ePtResultReg1, FAIL_WIFI);
}

extern uint16_t readSingleMbReg(int index); //this is only used for printing return value purposes @todo remove after debug


/******************************************************************************
 * Public Methods =============================================================
 ******************************************************************************/

/**
 * @brief function call to trigger a loopback test of the RS485 ports
 * @details ptRs485Ports - Production Test RS485 ports
 * This api can be called from a UART CLI for debug testing or from a MODBUS TCP command.
 * This is a "loopback" test.
 * That is, it requires that the Sensor & HMI ports are directly tied to each other.
 * The Sensor & HMI ports are tested by sending a message out the Sensor port to the HMI port.
 * The HMI port then echos (transmits) the received message back to the Sensor port.
 * Finally, the message received by the Sensor is compared to the message originally sent.
 * NOTE: Each RS485 port has an input and an output FIFO associated with it.
 * Under normal operation we would need to manage these FIFO's to ensure we do not overwrite
 * the buffers. However, this is a simple "one-shot" message to prove the hardware was
 * manufactured properly. The message we are sending is smaller than the size of the FIFO.
 * So, we will not concern ourselves with the overhead of managing the FIFO's.
 *
 * @param SourcePort port to trigger a message send to
 */
uint8_t ptRs485Ports(eRS485LoopbackSource_t SourcePort)
{
	if( SourcePort == eLoopbackSourceSensor)
	{
		/*
		 * the message is to be sent to the HMI port from the sensor port
		 */
		TriggerHmiLoopbackCommand();
	}
	if( SourcePort == eLoopbackSourceHMI)
	{
		/*
		 * the message is to be sent to the sensor port from the HMI port
		 */
		TriggerSensorLoopbackCommand();
	}

	return(0);
}

/**
 * @brief parse function that triggers a production test command to execute
 *
 * @details though able to be called elsewhere it is currently triggered by
 * modbusTCP when a write message to the Modbus Address associated with
 * production test's command register is recieved
 *
 * @see ePTCmds_t for available commands
 */
void executePTCommand(int cmdValue)
{

	//save command value
	updatePTReg(ePtCommandReg, cmdValue);

	//save busy signal in the modbus to indicate operation has not been completed
	updatePTReg(ePtStatusReg, PROD_TEST_BUSY);
	//execute a specific behavior based on what command was specified
	switch(cmdValue)
	{
	case eVerify3VCmd:
		Trigger3vProductionTest();
		PRINTF("result value: %i\r\n", readSingleMbReg(ePtResultReg1));
		break;
	case eReleaseAllRelaysCmd:
		triggerReleaseAllRelays();
		PRINTF("result value: %i\r\n", readSingleMbReg(ePtResultReg1));
		break;
	case eEngergizeAllRelaysCmd:
		triggerEnergizeAllRelays();
		PRINTF("result value: %i\r\n", readSingleMbReg(ePtResultReg1));
		//indicate operation is complete
		updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);
		break;
	case eVerifyLEDsCmd:
		TriggerLEDProductionTest();
		PRINTF("result value: %i\r\n", readSingleMbReg(ePtResultReg1));
		break;
	case eVerifyDigitalInputsCmd:
		TriggerDigitalInputTest();
		PRINTF("result value: %i\r\n", readSingleMbReg(ePtResultReg1));
		break;
	case eVerifyRS485CmdSensor:
		ptRs485Ports(eLoopbackSourceSensor);
		break;

	case eVerifyRS485CmdHMI:
		ptRs485Ports(eLoopbackSourceHMI);
		break;

	case eVerifyWifiCmd:
		TestWifiCmd();
		PRINTF("result value: %i\r\n", readSingleMbReg(ePtResultReg1));
		//indicate operation is complete
		updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);
		break;
	default:
		//unknown command do nothing
		break;
	}

}
