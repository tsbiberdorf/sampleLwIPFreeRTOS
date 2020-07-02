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

/* TODO: reorder definitions to match grouping, or make separate file */
static void local_init_ImplicitConnection (CIP_ImplicitConnection * const p_ImplicitConnection);
static CIP_ImplicitConnection * local_find_free_implicit_connection (void);
static CIP_ReturnCode local_process_received_packet (CIP_ControlStruct const * const p_cip_ctrl, EIP_Item_Array_Implicit const * const p_Item_Array_Implicit);
static CIP_ReturnCode local_process_SendNeeded (CIP_ImplicitConnection * const p_ImplicitConnection);
static CIP_ReturnCode local_send_packet (EIP_ControlStruct * const p_eip_ctrl, CIP_ImplicitConnection * const p_ImplicitConnection);

static CIP_ReturnCode validate_and_store_ImplicitConnection_ConnectionPath(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_Path_Array const * const p_Path, CIP_ImplicitConnection * const p_CIP_ImplicitConnection);
static CIP_ReturnCode validate_ImplicitConnection_O2T_InternalNetworkConnectionParameters(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_CNXNMGR_Internal_NetworkConnectionParameters const * const p_Internal_NetworkConnectionParameters);
static CIP_ReturnCode validate_ImplicitConnection_T2O_InternalNetworkConnectionParameters(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_CNXNMGR_Internal_NetworkConnectionParameters const * const p_Internal_NetworkConnectionParameters);
static CIP_ReturnCode local_validate_O2T_instance(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const O2T_Instance);
static CIP_ReturnCode local_validate_O2T_connection_size(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const O2T_Instance, CIP_UINT const O2T_ExpectedSize);
static CIP_ReturnCode local_validate_T2O_instance(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const T2O_Instance);
static CIP_ReturnCode local_validate_T2O_connection_size(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const O2T_Instance, CIP_UINT const T2O_ExpectedSize);
static CIP_ReturnCode local_deepcopy_ImplicitConnection(CIP_ImplicitConnection * const p_dst_ImplicitConnection, CIP_ImplicitConnection const * const p_src_ImplicitConnection);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* TODO: Move this to a user file */
#define CIP_NUM_IMPLICIT_CONNECTIONS 1
static CIP_ImplicitConnection ImplicitConnection[CIP_NUM_IMPLICIT_CONNECTIONS];

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode cip_obj_cnxnmgr_implicit_init (CIP_InitType const init_type)
{
    RTA_Size_Type ii; 

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

    for(ii=0; ii<CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
        local_init_ImplicitConnection(&ImplicitConnection[ii]);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_obj_cnxnmgr_implicit_process (RTA_U32 const ms_since_last_call)
{
    CIP_ReturnCode rc;
    CIP_UDINT owning_ip;
    RTA_Size_Type ii;

    if (ms_since_last_call)
    {    
        for (ii = 0; ii < CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
        {
            if (!ImplicitConnection[ii].connection_is_used)
                continue;

            if (ms_since_last_call >= ImplicitConnection[ii].Connection_Timeout_remaining_ms)
            {    
                cip_userobj_assembly_timeout_received_for_o2t_instance(ImplicitConnection[ii].O2T.AssemblyInstance);

                owning_ip = ImplicitConnection[ii].OwningIPAddress;
                cip_obj_cnxnmgr_free_ImplicitConnection(&ImplicitConnection[ii]);
                cip_obj_cnxnmgr_timeout_common(owning_ip);
            }
            else
                ImplicitConnection[ii].Connection_Timeout_remaining_ms -= ms_since_last_call;

            /* if the connection timed out, we don't want to check for a send */
            if (!ImplicitConnection[ii].connection_is_used)
                continue;

            if (ms_since_last_call >= ImplicitConnection[ii].SendNeeded_Timeout_remaining_ms)
            {
                rc = local_process_SendNeeded(&ImplicitConnection[ii]);
                if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            }
            else
                ImplicitConnection[ii].SendNeeded_Timeout_remaining_ms -= ms_since_last_call;
        }
    }

    return(CIP_RETURNCODE_SUCCESS);
}

void cip_obj_cnxnmgr_free_ImplicitConnection(CIP_ImplicitConnection * const p_ImplicitConnection)
{
    RTA_Size_Type ii; 
    RTA_U32 ip_address;
    RTA_BOOL ip_used = RTA_FALSE;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ImplicitConnection)
        return;

    ip_address = p_ImplicitConnection->O2T.UDP_IPAddress;

    local_init_ImplicitConnection(p_ImplicitConnection);

    for(ii = 0; ii < CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
    {
        if(ImplicitConnection[ii].connection_is_used && ImplicitConnection[ii].O2T.UDP_IPAddress == ip_address)
        {
            ip_used = RTA_TRUE;
            break;
        }
    }

    if(ip_used) /* someone else is using the multicast, keep listening */
    {
        CIP_LOG_FUNCTION_EXIT;
        return;
    }

    CIP_LOG_FUNCTION_EXIT;
}

void cip_obj_cnxnmgr_ImplicitConnection_received(CIP_ImplicitConnection * const p_ImplicitConnection)
{
    if (!p_ImplicitConnection)
        return;
    
    p_ImplicitConnection->O2T.OneOrMorePacketsTransferred = RTA_TRUE;
    p_ImplicitConnection->Connection_Timeout_remaining_ms = p_ImplicitConnection->Connection_Timeout_reload_ms;
}

CIP_ReturnCode cip_obj_cnxnmgr_implicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen_Class1 (EIP_ControlStruct const * const p_eip_ctrl, CIP_ForwardOpen_Request const * const p_ForwardOpen_Request, CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct)
{
    CIP_ReturnCode rc;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_ImplicitConnection ImplicitConnection_Local;
    CIP_ImplicitConnection * p_ImplicitConnection_Local;
    CIP_ImplicitConnection * p_ImplicitConnection_New;
    CIP_ImplicitConnection const * p_MatchingImplicitConnection;
    CIP_ForwardOpen_Response_Success ForwardOpen_Response_Success;
    EIP_SessionStruct const * p_SessionStruct;
    CIP_ControlStruct const * p_cip_ctrl;

    EIP_Item_Explicit * p_Item_O2T;
    EIP_Item_Explicit * p_Item_T2O;
    EIP_Item_Sockaddr_Info * p_Sockaddr_Info_O2T;
    EIP_Item_Sockaddr_Info * p_Sockaddr_Info_T2O;
    EIP_TCPObject_InterfaceConfiguration * p_tcp;

    CIP_UINT Item_Index;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_ForwardOpen_Request || !p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);   

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_ImplicitConnection_Local = &ImplicitConnection_Local;

    local_init_ImplicitConnection(p_ImplicitConnection_Local);
    p_ImplicitConnection_Local->connection_is_used = RTA_TRUE;

    /* TODO: Refactor to smaller functions */

    /* ********************************** */
    /* Convert Network to Internal Format */
    /* ********************************** */

    /* CIP Volume 1 - 3-5.4.1.2.1 Unconnected Request Timing */
    /* When a target receives a Forward_Open, Large_Forward_Open or Forward_Close request, the target may either:
        1. ignore the Priority/Time_tick and Time-out ticks parameters and process the request or 
        2. evaluate and send an error */
    RTA_UNUSED_PARAM(p_ForwardOpen_Request->Priority_TimeTick);
    RTA_UNUSED_PARAM(p_ForwardOpen_Request->Timeout_Ticks);

    memcpy(&p_ImplicitConnection_Local->Connection_Triad, &p_ForwardOpen_Request->Connection_Triad, sizeof(p_ImplicitConnection_Local->Connection_Triad));

    rc = cip_obj_cnxnmgr_convert_network_to_Internal_NetworkConnectionParameters_ForwardOpen(p_ErrorStruct, p_ForwardOpen_Request->O2T.NetworkConnectionParameters, &p_ImplicitConnection_Local->O2T.InternalNetworkConnectionParameters);
    if(rc != CIP_RETURNCODE_SUCCESS) 
        CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_obj_cnxnmgr_convert_network_to_Internal_NetworkConnectionParameters_ForwardOpen(p_ErrorStruct, p_ForwardOpen_Request->T2O.NetworkConnectionParameters, &p_ImplicitConnection_Local->T2O.InternalNetworkConnectionParameters);
    if(rc != CIP_RETURNCODE_SUCCESS) 
        CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_obj_cnxnmgr_convert_network_to_Internal_ConnectionTimeoutMultiplier(p_ErrorStruct, p_ForwardOpen_Request->ConnectionTimeoutMultiplier, &p_ImplicitConnection_Local->ConnectionTimeoutMultiplier);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    /* *************************************** */
    /* process the Forward Open request type   */
    /* *************************************** */

    /* CIP Volume 1 - 3-5.4.2.2 Null Forward Open */
    if ((p_ImplicitConnection_Local->O2T.InternalNetworkConnectionParameters.ConnectionType == ConnectionType_Null) || 
        (p_ImplicitConnection_Local->T2O.InternalNetworkConnectionParameters.ConnectionType == ConnectionType_Null) )
    {
        /* if non matching we could ping or configure */
        /* if matching we could re-configure */
        /* we don't support either for now */
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_NULL_FORWARD_OPEN_FUNCTION_NOT_SUPPORTED));
    }

    /* CIP Volume 1 - 3-5.4.2.1 Non-Null Forward Open */
    else
    {    
        p_MatchingImplicitConnection = cip_obj_cnxnmgr_implicit_find_connection_by_Connection_Triad(&p_ForwardOpen_Request->Connection_Triad);

        /* CIP Volume 1 - 3-5.4.2.1.2 Non-Null Forward Open, Matching - Error */
        if (p_MatchingImplicitConnection)
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_CONNECTION_IN_USE_OR_DUPLICATE_FORWARD_OPEN));
    }

    /* CIP Volume 1 - 3-5.4.2.1.1 Non-Null Forward Open, Not Matching - Open a Connection */

    /* ****************************** */
    /* validate the passed parameters */
    /* ****************************** */
    rc = validate_ImplicitConnection_O2T_InternalNetworkConnectionParameters(p_ErrorStruct, &p_ImplicitConnection_Local->O2T.InternalNetworkConnectionParameters);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    rc = validate_ImplicitConnection_T2O_InternalNetworkConnectionParameters(p_ErrorStruct, &p_ImplicitConnection_Local->T2O.InternalNetworkConnectionParameters);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    rc = validate_and_store_ImplicitConnection_ConnectionPath(p_ErrorStruct, &p_ForwardOpen_Request->Connection_Path, p_ImplicitConnection_Local);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_obj_cnxnmgr_validate_RPIs_are_in_range(p_ErrorStruct, p_ForwardOpen_Request->O2T.RPI_in_microseconds, p_ForwardOpen_Request->T2O.RPI_in_microseconds);
    if (rc == CIP_RETURNCODE_ABORTED_PROCESS)
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);
 
    rc = local_validate_O2T_instance(p_ErrorStruct, p_ImplicitConnection_Local->O2T.AssemblyInstance);
    if (rc == CIP_RETURNCODE_ABORTED_PROCESS)
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);
 
    rc = local_validate_O2T_connection_size(p_ErrorStruct, p_ImplicitConnection_Local->O2T.AssemblyInstance, p_ImplicitConnection_Local->O2T.InternalNetworkConnectionParameters.ConnectionSize);
    if (rc == CIP_RETURNCODE_ABORTED_PROCESS)
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    rc = local_validate_T2O_instance(p_ErrorStruct, p_ImplicitConnection_Local->T2O.AssemblyInstance);
    if (rc == CIP_RETURNCODE_ABORTED_PROCESS)
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    rc = local_validate_T2O_connection_size(p_ErrorStruct, p_ImplicitConnection_Local->T2O.AssemblyInstance, p_ImplicitConnection_Local->T2O.InternalNetworkConnectionParameters.ConnectionSize);
    if (rc == CIP_RETURNCODE_ABORTED_PROCESS)
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    /* *************************** */
    /* Store our owning IP address */
    /* *************************** */
    p_SessionStruct = eip_encap_find_Session_from_SessionHandle(p_cip_ctrl->EIP_SessionHandle);
    if (!p_SessionStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

    if (rta_usersock_get_tcp_remote_ipaddr(p_SessionStruct->p_socket, &p_ImplicitConnection_Local->OwningIPAddress) != RTA_USERSOCK_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    /* we are good to go, try to allocate a connection */    
    p_ImplicitConnection_New = local_find_free_implicit_connection();
    if (!p_ImplicitConnection_New)
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_OUT_OF_CONNECTIONS));
    
    /* NOTE: from this point forward all errors must free p_ImplicitConnection_New */

    rc = local_deepcopy_ImplicitConnection(p_ImplicitConnection_New, p_ImplicitConnection_Local);
    if (rc != CIP_RETURNCODE_SUCCESS)
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    p_ImplicitConnection_New->O2T.API_in_milliseconds = (p_ForwardOpen_Request->O2T.RPI_in_microseconds / 1000UL);
    p_ImplicitConnection_New->T2O.API_in_milliseconds = (p_ForwardOpen_Request->T2O.RPI_in_microseconds / 1000UL);

    if ((p_ImplicitConnection_New->O2T.API_in_milliseconds == 0) || (p_ImplicitConnection_New->T2O.API_in_milliseconds == 0))
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_RPI_NOTSUPPORTED));
    }

    rc = cip_obj_cnxnmgr_convert_API_and_Multiplier_to_milliseconds(p_ErrorStruct, p_ImplicitConnection_New->O2T.API_in_milliseconds, p_ImplicitConnection_New->ConnectionTimeoutMultiplier, &p_ImplicitConnection_New->Connection_Timeout_reload_ms);
    if (rc != CIP_RETURNCODE_SUCCESS)
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    /* "Targets shall wait at least 10 seconds after sending a successful response for the first packet on a connection." */
    if (p_ImplicitConnection_New->Connection_Timeout_reload_ms < CIP_OBJ_CNXNMGR_FIRST_TIME_RPI_IN_MILLISECONDS)
        p_ImplicitConnection_New->Connection_Timeout_remaining_ms = CIP_OBJ_CNXNMGR_FIRST_TIME_RPI_IN_MILLISECONDS;
    else
        p_ImplicitConnection_New->Connection_Timeout_remaining_ms = p_ImplicitConnection_New->Connection_Timeout_reload_ms;

    /* for sending, we just use the API */
    p_ImplicitConnection_New->SendNeeded_Timeout_reload_ms = p_ImplicitConnection_New->T2O.API_in_milliseconds;
    p_ImplicitConnection_New->SendNeeded_Timeout_remaining_ms = 1; /* we want to timeout immediately */
    
    /* CIP Volume 2 EtherNet/IP - Table 3-3.2 Network Connection ID Selection */
    /*  Connection Type     Which Network Connection ID     Who chooses Connection ID
        ---------------     ---------------------------     -------------------------
        Point-to-point      Originator->Target              Target (us)
                            Target->Originator              Originator
        Multicast           Originator->Target              Originator
                            Target->Originator              Target (us)
    */
    switch(p_ImplicitConnection_New->T2O.InternalNetworkConnectionParameters.ConnectionType)
    {
        case ConnectionType_PointToPoint:
            p_ImplicitConnection_New->T2O.NetworkConnectionID = p_ForwardOpen_Request->T2O.NetworkConnectionID;

            p_ImplicitConnection_New->T2O.UDP_IPAddress = p_ImplicitConnection_New->OwningIPAddress;
            p_ImplicitConnection_New->T2O.UDP_PortID = EIP_USERSOCK_PORT_UDP_IO;

            /* Volume 2 2-6.3.3 Sockaddr Info Item

            For point-point connections, sin_port shall be set to the UDP port to which packets for this CIP connection will be sent. 
            For point-point connections, it is recommended that the registered UDP port (0x8AE) be used.
            When used with a multicast connection, the sin_port field shall be set to the registered UDP port number (0x08AE) and treated by the receiver as dont care. */
            if(p_eip_ctrl->p_Request_ItemArray)
            {
                for(Item_Index=0; Item_Index<p_eip_ctrl->p_Request_ItemArray->ItemCount; Item_Index++)
                {
                    if(p_eip_ctrl->p_Request_ItemArray->ItemData[Item_Index].ItemID == EIP_ItemID_Sockaddr_Info_T2O)
                    {
                        p_Item_T2O = &p_eip_ctrl->p_Request_ItemArray->ItemData[Item_Index];
                        if(p_Item_T2O->ItemData.p_Item_Sockaddr_Info)
                        {
                            p_ImplicitConnection_New->T2O.UDP_PortID = p_Item_T2O->ItemData.p_Item_Sockaddr_Info->SocketAddress.sin_port;
                        }
                    }
                }
            }
            break;

        case ConnectionType_Multicast:
        case ConnectionType_Null:
        case ConnectionType_Reserved:
        case ConnectionType_Unknown:
        default:
            cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_TYPE));
    }

    switch(p_ImplicitConnection_New->O2T.InternalNetworkConnectionParameters.ConnectionType)
    {
        case ConnectionType_PointToPoint:
            p_ImplicitConnection_New->O2T.NetworkConnectionID = cip_obj_cnxnmgr_get_ConnectionID();
    
            /* get our IP address */
            p_tcp = cip_binary_malloc(p_cip_ctrl, sizeof(*p_tcp));
            if(!p_tcp)
            { 
                cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
                CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
            }

            rc = eip_userobj_tcp_get_EIP_TCPObject_InterfaceConfiguration(p_cip_ctrl, p_tcp);
            if(rc != CIP_RETURNCODE_SUCCESS) 
            {
                cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
                CIP_LOG_ERROR_AND_RETURN(rc);
            }

            p_ImplicitConnection_New->O2T.UDP_IPAddress = p_tcp->IP_Address;
            p_ImplicitConnection_New->O2T.UDP_PortID = EIP_USERSOCK_PORT_UDP_IO;
            break;

        case ConnectionType_Multicast:
        case ConnectionType_Null:
        case ConnectionType_Reserved:
        case ConnectionType_Unknown:
        default:
            cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_TYPE));
    }
    
    /* build successful message */
    memset(&ForwardOpen_Response_Success, 0, sizeof(ForwardOpen_Response_Success));
    memcpy(&ForwardOpen_Response_Success.Connection_Triad, &p_ForwardOpen_Request->Connection_Triad, sizeof(ForwardOpen_Response_Success.Connection_Triad));
    ForwardOpen_Response_Success.O2T.NetworkConnectionID = p_ImplicitConnection_New->O2T.NetworkConnectionID;
    ForwardOpen_Response_Success.O2T.API_in_microseconds = (1000 * p_ImplicitConnection_New->O2T.API_in_milliseconds);
    ForwardOpen_Response_Success.T2O.NetworkConnectionID = p_ImplicitConnection_New->T2O.NetworkConnectionID;
    ForwardOpen_Response_Success.T2O.API_in_microseconds = (1000 * p_ImplicitConnection_New->T2O.API_in_milliseconds);
    ForwardOpen_Response_Success.Application.Reply_Size_In_Words = 0;

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Success(p_cip_ctrl, p_MessageRouterResponse, &ForwardOpen_Response_Success);
    if(rc != CIP_RETURNCODE_SUCCESS) 
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Success(p_cip_ctrl, p_MessageRouterResponse, &ForwardOpen_Response_Success);
    if(rc != CIP_RETURNCODE_SUCCESS) 
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    if(!p_eip_ctrl->p_Response_ItemArray)   
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);
    }

    p_eip_ctrl->p_Response_ItemArray->ItemCount = 4;
    if(p_eip_ctrl->p_Response_ItemArray->ItemCount > EIP_Max_Num_EIP_Item_Array_Elements)
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    }
    
    /* items 0 and 1 are filled in by the stack earlier */
    
    /* item 2 is the O2T socket */  
    p_Item_O2T = &p_eip_ctrl->p_Response_ItemArray->ItemData[2];
    p_Item_O2T->ItemID = EIP_ItemID_Sockaddr_Info_O2T;
    p_Item_O2T->ItemLength = sizeof(*p_Item_O2T->ItemData.p_Item_Sockaddr_Info);
    p_Item_O2T->ItemData.p_Item_Sockaddr_Info = cip_binary_malloc(p_cip_ctrl, p_Item_O2T->ItemLength);
    if(!p_Item_O2T->ItemData.p_Item_Sockaddr_Info)
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);   
    }

    p_Sockaddr_Info_O2T = p_Item_O2T->ItemData.p_Item_Sockaddr_Info;
    p_Sockaddr_Info_O2T->SocketAddress.sin_family = EIP_USERSOCK_SIN_FAMILY;
    p_Sockaddr_Info_O2T->SocketAddress.sin_port = p_ImplicitConnection_New->O2T.UDP_PortID;
    p_Sockaddr_Info_O2T->SocketAddress.sin_addr = p_ImplicitConnection_New->O2T.UDP_IPAddress;
    memset(p_Sockaddr_Info_O2T->SocketAddress.sin_zero, 0, sizeof(p_Sockaddr_Info_O2T->SocketAddress.sin_zero));

    /* item 3 is the T2O socket */
    p_Item_T2O = &p_eip_ctrl->p_Response_ItemArray->ItemData[3];
    p_Item_T2O->ItemID = EIP_ItemID_Sockaddr_Info_T2O;
    p_Item_T2O->ItemLength = sizeof(*p_Item_T2O->ItemData.p_Item_Sockaddr_Info);
    p_Item_T2O->ItemData.p_Item_Sockaddr_Info = cip_binary_malloc(p_cip_ctrl, p_Item_T2O->ItemLength);
    if(!p_Item_T2O->ItemData.p_Item_Sockaddr_Info)
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);   
    }

    p_Sockaddr_Info_T2O = p_Item_T2O->ItemData.p_Item_Sockaddr_Info;
    p_Sockaddr_Info_T2O->SocketAddress.sin_family = EIP_USERSOCK_SIN_FAMILY;
    p_Sockaddr_Info_T2O->SocketAddress.sin_port = p_ImplicitConnection_New->T2O.UDP_PortID;
    p_Sockaddr_Info_T2O->SocketAddress.sin_addr = p_ImplicitConnection_New->T2O.UDP_IPAddress;
    memset(p_Sockaddr_Info_T2O->SocketAddress.sin_zero, 0, sizeof(p_Sockaddr_Info_T2O->SocketAddress.sin_zero));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_obj_cnxnmgr_implicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose_Class1(CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardClose_Request const * const p_ForwardClose_Request, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;    
    CIP_OBJ_ForwardClose_ErrorStruct ErrorStruct;
    CIP_OBJ_ForwardClose_ErrorStruct const * const p_ErrorStruct = &ErrorStruct; /* common calling convention */
    CIP_ImplicitConnection * p_ImplicitConnection;
    CIP_ForwardClose_Response_Success ForwardClose_Response_Success;
    EIP_SessionStruct const * p_SessionStruct;
    CIP_UDINT socket_ip;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_ForwardClose_Request || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    ErrorStruct.p_cip_ctrl = p_cip_ctrl;
    ErrorStruct.p_Connection_Triad = &p_ForwardClose_Request->Connection_Triad;
    ErrorStruct.p_MessageRouterResponse = p_MessageRouterResponse;

    /* CIP Volume 1 - 3 - 5.4.4 Forward_Close */
    p_ImplicitConnection = cip_obj_cnxnmgr_implicit_find_connection_by_Connection_Triad(&p_ForwardClose_Request->Connection_Triad);  
    if(!p_ImplicitConnection)
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    rc = validate_and_store_ImplicitConnection_ConnectionPath(p_ErrorStruct, &p_ForwardClose_Request->Connection_Path, RTA_NULL);
    if (rc != CIP_RETURNCODE_SUCCESS)
        return(cip_obj_cnxnmgr_build_forwardclose_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_SEGMENT_IN_CONNECTION_PATH));

    /* CIP Volume 2 - 3-3.10 Forward_Close */
    /*  "In addition to the Forward_Close verification that is specified in Volume 1, section 3-5.4.4, 
        EtherNet/IP targets and CIP routers must verify that the IP address of the sender matches the IP address 
        of the sender that opened the connection. If the IP address does not match, the target or router shall 
        return an error with a General Status code of 0x0F (Privilege Violation). */
    p_SessionStruct = eip_encap_find_Session_from_SessionHandle(p_cip_ctrl->EIP_SessionHandle);
    if (!p_SessionStruct)
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Privilege_violation));
    if (rta_usersock_get_tcp_remote_ipaddr(p_SessionStruct->p_socket, &socket_ip) != RTA_USERSOCK_RETURNCODE_SUCCESS)
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Privilege_violation));        
    if(socket_ip != p_ImplicitConnection->OwningIPAddress)
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Privilege_violation));

    cip_userobj_assembly_close_received_for_o2t_instance(p_ImplicitConnection->O2T.AssemblyInstance);

    /* build successful message */
    memset(&ForwardClose_Response_Success, 0, sizeof(ForwardClose_Response_Success));
    memcpy(&ForwardClose_Response_Success.Connection_Triad, &p_ForwardClose_Request->Connection_Triad, sizeof(ForwardClose_Response_Success.Connection_Triad));
    ForwardClose_Response_Success.Application.Reply_Size_In_Words = 0;

    rc = cip_process_write_response_data_with_CIP_ForwardClose_Response_Success(p_cip_ctrl, p_MessageRouterResponse, &ForwardClose_Response_Success);
    if (rc != CIP_RETURNCODE_SUCCESS)
    {
        cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    /* success so free the connection */
    cip_obj_cnxnmgr_free_ImplicitConnection(p_ImplicitConnection);
    
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_obj_cnxnmgr_implicit_process_message (EIP_ControlStruct * const p_eip_ctrl)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct * p_cip_ctrl;
    EIP_Item_Array_Implicit * p_ItemArrayImplicit;

    CIP_LOG_FUNCTION_ENTER;

    if(!p_eip_ctrl)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    /* ************** */
    /* decode message */
    /* ************** */
    p_ItemArrayImplicit = cip_binary_malloc(p_cip_ctrl, sizeof(*p_ItemArrayImplicit));
    if(!p_ItemArrayImplicit)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    rc = eip_decode_EIP_Item_Array_Implicit (p_eip_ctrl, p_ItemArrayImplicit);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
    
    /* is there data remaining? */
    /* TODO: what to do if there is more data? */

    rc = cip_binary_swap_heaps(p_cip_ctrl, RTA_USER_MEMORY_ALIGNMENT_INTERNAL);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = local_process_received_packet(p_cip_ctrl, p_ItemArrayImplicit);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    return(CIP_RETURNCODE_SUCCESS);
}

void cip_obj_cnxnmgr_implicit_new_T2O_data (CIP_DWORD const instance)
{
    RTA_Size_Type ii; 

    for(ii=0; ii<CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
    {
        if(ImplicitConnection[ii].T2O.AssemblyInstance == instance)
            ImplicitConnection[ii].T2O.PDU_SequenceNumber++;
    }
}

CIP_ImplicitConnection * cip_obj_cnxnmgr_implicit_find_connection_by_Connection_Triad (CIP_Connection_Triad const * const p_Connection_Triad)
{
    RTA_Size_Type ii;

    if(!p_Connection_Triad)
        return(RTA_NULL);

    for(ii=0; ii<CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
    {
        if(!ImplicitConnection[ii].connection_is_used)
            continue;

        if(ImplicitConnection[ii].Connection_Triad.OriginatorVendorID != p_Connection_Triad->OriginatorVendorID)
            continue;

        if(ImplicitConnection[ii].Connection_Triad.OriginatorSerialNumber != p_Connection_Triad->OriginatorSerialNumber)
            continue;

        if(ImplicitConnection[ii].Connection_Triad.ConnectionSerialNumber != p_Connection_Triad->ConnectionSerialNumber)
            continue;

        return(&ImplicitConnection[ii]);
    }    

    return(RTA_NULL);
}

CIP_BOOL cip_obj_cnxnmgr_implicit_ip_still_open (CIP_UDINT const owning_ip_that_timed_out)
{
    RTA_Size_Type ii;

    for (ii = 0; ii < CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
    {
        if(!ImplicitConnection[ii].connection_is_used)
            continue;

        if(ImplicitConnection[ii].OwningIPAddress == owning_ip_that_timed_out)
            return(RTA_TRUE);
    }

    return(RTA_FALSE);
}

CIP_BOOL cip_obj_cnxnmgr_implicit_connections_established (void)
{
    RTA_Size_Type ii;

    for (ii = 0; ii < CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
    {
        if (ImplicitConnection[ii].connection_is_used)
            return(RTA_TRUE);
    }

    return(RTA_FALSE);
}

CIP_BOOL cip_obj_cnxnmgr_implicit_o2t_assembly_owned (CIP_UDINT const instance)
{
    RTA_Size_Type ii;

    for (ii = 0; ii < CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
    {
        if ((ImplicitConnection[ii].connection_is_used) && (ImplicitConnection[ii].O2T.AssemblyInstance == instance))
            return(RTA_TRUE);
    }

    return(RTA_FALSE);
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */
static void local_init_ImplicitConnection (CIP_ImplicitConnection * const p_ImplicitConnection)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_ImplicitConnection)
        return;

    memset(p_ImplicitConnection, 0, sizeof(*p_ImplicitConnection));

    p_ImplicitConnection->connection_is_used = RTA_FALSE;

    p_ImplicitConnection->ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_Unknown;

    /* TODO: if these blocks are called more than once make a function */
    p_ImplicitConnection->O2T.InternalNetworkConnectionParameters.ConnectionType = ConnectionType_Unknown;
    p_ImplicitConnection->O2T.InternalNetworkConnectionParameters.isVariableSize = RTA_FALSE;
    p_ImplicitConnection->O2T.InternalNetworkConnectionParameters.Priority = Priority_Unknown;
    p_ImplicitConnection->O2T.InternalNetworkConnectionParameters.RedundantOwner = RTA_FALSE;
    
    p_ImplicitConnection->T2O.InternalNetworkConnectionParameters.ConnectionType = ConnectionType_Unknown;
    p_ImplicitConnection->T2O.InternalNetworkConnectionParameters.isVariableSize = RTA_FALSE;
    p_ImplicitConnection->T2O.InternalNetworkConnectionParameters.Priority = Priority_Unknown;
    p_ImplicitConnection->T2O.InternalNetworkConnectionParameters.RedundantOwner = RTA_FALSE;

    CIP_LOG_FUNCTION_EXIT;
}

static CIP_ImplicitConnection * local_find_free_implicit_connection (void)
{
    RTA_Size_Type ii;
    
    for(ii=0; ii<CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
    {
        if(ImplicitConnection[ii].connection_is_used)
            continue;

        ImplicitConnection[ii].connection_is_used = RTA_TRUE;

        return(&ImplicitConnection[ii]);
    }    

    return(RTA_NULL);
}

/* TODO: change to decode, then process the packet based on what type of packet we expect */
static CIP_ReturnCode local_process_received_packet (CIP_ControlStruct const * const p_cip_ctrl, EIP_Item_Array_Implicit const * const p_Item_Array_Implicit)
{
    CIP_ReturnCode rc;

    EIP_Item_Implicit const * p_Item_Address;
    EIP_Item_SequencedAddress const * p_Item_SequencedAddress;
    EIP_Item_Implicit const * p_Item_Data;
    CIP_Opaque_String const * p_Item_ConnectedData_Opaque_String;

    CIP_ImplicitConnection * p_ImplicitConnection = RTA_NULL;
    RTA_Size_Type ii;

    CIP_BOOL is_heartbeat;
    CIP_UINT expected_size, application_data_size;

    CIP_BYTE const * p_BYTE;
    CIP_UINT pdu_sequence_number;
    CIP_UDINT run_idle_header;
    CIP_Opaque_String * p_Opaque_String_application_data;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_Item_Array_Implicit)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* CIP Volume 2 EIP - 3-2.2.1 CIP transport Class 0 and Class 1 Packets */
    if (p_Item_Array_Implicit->ItemCount != 2)  CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    /* validate address item */
    p_Item_Address = &p_Item_Array_Implicit->ItemData[0];

    if (p_Item_Address->ItemID != EIP_ItemID_SequencedAddress)  CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if (p_Item_Address->ItemLength != EIP_ItemLength_Expected_SequencedAddress) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if (!p_Item_Address->ItemData.p_Item_SequencedAddress) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    p_Item_SequencedAddress = p_Item_Address->ItemData.p_Item_SequencedAddress;

    /* validate data item */
    p_Item_Data = &p_Item_Array_Implicit->ItemData[1];

    if (p_Item_Data->ItemID != EIP_ItemID_ConnectedData)  CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if (p_Item_Data->ItemLength > EIP_ItemLength_Max_ConnectedData)  CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if (p_Item_Data->ItemLength > 0)
    {
        if (!p_Item_Data->ItemData.p_Item_ConnectedData_Opaque_String) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
    }

    p_Item_ConnectedData_Opaque_String = p_Item_Data->ItemData.p_Item_ConnectedData_Opaque_String;

    /* find if this packet is to an open connection */
    for (ii = 0; ii < CIP_NUM_IMPLICIT_CONNECTIONS; ii++)
    {
        if(ImplicitConnection[ii].O2T.NetworkConnectionID == p_Item_SequencedAddress->ConnectionIdentifier)
        {
            p_ImplicitConnection = &ImplicitConnection[ii];
            break;
        }
    }

    if(!p_ImplicitConnection) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    /* only error out on the sequence number for the second packet forward */
    if(p_ImplicitConnection->O2T.OneOrMorePacketsTransferred)
    {
        /* CIP Volume 2 EIP - 3-4.1 CIP Transport Class 0 and Class 1 Packet Ordering */
        if(CIP_32BIT_SEQ_LEQ(p_Item_SequencedAddress->EncapsulationSequenceNumber, p_ImplicitConnection->O2T.ConnectedData_SequenceNumber)) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
    }

    p_ImplicitConnection->O2T.ConnectedData_SequenceNumber = p_Item_SequencedAddress->EncapsulationSequenceNumber;

    /* ***************** */
    /* validate the size */
    /* ***************** */
    
    if(!cip_userobj_assembly_is_an_o2t_instance(p_ImplicitConnection->O2T.AssemblyInstance))
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);
            
    /* CIP Volume 1 - 3-6.1 Real time formats including RUN/IDLE notification */
    is_heartbeat = cip_userobj_assembly_is_an_o2t_heartbeat(p_ImplicitConnection->O2T.AssemblyInstance);
    application_data_size = cip_userobj_assembly_get_size_for_instance(p_ImplicitConnection->O2T.AssemblyInstance);

    /* CIP Volume 1 - 3-6.1.3 Heartbeat Format (Class 1) */
    if(is_heartbeat)
    {
        expected_size = 2; /* PDU sequence number for class 1 connections */
    }
    
    /* CIP Volume 1 - 3-6.1.4 32-Bit Header Format (Class 1) */
    else
    {
        expected_size = 2; /* PDU sequence number for class 1 connections */
        expected_size += 4; /* 32-bit Real Time Header */
        expected_size += application_data_size; 
    }

    if(p_Item_ConnectedData_Opaque_String->Length != expected_size) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);
    
    /* at this point we are valid, so process the data */
    cip_obj_cnxnmgr_ImplicitConnection_received(p_ImplicitConnection);

    p_BYTE = p_Item_ConnectedData_Opaque_String->pData;

    pdu_sequence_number = rta_endian_GetLitEndian16(p_BYTE); 
    p_BYTE+=2;

    if(p_ImplicitConnection->O2T.PDU_SequenceNumber == pdu_sequence_number) /* data isn't new, so don't send to user */
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

    p_ImplicitConnection->O2T.PDU_SequenceNumber = pdu_sequence_number;
    
    if(is_heartbeat) 
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

    /* CIP Volume 1 - 3-6.1.4 32-Bit Header Format */
    run_idle_header = rta_endian_GetLitEndian32(p_BYTE); 
    p_BYTE+=4;

    p_Opaque_String_application_data = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Opaque_String_application_data));
    if(!p_Opaque_String_application_data) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    if((run_idle_header & CIP_CNXNMGR_IMPLICIT_32Bit_Header_Running) == CIP_CNXNMGR_IMPLICIT_32Bit_Header_Running)      
        p_Opaque_String_application_data->Length = application_data_size;
    else
        p_Opaque_String_application_data->Length = 0;
 
    if (p_Opaque_String_application_data->Length)
    {
        p_Opaque_String_application_data->pData = cip_binary_malloc(p_cip_ctrl, p_Opaque_String_application_data->Length);
        if (!p_Opaque_String_application_data->pData) CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

        memcpy(p_Opaque_String_application_data->pData, p_BYTE, p_Opaque_String_application_data->Length);

        rc = cip_userobj_assembly_write_data_for_instance(p_ImplicitConnection->O2T.AssemblyInstance, p_Opaque_String_application_data);
        if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
    }
    else
    {
        cip_userobj_assembly_idle_received_for_o2t_instance(p_ImplicitConnection->O2T.AssemblyInstance);
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_SendNeeded (CIP_ImplicitConnection * const p_ImplicitConnection)
{
    CIP_ReturnCode rc;
    EIP_ControlStruct eip_ctrl;
    CIP_ControlStruct * p_cip_ctrl;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ImplicitConnection)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_ImplicitConnection->SendNeeded_Timeout_remaining_ms = p_ImplicitConnection->SendNeeded_Timeout_reload_ms;

    /* ************** */
    /* Allocate Heaps */
    /* ************** */
    p_cip_ctrl = &eip_ctrl.cip_ctrl;

    p_cip_ctrl->p_heap_dst = rta_memory_reserve_heap(RTA_USER_MEMORY_ALIGNMENT_NETWORK);
    p_cip_ctrl->p_heap_src = rta_memory_reserve_heap(RTA_USER_MEMORY_ALIGNMENT_NETWORK);

    if((p_cip_ctrl->p_heap_dst == RTA_NULL) || (p_cip_ctrl->p_heap_src == RTA_NULL))
    {
        rta_memory_abandon_heap(p_cip_ctrl->p_heap_src);
        rta_memory_abandon_heap(p_cip_ctrl->p_heap_dst);
        return(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    }

    rc = local_send_packet (&eip_ctrl, p_ImplicitConnection); /* delay rc process until we free the heaps */

    /* ********** */
    /* Free Heaps */
    /* ********** */
    rta_memory_abandon_heap(p_cip_ctrl->p_heap_src);
    rta_memory_abandon_heap(p_cip_ctrl->p_heap_dst);

    CIP_LOG_EXIT_AND_RETURN(rc);
}

/* TODO: refactor to have a build step, then a send part; create packet types of class 0 or class 1, with or w/o heartbeat, with or without run/idle  */
static CIP_ReturnCode local_send_packet (EIP_ControlStruct * const p_eip_ctrl, CIP_ImplicitConnection * const p_ImplicitConnection)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct * p_cip_ctrl;

    EIP_Item_Array_Implicit * p_Item_Array_Implicit;
    EIP_Item_Implicit * p_Item_Address;
    EIP_Item_SequencedAddress * p_SequencedAddress;
    EIP_Item_Implicit * p_Item_Data;
    CIP_Opaque_String * p_ConnectedData_Opaque_String;
#if EIP_USER_OPTION_QOS_ENABLED
    CIP_QoSObject QoSObject;
    CIP_USINT tos = 0;
#else
    CIP_USINT const tos = 0;
#endif /* #if EIP_USER_OPTION_QOS_ENABLED */

    CIP_BOOL is_heartbeat;
    CIP_UINT expected_size, application_data_size;

    CIP_BYTE * p_BYTE;
    CIP_Opaque_String * p_Opaque_String_application_data;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_ImplicitConnection)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    p_Item_Array_Implicit = cip_binary_malloc (p_cip_ctrl, sizeof(*p_Item_Array_Implicit));    
    if(!p_Item_Array_Implicit)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED); 
    
    /* CIP Volume 2 EIP - 3-2.2.1 CIP transport Class 0 and Class 1 Packets */
    p_Item_Array_Implicit->ItemCount = 2;
    if(p_Item_Array_Implicit->ItemCount > EIP_Max_Num_EIP_Item_Array_Implicit_Elements)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED); 

    /* store address item */
    p_Item_Address = &p_Item_Array_Implicit->ItemData[0];
    p_Item_Address->ItemID = EIP_ItemID_SequencedAddress;
    p_Item_Address->ItemLength = 0; /* filled in later */
    p_Item_Address->ItemData.p_Item_SequencedAddress = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item_Address->ItemData.p_Item_SequencedAddress));
    if (!p_Item_Address->ItemData.p_Item_SequencedAddress)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_SequencedAddress = p_Item_Address->ItemData.p_Item_SequencedAddress;
    p_SequencedAddress->ConnectionIdentifier = p_ImplicitConnection->T2O.NetworkConnectionID;
    p_ImplicitConnection->T2O.ConnectedData_SequenceNumber++; /* this increments on every packet we send */
    p_SequencedAddress->EncapsulationSequenceNumber = p_ImplicitConnection->T2O.ConnectedData_SequenceNumber;

    /* store data item */
    p_Item_Data = &p_Item_Array_Implicit->ItemData[1];
    p_Item_Data->ItemID = EIP_ItemID_ConnectedData;
    p_Item_Data->ItemLength = 0; /* filled in later */
    p_Item_Data->ItemData.p_Item_ConnectedData_Opaque_String = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Item_Data->ItemData.p_Item_ConnectedData_Opaque_String));
    if (!p_Item_Data->ItemData.p_Item_ConnectedData_Opaque_String)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    if(!cip_userobj_assembly_is_an_t2o_instance(p_ImplicitConnection->T2O.AssemblyInstance))
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);
            
    /* CIP Volume 1 - 3-6.1 Real time formats including RUN/IDLE notification */
    is_heartbeat = cip_userobj_assembly_is_an_t2o_heartbeat(p_ImplicitConnection->T2O.AssemblyInstance);
    application_data_size = cip_userobj_assembly_get_size_for_instance(p_ImplicitConnection->T2O.AssemblyInstance);

    /* CIP Volume 1 - 3-6.1.3 Heartbeat Format (Class 1) */
    if(is_heartbeat)
    {
        expected_size = 2; /* PDU sequence number for class 1 connections */
    }
    
    /* CIP Volume 1 - 3-6.1.1 Modeless Format (Class 1) */
    else
    {
        expected_size = 2; /* PDU sequence number for class 1 connections */
        expected_size += application_data_size; 
    }

    /* NOTE: if expected_size gets to a point where it can be 0, add check below before we malloc p_ConnectedData_Opaque_String->pData */

    /* get the application data */
    p_Opaque_String_application_data = cip_binary_malloc(p_cip_ctrl, sizeof(*p_Opaque_String_application_data));
    if(!p_Opaque_String_application_data) 
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_Opaque_String_application_data->Length = application_data_size;

    if(p_Opaque_String_application_data->Length)
    {
        p_Opaque_String_application_data->pData = cip_binary_malloc(p_cip_ctrl, p_Opaque_String_application_data->Length);
        if(!p_Opaque_String_application_data->pData)
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);
    }

    rc = cip_userobj_assembly_read_data_for_instance(p_ImplicitConnection->T2O.AssemblyInstance, p_Opaque_String_application_data);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    /* ********************* */
    /* build the packet data */    
    /* ********************* */
    p_ConnectedData_Opaque_String = p_Item_Data->ItemData.p_Item_ConnectedData_Opaque_String;
    p_ConnectedData_Opaque_String->Length = expected_size;

    p_ConnectedData_Opaque_String->pData = cip_binary_malloc(p_cip_ctrl, expected_size);
    if(!p_ConnectedData_Opaque_String->pData)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_BYTE = p_ConnectedData_Opaque_String->pData;
    rta_endian_PutLitEndian16(p_ImplicitConnection->T2O.PDU_SequenceNumber, p_BYTE); 
    p_BYTE+=2;
    memcpy(p_BYTE, p_Opaque_String_application_data->pData, p_Opaque_String_application_data->Length);

    /* ********** */
    /* swap heaps */
    /* ********** */
    rc = cip_binary_swap_heaps(p_cip_ctrl, RTA_USER_MEMORY_ALIGNMENT_NETWORK);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    rc = eip_encode_EIP_Item_Array_Implicit(p_eip_ctrl, p_Item_Array_Implicit);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    /* ********** */
    /* swap heaps */
    /* ********** */
    rc = cip_binary_swap_heaps(p_cip_ctrl, RTA_USER_MEMORY_ALIGNMENT_INTERNAL);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

#if EIP_USER_OPTION_QOS_ENABLED
    if(eip_obj_qos_get_CIP_QoSObject (RTA_NULL, &QoSObject) == CIP_RETURNCODE_SUCCESS)
    {
    	switch(p_ImplicitConnection->T2O.InternalNetworkConnectionParameters.Priority)
    	{
        	case Priority_Low: 			tos = QoSObject.DSCP_Low; break;
        	case Priority_High: 		tos = QoSObject.DSCP_High; break;
        	case Priority_Scheduled: 	tos = QoSObject.DSCP_Scheduled; break;
        	case Priority_Urgent: 		tos = QoSObject.DSCP_Urgent; break;
        	case Priority_Unknown:
        	default:
        		tos = 0;
				break;
    	}
    }
#endif /* #if EIP_USER_OPTION_QOS_ENABLED */

    rc = eip_sockets_send_udp_io (p_ImplicitConnection->T2O.UDP_IPAddress, p_ImplicitConnection->T2O.UDP_PortID, p_cip_ctrl->p_heap_src->buffer, p_cip_ctrl->p_heap_src->bytes_used, tos);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

    p_ImplicitConnection->T2O.OneOrMorePacketsTransferred = RTA_TRUE;

    return(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_deepcopy_ImplicitConnection(CIP_ImplicitConnection * const p_dst_ImplicitConnection, CIP_ImplicitConnection const * const p_src_ImplicitConnection)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_dst_ImplicitConnection || !p_src_ImplicitConnection)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    memcpy(p_dst_ImplicitConnection, p_src_ImplicitConnection, sizeof(*p_dst_ImplicitConnection));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode validate_and_store_ImplicitConnection_ConnectionPath(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_Path_Array const * const p_Path, CIP_ImplicitConnection * const p_CIP_ImplicitConnection)
{
    CIP_ReturnCode rc;
    CIP_UINT    ii;
    CIP_UDINT   class_id = 0;
    CIP_BOOL    class_id_found = RTA_FALSE;
    CIP_UDINT   instance_id = 0;
    CIP_BOOL    instance_id_found = RTA_FALSE;
    CIP_ElectronicKey electronic_key;
    CIP_BOOL    electronic_key_found = RTA_FALSE;
    CIP_BOOL    connection_point1_found = RTA_FALSE;
    CIP_UDINT   connection_point1 = 0;
    CIP_BOOL    connection_point2_found = RTA_FALSE;
    CIP_UDINT   connection_point2 = 0;
    CIP_BOOL    data_segment_found = RTA_FALSE;
    CIP_DataSegment_SimpleDataSegment DataSegment_SimpleDataSegment;

    CIP_Path_Element const * p_Path_Element;

    CIP_LOG_FUNCTION_ENTER;

    /* p_CIP_ImplicitConnection is validated below... if present we store, if not we don't */
    if (!p_ErrorStruct || !p_Path)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* the only path we accept CI, with or without a valid electronic key */
    for (ii = 0; ii<p_Path->Path_Element_Count; ii++)
    {
        p_Path_Element = &p_Path->Path_Elements[ii];

        /* the electronic key can come in any order */
        if (cip_process_CIP_Path_Element_is_an_electronic_key(p_Path_Element))
        {
            if (electronic_key_found)
                return(cip_process_build_error(p_ErrorStruct->p_MessageRouterResponse, CIP_Status_Path_segment_error));

            rc = cip_process_get_electronic_key_from_CIP_Path_Element(p_Path_Element, &electronic_key);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

            rc = cip_obj_cnxnmgr_validate_ElectronicKey(p_ErrorStruct, &electronic_key);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

            electronic_key_found = RTA_TRUE;
        }
      
        /* CI must be class, then instance */
        else if (cip_process_CIP_Path_Element_is_a_class_id(p_Path_Element))
        {
            if (class_id_found)
                return(cip_process_build_error(p_ErrorStruct->p_MessageRouterResponse, CIP_Status_Path_segment_error));

            rc = cip_process_get_class_id_from_CIP_Path_Element(p_Path_Element, &class_id);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

            class_id_found = RTA_TRUE;
        }

        else if (cip_process_CIP_Path_Element_is_a_instance_id(p_Path_Element))
        {
            if (!class_id_found) /* we must have a class here */
                return(cip_process_build_error(p_ErrorStruct->p_MessageRouterResponse, CIP_Status_Path_segment_error));

            if (instance_id_found)
                return(cip_process_build_error(p_ErrorStruct->p_MessageRouterResponse, CIP_Status_Path_segment_error));

            rc = cip_process_get_instance_id_from_CIP_Path_Element(p_Path_Element, &instance_id);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

            instance_id_found = RTA_TRUE;
        }

        /* Connection Points (up to two) must come after the Class */
        else if (cip_process_CIP_Path_Element_is_a_connection_point(p_Path_Element))
        {
            /* we must have a class here */
            if (!class_id_found) 
                return(cip_process_build_error(p_ErrorStruct->p_MessageRouterResponse, CIP_Status_Path_segment_error));

            if (!connection_point1_found)
            {
                rc = cip_process_get_connection_point_from_CIP_Path_Element(p_Path_Element, &connection_point1);
                if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

                connection_point1_found = RTA_TRUE;
            }
            else if (!connection_point2_found)
            {
                rc = cip_process_get_connection_point_from_CIP_Path_Element(p_Path_Element, &connection_point2);
                if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

                connection_point2_found = RTA_TRUE;
            }
            else
                return(cip_process_build_error(p_ErrorStruct->p_MessageRouterResponse, CIP_Status_Path_segment_error));        
        }

        else if(cip_process_CIP_Path_Element_is_a_simple_data_segment(p_Path_Element))
        {
            if (data_segment_found)
                return(cip_process_build_error(p_ErrorStruct->p_MessageRouterResponse, CIP_Status_Path_segment_error));

            rc = cip_process_get_simple_data_segment_from_CIP_Path_Element(p_Path_Element, &DataSegment_SimpleDataSegment);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);

            data_segment_found = RTA_TRUE;
        }
        
        else
        {
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_SEGMENT_IN_CONNECTION_PATH));
        }
    }

    /* CIP Volume 1 - Table 3-5.13 Encoded Application Path Ordering */ 
    /* we don't support NULL O2T or T2O */
    /* we don't support the Data Segment, so don't check the instance */
    /* the first connection point is for Consumption (O2T) */
    /* the second connection point is for Production (T2O) */

    /* implicit message must point to the assembly since that is all we can map our I/O data to */
    if ((!class_id_found) || (class_id != CIP_Class_Code_Assembly))
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_SEGMENT_IN_CONNECTION_PATH));

    /* TODO: add support for configuration data */
    /* configuation data is ignored for now, so don't check the instance */

    /* we need a produce and consume path */
    if((!connection_point1_found) || (!connection_point2_found))
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_SEGMENT_IN_CONNECTION_PATH));

    if(instance_id_found)
    {
        if(data_segment_found)
        {
            rc = cip_userobj_assembly_validate_ConfigurationData(instance_id, &DataSegment_SimpleDataSegment, p_ErrorStruct); 

            if(rc != CIP_RETURNCODE_SUCCESS)   
                CIP_LOG_ERROR_AND_RETURN(rc);
        }
    }

    if(p_CIP_ImplicitConnection)
    {
        p_CIP_ImplicitConnection->O2T.AssemblyInstance = connection_point1;    
        p_CIP_ImplicitConnection->T2O.AssemblyInstance = connection_point2;
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode validate_ImplicitConnection_O2T_InternalNetworkConnectionParameters(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_CNXNMGR_Internal_NetworkConnectionParameters const * const p_Internal_NetworkConnectionParameters)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct || !p_Internal_NetworkConnectionParameters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if (p_Internal_NetworkConnectionParameters->RedundantOwner)
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_REDUNDANT_OWNER));

    if ((p_Internal_NetworkConnectionParameters->ConnectionType != ConnectionType_PointToPoint) && (p_Internal_NetworkConnectionParameters->ConnectionType != ConnectionType_Multicast))
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_TYPE));

    switch (p_Internal_NetworkConnectionParameters->Priority)
    {
        case Priority_Low:
        case Priority_High:
        case Priority_Scheduled:
        case Priority_Urgent:
            /* Priority is don't care */
            break;

        case Priority_Unknown:
        default:
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_PRIORITY));
    }

#ifndef ENIP_EXPLORER_MODE
    if (p_Internal_NetworkConnectionParameters->isVariableSize)  /* Implicit must be fixed size */
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_FIXVAR));
#endif

    if (p_Internal_NetworkConnectionParameters->ConnectionSize == 0) 
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_SIZE));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode validate_ImplicitConnection_T2O_InternalNetworkConnectionParameters(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_CNXNMGR_Internal_NetworkConnectionParameters const * const p_Internal_NetworkConnectionParameters)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct || !p_Internal_NetworkConnectionParameters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    RTA_UNUSED_PARAM(p_Internal_NetworkConnectionParameters->RedundantOwner); /* T2O can't be owned so this value is don't care */

    if ((p_Internal_NetworkConnectionParameters->ConnectionType != ConnectionType_PointToPoint) && (p_Internal_NetworkConnectionParameters->ConnectionType != ConnectionType_Multicast))
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_TYPE));

    switch (p_Internal_NetworkConnectionParameters->Priority)
    {
    case Priority_Low:
    case Priority_High:
    case Priority_Scheduled:
    case Priority_Urgent:
        /* Priority is don't care */
        break;

    case Priority_Unknown:
    default:
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_PRIORITY));
    }

#ifndef ENIP_EXPLORER_MODE
    if (p_Internal_NetworkConnectionParameters->isVariableSize)  /* Implicit must be fixed size */
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_FIXVAR));
#endif

    if (p_Internal_NetworkConnectionParameters->ConnectionSize == 0)
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_SIZE));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_validate_O2T_instance(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const O2T_Instance)
{
    CIP_ReturnCode rc;
    CIP_ForwardOpen_Response_Unsuccessful ForwardOpen_Response_Unsuccessful;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_Connection_Triad const * p_Connection_Triad;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(cip_userobj_assembly_is_an_o2t_instance(O2T_Instance))
        return(CIP_RETURNCODE_SUCCESS);

    p_cip_ctrl = p_ErrorStruct->p_cip_ctrl;
    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_Connection_Triad = p_ErrorStruct->p_Connection_Triad;
    
    /* CIP Volume 1 - Table 3-5.33 Connection Manager Service Error Codes */
    /* Build special error code */
    p_MessageRouterResponse->GeneralStatus = CIP_Status_Connection_failure;
    p_MessageRouterResponse->AdditionalStatus.NumberofStatuses = 1;
    p_MessageRouterResponse->AdditionalStatus.p_Status = cip_binary_malloc(p_cip_ctrl, (p_MessageRouterResponse->AdditionalStatus.NumberofStatuses * sizeof(*p_MessageRouterResponse->AdditionalStatus.p_Status)));
    if(!p_MessageRouterResponse->AdditionalStatus.p_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_MessageRouterResponse->AdditionalStatus.p_Status[0] = CIP_Connection_failure_ExtStatus_INVALID_CONSUMING_APPLICATION_PATH;

    memcpy(&ForwardOpen_Response_Unsuccessful.Connection_Triad, p_Connection_Triad, sizeof(ForwardOpen_Response_Unsuccessful.Connection_Triad));

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Unsuccessful(p_MessageRouterResponse, &ForwardOpen_Response_Unsuccessful);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    /* special code since we already sent the response */
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ABORTED_PROCESS); /* don't treat return as an error, just a known abort */
}

static CIP_ReturnCode local_validate_O2T_connection_size(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const O2T_Instance, CIP_UINT const O2T_ExpectedSize)
{
    CIP_ReturnCode rc;
    CIP_BOOL is_heartbeat;
    CIP_UINT expected_size, application_data_size;
    CIP_ForwardOpen_Response_Unsuccessful ForwardOpen_Response_Unsuccessful;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_Connection_Triad const * p_Connection_Triad;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(!cip_userobj_assembly_is_an_o2t_instance(O2T_Instance))
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);
            
    /* CIP Volume 1 - 3-6.1 Real time formats including RUN/IDLE notification */
    is_heartbeat = cip_userobj_assembly_is_an_o2t_heartbeat(O2T_Instance);
    application_data_size = cip_userobj_assembly_get_size_for_instance(O2T_Instance);

    /* 3-6.1.3 Heartbeat Format (Class 1) */
    if(is_heartbeat)
    {
        expected_size = 2; /* PDU sequence number for class 1 connections */
    }
    
    /* 3-6.1.4 32-Bit Header Format (Class 1) */
    else
    {
        expected_size = 2; /* PDU sequence number for class 1 connections */
        expected_size += 4; /* 32-bit Real Time Header */
        expected_size += application_data_size; 
    }

    if(expected_size == O2T_ExpectedSize)
        return(CIP_RETURNCODE_SUCCESS);

    p_cip_ctrl = p_ErrorStruct->p_cip_ctrl;
    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_Connection_Triad = p_ErrorStruct->p_Connection_Triad;
    
    /* CIP Volume 1 - Table 3-5.33 Connection Manager Service Error Codes */
    /* Build special error code */
    p_MessageRouterResponse->GeneralStatus = CIP_Status_Connection_failure;
    p_MessageRouterResponse->AdditionalStatus.NumberofStatuses = 2;
    p_MessageRouterResponse->AdditionalStatus.p_Status = cip_binary_malloc(p_cip_ctrl, (p_MessageRouterResponse->AdditionalStatus.NumberofStatuses * sizeof(*p_MessageRouterResponse->AdditionalStatus.p_Status)));
    if(!p_MessageRouterResponse->AdditionalStatus.p_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_MessageRouterResponse->AdditionalStatus.p_Status[0] = CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_SIZE;
    p_MessageRouterResponse->AdditionalStatus.p_Status[1] = expected_size;

    memcpy(&ForwardOpen_Response_Unsuccessful.Connection_Triad, p_Connection_Triad, sizeof(ForwardOpen_Response_Unsuccessful.Connection_Triad));

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Unsuccessful(p_MessageRouterResponse, &ForwardOpen_Response_Unsuccessful);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    /* special code since we already sent the response */
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ABORTED_PROCESS); /* don't treat return as an error, just a known abort */
}

static CIP_ReturnCode local_validate_T2O_instance(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const T2O_Instance)
{
    CIP_ReturnCode rc;
    CIP_ForwardOpen_Response_Unsuccessful ForwardOpen_Response_Unsuccessful;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_Connection_Triad const * p_Connection_Triad;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(cip_userobj_assembly_is_an_t2o_instance(T2O_Instance))
        return(CIP_RETURNCODE_SUCCESS);

    p_cip_ctrl = p_ErrorStruct->p_cip_ctrl;
    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_Connection_Triad = p_ErrorStruct->p_Connection_Triad;
    
    /* CIP Volume 1 - Table 3-5.33 Connection Manager Service Error Codes */
    /* Build special error code */
    p_MessageRouterResponse->GeneralStatus = CIP_Status_Connection_failure;
    p_MessageRouterResponse->AdditionalStatus.NumberofStatuses = 1;
    p_MessageRouterResponse->AdditionalStatus.p_Status = cip_binary_malloc(p_cip_ctrl, (p_MessageRouterResponse->AdditionalStatus.NumberofStatuses * sizeof(*p_MessageRouterResponse->AdditionalStatus.p_Status)));
    if(!p_MessageRouterResponse->AdditionalStatus.p_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_MessageRouterResponse->AdditionalStatus.p_Status[0] = CIP_Connection_failure_ExtStatus_INVALID_PRODUCING_APPLICATION_PATH;

    memcpy(&ForwardOpen_Response_Unsuccessful.Connection_Triad, p_Connection_Triad, sizeof(ForwardOpen_Response_Unsuccessful.Connection_Triad));

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Unsuccessful(p_MessageRouterResponse, &ForwardOpen_Response_Unsuccessful);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    /* special code since we already sent the response */
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ABORTED_PROCESS); /* don't treat return as an error, just a known abort */
}

static CIP_ReturnCode local_validate_T2O_connection_size(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const T2O_Instance, CIP_UINT const T2O_ExpectedSize)
{
    CIP_ReturnCode rc;
    CIP_BOOL is_heartbeat;
    CIP_UINT expected_size, application_data_size;
    CIP_ForwardOpen_Response_Unsuccessful ForwardOpen_Response_Unsuccessful;
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_Connection_Triad const * p_Connection_Triad;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if(!cip_userobj_assembly_is_an_t2o_instance(T2O_Instance))
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INTERNAL);
            
    /* CIP Volume 1 - 3-6.1 Real time formats including RUN/IDLE notification */
    is_heartbeat = cip_userobj_assembly_is_an_t2o_heartbeat(T2O_Instance);
    application_data_size = cip_userobj_assembly_get_size_for_instance(T2O_Instance);

    /* 3-6.1.3 Heartbeat Format (Class 1) */
    if(is_heartbeat)
    {
        expected_size = 2; /* PDU sequence number for class 1 connections */
    }
    
    /* 3-6.1.1 Modeless Format (Class 1) */
    else
    {
        expected_size = 2; /* PDU sequence number for class 1 connections */
        expected_size += application_data_size; 
    }

    if(expected_size == T2O_ExpectedSize)
        return(CIP_RETURNCODE_SUCCESS);

    p_cip_ctrl = p_ErrorStruct->p_cip_ctrl;
    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_Connection_Triad = p_ErrorStruct->p_Connection_Triad;
    
    /* CIP Volume 1 - Table 3-5.33 Connection Manager Service Error Codes */
    /* Build special error code */
    p_MessageRouterResponse->GeneralStatus = CIP_Status_Connection_failure;
    p_MessageRouterResponse->AdditionalStatus.NumberofStatuses = 2;
    p_MessageRouterResponse->AdditionalStatus.p_Status = cip_binary_malloc(p_cip_ctrl, (p_MessageRouterResponse->AdditionalStatus.NumberofStatuses * sizeof(*p_MessageRouterResponse->AdditionalStatus.p_Status)));
    if(!p_MessageRouterResponse->AdditionalStatus.p_Status)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    p_MessageRouterResponse->AdditionalStatus.p_Status[0] = CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_SIZE;
    p_MessageRouterResponse->AdditionalStatus.p_Status[1] = expected_size;

    memcpy(&ForwardOpen_Response_Unsuccessful.Connection_Triad, p_Connection_Triad, sizeof(ForwardOpen_Response_Unsuccessful.Connection_Triad));

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Unsuccessful(p_MessageRouterResponse, &ForwardOpen_Response_Unsuccessful);
    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);

    /* special code since we already sent the response */
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ABORTED_PROCESS); /* don't treat return as an error, just a known abort */
}

/* *********** */
/* END OF FILE */
/* *********** */
