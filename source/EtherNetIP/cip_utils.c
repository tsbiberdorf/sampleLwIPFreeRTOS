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

/* ========================== */
/*   INCLUDE FILES            */
/* ========================== */
#include "cip_system.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "cip_utils.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */
static CIP_ReturnCode local_get_CIP_little_endian (CIP_ControlStruct const * const p_cip_ctrl, void * const p_data_val, RTA_Size_Type const len_in_bytes);
static CIP_ReturnCode local_put_CIP_little_endian (CIP_ControlStruct const * const p_cip_ctrl, void const * const p_data_val, RTA_Size_Type const len_in_bytes);
static CIP_ReturnCode local_get_CIP_big_endian (CIP_ControlStruct const * const p_cip_ctrl, void * const p_data_val, RTA_Size_Type const len_in_bytes);
static CIP_ReturnCode local_put_CIP_big_endian (CIP_ControlStruct const * const p_cip_ctrl, void const * const p_data_val, RTA_Size_Type const len_in_bytes);
static RTA_U8 * local_validate_size_and_return_new_pos (RTA_MEMORY_HEAP const * const p_heap, RTA_Size_Type const len_in_bytes);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */

/* NEW PAGE */
/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
/* ====================================================================
All functions with a native type can use the local_get_CIP_little_endian structure.
We use a function rather than a macro to ensure the traceability of the 
call stack when debugging.
======================================================================= */
CIP_ReturnCode cip_decode_CIP_BOOL (CIP_ControlStruct const * const p_cip_ctrl, CIP_BOOL  * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_SINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_SINT  * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_INT  (CIP_ControlStruct const * const p_cip_ctrl, CIP_INT   * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_DINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_DINT  * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_LINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_LINT  * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_UINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT  * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_UDINT * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_ULINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_ULINT * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_REAL (CIP_ControlStruct const * const p_cip_ctrl, CIP_REAL  * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_LREAL(CIP_ControlStruct const * const p_cip_ctrl, CIP_LREAL * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_BYTE (CIP_ControlStruct const * const p_cip_ctrl, CIP_BYTE  * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_WORD (CIP_ControlStruct const * const p_cip_ctrl, CIP_WORD  * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_DWORD(CIP_ControlStruct const * const p_cip_ctrl, CIP_DWORD * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_LWORD(CIP_ControlStruct const * const p_cip_ctrl, CIP_LWORD * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_CIP_OCTET(CIP_ControlStruct const * const p_cip_ctrl, CIP_OCTET * const p_data_val)    {   return(local_get_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }

CIP_ReturnCode cip_decode_big_endian_CIP_INT  (CIP_ControlStruct const * const p_cip_ctrl, CIP_INT * const p_data_val)     {   return(local_get_CIP_big_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_big_endian_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT * const p_data_val)   {   return(local_get_CIP_big_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_big_endian_CIP_UINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT * const p_data_val)    {   return(local_get_CIP_big_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_decode_big_endian_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_UDINT * const p_data_val)   {   return(local_get_CIP_big_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }

CIP_ReturnCode cip_encode_CIP_BOOL (CIP_ControlStruct const * const p_cip_ctrl, CIP_BOOL const * const p_data_val)   {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_SINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_SINT const * const p_data_val)   {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_INT  (CIP_ControlStruct const * const p_cip_ctrl, CIP_INT const * const p_data_val)    {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_DINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_DINT const * const p_data_val)   {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_LINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_LINT const * const p_data_val)   {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const * const p_data_val)  {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_UINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const * const p_data_val)   {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_UDINT const * const p_data_val)  {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_ULINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_ULINT const * const p_data_val)  {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_REAL (CIP_ControlStruct const * const p_cip_ctrl, CIP_REAL const * const p_data_val)   {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_LREAL(CIP_ControlStruct const * const p_cip_ctrl, CIP_LREAL const * const p_data_val)  {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_BYTE (CIP_ControlStruct const * const p_cip_ctrl, CIP_BYTE const * const p_data_val)   {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_WORD (CIP_ControlStruct const * const p_cip_ctrl, CIP_WORD const * const p_data_val)   {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_DWORD(CIP_ControlStruct const * const p_cip_ctrl, CIP_DWORD const * const p_data_val)  {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_LWORD(CIP_ControlStruct const * const p_cip_ctrl, CIP_LWORD const * const p_data_val)  {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_CIP_OCTET(CIP_ControlStruct const * const p_cip_ctrl, CIP_OCTET const * const p_data_val)  {   return(local_put_CIP_little_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }

CIP_ReturnCode cip_encode_big_endian_CIP_INT  (CIP_ControlStruct const * const p_cip_ctrl, CIP_INT const * const p_data_val)     {   return(local_put_CIP_big_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_big_endian_CIP_USINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_USINT const * const p_data_val)   {   return(local_put_CIP_big_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_big_endian_CIP_UINT (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const * const p_data_val)    {   return(local_put_CIP_big_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }
CIP_ReturnCode cip_encode_big_endian_CIP_UDINT(CIP_ControlStruct const * const p_cip_ctrl, CIP_UDINT const * const p_data_val)   {   return(local_put_CIP_big_endian(p_cip_ctrl, p_data_val, sizeof(*p_data_val)));   }

CIP_ReturnCode cip_decode_memcpy(CIP_ControlStruct const * const p_cip_ctrl, void * const p_data_val, RTA_Size_Type const len_in_bytes)
{
    RTA_U8 *p_new_current_end;
    RTA_MEMORY_HEAP *p_heap;
    
    if(!p_cip_ctrl || !p_data_val || !len_in_bytes)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
    
    p_heap = p_cip_ctrl->p_heap_src;
    
    if(!p_heap)
        return(CIP_RETURNCODE_ERROR_INTERNAL);

    p_new_current_end = local_validate_size_and_return_new_pos(p_heap, len_in_bytes);
    if(!p_new_current_end) return(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    memcpy(p_data_val, p_heap->p_curpos, len_in_bytes);

    p_heap->p_curpos = p_new_current_end;

    return(CIP_RETURNCODE_SUCCESS);
}

RTA_Size_Type cip_decode_get_buffer_size(CIP_ControlStruct const * const p_cip_ctrl)
{
    if(!p_cip_ctrl)
        return(0);

    if(!p_cip_ctrl->p_heap_src)
        return(0);

    return(p_cip_ctrl->p_heap_src->bytes_used);
}

RTA_Size_Type cip_decode_get_remaining_used_size(CIP_ControlStruct const * const p_cip_ctrl)
{
    RTA_Size_Type bytes_decoded;

    if(!p_cip_ctrl)
        return(0);

    if(!p_cip_ctrl->p_heap_src)
        return(0);

    bytes_decoded = (RTA_Size_Type)(p_cip_ctrl->p_heap_src->p_curpos - p_cip_ctrl->p_heap_src->buffer);

    if(bytes_decoded < p_cip_ctrl->p_heap_src->bytes_used)
        return((p_cip_ctrl->p_heap_src->bytes_used - bytes_decoded));

    return(0);
}

RTA_U8 * cip_decode_get_current_buffer_ptr(CIP_ControlStruct const * const p_cip_ctrl)
{
    if(!p_cip_ctrl)
        return(RTA_NULL);

    if(!p_cip_ctrl->p_heap_src)
        return(RTA_NULL); 

    return(rta_memory_get_current_position_from_heap(p_cip_ctrl->p_heap_src));
}

CIP_ReturnCode cip_encode_flush_heap(CIP_ControlStruct const * const p_cip_ctrl)
{
    if(!p_cip_ctrl)
    {
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
    }

    if(!p_cip_ctrl->p_heap_dst)
    {
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
    }

    rta_memory_malloc_restart_heap(p_cip_ctrl->p_heap_dst);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_memcpy(CIP_ControlStruct const * const p_cip_ctrl, void const * const p_data_val, RTA_Size_Type const len_in_bytes)
{
    RTA_U8 *p_data;

    if(!p_cip_ctrl || !p_data_val || !len_in_bytes)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_data = cip_binary_malloc(p_cip_ctrl, len_in_bytes);
    if(!p_data)
        return(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    memcpy(p_data, p_data_val, len_in_bytes);

    return(CIP_RETURNCODE_SUCCESS);
}

RTA_U8 * cip_encode_get_current_buffer_ptr(CIP_ControlStruct const * const p_cip_ctrl)
{
    if(!p_cip_ctrl)
        return(RTA_NULL);

    if(!p_cip_ctrl->p_heap_dst)
        return(RTA_NULL);

    return(rta_memory_get_current_buffer_ptr_from_heap(p_cip_ctrl->p_heap_dst));
}

CIP_UDINT cip_encode_get_len_from_p_start_to_end_of_data (CIP_ControlStruct const * const p_cip_ctrl, RTA_U8 const * const p_start)
{
    RTA_U8 const * p_end;
    CIP_UDINT length;

    if(!p_cip_ctrl || !p_start)
        return(0);

    if(!p_cip_ctrl->p_heap_dst)
        return(0);

    p_end = cip_encode_get_current_buffer_ptr(p_cip_ctrl);

    if(!rta_memory_is_startptr_before_endptr_in_heap(p_cip_ctrl->p_heap_dst, p_start, p_end))
        return(0);

    length = (CIP_UDINT)(p_end - p_start);
    return(length);
}

CIP_ReturnCode cip_encode_CIP_UINT_length_to_plen(CIP_UINT const length, RTA_U8 * const p_len)
{
    if(!p_len)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rta_endian_PutLitEndian16(length, p_len);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_encode_CIP_UDINT_length_to_plen(CIP_UDINT const length, RTA_U8 * const p_len)
{
    if(!p_len)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rta_endian_PutLitEndian32(length, p_len);

    return(CIP_RETURNCODE_SUCCESS);
}

void * cip_binary_malloc (CIP_ControlStruct const * const p_cip_ctrl, RTA_Size_Type const num_bytes)
{
    if(!p_cip_ctrl || !num_bytes)
        return(RTA_NULL);

    return(rta_memory_malloc_from_heap (p_cip_ctrl->p_heap_dst, num_bytes));
}

void * cip_binary_malloc_all (CIP_ControlStruct const * const p_cip_ctrl, RTA_Size_Type * const p_num_bytes)
{
    if(!p_cip_ctrl || !p_num_bytes)
        return(RTA_NULL);

    return(rta_memory_malloc_from_heap_all(p_cip_ctrl->p_heap_dst, p_num_bytes));
}

CIP_ReturnCode cip_binary_swap_heaps (CIP_ControlStruct * const p_cip_ctrl, RTA_Size_Type const new_destination_alignment_size_in_bytes)
{
    if(!p_cip_ctrl)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    rta_memory_abandon_heap(p_cip_ctrl->p_heap_src);

    p_cip_ctrl->p_heap_src = p_cip_ctrl->p_heap_dst;
    p_cip_ctrl->p_heap_dst = rta_memory_reserve_heap(new_destination_alignment_size_in_bytes);    
    if(!p_cip_ctrl->p_heap_dst) 
        return CIP_RETURNCODE_ERROR_INVALID_PARAMETER;

    return CIP_RETURNCODE_SUCCESS;
}

void cip_binary_decode_rewind (CIP_ControlStruct const * const p_cip_ctrl, void const * const passed_ptr)
{
    if(!p_cip_ctrl)
        return;

    rta_memory_set_current_position_in_heap(p_cip_ctrl->p_heap_src, passed_ptr);
}

void cip_binary_encode_rewind (CIP_ControlStruct const * const p_cip_ctrl, void const * const passed_ptr)
{
    if(!p_cip_ctrl)
        return;

    rta_memory_malloc_rewind_heap(p_cip_ctrl->p_heap_dst, passed_ptr);
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */
static CIP_ReturnCode local_get_CIP_little_endian (CIP_ControlStruct const * const p_cip_ctrl, void * const p_data_val, RTA_Size_Type const len_in_bytes)
{
    RTA_U8  *p_temp8;
    RTA_U16 *p_temp16;
    RTA_U32 *p_temp32;
    RTA_U64 *p_temp64;

    RTA_U8 *p_new_current_end;
    RTA_MEMORY_HEAP *p_heap;

    if(!p_cip_ctrl || !p_data_val || !len_in_bytes)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_heap = p_cip_ctrl->p_heap_src;   

    if(!p_heap)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_new_current_end = local_validate_size_and_return_new_pos(p_heap, len_in_bytes);
    if(!p_new_current_end) 
        return(CIP_RETURNCODE_DECODE_MALLOC_FAILED);

    switch(len_in_bytes)
    {
        case 1: 
            p_temp8 = p_data_val;
            *p_temp8 = *p_heap->p_curpos;
            break;

        case 2:
            p_temp16 = p_data_val;
            *p_temp16 = rta_endian_GetLitEndian16(p_heap->p_curpos);    
            break;

        case 4:
            p_temp32 = p_data_val;
            *p_temp32 = rta_endian_GetLitEndian32(p_heap->p_curpos);    
            break;

        case 8:
            p_temp64 = p_data_val;
            *p_temp64 = rta_endian_GetLitEndian64(p_heap->p_curpos);    
            break;

        default: return(CIP_RETURNCODE_ERROR_INTERNAL);
    }

    p_heap->p_curpos = p_new_current_end;

    return(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_put_CIP_little_endian (CIP_ControlStruct const * const p_cip_ctrl, void const * const p_data_val, RTA_Size_Type const len_in_bytes)
{
    RTA_U8  *p_data;
    RTA_U8  const *p_temp8;
    RTA_U16 const *p_temp16;
    RTA_U32 const *p_temp32;
    RTA_U64 const *p_temp64;

    if(!p_cip_ctrl || !p_data_val || !len_in_bytes)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_data = (RTA_U8 *)cip_binary_malloc(p_cip_ctrl, len_in_bytes);
    if(!p_data)
        return(CIP_RETURNCODE_ENCODE_MALLOC_FAILED);

    switch(len_in_bytes)
    {
        case 1: 
            p_temp8 = p_data_val;
            *p_data = *p_temp8;
            break;

        case 2:
            p_temp16 = p_data_val;
            rta_endian_PutLitEndian16(*p_temp16, p_data);
            break;

        case 4:
            p_temp32 = p_data_val;
            rta_endian_PutLitEndian32(*p_temp32, p_data);
            break;

        case 8:
            p_temp64 = p_data_val;
            rta_endian_PutLitEndian64(*p_temp64, p_data);
            break;

        default: return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
    }

    return(CIP_RETURNCODE_SUCCESS);
}

/* TODO local_get_CIP_big_endian is needed for EIP Client. Remove lint comment when client is implemented. */
/*lint -esym(528, local_get_CIP_big_endian) Symbol 'Symbol' (Location) not referenced */
static CIP_ReturnCode local_get_CIP_big_endian (CIP_ControlStruct const * const p_cip_ctrl, void * const p_data_val, RTA_Size_Type const len_in_bytes)
{
    RTA_U8  *p_temp8;
    RTA_U16 *p_temp16;
    RTA_U32 *p_temp32;
    RTA_U64 *p_temp64;

    RTA_U8 *p_new_current_end;
    RTA_MEMORY_HEAP *p_heap;

    if(!p_cip_ctrl || !p_data_val || !len_in_bytes)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_heap = p_cip_ctrl->p_heap_src;   

    if(!p_heap)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_new_current_end = local_validate_size_and_return_new_pos(p_heap, len_in_bytes);
    if(!p_new_current_end) 
        return(CIP_RETURNCODE_DECODE_MALLOC_FAILED);

    switch(len_in_bytes)
    {
        case 1: 
            p_temp8 = p_data_val;
            *p_temp8 = *p_heap->p_curpos;
            break;

        case 2:
            p_temp16 = p_data_val;
            *p_temp16 = rta_endian_GetBigEndian16(p_heap->p_curpos);    
            break;

        case 4:
            p_temp32 = p_data_val;
            *p_temp32 = rta_endian_GetBigEndian32(p_heap->p_curpos);    
            break;

        case 8:
            p_temp64 = p_data_val;
            *p_temp64 = rta_endian_GetBigEndian64(p_heap->p_curpos);    
            break;

        default: return(CIP_RETURNCODE_ERROR_INTERNAL);
    }

    p_heap->p_curpos = p_new_current_end;

    return(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_put_CIP_big_endian (CIP_ControlStruct const * const p_cip_ctrl, void const * const p_data_val, RTA_Size_Type const len_in_bytes)
{
    RTA_U8  *p_data;
    RTA_U8  const *p_temp8;
    RTA_U16 const *p_temp16;
    RTA_U32 const *p_temp32;
    RTA_U64 const *p_temp64;

    if(!p_cip_ctrl || !p_data_val || !len_in_bytes)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_data = (RTA_U8 *)cip_binary_malloc(p_cip_ctrl, len_in_bytes);
    if(!p_data)
        return(CIP_RETURNCODE_ENCODE_MALLOC_FAILED);

    switch(len_in_bytes)
    {
        case 1: 
            p_temp8 = p_data_val;
            *p_data = *p_temp8;
            break;

        case 2:
            p_temp16 = p_data_val;
            rta_endian_PutBigEndian16(*p_temp16, p_data);
            break;

        case 4:
            p_temp32 = p_data_val;
            rta_endian_PutBigEndian32(*p_temp32, p_data);
            break;

        case 8:
            p_temp64 = p_data_val;
            rta_endian_PutBigEndian64(*p_temp64, p_data);
            break;

        default: return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
    }

    return(CIP_RETURNCODE_SUCCESS);
}

static RTA_U8 * local_validate_size_and_return_new_pos (RTA_MEMORY_HEAP const * const p_heap, RTA_Size_Type const len_in_bytes)
{
    RTA_U8 const *p_malloc_end;
    RTA_U8 *p_new_current_end;

    if((!p_heap) || (len_in_bytes == 0))
        return(RTA_NULL);

    if(p_heap->bytes_used == 0)
        return(RTA_NULL);

    p_malloc_end = &p_heap->buffer[p_heap->bytes_used];

    p_new_current_end = p_heap->p_curpos+len_in_bytes; 

    if(p_new_current_end > p_malloc_end)
        return(RTA_NULL);

    return(p_new_current_end);
}

/* *********** */
/* END OF FILE */
/* *********** */
