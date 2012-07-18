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
 * @file		writer.c
 * @ingroup 	xml
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 		"xml"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "writer.h"
#include "../encoding/encoding.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the xml writer type
typedef struct __tb_xml_writer_t
{
	// stream
	tb_gstream_t* 			wstream;

	// is format?
	tb_bool_t 				bformat;

}tb_xml_writer_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_handle_t tb_xml_writer_init(tb_gstream_t* gst, tb_bool_t bformat)
{
	// check
	tb_assert_and_check_return_val(gst, TB_NULL);

	// alloc
	tb_xml_writer_t* writer = tb_malloc0(sizeof(tb_xml_writer_t));
	tb_assert_and_check_return_val(writer, TB_NULL);

	// init
	writer->wstream = gst;
	writer->bformat = bformat;

	// ok
	return writer;
}
tb_void_t tb_xml_writer_exit(tb_handle_t writer)
{
}
tb_void_t tb_xml_writer_save(tb_handle_t writer, tb_xml_node_t const* node)
{
}
tb_void_t tb_xml_writer_document(tb_handle_t writer, tb_char_t const* version, tb_char_t const* encoding)
{
}
tb_void_t tb_xml_writer_cdata(tb_handle_t writer, tb_char_t const* data)
{
}
tb_void_t tb_xml_writer_text(tb_handle_t writer, tb_char_t const* text)
{
}
tb_void_t tb_xml_writer_comment(tb_handle_t writer, tb_char_t const* comment)
{
}
tb_void_t tb_xml_writer_element_empty(tb_handle_t writer, tb_char_t const* name)
{
}
tb_void_t tb_xml_writer_element_enter(tb_handle_t writer, tb_char_t const* name)
{
}
tb_void_t tb_xml_writer_element_leave(tb_handle_t writer, tb_char_t const* name)
{
}
tb_void_t tb_xml_writer_attributes_long(tb_handle_t writer, tb_char_t const* name, tb_long_t value)
{
}
tb_void_t tb_xml_writer_attributes_bool(tb_handle_t writer, tb_char_t const* name, tb_bool_t value)
{
}
tb_void_t tb_xml_writer_attributes_cstr(tb_handle_t writer, tb_char_t const* name, tb_char_t const* value)
{
}
tb_void_t tb_xml_writer_attributes_format(tb_handle_t writer, tb_char_t const* name, tb_char_t const* format, ...)
{
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_void_t tb_xml_writer_attributes_float(tb_handle_t writer, tb_char_t const* name, tb_float_t value)
{
}
tb_void_t tb_xml_writer_attributes_double(tb_handle_t writer, tb_char_t const* name, tb_double_t value)
{
}
#endif

