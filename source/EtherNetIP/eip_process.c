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
 * *  www.rtaautomation.com
 */

/* ========================== */
/*	 INCLUDE FILES	          */
/* ========================== */
#include "eip_system.h"

/* used for debug */
/*! \cond */
#undef  __RTA_FILE__
#define __RTA_FILE__ "eip_process.c"
/*! \endcond */

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */
/*	 GLOBAL DATA	          */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */
static CIP_ReturnCode local_backup_MessageRouterResponse (CIP_ExplicitConnection * const p_ExplicitConnection, CIP_MessageRouterResponse const * const p_MessageRouterResponse);

/* ========================= */
/*	 LOCAL DATA	             */
/* ========================= */

/* ========================= */
/*	 MISCELLANEOUS	         */
/* ========================= */

/* ==================================================== */
/*		   GLOBAL FUNCTIONS	                            */
/* ==================================================== */
CIP_ReturnCode eip_process_EIP_Encap_ListServices(EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_ListServices_Request const * const p_Encap_ListServices_Request, EIP_Encap_ListServices_Response * const p_Encap_ListServices_Response)
{
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_Item_ListService * p_Item_ListService;
    EIP_Item_Explicit  * p_ItemData;

    RTA_UNUSED_PARAM(p_Encap_ListServices_Request);

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Encap_ListServices_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    p_Encap_ListServices_Response->Item_Array.ItemCount = 1;
    if(p_Encap_ListServices_Response->Item_Array.ItemCount > EIP_Max_Num_EIP_Item_Array_Elements)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_ItemData = p_Encap_ListServices_Response->Item_Array.ItemData;

    p_ItemData->ItemID = EIP_ItemID_ListService;
    p_ItemData->ItemData.p_Item_ListService = cip_binary_malloc(p_cip_ctrl, sizeof(*p_ItemData->ItemData.p_Item_ListService));
    if (!p_ItemData->ItemData.p_Item_ListService)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    p_Item_ListService = p_ItemData->ItemData.p_Item_ListService;

    p_ItemData->ItemLength = 0; /* filled in later; not in this function */

    p_Item_ListService->EncapProtocolVer = EIP_ENCAP_PROTOCOL_VER;
    p_Item_ListService->CapabilityFlags = EIP_ListService_CapabilityFlags; 
    rta_snprintf((char *)p_Item_ListService->NameOfService, sizeof(p_Item_ListService->NameOfService), "Communications");

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_process_EIP_Encap_ListIdentity (EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_ListIdentity_Request const * const p_Encap_ListIdentity_Request, EIP_Encap_ListIdentity_Response * const p_Encap_ListIdentity_Response)
{
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_ReturnCode rc;
    EIP_TCPObject_InterfaceConfiguration * p_tcp;
    EIP_Item_CIP_Identity * p_Item_CIP_Identity;
    EIP_Item_Explicit  * p_ItemData;

    RTA_UNUSED_PARAM(p_Encap_ListIdentity_Request);

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Encap_ListIdentity_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    p_Encap_ListIdentity_Response->Item_Array.ItemCount = 1;
    if(p_Encap_ListIdentity_Response->Item_Array.ItemCount > EIP_Max_Num_EIP_Item_Array_Elements)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_ItemData = p_Encap_ListIdentity_Response->Item_Array.ItemData;
      
    p_ItemData->ItemID = EIP_ItemID_CIP_Identity;
    p_ItemData->ItemData.p_Item_CIP_Identity = cip_binary_malloc(p_cip_ctrl, sizeof(*p_ItemData->ItemData.p_Item_CIP_Identity));
    if(!p_ItemData->ItemData.p_Item_CIP_Identity)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    p_Item_CIP_Identity = p_ItemData->ItemData.p_Item_CIP_Identity;

    p_ItemData->ItemLength = 0; /* filled in later; not in this function */

    p_Item_CIP_Identity->EncapProtocolVer = EIP_ENCAP_PROTOCOL_VER;

    rc = cip_userobj_identity_get_CIP_IdentityObject(p_cip_ctrl, &p_Item_CIP_Identity->IdentityObject);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    p_tcp = cip_binary_malloc(p_cip_ctrl, sizeof(*p_tcp));
    if(!p_tcp) EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    rc = eip_userobj_tcp_get_EIP_TCPObject_InterfaceConfiguration(p_cip_ctrl, p_tcp);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    p_Item_CIP_Identity->SocketAddress.sin_family = EIP_USERSOCK_SIN_FAMILY;
    p_Item_CIP_Identity->SocketAddress.sin_port = EIP_USERSOCK_SIN_PORT;
    p_Item_CIP_Identity->SocketAddress.sin_addr = p_tcp->IP_Address;
    memset(p_Item_CIP_Identity->SocketAddress.sin_zero, 0, sizeof(p_Item_CIP_Identity->SocketAddress.sin_zero));

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_process_EIP_Encap_RegisterSession (EIP_ControlStruct const * const p_cip_ctrl, EIP_Encap_RegisterSession_Request const * const p_Encap_RegisterSession_Request, EIP_Encap_RegisterSession_Response * const p_Encap_RegisterSession_Response)
{
    CIP_ReturnCode rc;
    EIP_SessionStruct const * p_Session;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_Encap_RegisterSession_Request || !p_Encap_RegisterSession_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_cip_ctrl->p_socket->socket_type != RTA_Usersock_Socket_Type_TCPSERVER)
    {
        rc = eip_encap_build_error(p_cip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_COMMAND);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    if (p_cip_ctrl->p_Encap_Header->Options != 0)
    {
        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_DISCARD_PACKET);
    }

    p_Encap_RegisterSession_Response->EncapProtocolVer = EIP_ENCAP_PROTOCOL_VER;
    p_Encap_RegisterSession_Response->OptionsFlags = 0;

    /* Error processing rules are defined in CIP Volume 2, Section 2-4.4.3 */

    if(p_Encap_RegisterSession_Request->EncapProtocolVer != EIP_ENCAP_PROTOCOL_VER)
    {
        rc = eip_encap_build_error(p_cip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_BAD_PROTOCOL_VERSION);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    if(p_Encap_RegisterSession_Request->OptionsFlags != 0)
    {
        rc = eip_encap_build_error(p_cip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_BAD_PROTOCOL_VERSION);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    if(eip_encap_find_Session_from_p_socket(p_cip_ctrl->p_socket) != RTA_NULL)
    {
        rc = eip_encap_build_error(p_cip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_COMMAND);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    p_Session = eip_encap_allocate_Session_from_p_socket(p_cip_ctrl->p_socket);

    if(p_Session == RTA_NULL)
    {
        rc = eip_encap_build_error(p_cip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_NO_MEMORY);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    p_cip_ctrl->p_Encap_Header->SessionHandle = p_Session->SessionHandle;

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_process_EIP_Encap_UnRegisterSession(EIP_ControlStruct const * const p_cip_ctrl, EIP_Encap_UnRegisterSession_Request const * const p_Encap_UnRegisterSession_Request, EIP_Encap_UnRegisterSession_Response const * const p_Encap_UnRegisterSession_Response)
{
    CIP_ReturnCode rc;

    EIP_LOG_FUNCTION_ENTER;

    /* no request or response data */
    RTA_UNUSED_PARAM(p_Encap_UnRegisterSession_Request);
    RTA_UNUSED_PARAM(p_Encap_UnRegisterSession_Response);  

    if (!p_cip_ctrl)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* Error processing rules are defined in CIP Volume 2, Section 2-4.5 */

    if (p_cip_ctrl->p_socket->socket_type != RTA_Usersock_Socket_Type_TCPSERVER)
    {
        rc = eip_encap_build_error(p_cip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_COMMAND);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    /*  The receiver shall not reject the UnRegisterSession due to unexpected values in the 
        encapsulation header (invalid Session Handle, non-zero Status, non-zero Options, 
        or additional command data). In all cases the TCP connection shall be closed. */
    eip_encap_free_Session_from_p_socket(p_cip_ctrl->p_socket);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_process_EIP_Encap_SendRRData(EIP_ControlStruct * const p_eip_ctrl, EIP_Encap_SendRRData_Request const * const p_Encap_SendRRData_Request, EIP_Encap_SendRRData_Response * const p_Encap_SendRRData_Response)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_SessionStruct const * p_Session;
    EIP_Item_Explicit const * p_Request_Item_Address;
    EIP_Item_Explicit const * p_Request_Item_Data;
    EIP_Item_Explicit * p_Response_Item_Address;
    EIP_Item_Explicit * p_Response_Item_Data;
    CIP_MessageRouterRequest * p_MessageRouterRequest;
    CIP_MessageRouterResponse * p_MessageRouterResponse;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Encap_SendRRData_Request || !p_Encap_SendRRData_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    if(p_eip_ctrl->p_socket->socket_type != RTA_Usersock_Socket_Type_TCPSERVER)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_COMMAND);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }
        
    p_Session = eip_encap_find_Session_from_p_socket(p_eip_ctrl->p_socket);
    if((!p_Session) || (p_Session->SessionHandle != p_eip_ctrl->p_Encap_Header->SessionHandle))
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_HANDLE);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    /* Error processing rules are defined in CIP Volume 2, Section 2-4.7.2 */
    if(p_eip_ctrl->p_Encap_Header->Options != 0)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_DISCARD_PACKET);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    if (p_Encap_SendRRData_Request->InterfaceHandle != 0)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INCORRECT_DATA);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    /* Error processing rules are defined in CIP Volume 2, 3-2.1 Unconnected Messages */
    if (p_Encap_SendRRData_Request->EncapsulatedPacket.ItemCount < 2)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INCORRECT_DATA);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    p_Request_Item_Address = &p_Encap_SendRRData_Request->EncapsulatedPacket.ItemData[0];
    p_Request_Item_Data = &p_Encap_SendRRData_Request->EncapsulatedPacket.ItemData[1];

    if (p_Request_Item_Address->ItemID != EIP_ItemID_NullAddress)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INCORRECT_DATA);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    if (p_Request_Item_Address->ItemLength > EIP_ItemLength_Max_NullAddress)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_LENGTH);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    if (p_Request_Item_Data->ItemID != EIP_ItemID_UnconnectedData)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INCORRECT_DATA);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    if (p_Request_Item_Data->ItemLength > EIP_ItemLength_Max_UnconnectedData)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_LENGTH);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    if (!p_Request_Item_Data->ItemData.p_Item_UnconnectedData)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

    if (!p_Request_Item_Data->ItemData.p_Item_UnconnectedData->is_request)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

    p_MessageRouterRequest = &p_Request_Item_Data->ItemData.p_Item_UnconnectedData->MessageRouter.Request;

    /* done with SendRRData validation, malloc for response. All other error codes are within CIP */
    p_Encap_SendRRData_Response->InterfaceHandle = 0;
    p_Encap_SendRRData_Response->Timeout = 0;
    p_Encap_SendRRData_Response->EncapsulatedPacket.ItemCount = 2;

    if(p_Encap_SendRRData_Response->EncapsulatedPacket.ItemCount > EIP_Max_Num_EIP_Item_Array_Elements)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_Response_Item_Address = &p_Encap_SendRRData_Response->EncapsulatedPacket.ItemData[0];

    p_Response_Item_Address->ItemID = EIP_ItemID_NullAddress;
    p_Response_Item_Address->ItemLength = 0; /* Filled in later */

    p_Response_Item_Data = &p_Encap_SendRRData_Response->EncapsulatedPacket.ItemData[1];

    p_Response_Item_Data->ItemID = EIP_ItemID_UnconnectedData;
    p_Response_Item_Data->ItemLength = 0; /* Filled in later */

    p_Response_Item_Data->ItemData.p_Item_UnconnectedData = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Response_Item_Data->ItemData.p_Item_UnconnectedData));
    if (!p_Response_Item_Data->ItemData.p_Item_UnconnectedData)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_Response_Item_Data->ItemData.p_Item_UnconnectedData->is_request = RTA_FALSE;
    p_MessageRouterResponse = &p_Response_Item_Data->ItemData.p_Item_UnconnectedData->MessageRouter.Response;
    
    rc = cip_process_message(p_eip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    /* re-check this value since process may have changed it */
    if(p_Encap_SendRRData_Response->EncapsulatedPacket.ItemCount > EIP_Max_Num_EIP_Item_Array_Elements)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_process_EIP_Encap_SendUnitData(EIP_ControlStruct * const p_eip_ctrl, EIP_Encap_SendUnitData_Request const * const p_Encap_SendUnitData_Request, EIP_Encap_SendUnitData_Response * const p_Encap_SendUnitData_Response)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_SessionStruct const * p_Session;
    EIP_Item_Explicit const * p_Request_Item_Address;
    EIP_Item_Explicit const * p_Request_Item_Data;
    EIP_Item_Explicit * p_Response_Item_Address;
    EIP_Item_Explicit * p_Response_Item_Data;
    CIP_MessageRouterRequest * p_MessageRouterRequest;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_BOOL skip_processing = RTA_FALSE;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Encap_SendUnitData_Request || !p_Encap_SendUnitData_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    if (p_eip_ctrl->p_socket->socket_type != RTA_Usersock_Socket_Type_TCPSERVER)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_COMMAND);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    p_Session = eip_encap_find_Session_from_p_socket(p_eip_ctrl->p_socket);
    if ((!p_Session) || (p_Session->SessionHandle != p_eip_ctrl->p_Encap_Header->SessionHandle))
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_HANDLE);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    /* Error processing rules are defined in CIP Volume 2, Section 2-4.7.2 */
    if (p_eip_ctrl->p_Encap_Header->Options != 0)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_DISCARD_PACKET);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    if (p_Encap_SendUnitData_Request->InterfaceHandle != 0)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INCORRECT_DATA);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    if (p_Encap_SendUnitData_Request->EncapsulatedPacket.ItemCount < 2)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INCORRECT_DATA);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    p_Request_Item_Address = &p_Encap_SendUnitData_Request->EncapsulatedPacket.ItemData[0];
    p_Request_Item_Data = &p_Encap_SendUnitData_Request->EncapsulatedPacket.ItemData[1];

    if (p_Request_Item_Address->ItemID != EIP_ItemID_ConnectedAddress)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INCORRECT_DATA);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    if (p_Request_Item_Address->ItemLength > EIP_ItemLength_Max_ConnectedAddress)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_LENGTH);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    if (!p_Request_Item_Address->ItemData.p_Item_ConnectedAddress)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

    if (!p_Session->p_ExplicitConnection)
        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_DISCARD_PACKET);

    if (p_Request_Item_Address->ItemData.p_Item_ConnectedAddress->ConnectionIdentifier != p_Session->p_ExplicitConnection->O2T.NetworkConnectionID)
    {
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_DISCARD_PACKET);
    }

    if (p_Request_Item_Data->ItemID != EIP_ItemID_ConnectedData)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INCORRECT_DATA);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    if (p_Request_Item_Data->ItemLength > EIP_ItemLength_Max_ConnectedData)
    {
        rc = eip_encap_build_error(p_eip_ctrl->p_Encap_Header, EIP_ENCAP_STATUS_INVALID_LENGTH);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    if (!p_Request_Item_Data->ItemData.p_Item_ConnectedData)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

    if (!p_Request_Item_Data->ItemData.p_Item_ConnectedData->is_request)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);

    p_MessageRouterRequest = &p_Request_Item_Data->ItemData.p_Item_ConnectedData->MessageRouter.Request;

    /* done with SendUnitData validation, malloc for response. All other error codes are within CIP */
    p_Encap_SendUnitData_Response->InterfaceHandle = 0;
    p_Encap_SendUnitData_Response->Timeout = 0;
    p_Encap_SendUnitData_Response->EncapsulatedPacket.ItemCount = 2;

    if(p_Encap_SendUnitData_Response->EncapsulatedPacket.ItemCount > EIP_Max_Num_EIP_Item_Array_Elements)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_Response_Item_Address = &p_Encap_SendUnitData_Response->EncapsulatedPacket.ItemData[0];

    p_Response_Item_Address->ItemID = EIP_ItemID_ConnectedAddress;
    p_Response_Item_Address->ItemLength = 4; /* Filled in later */

    p_Response_Item_Address->ItemData.p_Item_ConnectedAddress = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Response_Item_Address->ItemData.p_Item_ConnectedAddress));
    if (!p_Response_Item_Address->ItemData.p_Item_ConnectedAddress)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_Response_Item_Address->ItemData.p_Item_ConnectedAddress->ConnectionIdentifier = p_Session->p_ExplicitConnection->T2O.NetworkConnectionID;

    p_Response_Item_Data = &p_Encap_SendUnitData_Response->EncapsulatedPacket.ItemData[1];

    p_Response_Item_Data->ItemID = EIP_ItemID_ConnectedData;
    p_Response_Item_Data->ItemLength = 0; /* Filled in later */

    p_Response_Item_Data->ItemData.p_Item_ConnectedData = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Response_Item_Data->ItemData.p_Item_ConnectedData));
    if (!p_Response_Item_Data->ItemData.p_Item_ConnectedData)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_Response_Item_Data->ItemData.p_Item_ConnectedData->is_request = RTA_FALSE;

    cip_obj_cnxnmgr_ExplicitConnection_received(p_Session->p_ExplicitConnection);

    if(p_Session->p_ExplicitConnection->Last_message_received)
    {
        if(p_Session->p_ExplicitConnection->Last_PDU_SequenceNumber == p_Request_Item_Data->ItemData.p_Item_ConnectedData->sequence_number)
        {
            skip_processing = RTA_TRUE;
        }
    }

    else
    {
        p_Session->p_ExplicitConnection->Last_message_received = RTA_TRUE;
    }

    p_Session->p_ExplicitConnection->Last_PDU_SequenceNumber = p_Request_Item_Data->ItemData.p_Item_ConnectedData->sequence_number;   

    /* echo sequence number */
    p_Response_Item_Data->ItemData.p_Item_ConnectedData->sequence_number = p_Request_Item_Data->ItemData.p_Item_ConnectedData->sequence_number;   

    if(!skip_processing)
    {
        p_MessageRouterResponse = &p_Response_Item_Data->ItemData.p_Item_UnconnectedData->MessageRouter.Response;

        rc = cip_process_message(p_eip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        /* re-check this value since process may have changed it */
        if(p_Encap_SendUnitData_Response->EncapsulatedPacket.ItemCount > EIP_Max_Num_EIP_Item_Array_Elements)
            EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

        rc = local_backup_MessageRouterResponse (p_Session->p_ExplicitConnection, p_MessageRouterResponse);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);        
    }
    else
    {
        memcpy(&p_Response_Item_Data->ItemData.p_Item_UnconnectedData->MessageRouter.Response, &p_Session->p_ExplicitConnection->Last_MessageRouterResponse, sizeof(p_Response_Item_Data->ItemData.p_Item_UnconnectedData->MessageRouter.Response));
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_backup_MessageRouterResponse (CIP_ExplicitConnection * const p_ExplicitConnection, CIP_MessageRouterResponse const * const p_MessageRouterResponse)
{
    EIP_LOG_FUNCTION_ENTER;

    if(!p_ExplicitConnection || !p_MessageRouterResponse)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    memcpy(&p_ExplicitConnection->Last_MessageRouterResponse, p_MessageRouterResponse, sizeof(p_ExplicitConnection->Last_MessageRouterResponse));        
    
    switch(p_MessageRouterResponse->Response_Data.Type)
    {
        /* malloc-y data... point to useful memory not on the heap */
        case CIP_DATA_TYPE_CIP_STRING:
        case CIP_DATA_TYPE_CIP_STRING48_16BIT_PADDED:
        case CIP_DATA_TYPE_CIP_STRING64_16BIT_PADDED:
            if(p_MessageRouterResponse->Response_Data.Value.value_STRING.Length > sizeof(p_ExplicitConnection->Last_Message_Dummy_Heap))
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            p_ExplicitConnection->Last_MessageRouterResponse.Response_Data.Value.value_STRING.pData = p_ExplicitConnection->Last_Message_Dummy_Heap;
            memcpy(p_ExplicitConnection->Last_Message_Dummy_Heap, p_MessageRouterResponse->Response_Data.Value.value_STRING.pData, p_MessageRouterResponse->Response_Data.Value.value_STRING.Length);
            break;
    
        case CIP_DATA_TYPE_CIP_Opaque_String:
            if(p_MessageRouterResponse->Response_Data.Value.value_Opaque_String.Length > sizeof(p_ExplicitConnection->Last_Message_Dummy_Heap))
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            p_ExplicitConnection->Last_MessageRouterResponse.Response_Data.Value.value_Opaque_String.pData = p_ExplicitConnection->Last_Message_Dummy_Heap;
            memcpy(p_ExplicitConnection->Last_Message_Dummy_Heap, p_MessageRouterResponse->Response_Data.Value.value_Opaque_String.pData, p_MessageRouterResponse->Response_Data.Value.value_Opaque_String.Length);
            break;

        /* Plain Old Data (POD) */
        case CIP_DATA_TYPE_NO_DATA:
        case CIP_DATA_TYPE_CIP_BOOL:
        case CIP_DATA_TYPE_CIP_SINT:
        case CIP_DATA_TYPE_CIP_INT:
        case CIP_DATA_TYPE_CIP_DINT:
        case CIP_DATA_TYPE_CIP_LINT:
        case CIP_DATA_TYPE_CIP_USINT:
        case CIP_DATA_TYPE_CIP_UINT:
        case CIP_DATA_TYPE_CIP_UDINT:
        case CIP_DATA_TYPE_CIP_ULINT:
        case CIP_DATA_TYPE_CIP_REAL:
        case CIP_DATA_TYPE_CIP_LREAL:
        case CIP_DATA_TYPE_CIP_BYTE:
        case CIP_DATA_TYPE_CIP_WORD:
        case CIP_DATA_TYPE_CIP_DWORD:
        case CIP_DATA_TYPE_CIP_LWORD:
        case CIP_DATA_TYPE_CIP_SHORT_STRING_32:
        case CIP_DATA_TYPE_CIP_DATE_AND_TIME:
        case CIP_DATA_TYPE_CIP_IdentityObject_Revision:
        case CIP_DATA_TYPE_CIP_IdentityObject:
        case CIP_DATA_TYPE_CIP_ForwardOpen_Request:
        case CIP_DATA_TYPE_CIP_ForwardOpen_Response_Success:
        case CIP_DATA_TYPE_CIP_ForwardOpen_Response_Unsuccessful:
        case CIP_DATA_TYPE_CIP_ForwardClose_Request:
        case CIP_DATA_TYPE_CIP_ForwardClose_Response_Success:
        case CIP_DATA_TYPE_CIP_ForwardClose_Response_Unsuccessful:
        case CIP_DATA_TYPE_EIP_TCPObject_Status:
        case CIP_DATA_TYPE_EIP_TCPObject_ConfigurationCapability:
        case CIP_DATA_TYPE_EIP_TCPObject_ConfigurationControl:
        case CIP_DATA_TYPE_EIP_TCPObject_PhysicalLinkObject:
        case CIP_DATA_TYPE_EIP_TCPObject_InterfaceConfiguration:
        case CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceFlags_Internal:
        case CIP_DATA_TYPE_EIP_EthernetLinkObject_PhysicalAddress:
        case CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceCounters:
        case CIP_DATA_TYPE_EIP_EthernetLinkObject_MediaCounters:
        case CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceCapability:
            break;
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* *********** */
/* END OF FILE */
/* *********** */
