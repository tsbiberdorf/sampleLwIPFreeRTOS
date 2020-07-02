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

 #ifndef __RTA_USERSOCK_H__
 #define __RTA_USERSOCK_H__

/*
 * The argument n specifies the length of the queue for pending connections. When the queue fills, new clients
 * attempting to connect fail with ECONNREFUSED until the server calls accept to accept a connection from the
 * queue.
*/
#define MAX_NUMLISTEN_PENDING_CNXN	1

typedef struct netconn * RTA_Usersock_SockId;
#define RTA_Usersock_SockId_Unused  ((RTA_Usersock_SockId)RTA_NULL)

typedef RTA_U16 RTA_Usersock_PortId;
#define RTA_Usersock_PortId_Unused ((RTA_Usersock_PortId)0)

#define RTA_USERSOCK_BUFFER_MAX_SIZE 600
typedef struct
{
    RTA_BYTE *      p_current_data;
    RTA_Size_Type   data_size;
    RTA_BYTE        data[RTA_USERSOCK_BUFFER_MAX_SIZE];
}RTA_Usersock_Buffer;

/* RFC 3168 */
typedef struct
{
    RTA_U8 dscp; /* 6 most-significant bits */
    RTA_U8 ecn; /* 2 least-significant bits */
}RTA_Usersock_TOS;

/*  Runtime settings can dictate the number of each socket per port (i.e. 502 for Modbus TCP; 44818 for EtherNet/IP) 
    If ModbusTCP and EtherNet/IP are running, then 2 TCPSERVER ports are needed. 
    EtherNet/IP needs 2 UDP ports (one for I/O and one for encapsulation). 

    We want to keep track of some number of sockets per each port number to ensure we have enough resources. 
    We don't want one protocol to starve out another protocol.
*/
#define RTA_USERSOCK_MAX_NUM_PORTS_TCPSERVER    1
#define RTA_USERSOCK_MAX_NUM_PORTS_TCPCLIENT    0
#define RTA_USERSOCK_MAX_NUM_PORTS_UDP          2
#define RTA_USERSOCK_MAX_NUM_PORTS_RAW          0

/* This is the total number of sockets across all protocols and types (tcp/udp/raw) */
#define RTA_USERSOCK_MAX_NUM_SOCKETS    5

typedef enum 
{
    RTA_USERSOCK_RETURNCODE_SUCCESS,
    RTA_USERSOCK_RETURNCODE_ABORTED_PROCESS,                    /* message processing was successfully interrupted (like pass through) */
    RTA_USERSOCK_RETURNCODE_ERROR_INVALID_PARAMETER,            /* One or more passed parameter is invalid */
    RTA_USERSOCK_RETURNCODE_ERROR_INVALID_STATE,                /* socket state is invalid for operation */
    RTA_USERSOCK_RETURNCODE_ERROR_INVALID_SOCKET,               /* socket id is invalid for operation */
    RTA_USERSOCK_RETURNCODE_ERROR_INVALID_SOCKET_TYPE,          /* socket type is invalid for operation */
    RTA_USERSOCK_RETURNCODE_ERROR_MALLOC_FAILED,                /* Pointer problem */
    RTA_USERSOCK_RETURNCODE_ERROR_INTERNAL,                     /* Unexpected path reached (i.e. switch default case) */
    RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_FAILED,              /* Function failed to do described task (somewhat of a catch all) */
    RTA_USERSOCK_RETURNCODE_ERROR_FUNCTION_TIMEDOUT,            /* Blocking function timed out */
    RTA_USERSOCK_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED,   /* If a function has optional pointer, that is needed but not provided (like an error pointer) */
    RTA_USERSOCK_RETURNCODE_ERROR_NOT_IMPLEMENTED               /* Not implemented code */
}RTA_Usersock_ReturnCode;

typedef enum 
{
    RTA_Usersock_Socket_Type_DISABLED,
    RTA_Usersock_Socket_Type_TCPSERVER,
    RTA_Usersock_Socket_Type_TCPCLIENT,
    RTA_Usersock_Socket_Type_UDP,
    RTA_Usersock_Socket_Type_RAW
}RTA_Usersock_Socket_Type;

typedef enum 
{
    RTA_Usersock_State_Free,
    RTA_Usersock_State_Configuring,
    RTA_Usersock_State_Listening,
    RTA_Usersock_State_AttemptingToConnect,
    RTA_Usersock_State_Connected
}RTA_Usersock_State;

/* forward declaration of socket control structure */
typedef struct RTA_Usersock_Socket_Struct_t RTA_Usersock_Socket;
#define RTA_Usersock_Socket_Unused RTA_NULL

typedef struct
{
    RTA_Usersock_ReturnCode (*setBuffer_fptr)  (RTA_Usersock_Socket * const user_sock);
    RTA_Usersock_ReturnCode (*onAccept_fptr)   (RTA_Usersock_Socket const * const user_sock);
    RTA_Usersock_ReturnCode (*onData_fptr)	   (RTA_Usersock_Socket * const user_sock);
    RTA_Usersock_ReturnCode (*onSent_fptr)     (RTA_Usersock_Socket const * const user_sock);
    RTA_Usersock_ReturnCode (*onClose_fptr)    (RTA_Usersock_Socket const * const user_sock);
    RTA_Usersock_ReturnCode (*onTimeout_fptr)  (RTA_Usersock_Socket const * const user_sock);
    RTA_Usersock_ReturnCode (*onConnect_fptr)  (RTA_Usersock_Socket const * const user_sock);
}RTA_USERSOCK_CALLBACK_STRUCT;

/* one structure that has everything that we need need for sockets */
struct RTA_Usersock_Socket_Struct_t
{
	RTA_Usersock_Socket_Type    socket_type;
    RTA_Usersock_PortId         socket_port;
    RTA_Usersock_State          socket_state;
    RTA_Usersock_TOS            ip_tos;

    /* if enabled, automatically close the oldest socket if a new socket is needed */
    RTA_BOOL            socket_aging_allowed;
    RTA_Usersock_PortId socket_aging_port;      /* if !RTA_Usersock_PortId_Unused, match port to find oldest */
	RTA_U32             socket_age;             

    /* if value is non-zero, count down to close the socket */
	RTA_U32 inactivity_watchdog_ms_value;
    RTA_U32 inactivity_watchdog_ms_remaining;

    /* socket control fields */
    RTA_USERSOCK_CALLBACK_STRUCT callback;
    RTA_Usersock_SockId sockid;
    RTA_U8 	send_in_prog_flag;     
    RTA_U8 	close_socket_after_send_flag;

    RTA_U32 remote_ipaddr;
    RTA_Usersock_PortId remote_port;

    RTA_Usersock_Buffer * p_buffer;
};

typedef struct
{
    RTA_Usersock_SockId sockid;
    RTA_Usersock_PortId socket_port;
    RTA_U16 num_ports;
}RTA_Usersock_Listening;

RTA_Usersock_ReturnCode rta_usersock_init (void);
RTA_Usersock_ReturnCode rta_usersock_process (RTA_U32 const ms_since_last_call);
RTA_Usersock_Socket *   rta_usersock_allocate_socket (RTA_Usersock_Socket_Type const socket_type, RTA_Usersock_PortId const socket_port);
RTA_Usersock_ReturnCode rta_usersock_free_socket(RTA_Usersock_Socket * const p_user_sock);
RTA_Usersock_ReturnCode rta_usersock_register_callbacks (RTA_Usersock_Socket * const p_user_sock, RTA_USERSOCK_CALLBACK_STRUCT const * const callback_struct_ptr);
RTA_Usersock_ReturnCode rta_usersock_open_udp_sockid (RTA_Usersock_Socket * const p_user_sock);
RTA_Usersock_ReturnCode rta_usersock_open_tcp_sockid (RTA_Usersock_Socket * const p_user_sock);
RTA_Usersock_ReturnCode rta_usersock_start_tcp_listening (RTA_Usersock_PortId const socket_port, RTA_U16 const num_ports);
RTA_Usersock_ReturnCode rta_usersock_stop_tcp_listening (RTA_Usersock_PortId const socket_port);
RTA_Usersock_ReturnCode rta_usersock_close_sockid (RTA_Usersock_Socket * const p_user_sock);
RTA_Usersock_ReturnCode rta_usersock_close_all_by_type_and_port (RTA_Usersock_Socket_Type const type, RTA_Usersock_PortId const port);
RTA_Usersock_ReturnCode rta_usersock_close_all (void); /* nuclear option */
RTA_Usersock_ReturnCode rta_usersock_send_data (RTA_Usersock_Socket * const p_user_sock, RTA_U8 const * const p_data, RTA_Size_Type const length_in_bytes);
RTA_Usersock_ReturnCode rta_usersock_set_max_sockets_per_port(RTA_U16 const max_sockets, RTA_Usersock_PortId const socket_port);
RTA_U16 const rta_usersock_get_max_sockets_per_port(RTA_Usersock_PortId const socket_port);
RTA_Usersock_State const rta_usersock_get_state (RTA_Usersock_Socket const * const p_user_sock);
RTA_Size_Type rta_usersock_get_max_data_for_buffer (RTA_Usersock_Buffer const * const p_buffer);
RTA_Usersock_ReturnCode rta_usersock_get_tcp_remote_ipaddr(RTA_Usersock_Socket const * const p_user_sock, RTA_U32 * const p_remote_ipaddr);
RTA_Usersock_ReturnCode rta_usersock_set_inactivity_watchdog_ms(RTA_Usersock_Socket * const p_user_sock, RTA_U32 const inactivity_watchdog_ms);
RTA_Usersock_ReturnCode rta_usersock_reset_inactivity_watchdog(RTA_Usersock_Socket * const p_user_sock);

#endif /* __RTA_USERSOCK_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
