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
 * @file		sstring.h
 * @ingroup 	string
 *
 */
#ifndef TB_STRING_SSTRING_H
#define TB_STRING_SSTRING_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the static string type
typedef tb_sbuffer_t tb_sstring_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_sstring_init(tb_sstring_t* string, tb_char_t* data, tb_size_t maxn);
tb_void_t 			tb_sstring_exit(tb_sstring_t* string);

// accessors
tb_char_t const* 	tb_sstring_cstr(tb_sstring_t const* string);
tb_size_t 			tb_sstring_size(tb_sstring_t const* string);

// modifiors
tb_void_t 			tb_sstring_clear(tb_sstring_t* string);
tb_char_t const* 	tb_sstring_strip(tb_sstring_t* string, tb_size_t n);

// strchr
tb_long_t 			tb_sstring_strchr(tb_sstring_t const* string, tb_size_t p, tb_char_t c);
tb_long_t 			tb_sstring_strichr(tb_sstring_t const* string, tb_size_t p, tb_char_t c);

// strrchr
tb_long_t 			tb_sstring_strrchr(tb_sstring_t const* string, tb_size_t p, tb_char_t c);
tb_long_t 			tb_sstring_strirchr(tb_sstring_t const* string, tb_size_t p, tb_char_t c);

// strstr
tb_long_t 			tb_sstring_strstr(tb_sstring_t const* string, tb_size_t p, tb_sstring_t const* s);
tb_long_t 			tb_sstring_stristr(tb_sstring_t const* string, tb_size_t p, tb_sstring_t const* s);

tb_long_t 			tb_sstring_cstrstr(tb_sstring_t const* string, tb_size_t p, tb_char_t const* s);
tb_long_t 			tb_sstring_cstristr(tb_sstring_t const* string, tb_size_t p, tb_char_t const* s);

// strrstr
tb_long_t 			tb_sstring_strrstr(tb_sstring_t const* string, tb_size_t p, tb_sstring_t const* s);
tb_long_t 			tb_sstring_strirstr(tb_sstring_t const* string, tb_size_t p, tb_sstring_t const* s);

tb_long_t 			tb_sstring_cstrrstr(tb_sstring_t const* string, tb_size_t p, tb_char_t const* s);
tb_long_t 			tb_sstring_cstrirstr(tb_sstring_t const* string, tb_size_t p, tb_char_t const* s);

// strcpy
tb_char_t const* 	tb_sstring_strcpy(tb_sstring_t* string, tb_sstring_t const* s);
tb_char_t const* 	tb_sstring_cstrcpy(tb_sstring_t* string, tb_char_t const* s);
tb_char_t const* 	tb_sstring_cstrncpy(tb_sstring_t* string, tb_char_t const* s, tb_size_t n);
tb_char_t const* 	tb_sstring_cstrfcpy(tb_sstring_t* string, tb_char_t const* fmt, ...);

// chrcat
tb_char_t const* 	tb_sstring_chrcat(tb_sstring_t* string, tb_char_t c);
tb_char_t const* 	tb_sstring_chrncat(tb_sstring_t* string, tb_char_t c, tb_size_t n);

// strcat
tb_char_t const* 	tb_sstring_strcat(tb_sstring_t* string, tb_sstring_t const* s);
tb_char_t const* 	tb_sstring_cstrcat(tb_sstring_t* string, tb_char_t const* s);
tb_char_t const* 	tb_sstring_cstrncat(tb_sstring_t* string, tb_char_t const* s, tb_size_t n);
tb_char_t const* 	tb_sstring_cstrfcat(tb_sstring_t* string, tb_char_t const* fmt, ...);

// strcmp
tb_long_t 			tb_sstring_strcmp(tb_sstring_t* string, tb_sstring_t const* s);
tb_long_t 			tb_sstring_strimp(tb_sstring_t* string, tb_sstring_t const* s);

tb_long_t 			tb_sstring_cstrcmp(tb_sstring_t* string, tb_char_t const* s);
tb_long_t 			tb_sstring_cstricmp(tb_sstring_t* string, tb_char_t const* s);

tb_long_t 			tb_sstring_cstrncmp(tb_sstring_t* string, tb_char_t const* s, tb_size_t n);
tb_long_t 			tb_sstring_cstrnicmp(tb_sstring_t* string, tb_char_t const* s, tb_size_t n);

#endif

