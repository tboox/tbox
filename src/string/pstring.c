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
 * \file		pstring.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pstring.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value string 
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_STRING_GROW_SIZE 		(64)
#else
# 	define TB_STRING_GROW_SIZE 		(256)
#endif

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_void_t tb_pstring_init(tb_pstring_t* string)
{
	if (string) tb_memset(string, 0, sizeof(tb_pstring_t));
}
tb_void_t tb_pstring_exit(tb_pstring_t* string)
{
	if (string)
	{
		if (string->data && string->owner)
			tb_free(string->data);

		tb_pstring_init(string);
	}
}
tb_char_t const* tb_pstring_cstr(tb_pstring_t const* string)
{
	if (string) return (tb_char_t const*)string->data;
	else return TB_NULL;
}
tb_size_t tb_pstring_size(tb_pstring_t const* string)
{
	tb_assert_and_check_return_val(string && string->data && string->maxn, 0);
	return string->size;
}
tb_bool_t tb_pstring_resize(tb_pstring_t* string, tb_size_t size)
{
	if (!string) return TB_FALSE;
	
	// check size
	tb_assert(size < TB_STRING_MAX_SIZE);
	if (size >= TB_STRING_MAX_SIZE) return TB_FALSE;

	// is null?
	if (!string->data) 
	{
		// compute size
		string->size = size;
		string->maxn = tb_align(size + TB_STRING_GROW_SIZE, TB_STRING_GROW_SIZE);

		// check maxn
		tb_assert(string->maxn < TB_STRING_MAX_SIZE);
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
		tb_size_t osize = string->size;

		// compute size
		string->size = size;
		string->maxn = tb_align(size + TB_STRING_GROW_SIZE, TB_STRING_GROW_SIZE);

		// check maxn
		tb_assert(string->maxn < TB_STRING_MAX_SIZE);
		if (string->maxn >= TB_STRING_MAX_SIZE) goto fail;

		// alloc
		string->data = tb_malloc(string->maxn);
		string->owner = 1;
		if (!string->data) goto fail;
		tb_memcpy(string->data, odata, tb_min(osize, size));
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
		tb_size_t osize = string->size;

		// compute size
		string->size = size;
		string->maxn = tb_align(size + TB_STRING_GROW_SIZE, TB_STRING_GROW_SIZE);

		// check maxn
		tb_assert(string->maxn < TB_STRING_MAX_SIZE);
		if (string->maxn >= TB_STRING_MAX_SIZE) goto fail;

		// alloc
		string->data = tb_malloc(string->maxn);
		string->owner = 1;
		if (!string->data) goto fail;
		tb_memcpy(string->data, odata, tb_min(osize, size));
		string->data[string->size] = '\0';
	}
	// increase
	else
	{
		// compute size
		string->maxn = tb_align(size + TB_STRING_GROW_SIZE, TB_STRING_GROW_SIZE);

		// check maxn
		tb_assert(string->maxn < TB_STRING_MAX_SIZE);
		if (string->maxn >= TB_STRING_MAX_SIZE) goto fail;

		// realloc
		string->size = size;
		string->data = tb_realloc(string->data, string->maxn);
		if (!string->data) goto fail;
		else string->data[string->size] = '\0';
	}

	return TB_TRUE;
fail:
	tb_trace("resize string fail!");
	tb_pstring_exit(string);
	return TB_FALSE;
}
tb_void_t tb_pstring_clear(tb_pstring_t* string)
{
	if (string) 
	{
		if (!tb_pstring_is_null(string))
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
tb_char_t const* tb_pstring_strcpy(tb_pstring_t* string, tb_pstring_t const* s_string)
{
	if (!string) return TB_NULL;
	if (!tb_pstring_is_null(s_string))
	{
		// ensure enough size
		if (!tb_pstring_resize(string, s_string->size)) return TB_NULL;

		// attach string
		tb_memcpy(string->data, s_string->data, string->size);
		string->data[string->size] = '\0';

		return tb_pstring_c_string(string);
	}
	else 
	{
		tb_pstring_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_pstring_cstrcpy(tb_pstring_t* string, tb_char_t const* c_string)
{
	if (!string) return TB_NULL;
	if (c_string) 
	{
		// ensure enough size
		if (!tb_pstring_resize(string, tb_strlen(c_string))) return TB_NULL;

		// attach string
		tb_memcpy(string->data, c_string, string->size);
		string->data[string->size] = '\0';

		return tb_pstring_c_string(string);
	}
	else 
	{
		tb_pstring_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_pstring_strcpy_char(tb_pstring_t* string, tb_char_t ch)
{
	if (!string) return TB_NULL;

	// ensure enough size
	if (!tb_pstring_resize(string, 1)) return TB_NULL;

	// attach string
	string->data[0] = ch;
	string->data[1] = '\0';

	return tb_pstring_c_string(string);
}
tb_char_t const* tb_pstring_cstrfcpy(tb_pstring_t* string, tb_char_t const* fmt, ...)
{
	if (!string || !fmt) return TB_NULL;

	// format text
	tb_char_t text[4096];
	tb_size_t size = 0;
	TB_VA_FMT(text, 4096, fmt, &size);
	if (size) return tb_pstring_cstrncpy(string, text, size);
	else return TB_NULL;
}
tb_char_t const* tb_pstring_cstrncpy(tb_pstring_t* string, tb_char_t const* c_string, tb_size_t size)
{
	if (!string) return TB_NULL;
	if (c_string && size) 
	{
		// ensure enough size
		if (!tb_pstring_resize(string, size)) return TB_NULL;

		// attach string
		tb_memcpy(string->data, c_string, string->size);
		string->data[string->size] = '\0';

		return tb_pstring_c_string(string);
	}
	else 
	{
		tb_pstring_clear(string);
		return TB_NULL;
	}
}

tb_char_t const* tb_pstring_strcpy_by_ref(tb_pstring_t* string, tb_pstring_t const* s_string)
{
	if (!string) return TB_NULL;

	// ensure is null
	if (!tb_pstring_is_null(string))
		tb_pstring_exit(string);

	// attach string
	string->data = (tb_byte_t*)s_string->data;
	string->size = s_string->size;

	// readonly
	string->maxn = 0;
	string->owner = 0;

	return tb_pstring_c_string(string);
}
tb_char_t const* tb_pstring_cstrcpy_by_ref(tb_pstring_t* string, tb_char_t const* c_string)
{
	if (!string) return TB_NULL;
	if (c_string)
	{
		// ensure is null
		if (!tb_pstring_is_null(string))
			tb_pstring_exit(string);

		// attach string
		string->data = (tb_byte_t*)c_string;
		string->size = tb_strlen(c_string);

		// readonly
		string->maxn = 0;
		string->owner = 0;

		return tb_pstring_c_string(string);
	}
	else 
	{
		tb_pstring_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_pstring_cstrncpy_by_ref(tb_pstring_t* string, tb_char_t const* c_string, tb_size_t size)
{
	if (!string) return TB_NULL;
	if (c_string && size)
	{
		// ensure is null
		if (!tb_pstring_is_null(string))
			tb_pstring_exit(string);

		// attach string
		string->data = (tb_byte_t*)c_string;
		string->size = size;

		// readonly
		string->maxn = 0;
		string->owner = 0;

		return tb_pstring_c_string(string);
	}
	else 
	{
		tb_pstring_clear(string);
		return TB_NULL;
	}
}
tb_char_t const* tb_pstring_strcat(tb_pstring_t* string, tb_pstring_t const* s_string)
{
	if (tb_pstring_is_null(string)) return tb_pstring_strcpy(string, s_string);
	else if (!tb_pstring_is_null(s_string))
	{
		// get old size
		tb_size_t size = string->size;

		// get s_string size
		tb_size_t s_size = s_string->size;

		// ensure enough size
		if (!tb_pstring_resize(string, size + s_size)) return TB_NULL;

		// strcat string
		tb_memcpy(string->data + size, s_string->data, s_size);
		string->data[string->size] = '\0';
	}
	return tb_pstring_c_string(string);
}
tb_char_t const* tb_pstring_cstrcat(tb_pstring_t* string, tb_char_t const* c_string)
{
	if (tb_pstring_is_null(string)) return tb_pstring_cstrcpy(string, c_string);
	else if (c_string)
	{
		// get old size
		tb_size_t size = string->size;

		// get c_string size
		tb_size_t c_size = tb_strlen(c_string);

		// ensure enough size
		if (!tb_pstring_resize(string, size + c_size)) return TB_NULL;

		// strcat string
		tb_memcpy(string->data + size, c_string, c_size);
		string->data[string->size] = '\0';
	}
	return tb_pstring_c_string(string);
}
tb_char_t const* tb_pstring_cstrncat(tb_pstring_t* string, tb_char_t const* c_string, tb_size_t size)
{
	if (tb_pstring_is_null(string)) return tb_pstring_cstrncpy(string, c_string, size);
	else if (c_string)
	{
		// get old osize
		tb_size_t osize = string->size;

		// ensure enough size
		if (!tb_pstring_resize(string, osize + size)) return TB_NULL;

		// strcat string
		tb_memcpy(string->data + osize, c_string, size);
		string->data[string->size] = '\0';
	}
	return tb_pstring_c_string(string);
}
tb_char_t const* tb_pstring_strcat_format(tb_pstring_t* string, tb_char_t const* fmt, ...)
{
	if (!string || !fmt) return TB_NULL;

	// format text
	tb_char_t text[4096];
	tb_size_t size = 0;
	TB_VA_FMT(text, 4096, fmt, &size);
	if (size) return tb_pstring_cstrncat(string, text, size);
	else return tb_pstring_c_string(string);
}
tb_long_t tb_pstring_strchr(tb_pstring_t const* string, tb_char_t ch, tb_long_t start)
{
	// get string
	tb_char_t const* 	s = tb_pstring_c_string(string);
	tb_size_t 			n = tb_pstring_size(string);
	if (!s || start < 0 || start >= n) return -1;
	
	tb_char_t const* p = (tb_char_t const*)tb_strchr(s + start, ch);
	return (p? p - s : -1);
}
tb_long_t tb_pstring_cstrstr(tb_pstring_t const* string, tb_char_t const* sub, tb_long_t start)
{
	tb_char_t const* s = tb_pstring_c_string(string);
	tb_assert_and_check_return_val(s, -1);

	tb_char_t const* p = (tb_char_t const*)tb_cstrstr(s + start, sub);
	return (p? p - s : -1);
}
tb_long_t tb_pstring_strichr(tb_pstring_t const* string, tb_char_t ch, tb_long_t start)
{
	// get string
	tb_char_t const* 	s = tb_pstring_c_string(string);
	tb_size_t 			n = tb_pstring_size(string);
	if (!s || start < 0 || start >= n) return -1;
	
	// find it
	tb_char_t const* p = s + start;
	tb_char_t const* e = s + n;
	while (p < e && *p)
	{
		if (*p == ch || tb_tolower(*p) == tb_tolower(ch)) break;
		p++;
	}
	return (p < e)? (p - s) : -1;
}
tb_long_t tb_pstring_stristr(tb_pstring_t const* string, tb_char_t const* sub, tb_long_t start)
{
	tb_char_t const* s = tb_pstring_c_string(string);
	tb_assert_and_check_return_val(s, -1);

	tb_char_t const* p = (tb_char_t const*)tb_stristr(s + start, sub);
	return (p? p - s : -1);
}
tb_long_t tb_pstring_strstr(tb_pstring_t const* string, tb_pstring_t const* sub, tb_long_t start)
{
	return tb_pstring_cstrstr(string, tb_pstring_c_string(sub), start);
}
tb_long_t tb_pstring_stristr(tb_pstring_t const* string, tb_pstring_t const* sub, tb_long_t start)
{
	return tb_pstring_stristr(string, tb_pstring_c_string(sub), start);
}
tb_long_t tb_pstring_rstrchr(tb_pstring_t const* string, tb_char_t ch, tb_long_t start)
{
	// get string
	tb_char_t const* 	s = tb_pstring_c_string(string);
	tb_size_t 			n = tb_pstring_size(string);
	if (!s || start < 0 || start >= n) return -1;
	
	// find it
	tb_char_t const* b = s + start;
	tb_char_t const* p = s + n - 1;
	while (p >= b && *p)
	{
		if (*p == ch) break;
		p--;
	}
	return (p >= b)? (p - s) : -1;
}
tb_long_t tb_pstring_rstrichr(tb_pstring_t const* string, tb_char_t ch, tb_long_t start)
{
	// get string
	tb_char_t const* 	s = tb_pstring_c_string(string);
	tb_size_t 			n = tb_pstring_size(string);
	if (!s || start < 0 || start >= n) return -1;
	
	// find it
	tb_char_t const* b = s + start;
	tb_char_t const* p = s + n - 1;
	while (p >= b && *p)
	{
		if (*p == ch || tb_tolower(*p) == tb_tolower(ch)) break;
		p--;
	}
	return (p >= b)? (p - s) : -1;
}
tb_long_t tb_pstring_strcmp(tb_pstring_t* string, tb_pstring_t const* s_string)
{
	if (!tb_pstring_is_null(s_string))
		return tb_pstring_strcmp(string, tb_pstring_c_string(s_string));
	else return TB_FALSE;
}
tb_long_t tb_pstring_strcmp(tb_pstring_t* string, tb_char_t const* c_string)
{
	if (tb_pstring_is_null(string)) return TB_FALSE;
	else if (c_string) return !tb_strcmp(tb_pstring_c_string(string), c_string)? TB_TRUE : TB_FALSE;
	else return TB_FALSE;
}
tb_long_t tb_pstring_strcimp(tb_pstring_t* string, tb_pstring_t const* s_string)
{
	if (!tb_pstring_is_null(s_string))
		return tb_pstring_cstricmp(string, tb_pstring_c_string(s_string));
	else return TB_FALSE;
}
tb_long_t tb_pstring_cstricmp(tb_pstring_t* string, tb_char_t const* c_string)
{
	if (tb_pstring_is_null(string)) return TB_FALSE;
	else if (c_string) return !tb_cstricmp(tb_pstring_c_string(string), c_string)? TB_TRUE : TB_FALSE;
	else return TB_FALSE;
}

