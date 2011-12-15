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
 * \file		mstring.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "mstring.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_MSTRING_FMTD_SIZE 		(4096)
#else
# 	define TB_MSTRING_FMTD_SIZE 		(8192)
#endif

/* ////////////////////////////////////////////////////////////////////////
 * atomic
 */
static __tb_inline__ tb_handle_t tb_mstring_atomic_mutx_get(tb_mstring_t const* string)
{
	tb_check_return_val(string->mutx, TB_NULL);
	return tb_atomic_get(string->mutx);
}
static __tb_inline__ tb_handle_t tb_mstring_atomic_mutx_del(tb_mstring_t* string)
{
	tb_check_return_val(string->mutx, TB_NULL);
	return tb_atomic_fetch_and_set0(string->mutx);
}

/* ////////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_mstring_init(tb_mstring_t* string)
{
	tb_assert_and_check_return_val(string, TB_FALSE);
	tb_memset(string, 0, sizeof(tb_mstring_t));
	return tb_rstring_init(&string->rstr);
}
tb_void_t tb_mstring_exit(tb_mstring_t* string)
{
	if (string)
	{
		// refn--
		tb_mstring_decr(string);

		// clear
		tb_memset(string, 0, sizeof(tb_mstring_t));
	}
}

/* ////////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_char_t const* tb_mstring_cstr(tb_mstring_t const* string)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// cstr
	tb_char_t const* s = tb_rstring_cstr(&string->rstr);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return s;
}
tb_size_t tb_mstring_size(tb_mstring_t const* string)
{
	tb_assert_and_check_return_val(string, 0);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// size
	tb_size_t n = tb_rstring_size(&string->rstr);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return n;
}
tb_size_t tb_mstring_refn(tb_mstring_t const* string)
{	
	tb_assert_and_check_return_val(string, 0);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// refn
	tb_size_t r = tb_rstring_refn(&string->rstr);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_mstring_clear(tb_mstring_t* string)
{
	tb_assert_and_check_return(string);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return(mutx);

	// enter
	tb_check_return(tb_mutex_enter(mutx));

	// clear
	tb_rstring_clear(&string->rstr);

	// leave
	tb_check_return(tb_mutex_leave(mutx));
}
tb_char_t const* tb_mstring_strip(tb_mstring_t* string, tb_size_t n)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// cstr
	tb_char_t const* s = tb_rstring_strip(&string->rstr, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return s;
}
tb_size_t tb_mstring_incr(tb_mstring_t* string)
{	
	tb_assert_and_check_return_val(string, 0);

	// init
	tb_size_t r = 0;

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	if (!mutx)
	{
		// init mutx
		mutx = tb_mutex_init("rsting");
		tb_assert_and_check_goto(mutx, fail);

		// alloc the shared mutx 
		string->mutx = tb_calloc(1, sizeof(tb_handle_t));
		tb_assert_and_check_goto(string->mutx, fail);
	
		// init the shared mutx
		*(string->mutx) = mutx;
	
		// refn++
		r = tb_rstring_incr(&string->rstr);
	}
	else
	{
		// enter
		tb_check_return_val(tb_mutex_enter(mutx), 0);
		
		// refn++
		r = tb_rstring_incr(&string->rstr);

		// leave
		tb_check_return_val(tb_mutex_leave(mutx), 0);
	}

	return r;

fail:

	// free mutx
	if (string->mutx) tb_free(string->mutx);
	string->mutx = TB_NULL;

	// exit mutx
	if (mutx) tb_mutex_exit(mutx);

	return 0;
}
tb_size_t tb_mstring_decr(tb_mstring_t* string)
{	
	tb_assert_and_check_return_val(string, 0);

	// init
	tb_size_t r = 0;

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// refn--
	r = tb_rstring_decr(&string->rstr);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	// free mutx
	if (!r)
	{
		// atomic remove mutx
		mutx = tb_mstring_atomic_mutx_del(string);
		if (mutx) //!< only one get it
		{
			// exit mutex
			tb_mutex_exit(mutx);

			// free it
			tb_free(string->mutx);
		}
	}

	return r;
}
/* ////////////////////////////////////////////////////////////////////////
 * enter & leave
 */
tb_bool_t tb_mstring_enter(tb_mstring_t const* string)
{
	tb_assert_and_check_return_val(string, TB_FALSE);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, TB_FALSE);

	// enter
	return tb_mutex_enter(mutx);
}
tb_bool_t tb_mstring_leave(tb_mstring_t const* string)
{
	tb_assert_and_check_return_val(string, TB_FALSE);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, TB_FALSE);

	// leave
	return tb_mutex_leave(mutx);
}

/* ////////////////////////////////////////////////////////////////////////
 * strchr
 */
tb_long_t tb_mstring_strchr(tb_mstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strchr
	tb_long_t r = tb_rstring_strchr(&string->rstr, p, c);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_mstring_strichr(tb_mstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strichr
	tb_long_t r = tb_rstring_strichr(&string->rstr, p, c);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strrchr
 */
tb_long_t tb_mstring_strrchr(tb_mstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strrchr
	tb_long_t r = tb_rstring_strrchr(&string->rstr, p, c);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_mstring_strirchr(tb_mstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strirchr
	tb_long_t r = tb_rstring_strirchr(&string->rstr, p, c);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strstr
 */
tb_long_t tb_mstring_strstr(tb_mstring_t const* string, tb_size_t p, tb_mstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strstr
	tb_long_t r = tb_rstring_strstr(&string->rstr, p, s);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_mstring_stristr(tb_mstring_t const* string, tb_size_t p, tb_mstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// stristr
	tb_long_t r = tb_rstring_stristr(&string->rstr, p, s);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_mstring_cstrstr(tb_mstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// cstrstr
	tb_long_t r = tb_rstring_cstrstr(&string->rstr, p, s2);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_mstring_cstristr(tb_mstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// cstristr
	tb_long_t r = tb_rstring_cstristr(&string->rstr, p, s2);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strrstr
 */
tb_long_t tb_mstring_strrstr(tb_mstring_t const* string, tb_size_t p, tb_mstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strrstr
	tb_long_t r = tb_rstring_strrstr(&string->rstr, p, s);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_mstring_strirstr(tb_mstring_t const* string, tb_size_t p, tb_mstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strirstr
	tb_long_t r = tb_rstring_strirstr(&string->rstr, p, s);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

tb_long_t tb_mstring_cstrrstr(tb_mstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// cstrrstr
	tb_long_t r = tb_rstring_cstrrstr(&string->rstr, p, s2);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_mstring_cstrirstr(tb_mstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(string, -1);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// cstrirstr
	tb_long_t r = tb_rstring_cstrirstr(&string->rstr, p, s2);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strcpy
 */
tb_char_t const* tb_mstring_strcpy(tb_mstring_t* string, tb_mstring_t const* s)
{
	tb_assert_and_check_return_val(string && string != s, TB_NULL);

	// refn--
	tb_mstring_decr(string);

	// copy
	tb_memcpy(string, s, sizeof(tb_mstring_t));

	// refn++
	tb_mstring_incr(string);

	// ok
	return tb_mstring_cstr(string);
}
tb_char_t const* tb_mstring_cstrcpy(tb_mstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_mstring_cstrncpy(string, s, tb_strlen(s));
}
tb_char_t const* tb_mstring_cstrncpy(tb_mstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s && n, TB_NULL);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	if (!mutx)
	{
		// refn++
		tb_mstring_incr(string);

		// mutx
		mutx = tb_mstring_atomic_mutx_get(string);
		tb_assert_and_check_return_val(mutx, TB_NULL);
	}

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// cstrcat
	tb_char_t const* r = tb_rstring_cstrncpy(&string->rstr, s, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return r;
}
tb_char_t const* tb_mstring_cstrfcpy(tb_mstring_t* string, tb_char_t const* fmt, ...)
{
	tb_assert_and_check_return_val(string && fmt, TB_NULL);

	// format data
	tb_char_t p[TB_MSTRING_FMTD_SIZE] = {0};
	tb_size_t n = 0;
	tb_va_format(p, TB_MSTRING_FMTD_SIZE, fmt, &n);
	tb_assert_and_check_return_val(n, TB_NULL);
	
	return tb_mstring_cstrncpy(string, p, n);
}
/* ////////////////////////////////////////////////////////////////////////
 * chrcat
 */
tb_char_t const* tb_mstring_chrcat(tb_mstring_t* string, tb_char_t c)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	if (!mutx)
	{
		// refn++
		tb_mstring_incr(string);

		// mutx
		mutx = tb_mstring_atomic_mutx_get(string);
		tb_assert_and_check_return_val(mutx, TB_NULL);
	}

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// chrcat
	tb_char_t const* r = tb_rstring_chrcat(&string->rstr, c);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return r;
}
tb_char_t const* tb_mstring_chrncat(tb_mstring_t* string, tb_char_t c, tb_size_t n)
{
	tb_assert_and_check_return_val(string && n, TB_NULL);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	if (!mutx)
	{
		// refn++
		tb_mstring_incr(string);

		// mutx
		mutx = tb_mstring_atomic_mutx_get(string);
		tb_assert_and_check_return_val(mutx, TB_NULL);
	}

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// chrncat
	tb_char_t const* r = tb_rstring_chrncat(&string->rstr, c, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return r;
}
/* ////////////////////////////////////////////////////////////////////////
 * strcat
 */
tb_char_t const* tb_mstring_strcat(tb_mstring_t* string, tb_mstring_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);

	// copy it?
	if (!tb_mstring_size(string)) return tb_mstring_strcpy(string, s);

	// append it
	return tb_mstring_cstrncat(string, tb_mstring_cstr(s), tb_mstring_size(s));
}
tb_char_t const* tb_mstring_cstrcat(tb_mstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_mstring_cstrncat(string, s, tb_strlen(s));
}
tb_char_t const* tb_mstring_cstrncat(tb_mstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s && n, TB_NULL);

	// copy it?
	if (!tb_mstring_size(string)) return tb_mstring_cstrncpy(string, s, n);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// cstrcat
	tb_char_t const* r = tb_rstring_cstrncat(&string->rstr, s, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return r;
}
tb_char_t const* tb_mstring_cstrfcat(tb_mstring_t* string, tb_char_t const* fmt, ...)
{
	tb_assert_and_check_return_val(string && fmt, TB_NULL);

	// format data
	tb_char_t p[TB_MSTRING_FMTD_SIZE] = {0};
	tb_size_t n = 0;
	tb_va_format(p, TB_MSTRING_FMTD_SIZE, fmt, &n);
	tb_assert_and_check_return_val(n, TB_NULL);
	
	return tb_mstring_cstrncat(string, p, n);
}

/* ////////////////////////////////////////////////////////////////////////
 * strcmp
 */
tb_long_t tb_mstring_strcmp(tb_mstring_t* string, tb_mstring_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_mstring_cstrncmp(string, s, tb_mstring_size(s));
}
tb_long_t tb_mstring_strimp(tb_mstring_t* string, tb_mstring_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_mstring_cstrnicmp(string, s, tb_mstring_size(s));
}
tb_long_t tb_mstring_cstrcmp(tb_mstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_mstring_cstrncmp(string, s, tb_strlen(s));
}
tb_long_t tb_mstring_cstricmp(tb_mstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_mstring_cstrnicmp(string, s, tb_strlen(s));
}
tb_long_t tb_mstring_cstrncmp(tb_mstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string, 0);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);
	
	// cstrncmp
	tb_long_t r = tb_rstring_cstrncmp(&string->rstr, s, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return r;
}
tb_long_t tb_mstring_cstrnicmp(tb_mstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string, 0);

	// mutx
	tb_handle_t mutx = tb_mstring_atomic_mutx_get(string);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);
	
	// cstrnicmp
	tb_long_t r = tb_rstring_cstrnicmp(&string->rstr, s, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return r;
}

