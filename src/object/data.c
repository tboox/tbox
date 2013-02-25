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
 * @file		data.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 		"object"

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
	return tb_data_init_from_data(tb_data_addr(object), tb_data_size(object));
}
static tb_void_t tb_data_exit(tb_object_t* object)
{
	tb_data_t* data = tb_data_cast(object);
	if (data) 
	{
		tb_pbuffer_exit(&data->buff);
		tb_free(data);
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
	tb_data_t* data = tb_malloc0(sizeof(tb_data_t));
	tb_assert_and_check_return_val(data, tb_null);

	// init object
	if (!tb_object_init(data, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_DATA)) goto fail;

	// init base
	data->base.copy = tb_data_copy;
	data->base.cler = tb_data_cler;
	data->base.exit = tb_data_exit;

	// ok
	return data;

	// no
fail:
	if (data) tb_free(data);
	return tb_null;
}
static tb_object_t* tb_data_read_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_data_init_from_data(tb_null, 0);

	// walk
	tb_char_t* 		base64 	= tb_null;
	tb_object_t* 	data 	= tb_null;
	while (event = tb_xml_reader_next(reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
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
				tb_char_t const* text = tb_xml_reader_text(reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_impl("data: %s", text);

				// base64
				base64 = tb_strdup(text);
				tb_char_t* p = base64;
				tb_char_t* q = p;
				for (; *p; p++) if (!tb_isspace(*p)) *q++ = *p;
				*q = '\0';

				// decode base64 data
				tb_char_t const* 	ib = base64;
				tb_size_t 			in = tb_strlen(base64); 
				tb_size_t 			on = in;
				tb_byte_t* 			ob = tb_malloc0(on);
				tb_assert_and_check_goto(ob && on, end);
				on = tb_base64_decode(ib, in, ob, on);
				tb_trace_impl("base64: %u => %u", in, on);

				// init data
				data = tb_data_init_from_data(ob, on); tb_free(ob);
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
static tb_bool_t tb_data_writ_xml(tb_object_t* object, tb_gstream_t* gst, tb_bool_t deflate, tb_size_t level)
{
	// no empty?
	if (tb_data_size(object))
	{
		// writ beg
		tb_object_writ_tab(gst, deflate, level);
		tb_gstream_printf(gst, "<data>\n");

		// decode base64 data
		tb_byte_t const* 	ib = tb_data_addr(object);
		tb_size_t 			in = tb_data_size(object); 
		tb_size_t 			on = in << 1;
		tb_char_t* 			ob = tb_malloc0(on);
		tb_assert_and_check_return_val(ob && on, tb_false);
		on = tb_base64_encode(ib, in, ob, on);
		tb_trace_impl("base64: %u => %u", in, on);

		// writ data
		tb_char_t const* 	p = ob;
		tb_char_t const* 	e = ob + on;
		tb_size_t 			n = 0;
		for (; p < e && *p; p++, n++)
		{
			if (!(n & 63))
			{
				if (n) tb_gstream_printf(gst, "\n");
				tb_object_writ_tab(gst, deflate, level);
			}
			tb_gstream_printf(gst, "%c", *p);
		}
		tb_gstream_printf(gst, "\n");

		// free it
		tb_free(ob);
					
		// writ end
		tb_object_writ_tab(gst, deflate, level);
		tb_gstream_printf(gst, "</data>\n");
	}
	else 
	{
		// writ
		tb_object_writ_tab(gst, deflate, level);
		tb_gstream_printf(gst, "<data/>\n");
	}

	// ok
	return tb_true;
}
static tb_object_t* tb_data_read_bin(tb_gstream_t* gst, tb_size_t type, tb_size_t size)
{
	tb_trace_noimpl();
	return tb_null;
}
static tb_bool_t tb_data_writ_bin(tb_object_t* object, tb_gstream_t* gst)
{
	// writ type & size
	if (!tb_object_writ_bin_type_size(gst, object->type, tb_data_size(object))) return tb_false;

	// ok
	return tb_true;
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
	tb_gstream_t* gst = tb_gstream_init_from_url(url);
	tb_assert_and_check_return_val(gst, tb_null);

	// make stream
	tb_object_t* object = tb_null;
	if (tb_gstream_bopen(gst))
	{
		// size
		tb_size_t size = (tb_size_t)tb_gstream_size(gst);
		if (size)
		{
			tb_byte_t* data = tb_malloc0(size);
			if (data) 
			{
				if (tb_gstream_bread(gst, data, size))
					object = tb_data_init_from_data(data, size);
				tb_free(data);
			}
		}
		else object = tb_data_init_from_data(tb_null, 0);

		// exit stream
		tb_gstream_exit(gst);
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
	return data;

	// no
fail:
	tb_data_exit(data);
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
	return data;

	// no
fail:
	tb_data_exit(data);
	return tb_null;
}
tb_pointer_t tb_data_addr(tb_object_t* object)
{
	// check
	tb_data_t* data = tb_data_cast(object);
	tb_assert_and_check_return_val(data, tb_null);

	// data
	return tb_pbuffer_data(&data->buff);
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
	tb_assert_and_check_return_val(data && tb_data_addr(data) && url, tb_false);

	// make stream
	tb_gstream_t* gst = tb_gstream_init_from_url(url);
	tb_assert_and_check_return_val(gst, tb_false);

	// ctrl
	if (tb_gstream_type(gst) == TB_GSTREAM_TYPE_FILE)
		tb_gstream_ctrl(gst, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);
	
	// open stream
	tb_bool_t ok = tb_false;
	if (tb_gstream_bopen(gst))
	{
		// writ stream
		if (tb_gstream_bwrit(gst, tb_data_addr(data), tb_data_size(data))) ok = tb_true;
	}

	// exit stream
	tb_gstream_exit(gst);

	// ok?
	return ok;
}
