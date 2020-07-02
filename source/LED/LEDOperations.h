/*
 * LEDOperations.h
 *
 *  Created on: Dec 13, 2019
 *      Author: TBiberdorf
 */

#ifndef LED_GPIOCOPERATIONS_H_
#define LED_GPIOCOPERATIONS_H_
#include "freeRTOS.h"
#include "task.h"
/**
 * @todo need to update for NGR Relay board
 */
#define LED_NGR_ON 		(1<<0)
#define LED_NGR_OFF 	(1<<1)
#define LED_GND_ON 		(1<<2)
#define LED_GND_OFF 	(1<<3)
#define LED_TRIPPED_ON	(1<<4)
#define LED_TRIPPED_OFF	(1<<5)
#define LED_SENSOR_ON 	(1<<6)
#define LED_SENSOR_OFF 	(1<<7)
#define LED_POWER_ON 	(1<<8)
#define LED_POWER_OFF 	(1<<9)
#define LED_WIFI_ON 	(1<<10)
#define LED_WIFI_OFF 	(1<<11)
#define LED_TEST_ON 	(1<<12)
#define V24_TEST_ON 	(1<<13)
#define V33_TEST_ON 	(1<<14)
#define PT_LED 			(1<<15)


void StartLedTask();
TaskHandle_t GetLedTaskHandle();
int32_t LedSelfTestResults();

#endif // LED_GPIOCOPERATIONS_H_
