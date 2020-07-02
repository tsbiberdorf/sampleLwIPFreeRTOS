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
#define __RTA_FILE__ "cip_process.c"
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
CIP_BOOL local_obj_msgrouter_is_instance_supported(CIP_CIAM_Path const * const p_path);
CIP_BOOL local_obj_msgrouter_is_service_supported_for_instance(CIP_USINT const service, CIP_UDINT const instance);

CIP_PROCESS_OBJECT_ENTRY CIP_ProcessTable[] = 
{
    {CIP_Class_Code_Identity,           cip_userobj_identity_is_instance_supported, cip_userobj_identity_process_service,   cip_userobj_identity_is_service_supported_for_instance},
    {CIP_Class_Code_Message_Router,     local_obj_msgrouter_is_instance_supported,  RTA_NULL,                               local_obj_msgrouter_is_service_supported_for_instance },
    {CIP_Class_Code_Assembly,           cip_userobj_assembly_is_instance_supported, cip_userobj_assembly_process_service,   cip_userobj_assembly_is_service_supported_for_instance},
    {CIP_Class_Code_Connection_Manager, cip_obj_cnxnmgr_is_instance_supported,      cip_obj_cnxnmgr_process_service,        cip_obj_cnxnmgr_is_service_supported_for_instance},
#if EIP_USER_OPTION_QOS_ENABLED
    {CIP_Class_Code_QoS,                eip_obj_qos_is_instance_supported,          eip_obj_qos_process_service,            eip_obj_qos_is_service_supported_for_instance},
#endif /* #if EIP_USER_OPTION_QOS_ENABLED */
#if EIP_USER_OPTION_DLR_ENABLED
    {CIP_Class_Code_Device_Level_Ring,  eip_obj_dlr_is_instance_supported,          eip_obj_dlr_process_service,            eip_obj_dlr_is_service_supported_for_instance},
#endif /* #if EIP_USER_OPTION_DLR_ENABLED */
    {CIP_Class_Code_TCP_IP_Interface,   eip_userobj_tcp_is_instance_supported,      eip_userobj_tcp_process_service,        eip_userobj_tcp_is_service_supported_for_instance},
    {CIP_Class_Code_Ethernet_Link,      eip_userobj_ethernet_is_instance_supported, eip_userobj_ethernet_process_service,   eip_userobj_ethernet_is_service_supported_for_instance}
};


#define CIP_ProcessTable_Size (sizeof(CIP_ProcessTable)/sizeof(CIP_ProcessTable[0]))

/* ========================= */
/*	 LOCAL DATA	             */
/* ========================= */

/* ========================= */
/*	 MISCELLANEOUS	         */
/* ========================= */

/* ==================================================== */
/*		   GLOBAL FUNCTIONS	                            */
/* ==================================================== */
CIP_ReturnCode cip_process_message (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    RTA_Size_Type ii;
    CIP_ControlStruct const * p_cip_ctrl;

    CIP_PROCESS_OBJECT_ENTRY const * p_entry = RTA_NULL;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;
 
    /* CIP Volume 1 (Common Industrial Protocol) - Table 2-4.2 */
    /* Reply service code is the request service code plus 0x80. */
    p_MessageRouterResponse->Service = (p_MessageRouterRequest->Service + 0x80);
    
    if (p_cip_ctrl->decode_GeneralStatus != CIP_Status_Success)
        return(cip_process_build_error(p_MessageRouterResponse, p_cip_ctrl->decode_GeneralStatus));

    rc = cip_process_convert_CIP_Path_Array_to_CIAM_Path (&p_MessageRouterRequest->Path, &p_MessageRouterRequest->CIAM_Path);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    if(!p_MessageRouterRequest->CIAM_Path.path_valid)
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Path_segment_error));

    if((p_MessageRouterRequest->CIAM_Path.class_id >= CIP_Class_Code_VendorSpecific_FirstValid) && (p_MessageRouterRequest->CIAM_Path.class_id <= CIP_Class_Code_VendorSpecific_LastValid))
    {
        rc = cip_userobj_vendspec_process_service (p_eip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
        /* if we hit an error, the message is stored so this function is OK */
        if(rc == CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    for(ii=0; ii<CIP_ProcessTable_Size; ii++)
    {
        if(CIP_ProcessTable[ii].class_id == p_MessageRouterRequest->CIAM_Path.class_id)
        {
            p_entry = &CIP_ProcessTable[ii];
            break;
        }
    }

    if(!p_entry)
    {
        rc = cip_userobj_vendspec_process_service (p_eip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);

        /* see if the user knows how to process the message */
        if(rc == CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS) 
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
        if(rc == CIP_RETURNCODE_SUCCESS)
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));
    }

    if(!p_entry->fptr_is_instance_supported)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);
    if(!p_entry->fptr_is_instance_supported(&p_MessageRouterRequest->CIAM_Path))
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));

    if(!p_entry->fptr_is_service_supported_for_instance)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);
    if(!p_entry->fptr_is_service_supported_for_instance(p_MessageRouterRequest->Service, p_MessageRouterRequest->CIAM_Path.instance_id))
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Service_not_supported));

    if(!p_entry->fptr_process_service)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);
    rc = p_entry->fptr_process_service (p_eip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
    /* if we hit an error, the message is stored so this function is OK */
    if(rc == CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_build_error (CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_USINT const error)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->GeneralStatus = error;
    p_MessageRouterResponse->AdditionalStatus.NumberofStatuses = 0;
    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_NO_DATA;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_process_CIP_Path_Element_is_a_class_id(CIP_Path_Element const * const p_Path_Element)
{
    if (!p_Path_Element)
        return(RTA_FALSE);

    if (p_Path_Element->SegmentType != CIP_SegmentType_Logical)
        return(RTA_FALSE);

    if (!p_Path_Element->Segment.p_LogicalSegment)
        return(RTA_FALSE);

    if (p_Path_Element->Segment.p_LogicalSegment->LogicalType != CIP_LogicalSegment_LogicalType_ClassID)
        return(RTA_FALSE);

    return(RTA_TRUE);
}

CIP_ReturnCode cip_process_get_class_id_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_class_id)
{
    if (!p_Path_Element || !p_class_id)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* coupled to use the validate from the above function */
    if (!cip_process_CIP_Path_Element_is_a_class_id(p_Path_Element))
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    *p_class_id = p_Path_Element->Segment.p_LogicalSegment->Value.ClassID.Value;
    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_process_CIP_Path_Element_is_a_instance_id(CIP_Path_Element const * const p_Path_Element)
{
    if (!p_Path_Element)
        return(RTA_FALSE);

    if (p_Path_Element->SegmentType != CIP_SegmentType_Logical)
        return(RTA_FALSE);

    if (!p_Path_Element->Segment.p_LogicalSegment)
        return(RTA_FALSE);

    if (p_Path_Element->Segment.p_LogicalSegment->LogicalType != CIP_LogicalSegment_LogicalType_InstanceID)
        return(RTA_FALSE);

    return(RTA_TRUE);
}

CIP_ReturnCode cip_process_get_instance_id_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_instance_id)
{
    if (!p_Path_Element || !p_instance_id)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* coupled to use the validate from the above function */
    if (!cip_process_CIP_Path_Element_is_a_instance_id(p_Path_Element))
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    *p_instance_id = p_Path_Element->Segment.p_LogicalSegment->Value.InstanceID.Value;
    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_process_CIP_Path_Element_is_a_attribute_id(CIP_Path_Element const * const p_Path_Element)
{
    if (!p_Path_Element)
        return(RTA_FALSE);

    if (p_Path_Element->SegmentType != CIP_SegmentType_Logical)
        return(RTA_FALSE);

    if (!p_Path_Element->Segment.p_LogicalSegment)
        return(RTA_FALSE);

    if (p_Path_Element->Segment.p_LogicalSegment->LogicalType != CIP_LogicalSegment_LogicalType_AttributeID)
        return(RTA_FALSE);

    return(RTA_TRUE);
}

CIP_ReturnCode cip_process_get_attribute_id_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_attribute_id)
{
    if (!p_Path_Element || !p_attribute_id)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* coupled to use the validate from the above function */
    if (!cip_process_CIP_Path_Element_is_a_attribute_id(p_Path_Element))
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    *p_attribute_id = p_Path_Element->Segment.p_LogicalSegment->Value.AttributeID.Value;
    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_process_CIP_Path_Element_is_a_member_id(CIP_Path_Element const * const p_Path_Element)
{
    if (!p_Path_Element)
        return(RTA_FALSE);

    if (p_Path_Element->SegmentType != CIP_SegmentType_Logical)
        return(RTA_FALSE);

    if (!p_Path_Element->Segment.p_LogicalSegment)
        return(RTA_FALSE);

    if (p_Path_Element->Segment.p_LogicalSegment->LogicalType != CIP_LogicalSegment_LogicalType_MemberID)
        return(RTA_FALSE);

    return(RTA_TRUE);
}

CIP_ReturnCode cip_process_get_member_id_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_member_id)
{
    if (!p_Path_Element || !p_member_id)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* coupled to use the validate from the above function */
    if (!cip_process_CIP_Path_Element_is_a_member_id(p_Path_Element))
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    *p_member_id = p_Path_Element->Segment.p_LogicalSegment->Value.MemberID.Value;
    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_process_CIP_Path_Element_is_a_connection_point(CIP_Path_Element const * const p_Path_Element)
{
    if (!p_Path_Element)
        return(RTA_FALSE);

    if (p_Path_Element->SegmentType != CIP_SegmentType_Logical)
        return(RTA_FALSE);

    if (!p_Path_Element->Segment.p_LogicalSegment)
        return(RTA_FALSE);

    if (p_Path_Element->Segment.p_LogicalSegment->LogicalType != CIP_LogicalSegment_LogicalType_ConnectionPoint)
        return(RTA_FALSE);

    return(RTA_TRUE);
}

CIP_ReturnCode cip_process_get_connection_point_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_connection_point)
{
    if (!p_Path_Element || !p_connection_point)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* coupled to use the validate from the above function */
    if (!cip_process_CIP_Path_Element_is_a_connection_point(p_Path_Element))
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    *p_connection_point = p_Path_Element->Segment.p_LogicalSegment->Value.ConnectionPoint.Value;
    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_process_CIP_Path_Element_is_an_electronic_key(CIP_Path_Element const * const p_Path_Element)
{
    if (!p_Path_Element)
        return(RTA_FALSE);

    if (p_Path_Element->SegmentType != CIP_SegmentType_Logical)
        return(RTA_FALSE);

    if (!p_Path_Element->Segment.p_LogicalSegment)
        return(RTA_FALSE);

    if (p_Path_Element->Segment.p_LogicalSegment->LogicalType != CIP_LogicalSegment_LogicalType_Special)
        return(RTA_FALSE);

    if (!p_Path_Element->Segment.p_LogicalSegment->Value.p_Special)
        return(RTA_FALSE);

    if (p_Path_Element->Segment.p_LogicalSegment->Value.p_Special->SpecialFormat != CIP_LogicalSegment_SpecialFormat_ElectronicKey)
        return(RTA_FALSE);

    if (p_Path_Element->Segment.p_LogicalSegment->Value.p_Special->KeyFormat != CIP_LogicalSegment_SpecialFormat_KeyFormat_ElectronicKey)
        return(RTA_FALSE);        

    return(RTA_TRUE);
}

CIP_ReturnCode cip_process_get_electronic_key_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_ElectronicKey * const p_ElectronicKey)
{
    if (!p_Path_Element || !p_ElectronicKey)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* coupled to use the validate from the above function */
    if (!cip_process_CIP_Path_Element_is_an_electronic_key(p_Path_Element))
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    memcpy(p_ElectronicKey, &p_Path_Element->Segment.p_LogicalSegment->Value.p_Special->Value.ElectronicKey, sizeof(*p_ElectronicKey));
    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_process_CIP_Path_Element_is_a_simple_data_segment(CIP_Path_Element const * const p_Path_Element)
{
    if (!p_Path_Element)
        return(RTA_FALSE);
    if (p_Path_Element->SegmentType != CIP_SegmentType_Data)
        return(RTA_FALSE);
    if (!p_Path_Element->Segment.p_DataSegment)
        return(RTA_FALSE);
    return(RTA_TRUE);    
}
CIP_ReturnCode cip_process_get_simple_data_segment_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_DataSegment_SimpleDataSegment * const p_DataSegment_SimpleDataSegment)
{
    if (!p_Path_Element || !p_DataSegment_SimpleDataSegment)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
    /* coupled to use the validate from the above function */
    if (!cip_process_CIP_Path_Element_is_a_simple_data_segment(p_Path_Element))
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
    memcpy(p_DataSegment_SimpleDataSegment, &p_Path_Element->Segment.p_DataSegment->Value.SimpleDataSegment, sizeof(*p_DataSegment_SimpleDataSegment));
    return(CIP_RETURNCODE_SUCCESS);
}
CIP_ReturnCode cip_process_convert_CIP_Path_Array_to_CIAM_Path (CIP_Path_Array const * const p_Path, CIP_CIAM_Path * const p_CIAM_Path)
{
    CIP_ReturnCode rc;
    RTA_Size_Type path_ix = 0;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_Path || !p_CIAM_Path)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    memset(p_CIAM_Path, 0, sizeof(*p_CIAM_Path));

    /* the only paths we accept are... CI, CIA, CIAM */

    if (path_ix < p_Path->Path_Element_Count)
    {
        rc = cip_process_get_class_id_from_CIP_Path_Element(&p_Path->Path_Elements[path_ix], &p_CIAM_Path->class_id);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
        p_CIAM_Path->class_id_found = RTA_TRUE;
        path_ix++;
    }

    if (path_ix < p_Path->Path_Element_Count)
    {
        rc = cip_process_get_instance_id_from_CIP_Path_Element(&p_Path->Path_Elements[path_ix], &p_CIAM_Path->instance_id);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
        p_CIAM_Path->instance_id_found = RTA_TRUE;
        path_ix++;
    }

    if (path_ix < p_Path->Path_Element_Count)
    {
        rc = cip_process_get_attribute_id_from_CIP_Path_Element(&p_Path->Path_Elements[path_ix], &p_CIAM_Path->attribute_id);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
        p_CIAM_Path->attribute_id_found = RTA_TRUE;
        path_ix++;
    }

    if (path_ix < p_Path->Path_Element_Count)
    {
        rc = cip_process_get_member_id_from_CIP_Path_Element(&p_Path->Path_Elements[path_ix], &p_CIAM_Path->member_id);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
        p_CIAM_Path->member_id_found = RTA_TRUE;
        path_ix++;
    }

    if (path_ix < p_Path->Path_Element_Count)
    {
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
    }

    /* the minimum for success is CI */
    if (!p_CIAM_Path->class_id_found || !p_CIAM_Path->instance_id_found)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    p_CIAM_Path->path_valid = RTA_TRUE;
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_BOOL(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_BOOL const * const p_BOOL)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_BOOL)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_BOOL;
    p_MessageRouterResponse->Response_Data.Value.value_BOOL = *p_BOOL;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_SINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_SINT const * const p_SINT)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_SINT)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_SINT;
    p_MessageRouterResponse->Response_Data.Value.value_SINT = *p_SINT;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_INT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_INT const * const p_INT)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_INT)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_INT;
    p_MessageRouterResponse->Response_Data.Value.value_INT = *p_INT;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_DINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_DINT const * const p_CIP_DINT)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_CIP_DINT)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_DINT;
    p_MessageRouterResponse->Response_Data.Value.value_DINT = *p_CIP_DINT;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_LINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_LINT const * const p_CIP_LINT)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_CIP_LINT)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_LINT;
    p_MessageRouterResponse->Response_Data.Value.value_LINT = *p_CIP_LINT;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_USINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_USINT const * const p_USINT)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_USINT)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_USINT;
    p_MessageRouterResponse->Response_Data.Value.value_USINT = *p_USINT;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_UINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_UINT const * const p_UINT)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_UINT)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_UINT;
    p_MessageRouterResponse->Response_Data.Value.value_UINT = *p_UINT;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_UDINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_UDINT const * const p_UDINT)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_UDINT)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_UDINT;
    p_MessageRouterResponse->Response_Data.Value.value_UDINT = *p_UDINT;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_ULINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ULINT const * const p_ULINT)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_ULINT)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_ULINT;
    p_MessageRouterResponse->Response_Data.Value.value_ULINT = *p_ULINT;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_REAL(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_REAL const * const p_REAL)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_REAL)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_REAL;
    p_MessageRouterResponse->Response_Data.Value.value_REAL = *p_REAL;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_LREAL(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_LREAL const * const p_LREAL)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_LREAL)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_LREAL;
    p_MessageRouterResponse->Response_Data.Value.value_LREAL = *p_LREAL;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_BYTE(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_BYTE const * const p_BYTE)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_BYTE)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_BYTE;
    p_MessageRouterResponse->Response_Data.Value.value_BYTE = *p_BYTE;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_WORD(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_WORD const * const p_WORD)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_WORD)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_WORD;
    p_MessageRouterResponse->Response_Data.Value.value_WORD = *p_WORD;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_DWORD(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_DWORD const * const p_DWORD)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_DWORD)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_DWORD;
    p_MessageRouterResponse->Response_Data.Value.value_DWORD = *p_DWORD;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_LWORD(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_LWORD const * const p_LWORD)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_LWORD)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_LWORD;
    p_MessageRouterResponse->Response_Data.Value.value_LWORD = *p_LWORD;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_DATE_AND_TIME(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_DATE_AND_TIME const * const p_CIP_DATE_AND_TIME)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_CIP_DATE_AND_TIME)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_DATE_AND_TIME;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_CIP_DATE_AND_TIME, p_CIP_DATE_AND_TIME, sizeof(p_MessageRouterResponse->Response_Data.Value.value_CIP_DATE_AND_TIME));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_Opaque_String(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_Opaque_String const * const p_Opaque_String)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_Opaque_String)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_Opaque_String;    
    p_MessageRouterResponse->Response_Data.Value.value_Opaque_String.Length = p_Opaque_String->Length;

    if(p_MessageRouterResponse->Response_Data.Value.value_Opaque_String.Length > 0)
    {
        p_MessageRouterResponse->Response_Data.Value.value_Opaque_String.pData = cip_binary_malloc(p_cip_ctrl, p_Opaque_String->Length);
        if(!p_MessageRouterResponse->Response_Data.Value.value_Opaque_String.pData)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

        memcpy(p_MessageRouterResponse->Response_Data.Value.value_Opaque_String.pData, p_Opaque_String->pData, p_Opaque_String->Length);       
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_SHORT_STRING_32(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_SHORT_STRING_32 const * const p_SHORT_STRING_32)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_SHORT_STRING_32)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_SHORT_STRING_32;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_SHORT_STRING_32, p_SHORT_STRING_32, sizeof(p_MessageRouterResponse->Response_Data.Value.value_SHORT_STRING_32));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_IdentityObject_Revision(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_IdentityObject_Revision const * const p_IdentityObject_Revision)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_IdentityObject_Revision)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_IdentityObject_Revision;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_IdentityObject_Revision, p_IdentityObject_Revision, sizeof(p_MessageRouterResponse->Response_Data.Value.value_IdentityObject_Revision));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_IdentityObject(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_IdentityObject const * const p_IdentityObject)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_IdentityObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_IdentityObject;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_IdentityObject, p_IdentityObject, sizeof(p_MessageRouterResponse->Response_Data.Value.value_IdentityObject));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_Status(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_Status const * const p_TCPObject_Status)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_TCPObject_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_TCPObject_Status;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_TCPObject_Status, p_TCPObject_Status, sizeof(p_MessageRouterResponse->Response_Data.Value.value_TCPObject_Status));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_ConfigurationCapability(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_ConfigurationCapability const * const p_TCPObject_ConfigurationCapability)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_TCPObject_ConfigurationCapability)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_TCPObject_ConfigurationCapability;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_TCPObject_Status, p_TCPObject_ConfigurationCapability, sizeof(p_MessageRouterResponse->Response_Data.Value.value_TCPObject_ConfigurationCapability));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_ConfigurationControl(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_ConfigurationControl const * const p_TCPObject_ConfigurationControl)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_TCPObject_ConfigurationControl)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_TCPObject_ConfigurationControl;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_TCPObject_ConfigurationControl, p_TCPObject_ConfigurationControl, sizeof(p_MessageRouterResponse->Response_Data.Value.value_TCPObject_ConfigurationControl));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_PhysicalLinkObject(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_PhysicalLinkObject const * const p_TCPObject_PhysicalLinkObject)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_TCPObject_PhysicalLinkObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_TCPObject_PhysicalLinkObject;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_TCPObject_PhysicalLinkObject, p_TCPObject_PhysicalLinkObject, sizeof(p_MessageRouterResponse->Response_Data.Value.value_TCPObject_PhysicalLinkObject));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_InterfaceConfiguration(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_InterfaceConfiguration const * const p_TCPObject_InterfaceConfiguration)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_TCPObject_InterfaceConfiguration)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_TCPObject_InterfaceConfiguration;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_TCPObject_InterfaceConfiguration, p_TCPObject_InterfaceConfiguration, sizeof(p_MessageRouterResponse->Response_Data.Value.value_TCPObject_InterfaceConfiguration));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_STRING64_16BIT_PADDED(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_STRING const * const p_STRING)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterResponse || !p_STRING)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_STRING64_16BIT_PADDED;

    p_MessageRouterResponse->Response_Data.Value.value_STRINGn_16BIT_PADDED.Length = p_STRING->Length;

    if(p_MessageRouterResponse->Response_Data.Value.value_STRINGn_16BIT_PADDED.Length)
    {
        p_MessageRouterResponse->Response_Data.Value.value_STRINGn_16BIT_PADDED.pData = cip_binary_malloc(p_cip_ctrl, p_STRING->Length);
        if(!p_MessageRouterResponse->Response_Data.Value.value_Opaque_String.pData)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

        memcpy(p_MessageRouterResponse->Response_Data.Value.value_STRINGn_16BIT_PADDED.pData, p_STRING->pData, p_STRING->Length);       
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceFlags_Internal(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_InterfaceFlags_Internal const * const p_EthernetLinkObject_InterfaceFlags_Internal)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_EthernetLinkObject_InterfaceFlags_Internal)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceFlags_Internal;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_InterfaceFlags_Internal, p_EthernetLinkObject_InterfaceFlags_Internal, sizeof(p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_InterfaceFlags_Internal));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_PhysicalAddress(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_PhysicalAddress const * const p_EthernetLinkObject_PhysicalAddress)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_EthernetLinkObject_PhysicalAddress)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_EthernetLinkObject_PhysicalAddress;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_PhysicalAddress, p_EthernetLinkObject_PhysicalAddress, sizeof(p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_PhysicalAddress));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceCounters(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_InterfaceCounters const * const p_EthernetLinkObject_InterfaceCounters)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_EthernetLinkObject_InterfaceCounters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceCounters;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_InterfaceCounters, p_EthernetLinkObject_InterfaceCounters, sizeof(p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_InterfaceCounters));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_MediaCounters(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_MediaCounters const * const p_EthernetLinkObject_MediaCounters)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_EthernetLinkObject_MediaCounters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_EthernetLinkObject_MediaCounters;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_MediaCounters, p_EthernetLinkObject_MediaCounters, sizeof(p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_MediaCounters));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceCapability(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_InterfaceCapability const * const p_EthernetLinkObject_InterfaceCapability)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_EthernetLinkObject_InterfaceCapability)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceCapability;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_InterfaceCapability, p_EthernetLinkObject_InterfaceCapability, sizeof(p_MessageRouterResponse->Response_Data.Value.value_EthernetLinkObject_InterfaceCapability));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}   

CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardOpen_Request(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardOpen_Request const * const p_ForwardOpen_Request)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_ForwardOpen_Request)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_ForwardOpen_Request;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_ForwardOpen_Request, p_ForwardOpen_Request, sizeof(p_MessageRouterResponse->Response_Data.Value.value_ForwardOpen_Request));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardOpen_Response_Success(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardOpen_Response_Success const * const p_ForwardOpen_Response_Success)
{
    CIP_ForwardOpen_Application_Reply * p_ApplicationReply_Destination;
    CIP_ForwardOpen_Application_Reply const * p_ApplicationReply_Source;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterResponse || !p_ForwardOpen_Response_Success)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_ForwardOpen_Response_Success;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_ForwardOpen_Response_Success, p_ForwardOpen_Response_Success, sizeof(p_MessageRouterResponse->Response_Data.Value.value_ForwardOpen_Response_Success));

    p_ApplicationReply_Destination = &p_MessageRouterResponse->Response_Data.Value.value_ForwardOpen_Response_Success.Application;
    p_ApplicationReply_Source = &p_ForwardOpen_Response_Success->Application;

    if(p_ApplicationReply_Source->Reply_Size_In_Words)
    {
        p_ApplicationReply_Destination->Reply_Size_In_Words = p_ApplicationReply_Source->Reply_Size_In_Words;
        p_ApplicationReply_Destination->p_Reply_Bytes = cip_binary_malloc(p_cip_ctrl, (2*p_ApplicationReply_Destination->Reply_Size_In_Words));
        if(!p_ApplicationReply_Destination->p_Reply_Bytes)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

        memcpy(p_ApplicationReply_Destination->p_Reply_Bytes, p_ApplicationReply_Source->p_Reply_Bytes, p_ApplicationReply_Destination->Reply_Size_In_Words);       
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardOpen_Response_Unsuccessful(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardOpen_Response_Unsuccessful const * const p_ForwardOpen_Response_Unsuccessful)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_ForwardOpen_Response_Unsuccessful)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_ForwardOpen_Response_Unsuccessful;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_ForwardOpen_Response_Unsuccessful, p_ForwardOpen_Response_Unsuccessful, sizeof(p_MessageRouterResponse->Response_Data.Value.value_ForwardOpen_Response_Unsuccessful));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardClose_Request(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardClose_Request const * const p_ForwardClose_Request)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_ForwardClose_Request)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_ForwardClose_Request;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_ForwardClose_Request, p_ForwardClose_Request, sizeof(p_MessageRouterResponse->Response_Data.Value.value_ForwardClose_Request));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardClose_Response_Success(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardClose_Response_Success const * const p_ForwardClose_Response_Success)
{
    CIP_ForwardClose_Application_Reply * p_ApplicationReply_Destination;
    CIP_ForwardClose_Application_Reply const * p_ApplicationReply_Source;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterResponse || !p_ForwardClose_Response_Success)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_ForwardClose_Response_Success;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_ForwardClose_Response_Success, p_ForwardClose_Response_Success, sizeof(p_MessageRouterResponse->Response_Data.Value.value_ForwardClose_Response_Success));

    p_ApplicationReply_Destination = &p_MessageRouterResponse->Response_Data.Value.value_ForwardClose_Response_Success.Application;
    p_ApplicationReply_Source = &p_ForwardClose_Response_Success->Application;

    if (p_ApplicationReply_Source->Reply_Size_In_Words)
    {
        p_ApplicationReply_Destination->Reply_Size_In_Words = p_ApplicationReply_Source->Reply_Size_In_Words;
        p_ApplicationReply_Destination->p_Reply_Bytes = cip_binary_malloc(p_cip_ctrl, (2 * p_ApplicationReply_Destination->Reply_Size_In_Words));
        if (!p_ApplicationReply_Destination->p_Reply_Bytes)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

        memcpy(p_ApplicationReply_Destination->p_Reply_Bytes, p_ApplicationReply_Source->p_Reply_Bytes, p_ApplicationReply_Destination->Reply_Size_In_Words);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardClose_Response_Unsuccessful(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardClose_Response_Unsuccessful const * const p_ForwardClose_Response_Unsuccessful)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_MessageRouterResponse || !p_ForwardClose_Response_Unsuccessful)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_MessageRouterResponse->Response_Data.Type = CIP_DATA_TYPE_CIP_ForwardClose_Response_Unsuccessful;
    memcpy(&p_MessageRouterResponse->Response_Data.Value.value_ForwardClose_Response_Unsuccessful, p_ForwardClose_Response_Unsuccessful, sizeof(p_MessageRouterResponse->Response_Data.Value.value_ForwardClose_Response_Unsuccessful));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* ==================================================== */
/*		   LOCAL FUNCTIONS	                            */
/* ==================================================== */
CIP_BOOL local_obj_msgrouter_is_instance_supported(CIP_CIAM_Path const * const p_path)
{
    if (!p_path)
        return(RTA_FALSE);

    if (!p_path->instance_id_found)
        return(RTA_FALSE);

    if (p_path->instance_id == CIP_InstanceID_Class_Level)
        return(RTA_TRUE);
    if (p_path->instance_id == 1) /* instance level */
        return(RTA_TRUE);
    return(RTA_FALSE);
}

CIP_BOOL local_obj_msgrouter_is_service_supported_for_instance(CIP_USINT const service, CIP_UDINT const instance)
{
    RTA_UNUSED_PARAM(service);
    RTA_UNUSED_PARAM(instance);

    return(RTA_FALSE);
}

/* *********** */
/* END OF FILE */
/* *********** */
