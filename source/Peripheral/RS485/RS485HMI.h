/**
 * @ingroup groupHMI485
 * {
 * @file RS485HMI.h
 *
 * @brief public API for interacting with RS485 HMI specific communication
 */

#ifndef RS485_RS485HMI_H_
#define RS485_RS485HMI_H_

#include "RS485Common.h"

#define HMI_TX_MODE (1<<0)
#define HMI_RX_MODE (1<<1)
#define HMI_RX_DATA (1<<2)
#define HMI_TX_DATA (1<<3)
#define HMI_SELFTESTA (1<<4) /**< receive data from Sensor Port */
#define HMI_SELFTESTB (1<<5) /**< echo data back to Sensor Port */
#define HMI_SELFTESTC (1<<6) /**< send test data to Sensor Port */
#define HMI_SELFTESTD (1<<7) /**< receive test data back from Sensor Port and verify */
#define HMI_TX_ACK (1<<8)

void StartRs485HMITask();
TaskHandle_t GetHMITaskHandle();
int32_t GetHMILoopBackTestResults();
void GetHmiLoopbackStatus();
int32_t SendHmiTaskCmd(sRS485Cmd_t * SendHmiCmd);
void TriggerHmiLoopbackCommand();

#endif /* RS485_RS485HMI_H_ */

/**}*/
