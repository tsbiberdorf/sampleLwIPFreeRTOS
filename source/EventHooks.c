/**
 * @file EventHooks.c
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"


static uint32_t RTOS_RunTimeCounter; /* runtime counter, used for configGENERATE_RUNTIME_STATS */
//
//void FTM0_IRQHandler(void) {
//  /* Clear interrupt flag.*/
//  FTM_ClearStatusFlags(FTM0, kFTM_TimeOverflowFlag);
//  RTOS_RunTimeCounter++; /* increment runtime counter */
//}

void RTOS_AppConfigureTimerForRuntimeStats(void)
{
  RTOS_RunTimeCounter = 0;
//  EnableIRQ(FTM0_IRQn);
}


uint32_t RTOS_AppGetRuntimeCounterValueFromISR(void)
{
  return RTOS_RunTimeCounter;
}

uint32_t RTOS_GetAppConfigureTimerForRuntimeStats(void)
{


//	return rtcGetRTCTime();
//	return rtcGetRunTimeStateTime();
	return 0;
}

/**
 * @brief Idle Task Hook
 * @details Place all Idle task tracking operations in this method
 */
 void vApplicationIdleHook( void )
 {

 }


 /**
  * @brief Malloc Memory Failure detected
  */
 void vApplicationMallocFailedHook()
 {
	 volatile uint8_t i=1;
	 while(i)
		 ;
 }

 /**
  * @brief Critical Error, Stack Overflow has been detected
  */
 void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
 {
	 volatile uint8_t i=1;
	 while(i)
		 ;
 }

 /**
  * @brief called on each tick count
  */
 void vApplicationTickHook()
 {

 }


