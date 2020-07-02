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

#ifndef __EIP_PROCESS_H__
#define __EIP_PROCESS_H__

CIP_ReturnCode eip_process_EIP_Encap_ListServices(EIP_ControlStruct const * const p_cip_ctrl, EIP_Encap_ListServices_Request const * const p_Encap_ListServices_Request, EIP_Encap_ListServices_Response * const p_Encap_ListServices_Response);
CIP_ReturnCode eip_process_EIP_Encap_ListIdentity (EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_ListIdentity_Request const * const p_Encap_ListIdentity_Request, EIP_Encap_ListIdentity_Response * const p_Encap_ListIdentity_Response);
CIP_ReturnCode eip_process_EIP_Encap_RegisterSession (EIP_ControlStruct const * const p_cip_ctrl, EIP_Encap_RegisterSession_Request const * const p_Encap_RegisterSession_Request, EIP_Encap_RegisterSession_Response * const p_Encap_RegisterSession_Response);
CIP_ReturnCode eip_process_EIP_Encap_UnRegisterSession(EIP_ControlStruct const * const p_cip_ctrl, EIP_Encap_UnRegisterSession_Request const * const p_Encap_UnRegisterSession_Request, EIP_Encap_UnRegisterSession_Response const * const p_Encap_UnRegisterSession_Response);
CIP_ReturnCode eip_process_EIP_Encap_SendRRData(EIP_ControlStruct * const p_cip_ctrl, EIP_Encap_SendRRData_Request const * const p_Encap_SendRRData_Request, EIP_Encap_SendRRData_Response * const p_Encap_SendRRData_Response);
CIP_ReturnCode eip_process_EIP_Encap_SendUnitData(EIP_ControlStruct * const p_cip_ctrl, EIP_Encap_SendUnitData_Request const * const p_Encap_SendUnitData_Request, EIP_Encap_SendUnitData_Response * const p_Encap_SendUnitData_Response);

#endif /* __EIP_PROCESS_H__ */
