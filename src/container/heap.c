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
 * @file		heap.c
 * @ingroup 	container
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "heap.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the item maxn
#define TB_HEAP_ITEM_MAXN				(1 << 30)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*! the heap type
 *
 * <pre>
 * heap:    16      14      10      8       7       9       3       2       4       1
 *
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              14                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8       (last / 2 - 1)7    9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         4     1(last - 1)
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
typedef struct __tb_heap_impl_t
{
	// the itor
	tb_iterator_t 			itor;

	// the data
	tb_byte_t* 				data;

	// the size
	tb_size_t 				size;

	// the maxn
	tb_size_t 				maxn;

	// the grow
	tb_size_t 				grow;

	// the func
	tb_item_func_t 			func;

}tb_heap_impl_t;

/* ///////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_heap_iterator_head(tb_iterator_t* iterator)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
	tb_assert_and_check_return_val(heap, 0);

	// head
	return 0;
}
static tb_size_t tb_heap_iterator_tail(tb_iterator_t* iterator)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
	tb_assert_and_check_return_val(heap, 0);

	// tail
	return heap->size;
}
static tb_size_t tb_heap_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
	tb_assert_and_check_return_val(heap, 0);
	tb_assert_and_check_return_val(itor < heap->size, heap->size);

	// next
	return itor + 1;
}
static tb_size_t tb_heap_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
	tb_assert_and_check_return_val(heap, 0);
	tb_assert_and_check_return_val(itor && itor < heap->size, 0);

	// prev
	return itor - 1;
}
static tb_pointer_t tb_heap_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
	tb_assert_and_check_return_val(heap && itor < heap->size, tb_null);
	
	// data
	return heap->func.data(&heap->func, heap->data + itor * iterator->step);
}
static tb_void_t tb_heap_iterator_move(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
	tb_assert_return(heap);

	// move
	if (iterator->step > sizeof(tb_pointer_t))
	{
		tb_assert_return(item);
		tb_memcpy(heap->data + itor * iterator->step, item, iterator->step);
	}
	else *((tb_pointer_t*)(heap->data + itor * iterator->step)) = item;
}
static tb_long_t tb_heap_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
	tb_assert_and_check_return_val(heap && heap->func.comp, 0);

	// comp
	return heap->func.comp(&heap->func, ltem, rtem);
}
/*! remove the heap item
 *
 * <pre>
 * init:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                           (hole)                       10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8(larger)      7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         4     1(last)
 *
 * after:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              8                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                      (hole)          7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2 (larger)4     1(last)
 *
 * after:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              8                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       4              7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2      (hole)   1(last)
 * 
 * </pre>
 */
static tb_void_t tb_heap_iterator_delt(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)iterator;
	tb_assert_and_check_return(heap && heap->data && heap->size && itor < heap->size);

	// init
	tb_size_t 	hole = itor;
	tb_byte_t* 	data = heap->data;
	tb_size_t 	tail = heap->size;
	tb_size_t 	step = heap->func.size;

	// 2 * hole + 2: the right child node of hole
	tb_size_t 	child = (hole << 1) + 2;
	for (; child < tail; child = (child << 1) + 2)
	{	
		// the larger child node
		if (heap->func.comp(&heap->func, data + child * step, data + (child - 1) * step) < 0) child--;

		// the larger child node => hole
		tb_memcpy(data + hole * step, data + child * step, step);

		// move the hole down to it's larger child node 
		hole = child;
	}

	// no right child node? 
	if (child == tail)
	{	
		// the last child => hole
		tb_memcpy(data + hole * step, data + (tail - 1) * step, step);

		// move hole down to last
		hole = tail - 1;
	}

	// FIXME
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_heap_t* tb_heap_init(tb_size_t mode, tb_size_t grow, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(grow, tb_null);
	tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.copy, tb_null);

	// make heap
	tb_heap_impl_t* heap = (tb_heap_impl_t*)tb_malloc0(sizeof(tb_heap_impl_t));
	tb_assert_and_check_return_val(heap, tb_null);

	// init heap
	heap->size = 0;
	heap->grow = grow;
	heap->maxn = grow;
	heap->func = func;
	tb_assert_and_check_goto(heap->maxn < TB_HEAP_ITEM_MAXN, fail);

	// init iterator
	heap->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
	heap->itor.size = 0;
	heap->itor.priv = tb_null;
	heap->itor.step = func.size;
	heap->itor.head = tb_heap_iterator_head;
	heap->itor.tail = tb_heap_iterator_tail;
	heap->itor.prev = tb_heap_iterator_prev;
	heap->itor.next = tb_heap_iterator_next;
	heap->itor.item = tb_heap_iterator_item;
	heap->itor.move = tb_heap_iterator_move;
	heap->itor.comp = tb_heap_iterator_comp;
	heap->itor.delt = tb_heap_iterator_delt;

	// make data
	heap->data = tb_nalloc0(heap->maxn, func.size);
	tb_assert_and_check_goto(heap->data, fail);

	// ok
	return heap;
fail:
	if (heap) tb_heap_exit(heap);
	return tb_null;
}
tb_void_t tb_heap_exit(tb_heap_t* handle)
{
	tb_heap_impl_t* heap = (tb_heap_impl_t*)handle;
	if (heap)
	{
		// clear data
		tb_heap_clear(heap);

		// free data
		if (heap->data) tb_free(heap->data);
		heap->data = tb_null;

		// free it
		tb_free(heap);
	}
}
tb_void_t tb_heap_clear(tb_heap_t* handle)
{	
	tb_heap_impl_t* heap = (tb_heap_impl_t*)handle;
	if (heap) 
	{
		// free data
		if (heap->func.nfree)
			heap->func.nfree(&heap->func, heap->data, heap->size);

		// reset size 
		heap->size = 0;
	}
}
tb_size_t tb_heap_size(tb_heap_t const* handle)
{
	// check
	tb_heap_impl_t const* heap = (tb_heap_impl_t const*)handle;
	tb_assert_and_check_return_val(heap, 0);

	// size
	return heap->size;
}
tb_size_t tb_heap_grow(tb_heap_t const* handle)
{
	// check
	tb_heap_impl_t const* heap = (tb_heap_impl_t const*)handle;
	tb_assert_and_check_return_val(heap, 0);

	// grow
	return heap->grow;
}
tb_size_t tb_heap_maxn(tb_heap_t const* handle)
{
	// check
	tb_heap_impl_t const* heap = (tb_heap_impl_t const*)handle;
	tb_assert_and_check_return_val(heap, 0);

	// maxn
	return heap->maxn;
}
tb_pointer_t tb_heap_top(tb_heap_t* handle)
{
	return tb_iterator_item(handle, tb_iterator_head(handle));
}
/*! put heap
 *
 * <pre>
 * init:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              14                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8(parent)      7           9             3
 *                   ---------      
 *                  |         |     
 *                  2(last) (hole) <= 11(val)
 * after:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              14(parent)                10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       11(hole)       7           9             3
 *                   ---------      
 *                  |         |    
 *                  2         8(last)
 * </pre>
 */
tb_void_t tb_heap_put(tb_heap_t* handle, tb_cpointer_t data)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)handle;
	tb_assert_and_check_return(heap && heap->data);

	// full? grow it
	if (heap->size == heap->maxn)
	{
		tb_size_t maxn = tb_align4(heap->maxn + heap->grow);
		tb_assert_and_check_return(maxn < TB_HEAP_ITEM_MAXN);

		// realloc data
		heap->data = (tb_byte_t*)tb_ralloc(heap->data, maxn * heap->func.size);
		tb_assert_and_check_return(heap->data);

		// must be align by 4-bytes
		tb_assert_and_check_return(!(((tb_size_t)(heap->data)) & 3));

		// clear the grow data
		tb_memset(heap->data + heap->size * heap->func.size, 0, (maxn - heap->maxn) * heap->func.size);

		// save maxn
		heap->maxn = maxn;
	}

	// check
	tb_assert_and_check_return(heap->size < heap->maxn);

	// (hole - 1) / 2: the parent node of the hole
	tb_size_t 	parent = 0;
	tb_byte_t* 	head = heap->data;
	tb_size_t 	hole = heap->size;
	tb_size_t 	step = heap->func.size;
	for (parent = (hole - 1) >> 1; hole > 0 && (heap->func.comp(&heap->func, head + parent * step, data) < 0); parent = (hole - 1) >> 1)
	{
		// move item: parent => hole
		tb_memcpy(head + hole * step, head + parent * step, step);

		// move node: hole => parent
		hole = parent;
	}

	// save data
	heap->func.dupl(&heap->func, head + hole * step, data);
	heap->size++;
}
tb_void_t tb_heap_pop(tb_heap_t* handle)
{
	tb_heap_iterator_delt(handle, 0);
}
tb_void_t tb_heap_del(tb_heap_t* handle, tb_size_t itor)
{
	tb_heap_iterator_delt(handle, itor);
}
tb_void_t tb_heap_walk(tb_heap_t* handle, tb_bool_t (*func)(tb_heap_t* handle, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data)
{
	// check
	tb_heap_impl_t* heap = (tb_heap_impl_t*)handle;
	tb_assert_and_check_return(heap && heap->data && func);

	// step
	tb_size_t step = heap->func.size;
	tb_assert_and_check_return(step);

	// walk
	tb_size_t 	i = 0;
	tb_size_t 	b = -1;
	tb_size_t 	o = heap->size;
	tb_size_t 	n = heap->size;
	tb_byte_t* 	d = heap->data;
	tb_bool_t 	bdel = tb_false;
	tb_bool_t 	stop = tb_false;
	for (i = 0; i < n; i++)
	{
		// item
		tb_pointer_t item = heap->func.data(&heap->func, d + i * step);

		// bdel
		bdel = tb_false;

		// callback: item
		if (!func(heap, &item, &bdel, data)) stop = tb_true;

		// free it?
		if (bdel)
		{
			// save
			if (b == -1) b = i;

			// free item
			if (heap->func.free) heap->func.free(&heap->func, d + i * step);
		}

		// remove items?
		if (!bdel || i + 1 == n || stop)
		{
			// has deleted items?
			if (b != -1)
			{
				// the removed items end
				tb_size_t e = !bdel? i : i + 1;
				if (e > b)
				{
					// the items number
					tb_size_t m = e - b;
					tb_assert(n >= m);
//					tb_trace("del: b: %u, e: %u, d: %u", b, e, bdel);

					// remove items
					if (e < n) tb_memmov(d + b * step, d + e * step, (n - e) * step);

					// remove all?
					if (n > m) 
					{
						// update the list size
						n -= m;
						heap->size = n;

						// update i
						i = b;
					}
					else
					{
						// update the list size
						n = 0;
						heap->size = 0;
					}
				}
			}

			// reset
			b = -1;

			// stop?
			tb_check_goto(!stop, end);
		}
	}

	// callback: tail
	if (!func(heap, tb_null, &bdel, data)) goto end;

end:

	// the heap have been modified, adjust it 
	if (o != heap->size)
	{
		// TODO
		// adjust heap
	}

	return ;
}
