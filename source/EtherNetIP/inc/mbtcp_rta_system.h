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
 *     Module Name: rta_system.h
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
 * This file contains generic application level definitions.
 *
 */

#ifndef __RTA_SYSTEM_H__
#define __RTA_SYSTEM_H__

/* --------------------------------------------------------------- */
/*      TYPE DEFINITIONS                                           */
/* --------------------------------------------------------------- */
#ifndef int8
  #define int8   signed char
#endif

#ifndef uint8
  #define uint8  unsigned char
#endif

#ifndef int16
  #define int16  short
#endif

#ifndef uint16
  #define uint16 unsigned short
#endif

#ifndef int32
  #define int32  long
#endif

#ifndef uint32
  #define uint32 unsigned long
#endif

#ifndef NULLFNPTR
  #define NULLFNPTR ((void (*)()) 0)
#endif

/* --------------------------------------------------------------- */
/*      GENERIC DEFINITIONS                                        */
/* --------------------------------------------------------------- */
#ifndef SUCCESS
  #define SUCCESS 0
#endif

#ifndef FAILURE
  #define FAILURE 1
#endif

#ifndef FAIL
  #define FAIL 1
#endif

#ifndef TRUE
  #define TRUE 1
#endif

#ifndef FALSE
  #define FALSE 0
#endif

#ifndef YES
  #define YES 1
#endif

#ifndef NO
  #define NO 0
#endif

#ifndef ON
  #define ON 1
#endif

#ifndef OFF
  #define OFF 0
#endif

#ifndef NULL
  #define NULL 0x00
#endif

#ifndef NULLPTR
  #define NULLPTR (void *) NULL
#endif

/* --------------------------------------------------------------- */
/*      MACROS                                                     */
/* --------------------------------------------------------------- */
#define Xmod(v,b) (v - ((v/b)*b))
#define Xmin(a,b) ((a<b)?a:b)
#define Xmax(a,b) ((a>b)?a:b)
#define BYTESWAP(a)  ((uint16)((((unsigned int)a)>>8)&0x00FF)|((((uint16)a)<<8)&0xFF00))

/* --------------------------------------------------------------- */
/*      RTA STANDARD INCLUDE FILES                                 */
/* --------------------------------------------------------------- */
#include <EtherNetIP/inc/mbtcp_rta_proto.h>

#endif /* __RTA_SYSTEM_H__ */
