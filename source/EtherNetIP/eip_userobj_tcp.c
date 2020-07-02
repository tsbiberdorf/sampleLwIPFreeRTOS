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
#include "eip_system.h"
#include "source/Memory/CommonComm.h"

//#include <winsock2.h>
//#include <iphlpapi.h>
//#include <stdio.h>
//#include <stdlib.h>
//#pragma comment(lib, "IPHLPAPI.lib")

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "cip_userobj_tcp.c"

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

//static RTA_U32 local_parse_ip_address (char const * const s);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* TODO: look into a NVRAM solution (file read/write?) */
/* by making this static, simulate NVRAM by allowing a software reset to not reset */
static CIP_UINT EncapsulationInactivityTimeout_in_seconds = 120;
static CIP_OCTET local_host_name[65] = { 0 }; /* max 64 characters */

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */
#define CIP_Class_Code_TCP_Revision 4

/* Class Level Attributes */
#define CIP_TCPClass_Attribute_Revision 1

/* Instance Level Attributes */
#define CIP_TCPInstance_Attribute_Status                            1
#define CIP_TCPInstance_Attribute_ConfigurationCapability           2
#define CIP_TCPInstance_Attribute_ConfigurationControl              3
#define CIP_TCPInstance_Attribute_PhysicalLinkObject                4
#define CIP_TCPInstance_Attribute_InterfaceConfiguration            5
#define CIP_TCPInstance_Attribute_HostName                          6
#define CIP_TCPInstance_Attribute_EncapsulationInactivityTimeout    13

#define LOCAL_TCPIP_INTERFACE_CONFIGURATION_SETTABLE  0   /* define to 1 if settable, else 0 */

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode eip_userobj_tcp_init (CIP_InitType const init_type)
{
    CIP_LOG_FUNCTION_ENTER;

    switch(init_type)
    {
        case CIP_INITTYPE_NORMAL:
        case CIP_INITTYPE_OUTOFBOX:
        case CIP_INITTYPE_OUTOFBOX_NOCOMMS:
        case CIP_INITTYPE_MFR:
        default:
        	EncapsulationInactivityTimeout_in_seconds = 120;
//        	MemMapRead(CFG_MEMMAP_EIPTO_ADDR,CFG_MEMMAP_EIPTO_SIZE,&EncapsulationInactivityTimeout_in_seconds); @todo if needed re include this?
            break;
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_userobj_tcp_process (RTA_U32 const ms_since_last_call)
{
    RTA_UNUSED_PARAM(ms_since_last_call);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL eip_userobj_tcp_is_instance_supported (CIP_CIAM_Path const * const p_path)
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
/*lint -efunc(818, eip_userobj_tcp_process_service) */ /* 818 - Pointer parameter 'p_eip_ctrl' could be declared ptr to const */
CIP_ReturnCode eip_userobj_tcp_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
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

CIP_BOOL eip_userobj_tcp_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance)
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
            case CIP_Service_Set_Attribute_Single: 
                return(RTA_TRUE);
            default:    
                return(RTA_FALSE);
        }
    }
}

CIP_ReturnCode eip_userobj_tcp_decode (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest)
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
            if(p_MessageRouterRequest->CIAM_Path.class_id != CIP_Class_Code_TCP_IP_Interface)
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED); /* this is an OK exit, so don't return as ERROR */
            if(p_MessageRouterRequest->CIAM_Path.instance_id == CIP_InstanceID_Class_Level) /* we only want instances */
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED); /* this is an OK exit, so don't return as ERROR */

            switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
            {
               case CIP_TCPInstance_Attribute_ConfigurationControl:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_EIP_TCPObject_ConfigurationControl(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
                    break;

               case CIP_TCPInstance_Attribute_InterfaceConfiguration:
                   rc = cip_decode_CIP_MessageRouter_Data_of_type_EIP_TCPObject_InterfaceConfiguration(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                   if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
                   break;

               case CIP_TCPInstance_Attribute_HostName:
                   rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_STRING(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                   if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
                   break;
               
                case CIP_TCPInstance_Attribute_EncapsulationInactivityTimeout:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_UINT(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
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


CIP_ReturnCode eip_userobj_tcp_get_EIP_TCPObject_InterfaceConfiguration (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_InterfaceConfiguration * const p_InterfaceConfiguration)
{
//    PIP_ADAPTER_INFO pAdapterInfo;
//    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);

    CIP_LOG_FUNCTION_ENTER;

    RTA_UNUSED_PARAM(p_cip_ctrl); /* single port solution, so this isn't needed */

    if(!p_InterfaceConfiguration)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    memset(p_InterfaceConfiguration, 0, sizeof(p_InterfaceConfiguration));

    extern uint32_t getCurrentIpv4Address(); // @note defined in IpAddrHandler.c
    extern uint32_t getCurrentIpv4Netmask(); // @note defined in IpAddrHandler.c
    extern uint32_t getCurrentIpv4Gateway(); // @note defined in IpAddrHandler.c
    extern uint32_t lwip_htonl(uint32_t n);

    p_InterfaceConfiguration->IP_Address = lwip_htonl( getCurrentIpv4Address());
    p_InterfaceConfiguration->Network_Mask = lwip_htonl( getCurrentIpv4Netmask());
    p_InterfaceConfiguration->Gateway_Address = lwip_htonl( getCurrentIpv4Gateway());

//    /* TODO: remove malloc/free from the Windows sample code */
//    pAdapterInfo = (IP_ADAPTER_INFO *) malloc (sizeof (IP_ADAPTER_INFO));
//    if (pAdapterInfo == NULL)
//        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
//
//    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
//    {
//        free(pAdapterInfo);
//        pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
//        if (pAdapterInfo == NULL)
//            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
//    }
//
//    /* TODO: change back to programatic; add in a check for which adapter has our IP */
//    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
//    {
//        /* TODO: Fix IP to search adapters for ours */
//        p_InterfaceConfiguration->IP_Address = 0xC0A80132;      /* local_parse_ip_address(pAdapterInfo->IpAddressList.IpAddress.String); */
//        p_InterfaceConfiguration->Network_Mask = 0xFFFFFF00;    /* local_parse_ip_address(pAdapterInfo->IpAddressList.IpMask.String); */
//        p_InterfaceConfiguration->Gateway_Address = 0;          /* local_parse_ip_address(pAdapterInfo->GatewayList.IpAddress.String); */
//    }
//    else
//    {
//        free(pAdapterInfo);
//        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
//    }
//
//    free(pAdapterInfo);

    /* user provided fields, so magic numbers are OK */
    p_InterfaceConfiguration->Name_Server = 0;
    p_InterfaceConfiguration->Name_Server2 = 0;
    p_InterfaceConfiguration->Domain_Name.Length = 0;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);    
}

CIP_ReturnCode eip_userobj_tcp_get_EIP_TCPObject_ConfigurationCapability (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_ConfigurationCapability * const p_ConfigurationCapability)
{
//    PIP_ADAPTER_INFO pAdapterInfo;
//    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
#define MP8000_DHCP_CAPABILITY (0x4) // always set bit 2 if MP8000 has the capability to support DHCP
    CIP_LOG_FUNCTION_ENTER;

    RTA_UNUSED_PARAM(p_cip_ctrl); /* single port solution, so this isn't needed */

    if(!p_ConfigurationCapability)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    memset(p_ConfigurationCapability, 0, sizeof(p_ConfigurationCapability));

    p_ConfigurationCapability->DHCP_Client = MP8000_DHCP_CAPABILITY;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_userobj_tcp_get_EIP_TCPObject_ConfigurationControl (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_ConfigurationControl * const p_ConfigurationControl)
{
//    PIP_ADAPTER_INFO pAdapterInfo;
//    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);

    CIP_LOG_FUNCTION_ENTER;

    RTA_UNUSED_PARAM(p_cip_ctrl); /* single port solution, so this isn't needed */

    if(!p_ConfigurationControl)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    memset(p_ConfigurationControl, 0, sizeof(p_ConfigurationControl));

    extern uint8_t DoWeHaveDHCPConnection();

    if( DoWeHaveDHCPConnection() )
    {
    	p_ConfigurationControl->ConfigurationMethod = ConfigurationControl_ConfigurationMethod_DHCP;
    }
    else
    {
    	p_ConfigurationControl->ConfigurationMethod = ConfigurationControl_ConfigurationMethod_Static;
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}


CIP_UINT eip_userobj_tcp_get_EncapsulationInactivityTimeout_in_seconds (void)
{
    return(EncapsulationInactivityTimeout_in_seconds);
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */
CIP_ReturnCode local_tcp_get_EIP_TCPObject (EIP_TCPObject * const p_TCPObject)
{
    CIP_ReturnCode rc;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_TCPObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* most values aren't used, so set to 0; Only non-zero values are initialized */
    memset(p_TCPObject, 0, sizeof(*p_TCPObject));

    p_TCPObject->Status.InterfaceConfigurationStatus = InterfaceConfigurationStatus_BootpDhcpNvram;
    
#if LOCAL_TCPIP_INTERFACE_CONFIGURATION_SETTABLE
    p_TCPObject->ConfigurationCapability.ConfigurationSettable = RTA_TRUE;
    p_TCPObject->ConfigurationControl.ConfigurationMethod = ConfigurationControl_ConfigurationMethod_Static;
#endif

    /*  If the CIP port associated with the TCP/IP Interface Object has an Ethernet physical layer, 
        this attribute shall point to an instance of the Ethernet Link Object */
    p_TCPObject->PhysicalLinkObject.class_id = CIP_Class_Code_Ethernet_Link;
    p_TCPObject->PhysicalLinkObject.instance_id = 1;

    rc = eip_userobj_tcp_get_EIP_TCPObject_InterfaceConfiguration(RTA_NULL, &p_TCPObject->InterfaceConfiguration);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = eip_userobj_tcp_get_EIP_TCPObject_ConfigurationCapability (RTA_NULL, &p_TCPObject->ConfigurationCapability);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = eip_userobj_tcp_get_EIP_TCPObject_ConfigurationControl (RTA_NULL, &p_TCPObject->ConfigurationControl);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    p_TCPObject->HostName.Length = (CIP_UINT)strlen((const char *)local_host_name);
    p_TCPObject->HostName.pData = local_host_name;

    /* By default, TTL Value shall be 1 */
    p_TCPObject->TTLValue = 1;

    p_TCPObject->EncapsulationInactivityTimeout = EncapsulationInactivityTimeout_in_seconds;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);    
}

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
    CIP_UINT const revision = CIP_Class_Code_TCP_Revision;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_TCPClass_Attribute_Revision:
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
    EIP_TCPObject * p_TCPObject;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    p_TCPObject = cip_binary_malloc(p_cip_ctrl, sizeof(*p_TCPObject));
    if(!p_TCPObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    rc = local_tcp_get_EIP_TCPObject(p_TCPObject);
    if(rc != CIP_RETURNCODE_SUCCESS)
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_No_stored_attribute_data));        

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_TCPInstance_Attribute_Status:
            rc = cip_process_write_response_data_with_EIP_TCPObject_Status(p_MessageRouterResponse, &p_TCPObject->Status);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_TCPInstance_Attribute_ConfigurationCapability:
            rc = cip_process_write_response_data_with_EIP_TCPObject_ConfigurationCapability(p_MessageRouterResponse, &p_TCPObject->ConfigurationCapability);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_TCPInstance_Attribute_ConfigurationControl:
            rc = cip_process_write_response_data_with_EIP_TCPObject_ConfigurationControl(p_MessageRouterResponse, &p_TCPObject->ConfigurationControl);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_TCPInstance_Attribute_PhysicalLinkObject:
            rc = cip_process_write_response_data_with_EIP_TCPObject_PhysicalLinkObject(p_MessageRouterResponse, &p_TCPObject->PhysicalLinkObject);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_TCPInstance_Attribute_InterfaceConfiguration:
            rc = cip_process_write_response_data_with_EIP_TCPObject_InterfaceConfiguration(p_MessageRouterResponse, &p_TCPObject->InterfaceConfiguration);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_TCPInstance_Attribute_HostName:
            rc = cip_process_write_response_data_with_CIP_STRING64_16BIT_PADDED(p_cip_ctrl, p_MessageRouterResponse, &p_TCPObject->HostName);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        case CIP_TCPInstance_Attribute_EncapsulationInactivityTimeout:
            rc = cip_process_write_response_data_with_CIP_UINT(p_MessageRouterResponse, &p_TCPObject->EncapsulationInactivityTimeout);
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
    CIP_ReturnCode rc;
    EIP_TCPObject * p_TCPObject;
    EIP_TCPObject_ConfigurationControl const * p_ConfigurationControl;
#if LOCAL_TCPIP_INTERFACE_CONFIGURATION_SETTABLE
    EIP_TCPObject_InterfaceConfiguration const * p_InterfaceConfiguration;
    CIP_STRING const * p_HostName;
#endif /* #if LOCAL_TCPIP_INTERFACE_CONFIGURATION_SETTABLE */

    CIP_UINT const * p_InactivityWatchdogTimer;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    p_TCPObject = cip_binary_malloc(p_cip_ctrl, sizeof(*p_TCPObject));
    if(!p_TCPObject)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    rc = local_tcp_get_EIP_TCPObject(p_TCPObject);
    if(rc != CIP_RETURNCODE_SUCCESS)
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_No_stored_attribute_data));        

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_TCPInstance_Attribute_Status:
        case CIP_TCPInstance_Attribute_ConfigurationCapability:
        case CIP_TCPInstance_Attribute_PhysicalLinkObject:
#if !LOCAL_TCPIP_INTERFACE_CONFIGURATION_SETTABLE
        case CIP_TCPInstance_Attribute_InterfaceConfiguration:
        case CIP_TCPInstance_Attribute_HostName:
#endif /* #if !LOCAL_TCPIP_INTERFACE_CONFIGURATION_SETTABLE */
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_settable));

        case CIP_TCPInstance_Attribute_ConfigurationControl:
            if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_EIP_TCPObject_ConfigurationControl)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data)); /* type didn't match, not enough data */

            p_ConfigurationControl = &p_MessageRouterRequest->Request_Data.Value.value_TCPObject_ConfigurationControl;

            /* conformance requires this to be settable, but we only support our existing config so make sure the set values match our current config */
            if(p_TCPObject->ConfigurationControl.ConfigurationMethod != p_ConfigurationControl->ConfigurationMethod)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));        
            if(p_TCPObject->ConfigurationControl.DNS_Enable != p_ConfigurationControl->DNS_Enable)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));        

           CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

#if LOCAL_TCPIP_INTERFACE_CONFIGURATION_SETTABLE
        case CIP_TCPInstance_Attribute_InterfaceConfiguration:
            if (p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_EIP_TCPObject_InterfaceConfiguration)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data)); /* type didn't match, not enough data */

            p_InterfaceConfiguration = &p_MessageRouterRequest->Request_Data.Value.value_TCPObject_InterfaceConfiguration;

            /* check if we have an I/O connection open */
            if (cip_obj_cnxnmgr_implicit_connections_established())
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Device_state_conflict));

#if 0 
            // check if we are using DHCP (don't allow static setting) */
            if (dhcp_enabled)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Device_state_conflict));
#endif

            if( (p_InterfaceConfiguration->IP_Address == 0) ||                          /* all 0's */
                (p_InterfaceConfiguration->IP_Address == 0xFFFFFFFF) ||                 /* all 1's */
                ((p_InterfaceConfiguration->IP_Address & 0xFF000000) == 0x7F000000) ||  /* loopback */
                ((p_InterfaceConfiguration->IP_Address & 0x000000FF) == 0) ||           /* bottom octect 0 */
                ((p_InterfaceConfiguration->IP_Address & 0xFF000000) > 0xDF000000))     /* make sure we aren't class D */
            {
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));
            }

            if ((p_InterfaceConfiguration->Network_Mask == 0) ||        /* all 0's */
                (p_InterfaceConfiguration->Network_Mask == 0xFFFFFFFF)) /* all 1's */
            {
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));
            }

            if ((p_InterfaceConfiguration->Gateway_Address == 0xFFFFFFFF) ||                /* all 1's */
                ((p_InterfaceConfiguration->Gateway_Address & 0xFF000000) == 0x7F000000) || /* loopback */
                ((p_InterfaceConfiguration->Gateway_Address & 0xFF000000) > 0xDF000000))    /* make sure we aren't class D (or higher, which includes broadcast) */
            {
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));
            }

            /* validate gateway is on our subnet */
            if (p_InterfaceConfiguration->Gateway_Address && ((p_InterfaceConfiguration->Gateway_Address & p_InterfaceConfiguration->Network_Mask) !=
                (p_InterfaceConfiguration->IP_Address & p_InterfaceConfiguration->Network_Mask)))
            {
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));
            }

            if (((p_InterfaceConfiguration->Name_Server & 0xFF000000) > 0xDF000000) ||  /* make sure we aren't class D (or higher, which includes broadcast) */
                ((p_InterfaceConfiguration->Name_Server2 & 0xFF000000) > 0xDF000000))   /* make sure we aren't class D (or higher, which includes broadcast) */
            {
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));
            }

            /* validate the IP isn't the subnet broadcast */
            if (p_InterfaceConfiguration->IP_Address == (p_InterfaceConfiguration->IP_Address | ~p_InterfaceConfiguration->Network_Mask))
            {
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));
            }

            if (p_InterfaceConfiguration->Domain_Name.Length > 48)
            {
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));
            }

            /* HERE IS WHERE YOU KNOW YOU ARE VALID... NOW STORE THE IP */

            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

        /* CIP Volume 2 - 5-4.3.2.6 Host Name � Attribute */
        case CIP_TCPInstance_Attribute_HostName:
            if (p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_STRING)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data)); /* type didn't match, not enough data */

            p_HostName = &p_MessageRouterRequest->Request_Data.Value.value_STRING;

            /* check if we have an I/O connection open */
            if (cip_obj_cnxnmgr_implicit_connections_established())
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Device_state_conflict));

            if (p_HostName->Length > 64)
            {
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));
            }

            if (p_HostName->Length > sizeof(local_host_name))
            {
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));
            }

            /* HERE IS WHERE YOU KNOW YOU ARE VALID... NOW STORE THE HOST NAME */
            memset(local_host_name, 0, sizeof(local_host_name));
            memcpy(local_host_name, p_HostName->pData, p_HostName->Length);

            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
#endif /* #if LOCAL_TCPIP_INTERFACE_CONFIGURATION_SETTABLE */

        /* CIP Volume 2 - Table 5-4.3 Instance Attributes */
        case CIP_TCPInstance_Attribute_EncapsulationInactivityTimeout:    
            if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_UINT)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Message_Format_Error));        

            p_InactivityWatchdogTimer = &p_MessageRouterRequest->Request_Data.Value.value_UINT;

            /* 0 - 3600 */
            if(*p_InactivityWatchdogTimer > 3600)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Invalid_attribute_value));        

            EncapsulationInactivityTimeout_in_seconds = *p_InactivityWatchdogTimer;
        	MemMapWrite(CFG_MEMMAP_EIPTO_ADDR,CFG_MEMMAP_EIPTO_SIZE,&EncapsulationInactivityTimeout_in_seconds,1);

            eip_sockets_update_all_tcp_timeouts(EncapsulationInactivityTimeout_in_seconds);

            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }
}

/* TODO Use with "Get IP from the user program argument" and remove lint comment */
/*lint -esym(528, local_parse_ip_address) Symbol 'Symbol' (Location) not referenced */
//static RTA_U32 local_parse_ip_address (char const * const s)
//{
//    struct in_addr addr;
//    int b1=0, b2=0, b3=0, b4=0;
//    if (sscanf(s, "%d.%d.%d.%d", &b1, &b2, &b3, &b4) != 4)
//        return(0);
//
//    addr.S_un.S_un_b.s_b1 = (RTA_U8) b1;
//    addr.S_un.S_un_b.s_b2 = (RTA_U8) b2;
//    addr.S_un.S_un_b.s_b3 = (RTA_U8) b3;
//    addr.S_un.S_un_b.s_b4 = (RTA_U8) b4;
//
//    return (ntohl(addr.S_un.S_addr));
//}

/* *********** */
/* END OF FILE */
/* *********** */
