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

 #ifndef __EIP_ENCAP_H__
 #define __EIP_ENCAP_H__

#define EIP_ENCAP_PROTOCOL_VER 1

/* CIP Volume 2 (EtherNet/IP) - Table 2-3.1 Encapsulation Packet */
#define EIP_ENCAP_OFFSET_COMMAND    0
#define EIP_ENCAP_OFFSET_LENGTH     2

/* ****************************************************** */
/* CIP Volume 2 (EtherNet/IP) - Table 2 - 3.3 Error Codes */
/* ****************************************************** */
#define EIP_ENCAP_STATUS_SUCCESS                0x0000
#define EIP_ENCAP_STATUS_INVALID_COMMAND        0x0001  /* The sender issued an invalid or unsupported encapsulation command. */
#define EIP_ENCAP_STATUS_NO_MEMORY              0x0002  /* Insufficient memory resources in the receiver to handle the command. */
#define EIP_ENCAP_STATUS_INCORRECT_DATA         0x0003  /* Poorly formed or incorrect data in the data portion of the encapsulation message. */
#define EIP_ENCAP_STATUS_INVALID_HANDLE         0x0064  /* An originator used an invalid session handle when sending an encapsulation message to the target. */
#define EIP_ENCAP_STATUS_INVALID_LENGTH         0x0065  /* The target received a message of invalid length. */
#define EIP_ENCAP_STATUS_BAD_PROTOCOL_VERSION   0x0069  /* Unsupported encapsulation protocol version. */
#define EIP_ENCAP_STATUS_CIP_NOT_ON_THIS_PORT   0x006A  /* Encapsulated CIP service not allowed on this port. */
#define EIP_ENCAP_STATUS_DISCARD_PACKET         0xFFFF  /* Special code to tell stack to not send a response */

typedef struct
{
    CIP_UDINT   SessionHandle;
    RTA_Usersock_Socket const * p_socket;
    
    CIP_ExplicitConnection * p_ExplicitConnection;
}EIP_SessionStruct;
#define EIP_ENCAP_NUM_SESSIONS  EIP_USERSOCK_MAX_NUM_TCP

CIP_ReturnCode eip_encap_init (CIP_InitType const init_type);
CIP_ReturnCode eip_encap_process (RTA_U32 const ms_since_last_call);
CIP_ReturnCode eip_encap_process_message (EIP_ControlStruct * const p_cip_ctrl);

CIP_ReturnCode eip_encap_build_error(EIP_Encap_Header * const p_Encap_Header, CIP_UDINT const Status);

EIP_SessionStruct * eip_encap_find_Session_from_p_socket (RTA_Usersock_Socket const * const p_socket);
EIP_SessionStruct * eip_encap_find_Session_from_SessionHandle (CIP_UDINT const SessionHandle);
EIP_SessionStruct * eip_encap_allocate_Session_from_p_socket (RTA_Usersock_Socket const * const p_socket);
void eip_encap_free_Session_from_p_socket (RTA_Usersock_Socket const * const p_socket);
void eip_encap_free_ExplicitConnection_from_SessionHandle(CIP_UDINT const SessionHandle);

#endif /* __EIP_ENCAP_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
