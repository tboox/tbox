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
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_RSTRING_FMTD_SIZE 		(4096)
#else
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
		// refn--
		tb_rstring_decr(string);

		// exit mutex
		if (string->mutx) tb_mutex_exit(string->mutx);

		// clear
		tb_memset(string, 0, sizeof(tb_rstring_t));
	}
}

/* ////////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_char_t const* tb_rstring_cstr(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, TB_NULL);
	tb_check_return_val(string->data && string->mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), TB_NULL);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// cstr
	tb_char_t const* s = pstr? tb_pstring_cstr(pstr) : TB_NULL;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), TB_NULL);

	return s;
}
tb_size_t tb_rstring_size(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, 0);
	tb_check_return_val(string->data && string->mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), 0);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// cstr
	tb_size_t n = pstr? tb_pstring_size(pstr) : 0;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), 0);

	return n;
}
tb_size_t tb_rstring_refn(tb_rstring_t const* string)
{	
	tb_assert_and_check_return_val(string, 0);
	tb_check_return_val(string->data && string->mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), 0);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// refn
	tb_size_t refn = pstr? *((tb_size_t*)&pstr[1]) : 0;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), 0);

	tb_assert(refn);
	return refn;
}

/* ////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_rstring_clear(tb_rstring_t* string)
{
	tb_assert_and_check_return(string);
	tb_check_return(string->data && string->mutx);

	// enter
	tb_check_return(tb_mutex_enter(string->mutx));

	// pstr
	tb_pstring_t* pstr = *string->data;

	// clear
	if (pstr) tb_pstring_clear(pstr);

	// leave
	tb_check_return(tb_mutex_leave(string->mutx));
}
tb_char_t const* tb_rstring_strip(tb_rstring_t* string, tb_size_t n)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), TB_NULL);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// strip
	tb_char_t const* s = pstr? tb_pstring_strip(pstr, n) : TB_NULL;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), TB_NULL);

	return s;
}
tb_size_t tb_rstring_incr(tb_rstring_t* string)
{	
	tb_assert_and_check_return_val(string, 0);
	tb_check_return_val(string->data && string->mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), 0);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// refn++
	tb_size_t refn = pstr? ++*((tb_size_t*)&pstr[1]) : 0;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), 0);

	return refn;
}
tb_size_t tb_rstring_decr(tb_rstring_t* string)
{	
	tb_assert_and_check_return_val(string, 0);
	tb_check_return_val(string->data && string->mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), 0);

	// pstr
	tb_size_t refn = 0;
	tb_pstring_t* pstr = *string->data;
	if (pstr)
	{
		// refn--
		refn = *((tb_size_t*)&pstr[1]);
		if (refn > 1) *((tb_size_t*)&pstr[1]) = --refn;
		else
		{
			// free pstring
			tb_pstring_exit(pstr);

			// free shared data
			tb_free(pstr);

			// clear shared pointer
			*string->data = TB_NULL;

			// clear rstring
			string->data = TB_NULL;

			// clear refn
			refn = 0;
		}
	}

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), 0);

	return refn;
}
/* ////////////////////////////////////////////////////////////////////////
 * enter & leave
 */
tb_bool_t tb_rstring_enter(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string && string->mutx, TB_FALSE);
	return tb_mutex_enter(string->mutx);
}
tb_bool_t tb_rstring_leave(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string && string->mutx, TB_FALSE);
	return tb_mutex_leave(string->mutx);
}

/* ////////////////////////////////////////////////////////////////////////
 * strchr
 */
tb_long_t tb_rstring_strchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// strchr
	tb_long_t r = pstr? tb_pstring_strchr(pstr, p, c) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}
tb_long_t tb_rstring_strichr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// strichr
	tb_long_t r = pstr? tb_pstring_strichr(pstr, p, c) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strrchr
 */
tb_long_t tb_rstring_strrchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// strrchr
	tb_long_t r = pstr? tb_pstring_strrchr(pstr, p, c) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}
tb_long_t tb_rstring_strirchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// strirchr
	tb_long_t r = pstr? tb_pstring_strirchr(pstr, p, c) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strstr
 */
tb_long_t tb_rstring_strstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// strstr
	tb_long_t r = pstr? tb_pstring_strstr(pstr, p, s) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}
tb_long_t tb_rstring_stristr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// stristr
	tb_long_t r = pstr? tb_pstring_stristr(pstr, p, s) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}
tb_long_t tb_rstring_cstrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// cstrstr
	tb_long_t r = pstr? tb_pstring_cstrstr(pstr, p, s2) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}
tb_long_t tb_rstring_cstristr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// cstristr
	tb_long_t r = pstr? tb_pstring_cstristr(pstr, p, s2) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strrstr
 */
tb_long_t tb_rstring_strrstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// strrstr
	tb_long_t r = pstr? tb_pstring_strrstr(pstr, p, s) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}
tb_long_t tb_rstring_strirstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// strirstr
	tb_long_t r = pstr? tb_pstring_strirstr(pstr, p, s) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}

tb_long_t tb_rstring_cstrrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// cstrrstr
	tb_long_t r = pstr? tb_pstring_cstrrstr(pstr, p, s2) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}
tb_long_t tb_rstring_cstrirstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(string, -1);
	tb_check_return_val(string->data && string->mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), -1);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// cstrirstr
	tb_long_t r = pstr? tb_pstring_cstrirstr(pstr, p, s2) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strcpy
 */
tb_char_t const* tb_rstring_strcpy(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string && string != s, TB_NULL);

	// refn--
	tb_rstring_decr(string);

	// copy
	tb_memcpy(string, s, sizeof(tb_rstring_t));

	// refn++
	tb_rstring_incr(string);

	// ok
	return tb_rstring_cstr(string);
}
tb_char_t const* tb_rstring_cstrcpy(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncpy(string, s, tb_strlen(s));
}
tb_char_t const* tb_rstring_cstrncpy(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s && n, TB_NULL);

	tb_trace_noimpl();
	return TB_NULL;
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
	tb_trace_noimpl();
	return 0;
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
	if (!tb_rstring_size(string)) return tb_rstring_cstrncpy(string, s, n);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), 0);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// cstrcat
	tb_char_t const* r = pstr? tb_pstring_cstrncat(pstr, s, n) : TB_NULL;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), 0);

	return r;
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
	tb_assert_and_check_return_val(string, 0);
	tb_check_return_val(string->data && string->mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), 0);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// cstrncmp
	tb_long_t r = pstr? tb_pstring_cstrncmp(pstr, s, n) : 0;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), 0);

	return r;
}
tb_long_t tb_rstring_cstrnicmp(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string, 0);
	tb_check_return_val(string->data && string->mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(string->mutx), 0);

	// pstr
	tb_pstring_t* pstr = *string->data;

	// cstrnicmp
	tb_long_t r = pstr? tb_pstring_cstrnicmp(pstr, s, n) : 0;

	// leave
	tb_check_return_val(tb_mutex_leave(string->mutx), 0);

	return r;
}

