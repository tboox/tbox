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
 * @file		string.h
 * @ingroup 	object
 *
 */
#ifndef TB_OBJECT_STRING_H
#define TB_OBJECT_STRING_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init string reader
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_string_init_reader();

/*! init string writer
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_string_init_writer();

/*! init string from c-string
 *
 * @param cstr 		the c-string
 *
 * @return 			the string object
 */
tb_object_t* 		tb_string_init_from_cstr(tb_char_t const* cstr);

/*! init string from pstring
 *
 * @param pstr 		the pstring
 *
 * @return 			the string object
 */
tb_object_t* 		tb_string_init_from_pstr(tb_pstring_t* pstr);

/*! the c-string
 *
 * @param string 	the string object
 *
 * @return 			the c-string
 */
tb_char_t* 			tb_string_cstr(tb_object_t* string);

/*! the string size
 *
 * @param string 	the string object
 *
 * @return 			the string size
 */
tb_size_t 			tb_string_size(tb_object_t* string);

/*! the pstring for the string
 *
 * @param string 	the string object
 *
 * @return 			the pstring
 */
tb_pstring_t* 		tb_string_pstr(tb_object_t* string);

#endif

