/*
 *            Copyright (c) 2002-2009 by Real Time Automation, Inc.
 *
 *  This software is copyrighted by and is the sole property of
 *  Real Time Automation Inc. (RTA).  All rights, title, ownership, 
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
 *  Real Time Automation
 *  150 S. Sunny Slope Road             USA 262.439.4999
 *  Suite 130                           http://www.rtaautomation.com
 *  Brookfield, WI 53005                software@rtaautomation.com
 *
 *************************************************************************
 *
 *     Module Name: mbtcp_usersys.h
 *         Version: 1.03
 *    Version Date: 05/05/2009
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains user definitions specific to Modbus TCP General
 * behavior.
 *
 */

#ifndef __MBTCP_USERSYS_H__
#define __MBTCP_USERSYS_H__

/* we need these include files */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* keep the compiler happy if a passed param isn't used */
#define MBTCP_UNUSED_PARAM(a)   if(a){}

/* we put lookup tables in code space to save RAM */
#define MBTCP_CODESPACE const

/* MB_GETHI returns the high byte of w */
#define MBTCP_GETHI(w)   ((uint8)(((uint16)w >> 8) & 0x00FF))

/* MB_GETLO returns the low byte of w */
#define MBTCP_GETLO(w)   ((uint8)((uint16)w & 0x00FF))

/* MB_SETHI stores b to the high byte of w */
#define MBTCP_SETHI(w,b) (w = (uint16)((uint16)w&0x00FF)|(uint16)(((uint16)b<<8)&0xFF00))

/* MB_SETLO stores b to the low byte of w */
#define MBTCP_SETLO(w,b) (w = (uint16)((uint16)w&0xFF00)|(uint16)((uint16)b&0x00FF))

/* MB_MODX is the macro for v%b */
#define MBTCP_MODX(v,b)  (v-((v/b)*b))

#define MBTCP_DEBUG 1 /* define this to 1 to enable debug printing */
#if MBTCP_DEBUG > 0
#if NOT_MP8000_HARDWARE
  #define mbtcp_user_dbprint0(f) printf(f)
  #define mbtcp_user_dbprint1(f,a1) printf(f,a1)
  #define mbtcp_user_dbprint2(f,a1,a2) printf(f,a1,a2)
  #define mbtcp_user_dbprint3(f,a1,a2,a3) printf(f,a1,a2,a3)
  #define mbtcp_user_dbprint4(f,a1,a2,a3,a4) printf(f,a1,a2,a3,a4)
  #define mbtcp_user_dbprint5(f,a1,a2,a3,a4,a5) printf(f,a1,a2,a3,a4,a5)
  #define mbtcp_user_dbprint6(f,a1,a2,a3,a4,a5,a6) printf(f,a1,a2,a3,a4,a5,a6)
#else
  #define mbtcp_user_dbprint0(f) PRINTF(f)
  #define mbtcp_user_dbprint1(f,a1) PRINTF(f,a1)
  #define mbtcp_user_dbprint2(f,a1,a2) PRINTF(f,a1,a2)
  #define mbtcp_user_dbprint3(f,a1,a2,a3) PRINTF(f,a1,a2,a3)
  #define mbtcp_user_dbprint4(f,a1,a2,a3,a4) PRINTF(f,a1,a2,a3,a4)
  #define mbtcp_user_dbprint5(f,a1,a2,a3,a4,a5) PRINTF(f,a1,a2,a3,a4,a5)
  #define mbtcp_user_dbprint6(f,a1,a2,a3,a4,a5,a6) PRINTF(f,a1,a2,a3,a4,a5,a6)

#endif
#else
  #define mbtcp_user_dbprint0(f)
  #define mbtcp_user_dbprint1(f,a1)
  #define mbtcp_user_dbprint2(f,a1,a2)
  #define mbtcp_user_dbprint3(f,a1,a2,a3)
  #define mbtcp_user_dbprint4(f,a1,a2,a3,a4)
  #define mbtcp_user_dbprint5(f,a1,a2,a3,a4,a5)
  #define mbtcp_user_dbprint6(f,a1,a2,a3,a4,a5,a6)
#endif

#endif /* __MBTCP_USERSYS_H__ */
