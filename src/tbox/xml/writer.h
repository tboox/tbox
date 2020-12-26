/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
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
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the xml writer ref type
typedef __tb_typeref__(xml_writer);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the xml writer
 *
 * @return              the writer
 */
tb_xml_writer_ref_t     tb_xml_writer_init(tb_noarg_t);

/*! exit the xml writer
 *
 * @param writer        the xml writer
 */
tb_void_t               tb_xml_writer_exit(tb_xml_writer_ref_t writer);

/*! open the xml writer
 *
 * @param writer        the xml writer
 * @param bformat       is format xml?
 * @param stream        the stream, will open it if be not opened
 * @param bowner        the xml writer is owner of the stream?
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_xml_writer_open(tb_xml_writer_ref_t writer, tb_bool_t bformat, tb_stream_ref_t stream, tb_bool_t bowner);

/*! clos the xml writer
 *
 * @param writer        the xml writer
 */
tb_void_t               tb_xml_writer_clos(tb_xml_writer_ref_t writer);

/*! save the xml document or node
 *
 * @param writer        the xml writer
 * @param node          the xml node
 */
tb_void_t               tb_xml_writer_save(tb_xml_writer_ref_t writer, tb_xml_node_ref_t node);

/*! write the xml document node: <?xml version = \"...\" encoding = \"...\" ?>
 *
 * @param writer        the xml writer
 * @param version       the xml version
 * @param encoding      the xml encoding
 */
tb_void_t               tb_xml_writer_document(tb_xml_writer_ref_t writer, tb_char_t const* version, tb_char_t const* encoding);

/*! write the xml document type: <!DOCTYPE type>
 *
 * @param writer        the xml writer
 * @param type          the xml document type
 */
tb_void_t               tb_xml_writer_document_type(tb_xml_writer_ref_t writer, tb_char_t const* type);

/*! write the xml cdata: <![CDATA[...]]>
 *
 * @param writer        the xml writer
 * @param data          the xml cdata
 */
tb_void_t               tb_xml_writer_cdata(tb_xml_writer_ref_t writer, tb_char_t const* data);

/*! write the xml text
 *
 * @param writer        the xml writer
 * @param text          the xml text
 */
tb_void_t               tb_xml_writer_text(tb_xml_writer_ref_t writer, tb_char_t const* text);

/*! write the xml comment: <!-- ... -->
 *
 * @param writer        the xml writer
 * @param comment       the xml comment
 */
tb_void_t               tb_xml_writer_comment(tb_xml_writer_ref_t writer, tb_char_t const* comment);

/*! write the empty xml element: <name/>
 *
 * @param writer        the xml writer
 * @param name          the xml element name
 */
tb_void_t               tb_xml_writer_element_empty(tb_xml_writer_ref_t writer, tb_char_t const* name);

/*! write the xml element head: <name> ...
 *
 * @param writer        the xml writer
 * @param name          the xml element name
 */
tb_void_t               tb_xml_writer_element_enter(tb_xml_writer_ref_t writer, tb_char_t const* name);

/*! write the xml element tail: ... </name>
 *
 * @param writer        the xml writer
 */
tb_void_t               tb_xml_writer_element_leave(tb_xml_writer_ref_t writer);

/*! write the xml attribute for long value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_long(tb_xml_writer_ref_t writer, tb_char_t const* name, tb_long_t value);

/*! write the xml attribute for boolean value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_bool(tb_xml_writer_ref_t writer, tb_char_t const* name, tb_bool_t value);

/*! write the xml attribute for cstr value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_cstr(tb_xml_writer_ref_t writer, tb_char_t const* name, tb_char_t const* value);

/*! write the xml attribute for format value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param format        the xml attribute format
 */
tb_void_t               tb_xml_writer_attributes_format(tb_xml_writer_ref_t writer, tb_char_t const* name, tb_char_t const* format, ...);

#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
/*! write the xml attribute for float value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_float(tb_xml_writer_ref_t writer, tb_char_t const* name, tb_float_t value);

/*! write the xml attribute for double value
 *
 * @param writer        the xml writer
 * @param name          the xml attribute name
 * @param value         the xml attribute value
 */
tb_void_t               tb_xml_writer_attributes_double(tb_xml_writer_ref_t writer, tb_char_t const* name, tb_double_t value);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
