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
 *     Module Name: mbtcp_userobj.h
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
 * This file defines supported function codes and data ranges for the 
 * Modbus TCP Server.
 *
 */

#ifndef __MBTCP_USEROBJ_H__
#define __MBTCP_USEROBJ_H__

/* uncomment the following definitions for each function supported */

// Class 0 Function Codes
#define FC03_RDHOLDREG    1
#define FC16_WRMULTREGS   1

// Class 1 Function Codes
//#define FC01_RDCOILSTAT   1
//#define FC02_RDINPUTSTAT  1
#define FC04_RDINPUTREG   1
//#define FC05_WRSINGLECOIL 1
#define FC06_WRSINGLEREG  1
//#define FC07_RDEXCEPTSTAT 1

// Class 2 Function Codes
//#define FC15_WRMULTCOILS  1
//#define FC23_RDWRMULTREGS 1

/* 
   There are 4 data tables available in Modbus
     Input Discretes  - bits/coils - read only
     Output Discretes - bits/coils - read/write
     Input Registers  - 16-bit values - read only
     Output Registers - 16-bit values - read/write 

   Input/Output Discretes often overlay each other.
   Input/Output Registers often overlay each other.
   Discretes may also overlay Registers.

   This is decided in "mbtcp_userobj.c" where the data 
   tables are defined.

   The total number of discretes/registers + the starting
   location must not go above 65535,
 
   Starting address starts at 1, not 0.
*/

#endif /* __MBTCP_USEROBJ_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
