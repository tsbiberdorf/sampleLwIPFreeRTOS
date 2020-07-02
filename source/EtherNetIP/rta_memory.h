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

#ifndef __RTA_MEMORY_H__
#define __RTA_MEMORY_H__

/* --------------------------------------------------------------- */
/*      GENERIC DEFINITIONS                                        */
/* --------------------------------------------------------------- */

/* --------------------------------------------------------------- */
/*      TYPE DEFINITIONS                                           */
/* --------------------------------------------------------------- */

typedef size_t RTA_Size_Type;

/* --------------------------------------------------------------- */
/*      STRUCTURE DEFINITIONS                                      */
/* --------------------------------------------------------------- */
typedef struct
{ 
    RTA_BOOL        heap_used; 
    RTA_Size_Type   alignment_size_in_bytes;    /* must be set to a power of 2 >= 1; 1 is 8-bit aligned */    
    RTA_Size_Type   bytes_used;                 /* this doubles as the next index */
    RTA_U8          buffer[RTA_USER_MEMORY_NUM_BYTES_PER_HEAP];

    /* Auto-magic hidden pointer use. We want the accessor to advance the pointer as we decode or encode */
    RTA_U8          *p_curpos; 
}RTA_MEMORY_HEAP;

/* --------------------------------------------------------------- */
/*      FUNCTION PROTOTYPES                                        */
/* --------------------------------------------------------------- */

RTA_RETURN_CODE rta_memory_init (RTA_INIT_TYPE const init_type);
RTA_MEMORY_HEAP * rta_memory_reserve_heap (RTA_Size_Type const alignment_size_in_bytes);
void rta_memory_abandon_heap (RTA_MEMORY_HEAP * const p_heap);
RTA_Size_Type rta_memory_get_free_memory_in_heap (RTA_MEMORY_HEAP const * const p_heap);
RTA_Size_Type rta_memory_get_heap_size (RTA_MEMORY_HEAP const * const p_heap);
void * rta_memory_malloc_from_heap (RTA_MEMORY_HEAP * const p_heap, RTA_Size_Type const num_bytes);
void * rta_memory_malloc_from_heap_all (RTA_MEMORY_HEAP * const p_heap, RTA_Size_Type * const p_num_bytes);
void rta_memory_set_current_position_in_heap (RTA_MEMORY_HEAP * const p_heap, void const * passed_ptr);
void rta_memory_malloc_rewind_heap (RTA_MEMORY_HEAP * const p_heap, void const * passed_ptr);
void rta_memory_malloc_restart_heap (RTA_MEMORY_HEAP * const p_heap);
void * rta_memory_get_current_position_from_heap (RTA_MEMORY_HEAP const * const p_heap);
void * rta_memory_get_current_buffer_ptr_from_heap (RTA_MEMORY_HEAP const * const p_heap);
RTA_BOOL rta_memory_is_pointer_in_heap (RTA_MEMORY_HEAP const * const p_heap, void const * const ptr);
RTA_BOOL rta_memory_is_startptr_before_endptr_in_heap(RTA_MEMORY_HEAP const * const p_heap, void const * const p_start, void const * const p_end);

/* heap metrics */
RTA_Size_Type rta_memory_get_max_heap_bytes_used (void);

#endif /* __RTA_MEMORY_H__ */
