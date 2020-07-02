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

/* ========================== */
/*   INCLUDE FILES            */
/* ========================== */
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/src/include/lwip/debug.h"
#include "rta_system.h"
/* ADD YOUR include file for LWIP / netconn here */

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "rta_usersock.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */
static RTA_Usersock_ReturnCode local_init_socket (RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_init_listening_socket (RTA_Usersock_Listening * const p_user_sock);
static void local_process_socket_timers (RTA_U32 const ms_since_last_call);

static RTA_Usersock_ReturnCode local_process_readfds_isset_listening_socket(RTA_Usersock_Listening const * const p_user_sock);
static RTA_Usersock_ReturnCode local_process_readfds_isset_udp(RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_process_readfds_isset_tcp(RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_process_writefds_isset_tcp(RTA_Usersock_Socket * const p_user_sock);

static RTA_Usersock_ReturnCode local_send_udp_data (RTA_Usersock_Socket const * const p_user_sock, RTA_U8 const * const p_data, RTA_Size_Type const length_in_bytes);
static RTA_Usersock_ReturnCode local_send_tcp_data (RTA_Usersock_Socket const * const p_user_sock, RTA_U8 const * const p_data, RTA_Size_Type const length_in_bytes);

static RTA_Usersock_ReturnCode local_onAccept (RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_onData (RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_onSent (RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_onClose (RTA_Usersock_Socket const * const p_user_sock);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */
static RTA_Usersock_Socket sockets[RTA_USERSOCK_MAX_NUM_SOCKETS];
static RTA_Usersock_Listening listening_sockets[RTA_USERSOCK_MAX_NUM_PORTS_TCPSERVER];

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
RTA_Usersock_ReturnCode rta_usersock_init (void)
{
    RTA_Usersock_ReturnCode rc;
    RTA_U16 sock_ix;

    for(sock_ix=0; sock_ix<RTA_USERSOCK_MAX_NUM_SOCKETS; sock_ix++)
    {
        rc = local_init_socket(&sockets[sock_ix]);
        if(rc!=RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
    }

    for(sock_ix=0; sock_ix<RTA_USERSOCK_MAX_NUM_PORTS_TCPSERVER; sock_ix++)
    {
        rc = local_init_listening_socket(&listening_sockets[sock_ix]);
        if(rc!=RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_process (RTA_U32 const ms_since_last_call)
{
    RTA_Usersock_ReturnCode rc;
    RTA_U16 sock_ix;

    if(ms_since_last_call)
        local_process_socket_timers(ms_since_last_call);

    for(sock_ix=0; sock_ix<RTA_USERSOCK_MAX_NUM_PORTS_TCPSERVER; sock_ix++)
    {
        if(listening_sockets[sock_ix].sockid == RTA_Usersock_SockId_Unused)
            continue;

        rc = local_process_readfds_isset_listening_socket(&listening_sockets[sock_ix]);
        if(rc!=RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
    }

    for(sock_ix=0; sock_ix<RTA_USERSOCK_MAX_NUM_SOCKETS; sock_ix++)
    {
        if(sockets[sock_ix].sockid == RTA_Usersock_SockId_Unused)
            continue;

        switch(sockets[sock_ix].socket_type)
        {
            case RTA_Usersock_Socket_Type_TCPSERVER:
                if(sockets[sock_ix].socket_state == RTA_Usersock_State_Connected)
                {
                    if (sockets[sock_ix].send_in_prog_flag)
                    {
                        local_process_writefds_isset_tcp(&sockets[sock_ix]);
                    }
                    rc = local_process_readfds_isset_tcp(&sockets[sock_ix]);
                    if(rc!=RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
                }
                break;

            case RTA_Usersock_Socket_Type_UDP:
                if(sockets[sock_ix].socket_state == RTA_Usersock_State_Connected)
                {
                    rc = local_process_readfds_isset_udp(&sockets[sock_ix]);
                    if(rc!=RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
                }
                break;

            case RTA_Usersock_Socket_Type_TCPCLIENT:
            case RTA_Usersock_Socket_Type_RAW:
            case RTA_Usersock_Socket_Type_DISABLED:
            default:
                break;
        }
    }

    return (RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_Socket * rta_usersock_allocate_socket (RTA_Usersock_Socket_Type const socket_type, RTA_U16 const socket_port)
{
    RTA_U16 sock_ix;

    RTA_UNUSED_PARAM(socket_type);
    RTA_UNUSED_PARAM(socket_port);

    for(sock_ix=0; sock_ix<RTA_USERSOCK_MAX_NUM_SOCKETS; sock_ix++)
    {
        if(sockets[sock_ix].socket_state == RTA_Usersock_State_Free)
        {
            sockets[sock_ix].socket_state = RTA_Usersock_State_Configuring;
            sockets[sock_ix].socket_type = socket_type;
            sockets[sock_ix].socket_port = socket_port;        
            sockets[sock_ix].ip_tos.dscp = 0;
            sockets[sock_ix].ip_tos.ecn = 0;
            return(&sockets[sock_ix]);
        }
    }

    return(RTA_NULL);
}

RTA_Usersock_ReturnCode rta_usersock_free_socket(RTA_Usersock_Socket * const p_user_sock)
{
    RTA_Usersock_ReturnCode sock_rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    (void)rta_usersock_close_sockid(p_user_sock);

    sock_rc = local_init_socket(p_user_sock);

    if(sock_rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(sock_rc);

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_register_callbacks (RTA_Usersock_Socket * const p_user_sock, RTA_USERSOCK_CALLBACK_STRUCT const * const callback_struct_ptr)
{

    if(!p_user_sock || !callback_struct_ptr)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_user_sock->socket_state != RTA_Usersock_State_Configuring)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_STATE);

    memcpy(&p_user_sock->callback, callback_struct_ptr, sizeof(p_user_sock->callback));

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_open_udp_sockid (RTA_Usersock_Socket * const p_user_sock)
{
    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_user_sock->socket_state != RTA_Usersock_State_Configuring)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_STATE);

    if(p_user_sock->socket_type != RTA_Usersock_Socket_Type_UDP)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_SOCKET_TYPE);

    if(p_user_sock->sockid != RTA_Usersock_SockId_Unused)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_SOCKET);

    // Create a new udp connection handle.
    p_user_sock->sockid = netconn_new(NETCONN_UDP);
    if (p_user_sock->sockid == NULL)
    {
        p_user_sock->sockid = RTA_Usersock_SockId_Unused;
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
    }
    if (netconn_bind(p_user_sock->sockid, IP_ADDR_ANY, p_user_sock->socket_port) != ERR_OK)
    {
        p_user_sock->sockid = RTA_Usersock_SockId_Unused;
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
    }

    // netconn_recv timeout, in ms, needs LWIP_SO_RCVTIMEO = 1 in lwipopts.h
    netconn_set_recvtimeout(p_user_sock->sockid, 1);

    p_user_sock->socket_state = RTA_Usersock_State_Connected;

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_open_tcp_sockid (RTA_Usersock_Socket * const p_user_sock)
{
    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_user_sock->socket_state != RTA_Usersock_State_Configuring)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_STATE);

    if(p_user_sock->socket_type != RTA_Usersock_Socket_Type_TCPSERVER)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_SOCKET_TYPE);

    if(p_user_sock->sockid != RTA_Usersock_SockId_Unused)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_SOCKET);

    p_user_sock->socket_state = RTA_Usersock_State_Listening;

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_start_tcp_listening (RTA_Usersock_PortId const socket_port, RTA_U16 const num_ports)
{
    int sock_ix;

    if(!num_ports)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    for(sock_ix=0; sock_ix<RTA_USERSOCK_MAX_NUM_PORTS_TCPSERVER; sock_ix++)
    {
        if(listening_sockets[sock_ix].socket_port == socket_port)
        {
//            USART_printf(&host, "start %d\r\n", __LINE__);
            return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);
        }
        if(listening_sockets[sock_ix].sockid == RTA_Usersock_SockId_Unused)
            break;
    }
    if(sock_ix >= RTA_USERSOCK_MAX_NUM_PORTS_TCPSERVER)
    {
//        USART_printf(&host, "start %d\r\n", __LINE__);
        return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);
    }

    // Create a new tcp connection handle.

    listening_sockets[sock_ix].socket_port = socket_port;
    listening_sockets[sock_ix].num_ports = num_ports;
    listening_sockets[sock_ix].sockid = netconn_new(NETCONN_TCP);

    if (listening_sockets[sock_ix].sockid == NULL)
    {
//        USART_printf(&host, "start %d\r\n", __LINE__);
        listening_sockets[sock_ix].sockid = RTA_Usersock_SockId_Unused;
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
    }

    netconn_set_recvtimeout(listening_sockets[sock_ix].sockid, 1);

    if (netconn_bind(listening_sockets[sock_ix].sockid, NULL, socket_port) != ERR_OK)
    {
//        USART_printf(&host, "start %d\r\n", __LINE__);
        listening_sockets[sock_ix].sockid = RTA_Usersock_SockId_Unused;
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
    }
    if (netconn_listen(listening_sockets[sock_ix].sockid) != ERR_OK)
    {
//        USART_printf(&host, "start %d\r\n", __LINE__);
        listening_sockets[sock_ix].sockid = RTA_Usersock_SockId_Unused;
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_stop_tcp_listening (RTA_Usersock_PortId const socket_port)
{

    int sock_ix;
    
    for(sock_ix=0; sock_ix<RTA_USERSOCK_MAX_NUM_PORTS_TCPSERVER; sock_ix++)
    {
        if(listening_sockets[sock_ix].socket_port == socket_port)
        {
            if(listening_sockets[sock_ix].sockid == RTA_Usersock_SockId_Unused)
                break;

            /* don't allow errors in close/init to trump our return code, so ignore close/init return codes */
            netconn_delete(listening_sockets[sock_ix].sockid);
            (void)local_init_listening_socket(&listening_sockets[sock_ix]);
        }
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_close_sockid (RTA_Usersock_Socket * const p_user_sock)
{
    RTA_Usersock_ReturnCode rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_user_sock->sockid != RTA_Usersock_SockId_Unused)
    {
        netconn_delete(p_user_sock->sockid);
        p_user_sock->sockid = RTA_Usersock_SockId_Unused;
    }

    switch(p_user_sock->socket_type)
    {
        case RTA_Usersock_Socket_Type_TCPSERVER:
            p_user_sock->socket_state = RTA_Usersock_State_Listening;
            break;

        case RTA_Usersock_Socket_Type_UDP:  
            break;

        case RTA_Usersock_Socket_Type_TCPCLIENT:
        case RTA_Usersock_Socket_Type_RAW:
        case RTA_Usersock_Socket_Type_DISABLED:
        default:
            break;
    }

    rc = local_onClose(p_user_sock);
    if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_close_all_by_type_and_port (RTA_Usersock_Socket_Type const type, RTA_U16 const port)
{
    RTA_UNUSED_PARAM(type);
    RTA_UNUSED_PARAM(port);

    return(RTA_USERSOCK_RETURNCODE_ERROR_NOT_IMPLEMENTED);
}

RTA_Usersock_ReturnCode rta_usersock_close_all (void)
{
    return(RTA_USERSOCK_RETURNCODE_ERROR_NOT_IMPLEMENTED);
}

RTA_Usersock_ReturnCode rta_usersock_send_data (RTA_Usersock_Socket * const p_user_sock, RTA_U8 const * const p_data, RTA_Size_Type const length_in_bytes)
{
#if 0 /* WINSOCK doesn't support TOS tagging, but this is what the code would look like */
    int tos, tos_len;
    tos_len = sizeof(tos);
#endif

    if(!p_user_sock || !p_data || 0==length_in_bytes)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

#if 0 /* WINSOCK doesn't support TOS tagging, but this is what the code would look like */
	if(p_user_sock->ip_tos.dscp > 0)
	{
		tos = (((p_user_sock->ip_tos.dscp & 0x3f) << 2) | (p_user_sock->ip_tos.ecn & 0x03));
		if(setsockopt(p_user_sock->sockid, IPPROTO_IP, IP_TOS, (char *)&tos, tos_len) == SOCKET_ERROR)
            return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
	}
#endif

    switch(p_user_sock->socket_type)
    {
        case RTA_Usersock_Socket_Type_TCPSERVER:
            p_user_sock->send_in_prog_flag = RTA_TRUE;
            return(local_send_tcp_data (p_user_sock, p_data, length_in_bytes));

        case RTA_Usersock_Socket_Type_UDP:  
            return(local_send_udp_data (p_user_sock, p_data, length_in_bytes));

        case RTA_Usersock_Socket_Type_TCPCLIENT:
        case RTA_Usersock_Socket_Type_RAW:
        case RTA_Usersock_Socket_Type_DISABLED:
        default:
            return(RTA_USERSOCK_RETURNCODE_ERROR_INTERNAL);
    }   
}

RTA_Usersock_ReturnCode rta_usersock_set_max_sockets_per_port(RTA_U16 const max_sockets, RTA_U16 const socket_port)
{
    RTA_UNUSED_PARAM(max_sockets);
    RTA_UNUSED_PARAM(socket_port);

    return(RTA_USERSOCK_RETURNCODE_ERROR_NOT_IMPLEMENTED);
}

RTA_U16 const rta_usersock_get_max_sockets_per_port(RTA_U16 const socket_port)
{
    RTA_UNUSED_PARAM(socket_port);

    return(0);
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */
static RTA_Usersock_ReturnCode local_init_socket (RTA_Usersock_Socket * const p_user_sock)
{
    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    memset(p_user_sock, 0, sizeof(*p_user_sock));

    p_user_sock->socket_type = RTA_Usersock_Socket_Type_DISABLED;
    p_user_sock->socket_port = RTA_Usersock_PortId_Unused;
    p_user_sock->socket_state = RTA_Usersock_State_Free;
    p_user_sock->socket_aging_allowed = RTA_FALSE;
    p_user_sock->socket_aging_port = RTA_Usersock_PortId_Unused;
    p_user_sock->sockid = RTA_Usersock_SockId_Unused;
    p_user_sock->send_in_prog_flag = RTA_FALSE;
    p_user_sock->close_socket_after_send_flag = RTA_FALSE;
    p_user_sock->remote_port = RTA_Usersock_PortId_Unused;
    p_user_sock->p_buffer = RTA_NULL;

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_init_listening_socket (RTA_Usersock_Listening * const p_user_sock)
{
    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_user_sock->sockid = RTA_Usersock_SockId_Unused;
    p_user_sock->socket_port = RTA_Usersock_PortId_Unused;
    p_user_sock->num_ports = 0;
    
    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static void local_process_socket_timers (RTA_U32 const ms_since_last_call)
{
    RTA_U16 sock_ix;

    if(!ms_since_last_call)
        return;

    for(sock_ix=0; sock_ix<RTA_USERSOCK_MAX_NUM_SOCKETS; sock_ix++)
    {
        if(sockets[sock_ix].sockid == RTA_Usersock_SockId_Unused)
            continue;

        if(sockets[sock_ix].socket_state != RTA_Usersock_State_Connected)
            continue;

        if(sockets[sock_ix].inactivity_watchdog_ms_value == 0)
            continue; /* timeout not used */

        if(sockets[sock_ix].inactivity_watchdog_ms_remaining == 0)
            continue; /* we already timed out */

        if(ms_since_last_call >= sockets[sock_ix].inactivity_watchdog_ms_remaining)
        {
            sockets[sock_ix].inactivity_watchdog_ms_remaining = 0;
            (void)rta_usersock_close_sockid(&sockets[sock_ix]);
        }
        else
        {
            sockets[sock_ix].inactivity_watchdog_ms_remaining -= ms_since_last_call;
        }
    }
}

static RTA_Usersock_ReturnCode local_process_readfds_isset_listening_socket(RTA_Usersock_Listening const * const p_user_sock)
{
    int sock_ix;
    RTA_Usersock_SockId temp_sockid;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    for(sock_ix=0; sock_ix<RTA_USERSOCK_MAX_NUM_SOCKETS; sock_ix++)
    {
        if(sockets[sock_ix].socket_port != p_user_sock->socket_port)
            continue;

        if(sockets[sock_ix].sockid != RTA_Usersock_SockId_Unused)
            continue;

        if(sockets[sock_ix].socket_type != RTA_Usersock_Socket_Type_TCPSERVER)
            continue;

        if(sockets[sock_ix].socket_state != RTA_Usersock_State_Listening)
            continue;

        /* we found a socket that meets our criteria */
        break;
    }

    if(sock_ix >= RTA_USERSOCK_MAX_NUM_SOCKETS)
    {
        /* since we don't have a free socket, just close the new socket attempt */
        if (netconn_accept(p_user_sock->sockid, &temp_sockid) == ERR_OK)
        {
            netconn_delete(temp_sockid);
        }
        return(RTA_USERSOCK_RETURNCODE_SUCCESS);
    }

    if (netconn_accept(p_user_sock->sockid, &sockets[sock_ix].sockid) == ERR_OK)
    {
        if (sockets[sock_ix].sockid != NULL)
        {
            // netconn_recv timeout, in ms, needs LWIP_SO_RCVTIMEO = 1 in lwipopts.h
            netconn_set_recvtimeout(sockets[sock_ix].sockid, 1);

            ip_addr_t ip;

            netconn_getaddr(sockets[sock_ix].sockid, &ip, &sockets[sock_ix].remote_port, 0);    // 0 for remote
            sockets[sock_ix].socket_state = RTA_Usersock_State_Connected;
            sockets[sock_ix].remote_ipaddr = ntohl(ip.addr);

            return(local_onAccept(&sockets[sock_ix]));
        }
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_process_readfds_isset_udp(RTA_Usersock_Socket * const p_user_sock)
{
    RTA_Usersock_ReturnCode rc;
    RTA_Size_Type max_data;

    struct netbuf *pNetBuf;
    portCHAR *pcRxString;
    unsigned portSHORT usLength;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(!p_user_sock->callback.setBuffer_fptr)
        return(RTA_USERSOCK_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

    rc = p_user_sock->callback.setBuffer_fptr(p_user_sock);    
    if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);

    if(!p_user_sock->p_buffer)
        return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);

    max_data = rta_usersock_get_max_data_for_buffer(p_user_sock->p_buffer);
    if(max_data == 0)
        return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);

    // Thread is blocked here (netconn_recv)...
    if (netconn_recv(p_user_sock->sockid, &pNetBuf) == ERR_OK)
    {
        netbuf_data(pNetBuf, (void *)&pcRxString, &usLength);

        if (pcRxString && usLength > 0 && usLength <= max_data)
        {
            ip_addr_t * p_ip = netbuf_fromaddr(pNetBuf);
            p_user_sock->remote_ipaddr = ntohl(p_ip->addr);
            p_user_sock->remote_port = netbuf_fromport(pNetBuf);

            p_user_sock->p_buffer->data_size += (RTA_U16)usLength;
            memcpy((char *)p_user_sock->p_buffer->p_current_data, pcRxString, usLength);
            rc = local_onData(p_user_sock);
            if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
        }

        netbuf_delete(pNetBuf);
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);

}

static RTA_Usersock_ReturnCode local_process_readfds_isset_tcp(RTA_Usersock_Socket * const p_user_sock)
{
    RTA_Usersock_ReturnCode rc;
    RTA_Size_Type max_data;

    struct netbuf *pNetBuf;
    portCHAR *pcRxString;
    unsigned portSHORT usLength;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(!p_user_sock->callback.setBuffer_fptr)
        return(RTA_USERSOCK_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

    rc = p_user_sock->callback.setBuffer_fptr(p_user_sock);    
    if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);

    if(!p_user_sock->p_buffer)
        return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);

    max_data = rta_usersock_get_max_data_for_buffer(p_user_sock->p_buffer);
    if (max_data == 0)
        return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);

    // Thread is blocked here (netconn_recv)...
    err_t err = netconn_recv(p_user_sock->sockid, &pNetBuf);
    if (err == ERR_OK)
    {
        netbuf_data(pNetBuf, (void *)&pcRxString, &usLength);

        if (pcRxString && usLength > 0 && usLength <= max_data)
        {
            //p_user_sock->remote_ipaddr = netbuf_fromaddr(pNetBuf);
            //p_user_sock->remote_port = netbuf_fromport(pNetBuf);

            p_user_sock->p_buffer->data_size += (RTA_U16)usLength;
            memcpy((char *)p_user_sock->p_buffer->p_current_data, pcRxString, usLength);
            rc = local_onData(p_user_sock);
            if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS)
            {
                netbuf_delete(pNetBuf);
                return(rc);
            }
        }

        netbuf_delete(pNetBuf);
    }
    else if (err == ERR_TIMEOUT)
    {
        // do nothing
    }
    else
    {
        (void)rta_usersock_close_sockid(p_user_sock); /* don't trump the error code */
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_process_writefds_isset_tcp(RTA_Usersock_Socket * const p_user_sock)
{
    RTA_Usersock_ReturnCode rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    switch(p_user_sock->socket_type)
    {
        case RTA_Usersock_Socket_Type_TCPSERVER:
            p_user_sock->send_in_prog_flag = RTA_FALSE;
            
            rc = local_onSent(p_user_sock);
            if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
            break;

        case RTA_Usersock_Socket_Type_UDP:  
        case RTA_Usersock_Socket_Type_TCPCLIENT:
        case RTA_Usersock_Socket_Type_RAW:
        case RTA_Usersock_Socket_Type_DISABLED:
        default:
            return(RTA_USERSOCK_RETURNCODE_ERROR_INTERNAL);
    }   

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_send_udp_data (RTA_Usersock_Socket const * const p_user_sock, RTA_U8 const * const p_data, RTA_Size_Type const length_in_bytes)
{
    struct netbuf *pBuf;
    char * rsp;

    if(!p_user_sock || !p_data || 0==length_in_bytes)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    pBuf = netbuf_new();
    if (!pBuf)
    {
        return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);
    }
    rsp = netbuf_alloc(pBuf, length_in_bytes);
    if (!rsp)
    {
        return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);
    }

    memcpy(rsp, p_data, length_in_bytes);
    ip_addr_t remote_ip;
    remote_ip.addr = htonl(p_user_sock->remote_ipaddr);
    err_t err = netconn_sendto(p_user_sock->sockid, pBuf, &remote_ip, p_user_sock->remote_port);

    // deallocate response packet buffer
    netbuf_delete(pBuf);

    if (err != ERR_OK)
    {
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_send_tcp_data (RTA_Usersock_Socket const * const p_user_sock, RTA_U8 const * const p_data, RTA_Size_Type const length_in_bytes)
{
    if(!p_user_sock || !p_data || 0==length_in_bytes)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if (netconn_write(p_user_sock->sockid, p_data, length_in_bytes, NETCONN_COPY) != ERR_OK)
    {
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_onAccept (RTA_Usersock_Socket * const p_user_sock)
{
    RTA_Usersock_ReturnCode rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_user_sock->callback.onAccept_fptr)
    {
        rc = p_user_sock->callback.onAccept_fptr(p_user_sock);
        if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
    }

    if(p_user_sock->inactivity_watchdog_ms_value)
        p_user_sock->inactivity_watchdog_ms_remaining = p_user_sock->inactivity_watchdog_ms_value;

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_onData (RTA_Usersock_Socket * const p_user_sock)
{
    RTA_Usersock_ReturnCode rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_user_sock->callback.onData_fptr)
    {
        rc = p_user_sock->callback.onData_fptr(p_user_sock);
        if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_onSent (RTA_Usersock_Socket * const p_user_sock)
{
    RTA_Usersock_ReturnCode rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if (p_user_sock->close_socket_after_send_flag)
    {
        p_user_sock->close_socket_after_send_flag = RTA_FALSE;
        (void)rta_usersock_close_sockid(p_user_sock);
        return(RTA_USERSOCK_RETURNCODE_SUCCESS); /* nothing more to do since we closed the socket */
    }

    if(p_user_sock->callback.onSent_fptr)
    {
        rc = p_user_sock->callback.onSent_fptr(p_user_sock);
        if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_onClose (RTA_Usersock_Socket const * const p_user_sock)
{
    RTA_Usersock_ReturnCode rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_user_sock->callback.onClose_fptr)
    {
        rc = p_user_sock->callback.onClose_fptr(p_user_sock);
        if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS) return(rc);
    }

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Size_Type rta_usersock_get_max_data_for_buffer (RTA_Usersock_Buffer const * const p_buffer)
{
    RTA_Size_Type max_data, sizeof_buffer;

    if(!p_buffer)
        return(0);

    sizeof_buffer = sizeof(p_buffer->data);

    if(p_buffer->data_size > sizeof_buffer)
        return(0);

    max_data = (sizeof_buffer - p_buffer->data_size);

    return(max_data);
}

RTA_Usersock_ReturnCode rta_usersock_get_tcp_remote_ipaddr(RTA_Usersock_Socket const * const p_user_sock, RTA_U32 * const p_remote_ipaddr)
{
    if (!p_user_sock || !p_remote_ipaddr)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);
    
    *p_remote_ipaddr = p_user_sock->remote_ipaddr;

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_set_inactivity_watchdog_ms(RTA_Usersock_Socket * const p_user_sock, RTA_U32 const inactivity_watchdog_ms)
{
    if (!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_user_sock->inactivity_watchdog_ms_value = inactivity_watchdog_ms;
    p_user_sock->inactivity_watchdog_ms_remaining = inactivity_watchdog_ms;

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

RTA_Usersock_ReturnCode rta_usersock_reset_inactivity_watchdog(RTA_Usersock_Socket * const p_user_sock)
{
    if (!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_user_sock->inactivity_watchdog_ms_remaining = p_user_sock->inactivity_watchdog_ms_value;

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
    
}

/* *********** */
/* END OF FILE */
/* *********** */
