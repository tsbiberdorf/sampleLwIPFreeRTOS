/**
 * @file TaskParameters.h
 * @brief definitions related to task priority and size
 */

#ifndef TASKPARAMETERS_H_
#define TASKPARAMETERS_H_



// Debug Task
#define DEBUG_TASK_PRIORITY (configMAX_PRIORITIES - 2)
#define DEBUG_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE*3 + 10)


// GPIO Task
#define GPIO_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define GPIO_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE*2 + 10)


// LED Task
#define LED_TASK_PRIORITY (configMAX_PRIORITIES - 3)
#define LED_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE*2 + 10)

// RS885 HMI Task
#define RS485HMI_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define RS485HMI_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE*2 + 10)

// RS885 Sensor Task
#define RS485Sensor_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define RS485Sensor_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE*2 + 10)

//external flash
#define EXTFLASH_TASK_PRIORITY (configMAX_PRIORITIES - 4)
#define EXTFLASH_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE*3 + 10)//(configMINIMAL_STACK_SIZE *3)

// Network Tasks
#define NET_CABLE_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE*10)
#define NET_CABLE_TASK_PRIORITY (configMAX_PRIORITIES - 4)

#define UDP_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE*10)
#define UDP_TASK_PRIORITY (configMAX_PRIORITIES - 4)

#define HTTPD_TREAD_STACKSIZE (3000 )
#define HTTPD_TREAD_PRIORITY (configMAX_PRIORITIES - 4)

#define TCPIP_THREAD_PRIORITY (configMAX_PRIORITIES - 3)

#define EIP_THREAD_STACKSIZE (configMINIMAL_STACK_SIZE*5 +10)//768

// network cable link task
#define LINKWATCH_THREAD_STACKSIZE 192
#define LINKWATCH_THREAD_PRIO (3)//(configMAX_PRIORITIES - configMAX_PRIORITIES +1)

/* @brief Stack size of the temporary lwIP initialization thread.
#define START_NETWORK_CONN_THREAD_STACKSIZE 1024
*/

/* @brief Priority of the temporary lwIP initialization thread.
#define START_NETWORK_CONN_THREAD_PRIO DEFAULT_THREAD_PRIO
#define START_NETWORK_CONN_THREAD_PRIORITY (configMAX_PRIORITIES - 4)
*/
#endif /* TASKPARAMETERS_H_ */
