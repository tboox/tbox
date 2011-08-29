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
 * \file		string.h
 *
 */
#ifndef TB_LIBC_STRING_H
#define TB_LIBC_STRING_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_void_t* 	tb_memset(tb_void_t* s, tb_size_t c, tb_size_t n);
tb_void_t* 	tb_memcpy(tb_void_t* s1, tb_void_t const* s2, tb_size_t n);
tb_void_t* 	tb_memmov(tb_void_t* s1, tb_void_t const* s2, tb_size_t n);

tb_size_t 	tb_strlen(tb_char_t const* s);
tb_size_t 	tb_strnlen(tb_char_t const* s, tb_size_t n);

tb_char_t* 	tb_strdup(tb_char_t const* s);
tb_char_t* 	tb_strndup(tb_char_t const* s, tb_size_t n);

tb_char_t* 	tb_strcat(tb_char_t* s1, tb_char_t const* s2);
tb_char_t* 	tb_strcpy(tb_char_t* s1, tb_char_t const* s2);
tb_char_t* 	tb_strncpy(tb_char_t* s1, tb_char_t const* s2, tb_size_t n);

tb_int_t 	tb_strcmp(tb_char_t* s1, tb_char_t const* s2);
tb_int_t 	tb_strncmp(tb_char_t* s1, tb_char_t const* s2, tb_size_t n);

tb_int_t 	tb_stricmp(tb_char_t const* s1, tb_char_t const* s2);
tb_int_t 	tb_strnicmp(tb_char_t const* s1, tb_char_t const* s2, tb_size_t n);

tb_char_t* 	tb_strchr(tb_char_t const* s, tb_char_t c);
tb_char_t* 	tb_strstr(tb_char_t const* s1, tb_char_t const* s2);
tb_char_t* 	tb_stristr(tb_char_t const* s1, tb_char_t const* s2);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
