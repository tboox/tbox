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
 * \file		vector.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "vector.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// max size
#define TB_VECTOR_MAX_SIZE 		(1 << 30)

// do free
#define TB_VECTOR_DO_FREE(b, n) \
{ \
	do \
	{ \
		if (vector->func.free && (n)) \
		{ \
			tb_size_t 	_i = 0; \
			tb_size_t 	_m = (n); \
			tb_size_t 	_s = vector->step; \
			tb_byte_t* 	_b = vector->data + (b) * _s; \
			for (_i = 0; _i < _m; _i++) \
				vector->func.free(_b + _i * _s, vector->func.priv); \
		} \
	 \
	} while (0); \
} 


/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_vector_t* tb_vector_init(tb_size_t step, tb_size_t grow, tb_vector_item_func_t const* func)
{
	tb_vector_t* vector = (tb_vector_t*)tb_calloc(1, sizeof(tb_vector_t));
	tb_assert_and_check_return_val(vector, TB_NULL);

	// init vector
	vector->step = step;
	vector->grow = grow;
	vector->size = 0;
	vector->maxn = grow;
	if (func) vector->func = *func;
	tb_assert_and_check_goto(vector->maxn < TB_VECTOR_MAX_SIZE, fail);

	// calloc data
	vector->data = tb_calloc(vector->maxn, vector->step);
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
		// do free
		TB_VECTOR_DO_FREE(0, vector->size);
		vector->size = 0;
	}
}
tb_void_t* tb_vector_itor_at(tb_vector_t* vector, tb_size_t itor)
{
	tb_assert_abort(vector && vector->size && itor < vector->maxn);
	return (vector->data + itor * vector->step);
}
tb_void_t* tb_vector_at_head(tb_vector_t* vector)
{
	return tb_vector_itor_at(vector, tb_vector_itor_head(vector));
}
tb_void_t* tb_vector_at_last(tb_vector_t* vector)
{
	return tb_vector_itor_at(vector, tb_vector_itor_last(vector));
}
tb_void_t const* tb_vector_itor_const_at(tb_vector_t const* vector, tb_size_t itor)
{
	tb_assert_abort(vector && vector->size && itor < vector->maxn);
	return (vector->data + itor * vector->step);
}
tb_void_t const* tb_vector_const_at_head(tb_vector_t const* vector)
{
	return tb_vector_itor_const_at(vector, tb_vector_itor_head(vector));
}
tb_void_t const* tb_vector_const_at_last(tb_vector_t const* vector)
{
	return tb_vector_itor_const_at(vector, tb_vector_itor_last(vector));
}
tb_size_t tb_vector_itor_head(tb_vector_t const* vector)
{
	tb_assert_abort(vector);
	return 0;
}
tb_size_t tb_vector_itor_last(tb_vector_t const* vector)
{
	tb_assert_abort(vector && vector->size);
	return (vector->size - 1);
}
tb_size_t tb_vector_itor_tail(tb_vector_t const* vector)
{
	tb_assert_abort(vector);
	return vector->size;
}
tb_size_t tb_vector_itor_next(tb_vector_t const* vector, tb_size_t itor)
{
	tb_assert_abort(vector);
	return (itor < vector->size)? (itor + 1) : vector->size;
}
tb_size_t tb_vector_itor_prev(tb_vector_t const* vector, tb_size_t itor)
{
	tb_assert_abort(vector);
	return (itor)? (itor - 1) : 0;
}
tb_size_t tb_vector_size(tb_vector_t const* vector)
{
	tb_assert_abort(vector);
	return vector->size;
}
tb_size_t tb_vector_maxn(tb_vector_t const* vector)
{
	tb_assert_abort(vector);
	return vector->maxn;
}
tb_bool_t tb_vector_resize(tb_vector_t* vector, tb_size_t size)
{
	tb_assert_and_check_return_val(vector, TB_FALSE);
	
	// free items if the vector is decreased
	if (size < vector->size)
	{
		// do free
		TB_VECTOR_DO_FREE(size, vector->size);
	}

	// resize buffer
	if (size > vector->maxn)
	{
		tb_size_t omaxn = vector->maxn;
		vector->maxn = tb_align4(size + vector->grow);
		tb_assert_and_check_return_val(vector->maxn < TB_VECTOR_MAX_SIZE, TB_FALSE);

		// realloc data
		vector->data = (tb_byte_t*)tb_realloc(vector->data, vector->maxn * vector->step);
		tb_assert_and_check_return_val(vector->data, TB_FALSE);

		// must be align by 4-bytes
		tb_assert_and_check_return_val(!(((tb_size_t)(vector->data)) & 3), TB_FALSE);

		// clear the grow data
		tb_memset(vector->data + vector->size * vector->step, 0, (vector->maxn - omaxn) * vector->step);

	}

	// update size
	vector->size = size;
	return TB_TRUE;
}
tb_void_t tb_vector_insert(tb_vector_t* vector, tb_size_t index, tb_void_t const* item)
{
	tb_assert_and_check_return(vector && index <= vector->size);

	// save size
	tb_size_t osize = vector->size;

	// grow a item
	if (!tb_vector_resize(vector, osize + 1)) 
	{
		tb_abort();
	}

	tb_byte_t* 	data = vector->data;
	tb_size_t 	step = vector->step;
	tb_assert_and_check_return(data && step);

	// move items if not at tail
	if (osize != index) tb_memmov(data + (index + 1) * step, data + index * step, (osize - index) * step);

	// copy data
	if (item) tb_memcpy(data + index * step, item, step);
	else tb_memset(data + index * step, 0, step);
}
tb_void_t tb_vector_insert_head(tb_vector_t* vector, tb_void_t const* item)
{
	tb_assert_and_check_return(vector);
	tb_vector_insert(vector, 0, item);
}
tb_void_t tb_vector_insert_tail(tb_vector_t* vector, tb_void_t const* item)
{
	tb_assert_and_check_return(vector);
	tb_vector_insert(vector, vector->size, item);
}
tb_void_t tb_vector_ninsert(tb_vector_t* vector, tb_size_t index, tb_void_t const* item, tb_size_t size)
{
	tb_assert_and_check_return(vector && size && index <= vector->size);

	// save size
	tb_size_t osize = vector->size;

	// grow size
	if (!tb_vector_resize(vector, osize + size)) 
	{
		tb_abort();
	}

	// move items if not at tail
	tb_byte_t* 	data = vector->data;
	tb_size_t 	step = vector->step;
	tb_assert_and_check_return(data && step);
	if (osize != index) tb_memmov(data + (index + size) * step, data + index * step, (osize - index) * step);

	// copy data
	if (item) 
	{	
		switch (step)
		{
		case 4:
			tb_memset_u32(data + (index << 2), tb_bits_get_u32_ne((tb_byte_t const*)item), size);
			break;
		case 2:
			tb_memset_u16(data + (index << 1), tb_bits_get_u16_ne((tb_byte_t const*)item), size);
			break;
		case 1:
			tb_memset(data + index, tb_bits_get_u8((tb_byte_t const*)item), size);
			break;
		default:
			while (size--) tb_memcpy(data + (index++) * step, item, step);
			break;
		}
	}
	else tb_memset(data + index * step, 0, size * step);
}
tb_void_t tb_vector_ninsert_head(tb_vector_t* vector, tb_void_t const* item, tb_size_t size)
{
	tb_assert_and_check_return(vector);
	tb_vector_ninsert(vector, 0, item, size);
}
tb_void_t tb_vector_ninsert_tail(tb_vector_t* vector, tb_void_t const* item, tb_size_t size)
{
	tb_assert_and_check_return(vector);
	tb_vector_ninsert(vector, vector->size, item, size);
}
tb_void_t tb_vector_replace(tb_vector_t* vector, tb_size_t index, tb_void_t const* item)
{
	tb_assert_and_check_return(vector && vector->data && item && index <= vector->size);

	tb_byte_t* 	data = vector->data;
	tb_size_t 	step = vector->step;
	tb_assert_and_check_return(data && step);

	// do free
	if (vector->func.free) vector->func.free(data + index * step, vector->func.priv);

	// copy data
	tb_memcpy(data + index * step, item, step);
}
tb_void_t tb_vector_replace_head(tb_vector_t* vector, tb_void_t const* item)
{
	tb_assert_and_check_return(vector && vector->size && item);
	tb_vector_replace(vector, 0, item);
}
tb_void_t tb_vector_replace_last(tb_vector_t* vector, tb_void_t const* item)
{
	tb_assert_and_check_return(vector && vector->size && item);
	tb_vector_replace(vector, vector->size - 1, item);
}
tb_void_t tb_vector_nreplace(tb_vector_t* vector, tb_size_t index, tb_void_t const* item, tb_size_t size)
{
	tb_assert_and_check_return(vector && vector->size && index <= vector->size && item && size);

	// strip size
	if (index + size > vector->size) size = vector->size - index;

	// do free
	TB_VECTOR_DO_FREE(index, size);

	// copy data
	tb_byte_t* 	data = vector->data;
	tb_size_t 	step = vector->step;
	tb_assert_and_check_return(data && step);
	switch (step)
	{
	case 4:
		tb_memset_u32(data + (index << 2), tb_bits_get_u32_ne((tb_byte_t const*)item), size);
		break;
	case 2:
		tb_memset_u16(data + (index << 1), tb_bits_get_u16_ne((tb_byte_t const*)item), size);
		break;
	case 1:
		tb_memset(data + index, tb_bits_get_u8((tb_byte_t const*)item), size);
		break;
	default:
		while (size--) tb_memcpy(data + (index++) * step, item, step);
		break;
	}
}
tb_void_t tb_vector_nreplace_head(tb_vector_t* vector, tb_void_t const* item, tb_size_t size)
{
	tb_assert_and_check_return(vector && vector->size && item && size);
	tb_vector_nreplace(vector, 0, item, size);
}
tb_void_t tb_vector_nreplace_last(tb_vector_t* vector, tb_void_t const* item, tb_size_t size)
{
	tb_assert_and_check_return(vector && vector->size && item && size);
	tb_vector_nreplace(vector, size >= vector->size? 0 : vector->size - size, item, size);
}
tb_void_t tb_vector_remove(tb_vector_t* vector, tb_size_t index)
{	
	tb_assert_and_check_return(vector && index < vector->size);
	if (vector->size)
	{
		// do free
		if (vector->func.free) vector->func.free(vector->data + index * vector->step, vector->func.priv);

		// move data if index is not last
		if (index < vector->size - 1) tb_memmov(vector->data + index * vector->step, vector->data + (index + 1) * vector->step, (vector->size - index - 1) * vector->step);
		
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
		if (vector->func.free) vector->func.free(vector->data + (vector->size - 1) * vector->step, vector->func.priv);

		// resize
		vector->size--;
	}
}
tb_void_t tb_vector_nremove(tb_vector_t* vector, tb_size_t index, tb_size_t size)
{
	tb_assert_and_check_return(vector && size && index < vector->size);

	// clear it
	if (!index && size >= vector->size) 
	{
		tb_vector_clear(vector);
		return ;
	}
	
	// strip size
	if (index + size > vector->size) size = vector->size - index;

	// compute the left size
	tb_size_t left = vector->size - index - size;

	// do free
	TB_VECTOR_DO_FREE(index, size);

	// move the left data
	if (left)
	{
		tb_byte_t* pd = vector->data + index * vector->step;
		tb_byte_t* ps = vector->data + (index + size) * vector->step;
		tb_memmov(pd, ps, left * vector->step);
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
