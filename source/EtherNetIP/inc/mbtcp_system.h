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
 *     Module Name: mbtcp_system.h
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
 * This file contains Modbus TCP Server specific definitions.
 *
 */

#ifndef __MBTCP_SYSTEM_H__
#define __MBTCP_SYSTEM_H__

#define MBTCP_PORT 502
#define SOCKTYPE_TCP 0

// parsing offsets
#define MBTCP_OFFSET_TRANSID  0
#define MBTCP_OFFSET_PROTOID  2
#define MBTCP_OFFSET_LENGTH   4
#define MBTCP_OFFSET_UNITID   6
#define MBTCP_OFFSET_FUNCCODE 7
#define MBTCP_OFFSET_DATA     8

// error codes
#define MBTCP_ERRCODE_FUNCTION 0x01
#define MBTCP_ERRCODE_ADDRESS  0x02
#define MBTCP_FCERRBIT         0x80

/* --------------------------------------------------------------- */
/*	MBTCP STANDARD INCLUDE FILES				   */
/* --------------------------------------------------------------- */
#include "EtherNetIP/inc/mbtcp_usersys.h"
#include "EtherNetIP/inc/mbtcp_userobj.h"
#include "EtherNetIP/inc/mbtcp_usersock.h"
#include "EtherNetIP/inc/mbtcp_struct.h"
#include "EtherNetIP/inc/mbtcp_proto.h"
#include "EtherNetIP/inc/mbtcp_revision.h"

#endif /* __MBTCP_SYSTEM_H__ */
