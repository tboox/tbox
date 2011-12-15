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
 * \file		mstring.h
 *
 */
#ifndef TB_STRING_MSTRING_H
#define TB_STRING_MSTRING_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "rstring.h"

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

/* the multi-thread reference string type
 *
 * mstring => rstring
 *        mutx
 *      (atomic)
 */
typedef struct __tb_mstring_t
{
	// the shared string
	tb_rstring_t 			rstr;

	// the shared mutex
	tb_handle_t* 			mutx;

}tb_mstring_t;

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_mstring_init(tb_mstring_t* string);
tb_void_t 			tb_mstring_exit(tb_mstring_t* string);

// accessors
tb_char_t const* 	tb_mstring_cstr(tb_mstring_t const* string); //!< no safe, need enter the mutex scope
tb_size_t 			tb_mstring_size(tb_mstring_t const* string);
tb_size_t 			tb_mstring_refn(tb_mstring_t const* string);

// modifiors
tb_void_t 			tb_mstring_clear(tb_mstring_t* string);
tb_size_t 			tb_mstring_incr(tb_mstring_t* string);
tb_size_t 			tb_mstring_decr(tb_mstring_t* string);

// enter & leave
tb_bool_t 			tb_mstring_enter(tb_mstring_t const* string);
tb_bool_t 			tb_mstring_leave(tb_mstring_t const* string);

// strchr
tb_long_t 			tb_mstring_strchr(tb_mstring_t const* string, tb_size_t p, tb_char_t c);
tb_long_t 			tb_mstring_strichr(tb_mstring_t const* string, tb_size_t p, tb_char_t c);

// strrchr
tb_long_t 			tb_mstring_strrchr(tb_mstring_t const* string, tb_size_t p, tb_char_t c);
tb_long_t 			tb_mstring_strirchr(tb_mstring_t const* string, tb_size_t p, tb_char_t c);

// strstr
tb_long_t 			tb_mstring_strstr(tb_mstring_t const* string, tb_size_t p, tb_mstring_t const* s);
tb_long_t 			tb_mstring_stristr(tb_mstring_t const* string, tb_size_t p, tb_mstring_t const* s);

tb_long_t 			tb_mstring_cstrstr(tb_mstring_t const* string, tb_size_t p, tb_char_t const* s);
tb_long_t 			tb_mstring_cstristr(tb_mstring_t const* string, tb_size_t p, tb_char_t const* s);

// strrstr
tb_long_t 			tb_mstring_strrstr(tb_mstring_t const* string, tb_size_t p, tb_mstring_t const* s);
tb_long_t 			tb_mstring_strirstr(tb_mstring_t const* string, tb_size_t p, tb_mstring_t const* s);

tb_long_t 			tb_mstring_cstrrstr(tb_mstring_t const* string, tb_size_t p, tb_char_t const* s);
tb_long_t 			tb_mstring_cstrirstr(tb_mstring_t const* string, tb_size_t p, tb_char_t const* s);

// strcpy
tb_char_t const* 	tb_mstring_strcpy(tb_mstring_t* string, tb_mstring_t const* s);
tb_char_t const* 	tb_mstring_cstrcpy(tb_mstring_t* string, tb_char_t const* s);
tb_char_t const* 	tb_mstring_cstrncpy(tb_mstring_t* string, tb_char_t const* s, tb_size_t n);
tb_char_t const* 	tb_mstring_cstrfcpy(tb_mstring_t* string, tb_char_t const* fmt, ...);

// chrcat
tb_char_t const* 	tb_mstring_chrcat(tb_mstring_t* string, tb_char_t c);
tb_char_t const* 	tb_mstring_chrncat(tb_mstring_t* string, tb_char_t c, tb_size_t n);

// strcat
tb_char_t const* 	tb_mstring_strcat(tb_mstring_t* string, tb_mstring_t const* s);
tb_char_t const* 	tb_mstring_cstrcat(tb_mstring_t* string, tb_char_t const* s);
tb_char_t const* 	tb_mstring_cstrncat(tb_mstring_t* string, tb_char_t const* s, tb_size_t n);
tb_char_t const* 	tb_mstring_cstrfcat(tb_mstring_t* string, tb_char_t const* fmt, ...);

// strcmp
tb_long_t 			tb_mstring_strcmp(tb_mstring_t* string, tb_mstring_t const* s);
tb_long_t 			tb_mstring_strimp(tb_mstring_t* string, tb_mstring_t const* s);

tb_long_t 			tb_mstring_cstrcmp(tb_mstring_t* string, tb_char_t const* s);
tb_long_t 			tb_mstring_cstricmp(tb_mstring_t* string, tb_char_t const* s);

tb_long_t 			tb_mstring_cstrncmp(tb_mstring_t* string, tb_char_t const* s, tb_size_t n);
tb_long_t 			tb_mstring_cstrnicmp(tb_mstring_t* string, tb_char_t const* s, tb_size_t n);

#endif

