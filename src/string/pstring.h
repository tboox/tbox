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
 * \file		pstring.h
 *
 */
#ifndef TB_STRING_PSTRING_H
#define TB_STRING_PSTRING_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

// the scoped string type
typedef struct __tb_pstring_t
{
	// the string data
	tb_char_t* 		data;

	// the string size
	tb_size_t 		size;

	// the string maxn
	tb_size_t 		maxn;

}tb_pstring_t;

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_void_t			tb_pstring_init(tb_pstring_t* string);
tb_void_t 			tb_pstring_exit(tb_pstring_t* string);

// accessors
tb_char_t const* 	tb_pstring_cstr(tb_pstring_t const* string);
tb_size_t 			tb_pstring_size(tb_pstring_t const* string);

// modifiors
tb_bool_t 			tb_pstring_resize(tb_pstring_t* string, tb_size_t size);
tb_void_t 			tb_pstring_clear(tb_pstring_t* string);

// strchr
tb_long_t 			tb_pstring_strchr(tb_pstring_t const* string, tb_char_t ch, tb_long_t start);
tb_long_t 			tb_pstring_strichr(tb_pstring_t const* string, tb_char_t ch, tb_long_t start);

tb_long_t 			tb_pstring_rstrchr(tb_pstring_t const* string, tb_char_t ch, tb_long_t start);
tb_long_t 			tb_pstring_rstrichr(tb_pstring_t const* string, tb_char_t ch, tb_long_t start);

// strstr
tb_long_t 			tb_pstring_strstr(tb_pstring_t const* string, tb_pstring_t const* sub, tb_long_t start);
tb_long_t 			tb_pstring_cstrstr(tb_pstring_t const* string, tb_char_t const* sub, tb_long_t start);

tb_long_t 			tb_pstring_stristr(tb_pstring_t const* string, tb_pstring_t const* sub, tb_long_t start);
tb_long_t 			tb_pstring_cstristr(tb_pstring_t const* string, tb_char_t const* sub, tb_long_t start);

tb_long_t 			tb_pstring_rstrstr(tb_pstring_t const* string, tb_pstring_t const* sub, tb_long_t start);
tb_long_t 			tb_pstring_rcstrstr(tb_pstring_t const* string, tb_char_t const* sub, tb_long_t start);

tb_long_t 			tb_pstring_rstristr(tb_pstring_t const* string, tb_pstring_t const* sub, tb_long_t start);
tb_long_t 			tb_pstring_rcstristr(tb_pstring_t const* string, tb_char_t const* sub, tb_long_t start);

// strcpy
tb_char_t const* 	tb_pstring_strcpy(tb_pstring_t* string, tb_pstring_t const* s_string);
tb_char_t const* 	tb_pstring_cstrcpy(tb_pstring_t* string, tb_char_t const* c_string);
tb_char_t const* 	tb_pstring_cstrncpy(tb_pstring_t* string, tb_char_t const* c_string, tb_size_t size);
tb_char_t const* 	tb_pstring_cstrfcpy(tb_pstring_t* string, tb_char_t const* fmt, ...);

// strcat
tb_char_t const* 	tb_pstring_strcat(tb_pstring_t* string, tb_pstring_t const* s_string);
tb_char_t const* 	tb_pstring_cstrcat(tb_pstring_t* string, tb_char_t const* c_string);
tb_char_t const* 	tb_pstring_cstrncat(tb_pstring_t* string, tb_char_t const* c_string, tb_size_t size);
tb_char_t const* 	tb_pstring_cstrfcat(tb_pstring_t* string, tb_char_t const* fmt, ...);

// strcmp
tb_long_t 			tb_pstring_strcmp(tb_pstring_t* string, tb_pstring_t const* s_string);
tb_long_t 			tb_pstring_cstrcmp(tb_pstring_t* string, tb_char_t const* c_string);
tb_long_t 			tb_pstring_strcimp(tb_pstring_t* string, tb_pstring_t const* s_string);
tb_long_t 			tb_pstring_cstricmp(tb_pstring_t* string, tb_char_t const* c_string);

#endif

