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
#include "eip_revision.h"
#include "NGRMRelay.h"
#include "source/Memory/CommonComm.h"
#include "networkTasks/EtherNetSupport.h"
//#include "utilities/EtherNetSupport.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "cip_userobj_identity.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */
RTA_Usersock_ReturnCode call_on_send_complete_reset_ethernetip_normal (RTA_Usersock_Socket const * const user_sock);

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

static CIP_ReturnCode local_process_CIP_Service_Reset (CIP_ControlStruct * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Reset_InstanceLevel (CIP_ControlStruct * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */
#define CIP_Class_Code_Identity_Revision    1

/* Class Level Attributes */
#define CIP_IdentityClass_Attribute_Revision        1

/* Instance Level Attributes */
#define CIP_IdentityInstance_Attribute_VendorID     1
#define CIP_IdentityInstance_Attribute_DeviceType   2
#define CIP_IdentityInstance_Attribute_ProductCode  3
#define CIP_IdentityInstance_Attribute_Revision     4
#define CIP_IdentityInstance_Attribute_Status       5
#define CIP_IdentityInstance_Attribute_SerialNumber 6
#define CIP_IdentityInstance_Attribute_ProductName  7

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode cip_userobj_identity_init (CIP_InitType const init_type)
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

CIP_ReturnCode cip_userobj_identity_process (RTA_U32 const ms_since_last_call)
{
    RTA_UNUSED_PARAM(ms_since_last_call);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_userobj_identity_is_instance_supported (CIP_CIAM_Path const * const p_path)
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

CIP_ReturnCode cip_userobj_identity_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct * p_cip_ctrl;

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

        case CIP_Service_Reset:
            rc = local_process_CIP_Service_Reset (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            CIP_LOG_ERROR_AND_RETURN (CIP_RETURNCODE_ERROR_INTERNAL);
    }    

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}


CIP_BOOL cip_userobj_identity_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance)
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
            case CIP_Service_Reset:
            case CIP_Service_Get_Attribute_Single: 
                return(RTA_TRUE);
            default:    
                return(RTA_FALSE);
        }
    }
}

extern unsigned char* getMacAddr();
CIP_ReturnCode cip_userobj_identity_get_CIP_IdentityObject (CIP_ControlStruct const * const p_cip_ctrl, CIP_IdentityObject * const p_IdentityObject)
{
    CIP_LOG_FUNCTION_ENTER;
//	unsigned char *mac = (unsigned char*)MAC_ADDR_LOCATION;
    unsigned char *mac = (unsigned char*) getMacAddr();

	union _SerialNumber
	{
		uint32_t u32Value;
		char cArray[4];
	}serialNumber;

	serialNumber.cArray[0] = mac[5];
	serialNumber.cArray[1] = mac[4];
	serialNumber.cArray[2] = mac[3];
	serialNumber.cArray[3] = mac[2];

    if(!p_cip_ctrl || !p_IdentityObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* user provided fields, so magic numbers are OK */
    p_IdentityObject->VendorID = 691;
    p_IdentityObject->DeviceType = 0x03;    /* Motor Overload Device */
    p_IdentityObject->Revision.Major = EIP_REVISION_MAJOR;
    p_IdentityObject->Revision.Minor = EIP_REVISION_MINOR;
    p_IdentityObject->Status = GetIdentityStatus();
    p_IdentityObject->SerialNumber = serialNumber.u32Value;
    p_IdentityObject->State = 255;  /* default if not implemented */
    p_IdentityObject->ProductCode = 8003;

//    MemMapRead(CFG_MEMMAP_PC_ADDR, CFG_MEMMAP_PC_SIZE, &p_IdentityObject->ProductCode);

    /* specification is RTA_U8... string lib is char, so casts are OK... we need to maintain spec types */
    rta_snprintf((char *)p_IdentityObject->ProductName.Data, sizeof(p_IdentityObject->ProductName.Data), "MP8000");
    p_IdentityObject->ProductName.Length = (CIP_USINT)strlen((char *)p_IdentityObject->ProductName.Data);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);    
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
    CIP_UINT const revision = CIP_Class_Code_Identity_Revision;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_IdentityClass_Attribute_Revision:
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
    CIP_IdentityObject IdentityObject;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    rc = cip_userobj_identity_get_CIP_IdentityObject(p_cip_ctrl, &IdentityObject);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_IdentityInstance_Attribute_VendorID:
            rc = cip_process_write_response_data_with_CIP_UINT(p_MessageRouterResponse, &IdentityObject.VendorID);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case CIP_IdentityInstance_Attribute_DeviceType:
            rc = cip_process_write_response_data_with_CIP_UINT(p_MessageRouterResponse, &IdentityObject.DeviceType);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case CIP_IdentityInstance_Attribute_ProductCode:
            rc = cip_process_write_response_data_with_CIP_UINT(p_MessageRouterResponse, &IdentityObject.ProductCode);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case CIP_IdentityInstance_Attribute_Revision:
            rc = cip_process_write_response_data_with_CIP_IdentityObject_Revision(p_MessageRouterResponse, &IdentityObject.Revision);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case CIP_IdentityInstance_Attribute_Status:
            rc = cip_process_write_response_data_with_CIP_WORD(p_MessageRouterResponse, &IdentityObject.Status);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case CIP_IdentityInstance_Attribute_SerialNumber:
            rc = cip_process_write_response_data_with_CIP_UDINT(p_MessageRouterResponse, &IdentityObject.SerialNumber);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 
        case CIP_IdentityInstance_Attribute_ProductName:
            rc = cip_process_write_response_data_with_CIP_SHORT_STRING_32(p_MessageRouterResponse, &IdentityObject.ProductName);
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

static CIP_ReturnCode local_process_CIP_Service_Get_Attributes_All_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_IdentityObject IdentityObject;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    rc = cip_userobj_identity_get_CIP_IdentityObject(p_cip_ctrl, &IdentityObject);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_process_write_response_data_with_CIP_IdentityObject(p_MessageRouterResponse, &IdentityObject);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Reset (CIP_ControlStruct * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
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
            rc = local_process_CIP_Service_Reset_InstanceLevel(p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));        
    }
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Reset_InstanceLevel (CIP_ControlStruct * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_Opaque_String const * p_ParameterData;
    CIP_USINT ResetType;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    /* CIP Volume 1 - Table 5A-2.8 Reset Service Request Parameters */
    
    /* if parameter is omitted, we default to a normal reset */
    if(p_MessageRouterRequest->Request_Data.Type == CIP_DATA_TYPE_NO_DATA)
    {
        p_cip_ctrl->p_fptr_call_on_send_complete = call_on_send_complete_reset_ethernetip_normal;
    }

    /* CIP Volume 1 - Table 5A-2.9 Reset Type Parameter Values */
    else if (p_MessageRouterRequest->Request_Data.Type == CIP_DATA_TYPE_CIP_Opaque_String)
    {
        p_ParameterData = &p_MessageRouterRequest->Request_Data.Value.value_Opaque_String;

        if(p_ParameterData->Length > 1)     
           return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Too_much_data));        

        ResetType = p_ParameterData->pData[0];

        /* we only support a Power Cycle (normal reset) */
        if(ResetType != 0) 
           return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_parameter));        

        p_cip_ctrl->p_fptr_call_on_send_complete = call_on_send_complete_reset_ethernetip_normal;
    }
    else
    {
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_parameter));        
    }


    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* *********** */
/* END OF FILE */
/* *********** */
