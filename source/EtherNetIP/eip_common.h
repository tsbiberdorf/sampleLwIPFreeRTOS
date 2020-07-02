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

#ifndef __EIP_COMMON_H__
#define __EIP_COMMON_H__

/* --------------------------------------------------------------- */
/*      RTA DEFINED COMMON DEFINITIONS                             */
/* --------------------------------------------------------------- */

/* --------------------------------------------------------------- */
/*      RTA DEFINED GLOBAL FUNCTIONS                               */
/* --------------------------------------------------------------- */

/* ********************* */
/* user called functions */
/* ********************* */
/* eip_common.c */
CIP_ReturnCode eip_startup (CIP_InitType const InitType);
CIP_ReturnCode eip_shutdown (void);
CIP_ReturnCode eip_process (RTA_U32 const ms_since_last_call);

CIP_ReturnCode eip_enable_logging (CIP_LogLevel const loglevel);
CIP_ReturnCode eip_disable_logging (void);
CIP_ReturnCode eip_register_user_log_function_ptr (void (* const fptr) (char const * const p_data));

/* ********************** */
/* stack called functions */
/* ********************** */

/* software profiling function (turn off for production) */
#define EIP_LOG_FUNCTION_ENTER   
/*
#define EIP_LOG_FUNCTION_ENTER  cip_log(CIP_LOGLEVEL_VERBOSE, "ENTER %s()[%s:%d]", __FUNCTION__, __RTA_FILE__, __LINE__)
*/

#define EIP_LOG_FUNCTION_EXIT   
/*
#define EIP_LOG_FUNCTION_EXIT   cip_log(CIP_LOGLEVEL_VERBOSE, "EXIT %s()[%s:%d]", __FUNCTION__, __RTA_FILE__, __LINE__)   
*/

/*
#define EIP_LOG_FUNCTION_ERROR  
*/
#define EIP_LOG_FUNCTION_ERROR  cip_log(CIP_LOGLEVEL_VERBOSE, "ERROR %s()[%s:%d]", __FUNCTION__, __RTA_FILE__, __LINE__)    

#define EIP_LOG_EXIT_AND_RETURN(a)     {EIP_LOG_FUNCTION_EXIT; return(a);} /*lint -e(527) Unreachable code at token ';' */ 

#define EIP_LOG_ERROR_AND_RETURN(a)    {main_preprocess_CIP_ReturnCode(a); return(a);} /*lint -e(527) Unreachable code at token ';' */ 

#endif /* __EIP_COMMON_H__ */
