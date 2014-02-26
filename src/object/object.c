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
 * @file		object.c
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
 * globals
 */

// the object xml reader
static tb_handle_t 				g_object_xml_reader = tb_null;

// the object xml writer
static tb_handle_t 				g_object_xml_writer = tb_null;

// the object bin reader
static tb_handle_t 				g_object_bin_reader = tb_null;

// the object bin writer
static tb_handle_t 				g_object_bin_writer = tb_null;

// the object jsn reader
static tb_handle_t 				g_object_jsn_reader = tb_null;

// the object jsn writer
static tb_handle_t 				g_object_jsn_writer = tb_null;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_object_t* tb_object_read_xml(tb_gstream_t* stream)
{
	// init reader 
	tb_object_xml_reader_t reader = {0};
	reader.reader = tb_xml_reader_init(stream);
	tb_assert_and_check_return_val(reader.reader, tb_null);

	// init object
	tb_object_t* object = tb_null;

	// walk
	tb_size_t event = TB_XML_READER_EVENT_NONE;
	while (!object && (event = tb_xml_reader_next(reader.reader)))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader.reader);
				tb_assert_and_check_goto(name, end);

				// func
				tb_object_xml_reader_func_t func = tb_object_get_xml_reader(name);
				tb_assert_and_check_goto(func, end);

				// read
				object = func(&reader, event);
			}
			break;
		default:
			break;
		}
	}

end:

	// exit reader
	tb_xml_reader_exit(reader.reader);

	// ok?
	return object;
}
static tb_bool_t tb_object_writ_xml(tb_object_t* object, tb_gstream_t* stream, tb_bool_t deflate)
{
	// init writer 
	tb_object_xml_writer_t writer = {0};
	writer.stream 	= stream;
	writer.deflate 	= deflate;

	// func
	tb_object_xml_writer_func_t func = tb_object_get_xml_writer(object->type);
	tb_assert_and_check_return_val(func, tb_false);

	// writ xml header
	tb_gstream_printf(stream, "<?xml version=\"2.0\" encoding=\"utf-8\"?>");
	tb_object_writ_newline(stream, deflate);

	// writ
	tb_bool_t ok = func(&writer, object, 0);

	// flush
	tb_gstream_bfwrit(stream, tb_null, 0);

	// ok
	return ok;
}
static tb_object_t* tb_object_read_bin(tb_gstream_t* stream)
{
	// read bin header
	tb_byte_t data[32] = {0};
	if (!tb_gstream_bread(stream, data, 5)) return tb_null;

	// check 
	if (tb_strnicmp(data, "tbo00", 5)) return tb_null;

	// init
	tb_object_t* 			object = tb_null;
	tb_object_bin_reader_t 	reader = {0};

	// init reader
	reader.stream 			= stream;
	reader.list 			= tb_vector_init(256, tb_item_func_obj());
	tb_assert_and_check_return_val(reader.list, tb_null);

	// the type & size
	tb_size_t 				type = 0;
	tb_uint64_t 			size = 0;
	tb_object_read_bin_type_size(stream, &type, &size);

	// trace
	tb_trace_impl("root: type: %lu, size: %llu", type, size);

	// the func
	tb_object_bin_reader_func_t func = tb_object_get_bin_reader(type);
	tb_assert_and_check_goto(func, end);

	// read it
	object = func(&reader, type, size);

end:

	// exit the list
	if (reader.list) tb_vector_exit(reader.list);

	// ok?
	return object;
}
static tb_bool_t tb_object_writ_bin(tb_object_t* object, tb_gstream_t* stream, tb_bool_t deflate)
{
	// check
	tb_assert_and_check_return_val(object && stream, tb_false);

	// the func
	tb_object_bin_writer_func_t func = tb_object_get_bin_writer(object->type);
	tb_assert_and_check_return_val(func, tb_false);

	// writ bin header
	if (!tb_gstream_bwrit(stream, "tbo00", 5)) return tb_false;

	// init
	tb_bool_t 				ok = tb_false;
	tb_object_bin_writer_t 	writer = {0};

	// init writer
	writer.stream 			= stream;
	writer.ohash 			= tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_ptr(tb_null, tb_null), tb_item_func_uint32());
	writer.shash 			= tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_str(tb_true, tb_null), tb_item_func_uint32());
	tb_assert_and_check_return_val(writer.shash && writer.ohash, tb_false);

	// writ
	if (!func(&writer, object)) goto end;

	// flush
	if (!tb_gstream_bfwrit(stream, tb_null, 0)) goto end;

	// ok
	ok = tb_true;

end:

	// exit the hash
	if (writer.ohash) tb_hash_exit(writer.ohash);
	if (writer.shash) tb_hash_exit(writer.shash);

	// exit the data
	if (writer.data) tb_free(writer.data);

	// ok?
	return ok;
}
static tb_object_t* tb_object_read_jsn(tb_gstream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, tb_null);

	// init reader
	tb_object_jsn_reader_t reader = {0};
	reader.stream = stream;

	// skip spaces
	tb_char_t type;
	while (tb_stream_left(stream)) 
	{
		type = tb_gstream_bread_s8(stream);
		if (!tb_isspace(type)) break;
	}

	// empty?
	tb_check_return_val(tb_stream_left(stream), tb_null);

	// the func
	tb_object_jsn_reader_func_t func = tb_object_get_jsn_reader(type);
	tb_assert_and_check_return_val(func, tb_null);

	// read it
	return func(&reader, type);
}
static tb_bool_t tb_object_writ_jsn(tb_object_t* object, tb_gstream_t* stream, tb_bool_t deflate)
{
	// init writer 
	tb_object_jsn_writer_t writer = {0};
	writer.stream 	= stream;
	writer.deflate 	= deflate;

	// func
	tb_object_jsn_writer_func_t func = tb_object_get_jsn_writer(object->type);
	tb_assert_and_check_return_val(func, tb_false);

	// writ
	tb_bool_t ok = func(&writer, object, 0);

	// flush
	tb_gstream_bfwrit(stream, tb_null, 0);

	// ok
	return ok;
}
static tb_bool_t tb_object_init_reader()
{
	// init null
	if (!tb_null_init_reader()) return tb_false;

	// init data
	if (!tb_data_init_reader()) return tb_false;

	// init date
	if (!tb_date_init_reader()) return tb_false;

	// init array
	if (!tb_array_init_reader()) return tb_false;

	// init number
	if (!tb_number_init_reader()) return tb_false;

	// init string
	if (!tb_string_init_reader()) return tb_false;

	// init boolean
	if (!tb_boolean_init_reader()) return tb_false;

	// init dictionary
	if (!tb_dictionary_init_reader()) return tb_false;

	// ok
	return tb_true;
}
static tb_void_t tb_object_exit_reader()
{
	// exit the object xml reader
	if (g_object_xml_reader) tb_hash_exit(g_object_xml_reader);
	g_object_xml_reader = tb_null;
		
	// exit the object bin reader
	if (g_object_bin_reader) tb_hash_exit(g_object_bin_reader);
	g_object_bin_reader = tb_null;
	
	// exit the object jsn reader
	if (g_object_jsn_reader) tb_hash_exit(g_object_jsn_reader);
	g_object_jsn_reader = tb_null;
}
static tb_bool_t tb_object_init_writer()
{
	// init null
	if (!tb_null_init_writer()) return tb_false;

	// init data
	if (!tb_data_init_writer()) return tb_false;

	// init date
	if (!tb_date_init_writer()) return tb_false;

	// init array
	if (!tb_array_init_writer()) return tb_false;

	// init number
	if (!tb_number_init_writer()) return tb_false;

	// init string
	if (!tb_string_init_writer()) return tb_false;

	// init boolean
	if (!tb_boolean_init_writer()) return tb_false;

	// init dictionary
	if (!tb_dictionary_init_writer()) return tb_false;

	// ok
	return tb_true;
}
static tb_void_t tb_object_exit_writer()
{
	// exit the object xml writer
	if (g_object_xml_writer) tb_hash_exit(g_object_xml_writer);
	g_object_xml_writer = tb_null;

	// exit the object bin writer
	if (g_object_bin_writer) tb_hash_exit(g_object_bin_writer);
	g_object_bin_writer = tb_null;

	// exit the object jsn writer
	if (g_object_jsn_writer) tb_hash_exit(g_object_jsn_writer);
	g_object_jsn_writer = tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_object_context_init()
{
	// init opool
	if (!tb_opool_init()) return tb_false;

	// init object
	if (!tb_object_init_reader()) return tb_false;
	if (!tb_object_init_writer()) return tb_false;

	// ok
	return tb_true;
}
tb_void_t tb_object_context_exit()
{
	// exit object
	tb_object_exit_reader();
	tb_object_exit_writer();

	// exit opool
	tb_opool_exit();
}
tb_bool_t tb_object_init(tb_object_t* object, tb_size_t flag, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(object, tb_false);

	// init
	tb_memset(object, 0, sizeof(tb_object_t));
	object->flag = flag;
	object->type = type;
	object->refn = 1;

	// ok
	return tb_true;
}
tb_void_t tb_object_exit(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// exit
	tb_object_dec(object);
}
tb_void_t tb_object_cler(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// clear
	if (object->cler) object->cler(object);
}
tb_void_t tb_object_setp(tb_object_t* object, tb_cpointer_t priv)
{
	// check
	tb_assert_and_check_return(object);
	object->priv = priv;
}
tb_cpointer_t tb_object_getp(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object, tb_null);
	return object->priv;
}
tb_object_t* tb_object_copy(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->copy, tb_null);

	// copy
	return object->copy(object);
}
tb_size_t tb_object_type(tb_object_t* object)
{
	tb_assert_and_check_return_val(object, TB_OBJECT_TYPE_NONE);
	return object->type;
}
tb_object_t* tb_object_data(tb_object_t* object, tb_size_t format)
{	
	// check
	tb_assert_and_check_return_val(object, tb_null);

	// done
	tb_object_t* 	odata = tb_null;
	tb_size_t 		maxn = 4096;
	tb_byte_t* 		data = tb_null;
	do
	{
		// make data
		data = data? tb_ralloc(data, maxn) : tb_malloc(maxn);
		tb_assert_and_check_break(data);

		// init stream
		tb_gstream_t* stream = tb_gstream_init_from_data(data, maxn);
		tb_assert_and_check_break(stream);

		// open stream
		if (tb_gstream_bopen(stream))
		{
			// writ object
			if (tb_object_writ(object, stream, format))
			{
				// size
				tb_size_t size = (tb_size_t)tb_stream_offset(stream);

				// the data object
				if (size < maxn) odata = tb_data_init_from_data(data, size);
				else maxn <<= 1;
			}
			else maxn <<= 1;
		
			// exit stream
			tb_gstream_exit(stream);
		}
		else
		{
			// exit stream
			tb_gstream_exit(stream);

			// break
			break;
		}

	} while (!odata);

	// exit data
	if (data) tb_free(data);
	data = tb_null;

	// ok?
	return odata;
}
tb_object_t* tb_object_seek(tb_object_t* object, tb_char_t const* path, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(object, tb_null);

	// null?
	tb_check_return_val(path, object);

	// walk
	tb_char_t const* p = path;
	tb_char_t const* e = path + tb_strlen(path);
	while (p < e && object)
	{
		// done seek
		switch (*p)
		{
		case '.':
			{
				// check
				tb_assert_and_check_return_val(tb_object_type(object) == TB_OBJECT_TYPE_DICTIONARY, tb_null);

				// skip
				p++;

				// read the key name
				tb_char_t 	key[4096] = {0};
				tb_char_t* 	kb = key;
				tb_char_t* 	ke = key + 4095;
				for (; p < e && kb < ke && *p && (*p != '.' && *p != '[' && *p != ']'); p++, kb++) 
				{
					if (*p == '\\') p++;
					*kb = *p;
				}

				// trace
				tb_trace_impl("key: %s", key);
			
				// the value
				object = tb_dictionary_val(object, key);
			}
			break;
		case '[':
			{
				// check
				tb_assert_and_check_return_val(tb_object_type(object) == TB_OBJECT_TYPE_ARRAY, tb_null);

				// skip
				p++;

				// read the item index
				tb_char_t 	index[32] = {0};
				tb_char_t* 	ib = index;
				tb_char_t* 	ie = index + 31;
				for (; p < e && ib < ie && *p && tb_isdigit10(*p); p++, ib++) *ib = *p;

				// trace
				tb_trace_impl("index: %s", index);

				// check
				tb_size_t i = tb_atoi(index);
				tb_assert_and_check_return_val(i < tb_array_size(object), tb_null);

				// the value
				object = tb_array_item(object, i);
			}
			break;
		case ']':
		default:
			p++;
			break;
		}
	}

	// check it, if not none
	if (object && type != TB_OBJECT_TYPE_NONE) 
	{
		// is this type?
		if (tb_object_type(object) != type) return tb_null;
	}

	// ok?
	return object;
}
tb_object_t* tb_object_dump(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object, tb_null);

	// data
	tb_object_t* odata = tb_object_data(object, TB_OBJECT_FORMAT_XML);
	if (odata)
	{
		// data & size 
		tb_byte_t const* 	data = tb_data_getp(odata);
		tb_size_t 			size = tb_data_size(odata);
		if (data && size)
		{
			tb_char_t const* 	p = tb_strstr(data, "?>");
			tb_char_t const* 	e = data + size;
			tb_char_t 			b[4096 + 1];
			if (p && p + 2 < e)
			{
				p += 2;
				while (p < e && *p && tb_isspace(*p)) p++;
				while (p < e && *p)
				{
					tb_char_t* 			q = b;
					tb_char_t const* 	d = b + 4096;
					for (; p < e && q < d && *p; p++, q++) *q = *p;
					*q = '\0';
					tb_printf("%s", b);
				}
				tb_printf("\n");
			}
		}

		// exit data
		tb_object_exit(odata);
	}

	return object;
}
tb_size_t tb_object_ref(tb_object_t* object)
{
	tb_assert_and_check_return_val(object, 0);
	return object->refn;
}
tb_void_t tb_object_inc(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// refn++
	object->refn++;
}
tb_void_t tb_object_dec(tb_object_t* object)
{
	// check
	tb_assert_and_check_return(object);

	// readonly?
	tb_check_return(!(object->flag & TB_OBJECT_FLAG_READONLY));

	// check refn
	tb_assert_and_check_return(object->refn);

	// refn--
	object->refn--;

	// exit it?
	if (!object->refn && object->exit) object->exit(object);
}
tb_object_t* tb_object_read(tb_gstream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, tb_null);

	// need
	tb_byte_t* p = tb_null;
	if (!tb_gstream_bneed(stream, &p, 5)) return tb_null;
	tb_assert_and_check_return_val(p, tb_null);

	// is tbox data?
	if (!tb_strnicmp(p, "tbo", 3)) return tb_object_read_bin(stream);

	// is xml data?
	if (!tb_strnicmp(p, "<?xml", 5)) return tb_object_read_xml(stream);

	// try to read the jsn data
	return tb_object_read_jsn(stream);
}
tb_object_t* tb_object_read_from_data(tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(data && size, tb_null);

	// init
	tb_object_t* object = tb_null;

	// make stream
	tb_gstream_t* stream = tb_gstream_init_from_data(data, size);
	tb_assert_and_check_return_val(stream, tb_null);

	// read object
	if (tb_gstream_bopen(stream)) object = tb_object_read(stream);

	// exit stream
	tb_gstream_exit(stream);

	// ok?
	return object;
}
tb_object_t* tb_object_read_from_url(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// init
	tb_object_t* object = tb_null;

	// make stream
	tb_gstream_t* stream = tb_gstream_init_from_url(url);
	tb_assert_and_check_return_val(stream, tb_null);

	// read object
	if (tb_gstream_bopen(stream)) object = tb_object_read(stream);

	// exit stream
	tb_gstream_exit(stream);

	// ok?
	return object;
}
tb_bool_t tb_object_writ(tb_object_t* object, tb_gstream_t* stream, tb_size_t format)
{
	// check
	tb_assert_and_check_return_val(object && stream, tb_false);

	// writ
	switch (format & 0x00ff)
	{
	case TB_OBJECT_FORMAT_XML:
		return tb_object_writ_xml(object, stream, format & TB_OBJECT_FORMAT_DEFLATE? tb_true : tb_false);
	case TB_OBJECT_FORMAT_BIN:
		return tb_object_writ_bin(object, stream, format & TB_OBJECT_FORMAT_DEFLATE? tb_true : tb_false);
	case TB_OBJECT_FORMAT_JSN:
		return tb_object_writ_jsn(object, stream, format & TB_OBJECT_FORMAT_DEFLATE? tb_true : tb_false);
	default:
		tb_assert(0);
		break;
	}

	return tb_false;
}
tb_bool_t tb_object_writ_to_url(tb_object_t* object, tb_char_t const* url, tb_size_t format)
{
	// check
	tb_assert_and_check_return_val(object && url, tb_false);

	// init
	tb_bool_t ok = tb_false;

	// make stream
	tb_gstream_t* stream = tb_gstream_init_from_url(url);
	if (stream)
	{
		if (tb_stream_type(stream) == TB_STREAM_TYPE_FILE)
			tb_stream_ctrl(stream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
		if (tb_gstream_bopen(stream))
		{
			if (tb_object_writ(object, stream, format)) ok = tb_true;
		}
		tb_gstream_exit(stream);
	}

	// ok?
	return ok;
}
tb_bool_t tb_object_set_xml_reader(tb_char_t const* type, tb_object_xml_reader_func_t func)
{
	// check
	tb_assert_and_check_return_val(type && func, tb_false);

	// init reader
	if (!g_object_xml_reader)
		g_object_xml_reader = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_str(tb_false, tb_null), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(g_object_xml_reader, tb_false);

	// set
	tb_hash_set(g_object_xml_reader, type, func);

	// ok
	return tb_true;
}
tb_pointer_t tb_object_get_xml_reader(tb_char_t const* type)
{
	// check
	tb_assert_and_check_return_val(g_object_xml_reader, tb_null);

	// get
	return tb_hash_get(g_object_xml_reader, type);
}
tb_bool_t tb_object_set_xml_writer(tb_size_t type, tb_object_xml_writer_func_t func)
{
	// check
	tb_assert_and_check_return_val(type && func, tb_false);

	// init writer
	if (!g_object_xml_writer)
		g_object_xml_writer = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(g_object_xml_writer, tb_false);

	// set
	tb_hash_set(g_object_xml_writer, (tb_pointer_t)type, func);

	// ok
	return tb_true;
}
tb_pointer_t tb_object_get_xml_writer(tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(g_object_xml_writer, tb_null);

	// get
	return tb_hash_get(g_object_xml_writer, (tb_pointer_t)type);
}
tb_bool_t tb_object_set_bin_reader(tb_size_t type, tb_object_bin_reader_func_t func)
{
	// check
	tb_assert_and_check_return_val(type && func, tb_false);

	// init reader
	if (!g_object_bin_reader)
		g_object_bin_reader = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(g_object_bin_reader, tb_false);

	// set
	tb_hash_set(g_object_bin_reader, (tb_pointer_t)type, func);

	// ok
	return tb_true;
}
tb_pointer_t tb_object_get_bin_reader(tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(g_object_bin_reader, tb_null);

	// get
	return tb_hash_get(g_object_bin_reader, (tb_pointer_t)type);
}
tb_bool_t tb_object_set_bin_writer(tb_size_t type, tb_object_bin_writer_func_t func)
{
	// check
	tb_assert_and_check_return_val(type && func, tb_false);

	// init writer
	if (!g_object_bin_writer)
		g_object_bin_writer = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(g_object_bin_writer, tb_false);

	// set
	tb_hash_set(g_object_bin_writer, (tb_pointer_t)type, func);

	// ok
	return tb_true;
}
tb_pointer_t tb_object_get_bin_writer(tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(g_object_bin_writer, tb_null);

	// get
	return tb_hash_get(g_object_bin_writer, (tb_pointer_t)type);
}
tb_bool_t tb_object_set_jsn_reader(tb_char_t type, tb_object_jsn_reader_func_t func)
{
	// check
	tb_assert_and_check_return_val(type && func, tb_false);

	// init reader
	if (!g_object_jsn_reader)
		g_object_jsn_reader = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_uint8(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(g_object_jsn_reader, tb_false);

	// set
	tb_hash_set(g_object_jsn_reader, (tb_pointer_t)(tb_size_t)type, func);

	// ok
	return tb_true;
}
tb_pointer_t tb_object_get_jsn_reader(tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(g_object_jsn_reader, tb_null);

	// get
	return tb_hash_get(g_object_jsn_reader, (tb_pointer_t)(tb_size_t)type);
}
tb_bool_t tb_object_set_jsn_writer(tb_size_t type, tb_object_jsn_writer_func_t func)
{
	// check
	tb_assert_and_check_return_val(type && func, tb_false);

	// init writer
	if (!g_object_jsn_writer)
		g_object_jsn_writer = tb_hash_init(TB_HASH_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_return_val(g_object_jsn_writer, tb_false);

	// set
	tb_hash_set(g_object_jsn_writer, (tb_pointer_t)type, func);

	// ok
	return tb_true;
}
tb_pointer_t tb_object_get_jsn_writer(tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(g_object_jsn_writer, tb_null);

	// get
	return tb_hash_get(g_object_jsn_writer, (tb_pointer_t)type);
}
