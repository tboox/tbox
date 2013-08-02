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
 * @file		string.c
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
#include "../string/string.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the scache string size
#define TB_STRING_SCACHE_SIZE 		(64)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the string type
typedef struct __tb_string_t
{
	// the object base
	tb_object_t 		base;

	// the pstring
	tb_pstring_t 		pstr;

	// the cache data
	tb_char_t const* 	cdata;

	// the cache size
	tb_size_t 			csize;

}tb_string_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_string_t* tb_string_cast(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_STRING, tb_null);

	// cast
	return (tb_string_t*)object;
}
static tb_object_t* tb_string_copy(tb_object_t* object)
{
	return tb_string_init_from_cstr(tb_string_cstr(object));
}
static tb_void_t tb_string_exit(tb_object_t* object)
{
	tb_string_t* string = tb_string_cast(object);
	if (string) 
	{
		if (string->cdata) tb_scache_del(string->cdata);
		tb_pstring_exit(&string->pstr);
		tb_opool_del(string);
	}
}
static tb_void_t tb_string_cler(tb_object_t* object)
{
	tb_string_t* string = tb_string_cast(object);
	if (string) 
	{
		if (string->cdata) tb_scache_del(string->cdata);
		string->cdata = tb_null;
		string->csize = 0;
		tb_pstring_clear(&string->pstr);
	}
}
static tb_string_t* tb_string_init_base()
{
	// make
	tb_string_t* string = tb_opool_get(sizeof(tb_string_t), TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_STRING);
	tb_assert_and_check_return_val(string, tb_null);

	// init base
	string->base.copy = tb_string_copy;
	string->base.cler = tb_string_cler;
	string->base.exit = tb_string_exit;

	// ok
	return string;
}
static tb_object_t* tb_string_read_xml(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_string_init_from_cstr(tb_null);

	// walk
	tb_object_t* string = tb_null;
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
				if (!tb_stricmp(name, "string"))
				{
					// empty?
					if (!string) string = tb_string_init_from_cstr(tb_null);
					goto end;
				}
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader->reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_impl("string: %s", text);
				
				// string
				string = tb_string_init_from_cstr(text);
				tb_assert_and_check_goto(string, end);
			}
			break;
		default:
			break;
		}
	}

end:

	// ok?
	return string;
}
static tb_bool_t tb_string_writ_xml(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	tb_object_writ_tab(writer->stream, writer->deflate, level);
	if (tb_string_size(object))
		tb_gstream_printf(writer->stream, "<string>%s</string>", tb_string_cstr(object));
	else tb_gstream_printf(writer->stream, "<string/>");
	tb_object_writ_newline(writer->stream, writer->deflate);

	// ok
	return tb_true;
}
static tb_object_t* tb_string_read_bin(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

	// empty?
	if (!size) return tb_string_init_from_cstr(tb_null);

	// make data
	tb_char_t* data = tb_malloc0((tb_size_t)size + 1);
	tb_assert_and_check_return_val(data, tb_null);

	// read data
	if (!tb_gstream_bread(reader->stream, data, (tb_size_t)size)) 
	{
		tb_free(data);
		return tb_null;
	}

	// decode string
	{
		tb_byte_t* 	pb = data;
		tb_byte_t* 	pe = data + size;
		tb_byte_t 	xb = (tb_byte_t)(((size >> 8) & 0xff) | (size & 0xff));
		for (; pb < pe; pb++, xb++) *pb ^= xb;
	}

	// make string
	tb_object_t* string = tb_string_init_from_cstr(data); 

	// exit data
	tb_free(data);

	// ok?
	return string;
}
static tb_bool_t tb_string_writ_bin(tb_object_bin_writer_t* writer, tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

	// the data & size
	tb_char_t const* 	data = tb_string_cstr(object);
	tb_size_t 			size = tb_string_size(object);

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
	tb_byte_t* 	pb = data;
	tb_byte_t* 	pe = data + size;
	tb_byte_t* 	qb = writer->data;
	tb_byte_t* 	qe = writer->data + writer->maxn;
	tb_byte_t 	xb = (tb_byte_t)(((size >> 8) & 0xff) | (size & 0xff));
	for (; pb < pe && qb < qe && *pb; pb++, qb++, xb++) *qb = *pb ^ xb;

	// writ it
	return tb_gstream_bwrit(writer->stream, writer->data, size);
}
static tb_object_t* tb_string_read_jsn(tb_object_jsn_reader_t* reader, tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && (type == '\"' || type == '\''), tb_null);

	// init data
	tb_pstring_t data;
	if (!tb_pstring_init(&data)) return tb_null;

	// walk
	tb_char_t ch;
	while (tb_gstream_left(reader->stream)) 
	{
		// read one character
		ch = tb_gstream_bread_s8(reader->stream);

		// end?
		if (ch == '\"' || ch == '\'') break;
		// append character
		else tb_pstring_chrcat(&data, ch);
	}

	// init string
	tb_object_t* string = tb_string_init_from_cstr(tb_pstring_cstr(&data));

	// trace
	tb_trace_impl("string: %s", tb_pstring_cstr(&data));

	// exit data
	tb_pstring_exit(&data);

	// ok?
	return string;
}
static tb_bool_t tb_string_writ_jsn(tb_object_jsn_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	if (tb_string_size(object))
		tb_gstream_printf(writer->stream, "\"%s\"", tb_string_cstr(object));
	else tb_gstream_printf(writer->stream, "\"\"");
	tb_object_writ_newline(writer->stream, writer->deflate);

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_string_init_reader()
{
	if (!tb_object_set_xml_reader("string", tb_string_read_xml)) return tb_false;
	if (!tb_object_set_bin_reader(TB_OBJECT_TYPE_STRING, tb_string_read_bin)) return tb_false;
	if (!tb_object_set_jsn_reader('\"', tb_string_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('\'', tb_string_read_jsn)) return tb_false;
	return tb_true;
}
tb_bool_t tb_string_init_writer()
{
	if (!tb_object_set_xml_writer(TB_OBJECT_TYPE_STRING, tb_string_writ_xml)) return tb_false;
	if (!tb_object_set_bin_writer(TB_OBJECT_TYPE_STRING, tb_string_writ_bin)) return tb_false;
	if (!tb_object_set_jsn_writer(TB_OBJECT_TYPE_STRING, tb_string_writ_jsn)) return tb_false;
	return tb_true;
}
tb_object_t* tb_string_init_from_cstr(tb_char_t const* cstr)
{
	// make
	tb_string_t* string = tb_string_init_base();
	tb_assert_and_check_return_val(string, tb_null);

	// init pstr
	if (!tb_pstring_init(&string->pstr)) goto fail;

	// copy string
	if (cstr) 
	{
		tb_size_t size = tb_strlen(cstr);
		if (size)
		{
			if (size < TB_STRING_SCACHE_SIZE) 
			{
				// put string to scache
				string->cdata = tb_scache_put(cstr);
				tb_assert_and_check_goto(string->cdata, fail);

				// the string size
				string->csize = size;
			}
			else tb_pstring_cstrncpy(&string->pstr, cstr, size);
		}
	}

	// ok
	return string;

	// no
fail:
	tb_string_exit(string);
	return tb_null;
}
tb_object_t* tb_string_init_from_pstr(tb_pstring_t* pstr)
{
	// make
	tb_string_t* string = tb_string_init_base();
	tb_assert_and_check_return_val(string, tb_null);

	// init pstr
	if (!tb_pstring_init(&string->pstr)) goto fail;

	// copy string
	if (pstr) 
	{
		tb_size_t size = tb_pstring_size(&string->pstr);
		if (size < TB_STRING_SCACHE_SIZE) 
		{
			// put string to scache
			string->cdata = tb_scache_put(tb_pstring_cstr(pstr));
			tb_assert_and_check_goto(string->cdata, fail);

			// the string size
			string->csize = size;
		}
		else tb_pstring_strcpy(&string->pstr, pstr);
	}

	// ok
	return string;

	// no
fail:
	tb_string_exit(string);
	return tb_null;
}
tb_char_t const* tb_string_cstr(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, tb_null);

	// cstr
	return string->cdata? string->cdata : tb_pstring_cstr(&string->pstr);
}
tb_size_t tb_string_cstr_set(tb_object_t* object, tb_char_t const* cstr)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string && cstr, 0);

	tb_size_t size = 0;
	if (cstr) 
	{
		size = tb_strlen(cstr);
		if (size)
		{
			if (size < TB_STRING_SCACHE_SIZE) 
			{
				// put string to scache
				tb_char_t const* cdata = tb_scache_put(cstr);
				if (cdata)
				{
					// save string
					if (string->cdata) tb_scache_del(string->cdata);
					string->cdata = cdata;
					string->csize = size;
				}
			}
			else 
			{
				// copy string
				tb_pstring_cstrncpy(&string->pstr, cstr, size);
				size = tb_pstring_size(&string->pstr);

				// remove string from scache
				if (string->cdata) tb_scache_del(string->cdata);
				string->cdata = tb_null;
				string->csize = 0;
			}
		}
		else
		{
			// clear string
			tb_pstring_clear(&string->pstr);

			// remove string from scache
			if (string->cdata) tb_scache_del(string->cdata);
			string->cdata = tb_null;
			string->csize = 0;
		}
	}

	// ok?
	return size;
}
tb_size_t tb_string_size(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, 0);

	// size
	return string->cdata? string->csize : tb_pstring_size(&string->pstr);
}

