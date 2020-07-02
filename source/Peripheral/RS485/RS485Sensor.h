/**
 * @ingroup groupSensor485
 * {
 * @file RS485Sensor.h
 *
 *  Created on: Feb 19, 2020
 *      Author: TBiberdorf
 */

#ifndef RS485_RS485SENSOR_H_
#define RS485_RS485SENSOR_H_

#include "RS485Common.h"

#define SENSOR_TX_MODE (1<<0)
#define SENSOR_RX_MODE (1<<1)
#define SENSOR_RX_DATA (1<<2)   // configure the RS485 Sensor port to receive data
#define SENSOR_TX_DATA (1<<3)   // trigger RS485 Sensor port to transmit data
#define SENSOR_SELFTESTA (1<<4) // send string to HMI port
#define SENSOR_SELFTESTB (1<<5) // receive data back HMI port and verify
#define SENSOR_SELFTESTC (1<<6) // receive data from HMI port
#define SENSOR_SELFTESTD (1<<7) // send data back to HMI port
#define PT_485 			(1<<8) // Production Test RS485 ports
#define SENSOR_TX		(1<<9) // Send Command to Sensor Port
#define SENSOR_TX_ACK	(1<<10) // Send Command should have a ACK


void StartRs485SENSORTask();
TaskHandle_t GetSensorTaskHandle();
int32_t GetSensorLoopBackTestResults();
int32_t SendSensorTaskCmd(sRS485Cmd_t *SendSensorCmd);
void GetSensorLoopbackStatus();
void TriggerSensorLoopbackCommand();


#endif /* RS485_RS485SENSOR_H_ */
/**}*/
