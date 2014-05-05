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
 * @file		object.c
 * @ingroup 	object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"object"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_object_context_init()
{
	// set reader
	if (!tb_object_reader_set(TB_OBJECT_FORMAT_BIN, tb_object_bin_reader())) return tb_false;
	if (!tb_object_reader_set(TB_OBJECT_FORMAT_JSON, tb_object_json_reader())) return tb_false;
	if (!tb_object_reader_set(TB_OBJECT_FORMAT_BPLIST, tb_object_bplist_reader())) return tb_false;
 
	// set writer
	if (!tb_object_writer_set(TB_OBJECT_FORMAT_BIN, tb_object_bin_writer())) return tb_false;
	if (!tb_object_writer_set(TB_OBJECT_FORMAT_JSON, tb_object_json_writer())) return tb_false;
	if (!tb_object_writer_set(TB_OBJECT_FORMAT_BPLIST, tb_object_bplist_writer())) return tb_false;

	// for xml
#ifdef TB_CONFIG_MODULE_HAVE_XML
	if (!tb_object_reader_set(TB_OBJECT_FORMAT_XML, tb_object_xml_reader())) return tb_false;
	if (!tb_object_writer_set(TB_OBJECT_FORMAT_XML, tb_object_xml_writer())) return tb_false;
	if (!tb_object_reader_set(TB_OBJECT_FORMAT_XPLIST, tb_object_xplist_reader())) return tb_false;
	if (!tb_object_writer_set(TB_OBJECT_FORMAT_XPLIST, tb_object_xplist_writer())) return tb_false;
#endif

	// ok
	return tb_true;
}
tb_void_t tb_object_context_exit()
{
	// exit reader
	tb_object_reader_del(TB_OBJECT_FORMAT_BIN);
	tb_object_reader_del(TB_OBJECT_FORMAT_JSON);
	tb_object_reader_del(TB_OBJECT_FORMAT_BPLIST);

	// exit writer
	tb_object_writer_del(TB_OBJECT_FORMAT_BIN);
	tb_object_writer_del(TB_OBJECT_FORMAT_JSON);
	tb_object_writer_del(TB_OBJECT_FORMAT_BPLIST);

	// for xml
#ifdef TB_CONFIG_MODULE_HAVE_XML
	tb_object_reader_del(TB_OBJECT_FORMAT_XML);
	tb_object_writer_del(TB_OBJECT_FORMAT_XML);
	tb_object_reader_del(TB_OBJECT_FORMAT_XPLIST);
	tb_object_writer_del(TB_OBJECT_FORMAT_XPLIST);
#endif
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
tb_void_t tb_object_setp(tb_object_t* object, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return(object);
	object->priv = priv;
}
tb_pointer_t tb_object_getp(tb_object_t* object)
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

		// writ object to data
		tb_long_t size = tb_object_writ_to_data(object, data, maxn, format);
	
		// ok? make the data object
		if (size >= 0) odata = tb_data_init_from_data(data, size);
		// failed? grow it
		else maxn <<= 1;

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
				tb_trace_d("key: %s", key);
			
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
				tb_trace_d("index: %s", index);

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
			tb_char_t const* 	p = tb_strstr((tb_char_t const*)data, "?>");
			tb_char_t const* 	e = (tb_char_t const*)data + size;
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
tb_object_t* tb_object_read(tb_basic_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, tb_null);

	// done reader
	return tb_object_reader_done(stream);
}
tb_object_t* tb_object_read_from_url(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// init
	tb_object_t* object = tb_null;

	// make stream
	tb_basic_stream_t* stream = tb_basic_stream_init_from_url(url);
	tb_assert_and_check_return_val(stream, tb_null);

	// read object
	if (tb_basic_stream_open(stream)) object = tb_object_read(stream);

	// exit stream
	tb_basic_stream_exit(stream);

	// ok?
	return object;
}
tb_object_t* tb_object_read_from_data(tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(data && size, tb_null);

	// init
	tb_object_t* object = tb_null;

	// make stream
	tb_basic_stream_t* stream = tb_basic_stream_init_from_data(data, size);
	tb_assert_and_check_return_val(stream, tb_null);

	// read object
	if (tb_basic_stream_open(stream)) object = tb_object_read(stream);

	// exit stream
	tb_basic_stream_exit(stream);

	// ok?
	return object;
}
tb_long_t tb_object_writ(tb_object_t* object, tb_basic_stream_t* stream, tb_size_t format)
{
	// check
	tb_assert_and_check_return_val(object && stream, -1);

	// writ it
	return tb_object_writer_done(object, stream, format);
}
tb_long_t tb_object_writ_to_url(tb_object_t* object, tb_char_t const* url, tb_size_t format)
{
	// check
	tb_assert_and_check_return_val(object && url, -1);

	// make stream
	tb_long_t 		writ = -1;
	tb_basic_stream_t* 	stream = tb_basic_stream_init_from_url(url);
	if (stream)
	{
		// ctrl stream
		if (tb_stream_type(stream) == TB_STREAM_TYPE_FILE)
			tb_stream_ctrl(stream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_RW | TB_FILE_MODE_BINARY | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);

		// open and writ stream
		if (tb_basic_stream_open(stream)) writ = tb_object_writ(object, stream, format);

		// exit stream
		tb_basic_stream_exit(stream);
	}

	// ok?
	return writ;
}
tb_long_t tb_object_writ_to_data(tb_object_t* object, tb_byte_t* data, tb_size_t size, tb_size_t format)
{
	// check
	tb_assert_and_check_return_val(object && data && size, -1);

	// make stream
	tb_long_t 		writ = -1;
	tb_basic_stream_t* 	stream = tb_basic_stream_init_from_data(data, size);
	if (stream)
	{
		// open and writ stream
		if (tb_basic_stream_open(stream)) writ = tb_object_writ(object, stream, format);

		// exit stream
		tb_basic_stream_exit(stream);
	}

	// ok?
	return writ;
}

