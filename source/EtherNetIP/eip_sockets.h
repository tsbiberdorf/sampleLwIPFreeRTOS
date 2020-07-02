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

#ifndef __EIP_SOCKETS_H__
#define __EIP_SOCKETS_H__

CIP_ReturnCode eip_sockets_init (CIP_InitType const init_type);
void eip_sockets_shutdown (void);
CIP_ReturnCode eip_sockets_process (RTA_U32 const ms_since_last_call);
void eip_sockets_close_tcp_socket (RTA_Usersock_Socket const * const p_socket);
void eip_sockets_close_all_tcp_by_ip_address (RTA_U32 const ip_address);
CIP_ReturnCode eip_sockets_send_udp_io (RTA_U32 const ip_address, RTA_U16 const port, RTA_U8 const * const p_data, RTA_Size_Type const length_in_bytes, RTA_U8 const qos_tos);
void eip_sockets_update_all_tcp_timeouts (RTA_U16 const timeout_seconds);

#endif /* __EIP_SOCKETS_H__ */
