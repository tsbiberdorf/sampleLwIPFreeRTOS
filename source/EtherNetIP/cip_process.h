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

#ifndef __CIP_PROCESS_H__
#define __CIP_PROCESS_H__

typedef struct
{
    CIP_UDINT       class_id;
    CIP_BOOL        (* fptr_is_instance_supported) (CIP_CIAM_Path const * const p_path);
    CIP_ReturnCode  (* fptr_process_service) (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
    CIP_BOOL        (* fptr_is_service_supported_for_instance) (CIP_USINT const service, CIP_UDINT const instance);
}CIP_PROCESS_OBJECT_ENTRY;

CIP_ReturnCode cip_process_message (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
CIP_ReturnCode cip_process_build_error (CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_USINT const error);

CIP_BOOL cip_process_CIP_Path_Element_is_a_class_id(CIP_Path_Element const * const p_Path_Element);
CIP_ReturnCode cip_process_get_class_id_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_class_id);
CIP_BOOL cip_process_CIP_Path_Element_is_a_instance_id(CIP_Path_Element const * const p_Path_Element);
CIP_ReturnCode cip_process_get_instance_id_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_instance_id);
CIP_BOOL cip_process_CIP_Path_Element_is_a_attribute_id(CIP_Path_Element const * const p_Path_Element);
CIP_ReturnCode cip_process_get_attribute_id_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_attribute_id);
CIP_BOOL cip_process_CIP_Path_Element_is_a_member_id(CIP_Path_Element const * const p_Path_Element);
CIP_ReturnCode cip_process_get_member_id_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_member_id);
CIP_BOOL cip_process_CIP_Path_Element_is_a_connection_point(CIP_Path_Element const * const p_Path_Element);
CIP_ReturnCode cip_process_get_connection_point_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_UDINT * const p_member_id);
CIP_BOOL cip_process_CIP_Path_Element_is_an_electronic_key(CIP_Path_Element const * const p_Path_Element);
CIP_ReturnCode cip_process_get_electronic_key_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_ElectronicKey * const p_ElectronicKey);
CIP_BOOL cip_process_CIP_Path_Element_is_a_simple_data_segment(CIP_Path_Element const * const p_Path_Element);
CIP_ReturnCode cip_process_get_simple_data_segment_from_CIP_Path_Element(CIP_Path_Element const * const p_Path_Element, CIP_DataSegment_SimpleDataSegment * const p_DataSegment_SimpleDataSegment);

CIP_ReturnCode cip_process_convert_CIP_Path_Array_to_CIAM_Path (CIP_Path_Array const * const p_Path, CIP_CIAM_Path * const p_CIAM_Path);

CIP_ReturnCode cip_process_write_response_data_with_CIP_BOOL(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_BOOL const * const p_BOOL);
CIP_ReturnCode cip_process_write_response_data_with_CIP_SINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_SINT const * const p_SINT);
CIP_ReturnCode cip_process_write_response_data_with_CIP_INT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_INT const * const p_INT);
CIP_ReturnCode cip_process_write_response_data_with_CIP_DINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_DINT const * const p_CIP_DINT);
CIP_ReturnCode cip_process_write_response_data_with_CIP_LINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_LINT const * const p_CIP_LINT);
CIP_ReturnCode cip_process_write_response_data_with_CIP_USINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_USINT const * const p_USINT);
CIP_ReturnCode cip_process_write_response_data_with_CIP_UINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_UINT const * const p_UINT);
CIP_ReturnCode cip_process_write_response_data_with_CIP_UDINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_UDINT const * const p_UDINT);
CIP_ReturnCode cip_process_write_response_data_with_CIP_ULINT(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ULINT const * const p_ULINT);
CIP_ReturnCode cip_process_write_response_data_with_CIP_REAL(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_REAL const * const p_REAL);
CIP_ReturnCode cip_process_write_response_data_with_CIP_LREAL(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_LREAL const * const p_LREAL);
CIP_ReturnCode cip_process_write_response_data_with_CIP_BYTE(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_BYTE const * const p_BYTE);
CIP_ReturnCode cip_process_write_response_data_with_CIP_WORD(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_WORD const * const p_WORD);
CIP_ReturnCode cip_process_write_response_data_with_CIP_DWORD(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_DWORD const * const p_DWORD);
CIP_ReturnCode cip_process_write_response_data_with_CIP_LWORD(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_LWORD const * const p_LWORD);
CIP_ReturnCode cip_process_write_response_data_with_CIP_Opaque_String(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_Opaque_String const * const p_Opaque_String);
CIP_ReturnCode cip_process_write_response_data_with_CIP_SHORT_STRING_32(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_SHORT_STRING_32 const * const p_SHORT_STRING_32);

CIP_ReturnCode cip_process_write_response_data_with_CIP_DATE_AND_TIME(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_DATE_AND_TIME const * const p_CIP_DATE_AND_TIME);

CIP_ReturnCode cip_process_write_response_data_with_CIP_IdentityObject_Revision(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_IdentityObject_Revision const * const p_IdentityObject_Revision);
CIP_ReturnCode cip_process_write_response_data_with_CIP_IdentityObject(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_IdentityObject const * const p_IdentityObject);

CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_Status(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_Status const * const p_TCPObject_Status);
CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_ConfigurationCapability(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_ConfigurationCapability const * const p_TCPObject_ConfigurationCapability);
CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_ConfigurationControl(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_ConfigurationControl const * const p_TCPObject_ConfigurationControl);
CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_PhysicalLinkObject(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_PhysicalLinkObject const * const p_TCPObject_PhysicalLinkObject);
CIP_ReturnCode cip_process_write_response_data_with_EIP_TCPObject_InterfaceConfiguration(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_TCPObject_InterfaceConfiguration const * const p_TCPObject_InterfaceConfiguration);
CIP_ReturnCode cip_process_write_response_data_with_CIP_STRING64_16BIT_PADDED(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_STRING const * const p_STRING);

CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceFlags_Internal(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_InterfaceFlags_Internal const * const p_EthernetLinkObject_InterfaceFlags_Internal);
CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_PhysicalAddress(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_PhysicalAddress const * const p_EthernetLinkObject_PhysicalAddress);
CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceCounters(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_InterfaceCounters const * const p_EthernetLinkObject_InterfaceCounters);
CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_MediaCounters(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_MediaCounters const * const p_EthernetLinkObject_MediaCounters);
CIP_ReturnCode cip_process_write_response_data_with_EIP_EthernetLinkObject_InterfaceCapability(CIP_MessageRouterResponse * const p_MessageRouterResponse, EIP_EthernetLinkObject_InterfaceCapability const * const p_EthernetLinkObject_InterfaceCapability);

CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardOpen_Request(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardOpen_Request const * const p_ForwardOpen_Request);
CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardOpen_Response_Success(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardOpen_Response_Success const * const p_ForwardOpen_Response_Success);
CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardOpen_Response_Unsuccessful(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardOpen_Response_Unsuccessful const * const p_ForwardOpen_Response_Unsuccessful);

CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardClose_Request(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardClose_Request const * const p_ForwardClose_Request);
CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardClose_Response_Success(CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardClose_Response_Success const * const p_ForwardClose_Response_Success);
CIP_ReturnCode cip_process_write_response_data_with_CIP_ForwardClose_Response_Unsuccessful(CIP_MessageRouterResponse * const p_MessageRouterResponse, CIP_ForwardClose_Response_Unsuccessful const * const p_ForwardClose_Response_Unsuccessful);

#endif /* __CIP_PROCESS_H__ */
