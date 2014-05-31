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
 * @author      ruki
 * @file        writer.h
 * @ingroup     xml
 *
 */
#ifndef TB_XML_WRITER_H
#define TB_XML_WRITER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "node.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the xml writer
 *
 * @param gst           the stream
 * @param bformat       is format xml?
 * @return              the writer handle
 */
tb_handle_t             tb_xml_writer_init(tb_basic_stream_t* gst, tb_bool_t bformat);

/*! exit the xml writer
 *
 * @param writer        the xml writer
 */
tb_void_t               tb_xml_writer_exit(tb_handle_t writer);

/*! save the xml document or node
 *
 * @param writer        the xml writer
 * @param node          the xml node
 */
tb_void_t               tb_xml_writer_save(tb_handle_t writer, tb_xml_node_t const* node);

/*! writ the xml document node: <?xml version = \"...\" encoding = \"...\" ?>
 *
 * @param writer        the xml writer 
 * @param version       the xml version
 * @param encoding      the xml encoding
 */
tb_void_t               tb_xml_writer_document(tb_handle_t writer, tb_char_t const* version, tb_char_t const* encoding);

/*! writ the xml document type: <!DOCTYPE type>
 *
 * @param writer        the xml writer
 * @param type          the xml document type
 */
tb_void_t               tb_xml_writer_document_type(tb_handle_t writer, tb_char_t const* type);

/*! writ the xml cdata: <![CDATA[...]]>
 *
 * @param writer        the xml writer
 * @param data          the xml cdata 
 */
tb_void_t               tb_xml_writer_cdata(tb_handle_t writer, tb_char_t const* data);

/*! writ the xml text
 *
 * @param writer        the xml writer
 * @param text          the xml text 
 */
tb_void_t               tb_xml_writer_text(tb_handle_t writer, tb_char_t const* text);

/*! writ the xml comment: <!-- ... -->
 *
 * @param writer        the xml writer
 * @param comment       the xml comment 
 */
tb_void_t               tb_xml_writer_comment(tb_handle_t writer, tb_char_t const* comment);

/*! writ the empty xml element: <name/>
 *
 * @param writer        the xml writer
 * @param name          the xml element name 
 */
tb_void_t               tb_xml_writer_element_empty(tb_handle_t writer, tb_char_t const* name);

/*! writ the xml element head: <name> ...
 *
 * @param writer        the xml writer
 * @param name          the xml element name 
 */
tb_void_t               tb_xml_writer_element_enter(tb_handle_t writer, tb_char_t const* name);

/*! writ the xml element tail: ... </name>
 *
 * @param writer        the xml writer
 */
tb_void_t               tb_xml_writer_element_leave(tb_handle_t writer);

/*! writ the xml attribute for long value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_long(tb_handle_t writer, tb_char_t const* name, tb_long_t value);

/*! writ the xml attribute for boolean value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_bool(tb_handle_t writer, tb_char_t const* name, tb_bool_t value);

/*! writ the xml attribute for cstr value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_cstr(tb_handle_t writer, tb_char_t const* name, tb_char_t const* value);

/*! writ the xml attribute for format value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param format        the xml attribute format
 */
tb_void_t               tb_xml_writer_attributes_format(tb_handle_t writer, tb_char_t const* name, tb_char_t const* format, ...);

#ifdef TB_CONFIG_TYPE_FLOAT
/*! writ the xml attribute for float value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_float(tb_handle_t writer, tb_char_t const* name, tb_float_t value);

/*! writ the xml attribute for double value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_double(tb_handle_t writer, tb_char_t const* name, tb_double_t value);
#endif

#endif
