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

#ifndef __CIP_OBJ_CNXNMGR_H__
#define __CIP_OBJ_CNXNMGR_H__

/* CIP Volume 2 - 3-4.1 CIP Transport Class 0 and Class 1 Packet Ordering */
#define CIP_32BIT_SEQ_LT(a,b)   ((CIP_DINT)((a)-(b)) < 0)
#define CIP_32BIT_SEQ_LEQ(a,b)  ((CIP_DINT)((a)-(b)) <= 0)
#define CIP_32BIT_SEQ_GT(a,b)   ((CIP_DINT)((a)-(b)) > 0)
#define CIP_32BIT_SEQ_GEQ(a,b)  ((CIP_DINT)((a)-(b)) >= 0)

typedef enum
{
    ProductionTrigger_Cyclic,
    ProductionTrigger_ChangeOfState,
    ProductionTrigger_ApplicationObject,
    ProductionTrigger_Unknown
}CIP_CNXNMGR_ProductionTrigger;

typedef enum
{
    CIP_CNXNMGR_TransportClass_0,
    CIP_CNXNMGR_TransportClass_1,
    CIP_CNXNMGR_TransportClass_2,
    CIP_CNXNMGR_TransportClass_3,
    CIP_CNXNMGR_TransportClass_Unknown
}CIP_CNXNMGR_TransportClass;

typedef struct
{
    CIP_BOOL Direction_isServer;
    CIP_CNXNMGR_ProductionTrigger ProductionTrigger;
    CIP_CNXNMGR_TransportClass TransportClass;
}CIP_CNXNMGR_Internal_TransportClass_and_Trigger;

typedef enum
{
    ConnectionType_Null,
    ConnectionType_Multicast,
    ConnectionType_PointToPoint,
    ConnectionType_Reserved,
    ConnectionType_Unknown
}CIP_CNXNMGR_Internal_ConnectionType;

typedef enum
{
    Priority_Low,
    Priority_High,
    Priority_Scheduled,
    Priority_Urgent,
    Priority_Unknown  
}CIP_CNXNMGR_Internal_Priority;

/* CIP Volume 1 (Common Industrial Protocol) - Table 3-5.8 Network Connection Parameters for Forward_Open */
/* CIP Volume 1 (Common Industrial Protocol) - Table 3-5.9 Network Connection Parameters for Large_Forward_Open */
typedef struct
{
    CIP_BOOL                            RedundantOwner;
    CIP_CNXNMGR_Internal_ConnectionType ConnectionType;
    CIP_CNXNMGR_Internal_Priority       Priority;
    CIP_BOOL                            isVariableSize;
    CIP_UINT                            ConnectionSize;
}CIP_CNXNMGR_Internal_NetworkConnectionParameters;

/* CIP Volume 1 (Common Industrial Protocol) - 3-5.4.1.4 Connection Timeout Multiplier */
typedef enum
{
    ConnectionTimeoutMultiplier_4X,
    ConnectionTimeoutMultiplier_8X,
    ConnectionTimeoutMultiplier_16X,
    ConnectionTimeoutMultiplier_32X,
    ConnectionTimeoutMultiplier_64X,
    ConnectionTimeoutMultiplier_128X,
    ConnectionTimeoutMultiplier_256X,
    ConnectionTimeoutMultiplier_512X,
    ConnectionTimeoutMultiplier_Unknown
}CIP_CNXNMGR_Internal_ConnectionTimeoutMultiplier;

typedef struct
{
    CIP_ControlStruct const * p_cip_ctrl;
    CIP_Connection_Triad const * p_Connection_Triad;
    CIP_MessageRouterResponse * p_MessageRouterResponse;
}CIP_OBJ_ForwardOpenClose_ErrorStruct;
typedef CIP_OBJ_ForwardOpenClose_ErrorStruct CIP_OBJ_ForwardOpen_ErrorStruct;
typedef CIP_OBJ_ForwardOpenClose_ErrorStruct CIP_OBJ_ForwardClose_ErrorStruct;

/* TODO: Move to a user file */ 
#define CIP_OBJ_CNXNMGR_MIN_RPI_IN_MICROSECONDS (10*1000UL)         /* 10 milliseconds (don't go below 1000 microseconds) */
#define CIP_OBJ_CNXNMGR_MAX_RPI_IN_MICROSECONDS (60*1000*1000UL)    /* 60 seconds */
#define CIP_OBJ_CNXNMGR_FIRST_TIME_RPI_IN_MILLISECONDS  (10*1000UL) /* 10 seconds */

CIP_ReturnCode cip_obj_cnxnmgr_init (CIP_InitType const init_type);
CIP_ReturnCode cip_obj_cnxnmgr_process (RTA_U32 const ms_since_last_call);
CIP_ReturnCode cip_obj_cnxnmgr_decode (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest);

CIP_BOOL cip_obj_cnxnmgr_is_instance_supported (CIP_CIAM_Path const * const p_path);
CIP_ReturnCode cip_obj_cnxnmgr_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
CIP_BOOL cip_obj_cnxnmgr_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance);

/* common for Explicit and Implicit functions */
CIP_ReturnCode cip_obj_cnxnmgr_build_forwardopen_error(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UINT const extended_status);
CIP_ReturnCode cip_obj_cnxnmgr_build_forwardopen_error_multiple_extended_status (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_USINT NumberofStatuses, CIP_UINT const * const p_Statuses);
CIP_ReturnCode cip_obj_cnxnmgr_build_forwardclose_error(CIP_OBJ_ForwardClose_ErrorStruct const * const p_ErrorStruct, CIP_UINT const extended_status);
CIP_ReturnCode cip_obj_cnxnmgr_convert_network_to_Internal_TransportClass_and_Trigger (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_BYTE const network_TransportClass_and_Trigger, CIP_CNXNMGR_Internal_TransportClass_and_Trigger * const p_Internal_TransportClass_and_Trigger);
/* in the future we may support the LargeForwardOpen which has NetworkConnectionParameters different than the ForwardOpen so name for that future enhancement */
CIP_ReturnCode cip_obj_cnxnmgr_convert_network_to_Internal_NetworkConnectionParameters_ForwardOpen (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UINT const network_NetworkConnectionParameters, CIP_CNXNMGR_Internal_NetworkConnectionParameters * const p_Internal_NetworkConnectionParameters);
CIP_ReturnCode cip_obj_cnxnmgr_convert_network_to_Internal_ConnectionTimeoutMultiplier (CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_USINT const network_ConnectionTimeoutMultiplier, CIP_CNXNMGR_Internal_ConnectionTimeoutMultiplier * const p_Internal_ConnectionTimeoutMultiplier);
CIP_ReturnCode cip_obj_cnxnmgr_convert_API_and_Multiplier_to_milliseconds(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const API_in_milliseconds, CIP_CNXNMGR_Internal_ConnectionTimeoutMultiplier const ConnectionTimeoutMultiplier, CIP_UDINT * const p_milliseconds);
CIP_ReturnCode cip_obj_cnxnmgr_validate_RPIs_are_in_range(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_UDINT const O2T_RPI_in_microseconds, CIP_UDINT const T2O_RPI_in_microseconds);
CIP_ReturnCode cip_obj_cnxnmgr_validate_ElectronicKey(CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct, CIP_ElectronicKey const * const p_ElectronicKey);
CIP_UDINT cip_obj_cnxnmgr_get_ConnectionID (void);

void cip_obj_cnxnmgr_timeout_common (CIP_UDINT const owning_ip_that_timed_out);

CIP_ReturnCode cip_obj_cnxnmgr_explicit_init (CIP_InitType const init_type);
CIP_ReturnCode cip_obj_cnxnmgr_explicit_process (RTA_U32 const ms_since_last_call);

CIP_ReturnCode cip_obj_cnxnmgr_implicit_init (CIP_InitType const init_type);
CIP_ReturnCode cip_obj_cnxnmgr_implicit_process (RTA_U32 const ms_since_last_call);

#endif /* __CIP_OBJ_CNXNMGR_H__ */
