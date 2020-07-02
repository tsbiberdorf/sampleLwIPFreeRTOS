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

#ifndef __EIP_DECODE_H__
#define __EIP_DECODE_H__

CIP_ReturnCode eip_decode_EIP_Encap_Header(EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_Header * const p_Encap_Header);
CIP_ReturnCode eip_decode_EIP_Encap_RegisterSession_Request(EIP_ControlStruct const * const p_eip_ctrl, EIP_Encap_RegisterSession_Request * const p_Encap_RegisterSession_Request);
CIP_ReturnCode eip_decode_EIP_Encap_SendRRData_Request(EIP_ControlStruct * const p_eip_ctrl, EIP_Encap_SendRRData_Request * const p_Encap_SendRRData_Request);
CIP_ReturnCode eip_decode_EIP_Encap_SendUnitData_Request(EIP_ControlStruct * const p_eip_ctrl, EIP_Encap_SendUnitData_Request * const p_Encap_SendUnitData_Request);

CIP_ReturnCode eip_decode_EIP_SocketAddress (EIP_ControlStruct const * const p_eip_ctrl, EIP_SocketAddress * const p_SocketAddress);

CIP_ReturnCode eip_decode_EIP_Item_Explicit (EIP_ControlStruct * const p_eip_ctrl, EIP_Item_Explicit * const p_EIP_Item);
CIP_ReturnCode eip_decode_EIP_Item_Array_Explicit (EIP_ControlStruct * const p_eip_ctrl, EIP_Item_Array_Explicit * const p_EIP_Item);

CIP_ReturnCode eip_decode_EIP_Item_Array_Implicit (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_Array_Implicit * const p_Item_Array_Implicit);
CIP_ReturnCode eip_decode_EIP_Item_Implicit (EIP_ControlStruct const * const p_eip_ctrl, EIP_Item_Implicit * const p_Item_Implicit);

CIP_ReturnCode eip_decode_EIP_Item_ConnectedAddress(EIP_ControlStruct const * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_ConnectedAddress * const p_Item_ConnectedAddress);
CIP_ReturnCode eip_decode_EIP_Item_UnconnectedData (EIP_ControlStruct * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_UnconnectedData * const p_Item_UnconnectedData);
CIP_ReturnCode eip_decode_EIP_Item_ConnectedData(EIP_ControlStruct * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_ConnectedData * const p_Item_ConnectedData);
CIP_ReturnCode eip_decode_EIP_Item_Sockaddr_Info(EIP_ControlStruct const * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_Sockaddr_Info * const p_Item_Sockaddr_Info);
CIP_ReturnCode eip_decode_EIP_Item_SequencedAddress(EIP_ControlStruct const * const p_eip_ctrl, CIP_UINT const expected_length, EIP_Item_SequencedAddress * const p_Item_SequencedAddress);

#endif /* __EIP_DECODE_H__ */
