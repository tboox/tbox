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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		heap.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_HEAP_H
#define TB_CONTAINER_HEAP_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the heap type
typedef tb_void_t 	tb_heap_t;

/// the heap mode enum
typedef enum __tb_heap_mode_e
{
	TB_HEAP_MODE_MIN 	= 0 	//!< the min-heap
,	TB_HEAP_MODE_MAX 	= 1 	//!< the max-heap

}tb_heap_mode_e;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init heap, default: minheap
 *
 * @param grow 		the item grow
 * @param func 		the item func
 *
 * @return 			the heap
 */
tb_heap_t* 			tb_heap_init(tb_size_t grow, tb_item_func_t func);

/*! exist heap
 *
 * @param heap 		the heap
 */
tb_void_t 			tb_heap_exit(tb_heap_t* heap);

/*! clear the heap
 *
 * @param heap 		the heap
 */
tb_void_t 			tb_heap_clear(tb_heap_t* heap);

/*! the heap size
 *
 * @param heap 		the heap
 *
 * @return 			the heap size
 */
tb_size_t 			tb_heap_size(tb_heap_t const* heap);

/*! the heap grow
 *
 * @param heap 		the heap
 *
 * @return 			the heap grow
 */
tb_size_t 			tb_heap_grow(tb_heap_t const* heap);

/*! the heap maxn
 *
 * @param heap 		the heap
 *
 * @return 			the heap maxn
 */
tb_size_t 			tb_heap_maxn(tb_heap_t const* heap);

/*! the heap top item
 *
 * @param heap 		the heap
 *
 * @return 			the heap top item
 */
tb_pointer_t 		tb_heap_top(tb_heap_t* heap);

/*! put the heap item
 *
 * @param heap 		the heap
 * @param data 		the item data
 */
tb_void_t 	 		tb_heap_put(tb_heap_t* heap, tb_cpointer_t data);

/*! pop the heap item
 *
 * @param heap 		the heap
 */
tb_void_t 	 		tb_heap_pop(tb_heap_t* heap);

/*! del the heap item
 *
 * @param heap 		the heap
 * @param itor 		the itor
 */
tb_void_t 	 		tb_heap_del(tb_heap_t* heap, tb_size_t itor);

/*!walk
 *
 * be faster than the iterator mode, optimizate to remove items for walking
 *
 * @code
 * tb_bool_t tb_heap_item_func(tb_heap_t* heap, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
 * {
 * 		tb_assert_and_check_return_val(heap && bdel, tb_false);
 *
 * 		// is tail?
 * 		if (!item) ;
 *
 * 		// delete it?
 * 		// *bdel = tb_true;
 *
 * 		// ok
 * 		return tb_true;
 *
 * fail:
 * 		// break
 * 		return tb_false;
 * }
 * @endcode
 *
 * @param heap 		the heap
 * @param func 		the walk func
 * @param data 		the walk data
 */
tb_void_t 			tb_heap_walk(tb_heap_t* heap, tb_bool_t (*func)(tb_heap_t* heap, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data);



#endif

