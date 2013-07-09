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
#ifdef __tb_small__
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

	// is increase refn?
	tb_bool_t 			incr;

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
	tb_array_t* copy = tb_array_init(array->vector->grow, array->incr);
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
static tb_object_t* tb_array_read_xml(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_array_init(TB_ARRAY_GROW, tb_false);

	// init array
	tb_object_t* array = tb_array_init(TB_ARRAY_GROW, tb_false);
	tb_assert_and_check_return_val(array, tb_null);

	// walk
	tb_bool_t ok = tb_false;
	while (!ok && (event = tb_xml_reader_next(reader->reader)))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				tb_trace_impl("item: %s", name);

				// func
				tb_object_xml_reader_func_t func = tb_object_get_xml_reader(name);
				tb_assert_and_check_goto(func, end);

				// read
				tb_object_t* object = func(reader, event);

				// append object
				if (object) tb_array_append(array, object);
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
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
static tb_bool_t tb_array_writ_xml(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_array_size(object))
	{
		// writ beg
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<array>");
		tb_object_writ_newline(writer->stream, writer->deflate);

		// walk
		tb_iterator_t* 	iterator = tb_array_itor(object);
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
				tb_assert_and_check_continue(func);

				// writ
				if (!func(writer, item, level + 1)) return tb_false;
			}
		}

		// writ end
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "</array>");
		tb_object_writ_newline(writer->stream, writer->deflate);
	}
	else 
	{
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<array/>");
		tb_object_writ_newline(writer->stream, writer->deflate);
	}

	// ok
	return tb_true;
}
static tb_object_t* tb_array_read_bin(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

	// empty?
	if (!size) return tb_array_init(TB_ARRAY_GROW, tb_false);

	// init array
	tb_object_t* array = tb_array_init(TB_ARRAY_GROW, tb_false);
	tb_assert_and_check_return_val(array, tb_null);

	// walk
	tb_size_t i = 0;
	tb_size_t n = (tb_size_t)size;
	for (i = 0; i < n; i++)
	{
		// the type & size
		tb_size_t 				type = 0;
		tb_uint64_t 			size = 0;
		tb_object_read_bin_type_size(reader->stream, &type, &size);

		// trace
		tb_trace_impl("item: type: %lu, size: %llu", type, size);

		// is index?
		tb_object_t* item = tb_null;
		if (!type)
		{
			// the object index
			tb_size_t index = (tb_size_t)size;
		
			// check
			tb_assert_and_check_break(index < tb_vector_size(reader->list));

			// the item
			item = (tb_object_t*)tb_iterator_item(reader->list, index);

			// refn++
			if (item) tb_object_inc(item);
		}
		else
		{
			// the reader func
			tb_object_bin_reader_func_t func = tb_object_get_bin_reader(type);
			tb_assert_and_check_break(func);

			// read it
			item = func(reader, type, size);

			// save it
			tb_vector_insert_tail(reader->list, item);
		}

		// check
		tb_assert_and_check_break(item);

		// append item
		tb_array_append(array, item);
	}

	// failed?
	if (i != n)
	{
		if (array) tb_object_exit(array);
		array = tb_null;
	}

	// ok?
	return array;
}
static tb_bool_t tb_array_writ_bin(tb_object_bin_writer_t* writer, tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && writer && writer->stream && writer->hash, tb_false);

	// writ type & size
	if (!tb_object_writ_bin_type_size(writer->stream, object->type, tb_array_size(object))) return tb_false;

	// walk
	tb_iterator_t* 	iterator = tb_array_itor(object);
	tb_size_t 		itor = tb_iterator_head(iterator);
	tb_size_t 		tail = tb_iterator_tail(iterator);
	for (; itor != tail; itor = tb_iterator_next(iterator, itor))
	{
		tb_object_t* item = tb_iterator_item(iterator, itor);
		if (item)
		{
			// exists?
			tb_size_t index = (tb_size_t)tb_hash_get(writer->hash, item);
			if (index)
			{
				// writ index
				if (!tb_object_writ_bin_type_size(writer->stream, 0, (tb_uint64_t)(index - 1))) return tb_false;
			}
			else
			{
				// the func
				tb_object_bin_writer_func_t func = tb_object_get_bin_writer(item->type);
				tb_assert_and_check_return_val(func, tb_false);

				// writ it
				func(writer, item);

				// save index
				tb_hash_set(writer->hash, item, (tb_cpointer_t)(++writer->index));
			}
		}
	}

	// ok
	return tb_true;
}
static tb_object_t* tb_array_read_jsn(tb_object_jsn_reader_t* reader, tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && type == '[', tb_null);

	// init array
	tb_object_t* array = tb_array_init(TB_ARRAY_GROW, tb_false);
	tb_assert_and_check_return_val(array, tb_null);

	// walk
	tb_char_t ch;
	tb_bool_t ok = tb_false;
	while (tb_gstream_left(reader->stream)) 
	{
		// read one character
		ch = tb_gstream_bread_s8(reader->stream);

		// end?
		if (ch == ']') break;
		// no space? skip ','
		else if (!tb_isspace(ch) && ch != ',')
		{
			// the func
			tb_object_jsn_reader_func_t func = tb_object_get_jsn_reader(ch);
			tb_assert_and_check_goto(func, end);

			// read item
			tb_object_t* item = func(reader, ch);
			tb_assert_and_check_goto(item, end);

			// append item
			tb_array_append(array, item);
		}
	}

	// ok
	ok = tb_true;

end:

	// failed?
	if (!ok && array)
	{
		tb_object_exit(array);
		array = tb_null;
	}

	// ok?
	return array;
}
static tb_bool_t tb_array_writ_jsn(tb_object_jsn_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_array_size(object))
	{
		// writ beg
		tb_gstream_printf(writer->stream, "[");
		tb_object_writ_newline(writer->stream, writer->deflate);

		// walk
		tb_iterator_t* 	iterator = tb_array_itor(object);
		tb_size_t 		itor = tb_iterator_head(iterator);
		tb_size_t 		head = tb_iterator_head(iterator);
		tb_size_t 		tail = tb_iterator_tail(iterator);
		for (; itor != tail; itor = tb_iterator_next(iterator, itor))
		{
			// item
			tb_object_t* item = tb_iterator_item(iterator, itor);
			if (item)
			{
				// func
				tb_object_jsn_writer_func_t func = tb_object_get_jsn_writer(item->type);
				tb_assert_and_check_continue(func);

				// writ tab
				if (itor != head)
				{
					tb_object_writ_tab(writer->stream, writer->deflate, level);
					tb_gstream_printf(writer->stream, ",");
					tb_object_writ_tab(writer->stream, writer->deflate, 1);
				}
				else tb_object_writ_tab(writer->stream, writer->deflate, level + 1);

				// writ
				if (!func(writer, item, level + 1)) return tb_false;
			}
		}

		// writ end
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "]");
		tb_object_writ_newline(writer->stream, writer->deflate);
	}
	else 
	{
		tb_gstream_printf(writer->stream, "[]");
		tb_object_writ_newline(writer->stream, writer->deflate);
	}

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_array_init_reader()
{
	if (!tb_object_set_xml_reader("array", tb_array_read_xml)) return tb_false;
	if (!tb_object_set_bin_reader(TB_OBJECT_TYPE_ARRAY, tb_array_read_bin)) return tb_false;
	if (!tb_object_set_jsn_reader('[', tb_array_read_jsn)) return tb_false;
	return tb_true;
}
tb_bool_t tb_array_init_writer()
{
	if (!tb_object_set_xml_writer(TB_OBJECT_TYPE_ARRAY, tb_array_writ_xml)) return tb_false;
	if (!tb_object_set_bin_writer(TB_OBJECT_TYPE_ARRAY, tb_array_writ_bin)) return tb_false;
	if (!tb_object_set_jsn_writer(TB_OBJECT_TYPE_ARRAY, tb_array_writ_jsn)) return tb_false;
	return tb_true;
}
tb_object_t* tb_array_init(tb_size_t grow, tb_bool_t incr)
{
	// make
	tb_array_t* array = tb_array_init_base();
	tb_assert_and_check_return_val(array, tb_null);

	// init item func
	tb_item_func_t func = tb_item_func_obj();

	// init vector
	array->vector = tb_vector_init(grow, func);
	tb_assert_and_check_goto(array->vector, fail);

	// init incr
	array->incr = incr;

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

	// insert
	tb_vector_insert_tail(array->vector, item);

	// refn--
	if (!array->incr) tb_object_dec(item);
}
tb_void_t tb_array_insert(tb_object_t* object, tb_size_t index, tb_object_t* item)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return(array && array->vector && item);

	// insert
	tb_vector_insert(array->vector, index, item);

	// refn--
	if (!array->incr) tb_object_dec(item);
}
tb_void_t tb_array_replace(tb_object_t* object, tb_size_t index, tb_object_t* item)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return(array && array->vector && item);

	// replace
	tb_vector_replace(array->vector, index, item);

	// refn--
	if (!array->incr) tb_object_dec(item);
}
tb_void_t tb_array_incr(tb_object_t* object, tb_bool_t incr)
{
	tb_array_t* array = tb_array_cast(object);
	tb_assert_and_check_return(array);

	array->incr = incr;
}
