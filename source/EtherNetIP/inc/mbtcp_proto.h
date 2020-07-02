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
 *     Module Name: mbtcp_proto.h
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
 * This file contains Modbus TCP Server funcion prototypes.
 *
 */

#ifndef __MBTCP_PROTO_H__
#define __MBTCP_PROTO_H__

/* ********************************** */
/* Global Functions in mbtcp_rtasys.c */
/* ********************************** */
void mbtcp_rtasys_init        (void);
void mbtcp_rtasys_onTCPAccept (MBTCP_USERSYS_SOCKTYPE socket_id);
void mbtcp_rtasys_onTCPData   (MBTCP_USERSYS_SOCKTYPE socket_id, uint8 * socket_data, uint16 socket_data_size);
void mbtcp_rtasys_onTCPSent   (MBTCP_USERSYS_SOCKTYPE socket_id);
void mbtcp_rtasys_onTCPClose  (MBTCP_USERSYS_SOCKTYPE socket_id);

/* ********************************** */
/* Global Functions in mbtcp_server.c */
/* ********************************** */
void mbtcp_server_init (void);
void mbtcp_server_process (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);

/* *********************************** */
/* Global Functions in mbtcp_userobj.c */
/* *********************************** */
void   mbtcp_userobj_init (void);
uint8  *mbtcp_userobj_getInputCoilPtr   (uint16 start_addr, uint16 size_in_bits, uint8 * bit_pos);
uint8  *mbtcp_userobj_getOutputCoilPtr  (uint16 start_addr, uint16 size_in_bits, uint8 * bit_pos);
uint16 *mbtcp_userobj_getInputRegPtr    (uint16 start_addr, uint16 size_in_words);
uint16 *mbtcp_userobj_getOutputRegPtr   (uint16 start_addr, uint16 size_in_words);
void   mbtcp_userobj_coilsWritten       (uint16 start_addr, uint16 length_in_bits);
void   mbtcp_userobj_registersWritten   (uint16 start_addr, uint16 length_in_words);
uint8  mbtcp_userobj_getExceptionStatus (void);

/* ************************************ */
/* Global Functions in mbtcp_usersock.c */
/* ************************************ */
void   mbtcp_usersock_init     (void);
void   mbtcp_usersock_process  (void);
void   mbtcp_usersock_sendData (MBTCP_USERSYS_SOCKTYPE socket_id, uint8 * socket_data, uint16 socket_data_size);
void   mbtcp_usersock_tcpClose (MBTCP_USERSYS_SOCKTYPE socket_id);

/* *********************************** */
/* Global Functions in mbtcp_usersys.c */
/* *********************************** */
void   mbtcp_usersys_init       (void);
void   mbtcp_usersys_fatalError (char *function_name, int16 error_num);

#endif /* __MBTCP_PROTO_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
