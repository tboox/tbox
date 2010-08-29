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
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		string.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "math.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value string 
#ifdef TB_MEMORY_MODE_SMALL
# 	define TB_STRING_GROW_SIZE 		(64)
#else
# 	define TB_STRING_GROW_SIZE 		(256)
#endif


// the string max size
#define TB_STRING_MAX_SIZE 			(65535 >> 1)
/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_string_t* tb_string_create()
{
	// alloc
	tb_string_t* string = tb_malloc(sizeof(tb_string_t));
	if (!string) return TB_NULL;

	// init
	tb_string_init(string);
	return string;
}
tb_string_t* tb_string_create_from_buffer(tb_byte_t* data, tb_uint16_t size)
{
	// alloc
	tb_string_t* string = tb_malloc(sizeof(tb_string_t));
	if (!string) return TB_NULL;

	// init
	tb_string_init_from_buffer(string, data, size);
	return string;
}
void tb_string_destroy(tb_string_t* string)
{
	if (string) 
	{
		tb_string_uninit(string);
		tb_free(string);
	}
}

void tb_string_init(tb_string_t* string)
{
	if (string) memset(string, 0, sizeof(tb_string_t));
}
void tb_string_init_from_buffer(tb_string_t* string, tb_byte_t* data, tb_uint16_t size)
{
	if (string)
	{
		if (data && size)
		{
			string->data = data;
			string->size = 0;
			string->maxn = size;
			string->owner = 0;
			string->data[0] = '\0';
		}
		else tb_string_init(string);
	}
}
void tb_string_init_stack_string(tb_stack_string_t* string)
{
	tb_string_init_from_buffer(&string->base, string->stack, TB_STRING_STACK_MAX);
}
void tb_string_uninit(tb_string_t* string)
{
	if (string)
	{
		if (string->data && string->owner)
			tb_free(string->data);

		tb_string_init(string);
	}
}

tb_char_t const* tb_string_c_string(tb_string_t const* string)
{
	if (string) return (tb_char_t const*)string->data;
	else return TB_NULL;
}
tb_uint16_t tb_string_size(tb_string_t const* string)
{
	if (string) return string->size;
	else return 0;
}
tb_char_t tb_string_at(tb_string_t const* string, tb_int_t index)
{
	if (TB_FALSE == tb_string_is_null(string) && index >= 0 && index < string->size)
		return string->data[index];
	else return '\0';
}
void tb_string_set(tb_string_t const* string, tb_int_t index, tb_char_t ch)
{
	if (TB_FALSE == tb_string_is_null(string) && index >= 0 && index < string->size)
		string->data[index] = ch;
}
tb_bool_t tb_string_resize(tb_string_t* string, tb_uint16_t size)
{
	if (!string) return TB_FALSE;
	
	// check size
	TB_ASSERT(size < TB_STRING_MAX_SIZE);
	if (size >= TB_STRING_MAX_SIZE) return TB_FALSE;

	// is null?
	if (!string->data) 
	{
		// compute size
		string->size = size;
		string->maxn = TB_MATH_ALIGN(size + TB_STRING_GROW_SIZE, TB_STRING_GROW_SIZE);

		// check maxn
		TB_ASSERT(string->maxn < TB_STRING_MAX_SIZE);
		if (string->maxn >= TB_STRING_MAX_SIZE) goto fail;

		// alloc
		string->data = tb_malloc(string->maxn);
		string->owner = 1;
		if (!string->data) goto fail;
		string->data[string->size] = '\0';
	}
	// readonly => owner
	else if (!string->owner && !string->maxn) 
	{
		tb_byte_t* 	odata = string->data;
		tb_uint16_t osize = string->size;

		// compute size
		string->size = size;
		string->maxn = TB_MATH_ALIGN(size + TB_STRING_GROW_SIZE, TB_STRING_GROW_SIZE);

		// check maxn
		TB_ASSERT(string->maxn < TB_STRING_MAX_SIZE);
		if (string->maxn >= TB_STRING_MAX_SIZE) goto fail;

		// alloc
		string->data = tb_malloc(string->maxn);
		string->owner = 1;
		if (!string->data) goto fail;
		memcpy(string->data, odata, TB_MATH_MIN(osize, size));
		string->data[string->size] = '\0';
	}
	// decrease
	else if (size <= string->maxn)
	{
		string->size = size;
		string->data[string->size] = '\0';
	}
	// external buffer => owner
	else if (!string->owner)
	{
		tb_byte_t* 	odata = string->data;
		tb_uint16_t osize = string->size;

		// compute size
		string->size = size;
		string->maxn = TB_MATH_ALIGN(size + TB_STRING_GROW_SIZE, TB_STRING_GROW_SIZE);

		// check maxn
		TB_ASSERT(string->maxn < TB_STRING_MAX_SIZE);
		if (string->maxn >= TB_STRING_MAX_SIZE) goto fail;

		// alloc
		string->data = tb_malloc(string->maxn);
		string->owner = 1;
		if (!string->data) goto fail;
		memcpy(string->data, odata, TB_MATH_MIN(osize, size));
		string->data[string->size] = '\0';
	}
	// increase
	else
	{
		// compute size
		string->maxn = TB_MATH_ALIGN(size + TB_STRING_GROW_SIZE, TB_STRING_GROW_SIZE);

		// check maxn
		TB_ASSERT(string->maxn < TB_STRING_MAX_SIZE);
		if (string->maxn >= TB_STRING_MAX_SIZE) goto fail;

		// realloc
		string->size = size;
		string->data = tb_realloc(string->data, string->maxn);
		if (!string->data) goto fail;
		else string->data[string->size] = '\0';
	}

	return TB_TRUE;
fail:
	TB_DBG("resize string fail!");
	tb_string_uninit(string);
	return TB_FALSE;
}
void tb_string_clear(tb_string_t* string)
{
	if (string) 
	{
		if (TB_FALSE == tb_string_is_null(string))
		{
			string->size = 0;

			// readonly
			if (!string->owner && !string->maxn) 
				string->data = TB_NULL;
			// clear data
			else if (string->data)
				string->data[0] = '\0';
		}
	}
}
tb_byte_t* tb_string_data(tb_string_t* string)
{
	// only return non-readonly data
	if (string && (string->owner || string->maxn)) return string->data;
	else return TB_NULL;
}
tb_bool_t tb_string_is_null(tb_string_t const* string)
{
	if (!string || !string->size || !string->data)
		return TB_TRUE;
	else return TB_FALSE;
}
tb_char_t const* tb_string_shift(tb_string_t* string, tb_string_t* s_string)
{
	if (string)
	{
		if (s_string)
		{
			*string = *s_string;
			tb_string_init(s_string);
			return tb_string_c_string(string);
		}
		else tb_string_init(string);
	}
	return TB_NULL;
}
tb_char_t const* tb_string_assign(tb_string_t* string, tb_string_t const* s_string)
{
	if (!string) return TB_NULL;
	if (TB_FALSE == tb_string_is_null(s_string))
	{
		// ensure enough size
		if (TB_FALSE == tb_string_resize(string, s_string->size)) return TB_NULL;

		// attach string
		memcpy(string->data, s_string->data, string->size);
		string->data[string->size] = '\0';

		return tb_string_c_string(string);
	}
	else 
	{
		tb_string_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_string_assign_c_string(tb_string_t* string, tb_char_t const* c_string)
{
	if (!string) return TB_NULL;
	if (c_string) 
	{
		// ensure enough size
		if (TB_FALSE == tb_string_resize(string, strlen(c_string))) return TB_NULL;

		// attach string
		memcpy(string->data, c_string, string->size);
		string->data[string->size] = '\0';

		return tb_string_c_string(string);
	}
	else 
	{
		tb_string_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_string_assign_char(tb_string_t* string, tb_char_t ch)
{
	if (!string) return TB_NULL;

	// ensure enough size
	if (TB_FALSE == tb_string_resize(string, 1)) return TB_NULL;

	// attach string
	string->data[0] = ch;
	string->data[1] = '\0';

	return tb_string_c_string(string);
}
tb_char_t const* tb_string_assign_c_string_with_size(tb_string_t* string, tb_char_t const* c_string, tb_size_t size)
{
	if (!string) return TB_NULL;
	if (c_string && size) 
	{
		// ensure enough size
		if (TB_FALSE == tb_string_resize(string, size)) return TB_NULL;

		// attach string
		memcpy(string->data, c_string, string->size);
		string->data[string->size] = '\0';

		return tb_string_c_string(string);
	}
	else 
	{
		tb_string_clear(string);
		return TB_NULL;
	}
}

tb_char_t const* tb_string_assign_by_ref(tb_string_t* string, tb_string_t const* s_string)
{
	if (!string) return TB_NULL;

	// ensure is null
	if (TB_FALSE == tb_string_is_null(string))
		tb_string_uninit(string);

	// attach string
	string->data = (tb_byte_t*)s_string->data;
	string->size = s_string->size;

	// readonly
	string->maxn = 0;
	string->owner = 0;

	return tb_string_c_string(string);
}
tb_char_t const* tb_string_assign_c_string_by_ref(tb_string_t* string, tb_char_t const* c_string)
{
	if (!string) return TB_NULL;
	if (c_string)
	{
		// ensure is null
		if (TB_FALSE == tb_string_is_null(string))
			tb_string_uninit(string);

		// attach string
		string->data = (tb_byte_t*)c_string;
		string->size = strlen(c_string);

		// readonly
		string->maxn = 0;
		string->owner = 0;

		return tb_string_c_string(string);
	}
	else 
	{
		tb_string_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_string_assign_c_string_with_size_by_ref(tb_string_t* string, tb_char_t const* c_string, tb_size_t size)
{
	if (!string) return TB_NULL;
	if (c_string && size)
	{
		// ensure is null
		if (TB_FALSE == tb_string_is_null(string))
			tb_string_uninit(string);

		// attach string
		string->data = (tb_byte_t*)c_string;
		string->size = size;

		// readonly
		string->maxn = 0;
		string->owner = 0;

		return tb_string_c_string(string);
	}
	else 
	{
		tb_string_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_string_append(tb_string_t* string, tb_string_t const* s_string)
{
	if (TB_TRUE == tb_string_is_null(string)) return tb_string_assign(string, s_string);
	else if (TB_FALSE == tb_string_is_null(s_string))
	{
		// get old size
		tb_uint16_t size = string->size;

		// get s_string size
		tb_uint16_t s_size = s_string->size;

		// ensure enough size
		if (TB_FALSE == tb_string_resize(string, size + s_size)) return TB_NULL;

		// append string
		memcpy(string->data + size, s_string->data, s_size);
		string->data[string->size] = '\0';

		return tb_string_c_string(string);
	}
	else 
	{
		tb_string_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_string_append_c_string(tb_string_t* string, tb_char_t const* c_string)
{
	if (TB_TRUE == tb_string_is_null(string)) return tb_string_assign_c_string(string, c_string);
	else if (c_string)
	{
		// get old size
		tb_uint16_t size = string->size;

		// get c_string size
		tb_uint16_t c_size = strlen(c_string);

		// ensure enough size
		if (TB_FALSE == tb_string_resize(string, size + c_size)) return TB_NULL;

		// append string
		memcpy(string->data + size, c_string, c_size);
		string->data[string->size] = '\0';

		return tb_string_c_string(string);
	}
	else 
	{
		tb_string_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_string_append_char(tb_string_t* string, tb_char_t ch)
{
	if (TB_TRUE == tb_string_is_null(string)) return tb_string_assign_char(string, ch);
	else
	{
		// get old size
		tb_uint16_t size = string->size;

		// ensure enough size
		if (TB_FALSE == tb_string_resize(string, size + 1)) return TB_NULL;

		// append string
		string->data[size] = ch;
		string->data[string->size] = '\0';

		return tb_string_c_string(string);
	}
}
tb_bool_t tb_string_subat(tb_string_t const* string, tb_string_t* sub, tb_int_t start, tb_int_t size)
{
	tb_char_t const* 	s = tb_string_c_string(string);
	tb_size_t 			n = tb_string_size(string);
	if (!s || !n || start < 0 || start >= n || !size) return TB_FALSE;

	tb_string_assign_c_string_with_size(sub, s + start, n - start);
	return tb_string_resize(sub, size);
}

tb_int_t tb_string_find_c_string(tb_string_t const* string, tb_char_t const* sub, tb_int_t start)
{
	// get string
	tb_char_t const* 	s = tb_string_c_string(string);
	tb_size_t 			n = tb_string_size(string);
	if (!s || !sub || start < 0 || start >= n) return -1;

	tb_int_t idx = -1;
	tb_char_t const* ps = s + start;
	tb_char_t const* p1 = ps;
	tb_char_t const* p2 = sub;

	do
	{
		if (!*p2) 
		{
			idx = ps - s;
			break;
		}
		if (*p2 == *p1)
		{
			++p2;
			++p1;
		} 
		else
		{
			p2 = sub;
			if (!*p1) return -1;
			p1 = ++ps;
		}

	} while (1);

	return ((idx < 0 || idx >= n)? -1 : idx);
}
tb_int_t tb_string_find_string(tb_string_t const* string, tb_string_t const* sub, tb_int_t start)
{
	return tb_string_find_c_string(string, tb_string_c_string(sub), start);
}
