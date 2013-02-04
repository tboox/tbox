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
 * @file		dictionary.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the dictionary type
typedef struct __tb_dictionary_t
{
	// the object base
	tb_object_t 		base;

	// the capacity size
	tb_size_t 			size;

	// the object hash
	tb_hash_t* 			hash;

	// the string pool
	tb_handle_t 		pool;

}tb_dictionary_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_dictionary_t* tb_dictionary_cast(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_DICTIONARY, tb_null);

	// cast
	return (tb_dictionary_t*)object;
}
static tb_object_t* tb_dictionary_copy(tb_object_t* object)
{
	// check
	tb_dictionary_t* dictionary = tb_dictionary_cast(object);
	tb_assert_and_check_return_val(dictionary, tb_null);

	// init copy
	tb_dictionary_t* copy = tb_dictionary_init(dictionary->size);
	tb_assert_and_check_return_val(copy, tb_null);

	// walk copy
	tb_iterator_t* 	iterator 	= tb_dictionary_itor(dictionary);
	tb_size_t 		itor 		= tb_iterator_head(iterator);
	tb_size_t 		tail 		= tb_iterator_tail(iterator);
	for (; itor != tail; itor = tb_iterator_next(iterator, itor))
	{
		tb_dictionary_item_t* item = tb_iterator_item(iterator, itor);
		if (item && item->key) 
		{
			// refn++
			if (item->val) tb_object_inc(item->val);

			// copy
			tb_dictionary_set(copy, item->key, item->val);
		}
	}

	// ok
	return copy;
}
static tb_void_t tb_dictionary_exit(tb_object_t* object)
{
	tb_dictionary_t* dictionary = tb_dictionary_cast(object);
	tb_assert_and_check_return(dictionary);

	// exit hash
	if (dictionary->hash) tb_hash_exit(dictionary->hash);
	dictionary->hash = tb_null;

	// exit pool
	if (dictionary->pool) tb_spool_exit(dictionary->pool);
	dictionary->pool = tb_null;

	// exit it
	tb_free(dictionary);
}
static tb_void_t tb_dictionary_cler(tb_object_t* object)
{
	tb_dictionary_t* dictionary = tb_dictionary_cast(object);
	tb_assert_and_check_return(dictionary);

	// clear
	if (dictionary->hash) tb_hash_clear(dictionary->hash);

	// clear
	if (dictionary->pool) tb_spool_clear(dictionary->pool);
}
static tb_dictionary_t* tb_dictionary_init_base()
{
	// make
	tb_dictionary_t* dictionary = tb_malloc0(sizeof(tb_dictionary_t));
	tb_assert_and_check_return_val(dictionary, tb_null);

	// init object
	if (!tb_object_init(dictionary, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_DICTIONARY)) goto fail;

	// init base
	dictionary->base.copy = tb_dictionary_copy;
	dictionary->base.cler = tb_dictionary_cler;
	dictionary->base.exit = tb_dictionary_exit;

	// ok
	return dictionary;

	// no
fail:
	if (dictionary) tb_free(dictionary);
	return tb_null;
}
static tb_void_t tb_dictionary_item_free(tb_item_func_t* func, tb_pointer_t item)
{
	// object
	tb_object_t* object = item? *((tb_object_t**)item) : tb_null;
	tb_check_return(object);

	// refn--
	tb_object_dec(object);
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_t* tb_dictionary_init(tb_size_t size)
{
	// make
	tb_dictionary_t* dictionary = tb_dictionary_init_base();
	tb_assert_and_check_return_val(dictionary, tb_null);

	// init size
	dictionary->size = size;

	// init item func
	tb_item_func_t func = tb_item_func_ptr();
	func.free = tb_dictionary_item_free;

	// init pool
	dictionary->pool = tb_spool_init(TB_SPOOL_GROW_SMALL, 0);
	tb_assert_and_check_goto(dictionary->pool, fail);

	// init hash
	dictionary->hash = tb_hash_init(size, tb_item_func_str(tb_true, dictionary->pool), func);
	tb_assert_and_check_goto(dictionary->hash, fail);

	// ok
	return dictionary;

fail:
	// no
	tb_dictionary_exit(dictionary);
	return tb_null;
}

tb_size_t tb_dictionary_size(tb_object_t* object)
{
	tb_dictionary_t* dictionary = tb_dictionary_cast(object);
	tb_assert_and_check_return_val(dictionary && dictionary->hash, 0);

	// size
	return tb_hash_size(dictionary->hash);
}

tb_iterator_t* tb_dictionary_itor(tb_object_t* object)
{
	tb_dictionary_t* dictionary = tb_dictionary_cast(object);
	tb_assert_and_check_return_val(dictionary, tb_null);

	// iterator
	return (tb_iterator_t*)dictionary->hash;
}

tb_object_t* tb_dictionary_val(tb_object_t* object, tb_char_t const* key)
{
	tb_dictionary_t* dictionary = tb_dictionary_cast(object);
	tb_assert_and_check_return_val(dictionary && dictionary->hash && key, tb_null);

	// value
	return tb_hash_get(dictionary->hash, key);
}

tb_void_t tb_dictionary_del(tb_object_t* object, tb_char_t const* key)
{
	tb_dictionary_t* dictionary = tb_dictionary_cast(object);
	tb_assert_and_check_return(dictionary && dictionary->hash && key);

	// del
	return tb_hash_del(dictionary->hash, key);
}
tb_void_t tb_dictionary_set(tb_object_t* object, tb_char_t const* key, tb_object_t* val)
{
	tb_dictionary_t* dictionary = tb_dictionary_cast(object);
	tb_assert_and_check_return(dictionary && dictionary->hash && key && val);

	// refn++
	tb_object_inc(val);

	// add
	return tb_hash_set(dictionary->hash, key, val);
}


