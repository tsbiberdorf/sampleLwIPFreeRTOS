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
#include "cip_system.h"

/* used for debug */
/*! \cond */
#undef  __RTA_FILE__
#define __RTA_FILE__ "cip_encode.c"
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
CIP_ReturnCode cip_encode_CIP_MessageRouterResponse (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse const * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_UINT ii;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_MessageRouterResponse->Service);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_OCTET(p_cip_ctrl, &p_MessageRouterResponse->Reserved);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_MessageRouterResponse->GeneralStatus);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_MessageRouterResponse->AdditionalStatus.NumberofStatuses);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    
    if(p_MessageRouterResponse->AdditionalStatus.NumberofStatuses > 0)
    {
        if(!p_MessageRouterResponse->AdditionalStatus.p_Status)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);
        
        for(ii=0; ii<p_MessageRouterResponse->AdditionalStatus.NumberofStatuses; ii++)
        {
            rc = cip_encode_CIP_WORD(p_cip_ctrl, &p_MessageRouterResponse->AdditionalStatus.p_Status[ii]);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
        }
    }

    switch(p_MessageRouterResponse->Response_Data.Type)
    {
        case CIP_DATA_TYPE_NO_DATA:
            break;

        case CIP_DATA_TYPE_CIP_BOOL: 
            rc = cip_encode_CIP_BOOL(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_BOOL);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_SINT: 
            rc = cip_encode_CIP_SINT(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_SINT);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_INT: 
            rc = cip_encode_CIP_INT(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_INT);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_DINT: 
            rc = cip_encode_CIP_DINT(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_DINT);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_LINT: 
            rc = cip_encode_CIP_LINT(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_LINT);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_USINT: 
            rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_USINT);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_UINT: 
            rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_UINT);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_UDINT: 
            rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_UDINT);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_ULINT: 
            rc = cip_encode_CIP_ULINT(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_ULINT);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_REAL: 
            rc = cip_encode_CIP_REAL(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_REAL);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_LREAL: 
            rc = cip_encode_CIP_LREAL(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_LREAL);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_BYTE: 
            rc = cip_encode_CIP_BYTE(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_BYTE);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_WORD: 
            rc = cip_encode_CIP_WORD(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_WORD);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_DWORD: 
            rc = cip_encode_CIP_DWORD(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_DWORD);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_LWORD: 
            rc = cip_encode_CIP_LWORD(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_LWORD);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_DATE_AND_TIME:
            rc = cip_encode_CIP_DATE_AND_TIME(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_CIP_DATE_AND_TIME);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_CIP_Opaque_String: 
            rc = cip_encode_CIP_Opaque_String(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_Opaque_String);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_SHORT_STRING_32: 
            rc = cip_encode_CIP_SHORT_STRING_32(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_SHORT_STRING_32);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_IdentityObject_Revision: 
            rc = cip_encode_CIP_IdentityObject_Revision(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_IdentityObject_Revision);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_IdentityObject: 
            rc = cip_encode_CIP_IdentityObject(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_IdentityObject);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_ForwardOpen_Response_Success: 
            rc = cip_encode_CIP_ForwardOpen_Response_Success(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_ForwardOpen_Response_Success);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_ForwardOpen_Response_Unsuccessful: 
            rc = cip_encode_CIP_ForwardOpen_Response_Unsuccessful(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_ForwardOpen_Response_Unsuccessful);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_DATA_TYPE_CIP_ForwardClose_Response_Success:
            rc = cip_encode_CIP_ForwardClose_Response_Success(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_ForwardClose_Response_Success);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_CIP_ForwardClose_Response_Unsuccessful:
            rc = cip_encode_CIP_ForwardClose_Response_Unsuccessful(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_ForwardClose_Response_Unsuccessful);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceFlags_Internal:
            rc = cip_encode_EIP_EthernetLinkObject_InterfaceFlags_Internal(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_InterfaceFlags_Internal);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_EthernetLinkObject_PhysicalAddress:
            rc = cip_encode_EIP_EthernetLinkObject_PhysicalAddress(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_PhysicalAddress);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceCounters:
            rc = cip_encode_EIP_EthernetLinkObject_InterfaceCounters(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_InterfaceCounters);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_EthernetLinkObject_MediaCounters:
            rc = cip_encode_EIP_EthernetLinkObject_MediaCounters(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_MediaCounters);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceCapability:
            rc = cip_encode_EIP_EthernetLinkObject_InterfaceCapability(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_InterfaceCapability);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_TCPObject_Status:
            rc = cip_encode_EIP_TCPObject_Status(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_TCPObject_Status);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_TCPObject_ConfigurationCapability:
            rc = cip_encode_EIP_TCPObject_ConfigurationCapability(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_TCPObject_ConfigurationCapability);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_TCPObject_ConfigurationControl:
            rc = cip_encode_EIP_TCPObject_ConfigurationControl(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_TCPObject_ConfigurationControl);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_TCPObject_PhysicalLinkObject:
            rc = cip_encode_EIP_TCPObject_PhysicalLinkObject(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_TCPObject_PhysicalLinkObject);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_EIP_TCPObject_InterfaceConfiguration:
            rc = cip_encode_EIP_TCPObject_InterfaceConfiguration(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_TCPObject_InterfaceConfiguration);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_CIP_STRING48_16BIT_PADDED:
            rc = cip_encode_CIP_STRING_as_MaxNumCharacters_16bitPadded(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_STRINGn_16BIT_PADDED, 48);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_CIP_STRING64_16BIT_PADDED:
            rc = cip_encode_CIP_STRING_as_MaxNumCharacters_16bitPadded(p_cip_ctrl, &p_MessageRouterResponse->Response_Data.Value.value_STRINGn_16BIT_PADDED, 64);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_DATA_TYPE_CIP_STRING: /* we need to know max len / padding rules to encode this */
        case CIP_DATA_TYPE_CIP_ForwardOpen_Request:
        case CIP_DATA_TYPE_CIP_ForwardClose_Request:
        default:
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_CIP_IdentityObject_Revision (CIP_ControlStruct const * const p_cip_ctrl, CIP_IdentityObject_Revision const * const p_IdentityObject_Revision)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_IdentityObject_Revision)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
   
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_IdentityObject_Revision->Major);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_IdentityObject_Revision->Minor);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_CIP_IdentityObject (CIP_ControlStruct const * const p_cip_ctrl, CIP_IdentityObject const * const p_IdentityObject)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_IdentityObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_IdentityObject->VendorID);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_IdentityObject->DeviceType);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_IdentityObject->ProductCode);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_IdentityObject_Revision(p_cip_ctrl, &p_IdentityObject->Revision);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_WORD(p_cip_ctrl, &p_IdentityObject->Status);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_IdentityObject->SerialNumber);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_SHORT_STRING_32(p_cip_ctrl, &p_IdentityObject->ProductName);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_IdentityObject->State);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_CIP_SHORT_STRING_32 (CIP_ControlStruct const * const p_cip_ctrl, CIP_SHORT_STRING_32 const * const p_SHORT_STRING_32)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_SHORT_STRING_32)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_SHORT_STRING_32->Length > 32)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
    
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_SHORT_STRING_32->Length);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    if(p_SHORT_STRING_32->Length > 0)
    {
        rc = cip_encode_memcpy(p_cip_ctrl, p_SHORT_STRING_32->Data, p_SHORT_STRING_32->Length);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_CIP_Opaque_String (CIP_ControlStruct const * const p_cip_ctrl, CIP_Opaque_String const * const p_Opaque_String)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_Opaque_String)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_Opaque_String->Length)
    {
        if(!p_Opaque_String->pData)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

        rc = cip_encode_memcpy(p_cip_ctrl, p_Opaque_String->pData, p_Opaque_String->Length);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_EXIT_AND_RETURN (rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_CIP_DATE_AND_TIME(CIP_ControlStruct const * const p_cip_ctrl, CIP_DATE_AND_TIME const * const p_DATE_AND_TIME)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_DATE_AND_TIME)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_DATE_AND_TIME->Time);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_DATE_AND_TIME->Date);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_CIP_STRING_as_MaxNumCharacters_16bitPadded (CIP_ControlStruct const * const p_cip_ctrl, CIP_STRING const * const p_STRING, RTA_Size_Type const MaxNumCharacters)
{
    CIP_ReturnCode rc;
    CIP_OCTET const pad = 0;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_STRING)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(MaxNumCharacters < 1)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_STRING->Length > MaxNumCharacters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_STRING->Length);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    if(p_STRING->Length)
    {
        if(!p_STRING->pData)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

        rc = cip_encode_memcpy(p_cip_ctrl, p_STRING->pData, p_STRING->Length);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_EXIT_AND_RETURN (rc);
    }

    if(p_STRING->Length % 2) /* if we are an odd length, pad a 0 */
    {
        rc = cip_encode_CIP_OCTET(p_cip_ctrl, &pad);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 1 - Table 3-5.19 Successful Forward_Open / Large_Forward_Open Response */
CIP_ReturnCode cip_encode_CIP_ForwardOpen_Response_Success (CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardOpen_Response_Success const * const p_ForwardOpen_Response_Success)
{
    CIP_ReturnCode rc;
    CIP_USINT const reserved = 0;
    RTA_Size_Type length_in_bytes;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_ForwardOpen_Response_Success)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_ForwardOpen_Response_Success->O2T.NetworkConnectionID);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_ForwardOpen_Response_Success->T2O.NetworkConnectionID);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_ForwardOpen_Response_Success->Connection_Triad.ConnectionSerialNumber);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_ForwardOpen_Response_Success->Connection_Triad.OriginatorVendorID);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_ForwardOpen_Response_Success->Connection_Triad.OriginatorSerialNumber);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_ForwardOpen_Response_Success->O2T.API_in_microseconds);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_ForwardOpen_Response_Success->T2O.API_in_microseconds);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_ForwardOpen_Response_Success->Application.Reply_Size_In_Words);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &reserved);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    if(p_ForwardOpen_Response_Success->Application.Reply_Size_In_Words)
    {
        if(!p_ForwardOpen_Response_Success->Application.p_Reply_Bytes)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

        length_in_bytes = (2 * p_ForwardOpen_Response_Success->Application.Reply_Size_In_Words);
        rc = cip_encode_memcpy(p_cip_ctrl, p_ForwardOpen_Response_Success->Application.p_Reply_Bytes, length_in_bytes);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_EXIT_AND_RETURN (rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 1 - Table 3-5.20 Unsuccessful Forward_Open / Large_Forward_Open Response */
CIP_ReturnCode cip_encode_CIP_ForwardOpen_Response_Unsuccessful (CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardOpen_Response_Unsuccessful const * const p_ForwardOpen_Response_Unsuccessful)
{
    CIP_ReturnCode rc;
    CIP_USINT const reserved = 0;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_ForwardOpen_Response_Unsuccessful)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_ForwardOpen_Response_Unsuccessful->Connection_Triad.ConnectionSerialNumber);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_ForwardOpen_Response_Unsuccessful->Connection_Triad.OriginatorVendorID);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_ForwardOpen_Response_Unsuccessful->Connection_Triad.OriginatorSerialNumber);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    /* remaining path size isn't used since we aren't a router */
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &reserved);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &reserved);        
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 1 - Table 3-5.22 Successful Forward_Close Response */
CIP_ReturnCode cip_encode_CIP_ForwardClose_Response_Success(CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardClose_Response_Success const * const p_ForwardClose_Response_Success)
{
    CIP_ReturnCode rc;
    CIP_USINT const reserved = 0;
    RTA_Size_Type length_in_bytes;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_ForwardClose_Response_Success)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_ForwardClose_Response_Success->Connection_Triad.ConnectionSerialNumber);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_ForwardClose_Response_Success->Connection_Triad.OriginatorVendorID);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_ForwardClose_Response_Success->Connection_Triad.OriginatorSerialNumber);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_ForwardClose_Response_Success->Application.Reply_Size_In_Words);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &reserved);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    if (p_ForwardClose_Response_Success->Application.Reply_Size_In_Words)
    {
        if (!p_ForwardClose_Response_Success->Application.p_Reply_Bytes)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

        length_in_bytes = (2 * p_ForwardClose_Response_Success->Application.Reply_Size_In_Words);
        rc = cip_encode_memcpy(p_cip_ctrl, p_ForwardClose_Response_Success->Application.p_Reply_Bytes, length_in_bytes);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_EXIT_AND_RETURN(rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 1 - Table 3-5.23 Unsuccessful Forward_Close Response */
CIP_ReturnCode cip_encode_CIP_ForwardClose_Response_Unsuccessful(CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardClose_Response_Unsuccessful const * const p_ForwardClose_Response_Unsuccessful)
{
    CIP_ReturnCode rc;
    CIP_USINT const reserved = 0;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_ForwardClose_Response_Unsuccessful)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_ForwardClose_Response_Unsuccessful->Connection_Triad.ConnectionSerialNumber);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_ForwardClose_Response_Unsuccessful->Connection_Triad.OriginatorVendorID);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_ForwardClose_Response_Unsuccessful->Connection_Triad.OriginatorSerialNumber);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    /* remaining path size isn't used since we aren't a router */
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &reserved);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &reserved);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* ********************************************************* */
/* CIP Volume 2 - 5-4.3 Instance Attributes (TCP/IP Object ) */
/* ********************************************************* */

/* CIP Volume 2 - Table 5-4.4 Status Attribute */
CIP_ReturnCode cip_encode_EIP_TCPObject_Status (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_Status const * const p_TCPObject_Status)
{
    CIP_ReturnCode rc;
    CIP_DWORD Status;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_TCPObject_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* bits 0 -3, so just set the initial value */
    switch(p_TCPObject_Status->InterfaceConfigurationStatus)
    {
        case InterfaceConfigurationStatus_NotConfigured:    Status = 0; break; 
        case InterfaceConfigurationStatus_BootpDhcpNvram:   Status = 1; break; 
        case InterfaceConfigurationStatus_Hardware:         Status = 2; break; 

        case InterfaceConfigurationStatus_Unknown:
        default:
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);    
    }

    if(p_TCPObject_Status->McastChangePending)
        Status |= 0x0010; /* bit 4 */

    if(p_TCPObject_Status->InterfaceConfigurationChangePending)
        Status |= 0x0020; /* bit 5 */

    if(p_TCPObject_Status->AcdStatus_ConflictDetected)
        Status |= 0x0040; /* bit 6 */

    if(p_TCPObject_Status->AcdFault_ConflictDetected)
        Status |= 0x0080; /* bit 7 */

    if(p_TCPObject_Status->IANA_Port_Admin_Change_Pending)
        Status |= 0x0100; /* bit 8 */

    if(p_TCPObject_Status->IANA_Protocol_Admin_Change_Pending)
        Status |= 0x0200; /* bit 9 */

    /* bits 10 - 31 are reserved */

    rc = cip_encode_CIP_DWORD(p_cip_ctrl, &Status);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - 5-4.3.2.2 Configuration Capability – Attribute */
CIP_ReturnCode cip_encode_EIP_TCPObject_ConfigurationCapability (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_ConfigurationCapability const * const p_TCPObject_ConfigurationCapability)
{
    CIP_ReturnCode rc;
    CIP_DWORD ConfigurationCapability = 0;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_TCPObject_ConfigurationCapability)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_TCPObject_ConfigurationCapability->BOOTP_Client)
        ConfigurationCapability |= 0x0001; /* bit 0 */

    if(p_TCPObject_ConfigurationCapability->DNS_Client)
        ConfigurationCapability |= 0x0002; /* bit 1 */

    if(p_TCPObject_ConfigurationCapability->DHCP_Client)
        ConfigurationCapability |= 0x0004; /* bit 2 */

    if(p_TCPObject_ConfigurationCapability->DHCP_DNS_Update)
        ConfigurationCapability |= 0x0008; /* bit 3 */

    if(p_TCPObject_ConfigurationCapability->ConfigurationSettable)
        ConfigurationCapability |= 0x0010; /* bit 4 */

    if(p_TCPObject_ConfigurationCapability->HardwareConfigurable)
        ConfigurationCapability |= 0x0020; /* bit 5 */

    if(p_TCPObject_ConfigurationCapability->InterfaceConfigurationChangeRequiresReset)
        ConfigurationCapability |= 0x0040; /* bit 6 */

    if(p_TCPObject_ConfigurationCapability->ACDCapable)
        ConfigurationCapability |= 0x0080; /* bit 7 */

    /* bits 8 - 31 are reserved */

    rc = cip_encode_CIP_DWORD(p_cip_ctrl, &ConfigurationCapability);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - 5-4.3.2.3 Configuration Control – Attribute */
CIP_ReturnCode cip_encode_EIP_TCPObject_ConfigurationControl (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_ConfigurationControl const * const p_TCPObject_ConfigurationControl)
{
    CIP_ReturnCode rc;
    CIP_DWORD ConfigurationControl;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_TCPObject_ConfigurationControl)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* bits 0 -3, so just set the initial value */
    switch(p_TCPObject_ConfigurationControl->ConfigurationMethod)
    {
        case ConfigurationControl_ConfigurationMethod_Static:   ConfigurationControl = 0;   break; 
        case ConfigurationControl_ConfigurationMethod_BOOTP:    ConfigurationControl = 1;   break; 
        case ConfigurationControl_ConfigurationMethod_DHCP:     ConfigurationControl = 2;   break; 

        case ConfigurationControl_ConfigurationMethod_Unknown:
        default:
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);    
    }

    if(p_TCPObject_ConfigurationControl->DNS_Enable)
        ConfigurationControl |= 0x0010; /* bit 4 */

    /* bits 5 - 31 are reserved */

    rc = cip_encode_CIP_DWORD(p_cip_ctrl, &ConfigurationControl);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_EIP_TCPObject_PhysicalLinkObject (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_PhysicalLinkObject const * const p_TCPObject_PhysicalLinkObject)
{
    CIP_ReturnCode rc;
    CIP_UINT const path_size = 2;
    CIP_USINT temp_USINT;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_TCPObject_PhysicalLinkObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* TODO: Change to use the encoding path logic */
    if(p_TCPObject_PhysicalLinkObject->class_id > 255)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
    if(p_TCPObject_PhysicalLinkObject->instance_id > 255)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
    
    rc = cip_encode_CIP_UINT(p_cip_ctrl, &path_size);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    temp_USINT = 0x20;
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &temp_USINT);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    temp_USINT = (CIP_USINT)p_TCPObject_PhysicalLinkObject->class_id;
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &temp_USINT);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    temp_USINT = 0x24;
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &temp_USINT);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    temp_USINT = (CIP_USINT)p_TCPObject_PhysicalLinkObject->instance_id;
    rc = cip_encode_CIP_USINT(p_cip_ctrl, &temp_USINT);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_EIP_TCPObject_InterfaceConfiguration (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_InterfaceConfiguration const * const p_TCPObject_InterfaceConfiguration)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_TCPObject_InterfaceConfiguration)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->IP_Address);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->Network_Mask);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->Gateway_Address);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->Name_Server);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->Name_Server2);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_STRING_as_MaxNumCharacters_16bitPadded(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->Domain_Name, 48);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* ********************************************** */
/* CIP Volume 2 - Table 5-5.3 Instance Attributes */
/* ********************************************** */

/* CIP Volume 2 - Table 5-5.4 Interface Flags */
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceFlags_Internal (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceFlags_Internal const * const p_EthernetLinkObject_InterfaceFlags_Internal)
{
    CIP_ReturnCode rc;
    CIP_DWORD InterfaceFlags = 0;
    CIP_DWORD NegotiationStatus = 0;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_EthernetLinkObject_InterfaceFlags_Internal)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_EthernetLinkObject_InterfaceFlags_Internal->Link_Status_Active)
        InterfaceFlags |= 0x0001; /* bit 0 */

    if(p_EthernetLinkObject_InterfaceFlags_Internal->Link_Full_Duplex)
        InterfaceFlags |= 0x0002; /* bit 1 */

    switch(p_EthernetLinkObject_InterfaceFlags_Internal->Negotiation_Status)
    {
        case NegotiationStatus_AutoNegotiate_in_progress:           NegotiationStatus = 0;  break;
        case NegotiationStatus_AutoNegotiate_failed:                NegotiationStatus = 1;  break;
        case NegotiationStatus_AutoNegotiate_failed_detected_speed: NegotiationStatus = 2;  break;
        case NegotiationStatus_AutoNegotiate_successful:            NegotiationStatus = 3;  break;
        case NegotiationStatus_AutoNegotiate_not_attempted:         NegotiationStatus = 4;  break;
        default:
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);   
    }

    InterfaceFlags |= (NegotiationStatus << 2); /* bits 2-4 */

    if(p_EthernetLinkObject_InterfaceFlags_Internal->ManualSettingRequiresReset)
        InterfaceFlags |= 0x0020; /* bit 5 */

    if(p_EthernetLinkObject_InterfaceFlags_Internal->Local_Hardware_Fault_Detected)
        InterfaceFlags |= 0x0040; /* bit 6 */

    /* Bits 7-31 are reserved */

    rc = cip_encode_CIP_DWORD(p_cip_ctrl, &InterfaceFlags);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - 5-5.3.2.3 Physical Address */
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_PhysicalAddress (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_PhysicalAddress const * const p_EthernetLinkObject_PhysicalAddress)
{
    CIP_ReturnCode rc;
    RTA_Size_Type ii;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_EthernetLinkObject_PhysicalAddress)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    for(ii=0; ii<6; ii++) /* IPV4 is a 6 byte physical address */
    {    
        rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_EthernetLinkObject_PhysicalAddress->PhysicalAddress[ii]);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - 5-5.3.2.4 Interface Counters */
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCounters (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCounters const * const p_EthernetLinkObject_InterfaceCounters)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_EthernetLinkObject_InterfaceCounters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->In_Octets);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->In_Ucast_Packets);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->In_NUcast_Packets);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->In_Discards);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->In_Errors);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->In_Unknown_Protos);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->Out_Octets);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->Out_Ucast_Packets);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->Out_NUcast_Packets);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->Out_Discards);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCounters->Out_Errors);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - 5-5.3.2.5 Media Counters */
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_MediaCounters (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_MediaCounters const * const p_EthernetLinkObject_MediaCounters)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_EthernetLinkObject_MediaCounters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->Alignment_Errors);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->FCS_Errors);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->Single_Collisions);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->Multiple_Collisions);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->SQE_Test_Errors);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->Deferred_Transmissions);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->Late_Collisions);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->Excessive_Collisions);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->MAC_Transmit);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->Carrier_Sense_Errors);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->Frame_Too_Long);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_CIP_UDINT(p_cip_ctrl, &p_EthernetLinkObject_MediaCounters->MAC_Receive_Errors);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - Table 5-5.9 Capability Bits */
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCapability_Bits_Internal (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCapability_Bits_Internal const * const p_EthernetLinkObject_InterfaceCapability_Bits_Internal)
{
    CIP_ReturnCode rc;
    CIP_DWORD CapabilityBits = 0;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_EthernetLinkObject_InterfaceCapability_Bits_Internal)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(p_EthernetLinkObject_InterfaceCapability_Bits_Internal->ManualSettingRequiresReset)
        CapabilityBits |= 0x0001; /* bit 0 */

    if(p_EthernetLinkObject_InterfaceCapability_Bits_Internal->AutoNegotiate_Supported)
        CapabilityBits |= 0x0002; /* bit 1 */

    if(p_EthernetLinkObject_InterfaceCapability_Bits_Internal->AutoMDIX_Supported)
        CapabilityBits |= 0x0004; /* bit 2 */

    if(p_EthernetLinkObject_InterfaceCapability_Bits_Internal->ManualSettings_Supported)
        CapabilityBits |= 0x0008; /* bit 3 */

    /* Bits 4-31 are reserved */

    rc = cip_encode_CIP_DWORD(p_cip_ctrl, &CapabilityBits);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - 5-5.3.2.11 Interface Capability – Attribute 11 */
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array const * const p_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array)
{
    CIP_ReturnCode rc;
    RTA_Size_Type ii;
 
    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array->count);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    for(ii=0; ii<p_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array->count; ii++)
    {
        rc = cip_encode_EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array->p_value[ii]);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex const * const p_EthernetLinkObject_InterfaceCapability_SpeedDuplex)
{
    CIP_ReturnCode rc;
    CIP_USINT InterfaceDuplexMode = 0;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_EthernetLinkObject_InterfaceCapability_SpeedDuplex)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_CIP_UINT(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCapability_SpeedDuplex->InterfaceSpeed);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    switch(p_EthernetLinkObject_InterfaceCapability_SpeedDuplex->InterfaceDuplexMode)
    {
        case InterfaceDuplexMode_Half: InterfaceDuplexMode = 0; break;
        case InterfaceDuplexMode_Full: InterfaceDuplexMode = 1; break;
        default:
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);   
    }

    rc = cip_encode_CIP_USINT(p_cip_ctrl, &InterfaceDuplexMode);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - 5-5.3.2.11 Interface Capability – Attribute 11 */
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCapability (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCapability const * const p_EthernetLinkObject_InterfaceCapability)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_EthernetLinkObject_InterfaceCapability)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_encode_EIP_EthernetLinkObject_InterfaceCapability_Bits_Internal(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCapability->InterfaceCapability_Bits);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_encode_EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array(p_cip_ctrl, &p_EthernetLinkObject_InterfaceCapability->SpeedDuplex_Array);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* ==================================================== */
/*		   LOCAL FUNCTIONS                              */
/* ==================================================== */

/* *********** */
/* END OF FILE */
/* *********** */
