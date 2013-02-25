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
 * types
 */

// the string type
typedef struct __tb_string_t
{
	// the object base
	tb_object_t 		base;

	// the pstring
	tb_pstring_t 		pstr;

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
		tb_pstring_exit(&string->pstr);
		tb_free(string);
	}
}
static tb_void_t tb_string_cler(tb_object_t* object)
{
	tb_string_t* string = tb_string_cast(object);
	if (string) tb_pstring_clear(&string->pstr);
}
static tb_string_t* tb_string_init_base()
{
	// make
	tb_string_t* string = tb_malloc0(sizeof(tb_string_t));
	tb_assert_and_check_return_val(string, tb_null);

	// init object
	if (!tb_object_init(string, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_STRING)) goto fail;

	// init base
	string->base.copy = tb_string_copy;
	string->base.cler = tb_string_cler;
	string->base.exit = tb_string_exit;

	// ok
	return string;

	// no
fail:
	if (string) tb_free(string);
	return tb_null;
}
static tb_object_t* tb_string_read_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_string_init_from_cstr(tb_null);

	// walk
	tb_object_t* string = tb_null;
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
				tb_char_t const* text = tb_xml_reader_text(reader);
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
static tb_bool_t tb_string_writ_xml(tb_object_t* object, tb_gstream_t* gst, tb_bool_t deflate, tb_size_t level)
{
	// writ
	tb_object_writ_tab(gst, deflate, level);
	if (tb_string_size(object))
		tb_gstream_printf(gst, "<string>%s</string>", tb_string_cstr(object));
	else tb_gstream_printf(gst, "<string/>");
	tb_object_writ_newline(gst, deflate);

	// ok
	return tb_true;
}
static tb_object_t* tb_string_read_bin(tb_gstream_t* gst, tb_size_t type, tb_size_t size)
{
	tb_trace_noimpl();
	return tb_null;
}
static tb_bool_t tb_string_writ_bin(tb_object_t* object, tb_gstream_t* gst)
{
	// writ type & size
	if (!tb_object_writ_bin_type_size(gst, object->type, tb_string_size(object))) return tb_false;

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
	return tb_true;
}
tb_bool_t tb_string_init_writer()
{
	if (!tb_object_set_xml_writer(TB_OBJECT_TYPE_STRING, tb_string_writ_xml)) return tb_false;
	if (!tb_object_set_bin_writer(TB_OBJECT_TYPE_STRING, tb_string_writ_bin)) return tb_false;
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
	if (cstr) tb_pstring_cstrcpy(&string->pstr, cstr);

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
	if (pstr) tb_pstring_strcpy(&string->pstr, pstr);

	// ok
	return string;

	// no
fail:
	tb_string_exit(string);
	return tb_null;
}
tb_char_t* tb_string_cstr(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, tb_null);

	// cstr
	return tb_pstring_cstr(&string->pstr);
}
tb_size_t tb_string_size(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, 0);

	// size
	return tb_pstring_size(&string->pstr);
}
tb_pstring_t* tb_string_pstr(tb_object_t* object)
{
	// check
	tb_string_t* string = tb_string_cast(object);
	tb_assert_and_check_return_val(string, tb_null);

	// pstr
	return &string->pstr;
}

