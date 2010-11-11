/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		array.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "array.h"
#include "math/math.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_array_t* tb_array_create(tb_size_t step, tb_size_t size, tb_size_t grow)
{
	tb_array_t* array = (tb_array_t*)tb_malloc(sizeof(tb_array_t));
	TB_ASSERT(array);
	if (!array) return TB_NULL;
	memset(array, 0, sizeof(tb_array_t));

	// align by 8-byte for info
	TB_ASSERT(!(grow & 7));

	array->step = step;
	array->grow = grow;
	array->size = size;
	array->maxn = TB_MATH_ALIGN(size + grow, 8);

	array->data = tb_malloc(array->maxn * array->step);
	if (!array->data) goto fail;
	memset(array->data, 0, array->maxn * array->step);

	return array;
fail:
	if (array) tb_array_destroy(array);
	return TB_NULL;
}

void tb_array_destroy(tb_array_t* array)
{
	if (array)
	{
		if (array->data) tb_free(array->data);
		tb_free(array);
	}
}

tb_byte_t* tb_array_put(tb_array_t* array, tb_int_t index)
{
	if (index < 0) return TB_FALSE;
	if (index >= array->size && TB_FALSE == tb_array_resize(array, index + 1)) return TB_FALSE;

	tb_byte_t* item = array->data + index * array->step;
	memset(item, 0, array->step);
	return item;
}
tb_byte_t* tb_array_get(tb_array_t* array, tb_int_t index)
{
	TB_ASSERT(array && array->size && index >= 0 && index < array->maxn);

	if (array && array->size && index >= 0 && index < array->maxn)
		return (array->data + index * array->step);
	else return TB_NULL;
}

tb_byte_t* tb_array_push(tb_array_t* array)
{
	return tb_array_put(array, array->size);
}
void tb_array_pop(tb_array_t* array)
{
	if (array && array->size) array->size--;
}

tb_byte_t* tb_array_first(tb_array_t* array)
{
	return tb_array_get(array, 0);
}
tb_byte_t* tb_array_last(tb_array_t* array)
{
	return tb_array_get(array, array->size - 1);
}

tb_size_t tb_array_size(tb_array_t const* array)
{
	if (array) return array->size;
	else return 0;
}
tb_bool_t tb_array_resize(tb_array_t* array, tb_size_t size)
{
	if (!array) return TB_FALSE;
	if (size > array->maxn)
	{
		tb_size_t omaxn = array->maxn;
		array->maxn = TB_MATH_ALIGN(size + array->grow, 8);
		TB_ASSERT(array->maxn <= TB_ARRAY_MAX_SIZE);
		if (array->maxn > TB_ARRAY_MAX_SIZE) return TB_FALSE;

		// realloc data
		array->data = (tb_byte_t*)tb_realloc(array->data, array->maxn * array->step);
		TB_ASSERT(array->data);
		if (!array->data) return TB_FALSE;
		memset(array->data + array->size * array->step, 0, (array->maxn - omaxn) * array->step);
	}
	array->size = size;
	return TB_TRUE;
}

tb_array_t* tb_array_duplicate(tb_array_t* array)
{
	// alloc
	tb_array_t* dup = (tb_array_t*)tb_malloc(sizeof(tb_array_t));
	TB_ASSERT(dup);
	if (!dup) return TB_NULL;

	// copy info
	*dup = *array;

	// copy data
	dup->data = tb_malloc(array->maxn * array->step);
	if (!dup->data) goto fail;
	memcpy(dup->data, array->data, array->size * array->step);

	return dup;
fail:
	if (dup) tb_array_destroy(dup);
	return TB_NULL;
}

