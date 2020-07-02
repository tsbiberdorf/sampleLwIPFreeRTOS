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
 *  www.rtaautomation.com
 */

/* ========================== */
/*	 INCLUDE FILES	          */
/* ========================== */
#include "cip_system.h"

/* used for debug */
/*! \cond */
#undef  __RTA_FILE__
#define __RTA_FILE__ "cip_decode.c"
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

static CIP_ReturnCode local_decode_CIP_Path_Element_SegmentType_Logical (CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const segment_type, CIP_LogicalSegment * const p_LogicalSegment);
static CIP_ReturnCode local_decode_CIP_LogicalSegment_Normal (CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const segment_type, CIP_LogicalSegment_ClassID * const p_LogicalSegment_Normal);
#define local_decode_CIP_LogicalSegment_ClassID local_decode_CIP_LogicalSegment_Normal
#define local_decode_CIP_LogicalSegment_InstanceID local_decode_CIP_LogicalSegment_Normal
#define local_decode_CIP_LogicalSegment_MemberID local_decode_CIP_LogicalSegment_Normal
#define local_decode_CIP_LogicalSegment_ConnectionPoint local_decode_CIP_LogicalSegment_Normal
#define local_decode_CIP_LogicalSegment_AttributeID local_decode_CIP_LogicalSegment_Normal
static CIP_ReturnCode local_decode_CIP_LogicalSegment_Special (CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const segment_type, CIP_LogicalSegment_Special * const p_LogicalSegment_Special);
static CIP_ReturnCode local_decode_CIP_ElectronicKey (CIP_ControlStruct const * const p_cip_ctrl, CIP_ElectronicKey * const p_ElectronicKey);
static CIP_ReturnCode local_decode_CIP_Path_Element_SegmentType_Data (CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const segment_type, CIP_DataSegment * const p_DataSegment);
static CIP_ReturnCode local_decode_CIP_MessageRouterRequest_Data (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest);

/* ========================= */
/*	 LOCAL DATA	             */
/* ========================= */

/* ========================= */
/*	 MISCELLANEOUS	         */
/* ========================= */

/* ==================================================== */
/*		   GLOBAL FUNCTIONS	                            */
/* ==================================================== */
CIP_ReturnCode cip_decode_store_and_return_general_status(CIP_ControlStruct * const p_cip_ctrl, CIP_USINT const general_status)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl->decode_GeneralStatus = general_status;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouterRequest (CIP_ControlStruct * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest)
{
    CIP_ReturnCode rc;
    RTA_U8 const * p_data_start;
    CIP_UINT used_length;
    CIP_UINT remaining_length;
    
    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !expected_length || !p_MessageRouterRequest)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* keep track of where the Item data starts */
    p_data_start = cip_decode_get_current_buffer_ptr(p_cip_ctrl);

    rc = cip_decode_CIP_USINT(p_cip_ctrl, &p_MessageRouterRequest->Service);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_USINT(p_cip_ctrl, &p_MessageRouterRequest->Path.Path_Element_Count);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_Path_Array(p_cip_ctrl, p_MessageRouterRequest->Path.Path_Element_Count, &p_MessageRouterRequest->Path);
    if (rc == CIP_RETURNCODE_ERROR_FUNCTION_FAILED) CIP_LOG_EXIT_AND_RETURN(cip_decode_store_and_return_general_status(p_cip_ctrl, CIP_Status_Path_segment_error));
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    used_length = (CIP_UINT)(cip_decode_get_current_buffer_ptr(p_cip_ctrl) - p_data_start);
    if(used_length < expected_length)
    {
        remaining_length = (CIP_UINT)(expected_length - used_length);
        rc = local_decode_CIP_MessageRouterRequest_Data(p_cip_ctrl, remaining_length, p_MessageRouterRequest);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_Path_Array(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const path_size_in_words, CIP_Path_Array * const p_Path_Array)
{
    CIP_ReturnCode rc;
    RTA_Size_Type expected_size_in_bytes;
    RTA_Size_Type actual_size_in_bytes;
    CIP_Path_Element * p_Path_Element;
    RTA_U8 const * p_data_start;
    CIP_USINT segment_type;
    CIP_BOOL error_decoding = RTA_FALSE;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_Path_Array)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
   
    /* keep track of where the Item data starts */
    p_data_start = cip_decode_get_current_buffer_ptr(p_cip_ctrl);

    expected_size_in_bytes = (RTA_Size_Type)(path_size_in_words * 2);
    actual_size_in_bytes = 0;
    p_Path_Array->Path_Element_Count = 0;
    while((actual_size_in_bytes < expected_size_in_bytes) && (p_Path_Array->Path_Element_Count < CIP_MAX_PATH_ELEMENTS))
    {
        p_Path_Element = &p_Path_Array->Path_Elements[p_Path_Array->Path_Element_Count]; 

        rc = cip_decode_CIP_USINT(p_cip_ctrl, &segment_type);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
        
        if(error_decoding)
        {
            actual_size_in_bytes = (RTA_Size_Type)(cip_decode_get_current_buffer_ptr(p_cip_ctrl) - p_data_start);
            continue;
        }

        p_Path_Element->SegmentType = CIP_SegmentType_get_type_from_typeformat(segment_type);
        switch(p_Path_Element->SegmentType)
        {
            case CIP_SegmentType_Logical:
                p_Path_Element->Segment.p_LogicalSegment = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Path_Element->Segment.p_LogicalSegment));
                if(!p_Path_Element->Segment.p_LogicalSegment)
                    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

                rc = local_decode_CIP_Path_Element_SegmentType_Logical(p_cip_ctrl, segment_type, p_Path_Element->Segment.p_LogicalSegment);
                if(rc != CIP_RETURNCODE_SUCCESS)
                {
                    p_Path_Element->SegmentType = CIP_SegmentType_Unknown;
                    error_decoding = RTA_TRUE;
                }
                break;

            case CIP_SegmentType_Data:
                p_Path_Element->Segment.p_DataSegment = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Path_Element->Segment.p_DataSegment));
                if(!p_Path_Element->Segment.p_DataSegment)
                    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
                rc = local_decode_CIP_Path_Element_SegmentType_Data(p_cip_ctrl, segment_type, p_Path_Element->Segment.p_DataSegment);
                if(rc != CIP_RETURNCODE_SUCCESS)
                {
                    p_Path_Element->SegmentType = CIP_SegmentType_Unknown;
                    error_decoding = RTA_TRUE;
                }
                break;
            case CIP_SegmentType_Port:
            case CIP_SegmentType_Network:
            case CIP_SegmentType_Symbolic:
            case CIP_SegmentType_DataType_Constructed:
            case CIP_SegmentType_DataType_Elementary:
            case CIP_SegmentType_Reserved:
            default:
                p_Path_Element->SegmentType = CIP_SegmentType_Unknown;
                error_decoding = RTA_TRUE;
                break;
        }

        p_Path_Array->Path_Element_Count++;

        actual_size_in_bytes = (RTA_Size_Type)(cip_decode_get_current_buffer_ptr(p_cip_ctrl) - p_data_start);
    }

    if(actual_size_in_bytes != expected_size_in_bytes)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT);

    if(error_decoding)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_STRING(CIP_ControlStruct const * const p_cip_ctrl, CIP_STRING * const p_STRING)
{
    CIP_ReturnCode rc;
    CIP_UINT length;
    CIP_OCTET pad;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_STRING)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &length);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    p_STRING->Length = length;
    if (p_STRING->Length)
    {
        p_STRING->pData = cip_binary_malloc(p_cip_ctrl, p_STRING->Length);
        if (!p_STRING->pData)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

        rc = cip_decode_memcpy(p_cip_ctrl, p_STRING->pData, p_STRING->Length);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
    }

    if (length % 2)
    {
        rc = cip_decode_CIP_OCTET(p_cip_ctrl, &pad);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_Opaque_String (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_Opaque_String * const p_Opaque_String)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_Opaque_String)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_Opaque_String->Length = expected_length;

    if(p_Opaque_String->Length)
    {
        p_Opaque_String->pData = cip_binary_malloc(p_cip_ctrl, p_Opaque_String->Length);
        if(!p_Opaque_String->pData)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

        rc = cip_decode_memcpy(p_cip_ctrl, p_Opaque_String->pData, p_Opaque_String->Length);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* ==================================================== */
/*		   LOCAL FUNCTIONS	                            */
/* ==================================================== */
static CIP_ReturnCode local_decode_CIP_Path_Element_SegmentType_Logical (CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const segment_type, CIP_LogicalSegment * const p_LogicalSegment)
{
    CIP_ReturnCode rc;
    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_LogicalSegment)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_LogicalSegment->LogicalType = CIP_LogicalSegment_get_LogicalType_from_typeformat(segment_type);
    
    switch(p_LogicalSegment->LogicalType)
    {
        case CIP_LogicalSegment_LogicalType_ClassID:
            rc = local_decode_CIP_LogicalSegment_ClassID(p_cip_ctrl, segment_type, &p_LogicalSegment->Value.ClassID);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_LogicalSegment_LogicalType_InstanceID:
            rc = local_decode_CIP_LogicalSegment_InstanceID(p_cip_ctrl, segment_type, &p_LogicalSegment->Value.InstanceID);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_LogicalSegment_LogicalType_MemberID:
            rc = local_decode_CIP_LogicalSegment_MemberID(p_cip_ctrl, segment_type, &p_LogicalSegment->Value.MemberID);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_LogicalSegment_LogicalType_ConnectionPoint:
            rc = local_decode_CIP_LogicalSegment_ConnectionPoint(p_cip_ctrl, segment_type, &p_LogicalSegment->Value.ConnectionPoint);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_LogicalSegment_LogicalType_AttributeID:
            rc = local_decode_CIP_LogicalSegment_AttributeID(p_cip_ctrl, segment_type, &p_LogicalSegment->Value.AttributeID);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_LogicalSegment_LogicalType_Special:
            p_LogicalSegment->Value.p_Special = cip_binary_malloc(p_cip_ctrl, sizeof(*p_LogicalSegment->Value.p_Special));
            if(!p_LogicalSegment->Value.p_Special)
                CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

            rc = local_decode_CIP_LogicalSegment_Special(p_cip_ctrl, segment_type, p_LogicalSegment->Value.p_Special);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_LogicalSegment_LogicalType_ServiceID:
        case CIP_LogicalSegment_LogicalType_ExtendedLogical:
        default:
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_decode_CIP_LogicalSegment_Normal (CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const segment_type, CIP_LogicalSegment_ClassID * const p_LogicalSegment_Normal)
{
    CIP_ReturnCode rc;
    CIP_USINT   val_8bit, pad_byte;
    CIP_UINT    val_16bit;
    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_LogicalSegment_Normal)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_LogicalSegment_Normal->LogicalFormat = CIP_LogicalSegment_get_LogicalFormat_from_typeformat(segment_type);
    
    switch(p_LogicalSegment_Normal->LogicalFormat)
    {
        case CIP_LogicalSegment_LogicalFormat_8Bit:
            rc = cip_decode_CIP_USINT(p_cip_ctrl, &val_8bit);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

            p_LogicalSegment_Normal->Value = (CIP_UDINT)val_8bit;
            break;

        case CIP_LogicalSegment_LogicalFormat_16Bit:
            rc = cip_decode_CIP_USINT(p_cip_ctrl, &pad_byte);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

            rc = cip_decode_CIP_UINT(p_cip_ctrl, &val_16bit);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

            p_LogicalSegment_Normal->Value = (CIP_UDINT)val_16bit;
            break;

        case CIP_LogicalSegment_LogicalFormat_32Bit:
            rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_LogicalSegment_Normal->Value);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_LogicalSegment_LogicalFormat_Reserved:
        default:
                CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_decode_CIP_LogicalSegment_Special (CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const segment_type, CIP_LogicalSegment_Special * const p_LogicalSegment_Special)
{
    CIP_ReturnCode rc;
    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_LogicalSegment_Special)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_LogicalSegment_Special->SpecialFormat = CIP_LogicalSegment_get_SpecialFormat_from_typeformat(segment_type);
    switch(p_LogicalSegment_Special->SpecialFormat)
    {
        case CIP_LogicalSegment_SpecialFormat_ElectronicKey:

            rc = cip_decode_CIP_USINT(p_cip_ctrl, &p_LogicalSegment_Special->KeyFormat);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

            if(p_LogicalSegment_Special->KeyFormat != CIP_LogicalSegment_SpecialFormat_KeyFormat_ElectronicKey)
                CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT);

            rc = local_decode_CIP_ElectronicKey(p_cip_ctrl, &p_LogicalSegment_Special->Value.ElectronicKey);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
                CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_decode_CIP_ElectronicKey (CIP_ControlStruct const * const p_cip_ctrl, CIP_ElectronicKey * const p_ElectronicKey)
{
    CIP_ReturnCode rc;
    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_ElectronicKey)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_ElectronicKey->VendorID);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_ElectronicKey->DeviceType);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_ElectronicKey->ProductCode);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_BYTE(p_cip_ctrl, &p_ElectronicKey->MajorRevision);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_USINT(p_cip_ctrl, &p_ElectronicKey->MinorRevision);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    /* CIP Volume 1 (Common Industrial Protocol) - Table C-1.4 Electronic Key Segment Format */
    if(p_ElectronicKey->MajorRevision & 0x80)
    {
        p_ElectronicKey->Compatibility = RTA_TRUE;
        p_ElectronicKey->MajorRevision = (CIP_BYTE)(p_ElectronicKey->MajorRevision & 0x7F);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_decode_CIP_Path_Element_SegmentType_Data (CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const segment_type, CIP_DataSegment * const p_DataSegment)
{
    CIP_ReturnCode  rc;
    RTA_Size_Type   word_ix;
    CIP_LOG_FUNCTION_ENTER;
    if(!p_cip_ctrl || !p_DataSegment)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
    p_DataSegment->DataType = CIP_PortSegment_get_PortType_from_typeformat(segment_type);
    switch(p_DataSegment->DataType)
    {
        case CIP_PortSegment_PortType_SimpleDataSegment:
            rc = cip_decode_CIP_USINT(p_cip_ctrl, &p_DataSegment->Value.SimpleDataSegment.Data_Size_In_Words);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            p_DataSegment->Value.SimpleDataSegment.p_WordData = cip_binary_malloc(p_cip_ctrl, (p_DataSegment->Value.SimpleDataSegment.Data_Size_In_Words*2));
            if(!p_DataSegment->Value.SimpleDataSegment.p_WordData)
                CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
            for(word_ix=0; word_ix<p_DataSegment->Value.SimpleDataSegment.Data_Size_In_Words; word_ix++)
            {
                rc = cip_decode_CIP_WORD(p_cip_ctrl, &p_DataSegment->Value.SimpleDataSegment.p_WordData[word_ix]);
                if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            }
            break;
        case CIP_PortSegment_PortType_ANSI_ExtendedSymbolSegment:
        default:
                CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_DECODE_PATH_SEGMENT);
    }
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}
static CIP_ReturnCode local_decode_CIP_MessageRouterRequest_Data (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest)
{
    CIP_ReturnCode rc;
    RTA_U8 const * p_decode_buffer_on_enter;
    RTA_U8 const * p_encode_buffer_on_enter;
    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !expected_length || !p_MessageRouterRequest)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* TODO: if this pattern is duplicated, create cip functions to mark positions and revert positions as a pair */
    p_decode_buffer_on_enter = cip_decode_get_current_buffer_ptr(p_cip_ctrl);
    if(!p_decode_buffer_on_enter)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_encode_buffer_on_enter = cip_encode_get_current_buffer_ptr(p_cip_ctrl);
    if(!p_encode_buffer_on_enter)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    rc = cip_process_convert_CIP_Path_Array_to_CIAM_Path (&p_MessageRouterRequest->Path, &p_MessageRouterRequest->CIAM_Path);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    if(!p_MessageRouterRequest->CIAM_Path.path_valid)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
   
    if(!p_MessageRouterRequest->CIAM_Path.class_id_found)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if(!p_MessageRouterRequest->CIAM_Path.instance_id_found)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if(p_MessageRouterRequest->CIAM_Path.class_id >= CIP_Class_Code_VendorSpecific_FirstValid && p_MessageRouterRequest->CIAM_Path.class_id <= CIP_Class_Code_VendorSpecific_LastValid)
    {
        rc = cip_userobj_vendspec_decode(p_cip_ctrl, expected_length, p_MessageRouterRequest);
        if(rc == CIP_RETURNCODE_SUCCESS)
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    /* Any error may mean we don't understand the data, so return on success and fall through
        to the opaque data storing on error. */
    switch(p_MessageRouterRequest->CIAM_Path.class_id)
    {
        case CIP_Class_Code_Connection_Manager:
            rc = cip_obj_cnxnmgr_decode(p_cip_ctrl, expected_length, p_MessageRouterRequest);
            if(rc == CIP_RETURNCODE_SUCCESS)  
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
            break;

        case CIP_Class_Code_TCP_IP_Interface:
            rc = eip_userobj_tcp_decode(p_cip_ctrl, expected_length, p_MessageRouterRequest);
            if(rc == CIP_RETURNCODE_SUCCESS)  
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
            break;

#if EIP_USER_OPTION_QOS_ENABLED
        case CIP_Class_Code_QoS:
            rc = eip_obj_qos_decode(p_cip_ctrl, expected_length, p_MessageRouterRequest);
            if(rc == CIP_RETURNCODE_SUCCESS)  
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
            break;
#endif /* #if EIP_USER_OPTION_QOS_ENABLED */

        default:
            rc = cip_userobj_vendspec_decode(p_cip_ctrl, expected_length, p_MessageRouterRequest);
            if(rc == CIP_RETURNCODE_SUCCESS)
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
            break;   
    }

    /* unknown data / bad data, store all data to opaque data */
    cip_binary_decode_rewind(p_cip_ctrl, p_decode_buffer_on_enter); /* discard what we may have decoded */
    cip_binary_encode_rewind(p_cip_ctrl, p_encode_buffer_on_enter); /* discard what we may have encoded */

    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_Opaque_String(p_cip_ctrl, expected_length, &p_MessageRouterRequest->Request_Data);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_BOOL(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_BOOL;

    rc = cip_decode_CIP_BOOL(p_cip_ctrl, &p_MessageRouter_Data->Value.value_BOOL);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_USINT;

    rc = cip_decode_CIP_USINT(p_cip_ctrl, &p_MessageRouter_Data->Value.value_USINT);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_UINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_UINT;

    rc = cip_decode_CIP_UINT (p_cip_ctrl, &p_MessageRouter_Data->Value.value_UINT);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_UDINT;

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_MessageRouter_Data->Value.value_UDINT);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_SINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_SINT;

    rc = cip_decode_CIP_SINT(p_cip_ctrl, &p_MessageRouter_Data->Value.value_SINT);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_INT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_INT;

    rc = cip_decode_CIP_INT (p_cip_ctrl, &p_MessageRouter_Data->Value.value_INT);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_DINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_DINT;

    rc = cip_decode_CIP_DINT(p_cip_ctrl, &p_MessageRouter_Data->Value.value_DINT);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_REAL(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_REAL;

    rc = cip_decode_CIP_REAL(p_cip_ctrl, &p_MessageRouter_Data->Value.value_REAL);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_STRING(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_STRING;

    rc = cip_decode_CIP_STRING(p_cip_ctrl, &p_MessageRouter_Data->Value.value_STRING);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_Opaque_String (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;
    CIP_Opaque_String * p_Opaque_String;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_Opaque_String;
    p_Opaque_String = &p_MessageRouter_Data->Value.value_Opaque_String;
    p_Opaque_String->Length = expected_length;

    if(expected_length)
    {
        p_Opaque_String->pData = cip_binary_malloc(p_cip_ctrl, expected_length);
        if(!p_Opaque_String->pData)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

        rc = cip_decode_memcpy(p_cip_ctrl, p_Opaque_String->pData, expected_length);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 1 (Common Industrial Protocol) - Table 3-5.17 Forward_Open / Large_Forward_Open Request */
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_ForwardOpen_Request (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{   
    CIP_ReturnCode rc;
    CIP_OCTET reserved;
    RTA_Size_Type ii;
    CIP_USINT path_size_in_words;
    CIP_ForwardOpen_Request * p_ForwardOpen_Request;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_ForwardOpen_Request;
    p_ForwardOpen_Request = &p_MessageRouter_Data->Value.value_ForwardOpen_Request;

    rc = cip_decode_CIP_BYTE (p_cip_ctrl, &p_ForwardOpen_Request->Priority_TimeTick);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_USINT (p_cip_ctrl, &p_ForwardOpen_Request->Timeout_Ticks);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UDINT (p_cip_ctrl, &p_ForwardOpen_Request->O2T.NetworkConnectionID);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UDINT (p_cip_ctrl, &p_ForwardOpen_Request->T2O.NetworkConnectionID);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UINT (p_cip_ctrl, &p_ForwardOpen_Request->Connection_Triad.ConnectionSerialNumber);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UINT (p_cip_ctrl, &p_ForwardOpen_Request->Connection_Triad.OriginatorVendorID);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UDINT (p_cip_ctrl, &p_ForwardOpen_Request->Connection_Triad.OriginatorSerialNumber);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_USINT (p_cip_ctrl, &p_ForwardOpen_Request->ConnectionTimeoutMultiplier);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    for(ii=0; ii<3; ii++)
    {
        rc = cip_decode_CIP_OCTET (p_cip_ctrl, &reserved);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    }

    rc = cip_decode_CIP_UDINT (p_cip_ctrl, &p_ForwardOpen_Request->O2T.RPI_in_microseconds);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_WORD (p_cip_ctrl, &p_ForwardOpen_Request->O2T.NetworkConnectionParameters);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_UDINT (p_cip_ctrl, &p_ForwardOpen_Request->T2O.RPI_in_microseconds);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_WORD (p_cip_ctrl, &p_ForwardOpen_Request->T2O.NetworkConnectionParameters);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_BYTE (p_cip_ctrl, &p_ForwardOpen_Request->TransportClass_and_Trigger);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_decode_CIP_USINT(p_cip_ctrl, &path_size_in_words);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_Path_Array(p_cip_ctrl, path_size_in_words, &p_ForwardOpen_Request->Connection_Path);
    if(rc != CIP_RETURNCODE_ERROR_FUNCTION_FAILED) /* the connection manager takes care of this error */
        CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 1 (Common Industrial Protocol) - Table 3-5.21 Forward_Close Service Request */
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_ForwardClose_Request(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;
    CIP_USINT reserved;
    CIP_USINT path_size_in_words;
    CIP_ForwardClose_Request * p_ForwardClose_Request;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_CIP_ForwardClose_Request;
    p_ForwardClose_Request = &p_MessageRouter_Data->Value.value_ForwardClose_Request;

    rc = cip_decode_CIP_BYTE(p_cip_ctrl, &p_ForwardClose_Request->Priority_TimeTick);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_USINT(p_cip_ctrl, &p_ForwardClose_Request->Timeout_Ticks);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_ForwardClose_Request->Connection_Triad.ConnectionSerialNumber);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &p_ForwardClose_Request->Connection_Triad.OriginatorVendorID);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_ForwardClose_Request->Connection_Triad.OriginatorSerialNumber);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_USINT(p_cip_ctrl, &path_size_in_words);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_USINT(p_cip_ctrl, &reserved);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_Path_Array(p_cip_ctrl, path_size_in_words, &p_ForwardClose_Request->Connection_Path);
    if(rc != CIP_RETURNCODE_ERROR_FUNCTION_FAILED) /* the connection manager takes care of this error */
        CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - 5-4.3.2.3 Configuration Control – Attribute */
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_EIP_TCPObject_ConfigurationControl(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;
    EIP_TCPObject_ConfigurationControl * p_TCPObject_ConfigurationControl;
    CIP_DWORD ConfigurationControl;
    CIP_DWORD ConfigurationMethod;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_EIP_TCPObject_ConfigurationControl;
    p_TCPObject_ConfigurationControl = &p_MessageRouter_Data->Value.value_TCPObject_ConfigurationControl;

    rc = cip_decode_CIP_DWORD(p_cip_ctrl, &ConfigurationControl);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    ConfigurationMethod = (ConfigurationControl & 0x000F);
    
    /* bits 0 -3, so just set the initial value */
    switch(ConfigurationMethod)
    {
        case 0:     p_TCPObject_ConfigurationControl->ConfigurationMethod = ConfigurationControl_ConfigurationMethod_Static;    break; 
        case 1:     p_TCPObject_ConfigurationControl->ConfigurationMethod = ConfigurationControl_ConfigurationMethod_BOOTP;     break; 
        case 2:     p_TCPObject_ConfigurationControl->ConfigurationMethod = ConfigurationControl_ConfigurationMethod_DHCP;      break; 
        default:    p_TCPObject_ConfigurationControl->ConfigurationMethod = ConfigurationControl_ConfigurationMethod_Unknown;   break; 
    }

    if(ConfigurationControl & 0x0010) /* bit 4 */
        p_TCPObject_ConfigurationControl->DNS_Enable = RTA_TRUE;

    /* bits 5 - 31 are reserved */

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* CIP Volume 2 - 5-4.3.2.5 Interface Configuration – Attribute */
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_EIP_TCPObject_InterfaceConfiguration(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data)
{
    CIP_ReturnCode rc;
    EIP_TCPObject_InterfaceConfiguration * p_TCPObject_InterfaceConfiguration;
    CIP_UINT expected_length;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouter_Data)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouter_Data->Type = CIP_DATA_TYPE_EIP_TCPObject_InterfaceConfiguration;
    p_TCPObject_InterfaceConfiguration = &p_MessageRouter_Data->Value.value_TCPObject_InterfaceConfiguration;

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->IP_Address);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->Network_Mask);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->Gateway_Address);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->Name_Server);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UDINT(p_cip_ctrl, &p_TCPObject_InterfaceConfiguration->Name_Server2);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_decode_CIP_UINT(p_cip_ctrl, &expected_length);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    if (expected_length > 0)
    {
        rc = cip_decode_CIP_Opaque_String(p_cip_ctrl, expected_length, &p_TCPObject_InterfaceConfiguration->Domain_Name);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
    }
    else
    {   
        p_TCPObject_InterfaceConfiguration->Domain_Name.Length = 0;
    }
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* *********** */
/* END OF FILE */
/* *********** */
