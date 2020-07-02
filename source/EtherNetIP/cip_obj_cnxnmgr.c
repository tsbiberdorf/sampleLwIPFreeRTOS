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
#include "eip_system.h" /* Connection Manager is just for EtherNet/IP for our implementation */

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "cip_obj_cnxnmgr.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */

static CIP_ReturnCode local_process_CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen (EIP_ControlStruct const * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */
static CIP_DWORD milliseconds_since_init;

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */

/* CIP Volume 1 (Common Industrial Protocol) - Table 3-5.7 Connection Manager Object Object-Specific Services */
#define CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen 0x54
#define CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose 0x4E

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode cip_obj_cnxnmgr_init (CIP_InitType const init_type)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    switch(init_type)
    {
        case CIP_INITTYPE_NORMAL:
        case CIP_INITTYPE_OUTOFBOX:
        case CIP_INITTYPE_OUTOFBOX_NOCOMMS:
        case CIP_INITTYPE_MFR:
        default:
            break;
    }

    milliseconds_since_init = 0;

    rc = cip_obj_cnxnmgr_explicit_init(init_type);
    if(rc != CIP_RETURNCODE_SUCCESS)
    CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_obj_cnxnmgr_implicit_init(init_type);
    if(rc != CIP_RETURNCODE_SUCCESS)
    CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_obj_cnxnmgr_process (RTA_U32 const ms_since_last_call)
{
    CIP_ReturnCode rc;

    milliseconds_since_init += ms_since_last_call;

    rc = cip_obj_cnxnmgr_explicit_process(ms_since_last_call);
    if(rc != CIP_RETURNCODE_SUCCESS)
    CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_obj_cnxnmgr_implicit_process(ms_since_last_call);
    if(rc != CIP_RETURNCODE_SUCCESS)
    CIP_LOG_ERROR_AND_RETURN(rc);
    
    return(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_obj_cnxnmgr_decode (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    RTA_UNUSED_PARAM(expected_length); /* only used if indeterminent length data is passed */

    if(!p_cip_ctrl || !p_MessageRouterRequest)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    switch(p_MessageRouterRequest->Service)
    {
        case CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen:
            rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_ForwardOpen_Request(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose:
            rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_ForwardClose_Request(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        default:
            rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_Opaque_String(p_cip_ctrl, expected_length, &p_MessageRouterRequest->Request_Data);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_obj_cnxnmgr_is_instance_supported (CIP_CIAM_Path const * const p_path)
{
    if(!p_path)
        return(RTA_FALSE);

    if(!p_path->instance_id_found)
        return(RTA_FALSE);

    if (p_path->instance_id == CIP_InstanceID_Class_Level)
        return(RTA_TRUE);
    if(p_path->instance_id == 1) /* instance level */
        return(RTA_TRUE);

    return(RTA_FALSE);
}

/* Since this is called from a function pointer, we can't restrict p_eip_ctrl to be 'const * const' as some functions need to modify p_eip_ctrl's contents */
/*lint -efunc(818, cip_obj_cnxnmgr_process_service) */ /* 818 - Pointer parameter 'p_eip_ctrl' could be declared ptr to const */
CIP_ReturnCode cip_obj_cnxnmgr_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    /* path was pre-validated for class_id and instance_id, so feel free to use it */

    switch(p_MessageRouterRequest->Service)
    {
        case CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen:
            rc = local_process_CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen (p_eip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose:
            rc = local_process_CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose(p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        default:
            CIP_LOG_ERROR_AND_RETURN (CIP_RETURNCODE_ERROR_INTERNAL);
    }    

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_obj_cnxnmgr_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance)
{
    if(instance == CIP_InstanceID_Class_Level)
    {
        return(RTA_FALSE);
    }

    else /* instance level */
    {
        switch(service)
        {
            case CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen:
            case CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose:
                return(RTA_TRUE);
            default:    
                return(RTA_FALSE);
        }
    }
}

CIP_ReturnCode cip_obj_cnxnmgr_convert_network_to_Internal_TransportClass_and_Trigger (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_BYTE const network_TransportClass_and_Trigger, CIP_CNXNMGR_Internal_TransportClass_and_Trigger * const p_Internal_TransportClass_and_Trigger)
{
    CIP_USINT ProductionTrigger;
    CIP_USINT TransportClass;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct || !p_Internal_TransportClass_and_Trigger)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* CIP Volume 1 - 3-4.4.3 TransportClass_trigger */

    if(network_TransportClass_and_Trigger & 0x80) /* bit 7 */
        p_Internal_TransportClass_and_Trigger->Direction_isServer = RTA_TRUE;
    else
        p_Internal_TransportClass_and_Trigger->Direction_isServer = RTA_FALSE;

    ProductionTrigger = ((network_TransportClass_and_Trigger & 0x70) >> 4); /* bits 6 - 4 */
    switch(ProductionTrigger)
    {
        case 0: p_Internal_TransportClass_and_Trigger->ProductionTrigger = ProductionTrigger_Cyclic;            break;
        case 1: p_Internal_TransportClass_and_Trigger->ProductionTrigger = ProductionTrigger_ChangeOfState;     break;
        case 2: p_Internal_TransportClass_and_Trigger->ProductionTrigger = ProductionTrigger_ApplicationObject; break;
        default:
            p_Internal_TransportClass_and_Trigger->ProductionTrigger = ProductionTrigger_Unknown; 
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_TRANSPORT_CLASS_AND_TRIGGER_COMBINATION_NOT_SUPPORTED));
    }
    
    TransportClass = (network_TransportClass_and_Trigger & 0x0F); /* bit 3 - 0 */
    switch(TransportClass)
    {
        case 0: p_Internal_TransportClass_and_Trigger->TransportClass = CIP_CNXNMGR_TransportClass_0;    break;
        case 1: p_Internal_TransportClass_and_Trigger->TransportClass = CIP_CNXNMGR_TransportClass_1;    break;
        case 2: p_Internal_TransportClass_and_Trigger->TransportClass = CIP_CNXNMGR_TransportClass_2;    break;
        case 3: p_Internal_TransportClass_and_Trigger->TransportClass = CIP_CNXNMGR_TransportClass_3;    break;
        default:
            p_Internal_TransportClass_and_Trigger->TransportClass = CIP_CNXNMGR_TransportClass_Unknown; 
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_TRANSPORT_CLASS_AND_TRIGGER_COMBINATION_NOT_SUPPORTED));
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS); 
}

CIP_ReturnCode cip_obj_cnxnmgr_convert_network_to_Internal_NetworkConnectionParameters_ForwardOpen (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UINT const network_NetworkConnectionParameters, CIP_CNXNMGR_Internal_NetworkConnectionParameters * const p_Internal_NetworkConnectionParameters)
{
    CIP_UINT ConnectionType;
    CIP_UINT Priority;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct || !p_Internal_NetworkConnectionParameters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* CIP Volume 1 - 3-5.4.1.1 Network Connection Parameters (for general rules) */
    /* CIP Volume 1 - Table 3-5.8 Network Connection Parameters for bit definitions specific to Forward_Open (not Large_Forward_Open) */

    /* since this is a field that is per direction and the error codes are per direction, we won't error on bad values */

    if(network_NetworkConnectionParameters & 0x8000) /* bit 15 */
        p_Internal_NetworkConnectionParameters->RedundantOwner = RTA_TRUE;
    else
        p_Internal_NetworkConnectionParameters->RedundantOwner = RTA_FALSE;

    ConnectionType = ((network_NetworkConnectionParameters & 0x6000) >> 13); /* bits 14-13 */
    switch(ConnectionType)
    {
        case 0:     p_Internal_NetworkConnectionParameters->ConnectionType = ConnectionType_Null;           break;
        case 1:     p_Internal_NetworkConnectionParameters->ConnectionType = ConnectionType_Multicast;       break;
        case 2:     p_Internal_NetworkConnectionParameters->ConnectionType = ConnectionType_PointToPoint;   break;
        case 3:     p_Internal_NetworkConnectionParameters->ConnectionType = ConnectionType_Reserved;       break;
        default:    p_Internal_NetworkConnectionParameters->ConnectionType = ConnectionType_Unknown;        break;
    }

    /* bit 12 is reserved */

    Priority = ((network_NetworkConnectionParameters & 0x0C00) >> 10); /* bits 11 - 10 */
    switch(Priority)
    {
        case 0:     p_Internal_NetworkConnectionParameters->Priority = Priority_Low;        break;
        case 1:     p_Internal_NetworkConnectionParameters->Priority = Priority_High;       break;
        case 2:     p_Internal_NetworkConnectionParameters->Priority = Priority_Scheduled;  break;
        case 3:     p_Internal_NetworkConnectionParameters->Priority = Priority_Urgent;     break;
        default:    p_Internal_NetworkConnectionParameters->Priority = Priority_Unknown;    break;
    }

    if(network_NetworkConnectionParameters & 0x0200) /* bit 9 */
        p_Internal_NetworkConnectionParameters->isVariableSize = RTA_TRUE;
    else
        p_Internal_NetworkConnectionParameters->isVariableSize = RTA_FALSE;

    /* bits 8 - 0 */
    p_Internal_NetworkConnectionParameters->ConnectionSize = (network_NetworkConnectionParameters & 0x01FF);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);    
}

CIP_ReturnCode cip_obj_cnxnmgr_convert_network_to_Internal_ConnectionTimeoutMultiplier (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_USINT const network_ConnectionTimeoutMultiplier, CIP_CNXNMGR_Internal_ConnectionTimeoutMultiplier * const p_Internal_ConnectionTimeoutMultiplier)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct || !p_Internal_ConnectionTimeoutMultiplier)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* CIP Volume 1 - 3-5.4.1.4 Connection Timeout Multiplier */

    switch(network_ConnectionTimeoutMultiplier)
    {
        case 0: *p_Internal_ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_4X;   break;
        case 1: *p_Internal_ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_8X;   break;
        case 2: *p_Internal_ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_16X;  break;
        case 3: *p_Internal_ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_32X;  break;
        case 4: *p_Internal_ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_64X;  break;
        case 5: *p_Internal_ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_128X; break;
        case 6: *p_Internal_ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_256X; break;
        case 7: *p_Internal_ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_512X; break;
        default:     
            *p_Internal_ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_Unknown;
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_CONNECTION_TIMEOUT_MULTIPLIER_NOT_ACCEPTABLE));
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS); 
}

CIP_ReturnCode cip_obj_cnxnmgr_convert_API_and_Multiplier_to_milliseconds(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const API_in_milliseconds, CIP_CNXNMGR_Internal_ConnectionTimeoutMultiplier const ConnectionTimeoutMultiplier, CIP_UDINT * const p_milliseconds)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct || !p_milliseconds)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    switch (ConnectionTimeoutMultiplier)
    {
        case ConnectionTimeoutMultiplier_4X:    *p_milliseconds = (CIP_UDINT)(4 * API_in_milliseconds);     break;
        case ConnectionTimeoutMultiplier_8X:    *p_milliseconds = (CIP_UDINT)(8 * API_in_milliseconds);     break;
        case ConnectionTimeoutMultiplier_16X:   *p_milliseconds = (CIP_UDINT)(16 * API_in_milliseconds);    break;
        case ConnectionTimeoutMultiplier_32X:   *p_milliseconds = (CIP_UDINT)(32 * API_in_milliseconds);    break;
        case ConnectionTimeoutMultiplier_64X:   *p_milliseconds = (CIP_UDINT)(64 * API_in_milliseconds);    break;
        case ConnectionTimeoutMultiplier_128X:  *p_milliseconds = (CIP_UDINT)(128 * API_in_milliseconds);   break;
        case ConnectionTimeoutMultiplier_256X:  *p_milliseconds = (CIP_UDINT)(256 * API_in_milliseconds);   break;
        case ConnectionTimeoutMultiplier_512X:  *p_milliseconds = (CIP_UDINT)(512 * API_in_milliseconds);   break;
        
        case ConnectionTimeoutMultiplier_Unknown:
        default:
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_CONNECTION_TIMEOUT_MULTIPLIER_NOT_ACCEPTABLE));
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_obj_cnxnmgr_validate_RPIs_are_in_range (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const O2T_RPI_in_microseconds, CIP_UDINT const T2O_RPI_in_microseconds)
{
    CIP_ReturnCode rc;
    CIP_ForwardOpen_Response_Unsuccessful ForwardOpen_Response_Unsuccessful;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_Connection_Triad const * p_Connection_Triad;

    CIP_USINT   O2T_ErrorType = 0;
    CIP_UDINT   O2T_ErrorRPI = 0;
    CIP_USINT   T2O_ErrorType = 0;
    CIP_UDINT   T2O_ErrorRPI = 0;
    CIP_OCTET   Additional_Status_Octet_Array[10];
    RTA_Size_Type   word_ix, byte_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /*  Acceptable RPI (see below) type, values:
            0 – the RPI specified in the forward open was acceptable (the returned RPI value is ignored).
            1 – unspecified (used to suggest an alternate RPI, e.g. default)
            2 – minimum acceptable RPI (used when RPI was too fast for range)
            3 – maximum acceptable RPI (used when RPI was too slow for range)
            4 – required RPI to correct mismatch (used when data already being consumed at a different interval)
            5-255 – reserve */
    if (O2T_RPI_in_microseconds < CIP_OBJ_CNXNMGR_MIN_RPI_IN_MICROSECONDS)
    {
        O2T_ErrorType   = 2;
        O2T_ErrorRPI    = CIP_OBJ_CNXNMGR_MIN_RPI_IN_MICROSECONDS;
    }
    else if (O2T_RPI_in_microseconds > CIP_OBJ_CNXNMGR_MAX_RPI_IN_MICROSECONDS)
    {
        O2T_ErrorType   = 3;
        O2T_ErrorRPI    = CIP_OBJ_CNXNMGR_MAX_RPI_IN_MICROSECONDS;
    }

    if (T2O_RPI_in_microseconds < CIP_OBJ_CNXNMGR_MIN_RPI_IN_MICROSECONDS)
    {
        T2O_ErrorType   = 2;
        T2O_ErrorRPI    = CIP_OBJ_CNXNMGR_MIN_RPI_IN_MICROSECONDS;
    }
    else if (T2O_RPI_in_microseconds > CIP_OBJ_CNXNMGR_MAX_RPI_IN_MICROSECONDS)
    {
        T2O_ErrorType   = 3;
        T2O_ErrorRPI    = CIP_OBJ_CNXNMGR_MAX_RPI_IN_MICROSECONDS;
    }

    if((O2T_ErrorType == 0) && (T2O_ErrorType == 0))
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

    p_cip_ctrl = p_ErrorStruct->p_cip_ctrl;
    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_Connection_Triad = p_ErrorStruct->p_Connection_Triad;
    
    /* CIP Volume 1 - Table 3-5.33 Connection Manager Service Error Codes */
    /* Build special error code */
    p_MessageRouterResponse->GeneralStatus = CIP_Status_Connection_failure;
    p_MessageRouterResponse->AdditionalStatus.NumberofStatuses = 6;
    p_MessageRouterResponse->AdditionalStatus.p_Status = cip_binary_malloc(p_cip_ctrl, (p_MessageRouterResponse->AdditionalStatus.NumberofStatuses * sizeof(*p_MessageRouterResponse->AdditionalStatus.p_Status)));
    if(!p_MessageRouterResponse->AdditionalStatus.p_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    /* TODO: is there a common / not brute force way to do this? */
    Additional_Status_Octet_Array[0] = O2T_ErrorType;
    Additional_Status_Octet_Array[1] = T2O_ErrorType;
    rta_endian_PutLitEndian32(O2T_ErrorRPI, &Additional_Status_Octet_Array[2]);
    rta_endian_PutLitEndian32(T2O_ErrorRPI, &Additional_Status_Octet_Array[6]);

    p_MessageRouterResponse->AdditionalStatus.p_Status[0] = CIP_Connection_failure_ExtStatus_RPI_VALUES_NOT_ACCEPTABLE;
    
    /* convert the byte representation to words */
    word_ix = 1;
    for(byte_ix=0; byte_ix<10; byte_ix+=2, word_ix++)
    {
        p_MessageRouterResponse->AdditionalStatus.p_Status[word_ix] = rta_endian_GetLitEndian16(&Additional_Status_Octet_Array[byte_ix]);
    }

    memcpy(&ForwardOpen_Response_Unsuccessful.Connection_Triad, p_Connection_Triad, sizeof(ForwardOpen_Response_Unsuccessful.Connection_Triad));

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Unsuccessful(p_MessageRouterResponse, &ForwardOpen_Response_Unsuccessful);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    /* special code since we already sent the response */
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ABORTED_PROCESS); /* don't treat return as an error, just a known abort */
}

CIP_ReturnCode cip_obj_cnxnmgr_validate_ElectronicKey(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_ElectronicKey const * const p_ElectronicKey)
{
    CIP_ReturnCode rc;
    CIP_IdentityObject IdentityObject;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct || !p_ElectronicKey)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* CIP Volume 1 - Table C-1.5 Key Format Table */
    /*  When the Compatibility bit is set(1) the Vendor ID, Device Type, Product Code and Revision are used to identify the device the recipient
    is being requested to emulate; therefore 0 is an invalid value for the Vendor ID, Device Type, Product Code, Major Revision and Minor Revision fields. */
    /* NOTE: Historically this is the Pass_segment_error, but I can't find a specification reference */
    if ((p_ElectronicKey->Compatibility) &&
        ((p_ElectronicKey->VendorID == 0) || (p_ElectronicKey->DeviceType == 0) || (p_ElectronicKey->ProductCode == 0)))
    {
        return(cip_process_build_error(p_ErrorStruct->p_MessageRouterResponse, CIP_Status_Path_segment_error));
    }

    rc = cip_userobj_identity_get_CIP_IdentityObject(p_ErrorStruct->p_cip_ctrl, &IdentityObject);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    /* CIP Volume 1 - Table 5A-3.10 Electronic Key Segment Error Response Values */

    /* we can only emulate older revisions so don't look at the compatibility bit here*/
    if ((p_ElectronicKey->VendorID != 0) && (p_ElectronicKey->VendorID != IdentityObject.VendorID))
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_VENDOR_ID_OR_PRODUCT_CODE_MISMATCH));
    if ((p_ElectronicKey->ProductCode != 0) && (p_ElectronicKey->ProductCode != IdentityObject.ProductCode))
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_VENDOR_ID_OR_PRODUCT_CODE_MISMATCH));
    if ((p_ElectronicKey->DeviceType != 0) && (p_ElectronicKey->DeviceType != IdentityObject.DeviceType))
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_DEVICE_TYPE_MISMATCH));


    if (p_ElectronicKey->MajorRevision)
    {
        if (p_ElectronicKey->MajorRevision != IdentityObject.Revision.Major)
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_REVISION_MISMATCH));

        /* minor revision is dependent on major revision; we are only compatible on minor revision less than ours */
        if (p_ElectronicKey->Compatibility)
        {
            if ((p_ElectronicKey->MinorRevision == 0) || (p_ElectronicKey->MinorRevision > IdentityObject.Revision.Minor))
                return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_REVISION_MISMATCH));
        }
        else
        {
            /* minor revision is dependent on major revision */
            if ((p_ElectronicKey->MinorRevision) && (p_ElectronicKey->MinorRevision != IdentityObject.Revision.Minor))
                return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_REVISION_MISMATCH));
        }
    }
    else
    {
        if (p_ElectronicKey->Compatibility)
        {
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_REVISION_MISMATCH));
        }
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_obj_cnxnmgr_build_forwardopen_error (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UINT const extended_status)
{
    CIP_ReturnCode rc;
    CIP_ForwardOpen_Response_Unsuccessful ForwardOpen_Response_Unsuccessful;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_Connection_Triad const * p_Connection_Triad;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = p_ErrorStruct->p_cip_ctrl;
    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_Connection_Triad = p_ErrorStruct->p_Connection_Triad;

    p_MessageRouterResponse->GeneralStatus = CIP_Status_Connection_failure;
    p_MessageRouterResponse->AdditionalStatus.NumberofStatuses = 1;
    p_MessageRouterResponse->AdditionalStatus.p_Status = cip_binary_malloc(p_cip_ctrl, (p_MessageRouterResponse->AdditionalStatus.NumberofStatuses * sizeof(*p_MessageRouterResponse->AdditionalStatus.p_Status)));
    if(!p_MessageRouterResponse->AdditionalStatus.p_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    p_MessageRouterResponse->AdditionalStatus.p_Status[0] = extended_status;

    memcpy(&ForwardOpen_Response_Unsuccessful.Connection_Triad, p_Connection_Triad, sizeof(ForwardOpen_Response_Unsuccessful.Connection_Triad));

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Unsuccessful(p_MessageRouterResponse, &ForwardOpen_Response_Unsuccessful);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
}

CIP_ReturnCode cip_obj_cnxnmgr_build_forwardopen_error_multiple_extended_status (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_USINT NumberofStatuses, CIP_UINT const * const p_Statuses)
{
    CIP_ReturnCode rc;
    CIP_ForwardOpen_Response_Unsuccessful ForwardOpen_Response_Unsuccessful;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_Connection_Triad const * p_Connection_Triad;
    RTA_Size_Type status_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = p_ErrorStruct->p_cip_ctrl;
    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_Connection_Triad = p_ErrorStruct->p_Connection_Triad;

    p_MessageRouterResponse->GeneralStatus = CIP_Status_Connection_failure;
    p_MessageRouterResponse->AdditionalStatus.NumberofStatuses = NumberofStatuses;
    p_MessageRouterResponse->AdditionalStatus.p_Status = cip_binary_malloc(p_cip_ctrl, (p_MessageRouterResponse->AdditionalStatus.NumberofStatuses * sizeof(*p_MessageRouterResponse->AdditionalStatus.p_Status)));
    if(!p_MessageRouterResponse->AdditionalStatus.p_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    for(status_ix=0; status_ix<NumberofStatuses; status_ix++)
        p_MessageRouterResponse->AdditionalStatus.p_Status[status_ix] = p_Statuses[status_ix];

    memcpy(&ForwardOpen_Response_Unsuccessful.Connection_Triad, p_Connection_Triad, sizeof(ForwardOpen_Response_Unsuccessful.Connection_Triad));

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Unsuccessful(p_MessageRouterResponse, &ForwardOpen_Response_Unsuccessful);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
}

CIP_ReturnCode cip_obj_cnxnmgr_build_forwardclose_error(CIP_OBJ_ForwardClose_ErrorStruct const * const p_ErrorStruct, CIP_UINT const extended_status)
{
    CIP_ReturnCode rc;
    CIP_ForwardClose_Response_Unsuccessful ForwardClose_Response_Unsuccessful;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_Connection_Triad const * p_Connection_Triad;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = p_ErrorStruct->p_cip_ctrl;
    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_Connection_Triad = p_ErrorStruct->p_Connection_Triad;

    p_MessageRouterResponse->GeneralStatus = CIP_Status_Connection_failure;
    p_MessageRouterResponse->AdditionalStatus.NumberofStatuses = 1;
    p_MessageRouterResponse->AdditionalStatus.p_Status = cip_binary_malloc(p_cip_ctrl, (p_MessageRouterResponse->AdditionalStatus.NumberofStatuses * sizeof(*p_MessageRouterResponse->AdditionalStatus.p_Status)));
    if (!p_MessageRouterResponse->AdditionalStatus.p_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    p_MessageRouterResponse->AdditionalStatus.p_Status[0] = extended_status;

    memcpy(&ForwardClose_Response_Unsuccessful.Connection_Triad, p_Connection_Triad, sizeof(ForwardClose_Response_Unsuccessful.Connection_Triad));

    rc = cip_process_write_response_data_with_CIP_ForwardClose_Response_Unsuccessful(p_MessageRouterResponse, &ForwardClose_Response_Unsuccessful);
    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_UDINT cip_obj_cnxnmgr_get_ConnectionID (void)
{
    static CIP_BOOL     first_time = RTA_TRUE;
    static CIP_UDINT    connection_id = 0;
    RTA_U32 rta_rand_val;

    /* on the first time, set the base connection id  */
    /* this needs to be uniques on every power up */
    if (first_time)
    {
        first_time = RTA_FALSE;
        rta_rand_val = rta_user_rand_u32(milliseconds_since_init);
        connection_id = ((rta_rand_val << 16) & 0xFFFF0000); 
    }

    /* get the next connection id and return it */
    connection_id++;

    return(connection_id);
}

/* CIP Volume 2 - 2-5.5.2 TCP Connection Management for EtherNet/IP */
void cip_obj_cnxnmgr_timeout_common (CIP_UDINT const owning_ip_that_timed_out)
{
    if( !cip_obj_cnxnmgr_explicit_ip_still_open(owning_ip_that_timed_out) && 
        !cip_obj_cnxnmgr_implicit_ip_still_open(owning_ip_that_timed_out))
    {
        eip_sockets_close_all_tcp_by_ip_address(owning_ip_that_timed_out);
    }    
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */
static CIP_ReturnCode local_process_CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen (EIP_ControlStruct const * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_ForwardOpen_Request const * p_ForwardOpen_Request;
    CIP_CNXNMGR_Internal_TransportClass_and_Trigger Internal_TransportClass_and_Trigger;
    CIP_OBJ_ForwardOpen_ErrorStruct ErrorStruct;
    CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct = &ErrorStruct; /* common calling convention */
    CIP_ControlStruct const * p_cip_ctrl;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);   

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_ForwardOpen_Request)
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data));

    p_ForwardOpen_Request = &p_MessageRouterRequest->Request_Data.Value.value_ForwardOpen_Request;

    ErrorStruct.p_cip_ctrl = p_cip_ctrl;
    ErrorStruct.p_Connection_Triad = &p_ForwardOpen_Request->Connection_Triad;
    ErrorStruct.p_MessageRouterResponse = p_MessageRouterResponse;

    rc = cip_obj_cnxnmgr_convert_network_to_Internal_TransportClass_and_Trigger(p_ErrorStruct, p_ForwardOpen_Request->TransportClass_and_Trigger, &Internal_TransportClass_and_Trigger);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    switch (Internal_TransportClass_and_Trigger.TransportClass)
    {
        case CIP_CNXNMGR_TransportClass_0:
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_TRANSPORT_CLASS_NOT_SUPPORTED));

        case CIP_CNXNMGR_TransportClass_1:
            if (Internal_TransportClass_and_Trigger.Direction_isServer) /* I/O is a client trigger */
                return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_DIRECTION_NOT_SUPPORTED));

            if (Internal_TransportClass_and_Trigger.ProductionTrigger != ProductionTrigger_Cyclic)
                return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_T2O_PRODUCTION_TRIGGER_NOT_SUPPORTED));

            rc = cip_obj_cnxnmgr_implicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen_Class1(p_eip_ctrl, p_ForwardOpen_Request, p_ErrorStruct);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_CNXNMGR_TransportClass_2:
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_TRANSPORT_CLASS_NOT_SUPPORTED));

        case CIP_CNXNMGR_TransportClass_3:
            if (!Internal_TransportClass_and_Trigger.Direction_isServer)
                return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_DIRECTION_NOT_SUPPORTED));

            if (Internal_TransportClass_and_Trigger.ProductionTrigger != ProductionTrigger_ApplicationObject)
                return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_T2O_PRODUCTION_TRIGGER_NOT_SUPPORTED));

            rc = cip_obj_cnxnmgr_explicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen_Class3(p_cip_ctrl, p_ForwardOpen_Request, p_ErrorStruct);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;

        case CIP_CNXNMGR_TransportClass_Unknown:
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_TRANSPORT_CLASS_NOT_SUPPORTED));
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_ForwardClose_Request const * p_ForwardClose_Request;
    CIP_OBJ_ForwardClose_ErrorStruct ErrorStruct;
    CIP_OBJ_ForwardClose_ErrorStruct const * const p_ErrorStruct = &ErrorStruct; /* common calling convention */
    
    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if (p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_ForwardClose_Request)
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data));

    p_ForwardClose_Request = &p_MessageRouterRequest->Request_Data.Value.value_ForwardClose_Request;

    ErrorStruct.p_cip_ctrl = p_cip_ctrl;
    ErrorStruct.p_Connection_Triad = &p_ForwardClose_Request->Connection_Triad;
    ErrorStruct.p_MessageRouterResponse = p_MessageRouterResponse;
    
    if(cip_obj_cnxnmgr_implicit_find_connection_by_Connection_Triad(&p_ForwardClose_Request->Connection_Triad))
    {
        rc = cip_obj_cnxnmgr_implicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose_Class1(p_cip_ctrl, p_ForwardClose_Request, p_MessageRouterResponse);
        CIP_LOG_EXIT_AND_RETURN(rc);
    }
    else if(cip_obj_cnxnmgr_explicit_find_connection_by_Connection_Triad(&p_ForwardClose_Request->Connection_Triad))
    {
        rc = cip_obj_cnxnmgr_explicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose_Class3(p_cip_ctrl, p_ForwardClose_Request, p_MessageRouterResponse);
        CIP_LOG_EXIT_AND_RETURN(rc);
    }
    else
    {
        CIP_LOG_EXIT_AND_RETURN(cip_obj_cnxnmgr_build_forwardclose_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_TARGET_CONNECTION_NOT_FOUND));
    }
}

/* *********** */
/* END OF FILE */
/* *********** */
