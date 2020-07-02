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

 #ifndef __RTA_GLOBAL_H__
 #define __RTA_GLOBAL_H__

/* ---------------------------------------------------------------- */
/*      COMMON INCLUDE FILES                                        */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/*      TYPE DEFINITIONS                                            */
/* ---------------------------------------------------------------- */
typedef unsigned char 		RTA_BOOL;
typedef char				RTA_CHAR;
typedef unsigned char 		RTA_BYTE;
typedef signed char			RTA_I8;
typedef unsigned char		RTA_U8;
typedef signed short		RTA_I16;
typedef unsigned short		RTA_U16;
typedef unsigned short 		RTA_WORD;
typedef signed long			RTA_I32;
typedef unsigned long		RTA_U32;
typedef unsigned long		RTA_DWORD;
typedef signed long long	RTA_I64;
typedef unsigned long long	RTA_U64;
typedef unsigned long long	RTA_LWORD;
typedef int                 RTA_INT;
typedef float				RTA_FLOAT;
typedef double				RTA_DOUBLE;

/* ---------------------------------------------------------------- */
/*      GENERIC DEFINITIONS                                         */
/* ---------------------------------------------------------------- */
#ifndef RTA_SUCCESS
    #define RTA_SUCCESS 0
#endif

#ifndef RTA_FAILURE
    #define RTA_FAILURE 1
#endif

#ifndef RTA_FAIL
    #define RTA_FAIL 1
#endif

#ifndef RTA_TRUE
    #define RTA_TRUE 1
#endif

#ifndef RTA_FALSE
    #define RTA_FALSE 0
#endif

#ifndef RTA_NULL
    #define RTA_NULL 0x00
#endif

#ifndef RTA_CODESPACE
    #define RTA_CODESPACE const
#endif

/* ---------------------------------------------------------------- */
/*      ENUMERATIONS                                                */
/* ---------------------------------------------------------------- */
typedef enum {
RTA_INIT_NORMAL = 0,	/* Read settings from NVRAM  */
RTA_INIT_OUTOFBOX,		/* Restore NVRAM settings to shipped default (user selected) */
RTA_INIT_MFR			/* Set NVRAM settings to MFR default (NVRAM CRC invalid) */
}RTA_INIT_TYPE;

typedef enum {
RTA_ERR_SUCCESS = 0,		                /* Successful return */
RTA_ERR_GENERAL = -1,		                /* General error code */
RTA_ERR_PTR = -2,                           /* Passed pointer NULL */
RTA_ERR_NO_RESOURCES = -3,                  /* Not enough resources for request */
RTA_ERR_PARAM_ERROR = -4,                   /* passed parameter invalid (not a NULL pointer) */
RTA_ERR_INIT_NVRAM = -1001,	                /* Init Error: NVRAM */
RTA_ERR_INIT_MUTEX = -1002,	                /* Init Error: Couldn't create mutex */
RTA_ERR_INCOMPLETE_CODE = -9999             /* Function hit a case that wasn't coded for */
}RTA_RETURN_CODE;

/* ---------------------------------------------------------------- */
/*      MACROS                                                      */
/* ---------------------------------------------------------------- */
#define RTA_UNUSED_PARAM(a)	if(a){}     /* Trick the compiler into thinking a function parameter is used */

#define RTA_MIN(a,b)    (a<b?a:b)		/* return the min of a and b */
#define RTA_MAX(a,b)    (a>b?a:b)		/* return the max of a and b */
#define RTA_MOD(a,b)   	(a-((a/b)*b))	/* perform a mod operation a%b */

#define RTA_GETHI(w)   	((RTA_U8)(((RTA_U16)w >> 8) & 0x00FF))	/* Returns the high byte of w */
#define RTA_GETLO(w)   	((RTA_U8)((RTA_U16)w & 0x00FF))		/* Returns the low byte of w */

#define RTA_SETHI(w,b) 	(w = (RTA_U16)((RTA_U16)w&0x00FF)|(RTA_U16)(((RTA_U16)b<<8)&0xFF00)) /* RTA_SETHI stores b to the high byte of w */
#define RTA_SETLO(w,b)	(w = (RTA_U16)((RTA_U16)w&0xFF00)|(RTA_U16)((RTA_U16)b&0x00FF))		/* RTA_SETLO stores b to the low byte of w */

#define RTA_PROFILE_PRINTF  printf("%s:%s:%d\r\n",__RTA_FILE__, __func__, __LINE__)	/* common code to show a line was hit */

#define RTA_PROFILE_CALLER	__RTA_FILE__,__LINE__	/* always pass the caller to a function for debug */

#endif /* __RTA_GLOBAL_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
