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
 * @file		writer.h
 * @ingroup 	xml
 *
 */
#ifndef TB_XML_WRITER_H
#define TB_XML_WRITER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "node.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*!init writer
 *
 * @param gst 		the stream
 * @param bformat 	is format xml?
 * @return 			the writer handle
 */
tb_handle_t 			tb_xml_writer_init(tb_gstream_t* gst, tb_bool_t bformat);

/// exit
tb_void_t 				tb_xml_writer_exit(tb_handle_t writer);

/// save document or node
tb_void_t 				tb_xml_writer_save(tb_handle_t writer, tb_xml_node_t const* node);

/// document: <?xml version = \"...\" encoding = \"...\" ?>
tb_void_t 				tb_xml_writer_document(tb_handle_t writer, tb_char_t const* version, tb_char_t const* encoding);

/// cdata: <![CDATA[...]]>
tb_void_t 				tb_xml_writer_cdata(tb_handle_t writer, tb_char_t const* data);

/// text
tb_void_t 				tb_xml_writer_text(tb_handle_t writer, tb_char_t const* text);

/// comment: <!-- ... -->
tb_void_t 				tb_xml_writer_comment(tb_handle_t writer, tb_char_t const* comment);

/// element: <name/>
tb_void_t 				tb_xml_writer_element_empty(tb_handle_t writer, tb_char_t const* name);
/// element: <name> ...
tb_void_t 				tb_xml_writer_element_enter(tb_handle_t writer, tb_char_t const* name);
/// element: ... </name>
tb_void_t 				tb_xml_writer_element_leave(tb_handle_t writer);

/// attributes: long
tb_void_t 				tb_xml_writer_attributes_long(tb_handle_t writer, tb_char_t const* name, tb_long_t value);
/// attributes: boolean
tb_void_t 				tb_xml_writer_attributes_bool(tb_handle_t writer, tb_char_t const* name, tb_bool_t value);
/// attributes: c-string
tb_void_t 				tb_xml_writer_attributes_cstr(tb_handle_t writer, tb_char_t const* name, tb_char_t const* value);
/// attributes: format
tb_void_t 				tb_xml_writer_attributes_format(tb_handle_t writer, tb_char_t const* name, tb_char_t const* format, ...);
#ifdef TB_CONFIG_TYPE_FLOAT
/// attributes: float
tb_void_t 				tb_xml_writer_attributes_float(tb_handle_t writer, tb_char_t const* name, tb_float_t value);
/// attributes: double
tb_void_t 				tb_xml_writer_attributes_double(tb_handle_t writer, tb_char_t const* name, tb_double_t value);
#endif

#endif
