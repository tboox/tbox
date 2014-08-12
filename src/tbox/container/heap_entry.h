/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        heap_entry.h
 * @ingroup     container
 *
 */
#ifndef TB_CONTAINER_HEAP_ENTRY_H
#define TB_CONTAINER_HEAP_ENTRY_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the heap entry
#define tb_heap_entry(head, entry)   ((((tb_byte_t*)(entry)) - (head)->eoff))

/*! init the heap entry 
 *
 * @code
 *
    // the xxxx entry type
    typedef struct __tb_xxxx_entry_t 
    {
        // the heap entry
        tb_heap_entry_t     entry;

        // the data
        tb_size_t           data;

    }tb_xxxx_entry_t;

    // the xxxx entry copy func
    static tb_void_t tb_xxxx_entry_copy(tb_pointer_t ltem, tb_pointer_t rtem)
    {
        // check
        tb_assert_return(ltem && rtem);

        // copy it
        ((tb_xxxx_entry_t*)ltem)->data = ((tb_xxxx_entry_t*)rtem)->data;
    }

    // init the heap
    tb_heap_entry_head_t heap;
    tb_heap_entry_init(&heap, tb_xxxx_entry_t, entry, tb_xxxx_entry_copy);

 * @endcode
 */
#define tb_heap_entry_init(heap, type, entry, copy)     tb_heap_entry_init_(heap, tb_offsetof(type, entry), sizeof(type), copy)

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the heap entry copy func type
typedef tb_void_t               (*tb_heap_entry_copy_t)(tb_pointer_t ltem, tb_pointer_t rtem);

/*! the heap entry type
 * 
 * <pre>
 * heap:    1      4      2      6       9       7       8       10       14       16
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       6       (last / 2 - 1)9    7             8
 *                   ---------       ----
 *                  |         |     |
 *                  10        14    16(last - 1)
 * </pre>
 */
typedef struct __tb_heap_entry_t 
{
    /// the next entry
    struct __tb_heap_entry_t*   next;

    /// the prev entry
    struct __tb_heap_entry_t*   prev;

}tb_heap_entry_t;

/// the heap entry head type
typedef struct __tb_heap_entry_head_t 
{
    /// the next entry
    struct __tb_heap_entry_t*   next;

    /// the prev entry
    struct __tb_heap_entry_t*   prev;

    /// the heap size
    tb_size_t                   size;

    /// the iterator 
    tb_iterator_t               itor;

    /// the entry offset
    tb_size_t                   eoff;

    /// the entry copy func
    tb_entry_copy_t             copy;

}tb_heap_entry_head_t;

/// the heap entry ref type
typedef tb_heap_entry_t*        tb_heap_entry_ref_t;

/// the heap entry head ref type
typedef tb_heap_entry_head_t*   tb_heap_entry_head_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the heap iterator
 *
 * @param heap                              the heap
 *
 * @return                                  the heap iterator
 */
tb_iterator_ref_t                           tb_heap_entry_itor(tb_heap_entry_head_ref_t heap);

/*! init heap
 *
 * @param heap                              the heap
 * @param entry_offset                      the entry offset 
 * @param entry_size                        the entry size 
 * @param copy                              the copy func of the entry for algorithm, .e.g sort
 */
tb_void_t                                   tb_heap_entry_init_(tb_heap_entry_head_ref_t heap, tb_size_t entry_offset, tb_size_t entry_size, tb_entry_copy_t copy);

/*! exit heap
 *
 * @param heap                              the heap
 */ 
tb_void_t                                   tb_heap_entry_exit(tb_heap_entry_head_ref_t heap);

/*! clear heap
 *
 * @param heap                              the heap
 */
static __tb_inline__ tb_void_t              tb_heap_entry_clear(tb_heap_entry_head_ref_t heap)
{
    // check
    tb_assert(heap);

    // clear it
    heap->next = (tb_heap_entry_ref_t)heap;
    heap->prev = (tb_heap_entry_ref_t)heap;
    heap->size = 0;
}

/*! the heap entry count
 *
 * @param heap                              the heap
 *
 * @return                                  the heap entry count
 */
static __tb_inline__ tb_size_t              tb_heap_entry_size(tb_heap_entry_head_ref_t heap)
{ 
    // check
    tb_assert(heap);

    // done
    return heap->size;
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

