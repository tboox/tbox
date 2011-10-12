/*!The Tiny Box Library
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

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */

/* the queue type
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
	tb_byte_t* 		data;

	// the info
	tb_size_t 		step;
	tb_size_t 		head;
	tb_size_t 		tail;
	tb_size_t 		maxn;

	// free
	tb_void_t 		(*free)(tb_void_t* data, tb_void_t* priv);
	tb_void_t* 		priv;

}tb_queue_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_queue_t* 		tb_queue_init(tb_size_t step, tb_size_t maxn, tb_void_t (*free)(tb_void_t* , tb_void_t* ), tb_void_t* priv);
tb_void_t 			tb_queue_exit(tb_queue_t* queue);

// accessors
tb_byte_t* 			tb_queue_at(tb_queue_t* queue, tb_size_t index);
tb_byte_t* 			tb_queue_at_head(tb_queue_t* queue);
tb_byte_t* 			tb_queue_at_last(tb_queue_t* queue);

tb_byte_t const* 	tb_queue_const_at(tb_queue_t const* queue, tb_size_t index);
tb_byte_t const* 	tb_queue_const_at_head(tb_queue_t const* queue);
tb_byte_t const* 	tb_queue_const_at_last(tb_queue_t const* queue);

// modifiors
tb_void_t 			tb_queue_clear(tb_queue_t* queue);

tb_void_t 	 		tb_queue_put(tb_queue_t* queue, tb_byte_t const* item);
tb_void_t 	 		tb_queue_pop(tb_queue_t* queue, tb_byte_t* item);

/* iterator
 * 
 * tb_size_t itor = tb_queue_head(queue);
 * tb_size_t tail = tb_queue_tail(queue);
 * for (; itor != tail; itor = tb_queue_next(queue, itor))
 * {
 * 		tb_byte_t const* item = tb_queue_const_at(queue, itor);
 * 		if (item)
 * 		{
 * 			// ...
 * 		}
 * }
 *
 * tb_size_t itor = 0;
 * tb_size_t size = tb_queue_size(queue);
 * for (itor = 0; itor < size; itor++)
 * {
 * 		// ...
 * }
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
 */
tb_size_t 			tb_queue_head(tb_queue_t const* queue);
tb_size_t 			tb_queue_tail(tb_queue_t const* queue);
tb_size_t 			tb_queue_last(tb_queue_t const* queue);
tb_size_t 			tb_queue_size(tb_queue_t const* queue);
tb_size_t 			tb_queue_next(tb_queue_t const* queue, tb_size_t index);
tb_size_t 			tb_queue_prev(tb_queue_t const* queue, tb_size_t index);

// attributes
tb_size_t 			tb_queue_maxn(tb_queue_t const* queue);
tb_bool_t 			tb_queue_full(tb_queue_t const* queue);
tb_bool_t 			tb_queue_null(tb_queue_t const* queue);



// c plus plus
#ifdef __cplusplus
}
#endif

#endif

