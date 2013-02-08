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
 * @file		dictionary.h
 * @ingroup 	object
 *
 */
#ifndef TB_OBJECT_DICTIONARY_H
#define TB_OBJECT_DICTIONARY_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DICTIONARY_SIZE_MICRO 				(64)
#define TB_DICTIONARY_SIZE_SMALL 				(256)
#define TB_DICTIONARY_SIZE_LARGE 				(65536)

#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_DICTIONARY_SIZE_DEFAULT 			TB_DICTIONARY_SIZE_SMALL
#else
# 	define TB_DICTIONARY_SIZE_DEFAULT 			TB_DICTIONARY_SIZE_LARGE
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the dictionary item type
typedef struct __tb_dictionary_item_t
{
	/// the key
	tb_char_t const* 		key;

	/// the value
	tb_object_t* 			val;

}tb_dictionary_item_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init dictionary reader
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_dictionary_init_reader();

/*! init dictionary writer
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_dictionary_init_writer();

/*! init dictionary
 *
 * @param size			the dictionary size
 *
 * @return 				the dictionary object
 */
tb_object_t* 			tb_dictionary_init(tb_size_t size);

/*! the dictionary size
 *
 * @param dictionary	the dictionary object
 *
 * @return 				the dictionary size
 */
tb_size_t 				tb_dictionary_size(tb_object_t* dictionary);

/*! the dictionary iterator
 *
 * @param dictionary	the dictionary object
 *
 * @return 				the dictionary iterator
 *
 * @code
 * tb_iterator_t* 	iterator = tb_dictionary_itor(dictionary);
 * tb_size_t 		itor = tb_iterator_head(iterator);
 * tb_size_t 		tail = tb_iterator_tail(iterator);
 * for (; itor != tail; itor = tb_iterator_next(iterator, itor))
 * {
 * 		tb_dictionary_item_t* item = tb_iterator_item(iterator, itor);
 * 		if (item)
 * 		{
 * 			tb_char_t const* 	key = item->key;
 * 			tb_object_t* 		val = item->val;
 *
 * 			// ...
 * 		}
 * }
 * @codeend
 */
tb_iterator_t* 			tb_dictionary_itor(tb_object_t* dictionary);

/*! the dictionary value
 *
 * @param dictionary	the dictionary object
 * @param key			the key
 *
 * @return 				the dictionary value
 */
tb_object_t* 			tb_dictionary_val(tb_object_t* dictionary, tb_char_t const* key);

/*! del dictionary item
 *
 * @param dictionary	the dictionary object
 * @param key			the key
 */
tb_void_t 				tb_dictionary_del(tb_object_t* dictionary, tb_char_t const* key);

/*! set dictionary item
 *
 * @param dictionary	the dictionary object
 * @param key			the key
 * @param val			the value
 */
tb_void_t 				tb_dictionary_set(tb_object_t* dictionary, tb_char_t const* key, tb_object_t* val);

#endif

