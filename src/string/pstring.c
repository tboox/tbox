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
 * @file		pstring.c
 * @ingroup 	string
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pstring.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value string 
#ifdef __tb_small__
# 	define TB_PSTRING_FMTD_SIZE 		(4096)
#else
# 	define TB_PSTRING_FMTD_SIZE 		(8192)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_pstring_init(tb_pstring_t* string)
{
	tb_assert_and_check_return_val(string, tb_false);
	return tb_pbuffer_init(string);
}
tb_void_t tb_pstring_exit(tb_pstring_t* string)
{
	if (string) tb_pbuffer_exit(string);
}

/* ///////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_char_t const* tb_pstring_cstr(tb_pstring_t const* string)
{
	tb_assert_and_check_return_val(string, tb_null);
	return (tb_char_t const*)tb_pbuffer_data(string);
}
tb_size_t tb_pstring_size(tb_pstring_t const* string)
{
	tb_assert_and_check_return_val(string, 0);
	tb_size_t n = tb_pbuffer_size(string);
	return n > 0? n - 1 : 0;
}


/* ///////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_pstring_clear(tb_pstring_t* string)
{
	tb_assert_and_check_return(string);

	// clear buffer
	tb_pbuffer_clear(string);

	// clear string
	tb_char_t* p = tb_pbuffer_data(string);
	if (p) p[0] = '\0';
}
tb_char_t const* tb_pstring_strip(tb_pstring_t* string, tb_size_t n)
{
	tb_assert_and_check_return_val(string, tb_null);

	// out?
	tb_check_return_val(n < tb_pstring_size(string), tb_pstring_cstr(string));

	// strip
	tb_char_t* p = tb_pbuffer_resize(string, n + 1);
	if (p) p[n] = '\0';
	return p;
}
/* ///////////////////////////////////////////////////////////////////////
 * strchr
 */
tb_long_t tb_pstring_strchr(tb_pstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_pstring_cstr(string);
	tb_size_t 			n = tb_pstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strchr(s + p, c);
	return (q? q - s : -1);
}
tb_long_t tb_pstring_strichr(tb_pstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_pstring_cstr(string);
	tb_size_t 			n = tb_pstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strichr(s + p, c);
	return (q? q - s : -1);
}

/* ///////////////////////////////////////////////////////////////////////
 * strrchr
 */
tb_long_t tb_pstring_strrchr(tb_pstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_pstring_cstr(string);
	tb_size_t 			n = tb_pstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnrchr(s + p, n, c);
	return (q? q - s : -1);
}
tb_long_t tb_pstring_strirchr(tb_pstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_pstring_cstr(string);
	tb_size_t 			n = tb_pstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnirchr(s + p, n, c);
	return (q? q - s : -1);
}

/* ///////////////////////////////////////////////////////////////////////
 * strstr
 */
tb_long_t tb_pstring_strstr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s)
{
	return tb_pstring_cstrstr(string, tb_pstring_cstr(s), p);
}
tb_long_t tb_pstring_stristr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s)
{
	return tb_pstring_cstristr(string, tb_pstring_cstr(s), p);
}
tb_long_t tb_pstring_cstrstr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_char_t const* 	s = tb_pstring_cstr(string);
	tb_size_t 			n = tb_pstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strstr(s + p, s2);
	return (q? q - s : -1);
}
tb_long_t tb_pstring_cstristr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_char_t const* 	s = tb_pstring_cstr(string);
	tb_size_t 			n = tb_pstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_stristr(s + p, s2);
	return (q? q - s : -1);
}

/* ///////////////////////////////////////////////////////////////////////
 * strrstr
 */
tb_long_t tb_pstring_strrstr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s)
{
	return tb_pstring_cstrrstr(string, tb_pstring_cstr(s), p);
}
tb_long_t tb_pstring_strirstr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s)
{
	return tb_pstring_cstrirstr(string, tb_pstring_cstr(s), p);
}

tb_long_t tb_pstring_cstrrstr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_char_t const* 	s = tb_pstring_cstr(string);
	tb_size_t 			n = tb_pstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnrstr(s + p, n, s2);
	return (q? q - s : -1);
}
tb_long_t tb_pstring_cstrirstr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_char_t const* 	s = tb_pstring_cstr(string);
	tb_size_t 			n = tb_pstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnirstr(s + p, n, s2);
	return (q? q - s : -1);
}

/* ///////////////////////////////////////////////////////////////////////
 * strcpy
 */
tb_char_t const* tb_pstring_strcpy(tb_pstring_t* string, tb_pstring_t const* s)
{
	// check
	tb_assert_and_check_return_val(s, tb_null);

	tb_size_t n = tb_pstring_size(s);
	if (n) return tb_pstring_cstrncpy(string, tb_pstring_cstr(s), n);
	else
	{
		tb_pstring_clear(string);
		return tb_null;
	}
}
tb_char_t const* tb_pstring_cstrcpy(tb_pstring_t* string, tb_char_t const* s)
{
	// check
	tb_assert_and_check_return_val(s, tb_null);
	return tb_pstring_cstrncpy(string, s, tb_strlen(s));
}
tb_char_t const* tb_pstring_cstrncpy(tb_pstring_t* string, tb_char_t const* s, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(string && s && n, tb_null);

	tb_char_t* p = tb_pbuffer_memncpy(string, s, n + 1);
	if (p) p[tb_pstring_size(string)] = '\0';
	return p;
}
tb_char_t const* tb_pstring_cstrfcpy(tb_pstring_t* string, tb_char_t const* fmt, ...)
{
	// check
	tb_assert_and_check_return_val(string && fmt, tb_null);

	// format data
	tb_char_t p[TB_PSTRING_FMTD_SIZE] = {0};
	tb_size_t n = 0;
	tb_vsnprintf_format(p, TB_PSTRING_FMTD_SIZE, fmt, &n);
	tb_assert_and_check_return_val(n, tb_null);
	
	return tb_pstring_cstrncpy(string, p, n);
}
/* ///////////////////////////////////////////////////////////////////////
 * chrcat
 */
tb_char_t const* tb_pstring_chrcat(tb_pstring_t* string, tb_char_t c)
{
	// check
	tb_assert_and_check_return_val(string, tb_null);
	
	tb_char_t* p = tb_pbuffer_memnsetp(string, tb_pstring_size(string), c, 2);
	if (p) p[tb_pstring_size(string)] = '\0';
	return p;
}
tb_char_t const* tb_pstring_chrncat(tb_pstring_t* string, tb_char_t c, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(string, tb_null);

	tb_char_t* p = tb_pbuffer_memnsetp(string, tb_pstring_size(string), c, n + 1);
	if (p) p[tb_pstring_size(string)] = '\0';
	return p;
}
/* ///////////////////////////////////////////////////////////////////////
 * strcat
 */
tb_char_t const* tb_pstring_strcat(tb_pstring_t* string, tb_pstring_t const* s)
{
	// check
	tb_assert_and_check_return_val(s, tb_null);
	return tb_pstring_cstrncat(string, tb_pstring_cstr(s), tb_pstring_size(s));
}
tb_char_t const* tb_pstring_cstrcat(tb_pstring_t* string, tb_char_t const* s)
{
	// check
	tb_assert_and_check_return_val(s, tb_null);
	return tb_pstring_cstrncat(string, s, tb_strlen(s));
}
tb_char_t const* tb_pstring_cstrncat(tb_pstring_t* string, tb_char_t const* s, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(string && s && n, tb_null);
	tb_char_t* p = tb_pbuffer_memncpyp(string, tb_pstring_size(string), s, n + 1);
	if (p) p[tb_pstring_size(string)] = '\0';
	return p;
}
tb_char_t const* tb_pstring_cstrfcat(tb_pstring_t* string, tb_char_t const* fmt, ...)
{
	// check
	tb_assert_and_check_return_val(string && fmt, tb_null);

	// format data
	tb_char_t p[TB_PSTRING_FMTD_SIZE] = {0};
	tb_long_t n = 0;
	tb_vsnprintf_format(p, TB_PSTRING_FMTD_SIZE, fmt, &n);
	tb_assert_and_check_return_val(n, tb_null);
	
	return tb_pstring_cstrncat(string, p, n);
}

/* ///////////////////////////////////////////////////////////////////////
 * strcmp
 */
tb_long_t tb_pstring_strcmp(tb_pstring_t* string, tb_pstring_t const* s)
{
	// check
	tb_assert_and_check_return_val(string && s, 0);
	return tb_pstring_cstrncmp(string, tb_pstring_cstr(s), tb_pstring_size(s) + 1);
}
tb_long_t tb_pstring_strimp(tb_pstring_t* string, tb_pstring_t const* s)
{
	// check
	tb_assert_and_check_return_val(string && s, 0);
	return tb_pstring_cstrnicmp(string, tb_pstring_cstr(s), tb_pstring_size(s) + 1);
}
tb_long_t tb_pstring_cstrcmp(tb_pstring_t* string, tb_char_t const* s)
{
	// check
	tb_assert_and_check_return_val(string && s, 0);
	return tb_pstring_cstrncmp(string, s, tb_strlen(s) + 1);
}
tb_long_t tb_pstring_cstricmp(tb_pstring_t* string, tb_char_t const* s)
{
	// check
	tb_assert_and_check_return_val(string && s, 0);
	return tb_pstring_cstrnicmp(string, s, tb_strlen(s) + 1);
}
tb_long_t tb_pstring_cstrncmp(tb_pstring_t* string, tb_char_t const* s, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(string && s, 0);
	return tb_strncmp(tb_pstring_cstr(string), s, n);
}
tb_long_t tb_pstring_cstrnicmp(tb_pstring_t* string, tb_char_t const* s, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(string && s, 0);
	return tb_strnicmp(tb_pstring_cstr(string), s, n);
}

