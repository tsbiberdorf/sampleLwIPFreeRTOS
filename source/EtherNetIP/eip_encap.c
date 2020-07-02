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
#define __RTA_FILE__ "eip_encap.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */
static CIP_ReturnCode local_validate_EIP_Encap_Header(EIP_ControlStruct const * const p_cip_ctrl, EIP_Encap_Header const * const p_Encap_Header);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */
static EIP_SessionStruct Session[EIP_ENCAP_NUM_SESSIONS];
static CIP_UDINT NextSessionHandle;

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode eip_encap_init (CIP_InitType const init_type)
{
    RTA_U16 ii;

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

    NextSessionHandle = 1;
    memset(Session, 0, sizeof(Session));
    for(ii=0; ii<EIP_ENCAP_NUM_SESSIONS; ii++)
    {
        Session[ii].p_socket = RTA_Usersock_Socket_Unused;
    }
    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encap_process (RTA_U32 const ms_since_last_call)
{
    RTA_UNUSED_PARAM(ms_since_last_call);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encap_process_message (EIP_ControlStruct * const p_eip_ctrl)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct * p_cip_ctrl;
    EIP_Message EIP;
    CIP_UINT length;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = eip_decode_EIP_Encap_Header (p_eip_ctrl, &EIP.Header);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    
    rc = local_validate_EIP_Encap_Header (p_eip_ctrl, &EIP.Header);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    p_eip_ctrl->p_Encap_Header = &EIP.Header;
    p_cip_ctrl->EIP_SessionHandle = EIP.Header.SessionHandle;

    switch(EIP.Header.Command)
    {
        case EIP_Command_NOP:
            /* no body to decode */ 
            break;

        case EIP_Command_ListServices:
            /* no body to decode */
            break;

        case EIP_Command_ListIdentity:
            /* no body to decode */
            break;

        case EIP_Command_RegisterSession:
            EIP.Request.p_RegisterSession = cip_binary_malloc(p_cip_ctrl, sizeof(*EIP.Request.p_RegisterSession));
            if(!EIP.Request.p_RegisterSession) EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_decode_EIP_Encap_RegisterSession_Request(p_eip_ctrl, EIP.Request.p_RegisterSession);
            if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

       case EIP_Command_UnRegisterSession:
           /* no body to decode */
           break;

        case EIP_Command_SendRRData:           
            EIP.Request.p_SendRRData = cip_binary_malloc(p_cip_ctrl, sizeof(*EIP.Request.p_SendRRData));
            if(!EIP.Request.p_SendRRData) EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_decode_EIP_Encap_SendRRData_Request(p_eip_ctrl, EIP.Request.p_SendRRData);

            if(rc == CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS)  break;

            if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_Command_SendUnitData:
            EIP.Request.p_SendUnitData = cip_binary_malloc(p_cip_ctrl, sizeof(*EIP.Request.p_SendUnitData));
            if (!EIP.Request.p_SendUnitData) EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_decode_EIP_Encap_SendUnitData_Request(p_eip_ctrl, EIP.Request.p_SendUnitData);

            if(rc == CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS)  break;

            if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        default:
            rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_COMMAND);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;
    }

    rc = cip_binary_swap_heaps(p_cip_ctrl, RTA_USER_MEMORY_ALIGNMENT_INTERNAL);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = CIP_RETURNCODE_SUCCESS;

    switch(EIP.Header.Command)
    {
        case EIP_Command_NOP:
            /* no response for NOP */ 
            EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

        case EIP_Command_ListServices:
            EIP.Response.p_ListServices = cip_binary_malloc(p_cip_ctrl, sizeof(*EIP.Response.p_ListServices));
            if (!EIP.Response.p_ListServices) EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_process_EIP_Encap_ListServices(p_eip_ctrl, EIP.Request.p_ListServices, EIP.Response.p_ListServices);
            break;

        case EIP_Command_ListIdentity:
            EIP.Response.p_ListIdentity = cip_binary_malloc(p_cip_ctrl, sizeof(*EIP.Response.p_ListIdentity));
            if(!EIP.Response.p_ListIdentity) EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_process_EIP_Encap_ListIdentity(p_eip_ctrl, EIP.Request.p_ListIdentity, EIP.Response.p_ListIdentity);
            break;

        case EIP_Command_RegisterSession:
            EIP.Response.p_RegisterSession = cip_binary_malloc(p_cip_ctrl, sizeof(*EIP.Response.p_RegisterSession));
            if(!EIP.Response.p_RegisterSession) EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_process_EIP_Encap_RegisterSession(p_eip_ctrl, EIP.Request.p_RegisterSession, EIP.Response.p_RegisterSession);
            break;

        case EIP_Command_UnRegisterSession:
            rc = eip_process_EIP_Encap_UnRegisterSession(p_eip_ctrl, EIP.Request.p_UnRegisterSession, EIP.Response.p_UnRegisterSession);
            break;

        case EIP_Command_SendRRData:
            EIP.Response.p_SendRRData = cip_binary_malloc(p_cip_ctrl, sizeof(*EIP.Response.p_SendRRData));
            if(!EIP.Response.p_SendRRData) EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            /* some messages need to modify the response items or use optional request items (like ForwardOpen) */
            p_eip_ctrl->p_Response_ItemArray = &EIP.Response.p_SendRRData->EncapsulatedPacket;
            p_eip_ctrl->p_Request_ItemArray = &EIP.Request.p_SendRRData->EncapsulatedPacket;

            rc = eip_process_EIP_Encap_SendRRData(p_eip_ctrl, EIP.Request.p_SendRRData, EIP.Response.p_SendRRData);
            break;

        case EIP_Command_SendUnitData:
            EIP.Response.p_SendUnitData = cip_binary_malloc(p_cip_ctrl, sizeof(*EIP.Response.p_SendUnitData));
            if (!EIP.Response.p_SendUnitData) EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            /* some messages need to modify the response items or use optional request items (like ForwardOpen) */
            p_eip_ctrl->p_Response_ItemArray = &EIP.Response.p_SendRRData->EncapsulatedPacket;
            p_eip_ctrl->p_Request_ItemArray = &EIP.Request.p_SendRRData->EncapsulatedPacket;

            rc = eip_process_EIP_Encap_SendUnitData(p_eip_ctrl, EIP.Request.p_SendUnitData, EIP.Response.p_SendUnitData);
            break;
            
        default: 
            /* this case is OK since we checked above... allow so error code falls through */
            break;
    }

    /* common process return code logic */
    if (rc == CIP_RETURNCODE_DISCARD_PACKET) /* some messages don't need a response */
    {
        (void)cip_binary_swap_heaps(p_cip_ctrl, RTA_USER_MEMORY_ALIGNMENT_NETWORK); /* zero out any data */
        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS)
    }
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_binary_swap_heaps(p_cip_ctrl, RTA_USER_MEMORY_ALIGNMENT_NETWORK);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    /*  All responses on the wire use an encapsulation header so encode it. */
    rc = eip_encode_EIP_Encap_Header(p_eip_ctrl, &EIP.Header);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    switch(EIP.Header.Command)
    {
        case EIP_Command_NOP: /* we should have already returned for a NOP */
            EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

        case EIP_Command_ListServices:
            rc = eip_encode_EIP_Encap_ListServices_Response(p_eip_ctrl, EIP.Response.p_ListServices);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_Command_ListIdentity:
            rc = eip_encode_EIP_Encap_ListIdentity_Response(p_eip_ctrl, EIP.Response.p_ListIdentity);
            if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_Command_RegisterSession:
            rc = eip_encode_EIP_Encap_RegisterSession_Response(p_eip_ctrl, EIP.Response.p_RegisterSession);
            if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_Command_UnRegisterSession:
            /* no response for UnRegisterSession if successful */
            if (EIP.Header.Status == EIP_ENCAP_STATUS_SUCCESS)
            {
                rc = cip_binary_swap_heaps(p_cip_ctrl, RTA_USER_MEMORY_ALIGNMENT_NETWORK);
                if(rc != CIP_RETURNCODE_SUCCESS)
                	EIP_LOG_ERROR_AND_RETURN(rc);
                EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
            }
            break;

        case EIP_Command_SendRRData:
            rc = eip_encode_EIP_Encap_SendRRData_Response(p_eip_ctrl, EIP.Response.p_SendRRData);
            if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_Command_SendUnitData:
            rc = eip_encode_EIP_Encap_SendUnitData_Response(p_eip_ctrl, EIP.Response.p_SendUnitData);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;
            
        default:
            /* this case is OK since we checked above... allow so error code falls through */
            break;
    }

    length = (CIP_UINT) cip_encode_get_len_from_p_start_to_end_of_data(p_cip_ctrl, p_eip_ctrl->p_EIP_Encap_data_start);
    rc = cip_encode_CIP_UINT_length_to_plen (length, p_eip_ctrl->p_EIP_Encap_Header_Length_EIP_UINT);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    
    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS)
}

CIP_ReturnCode eip_encap_build_error(EIP_Encap_Header * const p_Encap_Header, CIP_UDINT const Status)
{
    EIP_LOG_FUNCTION_ENTER;

    if (!p_Encap_Header)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_Encap_Header->Length = 0;
    p_Encap_Header->Status = Status;

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS)
}

EIP_SessionStruct * eip_encap_find_Session_from_p_socket (RTA_Usersock_Socket const * const p_socket)
{
    RTA_U16 ii;

    if (!p_socket)
        return(RTA_NULL);

    for(ii=0; ii<EIP_ENCAP_NUM_SESSIONS; ii++)
    {
        if(Session[ii].p_socket == p_socket)
            return(&Session[ii]);
    }

    return(RTA_NULL);
}

EIP_SessionStruct * eip_encap_find_Session_from_SessionHandle (CIP_UDINT const SessionHandle)
{
    RTA_U16 ii;

    for(ii=0; ii<EIP_ENCAP_NUM_SESSIONS; ii++)
    {
        if(Session[ii].SessionHandle == SessionHandle)
            return(&Session[ii]);
    }

    return(RTA_NULL);
}

EIP_SessionStruct * eip_encap_allocate_Session_from_p_socket (RTA_Usersock_Socket const * const p_socket)
{
    RTA_U16 ii;

    if(!p_socket)
        return(RTA_NULL);

    if(eip_encap_find_Session_from_p_socket(p_socket) != RTA_NULL)
        return(RTA_NULL);

    for(ii=0; ii<EIP_ENCAP_NUM_SESSIONS; ii++)
    {
        if(Session[ii].p_socket == RTA_Usersock_Socket_Unused)
        {
            Session[ii].p_socket = p_socket;
            Session[ii].SessionHandle = NextSessionHandle++;
            return(&Session[ii]);
        }
    }

    return(RTA_NULL);
}

void eip_encap_free_Session_from_p_socket(RTA_Usersock_Socket const * const p_socket)
{
    EIP_SessionStruct * p_Session;

    if(!p_socket)
        return;

    p_Session = eip_encap_find_Session_from_p_socket(p_socket);
    
    if (p_Session)
    {
        p_Session->SessionHandle = 0;
        p_Session->p_socket = RTA_Usersock_Socket_Unused;        
        eip_sockets_close_tcp_socket(p_socket);
        cip_obj_cnxnmgr_free_ExplicitConnection(p_Session->p_ExplicitConnection);
        p_Session->p_ExplicitConnection = RTA_NULL;
    }
}

void eip_encap_free_ExplicitConnection_from_SessionHandle (CIP_UDINT const SessionHandle)
{
    EIP_SessionStruct * p_Session;

    p_Session = eip_encap_find_Session_from_SessionHandle(SessionHandle);

    if (p_Session)
    {
        p_Session->p_ExplicitConnection = RTA_NULL;

        /* TODO: if we add multiple connections per session change this logic */
    }
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */
static CIP_ReturnCode local_validate_EIP_Encap_Header(EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_Header const * const p_Encap_Header)
{
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;
    
    if (!p_eip_ctrl || !p_Encap_Header)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    /*  From CIP Volume 2 (EtherNet/IP) 2-3.1 Encapsulation Packet Structure:
            All encapsulation messages, sent via TCP or sent to UDP port 0xAF12, shall be composed of a 
            fixed-length header of 24 bytes followed by an optional data portion. 
            The total encapsulation message length (including header) shall be limited to 65535 bytes.

        Taking into account the 24-byte header, the command specific data portion is limitetd to 65511 octets */  
    if (p_Encap_Header->Length > 65511)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if (p_Encap_Header->Length != cip_decode_get_remaining_used_size(p_cip_ctrl))
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if (p_Encap_Header->Status != EIP_ENCAP_STATUS_SUCCESS)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS)
}

/* *********** */
/* END OF FILE */
/* *********** */
