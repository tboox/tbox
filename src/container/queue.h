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
 * @file		queue.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_QUEUE_H
#define TB_CONTAINER_QUEUE_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_QUEUE_SIZE_MICRO 				(63)
#define TB_QUEUE_SIZE_SMALL 				(255)
#define TB_QUEUE_SIZE_LARGE 				(65535)

#ifdef __tb_small__
# 	define TB_QUEUE_SIZE_DEFAULT 			TB_QUEUE_SIZE_SMALL
#else
# 	define TB_QUEUE_SIZE_DEFAULT 			TB_QUEUE_SIZE_LARGE
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the queue type
typedef tb_void_t 	tb_queue_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init queue
 *
 * @param maxn 		the item maxn
 * @param func 		the item func
 *
 * @return 			the queue
 */
tb_queue_t* 		tb_queue_init(tb_size_t maxn, tb_item_func_t func);

/*! exit queue
 *
 * @param queue 	the queue
 */
tb_void_t 			tb_queue_exit(tb_queue_t* queue);

/*! the queue head item
 *
 * @param queue 	the queue
 *
 * @return 			the head item
 */
tb_pointer_t 		tb_queue_head(tb_queue_t const* queue);

/*! the queue last item
 *
 * @param queue 	the queue
 *
 * @return 			the last item
 */
tb_pointer_t 		tb_queue_last(tb_queue_t const* queue);

/*! clear the queue
 *
 * @param queue 	the queue
 */
tb_void_t 			tb_queue_clear(tb_queue_t* queue);

/*! remove the queue item
 *
 * @param queue 	the queue
 * @param itor 		the item itor
 */
tb_void_t 			tb_queue_remove(tb_queue_t* queue, tb_size_t itor);

/*! put the queue item
 *
 * @param queue 	the queue
 * @param data 		the item data
 */
tb_void_t 	 		tb_queue_put(tb_queue_t* queue, tb_cpointer_t data);

/*! pop the queue item
 *
 * @param queue 	the queue
 */
tb_void_t 	 		tb_queue_pop(tb_queue_t* queue);

/*! get the queue item
 *
 * @param queue 	the queue
 *
 * @return 			the queue item
 */
tb_pointer_t 	 	tb_queue_get(tb_queue_t const* queue);

/*! the queue size
 *
 * @param queue 	the queue
 *
 * @return 			the queue size
 */
tb_size_t 			tb_queue_size(tb_queue_t const* queue);

/*! the queue maxn
 *
 * @param queue 	the queue
 *
 * @return 			the queue maxn
 */
tb_size_t 			tb_queue_maxn(tb_queue_t const* queue);

/*! the queue full?
 *
 * @param queue 	the queue
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_queue_full(tb_queue_t const* queue);

/*! the queue null?
 *
 * @param queue 	the queue
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_queue_null(tb_queue_t const* queue);

/*! walk the queue
 *
 * be faster than the iterator mode, optimizate to remove items for walking
 *
 * @code
 * tb_bool_t tb_queue_item_func(tb_queue_t* queue, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
 * {
 * 		tb_assert_and_check_return_val(queue && bdel, tb_false);
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
 * 	fail:
 * 		// break
 * 		return tb_false;
 * }
 * @endcode
 * 
 * @param queue 	the queue
 * @param func 		the walk func
 * @param data 		the walk data
 *
 */
tb_void_t 			tb_queue_walk(tb_queue_t* queue, tb_bool_t (*func)(tb_queue_t* queue, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data);



#endif

