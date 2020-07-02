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

#ifndef __CIP_OBJ_CNXNMGR_EXPLICIT_H__
#define __CIP_OBJ_CNXNMGR_EXPLICIT_H__

/* Our internal representation is based on the ForwardOpen necessary timers */
/* CIP Volume 1 (Common Industrial Protocol) - Table 3-5.17 Forward_Open Request */

/* TODO: find common fields with Implicit and Explicit and move out of here into common struct from cip_obj_cnxnmgr.h */
typedef struct
{
    CIP_UDINT   NetworkConnectionID;
    CIP_UDINT   API_in_milliseconds;
    CIP_CNXNMGR_Internal_NetworkConnectionParameters    InternalNetworkConnectionParameters;
    CIP_UDINT   ConnectedData_SequenceNumber;
}CIP_CNXNMGR_ExplicitConnection_Cnxn_PerDirection;

/* TODO: find common fields with Implicit and Explicit and move out of here into common struct from cip_obj_cnxnmgr.h */
typedef struct
{  
    CIP_BOOL    connection_is_used;

    CIP_Connection_Triad Connection_Triad;
    CIP_CNXNMGR_Internal_ConnectionTimeoutMultiplier    ConnectionTimeoutMultiplier;
    CIP_CNXNMGR_ExplicitConnection_Cnxn_PerDirection   O2T;
    CIP_CNXNMGR_ExplicitConnection_Cnxn_PerDirection   T2O;
    
    /* keep track of who opened the connection... since they are the only one that can close it */
    CIP_UDINT   OwningIPAddress;
    
    /* keep track of session handle so we can close session on timeout */
    CIP_UDINT   SessionHandle;

    CIP_UDINT   Connection_Timeout_reload_ms;
    CIP_UDINT   Connection_Timeout_remaining_ms;

    /* CIP Volume 1, Figure 3-4.5 Server Transport Class 3 Behavior */
    /* If the sequence count is the same as the previous message, notify the Link Producer to produce the previous response including the same sequence count. */
    CIP_BOOL    Last_message_received;
    CIP_UINT    Last_PDU_SequenceNumber; 
    CIP_OCTET   Last_Message_Dummy_Heap[RTA_USER_MEMORY_NUM_BYTES_PER_HEAP]; /* TODO: what is a more meaningful number? */
    CIP_MessageRouterResponse Last_MessageRouterResponse;
}CIP_ExplicitConnection;

void cip_obj_cnxnmgr_free_ExplicitConnection(CIP_ExplicitConnection * const p_ExplicitConnection);
void cip_obj_cnxnmgr_ExplicitConnection_received(CIP_ExplicitConnection * const p_ExplicitConnection);
CIP_ExplicitConnection * cip_obj_cnxnmgr_explicit_find_connection_by_Connection_Triad(CIP_Connection_Triad const * const p_Connection_Triad);
CIP_ReturnCode cip_obj_cnxnmgr_explicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen_Class3 (CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardOpen_Request const * const p_ForwardOpen_Request, CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct);
CIP_ReturnCode cip_obj_cnxnmgr_explicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose_Class3(CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardClose_Request const * const p_ForwardClose_Request, CIP_MessageRouterResponse * const p_MessageRouterResponse);
CIP_BOOL cip_obj_cnxnmgr_explicit_ip_still_open (CIP_UDINT const owning_ip_that_timed_out);

#endif /* __CIP_OBJ_CNXNMGR_EXPLICIT_H__ */
