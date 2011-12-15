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

	// init
	tb_char_t const* s = TB_NULL;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstr
		s = tb_pstring_cstr(&data->pstr);
	}

	return s;
}
tb_size_t tb_rstring_size(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, 0);

	// init
	tb_size_t n = 0;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// size
		n = tb_pstring_size(&data->pstr);
	}

	return n;
}
tb_size_t tb_rstring_refn(tb_rstring_t const* string)
{	
	tb_assert_and_check_return_val(string, 0);

	// init
	tb_size_t r = 0;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// refn
		r = data->refn;
	}

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_rstring_clear(tb_rstring_t* string)
{
	tb_assert_and_check_return(string);

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// clear
		tb_pstring_clear(&data->pstr);
	}
}
tb_char_t const* tb_rstring_strip(tb_rstring_t* string, tb_size_t n)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// init
	tb_char_t const* s = TB_NULL;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstr
		s = tb_pstring_strip(&data->pstr, n);
	}

	return s;
}
tb_size_t tb_rstring_incr(tb_rstring_t* string)
{	
	tb_assert_and_check_return_val(string, 0);

	// init
	tb_size_t r = 0;

	// data
	tb_rstring_data_t* data = TB_NULL;

	// init
	if (!string->data)
	{
		// alloc the shared data pointer
		string->data = tb_calloc(1, sizeof(tb_rstring_data_t*));
		tb_assert_and_check_goto(string->data, fail);

		// alloc the shared data
		data = tb_calloc(1, sizeof(tb_rstring_data_t));
		tb_assert_and_check_goto(data, fail);
	
		// init the shared pointer
		*(string->data) = data;

		// init refn
		r = data->refn = 1;

		// init pstring
		if (!tb_pstring_init(&data->pstr)) goto fail;
	}
	else
	{
		// data
		data = *(string->data);
		if (data)
		{
			// check 
			tb_assert(data->refn);

			// refn++
			r = ++data->refn;
		}
	}

	return r;

fail:
	// free data
	if (data) tb_free(data);

	// free data pointer
	if (string->data) tb_free(string->data);

	// clear
	tb_memset(string, 0, sizeof(tb_rstring_t));

	return 0;
}
tb_size_t tb_rstring_decr(tb_rstring_t* string)
{	
	tb_assert_and_check_return_val(string, 0);

	// init
	tb_size_t r = 0;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// refn--
		r = --data->refn;

		// free it?
		if (!r)
		{
			// exit pstring
			tb_pstring_exit(&data->pstr);

			// free data
			tb_free(data);

			// reset pointer
			*string->data = TB_NULL;
		}
	}

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strchr
 */
tb_long_t tb_rstring_strchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// strchr
		r = tb_pstring_strchr(&data->pstr, p, c);
	}

	return r;
}
tb_long_t tb_rstring_strichr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// strichr
		r = tb_pstring_strichr(&data->pstr, p, c);
	}

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strrchr
 */
tb_long_t tb_rstring_strrchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// strrchr
		r = tb_pstring_strrchr(&data->pstr, p, c);
	}

	return r;
}
tb_long_t tb_rstring_strirchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// strirchr
		r = tb_pstring_strirchr(&data->pstr, p, c);
	}

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strstr
 */
tb_long_t tb_rstring_strstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// strstr
		r = tb_pstring_strstr(&data->pstr, p, s);
	}

	return r;
}
tb_long_t tb_rstring_stristr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// stristr
		r = tb_pstring_stristr(&data->pstr, p, s);
	}

	return r;
}
tb_long_t tb_rstring_cstrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstrstr
		r = tb_pstring_cstrstr(&data->pstr, p, s2);
	}

	return r;
}
tb_long_t tb_rstring_cstristr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstristr
		r = tb_pstring_cstristr(&data->pstr, p, s2);
	}

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strrstr
 */
tb_long_t tb_rstring_strrstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// strrstr
		r = tb_pstring_strrstr(&data->pstr, p, s);
	}

	return r;
}
tb_long_t tb_rstring_strirstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// strirstr
		r = tb_pstring_strirstr(&data->pstr, p, s);
	}

	return r;
}

tb_long_t tb_rstring_cstrrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstrrstr
		r = tb_pstring_cstrrstr(&data->pstr, p, s2);
	}

	return r;
}
tb_long_t tb_rstring_cstrirstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(string, -1);

	// init
	tb_long_t r = -1;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstrirstr
		r = tb_pstring_cstrirstr(&data->pstr, p, s2);
	}

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

	// init
	tb_char_t const* r = TB_NULL;

	// no data? refn++
	if (!string->data) tb_rstring_incr(string);
	tb_assert_and_check_return_val(string->data, TB_NULL);

	// data
	tb_rstring_data_t* data = *(string->data);
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstrncpy
		r = tb_pstring_cstrncpy(&data->pstr, s, n);
	}

	return r;
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

	// init
	tb_char_t const* r = TB_NULL;

	// no data? refn++
	if (!string->data) tb_rstring_incr(string);
	tb_assert_and_check_return_val(string->data, TB_NULL);

	// data
	tb_rstring_data_t* data = *(string->data);
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// chrcat
		r = tb_pstring_chrcat(&data->pstr, c);
	}

	return r;
}
tb_char_t const* tb_rstring_chrncat(tb_rstring_t* string, tb_char_t c, tb_size_t n)
{
	tb_assert_and_check_return_val(string && n, TB_NULL);

	// init
	tb_char_t const* r = TB_NULL;

	// no data? refn++
	if (!string->data) tb_rstring_incr(string);
	tb_assert_and_check_return_val(string->data, TB_NULL);

	// data
	tb_rstring_data_t* data = *(string->data);
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// chrncat
		r = tb_pstring_chrncat(&data->pstr, c, n);
	}

	return r;
}
/* ////////////////////////////////////////////////////////////////////////
 * strcat
 */
tb_char_t const* tb_rstring_strcat(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);

	// copy it?
	if (!tb_rstring_size(string)) return tb_rstring_strcpy(string, s);

	// append it
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

	// copy it?
	if (!tb_rstring_size(string)) return tb_rstring_cstrncpy(string, s, n);

	// init
	tb_char_t const* r = TB_NULL;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstrncat
		r = tb_pstring_cstrncat(&data->pstr, s, n);
	}

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

	// init
	tb_long_t r = 0;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstrncmp
		r = tb_pstring_cstrncmp(&data->pstr, s, n);
	}

	return r;
}
tb_long_t tb_rstring_cstrnicmp(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string, 0);

	// init
	tb_long_t r = 0;

	// data
	tb_rstring_data_t* data = string->data? *(string->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstrnicmp
		r = tb_pstring_cstrnicmp(&data->pstr, s, n);
	}

	return r;
}

