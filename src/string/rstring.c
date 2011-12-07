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
 * \file		rstring.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rstring.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value string 
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_RSTRING_GROW_SIZE 		(64)
# 	define TB_RSTRING_FMTD_SIZE 		(4096)
#else
# 	define TB_RSTRING_GROW_SIZE 		(256)
# 	define TB_RSTRING_FMTD_SIZE 		(8192)
#endif

/* ////////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_rstring_init(tb_rstring_t* string)
{
	tb_assert_and_check_return_val(string, TB_FALSE);
	tb_memset(string, 0, sizeof(tb_rstring_t));
	return TB_TRUE;
}
tb_void_t tb_rstring_exit(tb_rstring_t* string)
{
	if (string)
	{
		if (string->data) tb_free(string->data);
		tb_memset(string, 0, sizeof(tb_rstring_t));
	}
}

/* ////////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_char_t const* tb_rstring_cstr(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, TB_NULL);
	return (tb_char_t const*)string->data;
}
tb_size_t tb_rstring_size(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, 0);
	return string->size;
}


/* ////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_rstring_clear(tb_rstring_t* string)
{
	tb_assert_and_check_return(string);
	string->size = 0;
	if (string->data) string->data[0] = '\0';
}
tb_bool_t tb_rstring_resize(tb_rstring_t* string, tb_size_t size)
{
	tb_assert_and_check_return_val(string, TB_FALSE);

	// save it
	tb_rstring_t s = *string;
	
	// null?
	if (!string->data) 
	{
		// check size
		tb_assert(!string->size && size);

		// compute size
		string->size = size;
		string->maxn = tb_align8(size + TB_RSTRING_GROW_SIZE);
		tb_assert_and_check_goto(size < string->maxn, fail);

		// alloc data
		string->data = tb_malloc(string->maxn);
		tb_assert_and_check_goto(string->data, fail);
		string->data[size] = '\0';
	}
	// decrease
	else if (size < string->maxn)
	{
		string->size = size;
		string->data[size] = '\0';
	}
	// increase
	else
	{
		// compute size
		string->maxn = tb_align8(size + TB_RSTRING_GROW_SIZE);
		tb_assert_and_check_goto(size < string->maxn, fail);

		// realloc
		string->size = size;
		string->data = tb_realloc(string->data, string->maxn);
		tb_assert_and_check_goto(string->data, fail);
		string->data[size] = '\0';
	}

	// ok
	return TB_TRUE;

fail:

	// restore it
	*string = s;

	// failed
	tb_trace("failed to resize string: %s", s.data? s.data : "");
	return TB_FALSE;
}

/* ////////////////////////////////////////////////////////////////////////
 * strchr
 */
tb_long_t tb_rstring_strchr(tb_rstring_t const* string, tb_long_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strchr(s + p, c);
	return (q? q - s : -1);
}
tb_long_t tb_rstring_strichr(tb_rstring_t const* string, tb_long_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strichr(s + p, c);
	return (q? q - s : -1);
}

/* ////////////////////////////////////////////////////////////////////////
 * strrchr
 */
tb_long_t tb_rstring_strrchr(tb_rstring_t const* string, tb_long_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnrchr(s + p, n, c);
	return (q? q - s : -1);
}
tb_long_t tb_rstring_strirchr(tb_rstring_t const* string, tb_long_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnirchr(s + p, n, c);
	return (q? q - s : -1);
}

/* ////////////////////////////////////////////////////////////////////////
 * strstr
 */
tb_long_t tb_rstring_strstr(tb_rstring_t const* string, tb_long_t p, tb_rstring_t const* s)
{
	return tb_rstring_cstrstr(string, tb_rstring_cstr(s), p);
}
tb_long_t tb_rstring_stristr(tb_rstring_t const* string, tb_long_t p, tb_rstring_t const* s)
{
	return tb_rstring_cstristr(string, tb_rstring_cstr(s), p);
}
tb_long_t tb_rstring_cstrstr(tb_rstring_t const* string, tb_long_t p, tb_char_t const* s2)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strstr(s + p, s2);
	return (q? q - s : -1);
}
tb_long_t tb_rstring_cstristr(tb_rstring_t const* string, tb_long_t p, tb_char_t const* s2)
{	
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_stristr(s + p, s2);
	return (q? q - s : -1);
}

/* ////////////////////////////////////////////////////////////////////////
 * strrstr
 */
tb_long_t tb_rstring_strrstr(tb_rstring_t const* string, tb_long_t p, tb_rstring_t const* s)
{
	return tb_rstring_cstrrstr(string, tb_rstring_cstr(s), p);
}
tb_long_t tb_rstring_strirstr(tb_rstring_t const* string, tb_long_t p, tb_rstring_t const* s)
{
	return tb_rstring_cstrirstr(string, tb_rstring_cstr(s), p);
}

tb_long_t tb_rstring_cstrrstr(tb_rstring_t const* string, tb_long_t p, tb_char_t const* s2)
{	
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnrstr(s + p, n, s2);
	return (q? q - s : -1);
}
tb_long_t tb_rstring_cstrirstr(tb_rstring_t const* string, tb_long_t p, tb_char_t const* s2)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnirstr(s + p, n, s2);
	return (q? q - s : -1);
}

/* ////////////////////////////////////////////////////////////////////////
 * strcpy
 */
tb_char_t const* tb_rstring_strcpy(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncpy(string, s, tb_rstring_size(s));
}
tb_char_t const* tb_rstring_cstrcpy(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncpy(string, s, tb_strlen(s));
}
tb_char_t const* tb_rstring_cstrncpy(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s && n, TB_NULL);

	// resize
	if (!tb_rstring_resize(string, n)) return TB_NULL;

	// check
	tb_assert(string->data && string->size == n);

	// copy data
	tb_memcpy(string->data, s, n);
	string->data[n] = '\0';

	// ok
	return string->data;
}
tb_char_t const* tb_rstring_cstrfcpy(tb_rstring_t* string, tb_char_t const* fmt, ...)
{
	tb_assert_and_check_return_val(string && fmt, TB_NULL);

	// format data
	tb_char_t p[TB_RSTRING_FMTD_SIZE] = {0};
	tb_size_t n = 0;
	tb_va_format(p, TB_RSTRING_FMTD_SIZE, fmt, &n);
	tb_assert_and_check_return_val(n, TB_NULL);
	
	return tb_rstring_cstrncpy(string, p, n);
}
/* ////////////////////////////////////////////////////////////////////////
 * chrcat
 */
tb_char_t const* tb_rstring_chrcat(tb_rstring_t* string, tb_char_t c)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// get old size
	tb_size_t n = string->size;

	// resize
	if (!tb_rstring_resize(string, n + 1)) return TB_NULL;

	// check
	tb_assert(string->data && string->size == n + 1);

	// append char
	string->data[n] = c;
	string->data[n + 1] = '\0';

	// ok
	return string->data;
}
/* ////////////////////////////////////////////////////////////////////////
 * strcat
 */
tb_char_t const* tb_rstring_strcat(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncat(string, s, tb_rstring_size(s));
}
tb_char_t const* tb_rstring_cstrcat(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncat(string, s, tb_strlen(s));
}
tb_char_t const* tb_rstring_cstrncat(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s && n, TB_NULL);

	// copy it if null
	if (!string->size) return tb_rstring_cstrncpy(string, s, n);

	// get old size
	tb_size_t on = string->size;

	// resize
	if (!tb_rstring_resize(string, on + n)) return TB_NULL;

	// check
	tb_assert(string->data && string->size == on + n);

	// copy data
	tb_memcpy(string->data + on, s, n);
	string->data[on + n] = '\0';

	// ok
	return string->data;
}
tb_char_t const* tb_rstring_cstrfcat(tb_rstring_t* string, tb_char_t const* fmt, ...)
{
	tb_assert_and_check_return_val(string && fmt, TB_NULL);

	// format data
	tb_char_t p[TB_RSTRING_FMTD_SIZE] = {0};
	tb_size_t n = 0;
	tb_va_format(p, TB_RSTRING_FMTD_SIZE, fmt, &n);
	tb_assert_and_check_return_val(n, TB_NULL);
	
	return tb_rstring_cstrncat(string, p, n);
}

/* ////////////////////////////////////////////////////////////////////////
 * strcmp
 */
tb_long_t tb_rstring_strcmp(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_rstring_cstrncmp(string, s, tb_rstring_size(s));
}
tb_long_t tb_rstring_strimp(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_rstring_cstrnicmp(string, s, tb_rstring_size(s));
}
tb_long_t tb_rstring_cstrcmp(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_rstring_cstrncmp(string, s, tb_strlen(s));
}
tb_long_t tb_rstring_cstricmp(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_rstring_cstrnicmp(string, s, tb_strlen(s));
}
tb_long_t tb_rstring_cstrncmp(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && string->data && s, 0);
	return tb_strncmp(string->data, s, n);
}
tb_long_t tb_rstring_cstrnicmp(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && string->data && s, 0);
	return tb_strnicmp(string->data, s, n);
}

