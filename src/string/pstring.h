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
 * @file		pstring.h
 * @ingroup 	string
 *
 */
#ifndef TB_STRING_PSTRING_H
#define TB_STRING_PSTRING_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the scoped string type
typedef tb_pbuffer_t tb_pstring_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_pstring_init(tb_pstring_t* string);
tb_void_t 			tb_pstring_exit(tb_pstring_t* string);

// accessors
tb_char_t const* 	tb_pstring_cstr(tb_pstring_t const* string);
tb_size_t 			tb_pstring_size(tb_pstring_t const* string);

// modifiors
tb_void_t 			tb_pstring_clear(tb_pstring_t* string);
tb_char_t const* 	tb_pstring_strip(tb_pstring_t* string, tb_size_t n);

// strchr
tb_long_t 			tb_pstring_strchr(tb_pstring_t const* string, tb_size_t p, tb_char_t c);
tb_long_t 			tb_pstring_strichr(tb_pstring_t const* string, tb_size_t p, tb_char_t c);

// strrchr
tb_long_t 			tb_pstring_strrchr(tb_pstring_t const* string, tb_size_t p, tb_char_t c);
tb_long_t 			tb_pstring_strirchr(tb_pstring_t const* string, tb_size_t p, tb_char_t c);

// strstr
tb_long_t 			tb_pstring_strstr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s);
tb_long_t 			tb_pstring_stristr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s);

tb_long_t 			tb_pstring_cstrstr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s);
tb_long_t 			tb_pstring_cstristr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s);

// strrstr
tb_long_t 			tb_pstring_strrstr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s);
tb_long_t 			tb_pstring_strirstr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s);

tb_long_t 			tb_pstring_cstrrstr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s);
tb_long_t 			tb_pstring_cstrirstr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s);

// strcpy
tb_char_t const* 	tb_pstring_strcpy(tb_pstring_t* string, tb_pstring_t const* s);
tb_char_t const* 	tb_pstring_cstrcpy(tb_pstring_t* string, tb_char_t const* s);
tb_char_t const* 	tb_pstring_cstrncpy(tb_pstring_t* string, tb_char_t const* s, tb_size_t n);
tb_char_t const* 	tb_pstring_cstrfcpy(tb_pstring_t* string, tb_char_t const* fmt, ...);

// chrcat
tb_char_t const* 	tb_pstring_chrcat(tb_pstring_t* string, tb_char_t c);
tb_char_t const* 	tb_pstring_chrncat(tb_pstring_t* string, tb_char_t c, tb_size_t n);

// strcat
tb_char_t const* 	tb_pstring_strcat(tb_pstring_t* string, tb_pstring_t const* s);
tb_char_t const* 	tb_pstring_cstrcat(tb_pstring_t* string, tb_char_t const* s);
tb_char_t const* 	tb_pstring_cstrncat(tb_pstring_t* string, tb_char_t const* s, tb_size_t n);
tb_char_t const* 	tb_pstring_cstrfcat(tb_pstring_t* string, tb_char_t const* fmt, ...);

// strcmp
tb_long_t 			tb_pstring_strcmp(tb_pstring_t* string, tb_pstring_t const* s);
tb_long_t 			tb_pstring_strimp(tb_pstring_t* string, tb_pstring_t const* s);

tb_long_t 			tb_pstring_cstrcmp(tb_pstring_t* string, tb_char_t const* s);
tb_long_t 			tb_pstring_cstricmp(tb_pstring_t* string, tb_char_t const* s);

tb_long_t 			tb_pstring_cstrncmp(tb_pstring_t* string, tb_char_t const* s, tb_size_t n);
tb_long_t 			tb_pstring_cstrnicmp(tb_pstring_t* string, tb_char_t const* s, tb_size_t n);

#endif

