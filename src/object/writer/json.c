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
 * @file		json.c
 * @ingroup 	object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"object_writer_json"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "json.h"
#include "../object.h"
#include "../../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_object_json_writer_func_null(tb_object_json_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_basic_stream_printf(writer->stream, "null") < 0) return tb_false;
	if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

	// ok
	return tb_true;
}
static tb_bool_t tb_object_json_writer_func_array(tb_object_json_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_array_size(object))
	{
		// writ beg
		if (tb_basic_stream_printf(writer->stream, "[") < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

		// walk
		tb_for_all (tb_object_t*, item, tb_array_itor(object))
		{
			// item
			if (item)
			{
				// func
				tb_object_json_writer_func_t func = tb_object_json_writer_func(item->type);
				tb_assert_and_check_continue(func);

				// writ tab
				if (item_itor != item_head)
				{
					if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
					if (tb_basic_stream_printf(writer->stream, ",") < 0) return tb_false;
					if (!tb_object_writer_tab(writer->stream, writer->deflate, 1)) return tb_false;
				}
				else if (!tb_object_writer_tab(writer->stream, writer->deflate, level + 1)) return tb_false;

				// writ
				if (!func(writer, item, level + 1)) return tb_false;
			}
		}

		// writ end
		if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
		if (tb_basic_stream_printf(writer->stream, "]") < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
	}
	else 
	{
		if (tb_basic_stream_printf(writer->stream, "[]") < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_object_json_writer_func_string(tb_object_json_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_string_size(object))
	{
		if (tb_basic_stream_printf(writer->stream, "\"%s\"", tb_string_cstr(object)) < 0) return tb_false;
	}
	else if (tb_basic_stream_printf(writer->stream, "\"\"") < 0) return tb_false;
	if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

	// ok
	return tb_true;
}
static tb_bool_t tb_object_json_writer_func_number(tb_object_json_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	switch (tb_number_type(object))
	{
	case TB_NUMBER_TYPE_UINT64:
		if (tb_basic_stream_printf(writer->stream, "%llu", tb_number_uint64(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT64:
		if (tb_basic_stream_printf(writer->stream, "%lld", tb_number_sint64(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
	case TB_NUMBER_TYPE_UINT32:
		if (tb_basic_stream_printf(writer->stream, "%u", tb_number_uint32(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT32:
		if (tb_basic_stream_printf(writer->stream, "%d", tb_number_sint32(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
	case TB_NUMBER_TYPE_UINT16:
		if (tb_basic_stream_printf(writer->stream, "%u", tb_number_uint16(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT16:
		if (tb_basic_stream_printf(writer->stream, "%d", tb_number_sint16(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
	case TB_NUMBER_TYPE_UINT8:
		if (tb_basic_stream_printf(writer->stream, "%u", tb_number_uint8(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT8:
		if (tb_basic_stream_printf(writer->stream, "%d", tb_number_sint8(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
#ifdef TB_CONFIG_TYPE_FLOAT
	case TB_NUMBER_TYPE_FLOAT:
		if (tb_basic_stream_printf(writer->stream, "%f", tb_number_float(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
	case TB_NUMBER_TYPE_DOUBLE:
		if (tb_basic_stream_printf(writer->stream, "%lf", tb_number_double(object)) < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
		break;
#endif
	default:
		break;
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_object_json_writer_func_boolean(tb_object_json_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_basic_stream_printf(writer->stream, "%s", tb_boolean_bool(object)? "true" : "false") < 0) return tb_false;
	if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

	// ok
	return tb_true;
}
static tb_bool_t tb_object_json_writer_func_dictionary(tb_object_json_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_dictionary_size(object))
	{
		// writ beg
		if (tb_basic_stream_printf(writer->stream, "{") < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

		// walk
		tb_for_all (tb_dictionary_item_t*, item, tb_dictionary_itor(object))
		{
			// item
			if (item && item->key && item->val)
			{
				// func
				tb_object_json_writer_func_t func = tb_object_json_writer_func(item->val->type);
				tb_assert_and_check_continue(func);

				// writ tab
				if (item_itor != item_head)
				{
					if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
					if (tb_basic_stream_printf(writer->stream, ",") < 0) return tb_false;
					if (!tb_object_writer_tab(writer->stream, writer->deflate, 1)) return tb_false;
				}
				else if (!tb_object_writer_tab(writer->stream, writer->deflate, level + 1)) return tb_false;

				// writ key
				if (tb_basic_stream_printf(writer->stream, "\"%s\":", item->key) < 0) return tb_false;

				// writ spaces
				if (!writer->deflate) if (tb_basic_stream_printf(writer->stream, " ") < 0) return tb_false;
				if (item->val->type == TB_OBJECT_TYPE_DICTIONARY || item->val->type == TB_OBJECT_TYPE_ARRAY)
				{
					if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
					if (!tb_object_writer_tab(writer->stream, writer->deflate, level + 1)) return tb_false;
				}

				// writ val
				if (!func(writer, item->val, level + 1)) return tb_false;
			}
		}

		// writ end
		if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
		if (tb_basic_stream_printf(writer->stream, "}") < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
	}
	else 
	{
		if (tb_basic_stream_printf(writer->stream, "{}") < 0) return tb_false;
		if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
	}

	// ok
	return tb_true;
}
static tb_long_t tb_object_json_writer_done(tb_basic_stream_t* stream, tb_object_t* object, tb_bool_t deflate)
{
	// check
	tb_assert_and_check_return_val(object && stream, -1);

	// init writer 
	tb_object_json_writer_t writer = {0};
	writer.stream 	= stream;
	writer.deflate 	= deflate;

	// func
	tb_object_json_writer_func_t func = tb_object_json_writer_func(object->type);
	tb_assert_and_check_return_val(func, tb_false);

	// the begin offset
	tb_hize_t bof = tb_stream_offset(stream);

	// writ
	if (!func(&writer, object, 0)) return -1;

	// sync
	if (!tb_basic_stream_sync(stream, tb_true)) return -1;

	// the end offset
	tb_hize_t eof = tb_stream_offset(stream);

	// ok?
	return eof >= bof? (tb_long_t)(eof - bof) : -1;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_writer_t* tb_object_json_writer()
{
	// the writer
	static tb_object_writer_t s_writer = {0};
  
	// init writer
	s_writer.writ = tb_object_json_writer_done;
 
	// init hooker
	s_writer.hooker = tb_hash_init(TB_HASH_BULK_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(s_writer.hooker, tb_null);

	// hook writer 
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NULL, tb_object_json_writer_func_null);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_ARRAY, tb_object_json_writer_func_array);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_STRING, tb_object_json_writer_func_string);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NUMBER, tb_object_json_writer_func_number);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_BOOLEAN, tb_object_json_writer_func_boolean);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DICTIONARY, tb_object_json_writer_func_dictionary);

	// ok
	return &s_writer;
}
tb_bool_t tb_object_json_writer_hook(tb_size_t type, tb_object_json_writer_func_t func)
{
	// check
	tb_assert_and_check_return_val(func, tb_false);
 
	// the writer
	tb_object_writer_t* writer = tb_object_writer_get(TB_OBJECT_FORMAT_JSON);
	tb_assert_and_check_return_val(writer && writer->hooker, tb_false);

	// hook it
	tb_hash_set(writer->hooker, (tb_pointer_t)type, func);

	// ok
	return tb_true;
}
tb_object_json_writer_func_t tb_object_json_writer_func(tb_size_t type)
{
	// the writer
	tb_object_writer_t* writer = tb_object_writer_get(TB_OBJECT_FORMAT_JSON);
	tb_assert_and_check_return_val(writer && writer->hooker, tb_null);

	// the func
	return tb_hash_get(writer->hooker, (tb_pointer_t)type);
}

