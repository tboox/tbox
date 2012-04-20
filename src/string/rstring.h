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
 * @file		rstring.h
 * @ingroup 	string
 *
 */
#ifndef TB_STRING_RSTRING_H
#define TB_STRING_RSTRING_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the reference string type
typedef tb_rbuffer_t tb_rstring_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_rstring_init(tb_rstring_t* string);
tb_void_t 			tb_rstring_exit(tb_rstring_t* string);

// accessors
tb_char_t const* 	tb_rstring_cstr(tb_rstring_t const* string);
tb_size_t 			tb_rstring_size(tb_rstring_t const* string);
tb_size_t 			tb_rstring_refn(tb_rstring_t const* string);

// modifiors
tb_void_t 			tb_rstring_clear(tb_rstring_t* string);
tb_char_t const* 	tb_rstring_strip(tb_rstring_t* string, tb_size_t n);
tb_size_t 			tb_rstring_incr(tb_rstring_t* string);
tb_size_t 			tb_rstring_decr(tb_rstring_t* string);

// strchr
tb_long_t 			tb_rstring_strchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c);
tb_long_t 			tb_rstring_strichr(tb_rstring_t const* string, tb_size_t p, tb_char_t c);

// strrchr
tb_long_t 			tb_rstring_strrchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c);
tb_long_t 			tb_rstring_strirchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c);

// strstr
tb_long_t 			tb_rstring_strstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s);
tb_long_t 			tb_rstring_stristr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s);

tb_long_t 			tb_rstring_cstrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s);
tb_long_t 			tb_rstring_cstristr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s);

// strrstr
tb_long_t 			tb_rstring_strrstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s);
tb_long_t 			tb_rstring_strirstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s);

tb_long_t 			tb_rstring_cstrrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s);
tb_long_t 			tb_rstring_cstrirstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s);

// strcpy
tb_char_t const* 	tb_rstring_strcpy(tb_rstring_t* string, tb_rstring_t const* s);
tb_char_t const* 	tb_rstring_cstrcpy(tb_rstring_t* string, tb_char_t const* s);
tb_char_t const* 	tb_rstring_cstrncpy(tb_rstring_t* string, tb_char_t const* s, tb_size_t n);
tb_char_t const* 	tb_rstring_cstrfcpy(tb_rstring_t* string, tb_char_t const* fmt, ...);

// chrcat
tb_char_t const* 	tb_rstring_chrcat(tb_rstring_t* string, tb_char_t c);
tb_char_t const* 	tb_rstring_chrncat(tb_rstring_t* string, tb_char_t c, tb_size_t n);

// strcat
tb_char_t const* 	tb_rstring_strcat(tb_rstring_t* string, tb_rstring_t const* s);
tb_char_t const* 	tb_rstring_cstrcat(tb_rstring_t* string, tb_char_t const* s);
tb_char_t const* 	tb_rstring_cstrncat(tb_rstring_t* string, tb_char_t const* s, tb_size_t n);
tb_char_t const* 	tb_rstring_cstrfcat(tb_rstring_t* string, tb_char_t const* fmt, ...);

// strcmp
tb_long_t 			tb_rstring_strcmp(tb_rstring_t* string, tb_rstring_t const* s);
tb_long_t 			tb_rstring_strimp(tb_rstring_t* string, tb_rstring_t const* s);

tb_long_t 			tb_rstring_cstrcmp(tb_rstring_t* string, tb_char_t const* s);
tb_long_t 			tb_rstring_cstricmp(tb_rstring_t* string, tb_char_t const* s);

tb_long_t 			tb_rstring_cstrncmp(tb_rstring_t* string, tb_char_t const* s, tb_size_t n);
tb_long_t 			tb_rstring_cstrnicmp(tb_rstring_t* string, tb_char_t const* s, tb_size_t n);

#endif

