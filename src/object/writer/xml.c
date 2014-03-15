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
 * @file		xml.c
 * @ingroup 	object
 *
 */
 
/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"object_writer_xml"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "xml.h"
#include "../object.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_object_xml_writer_func_null(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	tb_object_writer_tab(writer->stream, writer->deflate, level);
	tb_gstream_printf(writer->stream, "<null/>");
	tb_object_writer_newline(writer->stream, writer->deflate);

	// ok
	return tb_true;
}
static tb_bool_t tb_object_xml_writer_func_date(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// no empty?
	tb_time_t time = tb_date_time(object);
	if (time > 0)
	{
		// writ beg
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<date>");

		// writ date
		tb_tm_t date = {0};
		if (tb_localtime(time, &date))
		{
			tb_gstream_printf(writer->stream, 	"%04ld-%02ld-%02ld %02ld:%02ld:%02ld"
								, 	date.year
								, 	date.month
								, 	date.mday
								, 	date.hour
								, 	date.minute
								, 	date.second);
		}
					
		// writ end
		tb_gstream_printf(writer->stream, "</date>");
		tb_object_writer_newline(writer->stream, writer->deflate);
	}
	else 
	{
		// writ
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<date/>");
		tb_object_writer_newline(writer->stream, writer->deflate);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_object_xml_writer_func_data(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// no empty?
	if (tb_data_size(object))
	{
		// writ beg
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<data>");
		tb_object_writer_newline(writer->stream, writer->deflate);

		// decode base64 data
		tb_byte_t const* 	ib = tb_data_getp(object);
		tb_size_t 			in = tb_data_size(object); 
		tb_size_t 			on = in << 1;
		tb_char_t* 			ob = tb_malloc0(on);
		tb_assert_and_check_return_val(ob && on, tb_false);
		on = tb_base64_encode(ib, in, ob, on);
		tb_trace_d("base64: %u => %u", in, on);

		// writ data
		tb_char_t const* 	p = ob;
		tb_char_t const* 	e = ob + on;
		tb_size_t 			n = 0;
		for (; p < e && *p; p++, n++)
		{
			if (!(n & 63))
			{
				if (n) tb_object_writer_newline(writer->stream, writer->deflate);
				tb_object_writer_tab(writer->stream, writer->deflate, level);
			}
			tb_gstream_printf(writer->stream, "%c", *p);
		}
		tb_object_writer_newline(writer->stream, writer->deflate);

		// free it
		tb_free(ob);
					
		// writ end
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "</data>");
		tb_object_writer_newline(writer->stream, writer->deflate);
	}
	else 
	{
		// writ
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<data/>");
		tb_object_writer_newline(writer->stream, writer->deflate);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_object_xml_writer_func_array(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_array_size(object))
	{
		// writ beg
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<array>");
		tb_object_writer_newline(writer->stream, writer->deflate);

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
				tb_object_xml_writer_func_t func = tb_object_xml_writer_func(item->type);
				tb_assert_and_check_continue(func);

				// writ
				if (!func(writer, item, level + 1)) return tb_false;
			}
		}

		// writ end
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "</array>");
		tb_object_writer_newline(writer->stream, writer->deflate);
	}
	else 
	{
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<array/>");
		tb_object_writer_newline(writer->stream, writer->deflate);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_object_xml_writer_func_string(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	tb_object_writer_tab(writer->stream, writer->deflate, level);
	if (tb_string_size(object))
		tb_gstream_printf(writer->stream, "<string>%s</string>", tb_string_cstr(object));
	else tb_gstream_printf(writer->stream, "<string/>");
	tb_object_writer_newline(writer->stream, writer->deflate);

	// ok
	return tb_true;
}
static tb_bool_t tb_object_xml_writer_func_number(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	switch (tb_number_type(object))
	{
	case TB_NUMBER_TYPE_UINT64:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%llu</number>", tb_number_uint64(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT64:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%lld</number>", tb_number_sint64(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_UINT32:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%u</number>", tb_number_uint32(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT32:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%d</number>", tb_number_sint32(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_UINT16:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%u</number>", tb_number_uint16(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT16:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%d</number>", tb_number_sint16(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_UINT8:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%u</number>", tb_number_uint8(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT8:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%d</number>", tb_number_sint8(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
#ifdef TB_CONFIG_TYPE_FLOAT
	case TB_NUMBER_TYPE_FLOAT:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%f</number>", tb_number_float(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_DOUBLE:
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%lf</number>", tb_number_double(object));
		tb_object_writer_newline(writer->stream, writer->deflate);
		break;
#endif
	default:
		break;
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_object_xml_writer_func_boolean(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	tb_object_writer_tab(writer->stream, writer->deflate, level);
	tb_gstream_printf(writer->stream, "<%s/>", tb_boolean_bool(object)? "true" : "false");
	tb_object_writer_newline(writer->stream, writer->deflate);

	// ok
	return tb_true;
}
static tb_bool_t tb_object_xml_writer_func_dictionary(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_dictionary_size(object))
	{
		// writ beg
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<dict>");
		tb_object_writer_newline(writer->stream, writer->deflate);

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
				// func
				tb_object_xml_writer_func_t func = tb_object_xml_writer_func(item->val->type);
				tb_assert_and_check_continue(func);

				// writ key
				tb_object_writer_tab(writer->stream, writer->deflate, level + 1);
				tb_gstream_printf(writer->stream, "<key>%s</key>", item->key);
				tb_object_writer_newline(writer->stream, writer->deflate);

				// writ val
				if (!func(writer, item->val, level + 1)) return tb_false;
			}
		}

		// writ end
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "</dict>");
		tb_object_writer_newline(writer->stream, writer->deflate);
	}
	else 
	{
		tb_object_writer_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<dict/>");
		tb_object_writer_newline(writer->stream, writer->deflate);
	}

	// ok
	return tb_true;
}
static tb_long_t tb_object_xml_writer_done(tb_gstream_t* stream, tb_object_t* object, tb_bool_t deflate)
{
	// check
	tb_assert_and_check_return_val(object && stream, -1);
 
	// init writer 
	tb_object_xml_writer_t writer = {0};
	writer.stream 	= stream;
	writer.deflate 	= deflate;

	// func
	tb_object_xml_writer_func_t func = tb_object_xml_writer_func(object->type);
	tb_assert_and_check_return_val(func, -1);

	// the begin offset
	tb_hize_t bof = tb_stream_offset(stream);

	// writ xml header
	tb_gstream_printf(stream, "<?xml version=\"2.0\" encoding=\"utf-8\"?>");
	tb_object_writer_newline(stream, deflate);

	// writ
	tb_bool_t ok = func(&writer, object, 0);
	tb_check_return_val(ok, -1);

	// sync
	ok = tb_gstream_sync(stream, tb_true);
	tb_check_return_val(ok, -1);

	// the end offset
	tb_hize_t eof = tb_stream_offset(stream);

	// ok?
	return eof >= bof? (tb_long_t)(eof - bof) : -1;
}
static tb_size_t tb_object_xml_writer_probe(tb_gstream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, 0);

	// need it
	tb_byte_t* p = tb_null;
	if (!tb_gstream_need(stream, &p, 5)) return 0;
	tb_assert_and_check_return_val(p, 0);

	// ok?
	return !tb_strnicmp(p, "<?xml", 5)? 50 : 0;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_writer_t* tb_object_xml_writer()
{
	// the writer
	static tb_object_writer_t s_writer = {0};
  
	// init writer
	s_writer.writ = tb_object_xml_writer_done;
 
	// init hooker
	s_writer.hooker = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(s_writer.hooker, tb_null);

	// hook writer 
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NULL, tb_object_xml_writer_func_null);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATE, tb_object_xml_writer_func_date);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATA, tb_object_xml_writer_func_data);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_ARRAY, tb_object_xml_writer_func_array);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_STRING, tb_object_xml_writer_func_string);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NUMBER, tb_object_xml_writer_func_number);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_BOOLEAN, tb_object_xml_writer_func_boolean);
	tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DICTIONARY, tb_object_xml_writer_func_dictionary);

	// ok
	return &s_writer;
}
tb_bool_t tb_object_xml_writer_hook(tb_size_t type, tb_object_xml_writer_func_t func)
{
	// check
	tb_assert_and_check_return_val(func, tb_false);
 
	// the writer
	tb_object_writer_t* writer = tb_object_get_writer(TB_OBJECT_FORMAT_XML);
	tb_assert_and_check_return_val(writer && writer->hooker, tb_false);

	// hook it
	tb_hash_set(writer->hooker, (tb_pointer_t)type, func);

	// ok
	return tb_true;
}
tb_object_xml_writer_func_t tb_object_xml_writer_func(tb_size_t type)
{
	// the writer
	tb_object_writer_t* writer = tb_object_get_writer(TB_OBJECT_FORMAT_XML);
	tb_assert_and_check_return_val(writer && writer->hooker, tb_null);

	// the func
	return tb_hash_get(writer->hooker, (tb_pointer_t)type);
}

