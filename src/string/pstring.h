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

/// the scoped string type
typedef tb_pbuffer_t 	tb_pstring_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init string
 *
 * @param string 		the string
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t				tb_pstring_init(tb_pstring_t* string);

/*! exit string
 *
 * @param string 		the string
 */
tb_void_t 				tb_pstring_exit(tb_pstring_t* string);

/*! the c-string pointer
 *
 * @param string 		the string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_cstr(tb_pstring_t const* string);

/*! the string size
 *
 * @param string 		the string
 *
 * @return 				the string size
 */
tb_size_t 				tb_pstring_size(tb_pstring_t const* string);

/*! clear the string
 *
 * @param string 		the string
 */
tb_void_t 				tb_pstring_clear(tb_pstring_t* string);

/*! strip the string
 *
 * @param string 		the string
 * @param n 			the striped size
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_strip(tb_pstring_t* string, tb_size_t n);

/*! find charactor position
 *
 * @param string 		the string
 * @param p 			the start position
 * @param c 			the finded charactor
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_strchr(tb_pstring_t const* string, tb_size_t p, tb_char_t c);

/*! find charactor position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param c 			the finded charactor
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_strichr(tb_pstring_t const* string, tb_size_t p, tb_char_t c);

/*! reverse to find charactor position
 *
 * @param string 		the string
 * @param p 			the start position
 * @param c 			the finded charactor
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_strrchr(tb_pstring_t const* string, tb_size_t p, tb_char_t c);

/*! reverse to find charactor position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param c 			the finded charactor
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_strirchr(tb_pstring_t const* string, tb_size_t p, tb_char_t c);

/*! find string position 
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_strstr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s);

/*! find string position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_stristr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s);

/*! find c-string position 
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded c-string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_cstrstr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s);

/*! find c-string position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded c-string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_cstristr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s);

/*! reverse to find string position 
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_strrstr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s);

/*! reverse to find string position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_strirstr(tb_pstring_t const* string, tb_size_t p, tb_pstring_t const* s);

/*! reverse to find c-string position 
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded c-string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_cstrrstr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s);

/*! reverse to find c-string position and ignore case
 *
 * @param string 		the string
 * @param p 			the start position
 * @param s 			the finded c-string
 *
 * @return 				the real position, no find: -1
 */
tb_long_t 				tb_pstring_cstrirstr(tb_pstring_t const* string, tb_size_t p, tb_char_t const* s);

/*! copy string
 *
 * @param string 		the string
 * @param s 			the copied string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_strcpy(tb_pstring_t* string, tb_pstring_t const* s);

/*! copy c-string
 *
 * @param string 		the string
 * @param s 			the copied c-string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_cstrcpy(tb_pstring_t* string, tb_char_t const* s);

/*! copy c-string with the given size
 *
 * @param string 		the string
 * @param s 			the copied c-string
 * @param n 			the copied c-string size
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_cstrncpy(tb_pstring_t* string, tb_char_t const* s, tb_size_t n);

/*! copy format c-string
 *
 * @param string 		the string
 * @param fmt 			the copied format c-string 
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_cstrfcpy(tb_pstring_t* string, tb_char_t const* fmt, ...);

/*! append charactor
 *
 * @param string 		the string
 * @param c 			the appended charactor
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_chrcat(tb_pstring_t* string, tb_char_t c);

/*! append charactor with the given size
 *
 * @param string 		the string
 * @param c 			the appended charactor
 * @param n 			the appended size
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_chrncat(tb_pstring_t* string, tb_char_t c, tb_size_t n);

/*! append string
 *
 * @param string 		the string
 * @param s 			the appended string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_strcat(tb_pstring_t* string, tb_pstring_t const* s);

/*! append c-string
 *
 * @param string 		the string
 * @param s 			the appended c-string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_cstrcat(tb_pstring_t* string, tb_char_t const* s);

/*! append c-string with the given size
 *
 * @param string 		the string
 * @param s 			the appended c-string
 * @param n 			the appended c-string size
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_cstrncat(tb_pstring_t* string, tb_char_t const* s, tb_size_t n);

/*! append format c-string 
 *
 * @param string 		the string
 * @param fmt 			the appended format c-string
 *
 * @return 				the c-string
 */
tb_char_t const* 		tb_pstring_cstrfcat(tb_pstring_t* string, tb_char_t const* fmt, ...);

/*! compare string
 *
 * @param string 		the string
 * @param s 			the compared string
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_pstring_strcmp(tb_pstring_t* string, tb_pstring_t const* s);

/*! compare string and ignore case
 *
 * @param string 		the string
 * @param s 			the compared string
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_pstring_strimp(tb_pstring_t* string, tb_pstring_t const* s);

/*! compare c-string
 *
 * @param string 		the string
 * @param s 			the compared c-string
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_pstring_cstrcmp(tb_pstring_t* string, tb_char_t const* s);

/*! compare c-string and ignore case
 *
 * @param string 		the string
 * @param s 			the compared c-string
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_pstring_cstricmp(tb_pstring_t* string, tb_char_t const* s);

/*! compare c-string with given size
 *
 * @param string 		the string
 * @param s 			the compared c-string
 * #param n 			the compared c-string size
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_pstring_cstrncmp(tb_pstring_t* string, tb_char_t const* s, tb_size_t n);

/*! compare c-string with given size and ignore case
 *
 * @param string 		the string
 * @param s 			the compared c-string
 * #param n 			the compared c-string size
 *
 * @return 				equal: 0
 */
tb_long_t 				tb_pstring_cstrnicmp(tb_pstring_t* string, tb_char_t const* s, tb_size_t n);

#endif

