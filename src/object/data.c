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
 * @file		data.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"object"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the data type
typedef struct __tb_data_t
{
	// the object base
	tb_object_t 		base;

	// the data buffer
	tb_pbuffer_t 		buff;

}tb_data_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_data_t* tb_data_cast(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_DATA, tb_null);

	// cast
	return (tb_data_t*)object;
}
static tb_object_t* tb_data_copy(tb_object_t* object)
{
	return tb_data_init_from_data(tb_data_getp(object), tb_data_size(object));
}
static tb_void_t tb_data_exit(tb_object_t* object)
{
	tb_data_t* data = tb_data_cast(object);
	if (data) 
	{
		tb_pbuffer_exit(&data->buff);
		tb_opool_del((tb_object_t*)data);
	}
}
static tb_void_t tb_data_cler(tb_object_t* object)
{
	tb_data_t* data = tb_data_cast(object);
	if (data) tb_pbuffer_clear(&data->buff);
}
static tb_data_t* tb_data_init_base()
{
	// make
	tb_data_t* data = (tb_data_t*)tb_opool_get(sizeof(tb_data_t), TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_DATA);
	tb_assert_and_check_return_val(data, tb_null);

	// init base
	data->base.copy = tb_data_copy;
	data->base.cler = tb_data_cler;
	data->base.exit = tb_data_exit;

	// ok
	return data;
}
static tb_object_t* tb_data_read_xml(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_data_init_from_data(tb_null, 0);

	// walk
	tb_char_t* 		base64 	= tb_null;
	tb_object_t* 	data 	= tb_null;
	while (event = tb_xml_reader_next(reader->reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "data"))
				{
					// empty?
					if (!data) data = tb_data_init_from_data(tb_null, 0);
					goto end;
				}
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader->reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_d("data: %s", text);

				// base64
				base64 = tb_strdup(text);
				tb_char_t* p = base64;
				tb_char_t* q = p;
				for (; *p; p++) if (!tb_isspace(*p)) *q++ = *p;
				*q = '\0';

				// decode base64 data
				tb_char_t const* 	ib = base64;
				tb_size_t 			in = tb_strlen(base64); 
				if (in)
				{
					tb_size_t 			on = in;
					tb_byte_t* 			ob = tb_malloc0(on);
					tb_assert_and_check_goto(ob && on, end);
					on = tb_base64_decode(ib, in, ob, on);
					tb_trace_d("base64: %u => %u", in, on);

					// init data
					data = tb_data_init_from_data(ob, on); tb_free(ob);
				}
				else data = tb_data_init_from_data(tb_null, 0);
				tb_assert_and_check_goto(data, end);
			}
			break;
		default:
			break;
		}
	}

end:

	// free
	if (base64) tb_free(base64);

	// ok?
	return data;
}
static tb_bool_t tb_data_writ_xml(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// no empty?
	if (tb_data_size(object))
	{
		// writ beg
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<data>\n");

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
				if (n) tb_gstream_printf(writer->stream, "\n");
				tb_object_writ_tab(writer->stream, writer->deflate, level);
			}
			tb_gstream_printf(writer->stream, "%c", *p);
		}
		tb_gstream_printf(writer->stream, "\n");

		// free it
		tb_free(ob);
					
		// writ end
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "</data>\n");
	}
	else 
	{
		// writ
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<data/>\n");
	}

	// ok
	return tb_true;
}
static tb_object_t* tb_data_read_bin(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

	// empty?
	if (!size) return tb_data_init_from_data(tb_null, 0);

	// make data
	tb_char_t* data = tb_malloc0((tb_size_t)size);
	tb_assert_and_check_return_val(data, tb_null);

	// read data
	if (!tb_gstream_bread(reader->stream, data, (tb_size_t)size)) 
	{
		tb_free(data);
		return tb_null;
	}

	// decode data
	{
		tb_byte_t* 	pb = data;
		tb_byte_t* 	pe = data + size;
		tb_byte_t 	xb = (tb_byte_t)(((size >> 8) & 0xff) | (size & 0xff));
		for (; pb < pe; pb++, xb++) *pb ^= xb;
	}

	// make the data object
	tb_object_t* object = tb_data_init_from_data(data, (tb_size_t)size); 

	// exit data
	tb_free(data);

	// ok?
	return object;
}
static tb_bool_t tb_data_writ_bin(tb_object_bin_writer_t* writer, tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

	// the data & size
	tb_byte_t const* 	data = tb_data_getp(object);
	tb_size_t 			size = tb_data_size(object);

	// writ type & size
	if (!tb_object_writ_bin_type_size(writer->stream, object->type, size)) return tb_false;

	// empty?
	tb_check_return_val(size, tb_true);

	// check 
	tb_assert_and_check_return_val(data, tb_false);

	// make the encoder data
	if (!writer->data)
	{
		writer->maxn = tb_max(size, 8192);
		writer->data = tb_malloc0(writer->maxn);
	}
	else if (writer->maxn < size)
	{
		writer->maxn = size;
		writer->data = tb_ralloc(writer->data, writer->maxn);
	}
	tb_assert_and_check_return_val(writer->data && size <= writer->maxn, tb_false);

	// copy data to encoder
	tb_memcpy(writer->data, data, size);

	// encode data
	tb_byte_t const* 	pb = data;
	tb_byte_t const* 	pe = data + size;
	tb_byte_t* 			qb = writer->data;
	tb_byte_t* 			qe = writer->data + writer->maxn;
	tb_byte_t 			xb = (tb_byte_t)(((size >> 8) & 0xff) | (size & 0xff));
	for (; pb < pe && qb < qe; pb++, qb++, xb++) *qb = *pb ^ xb;

	// writ it
	return tb_gstream_bwrit(writer->stream, writer->data, size);
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_data_init_reader()
{
	if (!tb_object_set_xml_reader("data", tb_data_read_xml)) return tb_false;
	if (!tb_object_set_bin_reader(TB_OBJECT_TYPE_DATA, tb_data_read_bin)) return tb_false;
	return tb_true;
}
tb_bool_t tb_data_init_writer()
{
	if (!tb_object_set_xml_writer(TB_OBJECT_TYPE_DATA, tb_data_writ_xml)) return tb_false;
	if (!tb_object_set_bin_writer(TB_OBJECT_TYPE_DATA, tb_data_writ_bin)) return tb_false;
	return tb_true;
}
tb_object_t* tb_data_init_from_url(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// init stream
	tb_gstream_t* stream = tb_gstream_init_from_url(url);
	tb_assert_and_check_return_val(stream, tb_null);

	// make stream
	tb_object_t* object = tb_null;
	if (tb_gstream_open(stream))
	{
		// size
		tb_hong_t size = tb_stream_size(stream);
		if (size > 0)
		{
			tb_byte_t* data = tb_malloc0(size);
			if (data) 
			{
				if (tb_gstream_bread(stream, data, size))
					object = tb_data_init_from_data(data, size);
				tb_free(data);
			}
		}
		else object = tb_data_init_from_data(tb_null, 0);

		// check, TODO: read stream if no size
		tb_assert(size >= 0);

		// exit stream
		tb_gstream_exit(stream);
	}

	// ok?
	return object;
}
tb_object_t* tb_data_init_from_data(tb_pointer_t addr, tb_size_t size)
{
	// make
	tb_data_t* data = tb_data_init_base();
	tb_assert_and_check_return_val(data, tb_null);

	// init buff
	if (!tb_pbuffer_init(&data->buff)) goto fail;

	// copy data
	if (addr && size) tb_pbuffer_memncpy(&data->buff, addr, size);

	// ok
	return (tb_object_t*)data;

	// no
fail:
	tb_data_exit((tb_object_t*)data);
	return tb_null;
}
tb_object_t* tb_data_init_from_pbuf(tb_pbuffer_t* pbuf)
{	
	// make
	tb_data_t* data = tb_data_init_base();
	tb_assert_and_check_return_val(data, tb_null);

	// init buff
	if (!tb_pbuffer_init(&data->buff)) goto fail;

	// copy data
	if (pbuf) tb_pbuffer_memcpy(&data->buff, pbuf);

	// ok
	return (tb_object_t*)data;

	// no
fail:
	tb_data_exit((tb_object_t*)data);
	return tb_null;
}
tb_pointer_t tb_data_getp(tb_object_t* object)
{
	// check
	tb_data_t* data = tb_data_cast(object);
	tb_assert_and_check_return_val(data, tb_null);

	// data
	return tb_pbuffer_data(&data->buff);
}
tb_bool_t tb_data_setp(tb_object_t* object, tb_pointer_t addr, tb_size_t size)
{
	// check
	tb_data_t* data = tb_data_cast(object);
	tb_assert_and_check_return_val(data && addr, tb_false);

	// data
	tb_pbuffer_memncpy(&data->buff, addr, size);

	// ok
	return tb_true;
}
tb_size_t tb_data_size(tb_object_t* object)
{
	// check
	tb_data_t* data = tb_data_cast(object);
	tb_assert_and_check_return_val(data, 0);

	// data
	return tb_pbuffer_size(&data->buff);
}
tb_pbuffer_t* tb_data_buff(tb_object_t* object)
{
	// check
	tb_data_t* data = tb_data_cast(object);
	tb_assert_and_check_return_val(data, tb_null);

	// buff
	return &data->buff;
}
tb_bool_t tb_data_writ_to_url(tb_object_t* object, tb_char_t const* url)
{
	// check
	tb_data_t* data = tb_data_cast(object);
	tb_assert_and_check_return_val(data && tb_data_getp((tb_object_t*)data) && url, tb_false);

	// make stream
	tb_gstream_t* stream = tb_gstream_init_from_url(url);
	tb_assert_and_check_return_val(stream, tb_false);

	// ctrl
	if (tb_stream_type(stream) == TB_STREAM_TYPE_FILE)
		tb_stream_ctrl(stream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
	
	// open stream
	tb_bool_t ok = tb_false;
	if (tb_gstream_open(stream))
	{
		// writ stream
		if (tb_gstream_bwrit(stream, tb_data_getp((tb_object_t*)data), tb_data_size((tb_object_t*)data))) ok = tb_true;
	}

	// exit stream
	tb_gstream_exit(stream);

	// ok?
	return ok;
}
