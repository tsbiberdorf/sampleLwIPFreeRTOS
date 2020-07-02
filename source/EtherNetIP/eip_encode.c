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
#define __RTA_FILE__ "eip_encode.c"
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

/* ========================= */
/*	 LOCAL DATA	             */
/* ========================= */

/* ========================= */
/*	 MISCELLANEOUS	         */
/* ========================= */

/* ==================================================== */
/*		   GLOBAL FUNCTIONS	                            */
/* ==================================================== */
CIP_ReturnCode eip_encode_EIP_Encap_Header (EIP_ControlStruct * const p_eip_ctrl, EIP_Encap_Header const * const p_Encap_Header)
{   
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Encap_Header)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Encap_Header->Command);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    /* store where we need to store the length later */
    p_eip_ctrl->p_EIP_Encap_Header_Length_EIP_UINT = cip_encode_get_current_buffer_ptr(p_cip_ctrl);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Encap_Header->Length);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_Encap_Header->SessionHandle);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_Encap_Header->Status);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_memcpy(p_cip_ctrl, p_Encap_Header->SenderContext, sizeof(p_Encap_Header->SenderContext));
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_Encap_Header->Options);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    /* keep track of where the Encapsulation data starts */
    p_eip_ctrl->p_EIP_Encap_data_start = cip_encode_get_current_buffer_ptr(p_cip_ctrl);
  
    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Encap_ListServices_Response(EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_ListServices_Response const * const p_Encap_ListServices_Response)
{
    CIP_ReturnCode rc;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Encap_ListServices_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = eip_encode_EIP_Item_Array_Explicit(p_eip_ctrl, &p_Encap_ListServices_Response->Item_Array);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Encap_ListIdentity_Response (EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_ListIdentity_Response const * const p_Encap_ListIdentity_Response)
{   
    CIP_ReturnCode rc;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Encap_ListIdentity_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = eip_encode_EIP_Item_Array_Explicit(p_eip_ctrl, &p_Encap_ListIdentity_Response->Item_Array);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Encap_RegisterSession_Response(EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_RegisterSession_Response const * const p_Encap_RegisterSession_Response)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Encap_RegisterSession_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Encap_RegisterSession_Response->EncapProtocolVer);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Encap_RegisterSession_Response->OptionsFlags);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Encap_SendRRData_Response(EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_SendRRData_Response const * const p_Encap_SendRRData_Response)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Encap_SendRRData_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_Encap_SendRRData_Response->InterfaceHandle);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Encap_SendRRData_Response->Timeout);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = eip_encode_EIP_Item_Array_Explicit (p_eip_ctrl, &p_Encap_SendRRData_Response->EncapsulatedPacket);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Encap_SendUnitData_Response(EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_SendUnitData_Response const * const p_Encap_SendUnitData_Response)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Encap_SendUnitData_Response)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_Encap_SendUnitData_Response->InterfaceHandle);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Encap_SendUnitData_Response->Timeout);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = eip_encode_EIP_Item_Array_Explicit(p_eip_ctrl, &p_Encap_SendUnitData_Response->EncapsulatedPacket);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_Array_Explicit (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_Array_Explicit const * const p_EIP_Item_Array)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_UINT ii;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_EIP_Item_Array)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    if(p_EIP_Item_Array->ItemCount > EIP_Max_Num_EIP_Item_Array_Elements)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_EIP_Item_Array->ItemCount);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    for(ii=0; ii<p_EIP_Item_Array->ItemCount; ii++)
    {
        rc = eip_encode_EIP_Item_Explicit (p_eip_ctrl, &p_EIP_Item_Array->ItemData[ii]);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_Explicit (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_Explicit const * const p_EIP_Item)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_UINT length;

    RTA_U8 * p_EIP_Item_Length_EIP_UINT;
    RTA_U8 const * p_EIP_Item_data_start;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_EIP_Item)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_EIP_Item->ItemID);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    /* store where we need to store the length later */
    p_EIP_Item_Length_EIP_UINT = cip_encode_get_current_buffer_ptr(p_cip_ctrl);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_EIP_Item->ItemLength);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    /* keep track of where the Item data starts */
    p_EIP_Item_data_start = cip_encode_get_current_buffer_ptr(p_cip_ctrl);

    switch(p_EIP_Item->ItemID)
    {
        case EIP_ItemID_NullAddress:
            /* nothing to encode */
            break;

        case EIP_ItemID_ConnectedAddress:
            rc = eip_encode_EIP_Item_ConnectedAddress(p_eip_ctrl, p_EIP_Item->ItemData.p_Item_ConnectedAddress);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_ItemID_UnconnectedData:
            rc = eip_encode_EIP_Item_UnconnectedData(p_eip_ctrl, p_EIP_Item->ItemData.p_Item_UnconnectedData);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_ItemID_ConnectedData:
            rc = eip_encode_EIP_Item_ConnectedData(p_eip_ctrl, p_EIP_Item->ItemData.p_Item_ConnectedData);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_ItemID_CIP_Identity:
            rc = eip_encode_EIP_Item_CIP_Identity(p_eip_ctrl, p_EIP_Item->ItemData.p_Item_CIP_Identity);        
            if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case EIP_ItemID_ListService:
            rc = eip_encode_EIP_Item_ListService(p_eip_ctrl, p_EIP_Item->ItemData.p_Item_ListService);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_ItemID_Sockaddr_Info_O2T:
        case EIP_ItemID_Sockaddr_Info_T2O:
            rc = eip_encode_EIP_Item_Sockaddr_Info(p_eip_ctrl, p_EIP_Item->ItemData.p_Item_Sockaddr_Info);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        default:
            EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);
    }
   
    length = (CIP_UINT) cip_encode_get_len_from_p_start_to_end_of_data(p_cip_ctrl, p_EIP_Item_data_start);
    rc = cip_encode_CIP_UINT_length_to_plen (length, p_EIP_Item_Length_EIP_UINT);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_Array_Implicit (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_Array_Implicit const * const p_Item_Array_Implicit)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_UINT ii;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Item_Array_Implicit)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    if(p_Item_Array_Implicit->ItemCount > EIP_Max_Num_EIP_Item_Array_Implicit_Elements)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Item_Array_Implicit->ItemCount);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    for(ii=0; ii<p_Item_Array_Implicit->ItemCount; ii++)
    {
        rc = eip_encode_EIP_Item_Implicit (p_eip_ctrl, &p_Item_Array_Implicit->ItemData[ii]);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_Implicit (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_Implicit const * const p_Item_Implicit)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_UINT length;

    RTA_U8 * p_EIP_Item_Length_EIP_UINT;
    RTA_U8 const * p_EIP_Item_data_start;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Item_Implicit)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Item_Implicit->ItemID);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    /* store where we need to store the length later */
    p_EIP_Item_Length_EIP_UINT = cip_encode_get_current_buffer_ptr(p_cip_ctrl);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Item_Implicit->ItemLength);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    /* keep track of where the Item data starts */
    p_EIP_Item_data_start = cip_encode_get_current_buffer_ptr(p_cip_ctrl);

    switch(p_Item_Implicit->ItemID)
    {
        case EIP_ItemID_SequencedAddress:
            rc = eip_encode_EIP_ItemID_SequencedAddress(p_eip_ctrl, p_Item_Implicit->ItemData.p_Item_SequencedAddress);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_ItemID_ConnectedData:
            rc = cip_encode_CIP_Opaque_String(p_cip_ctrl, p_Item_Implicit->ItemData.p_Item_ConnectedData_Opaque_String);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        default:
            EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);
    }
   
    length = (CIP_UINT) cip_encode_get_len_from_p_start_to_end_of_data(p_cip_ctrl, p_EIP_Item_data_start);
    rc = cip_encode_CIP_UINT_length_to_plen (length, p_EIP_Item_Length_EIP_UINT);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_CIP_Identity (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_CIP_Identity const * const p_Item_CIP_Identity)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Item_CIP_Identity)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Item_CIP_Identity->EncapProtocolVer);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = eip_encode_EIP_SocketAddress(p_eip_ctrl, &p_Item_CIP_Identity->SocketAddress);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_IdentityObject(p_cip_ctrl, &p_Item_CIP_Identity->IdentityObject);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_ListService(EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_ListService const * const p_Item_ListService)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Item_ListService)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Item_ListService->EncapProtocolVer);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Item_ListService->CapabilityFlags);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_memcpy(p_cip_ctrl, p_Item_ListService->NameOfService, sizeof(p_Item_ListService->NameOfService));
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_ConnectedAddress(EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_ConnectedAddress const * const p_Item_ConnectedAddress)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Item_ConnectedAddress)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_Item_ConnectedAddress->ConnectionIdentifier);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_SocketAddress (EIP_ControlStruct const * const p_eip_ctrl, EIP_SocketAddress const * const p_SocketAddress)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_SocketAddress)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_big_endian_CIP_INT(p_cip_ctrl, &p_SocketAddress->sin_family);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_big_endian_CIP_UINT(p_cip_ctrl, &p_SocketAddress->sin_port);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_big_endian_CIP_UDINT(p_cip_ctrl, &p_SocketAddress->sin_addr);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_memcpy(p_cip_ctrl, p_SocketAddress->sin_zero, sizeof(p_SocketAddress->sin_zero));
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_UnconnectedData (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_UnconnectedData const * const p_Item_UnconnectedData)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Item_UnconnectedData)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    if(p_Item_UnconnectedData->is_request) /* must be a response, so is_request is not expected to be set */
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);
    
    rc = cip_encode_CIP_MessageRouterResponse(p_cip_ctrl, &p_Item_UnconnectedData->MessageRouter.Response);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_ConnectedData(EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_ConnectedData const * const p_Item_ConnectedData)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Item_ConnectedData)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    if (p_Item_ConnectedData->is_request) /* must be a response, so is_request is not expected to be set */
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);
    
    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_Item_ConnectedData->sequence_number);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_MessageRouterResponse(p_cip_ctrl, &p_Item_ConnectedData->MessageRouter.Response);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_Item_Sockaddr_Info(EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_Sockaddr_Info const * const p_Item_Sockaddr_Info)
{
    CIP_ReturnCode rc;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Item_Sockaddr_Info)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
   
    rc = eip_encode_EIP_SocketAddress(p_eip_ctrl, &p_Item_Sockaddr_Info->SocketAddress);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_encode_EIP_ItemID_SequencedAddress(EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_SequencedAddress const * const p_Item_SequencedAddress)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Item_SequencedAddress)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
   
    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_Item_SequencedAddress->ConnectionIdentifier);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_Item_SequencedAddress->EncapsulationSequenceNumber);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}


/* *********** */
/* END OF FILE */
/* *********** */
