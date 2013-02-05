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
 * trace
 */
//#define TB_TRACE_IMPL_TAG 		"object"

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

	// the dictionary pool
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
static tb_object_t* tb_dictionary_read_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_dictionary_init(TB_DICTIONARY_SIZE_MICRO);

	// init key name
	tb_sstring_t 	kname;
	tb_char_t 		kdata[8192];
	if (!tb_sstring_init(&kname, kdata, 8192)) return tb_null;

	// init dictionary
	tb_object_t* dictionary = tb_dictionary_init(TB_DICTIONARY_SIZE_DEFAULT);
	tb_assert_and_check_return_val(dictionary, tb_null);

	// walk
	tb_bool_t 	ok = tb_false;
	tb_bool_t 	key = tb_false;
	while ((event = tb_xml_reader_next(reader)) && !ok)
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);
				tb_trace_impl("%s", name);

				// is key
				if (!tb_stricmp(name, "key")) key = tb_true;
				else if (!key)
				{
					// func
					tb_object_xml_reader_func_t func = tb_object_get_xml_reader(name);
					tb_assert_and_check_goto(func, end);

					// read
					tb_object_t* object = func(reader, event);
					tb_trace_impl("%s => %p", tb_sstring_cstr(&kname), object);
					tb_assert_and_check_goto(object, end);

					// set key & value
					if (tb_sstring_size(&kname) && dictionary) 
						tb_dictionary_set(dictionary, tb_sstring_cstr(&kname), object);

					// clear key name
					tb_sstring_clear(&kname);
				}
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "dict")) ok = tb_true;
				else if (!tb_stricmp(name, "key")) key = tb_false;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				if (key)
				{
					// text
					tb_char_t* text = tb_xml_reader_text(reader);
					tb_assert_and_check_goto(text, end);

					// writ key name
					tb_sstring_cstrcpy(&kname, text);
				}
			}
			break;
		default:
			break;
		}
	}

	// ok
	ok = tb_true;

end:

	// fail
	if (!ok) 
	{
		tb_object_exit(dictionary);
		dictionary = tb_null;
	}

	// exit key name
	tb_sstring_exit(&kname);

	// ok?
	return dictionary;
}
static tb_bool_t tb_dictionary_writ_xml(tb_object_t* object, tb_gstream_t* gst, tb_size_t level)
{
	// writ
	if (tb_dictionary_size(object))
	{
		// writ beg
		tb_object_writ_tab(gst, level);
		tb_gstream_printf(gst, "<dict>\n");

		// walk
		tb_iterator_t* 	iterator = tb_dictionary_itor(object);
		tb_size_t 		itor = tb_iterator_head(iterator);
		tb_size_t 		tail = tb_iterator_tail(iterator);
		for (; itor != tail; itor = tb_iterator_next(iterator, itor))
		{
			// item
			tb_dictionary_item_t* item = tb_iterator_item(iterator, itor);
			if (item && item->key && item->val)
			{
				// writ key
				tb_object_writ_tab(gst, level + 1);
				tb_gstream_printf(gst, "<key>%s</key>\n", item->key);

				// func
				tb_object_xml_writer_func_t func = tb_object_get_xml_writer(item->val->type);
				tb_assert_and_check_return_val(func, tb_false);

				// writ val
				if (!func(item->val, gst, level + 1)) return tb_false;
			}
		}

		// writ end
		tb_object_writ_tab(gst, level);
		tb_gstream_printf(gst, "</dict>\n");
	}
	else 
	{
		tb_object_writ_tab(gst, level);
		tb_gstream_printf(gst, "<dict/>\n");
	}

	// ok
	return tb_true;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_dictionary_init_reader()
{
	return tb_object_set_xml_reader("dict", tb_dictionary_read_xml);
}
tb_bool_t tb_dictionary_init_writer()
{
	return tb_object_set_xml_writer(TB_OBJECT_TYPE_DICTIONARY, tb_dictionary_writ_xml);
}
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


