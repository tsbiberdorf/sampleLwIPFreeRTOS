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

#ifndef __CIP_DECODE_H__
#define __CIP_DECODE_H__

/* call this function if decode finds a decode error (like too much data, not enought data, path error) */
CIP_ReturnCode cip_decode_store_and_return_general_status(CIP_ControlStruct * const p_cip_ctrl, CIP_USINT const general_status);

CIP_ReturnCode cip_decode_CIP_MessageRouterRequest (CIP_ControlStruct * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest);
CIP_ReturnCode cip_decode_CIP_Path_Array(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const path_size_in_words, CIP_Path_Array * const p_Path_Array);
CIP_ReturnCode cip_decode_CIP_STRING(CIP_ControlStruct const * const p_cip_ctrl, CIP_STRING * const p_STRING);

CIP_ReturnCode cip_decode_CIP_Opaque_String (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_Opaque_String * const p_Opaque_String);
CIP_ReturnCode cip_decode_CIP_ForwardOpen_Request (CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardOpen_Request * const p_ForwardOpen_Request);

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_BOOL(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_UINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_SINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_INT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_DINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_REAL(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_STRING(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_SHORT_STRING_32(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_ArrayOf_CIP_REAL(CIP_ControlStruct const * const p_cip_ctrl, RTA_Size_Type const count, CIP_MessageRouter_Data * const p_MessageRouter_Data);

CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_Opaque_String(CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_ForwardOpen_Request (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_CIP_ForwardClose_Request(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_EIP_TCPObject_ConfigurationControl(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);
CIP_ReturnCode cip_decode_CIP_MessageRouter_Data_of_type_EIP_TCPObject_InterfaceConfiguration(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouter_Data * const p_MessageRouter_Data);

#endif /* __CIP_DECODE_H__ */
