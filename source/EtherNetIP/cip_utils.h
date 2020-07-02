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

#ifndef __CIP_UTILS_H__
#define __CIP_UTILS_H__

CIP_ReturnCode cip_decode_CIP_BOOL (CIP_ControlStruct const * const p_cip_ctrl, CIP_BOOL  * const p_data_val);
CIP_ReturnCode cip_decode_CIP_SINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_SINT  * const p_data_val);
CIP_ReturnCode cip_decode_CIP_INT  (CIP_ControlStruct const * const p_cip_ctrl, CIP_INT   * const p_data_val);
CIP_ReturnCode cip_decode_CIP_DINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_DINT  * const p_data_val);
CIP_ReturnCode cip_decode_CIP_LINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_LINT  * const p_data_val);
CIP_ReturnCode cip_decode_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT * const p_data_val);
CIP_ReturnCode cip_decode_CIP_UINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT  * const p_data_val);
CIP_ReturnCode cip_decode_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_UDINT * const p_data_val);
CIP_ReturnCode cip_decode_CIP_ULINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_ULINT * const p_data_val);
CIP_ReturnCode cip_decode_CIP_REAL (CIP_ControlStruct const * const p_cip_ctrl, CIP_REAL  * const p_data_val);
CIP_ReturnCode cip_decode_CIP_LREAL(CIP_ControlStruct const * const p_cip_ctrl, CIP_LREAL * const p_data_val);
CIP_ReturnCode cip_decode_CIP_BYTE (CIP_ControlStruct const * const p_cip_ctrl, CIP_BYTE  * const p_data_val);
CIP_ReturnCode cip_decode_CIP_WORD (CIP_ControlStruct const * const p_cip_ctrl, CIP_WORD  * const p_data_val);
CIP_ReturnCode cip_decode_CIP_DWORD(CIP_ControlStruct const * const p_cip_ctrl, CIP_DWORD * const p_data_val);
CIP_ReturnCode cip_decode_CIP_LWORD(CIP_ControlStruct const * const p_cip_ctrl, CIP_LWORD * const p_data_val);
CIP_ReturnCode cip_decode_CIP_OCTET(CIP_ControlStruct const * const p_cip_ctrl, CIP_OCTET * const p_data_val);

/* most of EtherNet/IP is little endian, a few encodings are big endian */
CIP_ReturnCode cip_decode_big_endian_CIP_INT  (CIP_ControlStruct const * const p_cip_ctrl, CIP_INT * const p_data_val);
CIP_ReturnCode cip_decode_big_endian_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT * const p_data_val);
CIP_ReturnCode cip_decode_big_endian_CIP_UINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT * const p_data_val);
CIP_ReturnCode cip_decode_big_endian_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_UDINT * const p_data_val);

CIP_ReturnCode cip_decode_memcpy(CIP_ControlStruct const * const p_cip_ctrl, void * const p_data_val, RTA_Size_Type const len_in_bytes);

/* we need a way to get the pointer/size to the start of the heap (to peak data or transmit data) */
RTA_Size_Type cip_decode_get_buffer_size(CIP_ControlStruct const * const p_cip_ctrl);
RTA_Size_Type cip_decode_get_remaining_used_size(CIP_ControlStruct const * const p_cip_ctrl);
RTA_U8 * cip_decode_get_current_buffer_ptr(CIP_ControlStruct const * const p_cip_ctrl);

CIP_ReturnCode cip_encode_CIP_BOOL (CIP_ControlStruct const * const p_cip_ctrl, CIP_BOOL const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_SINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_SINT const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_INT  (CIP_ControlStruct const * const p_cip_ctrl, CIP_INT const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_DINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_DINT const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_LINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_LINT const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_UINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_UDINT const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_ULINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_ULINT const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_REAL (CIP_ControlStruct const * const p_cip_ctrl, CIP_REAL const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_LREAL(CIP_ControlStruct const * const p_cip_ctrl, CIP_LREAL const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_BYTE (CIP_ControlStruct const * const p_cip_ctrl, CIP_BYTE const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_WORD (CIP_ControlStruct const * const p_cip_ctrl, CIP_WORD const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_DWORD(CIP_ControlStruct const * const p_cip_ctrl, CIP_DWORD const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_LWORD(CIP_ControlStruct const * const p_cip_ctrl, CIP_LWORD const * const p_data_val);
CIP_ReturnCode cip_encode_CIP_OCTET(CIP_ControlStruct const * const p_cip_ctrl, CIP_OCTET const * const p_data_val);

/* most of EtherNet/IP is little endian, a few encodings are big endian */
CIP_ReturnCode cip_encode_big_endian_CIP_INT  (CIP_ControlStruct const * const p_cip_ctrl, CIP_INT const * const p_data_val);
CIP_ReturnCode cip_encode_big_endian_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const * const p_data_val);
CIP_ReturnCode cip_encode_big_endian_CIP_UINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const * const p_data_val);
CIP_ReturnCode cip_encode_big_endian_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_UDINT const * const p_data_val);

CIP_ReturnCode cip_encode_memcpy(CIP_ControlStruct const * const p_cip_ctrl, void const * const p_data_val, RTA_Size_Type const len_in_bytes);
CIP_ReturnCode cip_encode_flush_heap(CIP_ControlStruct const * const p_cip_ctrl);
RTA_U8 * cip_encode_get_current_buffer_ptr(CIP_ControlStruct const * const p_cip_ctrl);
CIP_UDINT cip_encode_get_len_from_p_start_to_end_of_data (CIP_ControlStruct const * const p_cip_ctrl, RTA_U8 const * const p_start);
CIP_ReturnCode cip_encode_CIP_UINT_length_to_plen(CIP_UINT const length, RTA_U8 * const p_len);
CIP_ReturnCode cip_encode_CIP_UDINT_length_to_plen(CIP_UDINT const length, RTA_U8 * const p_len);

void * cip_binary_malloc (CIP_ControlStruct const * const p_cip_ctrl, RTA_Size_Type const num_bytes);
void * cip_binary_malloc_all (CIP_ControlStruct const * const p_cip_ctrl, RTA_Size_Type * const p_num_bytes);
CIP_ReturnCode cip_binary_swap_heaps (CIP_ControlStruct * const p_cip_ctrl, RTA_Size_Type const new_destination_alignment_size_in_bytes);

void cip_binary_decode_rewind (CIP_ControlStruct const * const p_cip_ctrl, void const * const passed_ptr);
void cip_binary_encode_rewind (CIP_ControlStruct const * const p_cip_ctrl, void const * const passed_ptr);

#endif /* __CIP_UTILS_H__ */
