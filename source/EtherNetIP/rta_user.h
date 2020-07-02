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

 #ifndef __RTA_USER_H__
 #define __RTA_USER_H__

/* ---------------------------------------------------------------- */
/*      Memeory Management defines (rta_memory.h/c)                 */
/* ---------------------------------------------------------------- */
#define RTA_USER_MEMORY_NUM_HEAPS            2          
#define RTA_USER_MEMORY_NUM_BYTES_PER_HEAP   0x600

#define RTA_USER_MEMORY_ALIGNMENT_NETWORK   1   /* alignment when we encode/decode */
#define RTA_USER_MEMORY_ALIGNMENT_INTERNAL  8   /* alignment when we convert to internal */

/* ---------------------------------------------------------------- */
/*      ENUMERATIONS                                                */
/* ---------------------------------------------------------------- */
typedef enum {
	RTA_USER_LOGLEVEL_ALWAYS = 0,	/* always log */
	RTA_USER_LOGLEVEL_ERROR,		/* only log if RTA_USER_ENABLE_LOGLEVEL_ERROR is enabled */
	RTA_USER_LOGLEVEL_STARTUP,		/* only log if RTA_USER_ENABLE_LOGLEVEL_STARTUP is enabled */
	RTA_USER_LOGLEVEL_EVENT,		/* only log if RTA_USER_ENABLE_LOGLEVEL_EVENT is enabled */
	RTA_USER_LOGLEVEL_PROTOCOL,		/* only log if RTA_USER_ENABLE_LOGLEVEL_PROTOCOL is enabled */
	RTA_USER_LOGLEVEL_BSP 			/* only log if RTA_USER_ENABLE_LOGLEVEL_BSP is enabled */
}RTA_USER_LOGLEVEL_TYPE;

#define RTA_USER_ENABLE_LOGLEVEL_ERROR      1
#define RTA_USER_ENABLE_LOGLEVEL_STARTUP    1
#define RTA_USER_ENABLE_LOGLEVEL_EVENT      1
#define RTA_USER_ENABLE_LOGLEVEL_PROTOCOL   1
#define RTA_USER_ENABLE_LOGLEVEL_BSP        1  

/* ---------------------------------------------------------------- */
/*      COMMON DEFINITIONS                                          */
/* ---------------------------------------------------------------- */


/* ---------------------------------------------------------------- */
/*      FUNCTION PROTOTYPES                                         */
/* ---------------------------------------------------------------- */
RTA_U32 rta_user_rand_u32(RTA_U32 const seed_if_not_zero);

/* default for file includes path, allow files to over write */
#define __RTA_FILE__    __FILE__

void rta_user_print_memory(void const * const ptr, RTA_U32 const length, char * const str);
void rta_user_log(char const * const file, RTA_U32 const line_num, RTA_U16 const task_id, RTA_U16 const dev_id, RTA_USER_LOGLEVEL_TYPE const log_level, char const *fmt, ...);

#define _CRT_SECURE_NO_WARNINGS /* "safe" functions are a Microsoft concern... we want the portable snprint */
#define rta_snprintf snprintf	

/* ------------------------------------ */
/*         SYSTEM INCLUDE FILES         */
/* ------------------------------------ */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <sys/timeb.h>
//#include <time.h>
//#include <ctype.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
//#include <assert.h>

#define RTA_ASSERT(a)   assert(a) /*lint -unreachable */

#endif /* __RTA_USER_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
