/*
 *            Copyright (c) 2015-2017 by Real Time Automation, Inc.
 *
 *  This software is copyrighted by and is the sole property of
 *  Real Time Automation, Inc. (RTA).  All rights, title, ownership,
 *  or other interests in the software remain the property of RTA.
 *  This software may only be used in accordance with the corresponding
 *  license agreement.  Any unauthorized use, duplication, transmission,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MAY NOT be removed or modified without prior
 *  written consent of RTA.
 *
 *  RTA reserves the right to modify this software without notice.
 *
 *  www.rtaautomation.com
 */

/* ========================== */
/*   INCLUDE FILES            */
/* ========================== */
#include "cip_system.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "cip_common.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* user defined function options - THESE ARE NEVER CLEARED AUTOMATICALLY */
static void (*user_fptr_log) (char const * const p_data) = NULL;
static CIP_LogLevel user_log_level = CIP_LOGLEVEL_DISABLED;

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode cip_startup (CIP_InitType const InitType)
{
    RTA_UNUSED_PARAM(InitType);

    CIP_LOG_FUNCTION_ENTER;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_shutdown (void)
{
    CIP_LOG_FUNCTION_ENTER;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* Don't profile this function since the prints would be too much. */
CIP_ReturnCode cip_process (RTA_U32 const ms_since_last_call)
{
    RTA_UNUSED_PARAM(ms_since_last_call);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_enable_logging (CIP_LogLevel const loglevel)
{
    CIP_LOG_FUNCTION_ENTER;

    switch(loglevel)
    {
        case CIP_LOGLEVEL_TERSE:
        case CIP_LOGLEVEL_VERBOSE:
            user_log_level = loglevel;     
            break; 

        /* don't allow the enable function to disable... there is a disable function */
        case CIP_LOGLEVEL_DISABLED:
        default:
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);  
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_disable_logging (void)
{
    CIP_LOG_FUNCTION_ENTER;

    user_log_level = CIP_LOGLEVEL_DISABLED;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_register_user_log_function_ptr (void (* const fptr) (char const * const p_data))
{
    CIP_LOG_FUNCTION_ENTER;

    user_fptr_log = fptr;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

void cip_log (CIP_LogLevel const log_level, char const * const format, ...)
{
	va_list ap;
	char debug_buf[CIP_MAX_LOG_MSG_SIZE];

    if((user_fptr_log == NULL) || (user_log_level == CIP_LOGLEVEL_DISABLED))
        return;

    switch(log_level)
    {
        case CIP_LOGLEVEL_TERSE:
            /* if we are enabled, then always print terse */
            break;
        case CIP_LOGLEVEL_VERBOSE:
            /* ensure we are verbose enabled */    
            if(user_log_level != CIP_LOGLEVEL_VERBOSE)  
                return;
            break;
        case CIP_LOGLEVEL_DISABLED:
        default:
            return;
    }

	va_start(ap, format);
	(void)rta_vsnprintf(debug_buf, sizeof(debug_buf), format, ap);
	va_end(ap);
//    RTA_UNUSED_PARAM(ap);

    user_fptr_log(debug_buf);
}

const char * cip_get_rc_string (CIP_ReturnCode const rc)
{
    switch(rc)
    {
        case CIP_RETURNCODE_SUCCESS:                            return("SUCCESS");
        case CIP_RETURNCODE_ABORTED_PROCESS:                    return("APORTED_PROCESS");
        case CIP_RETURNCODE_ERROR_INVALID_PARAMETER:            return("INVALID_PARAMETER");
        case CIP_RETURNCODE_ERROR_MALLOC_FAILED:                return("MALLOC_FAILED");
        case CIP_RETURNCODE_ERROR_INTERNAL:                     return("INTERNAL");
        case CIP_RETURNCODE_ERROR_FUNCTION_FAILED:              return("FUNCTION_FAILED");
        case CIP_RETURNCODE_ERROR_FUNCTION_TIMEDOUT:            return("FUNCTION_TIMEDOUT");
        case CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED:   return("NECESSARY_PTR_NOT_PROVIDED");
        case CIP_RETURNCODE_ERROR_NOT_IMPLEMENTED:              return("NOT_IMPLEMENTED");
        case CIP_RETURNCODE_DISCARD_PACKET:                     return("DISCARD_PACKET");
        case CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS:          return("USE_GENERAL_STATUS");
        case CIP_RETURNCODE_DECODE_MALLOC_FAILED:               return("DECODE_MALLOC_FAILEDS");
        case CIP_RETURNCODE_ENCODE_MALLOC_FAILED:               return("ENCODE_MALLOC_FAILED");
        case CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT:          return("ERROR_DECODE_PATH_SEGMENT");
        default:                                                return("UNKNOWN rc");
    }
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */

/* *********** */
/* END OF FILE */
/* *********** */
