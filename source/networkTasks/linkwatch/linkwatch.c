/**
 * @file linkwatch.c
 * @details full implementation of monitoring the network cable link
 * @see linkwatch.h for public API interface
 */
#include <FreeRTOS.h>
#include <task.h>
#include "fsl_phy.h"
#include "TaskParameters.h"
#include "networkTasks/IpConnectHandler.h"
#include "networkTasks/linkwatch/linkwatch.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static TaskHandle_t tl_linkwatchTaskHandle;

/*******************************************************************************
 * Prototypes/externs =========================================================
 ******************************************************************************/
extern void eip_init(void); /**< external reference for initializing ethernet ip task*/

/*******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * @brief Main function for the linkwatch task
 * @details this tasks goal is to monitor if network cable is attached or not to
 * the unit. Whenever the cable is either attached or detached, it will attempt
 * to handle the change by either bringing up a new link, or releasing the current
 * one.
 *
 * @param arg args passed into the task
 */
static void linkwatchTask(void *arg)
{
    LWIP_UNUSED_ARG(arg); // LWIP deals with arguments

	static bool lastLinked = false; // Previous state of the link
	bool linked; // State of the link

    initPhyNetIf();
	eip_init(); //starts the ethernet ip/modbus task


    while (1)
    {
    	// Acquires the link state, populating the 'linked' variable
		PHY_GetLinkStatus( ENET, 0, &linked );

		// If the link has changed since the last time it was read
		if ( lastLinked != linked )
		{
			// Notify the state machine of the new state of the link
			IpAddrHandlerLinkChange(linked);
			//PRINTF("Link changed: %d\r\n", linked);
			lastLinked = linked; // Save the new state of the link to lastLinked
		}

        vTaskDelay( 1000 ); // Wait 1.0 seconds
    }

    vTaskDelete( NULL ); // End the task
}

/*******************************************************************************
 * Public functions
 ******************************************************************************/
void linkwatch_init(void) {
    //return sys_thread_new("linkwatch_thread", linkwatch_thread, NULL, LINKWATCH_THREAD_STACKSIZE, LINKWATCH_THREAD_PRIO);
	xTaskCreate(linkwatchTask, "linkwatch", LINKWATCH_THREAD_STACKSIZE , NULL, LINKWATCH_THREAD_PRIO, &tl_linkwatchTaskHandle);
	assert(tl_linkwatchTaskHandle);
}
