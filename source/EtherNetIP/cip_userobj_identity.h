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

 #ifndef __CIP_USEROBJ_IDENTITY_H__
 #define __CIP_USEROBJ_IDENTITY_H__

CIP_ReturnCode cip_userobj_identity_init (CIP_InitType const init_type);
CIP_ReturnCode cip_userobj_identity_process (RTA_U32 const ms_since_last_call);
CIP_ReturnCode cip_userobj_identity_get_CIP_IdentityObject (CIP_ControlStruct const * const p_cip_ctrl, CIP_IdentityObject * const p_IdentityObject);

CIP_BOOL cip_userobj_identity_is_instance_supported (CIP_CIAM_Path const * const p_path);
CIP_ReturnCode cip_userobj_identity_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
CIP_BOOL cip_userobj_identity_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance);

#endif /* __CIP_USEROBJ_IDENTITY_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
