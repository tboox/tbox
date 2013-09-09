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
 * @file		queue.c
 * @ingroup 	container
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "queue.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*! the queue type
 *
 * <pre>
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
 * </pre>
 *
 * @note the index of the same item is mutable
 *
 */
typedef struct __tb_queue_impl_t
{
	// the itor
	tb_iterator_t 			itor;

	// the data
	tb_byte_t* 				data;
	tb_size_t 				head;
	tb_size_t 				tail;
	tb_size_t 				maxn;

	// the func
	tb_item_func_t 			func;

}tb_queue_impl_t;

/* ///////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_queue_iterator_head(tb_iterator_t* iterator)
{
	tb_queue_impl_t* queue = (tb_queue_impl_t*)iterator;
	tb_assert_and_check_return_val(queue, 0);

	return queue->head;
}
static tb_size_t tb_queue_iterator_tail(tb_iterator_t* iterator)
{
	tb_queue_impl_t* queue = (tb_queue_impl_t*)iterator;
	tb_assert_and_check_return_val(queue, 0);

	return queue->tail;
}
static tb_size_t tb_queue_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_queue_impl_t* queue = (tb_queue_impl_t*)iterator;
	tb_assert_and_check_return_val(queue, 0);

	return ((itor + 1) & (queue->maxn - 1));
}
static tb_size_t tb_queue_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_queue_impl_t* queue = (tb_queue_impl_t*)iterator;
	tb_assert_and_check_return_val(queue, 0);

	return ((itor + queue->maxn - 1) & (queue->maxn - 1));
}
static tb_pointer_t tb_queue_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_queue_impl_t* queue = (tb_queue_impl_t*)iterator;
	tb_assert_and_check_return_val(queue && itor < queue->maxn, tb_null);
	return queue->func.data(&queue->func, queue->data + itor * iterator->step);
}
static tb_void_t tb_queue_iterator_move(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
	tb_queue_impl_t* queue = (tb_queue_impl_t*)iterator;
	tb_assert_return(queue);

	if (iterator->step > sizeof(tb_pointer_t))
	{
		tb_assert_return(item);
		tb_memcpy(queue->data + itor * iterator->step, item, iterator->step);
	}
	else *((tb_pointer_t*)(queue->data + itor * iterator->step)) = item;
}
static tb_long_t tb_queue_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	tb_queue_impl_t* queue = (tb_queue_impl_t*)iterator;
	tb_assert_and_check_return_val(queue && queue->func.comp, 0);
	return queue->func.comp(&queue->func, ltem, rtem);
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_queue_t* tb_queue_init(tb_size_t maxn, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(maxn, tb_null);
	tb_assert_and_check_return_val(func.size && func.dupl && func.data, tb_null);

	// alloc queue
	tb_queue_impl_t* queue = (tb_queue_impl_t*)tb_malloc0(sizeof(tb_queue_impl_t));
	tb_assert_and_check_return_val(queue, tb_null);

	// init queue
	queue->maxn = tb_align_pow2(maxn + 1); // + tail
	queue->func = func;
	tb_assert_and_check_goto(tb_ispow2(queue->maxn), fail);

	// init iterator
	queue->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
	queue->itor.size = 0;
	queue->itor.priv = tb_null;
	queue->itor.step = func.size;
	queue->itor.head = tb_queue_iterator_head;
	queue->itor.tail = tb_queue_iterator_tail;
	queue->itor.prev = tb_queue_iterator_prev;
	queue->itor.next = tb_queue_iterator_next;
	queue->itor.item = tb_queue_iterator_item;
	queue->itor.move = tb_queue_iterator_move;
	queue->itor.comp = tb_queue_iterator_comp;

	// calloc data
	queue->data = tb_nalloc0(queue->maxn, func.size);
	tb_assert_and_check_goto(queue->data, fail);

	return queue;
fail:
	if (queue) tb_queue_exit(queue);
	return tb_null;
}

tb_void_t tb_queue_exit(tb_queue_t* handle)
{	
	tb_queue_impl_t* queue = (tb_queue_impl_t*)handle;
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
tb_void_t tb_queue_clear(tb_queue_t* handle)
{
	tb_queue_impl_t* queue = (tb_queue_impl_t*)handle;
	if (queue) 
	{
		while (!tb_queue_null(queue)) tb_queue_pop(queue);
		queue->head = 0;
		queue->tail = 0;
	}
}
tb_void_t tb_queue_put(tb_queue_t* handle, tb_cpointer_t data)
{	
	// check
	tb_queue_impl_t* queue = (tb_queue_impl_t*)handle;
	tb_assert_and_check_return(queue && !tb_queue_full(queue));

	queue->func.dupl(&queue->func, queue->data + queue->tail * queue->func.size, data);
	queue->tail = ((queue->tail + 1) & (queue->maxn - 1));
}
tb_void_t tb_queue_pop(tb_queue_t* handle)
{	
	// check
	tb_queue_impl_t* queue = (tb_queue_impl_t*)handle;
	tb_assert_and_check_return(queue && !tb_queue_null(queue));

	if (queue->func.free) queue->func.free(&queue->func, queue->data + queue->head * queue->func.size);
	queue->head = ((queue->head + 1) & (queue->maxn - 1));
}
tb_pointer_t tb_queue_get(tb_queue_t const* handle)
{
	return tb_queue_head(handle);
}
tb_pointer_t tb_queue_head(tb_queue_t const* handle)
{
	return tb_iterator_item(handle, tb_iterator_head(handle));
}
tb_pointer_t tb_queue_last(tb_queue_t const* handle)
{
	return tb_iterator_item(handle, tb_iterator_last(handle));
}
tb_size_t tb_queue_size(tb_queue_t const* handle)
{	
	// check
	tb_queue_impl_t* queue = (tb_queue_impl_t*)handle;
	tb_assert_and_check_return_val(queue, 0);

	return ((queue->tail + queue->maxn - queue->head) & (queue->maxn - 1));
}
tb_size_t tb_queue_maxn(tb_queue_t const* handle)
{	
	// check
	tb_queue_impl_t* queue = (tb_queue_impl_t*)handle;
	tb_assert_and_check_return_val(queue, 0);

	return (queue->maxn? queue->maxn - 1 : 0);
}
tb_bool_t tb_queue_full(tb_queue_t const* handle)
{	
	// check
	tb_queue_impl_t* queue = (tb_queue_impl_t*)handle;
	tb_assert_and_check_return_val(queue, tb_true);

	return ((queue->head == ((queue->tail + 1) & (queue->maxn - 1)))? tb_true : tb_false);
}
tb_bool_t tb_queue_null(tb_queue_t const* handle)
{	
	// check
	tb_queue_impl_t* queue = (tb_queue_impl_t*)handle;
	tb_assert_and_check_return_val(queue, tb_true);

	return ((queue->head == queue->tail)? tb_true : tb_false);
}
tb_void_t tb_queue_remove(tb_queue_t* handle, tb_size_t itor)
{
	tb_trace_noimpl();
}
tb_void_t tb_queue_walk(tb_queue_t* handle, tb_bool_t (*func)(tb_queue_t* handle, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data)
{
	// check
	tb_queue_impl_t* queue = (tb_queue_impl_t*)handle;
	tb_assert_and_check_return(queue && queue->data && queue->maxn && func);

	// step
	tb_size_t step = queue->func.size;
	tb_assert_and_check_return(step);

	// walk
	tb_bool_t 	bdel = tb_false;
	tb_byte_t* 	base = queue->data;
	tb_size_t 	itor = queue->head;
	tb_size_t 	tail = queue->tail;
	tb_size_t 	maxn = queue->maxn;
	for (; itor != tail; itor = (itor + 1) & (maxn - 1))
	{
		// item
		tb_pointer_t item = queue->func.data(&queue->func, base + itor * step);

		// bdel
		bdel = tb_false;

		// callback: item
		if (!func(queue, &item, &bdel, data)) goto end;

		// free it?
		if (bdel)
		{
			tb_trace_noimpl();
		}
	}

	// callback: tail
	if (!func(queue, tb_null, &bdel, data)) goto end;

end:
	return ;
}
