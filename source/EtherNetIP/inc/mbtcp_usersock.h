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
 *     Module Name: mbtcp_usersock.h
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
 * This file contains user definitions specific to the sockets.
 *
 */

#ifndef __MBTCP_USERSOCK_H__
#define __MBTCP_USERSOCK_H__

#include <stdio.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <netinet/in.h>
//#include <errno.h>
//#include <netdb.h>
//#include <sys/types.h>
//#include <arpa/inet.h>
//#include <ifaddrs.h>
//#include <unistd.h>
//#include <fcntl.h>

#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/src/include/lwip/debug.h"

/* define what the socket type is (usually int) */
typedef struct netconn * MBTCP_USERSYS_SOCKTYPE;

//#define MBTCP_USER_SOCKIX_LISTENING  0
#define MBTCP_USER_MAX_NUM_SOCKETS   3
#define MBTCP_USER_MAX_NUM_SOCK_ID   FD_SETSIZE
#define MBTCP_USER_SOCKET_UNUSED     (0)

#endif /* __MBTCP_USERSOCK_H__ */
