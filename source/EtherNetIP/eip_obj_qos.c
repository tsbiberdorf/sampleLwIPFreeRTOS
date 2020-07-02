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

#if EIP_USER_OPTION_QOS_ENABLED

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "eip_obj_qos.c"

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

static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* TODO: move to NVRAM */
CIP_QoSObject NVRAM_QoSObject;
CIP_QoSObject QoSObject;

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */
#define EIP_Class_Code_QoS_Revision    1

/* Class Level Attributes */
#define EIP_QoSClass_Attribute_Revision        1

/* Instance Level Attributes */
#define EIP_QoSInstance_Attribute_DSCP_Urgent       4
#define EIP_QoSInstance_Attribute_DSCP_Scheduled    5
#define EIP_QoSInstance_Attribute_DSCP_High         6
#define EIP_QoSInstance_Attribute_DSCP_Low          7
#define EIP_QoSInstance_Attribute_DSCP_Explicit     8

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode eip_obj_qos_init (CIP_InitType const init_type)
{
    static CIP_BOOL first_time = 1;
    CIP_InitType modified_init_type = init_type;

   CIP_LOG_FUNCTION_ENTER;

    if(first_time)
    {
        first_time = 0;
        modified_init_type = CIP_INITTYPE_MFR;
    }

    switch(modified_init_type)
    {
        case CIP_INITTYPE_NORMAL:
            break;

        case CIP_INITTYPE_OUTOFBOX:
        case CIP_INITTYPE_OUTOFBOX_NOCOMMS:
        case CIP_INITTYPE_MFR:
        default:
            NVRAM_QoSObject.DSCP_Urgent = 55;
            NVRAM_QoSObject.DSCP_Scheduled = 47;
            NVRAM_QoSObject.DSCP_High = 43;
            NVRAM_QoSObject.DSCP_Low = 31;
            NVRAM_QoSObject.DSCP_Explicit = 27;
            break;
    }

    memcpy(&QoSObject, &NVRAM_QoSObject, sizeof(QoSObject));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_obj_qos_process (RTA_U32 const ms_since_last_call)
{
    RTA_UNUSED_PARAM(ms_since_last_call);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_obj_qos_get_CIP_QoSObject (CIP_ControlStruct const * const p_cip_ctrl, CIP_QoSObject * const p_QoSObject)
{
	RTA_UNUSED_PARAM(p_cip_ctrl);

    CIP_LOG_FUNCTION_ENTER;

    if(!p_QoSObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    memcpy(p_QoSObject, &QoSObject, sizeof(*p_QoSObject));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_obj_qos_decode (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    RTA_UNUSED_PARAM(expected_length); /* only used if indeterminent length data is passed */

    if(!p_cip_ctrl || !p_MessageRouterRequest)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    switch(p_MessageRouterRequest->Service)
    {
        case CIP_Service_Set_Attribute_Single:
            if(!p_MessageRouterRequest->CIAM_Path.class_id_found)
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED); /* this is an OK exit, so don't return as ERROR */
            if(!p_MessageRouterRequest->CIAM_Path.instance_id_found)
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED); /* this is an OK exit, so don't return as ERROR */
            if(!p_MessageRouterRequest->CIAM_Path.attribute_id_found)
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED); /* this is an OK exit, so don't return as ERROR */
            if(p_MessageRouterRequest->CIAM_Path.class_id != CIP_Class_Code_QoS)
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED); /* this is an OK exit, so don't return as ERROR */
            if(p_MessageRouterRequest->CIAM_Path.instance_id == CIP_InstanceID_Class_Level) /* we only want instances */
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED); /* this is an OK exit, so don't return as ERROR */

            switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
            {
               case EIP_QoSInstance_Attribute_DSCP_Urgent:
               case EIP_QoSInstance_Attribute_DSCP_Scheduled:
               case EIP_QoSInstance_Attribute_DSCP_High:
               case EIP_QoSInstance_Attribute_DSCP_Low:
               case EIP_QoSInstance_Attribute_DSCP_Explicit:
                   rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_USINT(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                   if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
                   break;

                default:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_Opaque_String(p_cip_ctrl, expected_length, &p_MessageRouterRequest->Request_Data);
                    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
                    break;
            }
            break;

        default:
            rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_Opaque_String(p_cip_ctrl, expected_length, &p_MessageRouterRequest->Request_Data);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL eip_obj_qos_is_instance_supported (CIP_CIAM_Path const * const p_path)
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
/*lint -efunc(818, eip_obj_qos_process_service) */ /* 818 - Pointer parameter 'p_eip_ctrl' could be declared ptr to const */
CIP_ReturnCode eip_obj_qos_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
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

        case CIP_Service_Set_Attribute_Single:
            if(!p_MessageRouterRequest->CIAM_Path.attribute_id_found)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Path_destination_unknown));

            rc = local_process_CIP_Service_Set_Attribute_Single (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            CIP_LOG_ERROR_AND_RETURN (CIP_RETURNCODE_ERROR_INTERNAL);
    }    

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL eip_obj_qos_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance)
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
            case CIP_Service_Get_Attribute_Single: 
                return(RTA_TRUE);
            case CIP_Service_Set_Attribute_Single: 
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
    CIP_UINT const revision = EIP_Class_Code_QoS_Revision;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case EIP_QoSClass_Attribute_Revision:
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

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case EIP_QoSInstance_Attribute_DSCP_Urgent:
            rc = cip_process_write_response_data_with_CIP_USINT(p_MessageRouterResponse, &QoSObject.DSCP_Urgent);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case EIP_QoSInstance_Attribute_DSCP_Scheduled:
            rc = cip_process_write_response_data_with_CIP_USINT(p_MessageRouterResponse, &QoSObject.DSCP_Scheduled);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case EIP_QoSInstance_Attribute_DSCP_High:
            rc = cip_process_write_response_data_with_CIP_USINT(p_MessageRouterResponse, &QoSObject.DSCP_High);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case EIP_QoSInstance_Attribute_DSCP_Low:
            rc = cip_process_write_response_data_with_CIP_USINT(p_MessageRouterResponse, &QoSObject.DSCP_Low);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case EIP_QoSInstance_Attribute_DSCP_Explicit:
            rc = cip_process_write_response_data_with_CIP_USINT(p_MessageRouterResponse, &QoSObject.DSCP_Explicit);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }

   CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
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
            rc = local_process_CIP_Service_Set_Attribute_Single_InstanceLevel(p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));        
    }
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{    
    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case EIP_QoSInstance_Attribute_DSCP_Urgent:
            if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_USINT)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data)); /* type didn't match, not enough data */

            if(p_MessageRouterRequest->Request_Data.Value.value_USINT > 0x3F) /* 6 bits, so 0-63 */
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));   

            NVRAM_QoSObject.DSCP_Urgent = p_MessageRouterRequest->Request_Data.Value.value_USINT;
            break; 

        case EIP_QoSInstance_Attribute_DSCP_Scheduled:
            if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_USINT)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data)); /* type didn't match, not enough data */

            if(p_MessageRouterRequest->Request_Data.Value.value_USINT > 0x3F) /* 6 bits, so 0-63 */
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));   

            NVRAM_QoSObject.DSCP_Scheduled = p_MessageRouterRequest->Request_Data.Value.value_USINT;
            break; 

        case EIP_QoSInstance_Attribute_DSCP_High:
            if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_USINT)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data)); /* type didn't match, not enough data */

            if(p_MessageRouterRequest->Request_Data.Value.value_USINT > 0x3F) /* 6 bits, so 0-63 */
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));   

            NVRAM_QoSObject.DSCP_High = p_MessageRouterRequest->Request_Data.Value.value_USINT;
            break; 

        case EIP_QoSInstance_Attribute_DSCP_Low:
            if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_USINT)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data)); /* type didn't match, not enough data */

            if(p_MessageRouterRequest->Request_Data.Value.value_USINT > 0x3F) /* 6 bits, so 0-63 */
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));   

            NVRAM_QoSObject.DSCP_Low = p_MessageRouterRequest->Request_Data.Value.value_USINT;
            break; 

        case EIP_QoSInstance_Attribute_DSCP_Explicit:
            if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_USINT)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data)); /* type didn't match, not enough data */

            if(p_MessageRouterRequest->Request_Data.Value.value_USINT > 0x3F) /* 6 bits, so 0-63 */
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));   

            NVRAM_QoSObject.DSCP_Explicit = p_MessageRouterRequest->Request_Data.Value.value_USINT;
            break; 

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }

   CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

#endif /* #if EIP_USER_OPTION_QOS_ENABLED */

/* *********** */
/* END OF FILE */
/* *********** */
