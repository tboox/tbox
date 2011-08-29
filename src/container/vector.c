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
#include "../memory/memory.h"
#include "../utils/utils.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// max size
#define TB_VECTOR_MAX_SIZE 		(1 << 30)

// do ctor
#define TB_VECTOR_DO_CTOR(b, n) \
{ \
	do \
	{ \
		if (vector->ctor && (n)) \
		{ \
			tb_size_t 	_i = 0; \
			tb_size_t 	_m = (n); \
			tb_size_t 	_s = vector->step; \
			tb_byte_t* 	_b = vector->data + (b) * _s; \
			for (_i = 0; _i < _m; _i++) \
				vector->ctor(_b + _i * _s, vector->priv); \
		} \
	 \
	} while (0); \
} 


// do dtor
#define TB_VECTOR_DO_DTOR(b, n) \
{ \
	do \
	{ \
		if (vector->dtor && (n)) \
		{ \
			tb_size_t 	_i = 0; \
			tb_size_t 	_m = (n); \
			tb_size_t 	_s = vector->step; \
			tb_byte_t* 	_b = vector->data + (b) * _s; \
			for (_i = 0; _i < _m; _i++) \
				vector->dtor(_b + _i * _s, vector->priv); \
		} \
	 \
	} while (0); \
} 


/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_vector_t* tb_vector_create(tb_size_t step, tb_size_t grow, tb_void_t (*ctor)(tb_void_t* , tb_void_t* ), tb_void_t (*dtor)(tb_void_t* , tb_void_t* ), tb_void_t* priv)
{
	tb_vector_t* vector = (tb_vector_t*)tb_calloc(1, sizeof(tb_vector_t));
	TB_ASSERT_RETURN_VAL(vector, TB_NULL);

	// init vector
	vector->step = step;
	vector->grow = grow;
	vector->size = 0;
	vector->maxn = grow;
	vector->ctor = ctor;
	vector->dtor = dtor;
	vector->priv = priv;
	TB_ASSERT_GOTO(vector->maxn < TB_VECTOR_MAX_SIZE, fail);

	// calloc data
	vector->data = tb_calloc(vector->maxn, vector->step);
	TB_ASSERT_GOTO(vector->data, fail);

	return vector;
fail:
	if (vector) tb_vector_destroy(vector);
	return TB_NULL;
}

tb_void_t tb_vector_destroy(tb_vector_t* vector)
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
		// do dtor
		TB_VECTOR_DO_DTOR(0, vector->size);
		vector->size = 0;
	}
}
tb_vector_t* tb_vector_duplicate(tb_vector_t const* vector)
{
	// alloc
	tb_vector_t* dup = (tb_vector_t*)tb_calloc(1, sizeof(tb_vector_t));
	TB_ASSERT_RETURN_VAL(dup, TB_NULL);

	// copy info
	tb_memcpy(dup, vector, sizeof(tb_vector_t));

	// copy data
	dup->data = tb_calloc(vector->maxn, vector->step);
	TB_ASSERT_GOTO(dup->data, fail);
	tb_memcpy(dup->data, vector->data, vector->size * vector->step);

	return dup;
fail:
	if (dup) tb_free(dup);
	return TB_NULL;
}
tb_byte_t* tb_vector_at(tb_vector_t* vector, tb_size_t index)
{
	TB_ASSERTA(vector && vector->size && index < vector->maxn);
	return (vector->data + index * vector->step);
}
tb_byte_t const* tb_vector_const_at(tb_vector_t const* vector, tb_size_t index)
{
	TB_ASSERTA(vector && vector->size && index < vector->maxn);
	return (vector->data + index * vector->step);
}
tb_size_t tb_vector_head(tb_vector_t const* vector)
{
	TB_ASSERTA(vector);
	return 0;
}
tb_size_t tb_vector_last(tb_vector_t const* vector)
{
	TB_ASSERTA(vector && vector->size);
	return (vector->size - 1);
}
tb_size_t tb_vector_tail(tb_vector_t const* vector)
{
	TB_ASSERTA(vector);
	return vector->size;
}
tb_size_t tb_vector_next(tb_vector_t const* vector, tb_size_t index)
{
	TB_ASSERTA(vector);
	return (index < vector->size)? (index + 1) : vector->size;
}
tb_size_t tb_vector_prev(tb_vector_t const* vector, tb_size_t index)
{
	TB_ASSERTA(vector);
	return (index)? (index - 1) : 0;
}
tb_size_t tb_vector_size(tb_vector_t const* vector)
{
	TB_ASSERTA(vector);
	return vector->size;
}
tb_size_t tb_vector_maxn(tb_vector_t const* vector)
{
	TB_ASSERTA(vector);
	return vector->maxn;
}
tb_bool_t tb_vector_resize(tb_vector_t* vector, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(vector, TB_FALSE);
	
	// destruct items if the vector is decreased
	if (size < vector->size)
	{
		// do dtor
		TB_VECTOR_DO_DTOR(size, vector->size);
	}

	// resize buffer
	if (size > vector->maxn)
	{
		tb_size_t omaxn = vector->maxn;
		vector->maxn = tb_align4(size + vector->grow);
		TB_ASSERT_RETURN_VAL(vector->maxn < TB_VECTOR_MAX_SIZE, TB_FALSE);

		// realloc data
		vector->data = (tb_byte_t*)tb_realloc(vector->data, vector->maxn * vector->step);
		TB_ASSERT_RETURN_VAL(vector->data, TB_FALSE);

		// must be align by 4-bytes
		TB_ASSERT_RETURN_VAL(!(((tb_size_t)(vector->data)) & 3), TB_FALSE);

		// clear the grow data
		tb_memset(vector->data + vector->size * vector->step, 0, (vector->maxn - omaxn) * vector->step);

	}

	// construct the growed items
	if (size > vector->size)
	{
		// do ctor for the grow data
		TB_VECTOR_DO_CTOR(vector->size, size - vector->size);
	}

	// update size
	vector->size = size;
	return TB_TRUE;
}
tb_void_t tb_vector_insert(tb_vector_t* vector, tb_size_t index, tb_byte_t const* item)
{
	TB_ASSERT_RETURN(vector && item && index <= vector->size);

	// save size
	tb_size_t osize = vector->size;

	// grow a item
	if (!tb_vector_resize(vector, osize + 1)) 
	{
		TB_ABORT();
	}

	// desctruct the tail item
	tb_byte_t* 	data = vector->data;
	tb_size_t 	step = vector->step;
	TB_ASSERT_RETURN(data && step);
	if (vector->dtor) vector->dtor(data + osize * step, vector->priv);

	// move items if not at tail
	if (osize != index) tb_memmov(data + (index + 1) * step, data + index * step, (osize - index) * step);

	// copy data
	tb_memcpy(data + index * step, item, step);
}
tb_void_t tb_vector_insert_head(tb_vector_t* vector, tb_byte_t const* item)
{
	TB_ASSERT_RETURN(vector && item);
	tb_vector_insert(vector, 0, item);
}
tb_void_t tb_vector_insert_tail(tb_vector_t* vector, tb_byte_t const* item)
{
	TB_ASSERT_RETURN(vector && item);
	tb_vector_insert(vector, vector->size, item);
}
tb_void_t tb_vector_ninsert(tb_vector_t* vector, tb_size_t index, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN(vector && item && size && index <= vector->size);

	// save size
	tb_size_t osize = vector->size;

	// grow size
	if (!tb_vector_resize(vector, osize + size)) 
	{
		TB_ABORT();
	}

	// do dtors
	TB_VECTOR_DO_DTOR(osize, size);

	// move items if not at tail
	tb_byte_t* 	data = vector->data;
	tb_size_t 	step = vector->step;
	TB_ASSERT_RETURN(data && step);
	if (osize != index) tb_memmov(data + (index + size) * step, data + index * step, (osize - index) * step);

	// copy data	
	switch (step)
	{
	case 4:
		tb_memset_u32(data + (index << 2), tb_bits_get_u32_ne(item), size);
		break;
	case 2:
		tb_memset_u16(data + (index << 1), tb_bits_get_u16_ne(item), size);
		break;
	case 1:
		tb_memset(data + index, tb_bits_get_u8(item), size);
		break;
	default:
		while (size--) tb_memcpy(data + (index++) * step, item, step);
		break;
	}
}
tb_void_t tb_vector_ninsert_head(tb_vector_t* vector, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN(vector && item);
	tb_vector_ninsert(vector, 0, item, size);
}
tb_void_t tb_vector_ninsert_tail(tb_vector_t* vector, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN(vector && item);
	tb_vector_ninsert(vector, vector->size, item, size);
}
tb_void_t tb_vector_replace(tb_vector_t* vector, tb_size_t index, tb_byte_t const* item)
{
	TB_ASSERT_RETURN(vector && vector->data && item && index <= vector->size);

	tb_byte_t* 	data = vector->data;
	tb_size_t 	step = vector->step;
	TB_ASSERT_RETURN(data && step);

	// do dtor
	if (vector->dtor) vector->dtor(data + index * step, vector->priv);

	// copy data
	tb_memcpy(data + index * step, item, step);
}
tb_void_t tb_vector_replace_head(tb_vector_t* vector, tb_byte_t const* item)
{
	TB_ASSERT_RETURN(vector && vector->size && item);
	tb_vector_replace(vector, 0, item);
}
tb_void_t tb_vector_replace_last(tb_vector_t* vector, tb_byte_t const* item)
{
	TB_ASSERT_RETURN(vector && vector->size && item);
	tb_vector_replace(vector, vector->size - 1, item);
}
tb_void_t tb_vector_nreplace(tb_vector_t* vector, tb_size_t index, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN(vector && vector->size && index <= vector->size && item && size);

	// strip size
	if (index + size > vector->size) size = vector->size - index;

	// do dtors
	TB_VECTOR_DO_DTOR(index, size);

	// copy data
	tb_byte_t* 	data = vector->data;
	tb_size_t 	step = vector->step;
	TB_ASSERT_RETURN(data && step);
	switch (step)
	{
	case 4:
		tb_memset_u32(data + (index << 2), tb_bits_get_u32_ne(item), size);
		break;
	case 2:
		tb_memset_u16(data + (index << 1), tb_bits_get_u16_ne(item), size);
		break;
	case 1:
		tb_memset(data + index, tb_bits_get_u8(item), size);
		break;
	default:
		while (size--) tb_memcpy(data + (index++) * step, item, step);
		break;
	}
}
tb_void_t tb_vector_nreplace_head(tb_vector_t* vector, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN(vector && vector->size && item && size);
	tb_vector_nreplace(vector, 0, item, size);
}
tb_void_t tb_vector_nreplace_last(tb_vector_t* vector, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN(vector && vector->size && item && size);
	tb_vector_nreplace(vector, size >= vector->size? 0 : vector->size - size, item, size);
}
tb_void_t tb_vector_remove(tb_vector_t* vector, tb_size_t index)
{	
	TB_ASSERT_RETURN(vector && index < vector->size);
	if (vector->size)
	{
		// do dtor
		if (vector->dtor) vector->dtor(vector->data + index * vector->step, vector->priv);

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
	TB_ASSERT_RETURN(vector);
	if (vector->size)
	{
		// do dtor
		if (vector->dtor) vector->dtor(vector->data + (vector->size - 1) * vector->step, vector->priv);

		// resize
		vector->size--;
	}
}
tb_void_t tb_vector_nremove(tb_vector_t* vector, tb_size_t index, tb_size_t size)
{
	TB_ASSERT_RETURN(vector && size && index < vector->size);

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

	// do dtor
	TB_VECTOR_DO_DTOR(index, size);

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
	TB_ASSERT_RETURN(vector && size);

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
	TB_ASSERT_RETURN(vector && size);

	// clear it
	if (size >= vector->size)
	{
		tb_vector_clear(vector);
		return ;
	}

	// remove last
	tb_vector_nremove(vector, vector->size - size, size);
}
