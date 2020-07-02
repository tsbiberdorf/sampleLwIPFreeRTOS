/*
 * @file  ProductionTest.h
 *
 * @brief Public API to interact with the production test options.
 *
 * @see ProductionTest.c for full implementation
 */

#ifndef PT_PRODUCTIONTEST_H_
#define PT_PRODUCTIONTEST_H_

/******************************************************************************
 * private defines/macros
 *****************************************************************************/

/**
 * @brief commands that are available for executing in production test
 * @details this is the value sent from the Tester via modbus to trigger
 * a test action of the Relay Board.
 *
 * @see executePTCommand to see where these commands trigger execution
 */
typedef enum ProductionTestCmds_e{
	eReleaseAllRelaysCmd	= 1,
	eEngergizeAllRelaysCmd,
	eVerify3VCmd,
	eVerifyLEDsCmd,
	eVerifyDigitalInputsCmd,
	eVerifyRS485CmdSensor,
	eVerifyRS485CmdHMI,
	eVerifyWifiCmd,
} ePTCmds_t;



/**
 * @brief Selection between which RS485 loopback test to start loop from
 * @see ptRs485Ports to see a usage of this
 */
typedef enum _RS485LoopbackSource_e
{
	eLoopbackSourceSensor, /**< start loopback test from Sensor port */
	eLoopbackSourceHMI /**< start loopback test from HMI port */
}eRS485LoopbackSource_t;

#define PROD_TEST_BUSY 		(0x0) /*!< indicates that command is being executed */
#define PROD_TEST_COMPLETED (0x1) /*!< indicates the command has been completed and response saved*/
#define ALL_RELAYS_ENERGIZED (302)
#define ALL_RELAYS_RELEASED  (303)
/******************************************************************************
 * function prototypes
 *****************************************************************************/

uint8_t ptRs485Ports(eRS485LoopbackSource_t SourcePort);
void executePTCommand(int cmdValue);

#endif /* PT_PRODUCTIONTEST_H_ */
