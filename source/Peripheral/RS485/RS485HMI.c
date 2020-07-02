/**
 * @ingroup groupHMI485
 * {
 * @file RS485HMI.c
 * @test
 * Connect a UART converter to pins Rx 90 PTC16, TX 91 PTC17
 *
 * @details
 * Full implementation of RS485 HMI side of communication
 *
 *
 * Send data package over the UART pins.  The HMI IRQ will copy the data
 * from an input buffer to an output buffer.  The CLI command is needed to
 * empty the output buffer.
 *
 * The CLI commands to test this module would be:
 * rs485 -hsel 1 to set high the sensor DIR pin 92, PTC18
 * rs485 -htrx 0 will empty the sensor UART receive buffer
 *
 * @see RS485HMI.h for public implementation
 * @todo Need to develop the RS485 protocol to manage the RS485 communication
 * and half duplex control.
 *
 *
 */

//#include <RTT/SEGGER_RTT.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
//#include "MK60D10.h"
#include "fsl_uart.h"
#include "TaskParameters.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "fsl_crc.h"
#include "board.h"
#include "pin_mux.h"
#include "RS485HMI.h"
#include "RS485Common.h"
#include "Memory/LocalModbusMemory.h"
#include "ProductionTest/ProductionTest.h"
#include "NGRMRelay.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* UART instance and clock */
#define HMI_UART UART3
#define HMI_UART_CLKSRC kCLOCK_BusClk
#define HMI_UART_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#define HMI_UART_IRQn UART3_RX_TX_IRQn
#define HMI_UART_IRQHANDLER UART3_RX_TX_IRQHandler
#define BOARD_HMI_UART_BAUDRATE 115200

/*! @brief Ring buffer size (Unit: Byte). */
#define HMI_RING_BUFFER_SIZE (256)

/*! @brief Ring buffer to save received data. */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
typedef enum _SensorSelfTest_e
{
	eSelfTestIdle = 0,
	eSelfTestBusy,
	eSelfTestResultsReady
}eSensorSelfTest_t;


/*******************************************************************************
 * Variables
 ******************************************************************************/
/*
  Ring buffer for data input and output, in this example, input data are saved
  to ring buffer in IRQ handler. The main function polls the ring buffer status,
  if there are new data, then send them out.
  Ring buffer full: (((rxIndex + 1) % DEMO_RING_BUFFER_SIZE) == txIndex)
  Ring buffer empty: (rxIndex == txIndex)
 */
uint8_t tl_HmiInRingBuffer[HMI_RING_BUFFER_SIZE];
volatile uint16_t tl_HmiInRingEndIdx=0; /* Index of the data to send out. */
volatile uint16_t tl_HmiInRingStartIdx=0; /* Index of the memory to save new arrived data. */

uint8_t tl_HmiOutRingBuffer[HMI_RING_BUFFER_SIZE];
volatile uint16_t tl_HmiOutRingEndIdx=0; /* Index of the data to send out. */
volatile uint16_t tl_HmiOutRingStartIdx=0; /* Index of the memory to save new arrived data. */

uint8_t tl_HmiBuffer[HMI_RING_BUFFER_SIZE];
static sRS485Cmd_t tl_Hmi485;

static eSensorSelfTest_t tl_HmiSelfTestFlag = eSelfTestIdle;
static int32_t tl_HmiSelfTestResults = 1;
static SemaphoreHandle_t tl_Hmi485SendMsgSem = NULL;
static eRS485LoopBackTest_t tl_HmiLoopbackVerifyState = e485LBNotStarted;

/*******************************************************************************
 * Code
 ******************************************************************************/

/**
 * @details overwitten interrupt handler for HMI communication
 */
void HMI_UART_IRQHANDLER(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint8_t data;
	/* If new data arrived. */
	if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(HMI_UART))
	{
		data = UART_ReadByte(HMI_UART);

		/* If ring buffer is not full, add data to ring buffer. */
		{
			tl_HmiInRingBuffer[tl_HmiInRingStartIdx] = data;
			tl_HmiInRingStartIdx++;
			tl_HmiInRingStartIdx %= HMI_RING_BUFFER_SIZE;

//			PRINTF("h:%x\n",data);
			xTaskNotifyFromISR(GetHMITaskHandle(),HMI_RX_DATA,eSetBits,&xHigherPriorityTaskWoken);
		}
	}
}


TaskHandle_t  tl_RS485HMITaskHandlerId = NULL;
const char *RS485HMITaskName = "HMI";


/**
 * @brief this method will now place the HMI RS485 port to receive data
 */
static void ReceiveDataMode()
{
//	PRINTF("in HMI_RX_MODE\r\n");
	GPIO_PortClear(BOARD_HMI_RS485_DIR_GPIO,1<<BOARD_HMI_RS485_DIR_PIN);
}

/**
 * @brief this method will now place the HMI RS485 port to transmit data
 */
static void TransmitDataMode()
{
//	PRINTF("in HMI_TX_MODE\r\n");
	GPIO_PortSet(BOARD_HMI_RS485_DIR_GPIO,1<<BOARD_HMI_RS485_DIR_PIN);
}

/**
 * @brief return entry from In Ring Buffer
 */
static uint8_t PopInRingBuffer()
{
	uint8_t data = tl_HmiInRingBuffer[tl_HmiInRingEndIdx];
	tl_HmiInRingEndIdx++;
	tl_HmiInRingEndIdx %= HMI_RING_BUFFER_SIZE;
	return data;
}



/**
 * @brief Process the RS485 command received into the HMI port
 *
 * @details Verify if the command received has a valid CRC and to process
 * what should be done.
 * Current process actions are for:
 * <ol>
 * <li> Loop Back commands from the Sensor port
 * (/ol>
 *
 * @return void
 */
static void Process485Cmd()
{
	uint16_t idx;


	if(tl_Hmi485.Cmd == e485CmdLoopback)
	{
		vTaskDelay(100); // need delay to allow the Sensor port to return to RX mode to see this reply
		if( VerifyCmdCRC(&tl_Hmi485) )
		{
			PRINTF(" RS485HMI Process485Cmd CRC failure Cmd:%x CRC:%x\r\n",tl_Hmi485.Cmd,tl_Hmi485.uCRC_t.CRC);
			seggerPrintRS485(&tl_Hmi485);
		}

		tl_Hmi485.PortId = SENSOR_RS485_ID;
		tl_Hmi485.Cmd = e485CmdLoopbackAck;

		tl_Hmi485.uCRC_t.CRC = 0;// set CRC to zero so the VerifyCmdCRC will return a new CRC value.
		tl_Hmi485.uCRC_t.CRC = VerifyCmdCRC(&tl_Hmi485); // generate the CRC for this transfer

	    tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = tl_Hmi485.PortId;
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;

	    tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = tl_Hmi485.Cmd;
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;

		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = tl_Hmi485.uSize_t.sizeInBytes[0];
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;
		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = tl_Hmi485.uSize_t.sizeInBytes[1];
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;

		for(idx=0; idx< tl_Hmi485.uSize_t.size; idx++)
		{
			tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = tl_Hmi485.Payload[idx];
			tl_HmiOutRingStartIdx++;
			tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;
		}

		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = tl_Hmi485.uCRC_t.crcInBytes[0];
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;
		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = tl_Hmi485.uCRC_t.crcInBytes[1];
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;

		PRINTF("HMI loopback Cmd:%x CRC:%x\r\n",tl_Hmi485.Cmd,tl_Hmi485.uCRC_t.CRC);
		seggerPrintRS485(&tl_Hmi485);

		xTaskNotify(GetHMITaskHandle(),HMI_TX_DATA,eSetBits);
	}
	else if(tl_Hmi485.Cmd == e485CmdLoopbackAck)
	{
		if( VerifyCmdCRC(&tl_Hmi485) )
		{
			tl_HmiLoopbackVerifyState = e485LBAck;
			PRINTF(" RS485HMI Process485Cmd CRC failure Cmd:%x CRC:%x\r\n",tl_Hmi485.Cmd,tl_Hmi485.uCRC_t.CRC);
		}
		else
		{
			PRINTF("Loopback success Cmd:%x CRC:%x\r\n",tl_Hmi485.Cmd,tl_Hmi485.uCRC_t.CRC);
			tl_HmiLoopbackVerifyState = e485LBVerified;
		}
		seggerPrintRS485(&tl_Hmi485);
		/*
		 * make call to the Modubus coded to update the status of the test.
		 * write the results
		 * then update the status to indicate the test has been completed
		 */
		updatePTReg(ePtResultReg1, tl_HmiLoopbackVerifyState);// pass the results of the test into this register.
		updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);
	}
	else
	{
		if( VerifyCmdCRC(&tl_Hmi485) )
		{
			PRINTF(" RS485HMI Process485Cmd CRC failure Cmd:%x CRC:%x\r\n",tl_Hmi485.Cmd,tl_Hmi485.uCRC_t.CRC);
		}

	}
}

/**
 * @brief Main RS485 HMI Task handling
 *
 * @details This task is in charge of sending/receiving/processing messages related to
 * the HMI sensor.

 */
static void RS485HMITask(void *pvParameters)
{
	uint32_t notifyBits;
	BaseType_t xResult;
	TickType_t hmiTaskWaitTime = portMAX_DELAY ;
	int32_t idx;

	PRINTF("RS485 HMI Task started\r\n");
	tl_Hmi485.Payload = tl_HmiBuffer;
	tl_Hmi485.State = e485Idle;

	while(1)
	{
		xResult = xTaskNotifyWait( 0x00,    /* Don't clear bits on entry. */
				0xFFFFFFFF,        /* Clear all bits on exit. */
				&notifyBits, /* Stores the notified value. */
				hmiTaskWaitTime );
		if( xResult == pdPASS )
		{
			if( notifyBits & HMI_RX_DATA)
			{
//				PRINTF("HMI_RX_DATA %d\r\n",tl_Hmi485.State);
				ReceiveDataMode();

				switch(tl_Hmi485.State)
				{
				case e485Idle:
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;
					tl_Hmi485.PortId = PopInRingBuffer();
					if( tl_Hmi485.PortId == HMI_RS485_ID)
					{
//						PRINTF("HMI Port Id %d\r\n",tl_HmiInRingEndIdx);
						// data is directed at the HMI port
						tl_Hmi485.State = e485Cmd;
						hmiTaskWaitTime = RS485_TIMEOUT_CNT; // shorten time between each byte for timeout
					}
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;

				case e485Cmd:
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;
					tl_Hmi485.Cmd = PopInRingBuffer();
//					PRINTF("HMI Cmd %x\r\n",tl_Hmi485.Cmd);
					tl_Hmi485.State = e485Size0;
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;

				case e485Size0:
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;
					tl_Hmi485.uSize_t.sizeInBytes[0] = PopInRingBuffer();
					tl_Hmi485.State = e485Size1;
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;

				case e485Size1:
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;
					tl_Hmi485.uSize_t.sizeInBytes[1] = PopInRingBuffer();
//					PRINTF("HMI Size %d\r\n",tl_Hmi485.uSize_t.size);

					tl_Hmi485.State = e485Data;
					idx = 0;
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;

				case e485Data:
					while( tl_HmiInRingStartIdx != tl_HmiInRingEndIdx && (tl_Hmi485.State==e485Data))
					{
						if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
							break;
						tl_Hmi485.Payload[idx++] = PopInRingBuffer();
//						PRINTF("HMI data %d %x %d\r\n",idx,tl_Hmi485.Payload[idx-1],tl_HmiInRingEndIdx);
						if(idx == tl_Hmi485.uSize_t.size)
						{
							// add data has been received
							tl_Hmi485.State = e485CRC0;
						}
					}
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;

				case e485CRC0:
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;
					tl_Hmi485.uCRC_t.crcInBytes[0] = PopInRingBuffer();
//					PRINTF("HMI crc0 %x\r\n",tl_Hmi485.uCRC_t.crcInBytes[0]);
					tl_Hmi485.State = e485CRC1;
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;
				case e485CRC1:
					if( tl_HmiInRingStartIdx == tl_HmiInRingEndIdx )
						break;
					tl_Hmi485.uCRC_t.crcInBytes[1] = PopInRingBuffer();
//					PRINTF("HMI crc1 %x\r\n",tl_Hmi485.uCRC_t.crcInBytes[1]);
					tl_Hmi485.State = e485Idle;

//					PRINTF("HMI RX Cmd:%x CRC:%x\r\n",tl_Hmi485.Cmd,tl_Hmi485.uCRC_t.CRC);

					Process485Cmd();
					hmiTaskWaitTime = portMAX_DELAY;

					break;
				}
			}
			if( notifyBits & HMI_TX_ACK)
			{
				hmiTaskWaitTime = RS485_TIMEOUT_CNT; // delay for a ACK
			}

			if( notifyBits & HMI_TX_DATA)
			{
				PRINTF("HMI_RX_DATA\r\n");
				TransmitDataMode();
				while(tl_HmiOutRingStartIdx != tl_HmiOutRingEndIdx)
				{
					if ((kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(HMI_UART)) && (tl_HmiOutRingStartIdx != tl_HmiOutRingEndIdx))
					{
						// output information to Hmi port
						UART_WriteByte(HMI_UART, tl_HmiOutRingBuffer[tl_HmiOutRingEndIdx]);
//						PRINTF("hw:%x\n",tl_HmiOutRingBuffer[tl_HmiOutRingEndIdx]);

						tl_HmiOutRingEndIdx++;
						tl_HmiOutRingEndIdx %= HMI_RING_BUFFER_SIZE;
					}
				}
				while( !(kUART_TransmissionCompleteFlag & UART_GetStatusFlags(HMI_UART)) )
				{
					; // flush all data
				}
				ReceiveDataMode();

			}
		}
		else
		{
			// timeout happened
			PRINTF( "HMI RS485 timeout in state: %d\r\n",tl_Hmi485.State);
			tl_Hmi485.State = e485Idle;
			hmiTaskWaitTime = portMAX_DELAY;
			ReceiveDataMode();
			if(tl_HmiLoopbackVerifyState == e485LBSent)
			{
				PRINTF( "Loopback test failed \r\n");
				// loopback test failed to return
				// set test as completed.
				updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);
			}
		}

	}
}

/**
 * @details provide results from the HMI Loopback test with HMI port
 *
 * @return test results
 */
int32_t GetHMILoopBackTestResults()
{
	if( tl_HmiSelfTestFlag != eSelfTestResultsReady)
	{
		return NGR_GENERAL_FAILURE;
	}
	else
	{
		tl_HmiSelfTestFlag = eSelfTestIdle;
		return tl_HmiSelfTestResults;
	}
}

/**
 * @brief get hmi task handle
 */
TaskHandle_t GetHMITaskHandle()
{
	return tl_RS485HMITaskHandlerId;
}

/**
 * @brief initalize the RS485HMI Task
 * @details this sould only be called once and it should only be called using extern.
 */
void StartRS485HMITask()
{
	xTaskCreate(RS485HMITask, RS485HMITaskName, RS485HMI_TASK_STACK_SIZE , NULL, RS485HMI_TASK_PRIORITY, &tl_RS485HMITaskHandlerId);
}

/**
 * @brief Send RS485 Hmi Command
 *
 * @param[in] SendHmiCmd pointer to the structure of command to transmit from the HMI RS485 port
 * @return status
 */
int32_t SendHmiTaskCmd(sRS485Cmd_t * SendHmiCmd)
{
	int32_t status;
	if(tl_Hmi485SendMsgSem == NULL)
	{
		tl_Hmi485SendMsgSem = xSemaphoreCreateMutex();
	}

	status = xSemaphoreTake(tl_Hmi485SendMsgSem,1);
	if(status == pdTRUE )
	{
		uint16_t idx;

		SendHmiCmd->uCRC_t.CRC = 0;
		SendHmiCmd->uCRC_t.CRC  = VerifyCmdCRC(SendHmiCmd);

		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = SendHmiCmd->PortId;
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;

		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = SendHmiCmd->Cmd;
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;

		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = SendHmiCmd->uSize_t.sizeInBytes[0];
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;
		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = SendHmiCmd->uSize_t.sizeInBytes[1];
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;

		for(idx=0; idx< SendHmiCmd->uSize_t.size; idx++)
		{
			tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = SendHmiCmd->Payload[idx];
			tl_HmiOutRingStartIdx++;
			tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;
		}


		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = SendHmiCmd->uCRC_t.crcInBytes[0];
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;
		tl_HmiOutRingBuffer[tl_HmiOutRingStartIdx] = SendHmiCmd->uCRC_t.crcInBytes[1];
		tl_HmiOutRingStartIdx++;
		tl_HmiOutRingStartIdx %= HMI_RING_BUFFER_SIZE;

		if( SendHmiCmd->Cmd == e485CmdLoopback)
		{
			xTaskNotify(GetHMITaskHandle(),HMI_TX_DATA|HMI_TX_ACK,eSetBits);
		}
		else
		{
			xTaskNotify(GetHMITaskHandle(),HMI_TX_DATA,eSetBits);
		}

		xSemaphoreGive(tl_Hmi485SendMsgSem);

		PRINTF("Hmi Cmd:%x CRC:%x\r\n",SendHmiCmd->Cmd,SendHmiCmd->uCRC_t.CRC);
	}
	else
	{
		PRINTF("semaphore failure on SendHmiTaskCmd\r\n");
	}
	return status;
}

/**
 * @brief CLI command to check the loop back status
 *
 * @return void
 */
void GetHmiLoopbackStatus()
{
	PRINTF("HMI Loopback status: %d \r\n",tl_HmiLoopbackVerifyState);

}


/**
 * This API will trigger a HMI Loopback command
 */
void TriggerHmiLoopbackCommand()
{
	sRS485Cmd_t rs485Cmd;

	PRINTF("HMI to Sensor loopback %d %d %d\r\n",tl_Hmi485.State
			,tl_HmiInRingStartIdx,tl_HmiInRingEndIdx );

	/*
	 * setup loopback command
	 */
	rs485Cmd.Cmd = e485CmdLoopback;
	rs485Cmd.Payload = (uint8_t *)"HMI RS485 test message";
	rs485Cmd.uSize_t.size = strlen((char *)rs485Cmd.Payload);
	tl_HmiLoopbackVerifyState = e485LBSent;

	/*
	 * the message is to be sent to the Sensor port from the HMI port
	 */
	rs485Cmd.PortId = SENSOR_RS485_ID;

	/*
	 * update ModbusTCP status register
	 */
	updatePTReg(ePtResultReg1, tl_HmiLoopbackVerifyState );
	SendHmiTaskCmd(&rs485Cmd);
}


/**}*/

