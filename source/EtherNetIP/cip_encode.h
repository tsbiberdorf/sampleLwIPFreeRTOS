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

#ifndef __CIP_ENCODE_H__
#define __CIP_ENCODE_H__

CIP_ReturnCode cip_encode_CIP_MessageRouterResponse (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterResponse const * const p_MessageRouterResponse);
CIP_ReturnCode cip_encode_CIP_IdentityObject_Revision (CIP_ControlStruct const * const p_cip_ctrl, CIP_IdentityObject_Revision const * const p_IdentityObject_Revision);
CIP_ReturnCode cip_encode_CIP_SHORT_STRING_32 (CIP_ControlStruct const * const p_cip_ctrl, CIP_SHORT_STRING_32 const * const p_SHORT_STRING_32);
CIP_ReturnCode cip_encode_CIP_IdentityObject (CIP_ControlStruct const * const p_cip_ctrl, CIP_IdentityObject const * const p_IdentityObject);
CIP_ReturnCode cip_encode_CIP_Opaque_String (CIP_ControlStruct const * const p_cip_ctrl, CIP_Opaque_String const * const p_Opaque_String);

CIP_ReturnCode cip_encode_CIP_DATE_AND_TIME(CIP_ControlStruct const * const p_cip_ctrl, CIP_DATE_AND_TIME const * const p_DATE_AND_TIME);

CIP_ReturnCode cip_encode_CIP_STRING_as_MaxNumCharacters_16bitPadded (CIP_ControlStruct const * const p_cip_ctrl, CIP_STRING const * const p_STRING, RTA_Size_Type const MaxNumCharacters);

CIP_ReturnCode cip_encode_CIP_ForwardOpen_Response_Success (CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardOpen_Response_Success const * const p_ForwardOpen_Response_Success);
CIP_ReturnCode cip_encode_CIP_ForwardOpen_Response_Unsuccessful (CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardOpen_Response_Unsuccessful const * const p_ForwardOpen_Response_Unsuccessful);

CIP_ReturnCode cip_encode_CIP_ForwardClose_Response_Success(CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardClose_Response_Success const * const p_ForwardClose_Response_Success);
CIP_ReturnCode cip_encode_CIP_ForwardClose_Response_Unsuccessful(CIP_ControlStruct const * const p_cip_ctrl, CIP_ForwardClose_Response_Unsuccessful const * const p_ForwardClose_Response_Unsuccessful);

CIP_ReturnCode cip_encode_EIP_TCPObject_Status (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_Status const * const p_TCPObject_Status);
CIP_ReturnCode cip_encode_EIP_TCPObject_ConfigurationCapability (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_ConfigurationCapability const * const p_TCPObject_ConfigurationCapability);
CIP_ReturnCode cip_encode_EIP_TCPObject_ConfigurationControl (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_ConfigurationControl const * const p_TCPObject_ConfigurationControl);
CIP_ReturnCode cip_encode_EIP_TCPObject_PhysicalLinkObject (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_PhysicalLinkObject const * const p_TCPObject_PhysicalLinkObject);
CIP_ReturnCode cip_encode_EIP_TCPObject_InterfaceConfiguration (CIP_ControlStruct const * const p_cip_ctrl, EIP_TCPObject_InterfaceConfiguration const * const p_TCPObject_InterfaceConfiguration);

CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceFlags_Internal (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceFlags_Internal const * const p_EthernetLinkObject_InterfaceFlags_Internal);
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_PhysicalAddress (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_PhysicalAddress const * const p_EthernetLinkObject_PhysicalAddress);
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCounters (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCounters const * const p_EthernetLinkObject_InterfaceCounters);
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_MediaCounters (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_MediaCounters const * const p_EthernetLinkObject_MediaCounters);
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCapability_Bits_Internal (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCapability_Bits_Internal const * const p_EthernetLinkObject_InterfaceCapability_Bits_Internal);
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array const * const p_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array);
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex const * const p_EthernetLinkObject_InterfaceCapability_SpeedDuplex);
CIP_ReturnCode cip_encode_EIP_EthernetLinkObject_InterfaceCapability (CIP_ControlStruct const * const p_cip_ctrl, EIP_EthernetLinkObject_InterfaceCapability const * const p_EthernetLinkObject_InterfaceCapability);

#endif /* __CIP_ENCODE_H__ */
