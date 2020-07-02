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

#ifndef __CIP_COMMON_H__
#define __CIP_COMMON_H__

/* --------------------------------------------------------------- */
/*      RTA DEFINED COMMON DEFINITIONS                             */
/* --------------------------------------------------------------- */
#define CIP_MAX_LOG_MSG_SIZE    256

typedef enum 
{
    CIP_LOGLEVEL_DISABLED,
    CIP_LOGLEVEL_TERSE,
    CIP_LOGLEVEL_VERBOSE   
}CIP_LogLevel;

typedef enum 
{
    CIP_INITTYPE_NORMAL,
    CIP_INITTYPE_OUTOFBOX,
    CIP_INITTYPE_OUTOFBOX_NOCOMMS, /* same as out-of-box, keep TCP / Ethernet settings */
    CIP_INITTYPE_MFR   
}CIP_InitType;

typedef enum 
{
    CIP_RETURNCODE_SUCCESS,
    CIP_RETURNCODE_ABORTED_PROCESS,                     /* message processing was successfully interrupted (like pass through) */
    CIP_RETURNCODE_ERROR_INVALID_PARAMETER,             /* One or more passed parameter is invalid*/
    CIP_RETURNCODE_ERROR_MALLOC_FAILED,                 /* Pointer problem */
    CIP_RETURNCODE_ERROR_INTERNAL,                      /* Unexpected path reached (i.e. switch default case) */
    CIP_RETURNCODE_ERROR_FUNCTION_FAILED,               /* Function failed to do described task (somewhat of a catch all) */
    CIP_RETURNCODE_ERROR_FUNCTION_TIMEDOUT,             /* Blocking function timed out */
    CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED,    /* If a function has optional pointer, that is needed but not provided (like an error pointer) */
    CIP_RETURNCODE_ERROR_NOT_IMPLEMENTED,               /* Not implemented code */
    CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS,           /* use the stored general status */
    CIP_RETURNCODE_DECODE_MALLOC_FAILED,                /* expected data not in buffer */
    CIP_RETURNCODE_ENCODE_MALLOC_FAILED,                /* no room to store response data */
    CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT,           /* error decoding the path */
    CIP_RETURNCODE_DISCARD_PACKET                       /* Don't send response */
}CIP_ReturnCode;

/* --------------------------------------------------------------- */
/*      RTA DEFINED GLOBAL FUNCTIONS                               */
/* --------------------------------------------------------------- */

/* ********************* */
/* user called functions */
/* ********************* */
/* cip_common.c */
const char * cip_get_rc_string (CIP_ReturnCode const rc);
CIP_ReturnCode cip_enable_logging (CIP_LogLevel const loglevel);
CIP_ReturnCode cip_disable_logging (void);
CIP_ReturnCode cip_register_user_log_function_ptr (void (* const fptr) (char const * const p_data));

/* ********************** */
/* stack called functions */
/* ********************** */

/* cip_common.c */
void cip_log (CIP_LogLevel const log_level, char const * const format, ...);

/* software profiling function (turn off for production) */
#define CIP_LOG_FUNCTION_ENTER   
/*
#define CIP_LOG_FUNCTION_ENTER  cip_log(CIP_LOGLEVEL_VERBOSE, "ENTER %s()[%s:%d]", __FUNCTION__, __RTA_FILE__, __LINE__)
*/

#define CIP_LOG_FUNCTION_EXIT   
/*
#define CIP_LOG_FUNCTION_EXIT   cip_log(CIP_LOGLEVEL_VERBOSE, "EXIT %s()[%s:%d]", __FUNCTION__, __RTA_FILE__, __LINE__)   
*/

/*
#define CIP_LOG_FUNCTION_ERROR  
*/
#define CIP_LOG_FUNCTION_ERROR  cip_log(CIP_LOGLEVEL_VERBOSE, "ERROR %s()[%s:%d]", __FUNCTION__, __RTA_FILE__, __LINE__)    

#define CIP_LOG_EXIT_AND_RETURN(a)     {CIP_LOG_FUNCTION_EXIT; return(a);} /*lint -e(527) Unreachable code at token ';' */

extern void main_preprocess_CIP_ReturnCode(CIP_ReturnCode const ReturnCode);
#define CIP_LOG_ERROR_AND_RETURN(a)    {main_preprocess_CIP_ReturnCode(a); return(a);} /*lint -e(527) Unreachable code at token ';' */ 

#endif /* __CIP_COMMON_H__ */
