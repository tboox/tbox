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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		queue.h
 *
 */
#ifndef TB_CONTAINER_QUEUE_H
#define TB_CONTAINER_QUEUE_H

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_QUEUE_SIZE_MICRO 				(63)
#define TB_QUEUE_SIZE_SMALL 				(255)
#define TB_QUEUE_SIZE_LARGE 				(65535)

#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_QUEUE_SIZE_DEFAULT 			TB_QUEUE_SIZE_SMALL
#else
# 	define TB_QUEUE_SIZE_DEFAULT 			TB_QUEUE_SIZE_LARGE
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

/* the queue type
 *
 * queue: |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||------|
 *       head                                                           last    tail
 *
 * queue: ||||||||||||||-----|--------------------------||||||||||||||||||||||||||
 *                   last  tail                       head                
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
 *
 * performance: 
 *
 * push: 	fast
 * pop: 	fast
 *
 * iterator:
 * next: 	fast
 * prev: 	fast
 *
 * \note the index of the same item is mutable
 *
 */
typedef struct __tb_queue_t
{
	// the data
	tb_byte_t* 				data;
	tb_size_t 				head;
	tb_size_t 				tail;
	tb_size_t 				maxn;

	// the func
	tb_item_func_t 			func;

}tb_queue_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_queue_t* 		tb_queue_init(tb_size_t maxn, tb_item_func_t func);
tb_void_t 			tb_queue_exit(tb_queue_t* queue);

// accessors & modifiors
tb_pointer_t 		tb_queue_at_head(tb_queue_t* queue);
tb_pointer_t 		tb_queue_at_last(tb_queue_t* queue);

tb_cpointer_t 		tb_queue_const_at_head(tb_queue_t const* queue);
tb_cpointer_t 		tb_queue_const_at_last(tb_queue_t const* queue);

tb_void_t 	 		tb_queue_put(tb_queue_t* queue, tb_cpointer_t data);
tb_void_t 	 		tb_queue_pop(tb_queue_t* queue);
tb_pointer_t 	 	tb_queue_get(tb_queue_t* queue);

tb_void_t 			tb_queue_clear(tb_queue_t* queue);

/* iterator
 * 
 * tb_size_t itor = tb_queue_itor_head(queue);
 * tb_size_t tail = tb_queue_itor_tail(queue);
 * for (; itor != tail; itor = tb_queue_itor_next(queue, itor))
 * {
 * 		tb_cpointer_t data = tb_queue_itor_const_at(queue, itor);
 * 		if (data)
 * 		{
 * 			// ...
 * 		}
 * }
 *
 */
tb_pointer_t 		tb_queue_itor_at(tb_queue_t* queue, tb_size_t itor);
tb_cpointer_t 		tb_queue_itor_const_at(tb_queue_t const* queue, tb_size_t itor);

tb_size_t 			tb_queue_itor_head(tb_queue_t const* queue);
tb_size_t 			tb_queue_itor_tail(tb_queue_t const* queue);
tb_size_t 			tb_queue_itor_last(tb_queue_t const* queue);
tb_size_t 			tb_queue_itor_next(tb_queue_t const* queue, tb_size_t itor);
tb_size_t 			tb_queue_itor_prev(tb_queue_t const* queue, tb_size_t itor);

// attributes
tb_size_t 			tb_queue_size(tb_queue_t const* queue);
tb_size_t 			tb_queue_maxn(tb_queue_t const* queue);
tb_bool_t 			tb_queue_full(tb_queue_t const* queue);
tb_bool_t 			tb_queue_null(tb_queue_t const* queue);



#endif

