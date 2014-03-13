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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
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

/// the static string type
typedef tb_sbuffer_t 	tb_sstring_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init string
 *
 * @param string 		the string
 * @param data 			the data
 * @param maxn 			the maxn
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t				tb_sstring_init(tb_sstring_t* string, tb_char_t* data, tb_size_t maxn);

/*! exit string
 *
 * @param string 		the string
 */
tb_void_t 				tb_sstring_exit(tb_sstring_t* string);

/*! the c-string pointer
 *
 * @param string 		the string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_cstr(tb_sstring_t const* string);

/*! the string size
 *
 * @param string 		the string
 *
 * @return 				the string size
 */
tb_size_t 				tb_sstring_size(tb_sstring_t const* string);

/*! clear the string
 *
 * @param string 		the string
 */
tb_void_t 				tb_sstring_clear(tb_sstring_t* string);

/*! strip the string
 *
 * @param string 		the string
 * @param n 			the striped size
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_strip(tb_sstring_t* string, tb_size_t n);

/*! find charactor position
 *
 * @param string 		the string
 * @param p 			the start position
 * @param c 			the finded charactor
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_strchr(tb_sstring_t const* string, tb_size_t p, tb_char_t c);

/*! find charactor position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param c 			the finded charactor
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_strichr(tb_sstring_t const* string, tb_size_t p, tb_char_t c);

/*! reverse to find charactor position
 *
 * @param string 		the string
 * @param p 			the start position
 * @param c 			the finded charactor
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_strrchr(tb_sstring_t const* string, tb_size_t p, tb_char_t c);

/*! reverse to find charactor position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param c 			the finded charactor
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_strirchr(tb_sstring_t const* string, tb_size_t p, tb_char_t c);

/*! find string position 
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_strstr(tb_sstring_t const* string, tb_size_t p, tb_sstring_t const* s);

/*! find string position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_stristr(tb_sstring_t const* string, tb_size_t p, tb_sstring_t const* s);

/*! find c-string position 
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded c-string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_cstrstr(tb_sstring_t const* string, tb_size_t p, tb_char_t const* s);

/*! find c-string position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded c-string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_cstristr(tb_sstring_t const* string, tb_size_t p, tb_char_t const* s);

/*! reverse to find string position 
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_strrstr(tb_sstring_t const* string, tb_size_t p, tb_sstring_t const* s);

/*! reverse to find string position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_strirstr(tb_sstring_t const* string, tb_size_t p, tb_sstring_t const* s);

/*! reverse to find c-string position 
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded c-string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_cstrrstr(tb_sstring_t const* string, tb_size_t p, tb_char_t const* s);

/*! reverse to find c-string position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded c-string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_sstring_cstrirstr(tb_sstring_t const* string, tb_size_t p, tb_char_t const* s);

/*! copy string
 *
 * @param string 		the string
 * @param s 			the copied string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_strcpy(tb_sstring_t* string, tb_sstring_t const* s);

/*! copy c-string
 *
 * @param string 		the string
 * @param s 			the copied c-string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_cstrcpy(tb_sstring_t* string, tb_char_t const* s);

/*! copy c-string with the given size
 *
 * @param string 		the string
 * @param s 			the copied c-string
 * @param n 			the copied c-string size
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_cstrncpy(tb_sstring_t* string, tb_char_t const* s, tb_size_t n);

/*! copy format c-string
 *
 * @param string 		the string
 * @param fmt 			the copied format c-string 
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_cstrfcpy(tb_sstring_t* string, tb_char_t const* fmt, ...);

/*! append charactor
 *
 * @param string 		the string
 * @param c 			the appended charactor
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_chrcat(tb_sstring_t* string, tb_char_t c);

/*! append charactor with the given size
 *
 * @param string 		the string
 * @param c 			the appended charactor
 * @param n 			the appended size
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_chrncat(tb_sstring_t* string, tb_char_t c, tb_size_t n);

/*! append string
 *
 * @param string 		the string
 * @param s 			the appended string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_strcat(tb_sstring_t* string, tb_sstring_t const* s);

/*! append c-string
 *
 * @param string 		the string
 * @param s 			the appended c-string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_cstrcat(tb_sstring_t* string, tb_char_t const* s);

/*! append c-string with the given size
 *
 * @param string 		the string
 * @param s 			the appended c-string
 * @param n 			the appended c-string size
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_cstrncat(tb_sstring_t* string, tb_char_t const* s, tb_size_t n);

/*! append format c-string 
 *
 * @param string 		the string
 * @param fmt 			the appended format c-string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_sstring_cstrfcat(tb_sstring_t* string, tb_char_t const* fmt, ...);

/*! compare string
 *
 * @param string 		the string
 * @param s 			the compared string
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_sstring_strcmp(tb_sstring_t* string, tb_sstring_t const* s);

/*! compare string and ignore case
 *
 * @param string 		the string
 * @param s 			the compared string
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_sstring_strimp(tb_sstring_t* string, tb_sstring_t const* s);

/*! compare c-string
 *
 * @param string 		the string
 * @param s 			the compared c-string
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_sstring_cstrcmp(tb_sstring_t* string, tb_char_t const* s);

/*! compare c-string and ignore case
 *
 * @param string 		the string
 * @param s 			the compared c-string
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_sstring_cstricmp(tb_sstring_t* string, tb_char_t const* s);

/*! compare c-string with given size
 *
 * @param string 		the string
 * @param s 			the compared c-string
 * #param n 			the compared c-string size
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_sstring_cstrncmp(tb_sstring_t* string, tb_char_t const* s, tb_size_t n);

/*! compare c-string with given size and ignore case
 *
 * @param string 		the string
 * @param s 			the compared c-string
 * #param n 			the compared c-string size
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_sstring_cstrnicmp(tb_sstring_t* string, tb_char_t const* s, tb_size_t n);

#endif

