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

 #ifndef __CIP_USEROBJ_ASSEMBLY_H__
 #define __CIP_USEROBJ_ASSEMBLY_H__

CIP_ReturnCode cip_userobj_assembly_init (CIP_InitType const init_type);
CIP_ReturnCode cip_userobj_assembly_process (RTA_U32 const ms_since_last_call);

CIP_BOOL cip_userobj_assembly_is_instance_supported (CIP_CIAM_Path const * const p_path);
CIP_ReturnCode cip_userobj_assembly_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
CIP_BOOL cip_userobj_assembly_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance);

CIP_UINT cip_userobj_assembly_get_size_for_instance (CIP_UDINT const instance);
CIP_ReturnCode cip_userobj_assembly_read_data_for_instance (CIP_UDINT const instance, CIP_Opaque_String const * const p_Opaque_String);
CIP_ReturnCode cip_userobj_assembly_write_data_for_instance (CIP_UDINT const instance, CIP_Opaque_String const * const p_Opaque_String);

void cip_userobj_assembly_idle_received_for_o2t_instance(CIP_UDINT const instance);
void cip_userobj_assembly_timeout_received_for_o2t_instance(CIP_UDINT const instance);
void cip_userobj_assembly_close_received_for_o2t_instance(CIP_UDINT const instance);

CIP_BOOL cip_userobj_assembly_is_an_o2t_instance (CIP_UDINT const instance);
CIP_BOOL cip_userobj_assembly_is_an_o2t_heartbeat (CIP_UDINT const instance);

CIP_BOOL cip_userobj_assembly_is_an_t2o_instance (CIP_UDINT const instance);
CIP_BOOL cip_userobj_assembly_is_an_t2o_heartbeat (CIP_UDINT const instance);

CIP_ReturnCode cip_userobj_assembly_validate_ConfigurationData(CIP_UDINT const instance, CIP_DataSegment_SimpleDataSegment * const p_DataSegment_SimpleDataSegment, CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct);
#endif /* __CIP_USEROBJ_ASSEMBLY_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
