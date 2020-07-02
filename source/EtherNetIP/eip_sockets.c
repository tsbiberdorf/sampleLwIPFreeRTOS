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
#include "eip_system.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "eip_socket.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */
static CIP_ReturnCode local_init_udp_socket (void);
static CIP_ReturnCode local_init_udp_io_socket (void);
static CIP_ReturnCode local_init_tcp_sockets (void);
static CIP_ReturnCode local_update_buffer_sizes (RTA_Usersock_Buffer * const p_buffer);

static RTA_Usersock_ReturnCode local_set_udp_buffer (RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_set_udp_io_buffer (RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_set_tcp_buffer (RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_on_encap_data (RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_on_udp_io_data (RTA_Usersock_Socket const * const p_user_sock);
static RTA_Usersock_ReturnCode local_on_tcp_accept (RTA_Usersock_Socket * const p_user_sock);
static RTA_Usersock_ReturnCode local_on_tcp_close (RTA_Usersock_Socket const * const p_user_sock);
static RTA_Usersock_ReturnCode local_on_tcp_timeout (RTA_Usersock_Socket const * const p_user_sock);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

typedef struct
{
    RTA_Usersock_Socket * p_socket;
    RTA_Usersock_Buffer buffer;
}EIP_Socket_with_Buffer;

EIP_Socket_with_Buffer usersock_udp;
EIP_Socket_with_Buffer usersock_udp_io;
EIP_Socket_with_Buffer usersock_tcp[EIP_USERSOCK_MAX_NUM_TCP];

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode eip_sockets_init (CIP_InitType const init_type)
{
    CIP_ReturnCode rc;

    EIP_LOG_FUNCTION_ENTER;

    switch(init_type)
    {
        case CIP_INITTYPE_NORMAL:
        case CIP_INITTYPE_OUTOFBOX:
        case CIP_INITTYPE_OUTOFBOX_NOCOMMS:
        case CIP_INITTYPE_MFR:
        default:
            break;
    }

    rc = local_init_udp_socket();
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = local_init_tcp_sockets();
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = local_init_udp_io_socket();
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

void eip_sockets_shutdown (void)
{
    RTA_U16 sock_ix;

    if(usersock_udp.p_socket)
        (void)rta_usersock_free_socket(usersock_udp.p_socket);

    if(usersock_udp_io.p_socket)
        (void)rta_usersock_free_socket(usersock_udp_io.p_socket);

    for (sock_ix = 0; sock_ix < EIP_USERSOCK_MAX_NUM_TCP; sock_ix++)
    {
        if (usersock_tcp[sock_ix].p_socket)
        {
            (void)rta_usersock_free_socket(usersock_tcp[sock_ix].p_socket);
        }
    }

    (void)rta_usersock_stop_tcp_listening(EIP_USERSOCK_PORT_TCP);
}

CIP_ReturnCode eip_sockets_process (RTA_U32 const ms_since_last_call)
{
    RTA_UNUSED_PARAM(ms_since_last_call);

    return(CIP_RETURNCODE_SUCCESS);
}

void eip_sockets_close_tcp_socket (RTA_Usersock_Socket const * const p_socket)
{
    RTA_U16 sock_ix;

    if (!p_socket)
        return;

    for (sock_ix = 0; sock_ix < EIP_USERSOCK_MAX_NUM_TCP; sock_ix++)
    {
        if (usersock_tcp[sock_ix].p_socket == p_socket)
        {
            (void)rta_usersock_close_sockid(usersock_tcp[sock_ix].p_socket);
        }
    }
}

void eip_sockets_update_all_tcp_timeouts (RTA_U16 const timeout_seconds)
{
    RTA_U16 sock_ix;
    RTA_U32 timeout_milliseconds;

    timeout_milliseconds = (1000UL * timeout_seconds);

    for (sock_ix = 0; sock_ix < EIP_USERSOCK_MAX_NUM_TCP; sock_ix++)
    {
        if (usersock_tcp[sock_ix].p_socket)
        {
            (void)rta_usersock_set_inactivity_watchdog_ms(usersock_tcp[sock_ix].p_socket, timeout_milliseconds);
        }
    }
}

void eip_sockets_close_all_tcp_by_ip_address (RTA_U32 const ip_address)
{
    RTA_U16 sock_ix;
    
    for (sock_ix = 0; sock_ix < EIP_USERSOCK_MAX_NUM_TCP; sock_ix++)
    {
        if (usersock_tcp[sock_ix].p_socket->socket_type != RTA_Usersock_Socket_Type_TCPSERVER)
            continue;

        if (usersock_tcp[sock_ix].p_socket->socket_state != RTA_Usersock_State_Connected)
            continue;

        if (usersock_tcp[sock_ix].p_socket->remote_ipaddr != ip_address)
            continue;

        (void)rta_usersock_close_sockid(usersock_tcp[sock_ix].p_socket);
    }
}

CIP_ReturnCode eip_sockets_send_udp_io (RTA_U32 const ip_address, RTA_U16 const port, RTA_U8 const * const p_data, RTA_Size_Type const length_in_bytes, RTA_U8 const qos_tos)
{
    EIP_LOG_FUNCTION_ENTER;

    if(!usersock_udp_io.p_socket)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    usersock_udp_io.p_socket->remote_ipaddr = ip_address;
    usersock_udp_io.p_socket->remote_port = port;
    usersock_udp_io.p_socket->ip_tos.dscp = qos_tos;

    if(rta_usersock_send_data (usersock_udp_io.p_socket, p_data, length_in_bytes) != RTA_USERSOCK_RETURNCODE_SUCCESS)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);        
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */
static CIP_ReturnCode local_update_buffer_sizes (RTA_Usersock_Buffer * const p_buffer)
{
    if(!p_buffer)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_buffer->data_size >= sizeof(p_buffer->data))
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    p_buffer->p_current_data = &p_buffer->data[p_buffer->data_size];

    return(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_init_udp_socket (void)
{
    RTA_Usersock_ReturnCode rc;
    RTA_USERSOCK_CALLBACK_STRUCT callback;

    EIP_LOG_FUNCTION_ENTER;

    callback.setBuffer_fptr = local_set_udp_buffer;
    callback.onAccept_fptr = RTA_NULL;
    callback.onClose_fptr = RTA_NULL;
    callback.onConnect_fptr = RTA_NULL;
    callback.onData_fptr = local_on_encap_data;
    callback.onSent_fptr = RTA_NULL;
    callback.onTimeout_fptr = RTA_NULL;
    
    memset(&usersock_udp, 0, sizeof(usersock_udp));

    usersock_udp.p_socket = rta_usersock_allocate_socket(RTA_Usersock_Socket_Type_UDP, EIP_USERSOCK_PORT_UDP_ENCAP);
    if(usersock_udp.p_socket == RTA_NULL)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    
    rc = rta_usersock_register_callbacks(usersock_udp.p_socket, &callback);
    if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

    rc = rta_usersock_open_udp_sockid(usersock_udp.p_socket);
    if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_init_udp_io_socket (void)
{
    RTA_Usersock_ReturnCode rc;
    RTA_USERSOCK_CALLBACK_STRUCT callback;

    EIP_LOG_FUNCTION_ENTER;

    callback.setBuffer_fptr = local_set_udp_io_buffer;
    callback.onAccept_fptr = RTA_NULL;
    callback.onClose_fptr = RTA_NULL;
    callback.onConnect_fptr = RTA_NULL;
    callback.onData_fptr = local_on_udp_io_data;
    callback.onSent_fptr = RTA_NULL;
    callback.onTimeout_fptr = RTA_NULL;
    
    usersock_udp_io.p_socket = rta_usersock_allocate_socket(RTA_Usersock_Socket_Type_UDP, EIP_USERSOCK_PORT_UDP_IO);
    if(usersock_udp_io.p_socket == RTA_NULL)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    
    rc = rta_usersock_register_callbacks(usersock_udp_io.p_socket, &callback);
    if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

    rc = rta_usersock_open_udp_sockid(usersock_udp_io.p_socket);
    if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}
 
static CIP_ReturnCode local_init_tcp_sockets (void)
{
    RTA_Usersock_ReturnCode rc;
    RTA_USERSOCK_CALLBACK_STRUCT callback;
    RTA_U16 tcp_ix;
    RTA_U16 watch_dog_seconds;
    RTA_U32 watch_dog_milliseconds;

    EIP_LOG_FUNCTION_ENTER;

    watch_dog_seconds = eip_userobj_tcp_get_EncapsulationInactivityTimeout_in_seconds();
    watch_dog_milliseconds = (1000UL * watch_dog_seconds);

    callback.setBuffer_fptr = local_set_tcp_buffer;
    callback.onAccept_fptr = local_on_tcp_accept;
    callback.onClose_fptr = local_on_tcp_close;
    callback.onConnect_fptr = RTA_NULL;
    callback.onData_fptr = local_on_encap_data;
    callback.onSent_fptr = RTA_NULL;
    callback.onTimeout_fptr = local_on_tcp_timeout;

    for(tcp_ix=0; tcp_ix<EIP_USERSOCK_MAX_NUM_TCP; tcp_ix++)
    {
        usersock_tcp[tcp_ix].p_socket = rta_usersock_allocate_socket(RTA_Usersock_Socket_Type_TCPSERVER, EIP_USERSOCK_PORT_TCP);
        if(usersock_tcp[tcp_ix].p_socket == RTA_NULL)
            EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    
        rc = rta_usersock_register_callbacks(usersock_tcp[tcp_ix].p_socket, &callback);
        if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS)
            EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

        rc = rta_usersock_open_tcp_sockid(usersock_tcp[tcp_ix].p_socket);
        if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS)
            EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

        (void)rta_usersock_set_inactivity_watchdog_ms(usersock_tcp[tcp_ix].p_socket, watch_dog_milliseconds);
    }

    rc = rta_usersock_start_tcp_listening(EIP_USERSOCK_PORT_TCP, EIP_USERSOCK_MAX_NUM_TCP);
    if(rc != RTA_USERSOCK_RETURNCODE_SUCCESS)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_set_udp_buffer (RTA_Usersock_Socket * const p_user_sock)
{
    CIP_ReturnCode rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(usersock_udp.p_socket != p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);

    p_user_sock->p_buffer = &usersock_udp.buffer;
    
    rc = local_update_buffer_sizes(p_user_sock->p_buffer);
    if(rc != CIP_RETURNCODE_SUCCESS)
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
        
    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_set_udp_io_buffer (RTA_Usersock_Socket * const p_user_sock)
{
    CIP_ReturnCode rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(usersock_udp_io.p_socket != p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);

    p_user_sock->p_buffer = &usersock_udp_io.buffer;

    rc = local_update_buffer_sizes(p_user_sock->p_buffer);
    if(rc != CIP_RETURNCODE_SUCCESS)
        return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
    
    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_set_tcp_buffer (RTA_Usersock_Socket * const p_user_sock)
{
    RTA_U16 sock_ix;
    CIP_ReturnCode rc;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    for(sock_ix=0; sock_ix<EIP_USERSOCK_MAX_NUM_TCP; sock_ix++)
    {
        if(usersock_tcp[sock_ix].p_socket != p_user_sock)
            continue;

        p_user_sock->p_buffer = &usersock_tcp[sock_ix].buffer;    
        rc = local_update_buffer_sizes(p_user_sock->p_buffer);
        if(rc != CIP_RETURNCODE_SUCCESS)
            return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);

        return(RTA_USERSOCK_RETURNCODE_SUCCESS);
    }
    
    return(RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED);
}

static RTA_Usersock_ReturnCode local_on_encap_data (RTA_Usersock_Socket * const p_user_sock)
{
    CIP_ReturnCode rc;
    EIP_ControlStruct eip_ctrl;
    CIP_ControlStruct * p_cip_ctrl;
#if EIP_USER_OPTION_QOS_ENABLED
    CIP_QoSObject QoSObject;
#endif /* #if EIP_USER_OPTION_QOS_ENABLED */

    RTA_U16 ii;
    RTA_U16 expected_length, remaining_length;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(!p_user_sock->p_buffer)
        return(RTA_USERSOCK_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

    /* TODO: what to log here? */

    /* ************** */
    /* Allocate Heaps */
    /* ************** */
    memset(&eip_ctrl, 0, sizeof(eip_ctrl));
    p_cip_ctrl = &eip_ctrl.cip_ctrl;
    p_cip_ctrl->p_heap_src = rta_memory_reserve_heap(RTA_USER_MEMORY_ALIGNMENT_NETWORK);
    p_cip_ctrl->p_heap_dst = rta_memory_reserve_heap(RTA_USER_MEMORY_ALIGNMENT_NETWORK);
    p_cip_ctrl->decode_GeneralStatus = CIP_Status_Success;

    eip_ctrl.p_socket = p_user_sock;

    if((p_cip_ctrl->p_heap_dst == RTA_NULL) || (p_cip_ctrl->p_heap_src == RTA_NULL))
    {
        rta_memory_abandon_heap(p_cip_ctrl->p_heap_src);
        rta_memory_abandon_heap(p_cip_ctrl->p_heap_dst);
        return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);
    }
    
    /* Process all messages (some are back to back) */
    for(;;)
    {        
        if(p_user_sock->p_buffer->data_size < EIP_ENCAP_HEADER_ENCODED_SIZE)
            break;

        expected_length = (RTA_U16)(rta_endian_GetLitEndian16(&p_user_sock->p_buffer->data[EIP_ENCAP_OFFSET_LENGTH]) + EIP_ENCAP_HEADER_ENCODED_SIZE);

        if(expected_length > p_user_sock->p_buffer->data_size)
            break;

        /* *********************************************** */
        /* copy receive message from socket buffer to heap */
        /* *********************************************** */
        for(ii=0; ii<expected_length; ii++)
        {    
            rc = cip_encode_CIP_BYTE(p_cip_ctrl, &p_user_sock->p_buffer->data[ii]);
            if(rc != CIP_RETURNCODE_SUCCESS)
                return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);
        }
    
        rc = cip_binary_swap_heaps(p_cip_ctrl, RTA_USER_MEMORY_ALIGNMENT_INTERNAL);
        if(rc != CIP_RETURNCODE_SUCCESS) return(RTA_USERSOCK_RETURNCODE_ERROR_INTERNAL);

        /* *************************************** */
        /* process message (decode/process/encode) */
        /* *************************************** */
        (void)rta_usersock_reset_inactivity_watchdog(p_user_sock);
        rc = eip_encap_process_message (&eip_ctrl); /* delay rc process until we free the heaps */

        /* ************************************************ */
        /* copy transmit message from heap to socket buffer */
        /* ************************************************ */
        if(rc == CIP_RETURNCODE_SUCCESS)
        {
            if(p_cip_ctrl->p_heap_dst->bytes_used)
            {
                if(p_cip_ctrl->p_fptr_call_on_send_complete)
                    p_user_sock->callback.onSent_fptr = p_cip_ctrl->p_fptr_call_on_send_complete;
                else 
                    p_user_sock->callback.onSent_fptr = RTA_NULL;

#if EIP_USER_OPTION_QOS_ENABLED
                if(eip_obj_qos_get_CIP_QoSObject (RTA_NULL, &QoSObject) == CIP_RETURNCODE_SUCCESS)
                {
                	/* all TCP and UDP encap messages are treated as explicit for priority */
                	p_user_sock->ip_tos.dscp = QoSObject.DSCP_Explicit;
                }
#endif /* #if EIP_USER_OPTION_QOS_ENABLED */

                if(rta_usersock_send_data (p_user_sock, p_cip_ctrl->p_heap_dst->buffer, (RTA_U16)p_cip_ctrl->p_heap_dst->bytes_used) != RTA_USERSOCK_RETURNCODE_SUCCESS)
                    rc = CIP_RETURNCODE_ERROR_FUNCTION_FAILED;
            }
        }

        if(expected_length <= p_user_sock->p_buffer->data_size)
        {
            remaining_length = (RTA_U16)(p_user_sock->p_buffer->data_size - expected_length);

            for(ii=0; ii<remaining_length; ii++)
                p_user_sock->p_buffer->data[ii] = p_user_sock->p_buffer->data[(ii+expected_length)];
            
            p_user_sock->p_buffer->data_size = remaining_length;
        }
    }

    switch(p_user_sock->socket_type)
    {
        case RTA_Usersock_Socket_Type_UDP:
            p_user_sock->p_buffer->data_size = 0;
            break;

        case RTA_Usersock_Socket_Type_TCPSERVER:
            if(p_user_sock->p_buffer->data_size < EIP_ENCAP_HEADER_ENCODED_SIZE)
                break;

            if(rta_endian_GetLitEndian16(&p_user_sock->p_buffer->data[EIP_ENCAP_OFFSET_COMMAND]) == EIP_Command_NOP)
            {
                /*  NOP is the only required command that can reach the max packet. 
                    Since we may not have room for that, we purge the ignored data to allow
                    us to receive an entire message. Keep the header intact by adjusting the expected size */

                expected_length = rta_endian_GetLitEndian16(&p_user_sock->p_buffer->data[EIP_ENCAP_OFFSET_LENGTH]);
                remaining_length = (RTA_U16)(p_user_sock->p_buffer->data_size - EIP_ENCAP_HEADER_ENCODED_SIZE);

                if(expected_length > remaining_length)
                {
                    expected_length = (RTA_U16)(expected_length - remaining_length);
                    rta_endian_PutLitEndian16(expected_length, &p_user_sock->p_buffer->data[EIP_ENCAP_OFFSET_LENGTH]);
                    p_user_sock->p_buffer->data_size = EIP_ENCAP_HEADER_ENCODED_SIZE;
                }
            }
            break;

        case RTA_Usersock_Socket_Type_DISABLED:
        case RTA_Usersock_Socket_Type_TCPCLIENT:
        case RTA_Usersock_Socket_Type_RAW:
        default:
            return(RTA_USERSOCK_RETURNCODE_ERROR_INTERNAL);
    }
    
    /* ********** */
    /* Free Heaps */
    /* ********** */
    rta_memory_abandon_heap(p_cip_ctrl->p_heap_src);
    rta_memory_abandon_heap(p_cip_ctrl->p_heap_dst);

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_on_udp_io_data (RTA_Usersock_Socket const * const p_user_sock)
{
    CIP_ReturnCode rc;
    EIP_ControlStruct eip_ctrl;
    CIP_ControlStruct * p_cip_ctrl;

    RTA_U16 ii;

    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(!p_user_sock->p_buffer)
        return(RTA_USERSOCK_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

    if(p_user_sock->socket_type != RTA_Usersock_Socket_Type_UDP)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_SOCKET_TYPE);

    /* TODO: what to log here? */

    /* ************** */
    /* Allocate Heaps */
    /* ************** */
    memset(&eip_ctrl, 0, sizeof(eip_ctrl));
    p_cip_ctrl = &eip_ctrl.cip_ctrl;
    p_cip_ctrl->p_heap_src = rta_memory_reserve_heap(RTA_USER_MEMORY_ALIGNMENT_NETWORK);
    p_cip_ctrl->p_heap_dst = rta_memory_reserve_heap(RTA_USER_MEMORY_ALIGNMENT_NETWORK);
    p_cip_ctrl->decode_GeneralStatus = CIP_Status_Success;

    eip_ctrl.p_socket = p_user_sock;
    
    if((p_cip_ctrl->p_heap_dst == RTA_NULL) || (p_cip_ctrl->p_heap_src == RTA_NULL))
    {
        rta_memory_abandon_heap(p_cip_ctrl->p_heap_src);
        rta_memory_abandon_heap(p_cip_ctrl->p_heap_dst);
        return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);
    }

    /* NOTE: Data is validated in the decode step, so pass the entire message */

    /* *********************************************** */
    /* copy receive message from socket buffer to heap */
    /* *********************************************** */
    for(ii=0; ii<p_user_sock->p_buffer->data_size; ii++)
    {    
        rc = cip_encode_CIP_BYTE(p_cip_ctrl, &p_user_sock->p_buffer->data[ii]);
        if(rc != CIP_RETURNCODE_SUCCESS)
            return(RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED);
    }
    
    rc = cip_binary_swap_heaps(p_cip_ctrl, RTA_USER_MEMORY_ALIGNMENT_INTERNAL);
    if(rc != CIP_RETURNCODE_SUCCESS) return(RTA_USERSOCK_RETURNCODE_ERROR_INTERNAL);

    /* *************************************** */
    /* process message (decode/process/encode) */
    /* *************************************** */
    (void)cip_obj_cnxnmgr_implicit_process_message (&eip_ctrl); /* TODO: what to do with an error? */

    /* we should have received the entire message, so discard any remaining data */
    p_user_sock->p_buffer->data_size = 0;
    
    /* ********** */
    /* Free Heaps */
    /* ********** */
    rta_memory_abandon_heap(p_cip_ctrl->p_heap_src);
    rta_memory_abandon_heap(p_cip_ctrl->p_heap_dst);

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_on_tcp_accept (RTA_Usersock_Socket * const p_user_sock)
{
    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

//    RTA_IGNORE_RETURN(rta_usersock_reset_inactivity_watchdog(p_user_sock));

    /* TODO: what to log here? */

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_on_tcp_close (RTA_Usersock_Socket const * const p_user_sock)
{
    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* TODO: what to log here? */

    eip_encap_free_Session_from_p_socket(p_user_sock);

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

static RTA_Usersock_ReturnCode local_on_tcp_timeout (RTA_Usersock_Socket const * const p_user_sock)
{
    if(!p_user_sock)
        return(RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* TODO: what to log here? */

    return(RTA_USERSOCK_RETURNCODE_SUCCESS);
}

/* *********** */
/* END OF FILE */
/* *********** */
