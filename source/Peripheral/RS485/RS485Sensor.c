/**
 * @ingroup groupSensor485
 *
 * @file RS485Sensor.c
 *
 * Send data package over the uart pins.  The sensor irq will copy the data
 * from an input buffer to an output buffer.  The CLI command is needed to
 * empty the output buffer.
 *
 * The CLI commands to test this module would be:
 * rs485 -ssel 1 to set hight the sensor DIR pin 93, PTD0
 * rs485 -sstrx 0 will empty the sensor uart recieve buffer
 *
 * @todo Need to developthe RS485 protocol to manage the RS485 communication
 * and half duplex control.
 *{
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
#include "fsl_crc.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "board.h"
#include "pin_mux.h"
#include "Memory/LocalModbusMemory.h"
#include "source/ProductionTest/ProductionTest.h"
#include "Peripheral/CRC/crcInterface.h"
#include "RS485Sensor.h"
#include "RS485Common.h"
#include "NGRMRelay.h"


/*******************************************************************************
 * Definitions   ==============================================================
 ******************************************************************************/

/* UART instance and clock */
#define SENSOR_UART UART2
#define SENSOR_UART_CLKSRC kCLOCK_BusClk
#define SENSOR_UART_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#define SENSOR_UART_IRQn UART2_RX_TX_IRQn
#define SENSOR_UART_IRQHANDLER UART2_RX_TX_IRQHandler
#define BOARD_SENSOR_UART_BAUDRATE 115200
#define DEBUG_UART UART0
#define PASS (1)

/*! @brief Ring buffer size (Unit: Byte). */
#define SENSOR_RING_BUFFER_SIZE (256)

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
uint8_t tl_SensorInRingBuffer[SENSOR_RING_BUFFER_SIZE];
volatile uint16_t tl_SensorInRingEndIdx=0; /* Index of the data to send out. */
volatile uint16_t tl_SensorInRingStartIdx=0; /* Index of the memory to save new arrived data. */

uint8_t tl_SensorOutRingBuffer[SENSOR_RING_BUFFER_SIZE];
volatile uint16_t tl_SensorOutRingEndIdx=0; /* Index of the data to send out. */
volatile uint16_t tl_SensorOutRingStartIdx=0; /* Index of the memory to save new arrived data. */

uint8_t tl_SensorBuffer[SENSOR_RING_BUFFER_SIZE];
static sRS485Cmd_t tl_Sensor485;

static eSensorSelfTest_t tl_SensorSelfTestFlag = eSelfTestIdle;
static int32_t tl_SensorSelfTestResults = 1;
static SemaphoreHandle_t tl_Sensor485SendMsgSem = NULL;

static eRS485LoopBackTest_t tl_SensorLoopbackVerifyState = e485LBNotStarted;

/*******************************************************************************
 * Code
 ******************************************************************************/

void SENSOR_UART_IRQHANDLER(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint8_t data;
	/* If new data arrived. */
	if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(SENSOR_UART))
	{
		data = UART_ReadByte(SENSOR_UART);

		/* If ring buffer is not full, add data to ring buffer. */
		if (((tl_SensorInRingStartIdx + 1) % SENSOR_RING_BUFFER_SIZE) != tl_SensorInRingEndIdx)
		{
			tl_SensorInRingBuffer[tl_SensorInRingStartIdx] = data;
			tl_SensorInRingStartIdx++;
			tl_SensorInRingStartIdx %= SENSOR_RING_BUFFER_SIZE;

//			PRINTF("s:%x\n",data);
			xTaskNotifyFromISR(GetSensorTaskHandle(),SENSOR_RX_DATA,eSetBits,&xHigherPriorityTaskWoken);
		}
	}
}


TaskHandle_t  tl_RS485SensorTaskHandlerId = NULL;
const char *RS485SensorTaskName = "Sensor";


/**
 * this method will now place the sensor RS485 port to receive data
 */
static void ReceiveDataMode()
{
//	PRINTF("in SENSOR_RX_MODE\r\n");
	GPIO_PortClear(BOARD_SENSOR_RS485_DIR_GPIO,1<<BOARD_SENSOR_RS485_DIR_PIN);
}

/**
 * this method will now place the sensor RS485 port to transmit data
 */
static void TransmitDataMode()
{
//	PRINTF("in SENSOR_TX_MODE\r\n");
	GPIO_PortSet(BOARD_SENSOR_RS485_DIR_GPIO,1<<BOARD_SENSOR_RS485_DIR_PIN);
}

/**
 * @brief return entry from In Ring Buffer
 */
static uint8_t PopInRingBuffer()
{
	uint8_t data = tl_SensorInRingBuffer[tl_SensorInRingEndIdx];
	tl_SensorInRingEndIdx++;
	tl_SensorInRingEndIdx %= SENSOR_RING_BUFFER_SIZE;

	return data;
}
/**
 * determine what the RS485 command should do
 */
static void Process485Cmd()
{
	uint16_t idx;


	if(tl_Sensor485.Cmd == e485CmdLoopback)
	{
		vTaskDelay(100); // need delay to allow the HMI port to return to RX mode to see this reply

		if( VerifyCmdCRC(&tl_Sensor485) )
		{
			PRINTF(" RS485HMI Process485Cmd CRC failure Cmd:%x CRC:%x\r\n",tl_Sensor485.Cmd,tl_Sensor485.uCRC_t.CRC);
			seggerPrintRS485(&tl_Sensor485);
		}

		/*
		 * we are going to send the same data and data size back to the HMI port.
		 * need to address the HMI port as well as send the command to loopback ACK
		 */
		tl_Sensor485.PortId = HMI_RS485_ID;
		tl_Sensor485.Cmd = e485CmdLoopbackAck;

		tl_Sensor485.uCRC_t.CRC = 0;// set CRC to zero so the VerifyCmdCRC will return a new CRC value.
		tl_Sensor485.uCRC_t.CRC = VerifyCmdCRC(&tl_Sensor485); // generate the CRC for this transfer

	    tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = tl_Sensor485.PortId;
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;

	    tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = tl_Sensor485.Cmd;
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;

		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = tl_Sensor485.uSize_t.sizeInBytes[0];
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;
		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = tl_Sensor485.uSize_t.sizeInBytes[1];
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;


		for(idx=0; idx< tl_Sensor485.uSize_t.size; idx++)
		{
			tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = tl_Sensor485.Payload[idx];
			tl_SensorOutRingStartIdx++;
			tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;
		}

		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = tl_Sensor485.uCRC_t.crcInBytes[0];
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;
		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = tl_Sensor485.uCRC_t.crcInBytes[1];
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;

		PRINTF("Sensor loopback Cmd:%x CRC:%x\r\n",tl_Sensor485.Cmd,tl_Sensor485.uCRC_t.CRC);
		seggerPrintRS485(&tl_Sensor485);

		xTaskNotify(GetSensorTaskHandle(),SENSOR_TX_DATA,eSetBits);
	}
	else if(tl_Sensor485.Cmd == e485CmdLoopbackAck)
	{
		if( VerifyCmdCRC(&tl_Sensor485) )
		{
			tl_SensorLoopbackVerifyState = e485LBAck;
			PRINTF(" RS485HMI Process485Cmd CRC failure Cmd:%x CRC:%x\r\n",tl_Sensor485.Cmd,tl_Sensor485.uCRC_t.CRC);
		}
		else
		{
			tl_SensorLoopbackVerifyState = e485LBVerified;
		}
		PRINTF("Loopback success Cmd:%x CRC:%x\r\n",tl_Sensor485.Cmd,tl_Sensor485.uCRC_t.CRC);
		seggerPrintRS485(&tl_Sensor485);
		/*
		 * make call to the Modubus coded to update the status of the test.
		 * write the results
		 * then update the status to indicate the test has been completed
		 */
		updatePTReg(ePtResultReg1, tl_SensorLoopbackVerifyState);// pass the results of the test into this register.
		updatePTReg(ePtStatusReg, PROD_TEST_COMPLETED);

	}
	else
	{
		if( VerifyCmdCRC(&tl_Sensor485) )
		{
			PRINTF(" RS485Sensor Process485Cmd CRC failure Cmd:%x CRC:%x\r\n",tl_Sensor485.Cmd,tl_Sensor485.uCRC_t.CRC);
		}

	}
	PRINTF("Sensor Cmd:%x CRC:%x\r\n",tl_Sensor485.Cmd,tl_Sensor485.uCRC_t.CRC);
}

static void RS485SensorTask(void *pvParameters)
{
	uint32_t notifyBits;
	BaseType_t xResult;
	TickType_t sensorTaskWaitTime = portMAX_DELAY ;
	int32_t idx;
	tl_Sensor485.Payload = tl_SensorBuffer;

	PRINTF("RS485 Sensor Task started\r\n");

	while(1)
	{
		xResult = xTaskNotifyWait( 0x00,    /* Don't clear bits on entry. */
				0xFFFFFFFF,        /* Clear all bits on exit. */
				&notifyBits, /* Stores the notified value. */
				sensorTaskWaitTime );
		if( xResult == pdPASS )
		{
			if( notifyBits & SENSOR_RX_DATA)
			{
//				PRINTF("SENSOR_RX_DATA %d\r\n",tl_Sensor485.State);
				ReceiveDataMode();

				switch(tl_Sensor485.State)
				{
				case e485Idle:
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;
					tl_Sensor485.PortId = PopInRingBuffer();
					if( tl_Sensor485.PortId == SENSOR_RS485_ID)
					{
						// data is directed at the SENSOR port
//						PRINTF("Sen Port Id %d\r\n",tl_SensorInRingEndIdx);
						tl_Sensor485.State = e485Cmd;
						sensorTaskWaitTime = RS485_TIMEOUT_CNT; // shorten time between each byte for timeout
					}
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;

				case e485Cmd:
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;
					tl_Sensor485.Cmd = PopInRingBuffer();
//					PRINTF("Sen Cmd %x\r\n",tl_Sensor485.Cmd);
					tl_Sensor485.State = e485Size0;
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;

				case e485Size0:
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;
					tl_Sensor485.uSize_t.sizeInBytes[0] = PopInRingBuffer();
					tl_Sensor485.State = e485Size1;
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;

				case e485Size1:
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;
					tl_Sensor485.uSize_t.sizeInBytes[1] = PopInRingBuffer();
					tl_Sensor485.State = e485Data;
//					PRINTF("Sen Size %d\r\n",tl_Sensor485.uSize_t.size);
					idx = 0;
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;

				case e485Data:
					while( tl_SensorInRingStartIdx != tl_SensorInRingEndIdx && (tl_Sensor485.State == e485Data))
					{
						if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
							break;
						tl_Sensor485.Payload[idx++] = PopInRingBuffer();
//						PRINTF("Sen data %d %x %d\r\n",idx,tl_Sensor485.Payload[idx-1],tl_SensorInRingEndIdx);
						if(idx == tl_Sensor485.uSize_t.size)
						{
							// add data has been received
							tl_Sensor485.State = e485CRC0;
						}
					}
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;

				case e485CRC0:
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;
					tl_Sensor485.uCRC_t.crcInBytes[0] = PopInRingBuffer();
//					PRINTF("Sen crc0 %x\r\n",tl_Sensor485.uCRC_t.crcInBytes[0]);
					tl_Sensor485.State = e485CRC1;
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;
				case e485CRC1:
					if( tl_SensorInRingStartIdx == tl_SensorInRingEndIdx )
						break;
					tl_Sensor485.uCRC_t.crcInBytes[1] = PopInRingBuffer();
//					PRINTF("Sen crc1 %x\r\n",tl_Sensor485.uCRC_t.crcInBytes[1]);
					tl_Sensor485.State = e485Idle;

					PRINTF("Sensor RX Cmd:%x CRC:%x\r\n",tl_Sensor485.Cmd,tl_Sensor485.uCRC_t.CRC);
					Process485Cmd();
					sensorTaskWaitTime = portMAX_DELAY;
					break;
				}

			}
			if( notifyBits & SENSOR_TX_ACK)
			{
				sensorTaskWaitTime = RS485_TIMEOUT_CNT; // delay for a ACK
			}
			if( notifyBits & SENSOR_TX_DATA)
			{
				PRINTF("Tx data\r\n");
				TransmitDataMode();
				while(tl_SensorOutRingStartIdx != tl_SensorOutRingEndIdx)
				{
					if ((kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(SENSOR_UART)) && (tl_SensorOutRingStartIdx != tl_SensorOutRingEndIdx))
					{
						// output information to Sensor port
						UART_WriteByte(SENSOR_UART, tl_SensorOutRingBuffer[tl_SensorOutRingEndIdx]);
//						PRINTF("sw:%x\n",tl_SensorOutRingBuffer[tl_SensorOutRingEndIdx]);

						tl_SensorOutRingEndIdx++;
						tl_SensorOutRingEndIdx %= SENSOR_RING_BUFFER_SIZE;
					}
				}
				while( !(kUART_TransmissionCompleteFlag & UART_GetStatusFlags(SENSOR_UART)) )
				{
					; // flush all data
				}
				ReceiveDataMode();
			}
		}
		else
		{
			// timeout happened
			PRINTF( "Sensor RS485 timeout in state: %d\r\n",tl_Sensor485.State);
			tl_Sensor485.State = e485Idle;
			sensorTaskWaitTime = portMAX_DELAY;
			ReceiveDataMode();
			if(tl_SensorLoopbackVerifyState == e485LBSent)
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
 * @details provide results from the Sensor Loopback test with HMI port
 *
 * @return test resuls
 */
int32_t GetSensorLoopBackTestResults()
{
	if( tl_SensorSelfTestFlag != eSelfTestResultsReady)
	{
		return NGR_GENERAL_FAILURE; // @todo should make list of more detailed errors somewhere
	}
	else
	{
		tl_SensorSelfTestFlag = eSelfTestIdle;
		return tl_SensorSelfTestResults;
	}
}

/**
 * @brief Get Sensor Task Handle
 *
 * @return tl_RS485SensorTaskHandlerId
 */
TaskHandle_t GetSensorTaskHandle()
{
	return tl_RS485SensorTaskHandlerId;
}

/**
 * @brief Start RS485 Sensor port task
 * @return void
 */
void StartRS485SensorTask()
{
	xTaskCreate(RS485SensorTask, RS485SensorTaskName, RS485Sensor_TASK_STACK_SIZE , NULL, RS485Sensor_TASK_PRIORITY, &tl_RS485SensorTaskHandlerId);
}


/**
 * @brief Send RS485 Sensor Command
 *
 * @param SendSensorCmd
 * @return status
 */
int32_t SendSensorTaskCmd(sRS485Cmd_t *SendSensorCmd)
{
	int32_t status;
	if(tl_Sensor485SendMsgSem == NULL)
	{
		tl_Sensor485SendMsgSem = xSemaphoreCreateMutex();
	}

	status = xSemaphoreTake(tl_Sensor485SendMsgSem,1);
	if(status == pdTRUE )
	{
		uint16_t idx;

		SendSensorCmd->uCRC_t.CRC = 0;
		SendSensorCmd->uCRC_t.CRC  = VerifyCmdCRC(SendSensorCmd);

		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = SendSensorCmd->PortId;
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;

		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = SendSensorCmd->Cmd;
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;

		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = SendSensorCmd->uSize_t.sizeInBytes[0];
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;
		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = SendSensorCmd->uSize_t.sizeInBytes[1];
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;

		for(idx=0; idx< SendSensorCmd->uSize_t.size; idx++)
		{
			tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = SendSensorCmd->Payload[idx];
			tl_SensorOutRingStartIdx++;
			tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;
		}


		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = SendSensorCmd->uCRC_t.crcInBytes[0];
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;
		tl_SensorOutRingBuffer[tl_SensorOutRingStartIdx] = SendSensorCmd->uCRC_t.crcInBytes[1];
		tl_SensorOutRingStartIdx++;
		tl_SensorOutRingStartIdx %= SENSOR_RING_BUFFER_SIZE;

		if( SendSensorCmd->Cmd == e485CmdLoopback)
		{
			xTaskNotify(GetSensorTaskHandle(),SENSOR_TX_DATA|SENSOR_TX_ACK,eSetBits);
		}
		else
		{
			xTaskNotify(GetSensorTaskHandle(),SENSOR_TX_DATA,eSetBits);
		}
		xSemaphoreGive(tl_Sensor485SendMsgSem);

		PRINTF("Sensor Cmd:%x CRC:%x\r\n",SendSensorCmd->Cmd,SendSensorCmd->uCRC_t.CRC);
	}
	else
	{
		PRINTF("semaphore failure on SendSensorTaskCmd\r\n");
	}
	return status;
}


/**
 * @brief CLI command to check the loop back status
 *
 * @return void
 */
void GetSensorLoopbackStatus()
{
	PRINTF("Sensor Loopback status: %d \r\n",tl_SensorLoopbackVerifyState);
}

/**
 * This API will trigger a Sensor Loopback command
 */
void TriggerSensorLoopbackCommand()
{
	sRS485Cmd_t rs485Cmd;

	PRINTF("Sensor to HMI loopback %d %d %d\r\n",tl_Sensor485.State
			,tl_SensorInRingStartIdx,tl_SensorInRingEndIdx );

	/*
	 * setup loopback command
	 */
	rs485Cmd.Cmd = e485CmdLoopback;
	rs485Cmd.Payload = (uint8_t *)"Sensor RS485 test message";
	rs485Cmd.uSize_t.size = strlen((char *)rs485Cmd.Payload);
	tl_SensorLoopbackVerifyState = e485LBSent;

	/*
	 * the message is to be sent to the HMI port from the Sensor port
	 */
	rs485Cmd.PortId = HMI_RS485_ID;

	/*
	 * update ModbusTCP status register
	 */
	updatePTReg(ePtResultReg1, tl_SensorLoopbackVerifyState);
	SendSensorTaskCmd(&rs485Cmd);
}

/**}*/
