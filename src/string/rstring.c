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
 * @file		rstring.c
 * @ingroup 	string
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rstring.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value string 
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_RSTRING_FMTD_SIZE 		(4096)
#else
# 	define TB_RSTRING_FMTD_SIZE 		(8192)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_rstring_init(tb_rstring_t* string)
{
	tb_assert_and_check_return_val(string, TB_FALSE);
	return tb_rbuffer_init(string);
}
tb_void_t tb_rstring_exit(tb_rstring_t* string)
{
	if (string) tb_rbuffer_exit(string);
}

/* ///////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_char_t const* tb_rstring_cstr(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, TB_NULL);
	return (tb_char_t const*)tb_rbuffer_data(string);
}
tb_size_t tb_rstring_size(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, 0);
	tb_size_t n = tb_rbuffer_size(string);
	return n > 0? n - 1 : 0;
}
tb_size_t tb_rstring_refn(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, 0);
	return tb_rbuffer_refn(string);
}

/* ///////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_rstring_clear(tb_rstring_t* string)
{
	tb_assert_and_check_return(string);

	// clear buffer
	tb_rbuffer_clear(string);

	// clear string
	tb_char_t* p = tb_rbuffer_data(string);
	if (p) p[0] = '\0';
}
tb_char_t const* tb_rstring_strip(tb_rstring_t* string, tb_size_t n)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// out?
	tb_check_return_val(n < tb_rstring_size(string), tb_rstring_cstr(string));

	// strip
	tb_char_t* p = tb_rbuffer_resize(string, n + 1);
	if (p) p[n] = '\0';
	return p;
}
tb_size_t tb_rstring_incr(tb_rstring_t* string)
{
	tb_assert_and_check_return_val(string, 0);
	return tb_rbuffer_incr(string);
}
tb_size_t tb_rstring_decr(tb_rstring_t* string)
{
	tb_assert_and_check_return_val(string, 0);
	return tb_rbuffer_decr(string);
}
/* ///////////////////////////////////////////////////////////////////////
 * strchr
 */
tb_long_t tb_rstring_strchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strchr(s + p, c);
	return (q? q - s : -1);
}
tb_long_t tb_rstring_strichr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strichr(s + p, c);
	return (q? q - s : -1);
}

/* ///////////////////////////////////////////////////////////////////////
 * strrchr
 */
tb_long_t tb_rstring_strrchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnrchr(s + p, n, c);
	return (q? q - s : -1);
}
tb_long_t tb_rstring_strirchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnirchr(s + p, n, c);
	return (q? q - s : -1);
}

/* ///////////////////////////////////////////////////////////////////////
 * strstr
 */
tb_long_t tb_rstring_strstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	return tb_rstring_cstrstr(string, tb_rstring_cstr(s), p);
}
tb_long_t tb_rstring_stristr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	return tb_rstring_cstristr(string, tb_rstring_cstr(s), p);
}
tb_long_t tb_rstring_cstrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strstr(s + p, s2);
	return (q? q - s : -1);
}
tb_long_t tb_rstring_cstristr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_stristr(s + p, s2);
	return (q? q - s : -1);
}

/* ///////////////////////////////////////////////////////////////////////
 * strrstr
 */
tb_long_t tb_rstring_strrstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	return tb_rstring_cstrrstr(string, tb_rstring_cstr(s), p);
}
tb_long_t tb_rstring_strirstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	return tb_rstring_cstrirstr(string, tb_rstring_cstr(s), p);
}

tb_long_t tb_rstring_cstrrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnrstr(s + p, n, s2);
	return (q? q - s : -1);
}
tb_long_t tb_rstring_cstrirstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_char_t const* 	s = tb_rstring_cstr(string);
	tb_size_t 			n = tb_rstring_size(string);
	tb_assert_and_check_return_val(s && p >= 0 && p < n, -1);

	tb_char_t* q = tb_strnirstr(s + p, n, s2);
	return (q? q - s : -1);
}

/* ///////////////////////////////////////////////////////////////////////
 * strcpy
 */
tb_char_t const* tb_rstring_strcpy(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncpy(string, tb_rstring_cstr(s), tb_rstring_size(s));
}
tb_char_t const* tb_rstring_cstrcpy(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncpy(string, s, tb_strlen(s));
}
tb_char_t const* tb_rstring_cstrncpy(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s && n, TB_NULL);

	return tb_rbuffer_memncpy(string, s, n + 1);
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
/* ///////////////////////////////////////////////////////////////////////
 * chrcat
 */
tb_char_t const* tb_rstring_chrcat(tb_rstring_t* string, tb_char_t c)
{
	tb_assert_and_check_return_val(string, TB_NULL);
	
	tb_char_t* p = tb_rbuffer_memnsetp(string, tb_rstring_size(string), c, 2);
	if (p) p[tb_rstring_size(string)] = '\0';
	return p;
}
tb_char_t const* tb_rstring_chrncat(tb_rstring_t* string, tb_char_t c, tb_size_t n)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	tb_char_t* p = tb_rbuffer_memnsetp(string, tb_rstring_size(string), c, n + 1);
	if (p) p[tb_rstring_size(string)] = '\0';
	return p;
}
/* ///////////////////////////////////////////////////////////////////////
 * strcat
 */
tb_char_t const* tb_rstring_strcat(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncat(string, tb_rstring_cstr(s), tb_rstring_size(s));
}
tb_char_t const* tb_rstring_cstrcat(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncat(string, s, tb_strlen(s));
}
tb_char_t const* tb_rstring_cstrncat(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s && n, TB_NULL);
	return tb_rbuffer_memncpyp(string, tb_rstring_size(string), s, n + 1);
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

/* ///////////////////////////////////////////////////////////////////////
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
	tb_assert_and_check_return_val(string && s, 0);
	return tb_strncmp(tb_rstring_cstr(string), s, n);
}
tb_long_t tb_rstring_cstrnicmp(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_strnicmp(tb_rstring_cstr(string), s, n);
}

