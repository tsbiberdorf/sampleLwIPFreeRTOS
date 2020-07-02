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
#define __RTA_FILE__ "eip_decode.c"
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
CIP_ReturnCode eip_decode_EIP_Encap_Header (EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_Header * const p_Encap_Header)
{
    CIP_ReturnCode rc;  
    CIP_ControlStruct const * p_cip_ctrl;
    
    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Encap_Header)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Encap_Header->Command);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Encap_Header->Length);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_Encap_Header->SessionHandle);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_Encap_Header->Status);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_memcpy(p_cip_ctrl, p_Encap_Header->SenderContext, sizeof(p_Encap_Header->SenderContext));
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_Encap_Header->Options);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);  
}

CIP_ReturnCode eip_decode_EIP_Encap_RegisterSession_Request(EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_RegisterSession_Request * const p_Encap_RegisterSession_Request)
{
    CIP_ReturnCode rc;  
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Encap_RegisterSession_Request)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Encap_RegisterSession_Request->EncapProtocolVer);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Encap_RegisterSession_Request->OptionsFlags);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);  
}

CIP_ReturnCode eip_decode_EIP_Encap_SendRRData_Request(EIP_ControlStruct * const p_eip_ctrl, EIP_Encap_SendRRData_Request * const p_Encap_SendRRData_Request)
{
    CIP_ReturnCode rc;  
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Encap_SendRRData_Request)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_Encap_SendRRData_Request->InterfaceHandle);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Encap_SendRRData_Request->Timeout);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = eip_decode_EIP_Item_Array_Explicit(p_eip_ctrl, &p_Encap_SendRRData_Request->EncapsulatedPacket);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_EXIT_AND_RETURN (rc); /* allow all errors to fall through to be processed */

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);  
}

CIP_ReturnCode eip_decode_EIP_Encap_SendUnitData_Request(EIP_ControlStruct * const p_eip_ctrl, EIP_Encap_SendUnitData_Request * const p_Encap_SendUnitData_Request)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Encap_SendUnitData_Request)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_Encap_SendUnitData_Request->InterfaceHandle);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Encap_SendUnitData_Request->Timeout);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = eip_decode_EIP_Item_Array_Explicit(p_eip_ctrl, &p_Encap_SendUnitData_Request->EncapsulatedPacket);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
    
    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_decode_EIP_SocketAddress (EIP_ControlStruct const * const p_eip_ctrl, EIP_SocketAddress * const p_SocketAddress)
{
    CIP_ReturnCode rc;  
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_SocketAddress)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_big_endian_CIP_INT(p_cip_ctrl, &p_SocketAddress->sin_family);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_big_endian_CIP_UINT(p_cip_ctrl, &p_SocketAddress->sin_port);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_big_endian_CIP_UDINT(p_cip_ctrl, &p_SocketAddress->sin_addr);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_memcpy(p_cip_ctrl, p_SocketAddress->sin_zero, sizeof(p_SocketAddress->sin_zero));
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);  
}

CIP_ReturnCode eip_decode_EIP_Item_Array_Explicit (EIP_ControlStruct * const p_eip_ctrl, EIP_Item_Array_Explicit * const p_Item_Array)
{
    CIP_ReturnCode rc;  
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_UINT ii;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Item_Array)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Item_Array->ItemCount);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    if(p_Item_Array->ItemCount > EIP_Max_Num_EIP_Item_Array_Elements)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    for(ii=0; ii<p_Item_Array->ItemCount; ii++)
    {
        rc = eip_decode_EIP_Item_Explicit(p_eip_ctrl, &p_Item_Array->ItemData[ii]);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_EXIT_AND_RETURN (rc); /* allow all errors to fall through to be processed */
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);  
}

CIP_ReturnCode eip_decode_EIP_Item_Explicit (EIP_ControlStruct * const p_eip_ctrl, EIP_Item_Explicit * const p_Item)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Item)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Item->ItemID);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Item->ItemLength);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    switch(p_Item->ItemID)
    {
        case EIP_ItemID_NullAddress:
            /* nothing to decode */
            break;

        case EIP_ItemID_ConnectedAddress:
            p_Item->ItemData.p_Item_ConnectedAddress = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item->ItemData.p_Item_ConnectedAddress));
            if (!p_Item->ItemData.p_Item_ConnectedAddress)
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_decode_EIP_Item_ConnectedAddress(p_eip_ctrl, p_Item->ItemLength, p_Item->ItemData.p_Item_ConnectedAddress);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_ItemID_UnconnectedData:            
            p_Item->ItemData.p_Item_UnconnectedData = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item->ItemData.p_Item_UnconnectedData));
            if(!p_Item->ItemData.p_Item_UnconnectedData)
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_decode_EIP_Item_UnconnectedData(p_eip_ctrl, p_Item->ItemLength, p_Item->ItemData.p_Item_UnconnectedData);
            EIP_LOG_EXIT_AND_RETURN(rc); /* treat all exits as "OK" so we don't hit user assert logic */

        case EIP_ItemID_ConnectedData:
            p_Item->ItemData.p_Item_ConnectedData = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item->ItemData.p_Item_ConnectedData));
            if (!p_Item->ItemData.p_Item_ConnectedData)
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_decode_EIP_Item_ConnectedData(p_eip_ctrl, p_Item->ItemLength, p_Item->ItemData.p_Item_ConnectedData);
            EIP_LOG_EXIT_AND_RETURN(rc); /* treat all exits as "OK" so we don't hit user assert logic */

        case EIP_ItemID_Sockaddr_Info_O2T:
        case EIP_ItemID_Sockaddr_Info_T2O:
            p_Item->ItemData.p_Item_Sockaddr_Info = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item->ItemData.p_Item_Sockaddr_Info));
            if (!p_Item->ItemData.p_Item_Sockaddr_Info)
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_decode_EIP_Item_Sockaddr_Info(p_eip_ctrl, p_Item->ItemLength, p_Item->ItemData.p_Item_Sockaddr_Info);
            EIP_LOG_EXIT_AND_RETURN(rc); /* treat all exits as "OK" so we don't hit user assert logic */

        default:
            p_Item->ItemData.p_Item_Unknown = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item->ItemData.p_Item_Unknown));
            if (!p_Item->ItemData.p_Item_Unknown)
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = cip_decode_CIP_Opaque_String (p_cip_ctrl, p_Item->ItemLength, p_Item->ItemData.p_Item_Unknown);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;
    }
   
    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_decode_EIP_Item_Array_Implicit (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_Array_Implicit * const p_Item_Array_Implicit)
{
    CIP_ReturnCode rc;  
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_UINT ii;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_Item_Array_Implicit)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Item_Array_Implicit->ItemCount);
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    if(p_Item_Array_Implicit->ItemCount > EIP_Max_Num_EIP_Item_Array_Implicit_Elements)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    for(ii=0; ii<p_Item_Array_Implicit->ItemCount; ii++)
    {
        rc = eip_decode_EIP_Item_Implicit(p_eip_ctrl, &p_Item_Array_Implicit->ItemData[ii]);
        if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_EXIT_AND_RETURN (rc); /* allow all errors to fall through to be processed */
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);  
}

CIP_ReturnCode eip_decode_EIP_Item_Implicit (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_Implicit * const p_Item_Implicit)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !p_Item_Implicit)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Item_Implicit->ItemID);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Item_Implicit->ItemLength);        
    if(rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN (rc);

    switch(p_Item_Implicit->ItemID)
    {
        case EIP_ItemID_SequencedAddress:
            p_Item_Implicit->ItemData.p_Item_SequencedAddress= cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item_Implicit->ItemData.p_Item_SequencedAddress));
            if (!p_Item_Implicit->ItemData.p_Item_SequencedAddress)
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = eip_decode_EIP_Item_SequencedAddress(p_eip_ctrl, p_Item_Implicit->ItemLength, p_Item_Implicit->ItemData.p_Item_SequencedAddress);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case EIP_ItemID_ConnectedData:
            p_Item_Implicit->ItemData.p_Item_ConnectedData_Opaque_String = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item_Implicit->ItemData.p_Item_ConnectedData_Opaque_String));
            if (!p_Item_Implicit->ItemData.p_Item_ConnectedData_Opaque_String)
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = cip_decode_CIP_Opaque_String (p_cip_ctrl, p_Item_Implicit->ItemLength, p_Item_Implicit->ItemData.p_Item_ConnectedData_Opaque_String);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;

        default:
            p_Item_Implicit->ItemData.p_Item_Unknown = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item_Implicit->ItemData.p_Item_Unknown));
            if (!p_Item_Implicit->ItemData.p_Item_Unknown)
                EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = cip_decode_CIP_Opaque_String (p_cip_ctrl, p_Item_Implicit->ItemLength, p_Item_Implicit->ItemData.p_Item_Unknown);
            if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);
            break;
    }
   
    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_decode_EIP_Item_ConnectedAddress(EIP_ControlStruct const * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_ConnectedAddress * const p_Item_ConnectedAddress)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;
    RTA_U8 const * p_EIP_Item_data_start;
    CIP_UINT length;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !expected_length || !p_Item_ConnectedAddress)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    /* keep track of where the Item data starts */
    p_EIP_Item_data_start = cip_decode_get_current_buffer_ptr(p_cip_ctrl);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_Item_ConnectedAddress->ConnectionIdentifier);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    length = (CIP_UINT)(cip_decode_get_current_buffer_ptr(p_cip_ctrl) - p_EIP_Item_data_start);
    if (length != expected_length)
    {
        /* TODO: length mismatch */
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_decode_EIP_Item_UnconnectedData (EIP_ControlStruct * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_UnconnectedData * const p_Item_UnconnectedData)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct * p_cip_ctrl;
    RTA_U8 const * p_EIP_Item_data_start;
    CIP_UINT length;

    EIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl || !expected_length || !p_Item_UnconnectedData)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    /* keep track of where the Item data starts */
    p_EIP_Item_data_start = cip_decode_get_current_buffer_ptr(p_cip_ctrl);
    
    p_Item_UnconnectedData->is_request = RTA_TRUE;
    rc = cip_decode_CIP_MessageRouterRequest(p_cip_ctrl, expected_length, &p_Item_UnconnectedData->MessageRouter.Request);
    if (rc == CIP_RETURNCODE_DECODE_MALLOC_FAILED) EIP_LOG_EXIT_AND_RETURN(cip_decode_store_and_return_general_status(p_cip_ctrl, CIP_Status_Not_enough_data));
    if (rc == CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT) EIP_LOG_EXIT_AND_RETURN(cip_decode_store_and_return_general_status(p_cip_ctrl, CIP_Status_Path_segment_error));
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_EXIT_AND_RETURN(rc); /* allow all errors to fall through to be processed */

    length = (CIP_UINT) (cip_decode_get_current_buffer_ptr(p_cip_ctrl) - p_EIP_Item_data_start);
    if(length != expected_length)
    {
        EIP_LOG_EXIT_AND_RETURN(cip_decode_store_and_return_general_status(p_cip_ctrl, CIP_Status_Too_much_data));
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_decode_EIP_Item_ConnectedData(EIP_ControlStruct * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_ConnectedData * const p_Item_ConnectedData)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct * p_cip_ctrl;
    RTA_U8 const * p_EIP_Item_data_start;
    CIP_UINT length;
    CIP_UINT remaining_expected_length;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !expected_length || !p_Item_ConnectedData)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_Item_ConnectedData->sequence_number);
    if (rc != CIP_RETURNCODE_SUCCESS) 
        EIP_LOG_EXIT_AND_RETURN(rc); /* this isn't a processing error, but a CIP error */

    if (expected_length > 2)
    {
        /* keep track of where the Item data starts */
        p_EIP_Item_data_start = cip_decode_get_current_buffer_ptr(p_cip_ctrl);

        remaining_expected_length = (expected_length - 2);

        p_Item_ConnectedData->is_request = RTA_TRUE;
        rc = cip_decode_CIP_MessageRouterRequest(p_cip_ctrl, remaining_expected_length, &p_Item_ConnectedData->MessageRouter.Request);
        if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

        length = (CIP_UINT)(cip_decode_get_current_buffer_ptr(p_cip_ctrl) - p_EIP_Item_data_start);
        if (length != remaining_expected_length)
        {
            EIP_LOG_EXIT_AND_RETURN(cip_decode_store_and_return_general_status(p_cip_ctrl, CIP_Status_Too_much_data));
        }
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_decode_EIP_Item_Sockaddr_Info(EIP_ControlStruct const * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_Sockaddr_Info * const p_Item_Sockaddr_Info)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;
    RTA_U8 const * p_EIP_Item_data_start;
    CIP_UINT length;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !expected_length || !p_Item_Sockaddr_Info)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    /* keep track of where the Item data starts */
    p_EIP_Item_data_start = cip_decode_get_current_buffer_ptr(p_cip_ctrl);

    rc = eip_decode_EIP_SocketAddress(p_eip_ctrl, &p_Item_Sockaddr_Info->SocketAddress);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    length = (CIP_UINT)(cip_decode_get_current_buffer_ptr(p_cip_ctrl) - p_EIP_Item_data_start);
    if (length != expected_length)
    {
        /* TODO: length mismatch */
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_decode_EIP_Item_SequencedAddress(EIP_ControlStruct const * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_SequencedAddress * const p_Item_SequencedAddress)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;
    RTA_U8 const * p_EIP_Item_data_start;
    CIP_UINT length;

    EIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !expected_length || !p_Item_SequencedAddress)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    /* keep track of where the Item data starts */
    p_EIP_Item_data_start = cip_decode_get_current_buffer_ptr(p_cip_ctrl);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_Item_SequencedAddress->ConnectionIdentifier);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_Item_SequencedAddress->EncapsulationSequenceNumber);
    if (rc != CIP_RETURNCODE_SUCCESS) EIP_LOG_ERROR_AND_RETURN(rc);

    length = (CIP_UINT)(cip_decode_get_current_buffer_ptr(p_cip_ctrl) - p_EIP_Item_data_start);
    if (length != expected_length)
    {
        /* TODO: length mismatch */
    }

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* *********** */
/* END OF FILE */
/* *********** */
