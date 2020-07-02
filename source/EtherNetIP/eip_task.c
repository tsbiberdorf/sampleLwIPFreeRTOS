/*
 * eip_task.c
 *
 *  Created on: Sep 28, 2017
 *      Author: tbiberdorf
 */


#include "EtherNetIP/cip_system.h"

//#include "BoardSupport/MP8000v3Config.h"
#include "TaskParameters.h"

#include "lwip/sys.h"
#include "lwip/api.h"
//#include "lwip/src/include/lwip/debug.h"


#include "Peripheral/RTC/rtcInterface.h"
//#include "Peripheral/WDOG/wdog.h"
#include "eip_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void mbtcp_usersock_process (void); // modbusTCP process

RTA_Usersock_ReturnCode call_on_send_complete_reset_ethernetip_normal (RTA_Usersock_Socket const * const user_sock)
{
    RTA_UNUSED_PARAM(user_sock);

    /* delay the reset to ensure we are out of the socket processing and back into main */
//    reset_needed = RTA_TRUE;
    // this is where a reset happens if needed

    // @todo tsb need to call WDOG
    //wdtForceReset("ETHERNET REQUEST RESET");
    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

/*******************************************************************************
 * Callback Functions
 ******************************************************************************/

/*******************************************************************************
 * Task Functions
 ******************************************************************************/

/*! @brief Main function */
static void eip_thread(void *arg)
{
//    int c;
//    EIP_TCPObject_InterfaceConfiguration InterfaceConfiguration;
//    RTA_U8 ip_bytes[4];
//    RTA_Size_Type max_heap_bytes = 0;
//    err_t err;
	CIP_ReturnCode rc;
    uint32_t deltaTime,subSeconds,lastCallSubSeconds;

//    typedef struct DeltaQueue_s
//    {
//    	uint32_t start;
//    	uint32_t end;
//    	uint32_t deltaTime;
//    	uint16_t idxCnt;
//    }sDeltaQueue_t;
//    sDeltaQueue_t *ptrDeltaTimeQueue;
//    uint16_t deltaTimeQueueIdx;
//    volatile uint32_t deltaTimeCheckValue = 7;
//#define DELTA_QUEUE_SIZE (24)

    struct netconn *conn;
//    struct netbuf *buf;
    lastCallSubSeconds = 0;
    LWIP_UNUSED_ARG(arg);
#define EIP_PROCESS_CYCLE_PERIOD_MS (1)

	rc = eip_startup(CIP_INITTYPE_NORMAL);
	if(rc != CIP_RETURNCODE_SUCCESS)
	{
		LWIP_ERROR ("EIP init failed\r\n",(rc != CIP_RETURNCODE_SUCCESS), return;);
	}


//	ptrDeltaTimeQueue = pvPortMalloc(DELTA_QUEUE_SIZE * sizeof(sDeltaQueue_t));
//	deltaTimeQueueIdx = 0;
//	ptrDeltaTimeQueue[deltaTimeQueueIdx].idxCnt = 0;

	// call the initialization code (RTA's init routines)
	extern void mbtcp_rtasys_init (void); // defined in lwip/contrib/apps/EtherNetIP/mbtcp_rtasys.c

	mbtcp_rtasys_init();

    while (1)
    {
    	rtcGetMSecRTCTime(&deltaTime,&subSeconds);
    	if( subSeconds != lastCallSubSeconds)
    	{
    		if( lastCallSubSeconds > subSeconds)
    		{
    			/*
    			 * the timer has wrapped
    			 */
    			deltaTime = (1000+subSeconds)- lastCallSubSeconds;
    		}
    		else
    		{
    			deltaTime = subSeconds - lastCallSubSeconds;
    		}

//    		if( deltaTime > deltaTimeCheckValue)
//    		{
//    			Profile5Set();
//
//    			ptrDeltaTimeQueue[deltaTimeQueueIdx].start = subSeconds;
//    			ptrDeltaTimeQueue[deltaTimeQueueIdx].end = lastCallSubSeconds;
//    			ptrDeltaTimeQueue[deltaTimeQueueIdx].deltaTime = deltaTime;
//    			deltaTimeQueueIdx++;
//    			if(deltaTimeQueueIdx > DELTA_QUEUE_SIZE)
//    			{
//    				deltaTimeQueueIdx = 0;
//    			}
//    			ptrDeltaTimeQueue[deltaTimeQueueIdx].idxCnt = 0;
//
//    			Profile5Clr();
//
//    		}
//    		else
//    		{
//    			if( ptrDeltaTimeQueue[deltaTimeQueueIdx].idxCnt < 0xFF00 )
//    			{
//    				ptrDeltaTimeQueue[deltaTimeQueueIdx].idxCnt++;
//    			}
//    		}

    		lastCallSubSeconds = subSeconds;
    		/* ADD USER PROCESS CODE HERE IF NEEDED */
    //        rc = eip_process(local_get_ticks_passed_since_last_call_to_this_function());

//    		if( deltaTimeQueue[deltaTimeQueueIdx].idxCnt < 0xFFFF )
//    		{
//    			deltaTimeQueue[deltaTimeQueueIdx].idxCnt++;
//    		}

//    		if(deltaTime > deltaTimeCheckValue)
//    		{
//    			deltaTimeQueue[deltaTimeQueueIdx++].deltaTime = deltaTime;
//    			if(deltaTimeQueueIdx > DELTA_QUEUE_SIZE)
//    			{
//    				deltaTimeQueueIdx=0;
//    			}
//    			deltaTimeQueue[deltaTimeQueueIdx].idxCnt = 0;
//    		}
//extern void Profile4Set(void);
//extern void Profile5Set(void);
//extern void Profile4Clr(void);
//extern void Profile5Clr(void);


//			if(deltaTimeQueueIdx == DELTA_QUEUE_SIZE)
//			{
//	    		Profile4Set();
//	    		Profile4Clr();
//			}
//
//    		Profile4Set();
//    		Profile5Set();
    		rc = eip_process(deltaTime);
//    		Profile4Clr();
//    		Profile5Clr();

            if(rc != CIP_RETURNCODE_SUCCESS) // @todo verify how return code will be used
            {
        		LWIP_ERROR ("eip_process failed\r\n",(rc != CIP_RETURNCODE_SUCCESS), return;);
            }
    	}

    	/*
    	 * process the RTA subsystem, modbusTCP communications
    	 */
		mbtcp_usersock_process();

    	vTaskDelay(EIP_PROCESS_CYCLE_PERIOD_MS/portTICK_PERIOD_MS); // delay 2 ms

    }

    netconn_delete( conn ); // Remove the connection
    vTaskDelete( NULL ); // End the task
}

void eip_init(void)
{
    sys_thread_new("eip_thread", eip_thread, NULL, EIP_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}

void main_preprocess_CIP_ReturnCode (CIP_ReturnCode const ReturnCode)
{
    switch (ReturnCode)
    {
        case CIP_RETURNCODE_SUCCESS:                            break;
        case CIP_RETURNCODE_ABORTED_PROCESS:                    break;
        case CIP_RETURNCODE_ERROR_INVALID_PARAMETER:            CIP_LOG_FUNCTION_ERROR; __asm("BKPT #0\n"); break;
        case CIP_RETURNCODE_ERROR_MALLOC_FAILED:                CIP_LOG_FUNCTION_ERROR; __asm("BKPT #0\n"); break;
        case CIP_RETURNCODE_ERROR_INTERNAL:                     CIP_LOG_FUNCTION_ERROR; __asm("BKPT #0\n"); break;
        case CIP_RETURNCODE_ERROR_FUNCTION_FAILED:              break;
        case CIP_RETURNCODE_ERROR_FUNCTION_TIMEDOUT:            break;
        case CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED:   CIP_LOG_FUNCTION_ERROR; __asm("BKPT #0\n"); break;
        case CIP_RETURNCODE_ERROR_NOT_IMPLEMENTED:              CIP_LOG_FUNCTION_ERROR; __asm("BKPT #0\n"); break;
        case CIP_RETURNCODE_DISCARD_PACKET:                     break;
        case CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS:          break;
        case CIP_RETURNCODE_DECODE_MALLOC_FAILED:               break;
        case CIP_RETURNCODE_ENCODE_MALLOC_FAILED:               break;
        case CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT:          break;
        default:                                                CIP_LOG_FUNCTION_ERROR; __asm("BKPT #0\n"); break;
    }
}
