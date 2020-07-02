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
#include "NGRMRelay.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "eip_userobj_ethernet.c"

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

static CIP_ReturnCode local_process_CIP_Service_Get_and_Clear (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Get_and_Clear_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */
#define CIP_Class_Code_EthernetLink_Revision    4

/* Class Level Attributes */
#define CIP_EthernetLinkClass_Attribute_Revision        1

/* Instance Level Attributes */
#define CIP_EthernetLinkInstance_Attribute_InterfaceSpeed       1
#define CIP_EthernetLinkInstance_Attribute_InterfaceFlags       2
#define CIP_EthernetLinkInstance_Attribute_PhysicalAddress      3
#define CIP_EthernetLinkInstance_Attribute_InterfaceCounters    4
#define CIP_EthernetLinkInstance_Attribute_MediaCounters        5
#define CIP_EthernetLinkInstance_Attribute_InterfaceCapability  11

#define CIP_Service_Get_and_Clear   0x4C

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode eip_userobj_ethernet_init (CIP_InitType const init_type)
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

CIP_ReturnCode eip_userobj_ethernet_process (RTA_U32 const ms_since_last_call)
{
    RTA_UNUSED_PARAM(ms_since_last_call);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL eip_userobj_ethernet_is_instance_supported (CIP_CIAM_Path const * const p_path)
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
/*lint -efunc(818, eip_userobj_ethernet_process_service) */ /* 818 - Pointer parameter 'p_eip_ctrl' could be declared ptr to const */
CIP_ReturnCode eip_userobj_ethernet_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
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

        case CIP_Service_Get_and_Clear:
            rc = local_process_CIP_Service_Get_and_Clear (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            CIP_LOG_ERROR_AND_RETURN (CIP_RETURNCODE_ERROR_INTERNAL);
    }    

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL eip_userobj_ethernet_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance)
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
            case CIP_Service_Get_and_Clear:
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
    CIP_UINT const revision = CIP_Class_Code_EthernetLink_Revision;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_EthernetLinkClass_Attribute_Revision:
            rc = cip_process_write_response_data_with_CIP_UINT(p_MessageRouterResponse, &revision);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

extern unsigned long PHY_GetLinkSpeed(); /** GitHub Issue #83 */
extern unsigned int PHY_GetLinkDuplex(); /** Github issue #110 */
extern unsigned char* getMacAddr();

static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
//	unsigned char *mac = (unsigned char*)MAC_ADDR_LOCATION; @todo reinclude once local mac is stored
    unsigned char *mac = (unsigned char*) getMacAddr();

    CIP_UDINT InterfaceSpeed;
    EIP_EthernetLinkObject_InterfaceFlags_Internal  InterfaceFlags;
    EIP_EthernetLinkObject_PhysicalAddress PhysicalAddress;
    EIP_EthernetLinkObject_InterfaceCapability InterfaceCapability;
    EIP_EthernetLinkObject_InterfaceCounters InterfaceCounters;
    EIP_EthernetLinkObject_MediaCounters MediaCounters;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_EthernetLinkInstance_Attribute_InterfaceSpeed:
            InterfaceSpeed = (CIP_UDINT)PHY_GetLinkSpeed();

            rc = cip_process_write_response_data_with_CIP_UDINT(p_MessageRouterResponse, &InterfaceSpeed);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_EthernetLinkInstance_Attribute_InterfaceFlags:
            InterfaceFlags.Link_Status_Active = RTA_TRUE;
            if(PHY_GetLinkDuplex())
            {
            	/*
            	 * is non zero full duplex
            	 */
            	InterfaceFlags.Link_Full_Duplex = RTA_TRUE; /* indeterminate */
            }
            else
            {
            	InterfaceFlags.Link_Full_Duplex = RTA_FALSE; /* indeterminate */
            }
            InterfaceFlags.Negotiation_Status = NegotiationStatus_AutoNegotiate_successful;
            InterfaceFlags.ManualSettingRequiresReset = RTA_FALSE; /* manual setting not supported */
            InterfaceFlags.Local_Hardware_Fault_Detected = RTA_FALSE;

            rc = cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceFlags_Internal(p_MessageRouterResponse, &InterfaceFlags);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_EthernetLinkInstance_Attribute_PhysicalAddress:
            /* TODO: Make programatic */
            PhysicalAddress.PhysicalAddress[0] = mac[0];
            PhysicalAddress.PhysicalAddress[1] = mac[1];
            PhysicalAddress.PhysicalAddress[2] = mac[2];
            PhysicalAddress.PhysicalAddress[3] = mac[3];
            PhysicalAddress.PhysicalAddress[4] = mac[4];
            PhysicalAddress.PhysicalAddress[5] = mac[5];

            rc = cip_process_write_response_data_with_EIP_EthernetLinkObject_PhysicalAddress(p_MessageRouterResponse, &PhysicalAddress);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_EthernetLinkInstance_Attribute_InterfaceCounters:
            /* TODO: Make programatic */
            memset(&InterfaceCounters, 0, sizeof(InterfaceCounters));
            rc = cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceCounters(p_MessageRouterResponse, &InterfaceCounters);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_EthernetLinkInstance_Attribute_MediaCounters:
            /* TODO: Make programatic */
            memset(&MediaCounters, 0, sizeof(MediaCounters));
            rc = cip_process_write_response_data_with_EIP_EthernetLinkObject_MediaCounters(p_MessageRouterResponse, &MediaCounters);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_EthernetLinkInstance_Attribute_InterfaceCapability:
            InterfaceCapability.InterfaceCapability_Bits.ManualSettingRequiresReset = RTA_FALSE;
            InterfaceCapability.InterfaceCapability_Bits.AutoNegotiate_Supported = RTA_TRUE;
            InterfaceCapability.InterfaceCapability_Bits.AutoMDIX_Supported = RTA_TRUE;
            InterfaceCapability.InterfaceCapability_Bits.ManualSettings_Supported = RTA_FALSE;
            InterfaceCapability.SpeedDuplex_Array.count = 0; /* we don't support any manual settings, so we don't advertise any valid combinations */

            rc = cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceCapability(p_MessageRouterResponse, &InterfaceCapability);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }

   CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Get_and_Clear (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
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
            rc = local_process_CIP_Service_Get_and_Clear_InstanceLevel(p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));        
    }
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Get_and_Clear_InstanceLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;

    EIP_EthernetLinkObject_InterfaceCounters InterfaceCounters;
    EIP_EthernetLinkObject_MediaCounters MediaCounters;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_EthernetLinkInstance_Attribute_InterfaceSpeed:
        case CIP_EthernetLinkInstance_Attribute_InterfaceFlags:
        case CIP_EthernetLinkInstance_Attribute_PhysicalAddress:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Service_Not_Supported_for_Path_1));        

        case CIP_EthernetLinkInstance_Attribute_InterfaceCounters:
            /* TODO: Make programatic */
            memset(&InterfaceCounters, 0, sizeof(InterfaceCounters));
            rc = cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceCounters(p_MessageRouterResponse, &InterfaceCounters);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

            /* get then clear counters */
            memset(&InterfaceCounters, 0, sizeof(InterfaceCounters));
            break; 

        case CIP_EthernetLinkInstance_Attribute_MediaCounters:
            /* TODO: Make programatic */
            memset(&MediaCounters, 0, sizeof(MediaCounters));
            rc = cip_process_write_response_data_with_EIP_EthernetLinkObject_MediaCounters(p_MessageRouterResponse, &MediaCounters);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

            /* get then clear counters */
            memset(&MediaCounters, 0, sizeof(MediaCounters));
            break; 

        case CIP_EthernetLinkInstance_Attribute_InterfaceCapability:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Service_Not_Supported_for_Path_1));        

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }

   CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* *********** */
/* END OF FILE */
/* *********** */
