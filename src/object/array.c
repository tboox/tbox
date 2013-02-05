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
 * @file		array.c
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
 * macros
 */
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_ARRAY_GROW 			(64)
#else
# 	define TB_ARRAY_GROW 			(256)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the array type
typedef struct __tb_array_t
{
	// the object base
	tb_object_t 		base;

	// the vector
	tb_vector_t* 		vector;

}tb_array_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_array_t* tb_array_cast(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_ARRAY, tb_null);

	// cast
	return (tb_array_t*)object;
}
static tb_bool_t tb_array_item_walk_incf(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(vector && item, tb_false);

	// refn++
	tb_object_t* object = *((tb_object_t**)item);
	if (object) tb_object_inc(object);

	// ok
	return tb_true;
}
static tb_object_t* tb_array_copy(tb_object_t* object)
{
	// check
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return_val(array && array->vector, tb_null);

	// init copy
	tb_array_t* copy = tb_array_init(array->vector->grow);
	tb_assert_and_check_return_val(copy && copy->vector, tb_null);

	// refn++
	tb_vector_walk(array->vector, tb_array_item_walk_incf, tb_null);

	// copy
	tb_vector_copy(copy->vector, array->vector);

	// ok
	return copy;
}
static tb_void_t tb_array_exit(tb_object_t* object)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return(array);

	// exit vector
	if (array->vector) tb_vector_exit(array->vector);
	array->vector = tb_null;

	// exit it
	tb_free(array);
}
static tb_void_t tb_array_cler(tb_object_t* object)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return(array && array->vector);

	// clear vector
	tb_vector_clear(array->vector);
}
static tb_array_t* tb_array_init_base()
{
	// make
	tb_array_t* array = tb_malloc0(sizeof(tb_array_t));
	tb_assert_and_check_return_val(array, tb_null);

	// init object
	if (!tb_object_init(array, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_ARRAY)) goto fail;

	// init base
	array->base.copy = tb_array_copy;
	array->base.cler = tb_array_cler;
	array->base.exit = tb_array_exit;

	// ok
	return array;

	// no
fail:
	if (array) tb_free(array);
	return tb_null;
}
static tb_void_t tb_array_item_free(tb_item_func_t* func, tb_pointer_t item)
{
	// object
	tb_object_t* object = item? *((tb_object_t**)item) : tb_null;
	tb_check_return(object);

	// refn--
	tb_object_dec(object);
}
static tb_object_t* tb_array_read_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_array_init(TB_ARRAY_GROW);

	// init array
	tb_object_t* array = tb_array_init(TB_ARRAY_GROW);
	tb_assert_and_check_return_val(array, tb_null);

	// walk
	tb_bool_t ok = tb_false;
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
				tb_trace_impl("item: %s", name);

				// func
				tb_object_xml_reader_func_t func = tb_object_get_xml_reader(name);
				tb_assert_and_check_goto(func, end);

				// read
				tb_object_t* object = func(reader, event);

				// append object
				if (array && object) tb_array_append(array, object);
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "array")) ok = tb_true;
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
		if (array) tb_object_exit(array);
		array = tb_null;
	}

	// ok?
	return array;
}
static tb_bool_t tb_array_writ_xml(tb_object_t* object, tb_gstream_t* gst, tb_size_t level)
{
	// check
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return_val(array, tb_false);

	// writ
	if (tb_array_size(array))
	{
		// writ beg
		tb_object_writ_tab(gst, level);
		tb_gstream_printf(gst, "<array>\n");

		// walk
		tb_iterator_t* 	iterator = tb_array_itor(array);
		tb_size_t 		itor = tb_iterator_head(iterator);
		tb_size_t 		tail = tb_iterator_tail(iterator);
		for (; itor != tail; itor = tb_iterator_next(iterator, itor))
		{
			// item
			tb_object_t* item = tb_iterator_item(iterator, itor);
			if (item)
			{
				// func
				tb_object_xml_writer_func_t func = tb_object_get_xml_writer(item->type);
				tb_assert_and_check_return_val(func, tb_false);

				// writ
				if (!func(item, gst, level + 1)) return tb_false;
			}
		}

		// writ end
		tb_object_writ_tab(gst, level);
		tb_gstream_printf(gst, "</array>\n");
	}
	else 
	{
		tb_object_writ_tab(gst, level);
		tb_gstream_printf(gst, "<array/>\n");
	}

	// ok
	return tb_true;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_array_init_reader()
{
	return tb_object_set_xml_reader("array", tb_array_read_xml);
}
tb_bool_t tb_array_init_writer()
{
	return tb_object_set_xml_writer(TB_OBJECT_TYPE_ARRAY, tb_array_writ_xml);
}
tb_object_t* tb_array_init(tb_size_t grow)
{
	// make
	tb_array_t* array = tb_array_init_base();
	tb_assert_and_check_return_val(array, tb_null);

	// init item func
	tb_item_func_t func = tb_item_func_ptr();
	func.free = tb_array_item_free;

	// init vector
	array->vector = tb_vector_init(grow, func);
	tb_assert_and_check_goto(array->vector, fail);

	// ok
	return array;

fail:
	// no
	tb_array_exit(array);
	return tb_null;
}
tb_size_t tb_array_size(tb_object_t* object)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return_val(array && array->vector, 0);

	// size
	return tb_vector_size(array->vector);
}
tb_object_t* tb_array_item(tb_object_t* object, tb_size_t index)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return_val(array && array->vector, tb_null);

	// item
	return (tb_object_t*)tb_iterator_item(array->vector, index);
}
tb_iterator_t* tb_array_itor(tb_object_t* object)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return_val(array, tb_null);

	// iterator
	return (tb_iterator_t*)array->vector;
}
tb_void_t tb_array_remove(tb_object_t* object, tb_size_t index)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return(array && array->vector);

	// remove
	tb_vector_remove(array->vector, index);
}
tb_void_t tb_array_append(tb_object_t* object, tb_object_t* item)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return(array && array->vector && item);

	// refn++
	tb_object_inc(item);

	// insert
	tb_vector_insert_tail(array->vector, item);
}
tb_void_t tb_array_insert(tb_object_t* object, tb_size_t index, tb_object_t* item)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return(array && array->vector && item);

	// refn++
	tb_object_inc(item);

	// insert
	tb_vector_insert(array->vector, index, item);
}
tb_void_t tb_array_replace(tb_object_t* object, tb_size_t index, tb_object_t* item)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return(array && array->vector && item);

	// refn++
	tb_object_inc(item);

	// replace
	tb_vector_replace(array->vector, index, item);
}

