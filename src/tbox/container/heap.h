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
 * @file        heap.h
 * @ingroup     container
 *
 */
#ifndef TB_CONTAINER_HEAP_H
#define TB_CONTAINER_HEAP_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the head impl type
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
 *
 * head: => the head item
 * last: => the last item
 * tail: => behind the last item, no item
 *
 * performance: 
 *
 * put: O(lgn)
 * pop: O(1)
 * top: O(1)
 *
 * iterator:
 *
 * next: fast
 * prev: fast
 *
 * </pre>
 *
 * @note the itor of the same item is mutable
 */
typedef tb_iterator_ref_t tb_heap_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init heap, default: minheap
 *
 * @param grow      the item grow
 * @param func      the item func
 *
 * @return          the heap
 */
tb_heap_ref_t       tb_heap_init(tb_size_t grow, tb_item_func_t func);

/*! exist heap
 *
 * @param heap      the heap
 */
tb_void_t           tb_heap_exit(tb_heap_ref_t heap);

/*! clear the heap
 *
 * @param heap      the heap
 */
tb_void_t           tb_heap_clear(tb_heap_ref_t heap);

/*! the heap size
 *
 * @param heap      the heap
 *
 * @return          the heap size
 */
tb_size_t           tb_heap_size(tb_heap_ref_t heap);

/*! the heap grow
 *
 * @param heap      the heap
 *
 * @return          the heap grow
 */
tb_size_t           tb_heap_grow(tb_heap_ref_t heap);

/*! the heap maxn
 *
 * @param heap      the heap
 *
 * @return          the heap maxn
 */
tb_size_t           tb_heap_maxn(tb_heap_ref_t heap);

/*! the heap top item
 *
 * @param heap      the heap
 *
 * @return          the heap top item
 */
tb_pointer_t        tb_heap_top(tb_heap_ref_t heap);

/*! put the heap item
 *
 * @param heap      the heap
 * @param data      the item data
 */
tb_void_t           tb_heap_put(tb_heap_ref_t heap, tb_cpointer_t data);

/*! pop the heap item
 *
 * @param heap      the heap
 */
tb_void_t           tb_heap_pop(tb_heap_ref_t heap);

/*! del the heap item
 *
 * @param heap      the heap
 * @param itor      the itor
 */
tb_void_t           tb_heap_del(tb_heap_ref_t heap, tb_size_t itor);

/*! load heap from the stream
 *
 * @param heap      the heap
 * @param stream    the stream
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_heap_load(tb_heap_ref_t heap, tb_stream_ref_t stream);

/*! save heap to the stream
 *
 * @param heap      the heap
 * @param stream    the stream
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_heap_save(tb_heap_ref_t heap, tb_stream_ref_t stream);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

