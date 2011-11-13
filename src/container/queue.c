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
 * \file		queue.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "queue.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_queue_t* tb_queue_init(tb_size_t maxn, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);
	tb_assert_and_check_return_val(func.size && func.dupl && func.data, TB_NULL);

	// alloc queue
	tb_queue_t* queue = (tb_queue_t*)tb_calloc(1, sizeof(tb_queue_t));
	tb_assert_and_check_return_val(queue, TB_NULL);

	// init queue
	queue->maxn = tb_align_pow2(maxn + 1); // + tail
	queue->func = func;
	tb_assert_and_check_goto(tb_ispow2(queue->maxn), fail);

	// calloc data
	queue->data = tb_calloc(queue->maxn, func.size);
	tb_assert_and_check_goto(queue->data, fail);

	return queue;
fail:
	if (queue) tb_queue_exit(queue);
	return TB_NULL;
}

tb_void_t tb_queue_exit(tb_queue_t* queue)
{
	if (queue)
	{
		// clear data
		tb_queue_clear(queue);

		// free data
		if (queue->data) tb_free(queue->data);

		// free it
		tb_free(queue);
	}
}
tb_void_t tb_queue_clear(tb_queue_t* queue)
{
	if (queue) 
	{
		while (!tb_queue_null(queue)) tb_queue_pop(queue);
		queue->head = 0;
		queue->tail = 0;
	}
}
tb_void_t tb_queue_put(tb_queue_t* queue, tb_cpointer_t data)
{
	tb_assert_and_check_return(queue && !tb_queue_full(queue));
	queue->func.dupl(&queue->func, queue->data + queue->tail * queue->func.size, data);
	queue->tail = ((queue->tail + 1) & (queue->maxn - 1));
}
tb_void_t tb_queue_pop(tb_queue_t* queue)
{
	tb_assert_and_check_return(queue && !tb_queue_null(queue));
	if (queue->func.free) queue->func.free(&queue->func, queue->data + queue->head * queue->func.size);
	queue->head = ((queue->head + 1) & (queue->maxn - 1));
}
tb_pointer_t tb_queue_get(tb_queue_t* queue)
{
	return tb_queue_at_head(queue);
}
tb_pointer_t tb_queue_itor_at(tb_queue_t* queue, tb_size_t itor)
{
	return (tb_pointer_t)tb_queue_itor_const_at(queue, itor);
}
tb_pointer_t tb_queue_at_head(tb_queue_t* queue)
{
	return tb_queue_itor_at(queue, tb_queue_itor_head(queue));
}
tb_pointer_t tb_queue_at_last(tb_queue_t* queue)
{
	return tb_queue_itor_at(queue, tb_queue_itor_last(queue));
}
tb_cpointer_t tb_queue_itor_const_at(tb_queue_t const* queue, tb_size_t itor)
{
	tb_assert_abort(queue && !tb_queue_null(queue) && itor < queue->maxn);
	return queue->func.data((tb_item_func_t*)&queue->func, queue->data + itor * queue->func.size);
}
tb_cpointer_t tb_queue_const_at_head(tb_queue_t const* queue)
{
	return tb_queue_itor_const_at(queue, tb_queue_itor_head(queue));
}
tb_cpointer_t tb_queue_const_at_last(tb_queue_t const* queue)
{
	return tb_queue_itor_const_at(queue, tb_queue_itor_last(queue));
}
tb_size_t tb_queue_itor_head(tb_queue_t const* queue)
{
	tb_assert_and_check_return_val(queue, 0);
	return queue->head;
}
tb_size_t tb_queue_itor_last(tb_queue_t const* queue)
{
	tb_assert_and_check_return_val(queue && !tb_queue_null(queue), 0);
	return ((queue->tail + queue->maxn - 1) & (queue->maxn - 1));
}
tb_size_t tb_queue_itor_tail(tb_queue_t const* queue)
{
	tb_assert_and_check_return_val(queue, 0);
	return queue->tail;
}
tb_size_t tb_queue_itor_next(tb_queue_t const* queue, tb_size_t itor)
{
	tb_assert_and_check_return_val(queue, 0);
	return ((itor + 1) & (queue->maxn - 1));
}
tb_size_t tb_queue_itor_prev(tb_queue_t const* queue, tb_size_t itor)
{
	tb_assert_and_check_return_val(queue, 0);
	return ((itor + queue->maxn - 1) & (queue->maxn - 1));
}
tb_size_t tb_queue_size(tb_queue_t const* queue)
{
	tb_assert_and_check_return_val(queue, 0);
	return ((queue->tail + queue->maxn - queue->head) & (queue->maxn - 1));
}
tb_size_t tb_queue_maxn(tb_queue_t const* queue)
{
	tb_assert_and_check_return_val(queue, 0);
	return (queue->maxn? queue->maxn - 1 : 0);
}
tb_bool_t tb_queue_full(tb_queue_t const* queue)
{
	tb_assert_and_check_return_val(queue, TB_TRUE);
	return ((queue->head == ((queue->tail + 1) & (queue->maxn - 1)))? TB_TRUE : TB_FALSE);
}
tb_bool_t tb_queue_null(tb_queue_t const* queue)
{
	tb_assert_and_check_return_val(queue, TB_TRUE);
	return ((queue->head == queue->tail)? TB_TRUE : TB_FALSE);
}

