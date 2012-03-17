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
 * \author		ruki
 * \file		vector.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "vector.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// max size
#define TB_VECTOR_MAX_SIZE 		(1 << 30)

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_vector_t* tb_vector_init(tb_size_t grow, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(grow, TB_NULL);
	tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.copy && func.ndupl && func.ncopy, TB_NULL);

	// alloc vector
	tb_vector_t* vector = (tb_vector_t*)tb_malloc0(sizeof(tb_vector_t));
	tb_assert_and_check_return_val(vector, TB_NULL);

	// init vector
	vector->grow = grow;
	vector->size = 0;
	vector->maxn = grow;
	vector->func = func;
	tb_assert_and_check_goto(vector->maxn < TB_VECTOR_MAX_SIZE, fail);

	// calloc data
	vector->data = tb_nalloc0(vector->maxn, func.size);
	tb_assert_and_check_goto(vector->data, fail);

	return vector;
fail:
	if (vector) tb_vector_exit(vector);
	return TB_NULL;
}

tb_void_t tb_vector_exit(tb_vector_t* vector)
{
	if (vector)
	{
		// clear data
		tb_vector_clear(vector);

		// free data
		if (vector->data) tb_free(vector->data);

		// free it
		tb_free(vector);
	}
}
tb_void_t tb_vector_clear(tb_vector_t* vector)
{
	if (vector) 
	{
		// free data
		if (vector->func.nfree)
			vector->func.nfree(&vector->func, vector->data, vector->size);

		// reset size 
		vector->size = 0;
	}
}
tb_pointer_t tb_vector_itor_at(tb_vector_t* vector, tb_size_t itor)
{
	tb_assert_and_check_return_val(vector && vector->size && itor < vector->maxn, 0);
	return vector->func.data(&vector->func, vector->data + itor * vector->func.size);
}
tb_pointer_t tb_vector_data(tb_vector_t* vector)
{
	tb_assert_and_check_return_val(vector, TB_NULL);
	return vector->data;
}
tb_pointer_t tb_vector_at_head(tb_vector_t* vector)
{
	return tb_vector_itor_at(vector, tb_vector_itor_head(vector));
}
tb_pointer_t tb_vector_at_last(tb_vector_t* vector)
{
	return tb_vector_itor_at(vector, tb_vector_itor_last(vector));
}
tb_cpointer_t tb_vector_itor_const_at(tb_vector_t const* vector, tb_size_t itor)
{
	tb_assert_and_check_return_val(vector && vector->size && itor < vector->maxn, 0);
	return vector->func.data(&vector->func, vector->data + itor * vector->func.size);
}
tb_cpointer_t tb_vector_const_at_head(tb_vector_t const* vector)
{
	return tb_vector_itor_const_at(vector, tb_vector_itor_head(vector));
}
tb_cpointer_t tb_vector_const_at_last(tb_vector_t const* vector)
{
	return tb_vector_itor_const_at(vector, tb_vector_itor_last(vector));
}
tb_size_t tb_vector_itor_head(tb_vector_t const* vector)
{
	return 0;
}
tb_size_t tb_vector_itor_last(tb_vector_t const* vector)
{
	tb_assert_and_check_return_val(vector && vector->size, 0);
	return (vector->size - 1);
}
tb_size_t tb_vector_itor_tail(tb_vector_t const* vector)
{
	tb_assert_and_check_return_val(vector, 0);
	return vector->size;
}
tb_size_t tb_vector_itor_next(tb_vector_t const* vector, tb_size_t itor)
{
	tb_assert_and_check_return_val(vector, 0);
	return (itor < vector->size)? (itor + 1) : vector->size;
}
tb_size_t tb_vector_itor_prev(tb_vector_t const* vector, tb_size_t itor)
{
	tb_assert_and_check_return_val(vector, 0);
	return (itor)? (itor - 1) : 0;
}
tb_size_t tb_vector_size(tb_vector_t const* vector)
{
	tb_assert_and_check_return_val(vector, 0);
	return vector->size;
}
tb_size_t tb_vector_maxn(tb_vector_t const* vector)
{
	tb_assert_and_check_return_val(vector, 0);
	return vector->maxn;
}
tb_bool_t tb_vector_resize(tb_vector_t* vector, tb_size_t size)
{
	tb_assert_and_check_return_val(vector, TB_FALSE);
	
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
		tb_size_t omaxn = vector->maxn;
		vector->maxn = tb_align4(size + vector->grow);
		tb_assert_and_check_return_val(vector->maxn < TB_VECTOR_MAX_SIZE, TB_FALSE);

		// realloc data
		vector->data = (tb_byte_t*)tb_ralloc(vector->data, vector->maxn * vector->func.size);
		tb_assert_and_check_return_val(vector->data, TB_FALSE);

		// must be align by 4-bytes
		tb_assert_and_check_return_val(!(((tb_size_t)(vector->data)) & 3), TB_FALSE);

		// clear the grow data
		tb_memset(vector->data + vector->size * vector->func.size, 0, (vector->maxn - omaxn) * vector->func.size);

	}

	// update size
	vector->size = size;
	return TB_TRUE;
}
tb_void_t tb_vector_insert(tb_vector_t* vector, tb_size_t itor, tb_cpointer_t data)
{
	tb_assert_and_check_return(vector && vector->data && vector->func.size && itor <= vector->size);

	// save size
	tb_size_t osize = vector->size;

	// grow a item
	if (!tb_vector_resize(vector, osize + 1)) 
	{
		tb_trace("vector resize: %u => %u failed", osize, osize + 1);
		return ;
	}

	// move items if not at tail
	if (osize != itor) tb_memmov(vector->data + (itor + 1) * vector->func.size, vector->data + itor * vector->func.size, (osize - itor) * vector->func.size);

	// duplicate data
	vector->func.dupl(&vector->func, vector->data + itor * vector->func.size, data);
}
tb_void_t tb_vector_insert_head(tb_vector_t* vector, tb_cpointer_t data)
{
	tb_assert_and_check_return(vector);
	tb_vector_insert(vector, 0, data);
}
tb_void_t tb_vector_insert_tail(tb_vector_t* vector, tb_cpointer_t data)
{
	tb_assert_and_check_return(vector);
	tb_vector_insert(vector, vector->size, data);
}
tb_void_t tb_vector_ninsert(tb_vector_t* vector, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(vector && vector->data && size && itor <= vector->size);

	// save size
	tb_size_t osize = vector->size;

	// grow size
	if (!tb_vector_resize(vector, osize + size)) 
	{
		tb_trace("vector resize: %u => %u failed", osize, osize + 1);
		return ;
	}

	// move items if not at tail
	if (osize != itor) tb_memmov(vector->data + (itor + size) * vector->func.size, vector->data + itor * vector->func.size, (osize - itor) * vector->func.size);

	// duplicate data
	vector->func.ndupl(&vector->func, vector->data + itor * vector->func.size, data, size);
}
tb_void_t tb_vector_ninsert_head(tb_vector_t* vector, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(vector);
	tb_vector_ninsert(vector, 0, data, size);
}
tb_void_t tb_vector_ninsert_tail(tb_vector_t* vector, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(vector);
	tb_vector_ninsert(vector, vector->size, data, size);
}
tb_void_t tb_vector_replace(tb_vector_t* vector, tb_size_t itor, tb_cpointer_t data)
{
	tb_assert_and_check_return(vector && vector->data && itor <= vector->size);

	// copy data
	vector->func.copy(&vector->func, vector->data + itor * vector->func.size, data);
}
tb_void_t tb_vector_replace_head(tb_vector_t* vector, tb_cpointer_t data)
{
	tb_assert_and_check_return(vector && vector->size);
	tb_vector_replace(vector, 0, data);
}
tb_void_t tb_vector_replace_last(tb_vector_t* vector, tb_cpointer_t data)
{
	tb_assert_and_check_return(vector && vector->size);
	tb_vector_replace(vector, vector->size - 1, data);
}
tb_void_t tb_vector_nreplace(tb_vector_t* vector, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(vector && vector->data && vector->size && itor <= vector->size && size);

	// strip size
	if (itor + size > vector->size) size = vector->size - itor;

	// copy data
	vector->func.ncopy(&vector->func, vector->data + itor * vector->func.size, data, size);
}
tb_void_t tb_vector_nreplace_head(tb_vector_t* vector, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(vector && vector->size && size);
	tb_vector_nreplace(vector, 0, data, size);
}
tb_void_t tb_vector_nreplace_last(tb_vector_t* vector, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(vector && vector->size && size);
	tb_vector_nreplace(vector, size >= vector->size? 0 : vector->size - size, data, size);
}
tb_void_t tb_vector_remove(tb_vector_t* vector, tb_size_t itor)
{	
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
tb_void_t tb_vector_remove_head(tb_vector_t* vector)
{
	tb_vector_remove(vector, 0);
}
tb_void_t tb_vector_remove_last(tb_vector_t* vector)
{
	tb_assert_and_check_return(vector);
	if (vector->size)
	{
		// do free
		if (vector->func.free) vector->func.free(&vector->func, vector->data + (vector->size - 1) * vector->func.size);

		// resize
		vector->size--;
	}
}
tb_void_t tb_vector_nremove(tb_vector_t* vector, tb_size_t itor, tb_size_t size)
{
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
tb_void_t tb_vector_nremove_head(tb_vector_t* vector, tb_size_t size)
{
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
tb_void_t tb_vector_nremove_last(tb_vector_t* vector, tb_size_t size)
{	
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
tb_void_t tb_vector_walk(tb_vector_t* vector, tb_bool_t (*func)(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data)
{
	tb_assert_and_check_return(vector && vector->data && func);

	// step
	tb_size_t step = vector->func.size;
	tb_assert_and_check_return(step);

	// walk
	tb_size_t 	i = 0;
	tb_size_t 	b = -1;
	tb_size_t 	n = vector->size;
	tb_byte_t* 	d = vector->data;
	tb_bool_t 	bdel = TB_FALSE;
	for (i = 0; i < n; i++)
	{
		// item
		tb_pointer_t item = vector->func.data(&vector->func, d + i * step);

		// bdel
		bdel = TB_FALSE;

		// callback: item
		if (!func(vector, &item, &bdel, data)) goto end;

		// free it?
		if (bdel)
		{
			// save
			if (b == -1) b = i;

			// free item
			if (vector->func.free) vector->func.free(&vector->func, d + i * step);
		}

		// remove items?
		if (!bdel || i + 1 == n)
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
		}
	}

	// callback: tail
	if (!func(vector, TB_NULL, &bdel, data)) goto end;

end:
	return ;
}
