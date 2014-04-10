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
 * @author		ruki
 * @file		vector.c
 * @ingroup 	container
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "vector.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the item maxn
#define TB_VECTOR_ITEM_MAXN				(1 << 30)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*!the vector type
 *
 * <pre>
 * vector: |-----|--------------------------------------------------------|------|
 *       head                                                           last    tail
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
 *
 * performance: 
 *
 * insert:
 * insert midd: slow
 * insert head: slow
 * insert tail: fast
 *
 * ninsert:
 * ninsert midd: fast
 * ninsert head: fast
 * ninsert tail: fast
 *
 * remove:
 * remove midd: slow
 * remove head: slow
 * remove last: fast
 *
 * nremove:
 * nremove midd: fast
 * nremove head: fast
 * nremove last: fast
 *
 * iterator:
 * next: fast
 * prev: fast
 * </pre>
 *
 * @note the itor of the same item is mutable
 *
 */
typedef struct __tb_vector_impl_t
{
	// the itor
	tb_iterator_t 			itor;

	// the data
	tb_byte_t* 				data;
	tb_size_t 				size;
	tb_size_t 				grow;
	tb_size_t 				maxn;

	// the func
	tb_item_func_t 			func;

}tb_vector_impl_t;

/* ///////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_vector_iterator_head(tb_iterator_t* iterator)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)iterator;
	tb_assert_and_check_return_val(vector, 0);

	// head
	return 0;
}
static tb_size_t tb_vector_iterator_tail(tb_iterator_t* iterator)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)iterator;
	tb_assert_and_check_return_val(vector, 0);

	// tail
	return vector->size;
}
static tb_size_t tb_vector_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)iterator;
	tb_assert_and_check_return_val(vector, 0);
	tb_assert_and_check_return_val(itor < vector->size, vector->size);

	// next
	return itor + 1;
}
static tb_size_t tb_vector_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)iterator;
	tb_assert_and_check_return_val(vector, 0);
	tb_assert_and_check_return_val(itor && itor <= vector->size, 0);

	// prev
	return itor - 1;
}
static tb_pointer_t tb_vector_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)iterator;
	tb_assert_and_check_return_val(vector && itor < vector->size, tb_null);
	
	// data
	return vector->func.data(&vector->func, vector->data + itor * iterator->step);
}
static tb_void_t tb_vector_iterator_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)iterator;
	tb_assert_and_check_return(vector);

	// copy
	vector->func.copy(&vector->func, vector->data + itor * iterator->step, item);
}
static tb_long_t tb_vector_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)iterator;
	tb_assert_and_check_return_val(vector && vector->func.comp, 0);

	// comp
	return vector->func.comp(&vector->func, ltem, rtem);
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_vector_t* tb_vector_init(tb_size_t grow, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(grow, tb_null);
	tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl && func.ndupl && func.nrepl, tb_null);

	// make vector
	tb_vector_impl_t* vector = (tb_vector_impl_t*)tb_malloc0(sizeof(tb_vector_impl_t));
	tb_assert_and_check_return_val(vector, tb_null);

	// init vector
	vector->size = 0;
	vector->grow = grow;
	vector->maxn = grow;
	vector->func = func;
	tb_assert_and_check_goto(vector->maxn < TB_VECTOR_ITEM_MAXN, fail);

	// init iterator
	vector->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
	vector->itor.size = 0;
	vector->itor.priv = tb_null;
	vector->itor.step = func.size;
	vector->itor.head = tb_vector_iterator_head;
	vector->itor.tail = tb_vector_iterator_tail;
	vector->itor.prev = tb_vector_iterator_prev;
	vector->itor.next = tb_vector_iterator_next;
	vector->itor.item = tb_vector_iterator_item;
	vector->itor.copy = tb_vector_iterator_copy;
	vector->itor.comp = tb_vector_iterator_comp;

	// make data
	vector->data = tb_nalloc0(vector->maxn, func.size);
	tb_assert_and_check_goto(vector->data, fail);

	// ok
	return vector;
fail:
	if (vector) tb_vector_exit(vector);
	return tb_null;
}

tb_void_t tb_vector_exit(tb_vector_t* handle)
{
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	if (vector)
	{
		// clear data
		tb_vector_clear(vector);

		// free data
		if (vector->data) tb_free(vector->data);
		vector->data = tb_null;

		// free it
		tb_free(vector);
	}
}
tb_void_t tb_vector_clear(tb_vector_t* handle)
{
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	if (vector) 
	{
		// free data
		if (vector->func.nfree)
			vector->func.nfree(&vector->func, vector->data, vector->size);

		// reset size 
		vector->size = 0;
	}
}
tb_void_t tb_vector_copy(tb_vector_t* handle, tb_vector_t const* hcopy)
{
	// check
	tb_vector_impl_t* 		vector = (tb_vector_impl_t*)handle;
	tb_vector_impl_t const* copy = (tb_vector_impl_t const*)hcopy;
	tb_assert_and_check_return(vector && copy);

	// check func
	tb_assert_and_check_return(vector->func.type == copy->func.type);
	tb_assert_and_check_return(vector->func.size == copy->func.size);

	// check itor
	tb_assert_and_check_return(vector->itor.mode == copy->itor.mode);
	tb_assert_and_check_return(vector->itor.size == copy->itor.size);
	tb_assert_and_check_return(vector->itor.step == copy->itor.step);

	// null? clear it
	if (!copy->size) 
	{
		tb_vector_clear(vector);
		return ;
	}
	
	// resize if small
	if (vector->size < copy->size) tb_vector_resize(vector, copy->size);
	tb_assert_and_check_return(vector->data && copy->data && vector->size >= copy->size);

	// copy data
	if (copy->data != vector->data) tb_memcpy(vector->data, copy->data, copy->size * copy->func.size);

	// copy size
	vector->size = copy->size;
}
tb_pointer_t tb_vector_data(tb_vector_t* handle)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return_val(vector, tb_null);

	// data
	return vector->data;
}
tb_pointer_t tb_vector_head(tb_vector_t* handle)
{
	return tb_iterator_item(handle, tb_iterator_head(handle));
}
tb_pointer_t tb_vector_last(tb_vector_t* handle)
{
	return tb_iterator_item(handle, tb_iterator_last(handle));
}
tb_size_t tb_vector_size(tb_vector_t const* handle)
{
	// check
	tb_vector_impl_t const* vector = (tb_vector_impl_t const*)handle;
	tb_assert_and_check_return_val(vector, 0);

	// size
	return vector->size;
}
tb_size_t tb_vector_grow(tb_vector_t const* handle)
{
	// check
	tb_vector_impl_t const* vector = (tb_vector_impl_t const*)handle;
	tb_assert_and_check_return_val(vector, 0);

	// grow
	return vector->grow;
}
tb_size_t tb_vector_maxn(tb_vector_t const* handle)
{
	// check
	tb_vector_impl_t const* vector = (tb_vector_impl_t const*)handle;
	tb_assert_and_check_return_val(vector, 0);

	// maxn
	return vector->maxn;
}
tb_bool_t tb_vector_resize(tb_vector_t* handle, tb_size_t size)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return_val(vector, tb_false);
	
	// free items if the vector is decreased
	if (size < vector->size)
	{
		// free data
		if (vector->func.nfree) 
			vector->func.nfree(&vector->func, vector->data + size * vector->func.size, vector->size - size);
	}

	// resize buffer
	if (size > vector->maxn)
	{
		tb_size_t maxn = tb_align4(size + vector->grow);
		tb_assert_and_check_return_val(maxn < TB_VECTOR_ITEM_MAXN, tb_false);

		// realloc data
		vector->data = (tb_byte_t*)tb_ralloc(vector->data, maxn * vector->func.size);
		tb_assert_and_check_return_val(vector->data, tb_false);

		// must be align by 4-bytes
		tb_assert_and_check_return_val(!(((tb_size_t)(vector->data)) & 3), tb_false);

		// clear the grow data
		tb_memset(vector->data + vector->size * vector->func.size, 0, (maxn - vector->maxn) * vector->func.size);

		// save maxn
		vector->maxn = maxn;
	}

	// update size
	vector->size = size;
	return tb_true;
}
tb_void_t tb_vector_insert_prev(tb_vector_t* handle, tb_size_t itor, tb_cpointer_t data)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && vector->data && vector->func.size && itor <= vector->size);

	// save size
	tb_size_t osize = vector->size;

	// grow a item
	if (!tb_vector_resize(vector, osize + 1)) 
	{
		tb_trace_d("vector resize: %u => %u failed", osize, osize + 1);
		return ;
	}

	// move items if not at tail
	if (osize != itor) tb_memmov(vector->data + (itor + 1) * vector->func.size, vector->data + itor * vector->func.size, (osize - itor) * vector->func.size);

	// save data
	vector->func.dupl(&vector->func, vector->data + itor * vector->func.size, data);
}
tb_void_t tb_vector_insert_next(tb_vector_t* handle, tb_size_t itor, tb_cpointer_t data)
{
	tb_vector_insert_prev(handle, tb_iterator_next(handle, itor), data);
}
tb_void_t tb_vector_insert_head(tb_vector_t* handle, tb_cpointer_t data)
{
	tb_vector_insert_prev(handle, 0, data);
}
tb_void_t tb_vector_insert_tail(tb_vector_t* handle, tb_cpointer_t data)
{
	tb_vector_insert_prev(handle, tb_vector_size(handle), data);
}
tb_void_t tb_vector_ninsert_prev(tb_vector_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && vector->data && size && itor <= vector->size);

	// save size
	tb_size_t osize = vector->size;

	// grow size
	if (!tb_vector_resize(vector, osize + size)) 
	{
		tb_trace_d("vector resize: %u => %u failed", osize, osize + 1);
		return ;
	}

	// move items if not at tail
	if (osize != itor) tb_memmov(vector->data + (itor + size) * vector->func.size, vector->data + itor * vector->func.size, (osize - itor) * vector->func.size);

	// duplicate data
	vector->func.ndupl(&vector->func, vector->data + itor * vector->func.size, data, size);
}
tb_void_t tb_vector_ninsert_next(tb_vector_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	tb_vector_ninsert_prev(handle, tb_iterator_next(handle, itor), data, size);
}
tb_void_t tb_vector_ninsert_head(tb_vector_t* handle, tb_cpointer_t data, tb_size_t size)
{
	tb_vector_ninsert_prev(handle, 0, data, size);
}
tb_void_t tb_vector_ninsert_tail(tb_vector_t* handle, tb_cpointer_t data, tb_size_t size)
{
	tb_vector_ninsert_prev(handle, tb_vector_size(handle), data, size);
}
tb_void_t tb_vector_replace(tb_vector_t* handle, tb_size_t itor, tb_cpointer_t data)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && vector->data && itor <= vector->size);

	// replace data
	vector->func.repl(&vector->func, vector->data + itor * vector->func.size, data);
}
tb_void_t tb_vector_replace_head(tb_vector_t* handle, tb_cpointer_t data)
{
	tb_vector_replace(handle, 0, data);
}
tb_void_t tb_vector_replace_last(tb_vector_t* handle, tb_cpointer_t data)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && vector->size);

	// replace
	tb_vector_replace(vector, vector->size - 1, data);
}
tb_void_t tb_vector_nreplace(tb_vector_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && vector->data && vector->size && itor <= vector->size && size);

	// strip size
	if (itor + size > vector->size) size = vector->size - itor;

	// replace data
	vector->func.nrepl(&vector->func, vector->data + itor * vector->func.size, data, size);
}
tb_void_t tb_vector_nreplace_head(tb_vector_t* handle, tb_cpointer_t data, tb_size_t size)
{
	tb_vector_nreplace(handle, 0, data, size);
}
tb_void_t tb_vector_nreplace_last(tb_vector_t* handle, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && vector->size && size);

	// replace
	tb_vector_nreplace(vector, size >= vector->size? 0 : vector->size - size, data, size);
}
tb_void_t tb_vector_remove(tb_vector_t* handle, tb_size_t itor)
{	
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && itor < vector->size);

	if (vector->size)
	{
		// do free
		if (vector->func.free) vector->func.free(&vector->func, vector->data + itor * vector->func.size);

		// move data if itor is not last
		if (itor < vector->size - 1) tb_memmov(vector->data + itor * vector->func.size, vector->data + (itor + 1) * vector->func.size, (vector->size - itor - 1) * vector->func.size);

		// resize
		vector->size--;
	}
}
tb_void_t tb_vector_remove_head(tb_vector_t* handle)
{
	tb_vector_remove(handle, 0);
}
tb_void_t tb_vector_remove_last(tb_vector_t* handle)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector);

	if (vector->size)
	{
		// do free
		if (vector->func.free) vector->func.free(&vector->func, vector->data + (vector->size - 1) * vector->func.size);

		// resize
		vector->size--;
	}
}
tb_void_t tb_vector_nremove(tb_vector_t* handle, tb_size_t itor, tb_size_t size)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && size && itor < vector->size);

	// clear it
	if (!itor && size >= vector->size) 
	{
		tb_vector_clear(vector);
		return ;
	}
	
	// strip size
	if (itor + size > vector->size) size = vector->size - itor;

	// compute the left size
	tb_size_t left = vector->size - itor - size;

	// free data
	if (vector->func.nfree)
		vector->func.nfree(&vector->func, vector->data + itor * vector->func.size, size);

	// move the left data
	if (left)
	{
		tb_byte_t* pd = vector->data + itor * vector->func.size;
		tb_byte_t* ps = vector->data + (itor + size) * vector->func.size;
		tb_memmov(pd, ps, left * vector->func.size);
	}

	// update size
	vector->size -= size;
}
tb_void_t tb_vector_nremove_head(tb_vector_t* handle, tb_size_t size)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && size);

	// clear it
	if (size >= vector->size)
	{
		tb_vector_clear(vector);
		return ;
	}

	// remove head
	tb_vector_nremove(vector, 0, size);
}
tb_void_t tb_vector_nremove_last(tb_vector_t* handle, tb_size_t size)
{	
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && size);

	// clear it
	if (size >= vector->size)
	{
		tb_vector_clear(vector);
		return ;
	}

	// remove last
	tb_vector_nremove(vector, vector->size - size, size);
}
tb_void_t tb_vector_walk(tb_vector_t* handle, tb_bool_t (*func)(tb_vector_t* vector, tb_pointer_t item, tb_bool_t* bdel, tb_pointer_t priv), tb_pointer_t priv)
{
	// check
	tb_vector_impl_t* vector = (tb_vector_impl_t*)handle;
	tb_assert_and_check_return(vector && vector->data && func);

	// step
	tb_size_t step = vector->func.size;
	tb_assert_and_check_return(step);

	// walk
	tb_size_t 	i = 0;
	tb_size_t 	b = -1;
	tb_size_t 	n = vector->size;
	tb_byte_t* 	d = vector->data;
	tb_bool_t 	bdel = tb_false;
	tb_bool_t 	stop = tb_false;
	for (i = 0; i < n; i++)
	{
		// item
		tb_pointer_t item = vector->func.data(&vector->func, d + i * step);

		// bdel
		bdel = tb_false;

		// callback: item
		if (!func(vector, item, &bdel, priv)) stop = tb_true;

		// free it?
		if (bdel)
		{
			// save
			if (b == -1) b = i;

			// free item
			if (vector->func.free) vector->func.free(&vector->func, d + i * step);
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
//					tb_trace_d("del: b: %u, e: %u, d: %u", b, e, bdel);

					// remove items
					if (e < n) tb_memmov(d + b * step, d + e * step, (n - e) * step);

					// remove all?
					if (n > m) 
					{
						// update the list size
						n -= m;
						vector->size = n;

						// update i
						i = b;
					}
					else
					{
						// update the list size
						n = 0;
						vector->size = 0;
					}
				}
			}

			// reset
			b = -1;

			// stop?
			tb_check_goto(!stop, end);
		}
	}

end:
	return ;
}
