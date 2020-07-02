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

#ifndef __CIP_OBJ_CNXNMGR_IMPLICIT_H__
#define __CIP_OBJ_CNXNMGR_IMPLICIT_H__

/* CIP Volume 1 -3-6.1.4 32-Bit Header Format */
#define CIP_CNXNMGR_IMPLICIT_32Bit_Header_Running   0x0001  /* we only support this single bit, so if it is set, we are running */

/* TODO: find common fields with Implicit and Explicit and move out of here into common struct from cip_obj_cnxnmgr.h */
typedef struct
{
    CIP_UDINT   AssemblyInstance;
    CIP_UDINT   NetworkConnectionID;
    CIP_UDINT   API_in_milliseconds;
    CIP_CNXNMGR_Internal_NetworkConnectionParameters    InternalNetworkConnectionParameters;
    CIP_UDINT   ConnectedData_SequenceNumber;

    CIP_UDINT   UDP_IPAddress;
    CIP_UINT    UDP_PortID;
    CIP_UINT    PDU_SequenceNumber; 
    CIP_BOOL    OneOrMorePacketsTransferred;

    /* keep track of who opened the connection... since they are the only one that can close it */
    CIP_UDINT   OwningIPAddress;
}CIP_CNXNMGR_IOConnection_Cnxn_PerDirection;

/* TODO: find common fields with Implicit and Explicit and move out of here into common struct from cip_obj_cnxnmgr.h */
typedef struct
{
    CIP_BOOL    connection_is_used;

    CIP_Connection_Triad Connection_Triad;
    CIP_CNXNMGR_Internal_ConnectionTimeoutMultiplier    ConnectionTimeoutMultiplier;

    CIP_CNXNMGR_IOConnection_Cnxn_PerDirection   O2T;
    CIP_CNXNMGR_IOConnection_Cnxn_PerDirection   T2O;

    /* keep track of who opened the connection... since they are the only one that can close it */
    CIP_UDINT   OwningIPAddress;

    CIP_UDINT   Connection_Timeout_reload_ms;
    CIP_UDINT   Connection_Timeout_remaining_ms;

    CIP_UDINT   SendNeeded_Timeout_reload_ms;
    CIP_UDINT   SendNeeded_Timeout_remaining_ms;
}CIP_ImplicitConnection;

void cip_obj_cnxnmgr_free_ImplicitConnection(CIP_ImplicitConnection * const p_ImplicitConnection);
void cip_obj_cnxnmgr_ImplicitConnection_received(CIP_ImplicitConnection * const p_ImplicitConnection);
CIP_ImplicitConnection * cip_obj_cnxnmgr_implicit_find_connection_by_Connection_Triad(CIP_Connection_Triad const * const p_Connection_Triad);
CIP_ReturnCode cip_obj_cnxnmgr_implicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardOpen_Class1 (EIP_ControlStruct const * const p_eip_ctrl, CIP_ForwardOpen_Request const * const p_ForwardOpen_Request, CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct);
CIP_ReturnCode cip_obj_cnxnmgr_implicit_CIP_OBJ_CNXNMGR_ServiceCode_ForwardClose_Class1 (CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardClose_Request const * const p_ForwardClose_Request, CIP_MessageRouterResponse * const p_MessageRouterResponse);
CIP_ReturnCode cip_obj_cnxnmgr_implicit_process_message (EIP_ControlStruct * const p_eip_ctrl);

void cip_obj_cnxnmgr_implicit_new_T2O_data (CIP_DWORD const instance);
CIP_BOOL cip_obj_cnxnmgr_implicit_ip_still_open (CIP_UDINT const owning_ip_that_timed_out);

CIP_BOOL cip_obj_cnxnmgr_implicit_connections_established(void);
CIP_BOOL cip_obj_cnxnmgr_implicit_o2t_assembly_owned(CIP_UDINT const instance);

#endif /* __CIP_OBJ_CNXNMGR_IMPLICIT_H__ */
