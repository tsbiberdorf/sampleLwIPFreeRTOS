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

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "rta_system.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "rta_memory.c"

/* ---------------------------- */
/* FUNCTION PROTOTYPES          */
/* ---------------------------- */

/* ---------------------------- */
/* VARIABLES                    */
/* ---------------------------- */
static RTA_MEMORY_HEAP rta_heap[RTA_USER_MEMORY_NUM_HEAPS];
static RTA_Size_Type rta_max_heap_size_used = 0;

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */

/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
RTA_RETURN_CODE rta_memory_init (RTA_INIT_TYPE const init_type)
{
    RTA_U16 ix;

    RTA_UNUSED_PARAM(init_type);

    memset(rta_heap, 0, sizeof(rta_heap));

    for(ix=0; ix<RTA_USER_MEMORY_NUM_HEAPS; ix++)
    {
        rta_heap[ix].heap_used = RTA_FALSE; /* mark as free */
        rta_heap[ix].p_curpos = rta_heap[ix].buffer; /* point to the start of the buffer */
    }

    return(RTA_ERR_SUCCESS);
}

RTA_MEMORY_HEAP * rta_memory_reserve_heap (RTA_Size_Type const alignment_size_in_bytes)
{
    RTA_U16 ix;

    for(ix=0; ix<RTA_USER_MEMORY_NUM_HEAPS; ix++)
    {
        if(!rta_heap[ix].heap_used)
        {
            rta_heap[ix].heap_used = RTA_TRUE;

            memset(rta_heap[ix].buffer, 0, sizeof(rta_heap[0].buffer));

            rta_heap[ix].alignment_size_in_bytes = alignment_size_in_bytes;
            rta_heap[ix].bytes_used = 0;
            rta_heap[ix].p_curpos = rta_heap[ix].buffer;

            return(&rta_heap[ix]);
        }
    }

    return(RTA_NULL);
}

void rta_memory_abandon_heap (RTA_MEMORY_HEAP * const p_heap)
{
    if(!p_heap)
        return;

    p_heap->heap_used = RTA_FALSE;
}

RTA_Size_Type rta_memory_get_free_memory_in_heap(RTA_MEMORY_HEAP const * const p_heap)
{
    if(!p_heap)
        return 0;

    if(!p_heap->heap_used)
        return 0;

    return(sizeof(p_heap->buffer) - p_heap->bytes_used);
}

RTA_Size_Type rta_memory_get_heap_size (RTA_MEMORY_HEAP const * const p_heap)
{
    if(!p_heap)
        return 0;

    if(!p_heap->heap_used)
        return 0;

    return(sizeof(p_heap->buffer));
}

void * rta_memory_malloc_from_heap (RTA_MEMORY_HEAP * const p_heap, RTA_Size_Type const num_bytes)
{
    RTA_Size_Type free_memory;
    RTA_U32 start_ix;
    RTA_Size_Type aligned_size;
    RTA_Size_Type num_bytes_to_be_aligned = 0;
    RTA_Size_Type mod_value;
    
    if(!p_heap)
        return (RTA_NULL);

    if(!p_heap->heap_used)
        return (RTA_NULL);
    
    if(p_heap->alignment_size_in_bytes > 0)
    {
        /* figure out how many byte we need to skip to start on the alignment boundary */
        mod_value = (p_heap->bytes_used % p_heap->alignment_size_in_bytes);
        if(mod_value)
            num_bytes_to_be_aligned = (p_heap->alignment_size_in_bytes - mod_value);
    }
 
    aligned_size = (num_bytes + num_bytes_to_be_aligned); 

    free_memory = rta_memory_get_free_memory_in_heap(p_heap);
    if(aligned_size > free_memory)
        return (RTA_NULL);

    start_ix = p_heap->bytes_used + num_bytes_to_be_aligned;
    p_heap->bytes_used += aligned_size;

    if(rta_max_heap_size_used < p_heap->bytes_used)
        rta_max_heap_size_used = p_heap->bytes_used;

    return(&p_heap->buffer[start_ix]);
}

RTA_Size_Type rta_memory_get_max_heap_bytes_used (void)
{
    return(rta_max_heap_size_used);
}

void * rta_memory_malloc_from_heap_all (RTA_MEMORY_HEAP * const p_heap, RTA_Size_Type * const p_num_bytes)
{
    if(!p_heap || !p_num_bytes)
        return (RTA_NULL);

    *p_num_bytes = rta_memory_get_free_memory_in_heap(p_heap);
    if(*p_num_bytes == 0)
        return (RTA_NULL);

    return rta_memory_malloc_from_heap(p_heap, *p_num_bytes);
}

void rta_memory_malloc_rewind_heap (RTA_MEMORY_HEAP * const p_heap, void const * const passed_ptr)
{
    RTA_U8 const * const ptr = (RTA_U8 const * const)passed_ptr; /* cast to RTA_U8 to allow for pointer math */
    RTA_U8 const * p_start;
    RTA_U8 const * p_end;
    
    if(!p_heap)
        return;

    if(!ptr)
        return;

    if(!p_heap->heap_used)
        return;

    p_start = p_heap->buffer;
    p_end = &(p_heap->buffer[(sizeof(rta_heap[0].buffer)-1)]);
    if((ptr >= p_start) && (ptr <= p_end))
    {
        p_heap->bytes_used = (RTA_Size_Type)(ptr - p_heap->buffer);

        memset(&p_heap->buffer[p_heap->bytes_used], 0, (sizeof(rta_heap[0].buffer)-p_heap->bytes_used));
    }
}

void rta_memory_set_current_position_in_heap (RTA_MEMORY_HEAP * const p_heap, void const * const passed_ptr)
{
    RTA_U8 * const ptr = (RTA_U8 * const)passed_ptr; /* cast to RTA_U8 to allow for pointer math */
    RTA_U8 const * p_start;
    RTA_U8 const * p_end;
    
    if(!p_heap)
        return;

    if(!ptr)
        return;

    if(!p_heap->heap_used)
        return;

    p_start = p_heap->buffer;
    p_end = &(p_heap->buffer[(sizeof(rta_heap[0].buffer) - 1)]);
    if ((ptr >= p_start) && (ptr <= p_end))
    {    
        p_heap->p_curpos = ptr;
    }
}

void rta_memory_malloc_restart_heap (RTA_MEMORY_HEAP * const p_heap)
{  
    if(!p_heap)
        return;

    if(!p_heap->heap_used)
        return;

    p_heap->p_curpos = p_heap->buffer;
    p_heap->bytes_used = 0;
    memset(p_heap->buffer, 0, sizeof(p_heap->buffer));
}

void * rta_memory_get_current_position_from_heap (RTA_MEMORY_HEAP const * const p_heap)
{
    if(!p_heap)
        return(RTA_NULL);

    if(!p_heap->heap_used)
        return(RTA_NULL);

    return((void *)p_heap->p_curpos);
}

void * rta_memory_get_current_buffer_ptr_from_heap (RTA_MEMORY_HEAP const * const p_heap)
{
    if(!p_heap)
        return(RTA_NULL);

    if(!p_heap->heap_used)
        return(RTA_NULL);

    return((void *)&p_heap->buffer[p_heap->bytes_used]);
}

RTA_BOOL rta_memory_is_pointer_in_heap (RTA_MEMORY_HEAP const * const p_heap, void const * const ptr)
{
    RTA_Size_Type last_ix;
    RTA_U8 const * p_start;
    RTA_U8 const * const p_current = ptr;
    RTA_U8 const * p_end;

    if(!ptr || !p_heap)
        return(RTA_FALSE);

    p_start = &p_heap->buffer[0];
    if(p_current < p_start)
        return(RTA_FALSE);
    
    last_ix = (sizeof(p_heap->buffer) - 1);
    p_end = &p_heap->buffer[last_ix];
    if(p_current > p_end )
        return(RTA_FALSE);

    return(RTA_TRUE);
}

RTA_BOOL rta_memory_is_startptr_before_endptr_in_heap(RTA_MEMORY_HEAP const * const p_heap, void const * const p_start, void const * const p_end)
{
    if(!p_start || !p_end || !p_heap)
        return(RTA_FALSE);

    if(!rta_memory_is_pointer_in_heap(p_heap, p_start))
        return(RTA_FALSE);

    if(!rta_memory_is_pointer_in_heap(p_heap, p_end))
        return(RTA_FALSE);

    if(p_start > p_end)
        return(RTA_FALSE);

    return(RTA_TRUE);
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */

/* *********** */
/* END OF FILE */
/* *********** */
