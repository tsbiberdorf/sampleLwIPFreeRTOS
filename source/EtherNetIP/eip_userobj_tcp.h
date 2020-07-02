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

 #ifndef __CIP_USEROBJ_TCP_H__
 #define __CIP_USEROBJ_TCP_H__

CIP_ReturnCode eip_userobj_tcp_init (CIP_InitType const init_type);
CIP_ReturnCode eip_userobj_tcp_process (RTA_U32 const ms_since_last_call);
CIP_BOOL eip_userobj_tcp_is_instance_supported (CIP_CIAM_Path const * const p_path);
CIP_ReturnCode eip_userobj_tcp_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
CIP_BOOL eip_userobj_tcp_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance);
CIP_ReturnCode eip_userobj_tcp_decode (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest);

CIP_ReturnCode eip_userobj_tcp_get_EIP_TCPObject_InterfaceConfiguration (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_InterfaceConfiguration * const p_InterfaceConfiguration);
CIP_UINT eip_userobj_tcp_get_EncapsulationInactivityTimeout_in_seconds (void);

#endif /* __CIP_USEROBJ_TCP_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
