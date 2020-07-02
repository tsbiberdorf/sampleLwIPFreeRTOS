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
#include "cip_system.h"

#if EIP_USER_OPTION_DLR_ENABLED

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "eip_obj_dlr.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_ClassLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);

static CIP_ReturnCode local_process_CIP_Service_Get_Attributes_All (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Get_Attributes_All_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);

static CIP_ReturnCode local_get_CIP_DLRObject (CIP_ControlStruct const * const p_cip_ctrl, CIP_DLRObject * const p_DLRObject);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */
#define EIP_Class_Code_DLR_Revision    3

/* Class Level Attributes */
#define EIP_DLRClass_Attribute_Revision        1

/* Instance Level Attributes */
#define EIP_DLRInstance_Attribute_NetworkTopology           1
#define EIP_DLRInstance_Attribute_NetworkStatus             2
#define EIP_DLRInstance_Attribute_ActiveSupervisorAddress   10
#define EIP_DLRInstance_Attribute_CapabilitiyFlags          12

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode eip_obj_dlr_init (CIP_InitType const init_type)
{
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

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_obj_dlr_process (RTA_U32 const ms_since_last_call)
{
    RTA_UNUSED_PARAM(ms_since_last_call);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL eip_obj_dlr_is_instance_supported (CIP_CIAM_Path const * const p_path)
{
    if(!p_path)
        return(RTA_FALSE);

    if(!p_path->instance_id_found)
        return(RTA_FALSE);

    if(p_path->instance_id == CIP_InstanceID_Class_Level)
        return(RTA_TRUE);   
    if(p_path->instance_id == 1) /* instance level */
        return(RTA_TRUE);
    return(RTA_FALSE);
}

/* Since this is called from a function pointer, we can't restrict p_eip_ctrl to be 'const * const' as some functions need to modify p_eip_ctrl's contents */
/*lint -efunc(818, eip_obj_dlr_process_service) */ /* 818 - Pointer parameter 'p_eip_ctrl' could be declared ptr to const */
CIP_ReturnCode eip_obj_dlr_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = & p_eip_ctrl->cip_ctrl;

    /* path was pre-validated for class_id and instance_id, so feel free to use it */

    switch(p_MessageRouterRequest->Service)
    {
        case CIP_Service_Get_Attribute_Single:
            if(!p_MessageRouterRequest->CIAM_Path.attribute_id_found)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Path_destination_unknown));

            rc = local_process_CIP_Service_Get_Attribute_Single (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_Service_Get_Attributes_All:
            rc = local_process_CIP_Service_Get_Attributes_All (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            CIP_LOG_ERROR_AND_RETURN (CIP_RETURNCODE_ERROR_INTERNAL);
    }    

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}


CIP_BOOL eip_obj_dlr_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance)
{
    if(instance == CIP_InstanceID_Class_Level)
    {
        switch(service)
        {
            case CIP_Service_Get_Attribute_Single: 
                return(RTA_TRUE);
            default:    
                return(RTA_FALSE);
        }
    }

    else /* instance level */
    {
        switch(service)
        {
            case CIP_Service_Get_Attributes_All:
            case CIP_Service_Get_Attribute_Single: 
                return(RTA_TRUE);
            default:    
                return(RTA_FALSE);
        }
    }
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.instance_id)
    {
        case CIP_InstanceID_Class_Level:
            rc = local_process_CIP_Service_Get_Attribute_Single_ClassLevel(p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case 1:
            rc = local_process_CIP_Service_Get_Attribute_Single_InstanceLevel(p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));        
    }
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_ClassLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_UINT const revision = EIP_Class_Code_DLR_Revision;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case EIP_DLRClass_Attribute_Revision:
            rc = cip_process_write_response_data_with_CIP_UINT(p_MessageRouterResponse, &revision);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_DLRObject DLRObject;
    CIP_Opaque_String Opaque_String;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    rc = local_get_CIP_DLRObject(p_cip_ctrl, &DLRObject);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case EIP_DLRInstance_Attribute_NetworkTopology:
            rc = cip_process_write_response_data_with_CIP_USINT(p_MessageRouterResponse, &DLRObject.NetworkTopology);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case EIP_DLRInstance_Attribute_NetworkStatus:
            rc = cip_process_write_response_data_with_CIP_USINT(p_MessageRouterResponse, &DLRObject.NetworkStatus);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case EIP_DLRInstance_Attribute_ActiveSupervisorAddress:
            Opaque_String.Length = 10; /* dummy data - real data would be 4 bytes for IP and 6 bytes for MAC */
            Opaque_String.pData = cip_binary_malloc(p_cip_ctrl, Opaque_String.Length);
            if(!Opaque_String.pData)
                CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    
            /* our heap is zero-filled so we don't need to modify the dummy data from cip_binary_malloc */

            rc = cip_process_write_response_data_with_CIP_Opaque_String(p_cip_ctrl, p_MessageRouterResponse, &Opaque_String);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case EIP_DLRInstance_Attribute_CapabilitiyFlags:
            rc = cip_process_write_response_data_with_CIP_DWORD(p_MessageRouterResponse, &DLRObject.CapabilityFlags);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }

   CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Get_Attributes_All (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.instance_id)
    {
        case CIP_InstanceID_Class_Level:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Service_not_supported));        

        case 1:
            rc = local_process_CIP_Service_Get_Attributes_All_InstanceLevel(p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));        
    }
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* TODO: Create an encode routine for the incomplete CIP_DLRObject data type. That way it can fill in the unused and default values rather than a hack here. */

/* From CIP Volume 2, Section 5-6.5 "Get_Attributes_All Resposne"
    The Get_Attributes_All response (see Table 5-6.11) shall contain the instance attributes 1 through 8 and 10 through 16, 
    up to the last implemented attribute. Any unimplemented attributes in the response shall use the default value specified. 
        *** We support up through attribute 12.
*/
static CIP_ReturnCode local_process_CIP_Service_Get_Attributes_All_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_DLRObject DLRObject;
    CIP_Opaque_String Opaque_String;
    RTA_U8 *p_data;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    rc = local_get_CIP_DLRObject(p_cip_ctrl, &DLRObject);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    /* Per the spec: The response length for an Object Revision 3, non-supervisor, non-gateway device is 54 bytes. */
    Opaque_String.Length = 54;
    Opaque_String.pData = cip_binary_malloc(p_cip_ctrl, Opaque_String.Length);
    if(!Opaque_String.pData)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
        
    p_data = Opaque_String.pData;

    /* Attributes 1 - 3 */
    *p_data++ = DLRObject.NetworkTopology;
    *p_data++ = DLRObject.NetworkStatus;
    *p_data++ = 0xFF; /* DEFAULT: Ring Supervisor Status */

    /* We are not a supervisor, so send defaults (all zeros) */
    /* Attribute 4 - Ring Supervisor Config (structure of 12 bytes) */
    /* Attribute 5 - Ring Faults Count (2 bytes) */
    /* Attribute 6 - Last Active Node on Port 1 (IP/MAC) (10 bytes) */
    /* Attribute 7 - Last Active Node on Port 2 (IP/MAC) (10 bytes) */
    p_data += 34; /* unused data */

    rta_endian_PutLitEndian16(0xFFFF, p_data); /* Attribute 8 - Ring Participant Count */
    p_data +=2;

    /* Attribute 9 is NOT included per the specification */

    rta_endian_PutLitEndian32(DLRObject.ActiveSupervisorAddress.IPAddress, p_data); /* Attribute 10 part 1 - Supervisor IP */
    p_data +=4;
    
    memcpy(p_data, DLRObject.ActiveSupervisorAddress.MACAddress, 6); /* Attribute 10 part 2 - Supervisor MAC address */
    p_data +=6;

    *p_data++ = 0x00; /* Attribute 11 - DEFAULT: Active Supervisor Precedence */

    rta_endian_PutLitEndian32(DLRObject.CapabilityFlags, p_data); /* Attribute 12 - Capability Flags */

    rc = cip_process_write_response_data_with_CIP_Opaque_String(p_cip_ctrl, p_MessageRouterResponse, &Opaque_String);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_get_CIP_DLRObject (CIP_ControlStruct const * const p_cip_ctrl, CIP_DLRObject * const p_DLRObject)
{
    CIP_LOG_FUNCTION_ENTER;

    if(!p_cip_ctrl || !p_DLRObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* user provided fields, so magic numbers are OK */
    p_DLRObject->NetworkTopology = 0;
    p_DLRObject->NetworkStatus = 0;
    p_DLRObject->ActiveSupervisorAddress.IPAddress = 0;
    memset(p_DLRObject->ActiveSupervisorAddress.MACAddress, 0, sizeof(p_DLRObject->ActiveSupervisorAddress.MACAddress));
    p_DLRObject->CapabilityFlags = 0x82;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);    
}

#endif /* #if EIP_USER_OPTION_DLR_ENABLED */

/* *********** */
/* END OF FILE */
/* *********** */
