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

static void local_init_ExplicitConnection (CIP_ExplicitConnection * const p_ExplicitConnection);
static CIP_ExplicitConnection * local_find_free_explicit_connection (void);
static CIP_ReturnCode validate_ExplicitConnection_ConnectionPath(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_Path_Array const * const p_Path_Array);
static CIP_ReturnCode validate_ExplicitConnection_O2T_InternalNetworkConnectionParameters(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_CNXNMGR_Internal_NetworkConnectionParameters const * const p_Internal_NetworkConnectionParameters);
static CIP_ReturnCode validate_ExplicitConnection_T2O_InternalNetworkConnectionParameters(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_CNXNMGR_Internal_NetworkConnectionParameters const * const p_Internal_NetworkConnectionParameters);
static CIP_ReturnCode local_deepcopy_ExplicitConnection(CIP_ExplicitConnection * const p_dst_ExplicitConnection, CIP_ExplicitConnection const * const p_src_ExplicitConnection);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* TODO: Move this to a user file */
#define CIP_NUM_EXPLICIT_CONNECTIONS 3
static CIP_ExplicitConnection ExplicitConnection[CIP_NUM_EXPLICIT_CONNECTIONS];

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode cip_obj_cnxnmgr_explicit_init (CIP_InitType const init_type)
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

    for(ii=0; ii<CIP_NUM_EXPLICIT_CONNECTIONS; ii++)
        local_init_ExplicitConnection(&ExplicitConnection[ii]);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_obj_cnxnmgr_explicit_process (RTA_U32 const ms_since_last_call)
{
    RTA_Size_Type ii;
    CIP_UDINT owning_ip;

    if (ms_since_last_call)
    {    
        for (ii = 0; ii < CIP_NUM_EXPLICIT_CONNECTIONS; ii++)
        {
            if (!ExplicitConnection[ii].connection_is_used)
                continue;

            if (ms_since_last_call >= ExplicitConnection[ii].Connection_Timeout_remaining_ms)
            {
                owning_ip = ExplicitConnection[ii].OwningIPAddress;
                cip_obj_cnxnmgr_free_ExplicitConnection(&ExplicitConnection[ii]);
                cip_obj_cnxnmgr_timeout_common(owning_ip);
            }
            else
            {   
                ExplicitConnection[ii].Connection_Timeout_remaining_ms -= ms_since_last_call;
            }
        }
    }

    return(CIP_RETURNCODE_SUCCESS);
}

void cip_obj_cnxnmgr_free_ExplicitConnection(CIP_ExplicitConnection * const p_ExplicitConnection)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_ExplicitConnection)
        return;

    eip_encap_free_ExplicitConnection_from_SessionHandle(p_ExplicitConnection->SessionHandle);

    local_init_ExplicitConnection(p_ExplicitConnection);

    CIP_LOG_FUNCTION_EXIT;
}

void cip_obj_cnxnmgr_ExplicitConnection_received(CIP_ExplicitConnection * const p_ExplicitConnection)
{
    if (!p_ExplicitConnection)
        return;

    p_ExplicitConnection->Connection_Timeout_remaining_ms = p_ExplicitConnection->Connection_Timeout_reload_ms;
}

CIP_ExplicitConnection * cip_obj_cnxnmgr_explicit_find_connection_by_Connection_Triad (CIP_Connection_Triad const * const p_Connection_Triad)
{
    RTA_Size_Type ii;

    if(!p_Connection_Triad)
        return(RTA_NULL);

    for(ii=0; ii<CIP_NUM_EXPLICIT_CONNECTIONS; ii++)
    {
        if(!ExplicitConnection[ii].connection_is_used)
            continue;

        if(ExplicitConnection[ii].Connection_Triad.OriginatorVendorID != p_Connection_Triad->OriginatorVendorID)
            continue;

        if(ExplicitConnection[ii].Connection_Triad.OriginatorSerialNumber != p_Connection_Triad->OriginatorSerialNumber)
            continue;

        if(ExplicitConnection[ii].Connection_Triad.ConnectionSerialNumber != p_Connection_Triad->ConnectionSerialNumber)
            continue;

        return(&ExplicitConnection[ii]);
    }    

    return(RTA_NULL);
}

CIP_ReturnCode cip_obj_cnxnmgr_explicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen_Class3 (CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardOpen_Request const * const p_ForwardOpen_Request, CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct)
{
    CIP_ReturnCode rc;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
    CIP_ExplicitConnection ExplicitConnection_Local;
    CIP_ExplicitConnection * p_ExplicitConnection_Local;
    CIP_ExplicitConnection * p_ExplicitConnection_New;
    CIP_ExplicitConnection const * p_MatchingExplicitConnection;
    CIP_ForwardOpen_Response_Success ForwardOpen_Response_Success;
    EIP_SessionStruct * p_SessionStruct;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_ForwardOpen_Request || !p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);   

    p_MessageRouterResponse = p_ErrorStruct->p_MessageRouterResponse;
    p_ExplicitConnection_Local = &ExplicitConnection_Local;

    local_init_ExplicitConnection(p_ExplicitConnection_Local);
    p_ExplicitConnection_Local->connection_is_used = RTA_TRUE;

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

    memcpy(&p_ExplicitConnection_Local->Connection_Triad, &p_ForwardOpen_Request->Connection_Triad, sizeof(p_ExplicitConnection_Local->Connection_Triad));

    rc = cip_obj_cnxnmgr_convert_network_to_Internal_NetworkConnectionParameters_ForwardOpen(p_ErrorStruct, p_ForwardOpen_Request->O2T.NetworkConnectionParameters, &p_ExplicitConnection_Local->O2T.InternalNetworkConnectionParameters);
    if(rc != CIP_RETURNCODE_SUCCESS) 
        CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_obj_cnxnmgr_convert_network_to_Internal_NetworkConnectionParameters_ForwardOpen(p_ErrorStruct, p_ForwardOpen_Request->T2O.NetworkConnectionParameters, &p_ExplicitConnection_Local->T2O.InternalNetworkConnectionParameters);
    if(rc != CIP_RETURNCODE_SUCCESS) 
        CIP_LOG_ERROR_AND_RETURN (rc);

    rc = cip_obj_cnxnmgr_convert_network_to_Internal_ConnectionTimeoutMultiplier(p_ErrorStruct, p_ForwardOpen_Request->ConnectionTimeoutMultiplier, &p_ExplicitConnection_Local->ConnectionTimeoutMultiplier);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    /* *************************************** */
    /* process the Forward Open request type   */
    /* *************************************** */

    /* CIP Volume 1 - 3-5.4.2.2 Null Forward Open */
    if ((p_ExplicitConnection_Local->O2T.InternalNetworkConnectionParameters.ConnectionType == ConnectionType_Null) || 
        (p_ExplicitConnection_Local->T2O.InternalNetworkConnectionParameters.ConnectionType == ConnectionType_Null) )
    {
        /* if non matching we could ping or configure */
        /* if matching we could re-configure */
        /* we don't support either for now */
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_NULL_FORWARD_OPEN_FUNCTION_NOT_SUPPORTED));
    }

    /* CIP Volume 1 - 3-5.4.2.1 Non-Null Forward Open */
    else
    {    
        p_MatchingExplicitConnection = cip_obj_cnxnmgr_explicit_find_connection_by_Connection_Triad(&p_ForwardOpen_Request->Connection_Triad);

        /* CIP Volume 1 - 3-5.4.2.1.2 Non-Null Forward Open, Matching - Error */
        if (p_MatchingExplicitConnection)
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_CONNECTION_IN_USE_OR_DUPLICATE_FORWARD_OPEN));
    }

    /* CIP Volume 1 - 3-5.4.2.1.1 Non-Null Forward Open, Not Matching - Open a Connection */

    /* ****************************** */
    /* validate the passed parameters */
    /* ****************************** */
    rc = validate_ExplicitConnection_O2T_InternalNetworkConnectionParameters(p_ErrorStruct, &p_ExplicitConnection_Local->O2T.InternalNetworkConnectionParameters);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    rc = validate_ExplicitConnection_T2O_InternalNetworkConnectionParameters(p_ErrorStruct, &p_ExplicitConnection_Local->T2O.InternalNetworkConnectionParameters);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    rc = validate_ExplicitConnection_ConnectionPath(p_ErrorStruct, &p_ForwardOpen_Request->Connection_Path);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    rc = cip_obj_cnxnmgr_validate_RPIs_are_in_range(p_ErrorStruct, p_ForwardOpen_Request->O2T.RPI_in_microseconds, p_ForwardOpen_Request->T2O.RPI_in_microseconds);
    if (rc == CIP_RETURNCODE_ABORTED_PROCESS)
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    if (rc != CIP_RETURNCODE_SUCCESS)
        CIP_LOG_ERROR_AND_RETURN(rc);

    /* **************** */
    /* Tie into session */
    /* **************** */
    p_SessionStruct = eip_encap_find_Session_from_SessionHandle(p_cip_ctrl->EIP_SessionHandle);
    if (!p_SessionStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);

    /* check if we already have a connection open for our session */
    if (p_SessionStruct->p_ExplicitConnection)
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_OUT_OF_CONNECTIONS));

    /* we are good to go, try to allocate a connection */    
    p_ExplicitConnection_New = local_find_free_explicit_connection();
    if (!p_ExplicitConnection_New)
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_OUT_OF_CONNECTIONS));
    
    /* NOTE: from this point forward all errors must free p_ExplicitConnection_New */

    rc = local_deepcopy_ExplicitConnection(p_ExplicitConnection_New, p_ExplicitConnection_Local);
    if (rc != CIP_RETURNCODE_SUCCESS)
    {
        cip_obj_cnxnmgr_free_ExplicitConnection(p_ExplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    p_ExplicitConnection_New->SessionHandle = p_SessionStruct->SessionHandle;
    if (rta_usersock_get_tcp_remote_ipaddr(p_SessionStruct->p_socket, &p_ExplicitConnection_New->OwningIPAddress) != RTA_USERSOCK_RETURNCODE_SUCCESS)
    {
        cip_obj_cnxnmgr_free_ExplicitConnection(p_ExplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    p_ExplicitConnection_New->O2T.API_in_milliseconds = (p_ForwardOpen_Request->O2T.RPI_in_microseconds / 1000UL);
    p_ExplicitConnection_New->T2O.API_in_milliseconds = (p_ForwardOpen_Request->T2O.RPI_in_microseconds / 1000UL);

    if ((p_ExplicitConnection_New->O2T.API_in_milliseconds == 0) || (p_ExplicitConnection_New->T2O.API_in_milliseconds == 0))
    {
        cip_obj_cnxnmgr_free_ExplicitConnection(p_ExplicitConnection_New);
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_RPI_NOTSUPPORTED));
    }

    rc = cip_obj_cnxnmgr_convert_API_and_Multiplier_to_milliseconds(p_ErrorStruct, p_ExplicitConnection_New->O2T.API_in_milliseconds, p_ExplicitConnection_New->ConnectionTimeoutMultiplier, &p_ExplicitConnection_New->Connection_Timeout_reload_ms);
    if (rc != CIP_RETURNCODE_SUCCESS)
    {
        cip_obj_cnxnmgr_free_ExplicitConnection(p_ExplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    /* "Targets shall wait at least 10 seconds after sending a successful response for the first packet on a connection." */
    if (p_ExplicitConnection_New->Connection_Timeout_reload_ms < CIP_OBJ_CNXNMGR_FIRST_TIME_RPI_IN_MILLISECONDS)
        p_ExplicitConnection_New->Connection_Timeout_remaining_ms = CIP_OBJ_CNXNMGR_FIRST_TIME_RPI_IN_MILLISECONDS;
    else
        p_ExplicitConnection_New->Connection_Timeout_remaining_ms = p_ExplicitConnection_New->Connection_Timeout_reload_ms;


    /* CIP Volume 3 EtherNet/IP - Table 3 - 3.2 Network Connection ID Selection */
    /*  Connection Type     Which Network Connection ID     Who chooses Connection ID
        ---------------     ---------------------------     -------------------------
        Point-to-point      Originator->Target              Target (us)
                            Target->Originator              Originator
    */
    p_ExplicitConnection_New->T2O.NetworkConnectionID = p_ForwardOpen_Request->T2O.NetworkConnectionID;
    p_ExplicitConnection_New->O2T.NetworkConnectionID = cip_obj_cnxnmgr_get_ConnectionID();
    
    /* build successful message */
    memset(&ForwardOpen_Response_Success, 0, sizeof(ForwardOpen_Response_Success));
    memcpy(&ForwardOpen_Response_Success.Connection_Triad, &p_ForwardOpen_Request->Connection_Triad, sizeof(ForwardOpen_Response_Success.Connection_Triad));
    ForwardOpen_Response_Success.O2T.NetworkConnectionID = p_ExplicitConnection_New->O2T.NetworkConnectionID;
    ForwardOpen_Response_Success.O2T.API_in_microseconds = (1000 * p_ExplicitConnection_New->O2T.API_in_milliseconds);
    ForwardOpen_Response_Success.T2O.NetworkConnectionID = p_ExplicitConnection_New->T2O.NetworkConnectionID;
    ForwardOpen_Response_Success.T2O.API_in_microseconds = (1000 * p_ExplicitConnection_New->T2O.API_in_milliseconds);
    ForwardOpen_Response_Success.Application.Reply_Size_In_Words = 0;

    rc = cip_process_write_response_data_with_CIP_ForwardOpen_Response_Success(p_cip_ctrl, p_MessageRouterResponse, &ForwardOpen_Response_Success);
    if(rc != CIP_RETURNCODE_SUCCESS) 
    {
        cip_obj_cnxnmgr_free_ExplicitConnection(p_ExplicitConnection_New);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    /* if everthing worked OK, set the session to point to this connection */
    p_SessionStruct->p_ExplicitConnection = p_ExplicitConnection_New;

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_obj_cnxnmgr_explicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose_Class3(CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardClose_Request const * const p_ForwardClose_Request,  CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_OBJ_ForwardClose_ErrorStruct ErrorStruct;
    CIP_OBJ_ForwardClose_ErrorStruct const * const p_ErrorStruct = &ErrorStruct; /* common calling convention */
    CIP_ExplicitConnection * p_ExplicitConnection;
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

    p_ExplicitConnection = cip_obj_cnxnmgr_explicit_find_connection_by_Connection_Triad(&p_ForwardClose_Request->Connection_Triad);
    if(!p_ExplicitConnection)
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    rc = validate_ExplicitConnection_ConnectionPath(p_ErrorStruct, &p_ForwardClose_Request->Connection_Path);
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
    if(socket_ip != p_ExplicitConnection->OwningIPAddress)
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Privilege_violation));

    /* build successful message */
    memset(&ForwardClose_Response_Success, 0, sizeof(ForwardClose_Response_Success));
    memcpy(&ForwardClose_Response_Success.Connection_Triad, &p_ForwardClose_Request->Connection_Triad, sizeof(ForwardClose_Response_Success.Connection_Triad));
    ForwardClose_Response_Success.Application.Reply_Size_In_Words = 0;

    rc = cip_process_write_response_data_with_CIP_ForwardClose_Response_Success(p_cip_ctrl, p_MessageRouterResponse, &ForwardClose_Response_Success);
    if (rc != CIP_RETURNCODE_SUCCESS)
    {
        cip_obj_cnxnmgr_free_ExplicitConnection(p_ExplicitConnection);
        CIP_LOG_ERROR_AND_RETURN(rc);
    }

    /* success so free the connection */
    cip_obj_cnxnmgr_free_ExplicitConnection(p_ExplicitConnection);
   
    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_obj_cnxnmgr_explicit_ip_still_open (CIP_UDINT const owning_ip_that_timed_out)
{
    RTA_Size_Type ii;

    for (ii = 0; ii < CIP_NUM_EXPLICIT_CONNECTIONS; ii++)
    {
        if(!ExplicitConnection[ii].connection_is_used)
            continue;

        if(ExplicitConnection[ii].OwningIPAddress == owning_ip_that_timed_out)
            return(RTA_TRUE);
    }

    return(RTA_FALSE);
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */

static CIP_ReturnCode validate_ExplicitConnection_ConnectionPath(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_Path_Array const * const p_Path)
{
    CIP_ReturnCode rc;
    CIP_UINT    ii;
    CIP_UDINT   class_id = 0;
    CIP_BOOL    class_id_found = RTA_FALSE;
    CIP_UDINT   instance_id = 0;
    CIP_BOOL    instance_id_found = RTA_FALSE;
    CIP_ElectronicKey electronic_key;
    CIP_BOOL    electronic_key_found = RTA_FALSE;
    CIP_Path_Element const * p_Path_Element;

    CIP_LOG_FUNCTION_ENTER;

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

        else
        {
            return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_SEGMENT_IN_CONNECTION_PATH));
        }
    }

    /* explicit message must point to the message router, instance 1 */
    if ((!class_id_found) || (class_id != CIP_Class_Code_Message_Router) || (!instance_id_found) || (instance_id != 1))
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_SEGMENT_IN_CONNECTION_PATH));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode validate_ExplicitConnection_O2T_InternalNetworkConnectionParameters(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_CNXNMGR_Internal_NetworkConnectionParameters const * const p_Internal_NetworkConnectionParameters)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct || !p_Internal_NetworkConnectionParameters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    if (p_Internal_NetworkConnectionParameters->RedundantOwner)
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_REDUNDANT_OWNER));

    if (p_Internal_NetworkConnectionParameters->ConnectionType != ConnectionType_PointToPoint)
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

    if (!p_Internal_NetworkConnectionParameters->isVariableSize)  /* Explicit must be variable size */
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_FIXVAR));

    if (p_Internal_NetworkConnectionParameters->ConnectionSize == 0)
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_SIZE));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode validate_ExplicitConnection_T2O_InternalNetworkConnectionParameters(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_CNXNMGR_Internal_NetworkConnectionParameters const * const p_Internal_NetworkConnectionParameters)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_ErrorStruct || !p_Internal_NetworkConnectionParameters)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    RTA_UNUSED_PARAM(p_Internal_NetworkConnectionParameters->RedundantOwner); /* T2O can't be owned so this value is don't care */

    if (p_Internal_NetworkConnectionParameters->ConnectionType != ConnectionType_PointToPoint)
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

    if (!p_Internal_NetworkConnectionParameters->isVariableSize)  /* Explicit must be variable size */
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_FIXVAR));

    if (p_Internal_NetworkConnectionParameters->ConnectionSize == 0)
        return(cip_obj_cnxnmgr_build_forwardopen_error(p_ErrorStruct, CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_SIZE));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static void local_init_ExplicitConnection (CIP_ExplicitConnection * const p_ExplicitConnection)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_ExplicitConnection)
        return;

    memset(p_ExplicitConnection, 0, sizeof(*p_ExplicitConnection));

    p_ExplicitConnection->connection_is_used = RTA_FALSE;

    p_ExplicitConnection->ConnectionTimeoutMultiplier = ConnectionTimeoutMultiplier_Unknown;

    p_ExplicitConnection->Last_message_received = RTA_FALSE;

    /* TODO: if these blocks are called more than once make a function */
    p_ExplicitConnection->O2T.InternalNetworkConnectionParameters.ConnectionType = ConnectionType_Unknown;
    p_ExplicitConnection->O2T.InternalNetworkConnectionParameters.isVariableSize = RTA_FALSE;
    p_ExplicitConnection->O2T.InternalNetworkConnectionParameters.Priority = Priority_Unknown;
    p_ExplicitConnection->O2T.InternalNetworkConnectionParameters.RedundantOwner = RTA_FALSE;

    p_ExplicitConnection->T2O.InternalNetworkConnectionParameters.ConnectionType = ConnectionType_Unknown;
    p_ExplicitConnection->T2O.InternalNetworkConnectionParameters.isVariableSize = RTA_FALSE;
    p_ExplicitConnection->T2O.InternalNetworkConnectionParameters.Priority = Priority_Unknown;
    p_ExplicitConnection->T2O.InternalNetworkConnectionParameters.RedundantOwner = RTA_FALSE;

    CIP_LOG_FUNCTION_EXIT;
}

static CIP_ExplicitConnection * local_find_free_explicit_connection (void)
{
    RTA_Size_Type ii;
    
    for(ii=0; ii<CIP_NUM_EXPLICIT_CONNECTIONS; ii++)
    {
        if(ExplicitConnection[ii].connection_is_used)
            continue;

        ExplicitConnection[ii].connection_is_used = RTA_TRUE;

        return(&ExplicitConnection[ii]);
    }    

    return(RTA_NULL);
}

static CIP_ReturnCode local_deepcopy_ExplicitConnection(CIP_ExplicitConnection * const p_dst_ExplicitConnection, CIP_ExplicitConnection const * const p_src_ExplicitConnection)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_dst_ExplicitConnection || !p_src_ExplicitConnection)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    memcpy(p_dst_ExplicitConnection, p_src_ExplicitConnection, sizeof(*p_dst_ExplicitConnection));

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* *********** */
/* END OF FILE */
/* *********** */
