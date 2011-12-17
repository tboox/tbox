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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		writer.h
 *
 */
#ifndef TB_XML_WRITER_H
#define TB_XML_WRITER_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "node.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_XML_WRITER_ATTRIBUTES_MAX 		(256)
#else
# 	define TB_XML_WRITER_ATTRIBUTES_MAX 		(512)
#endif

/* /////////////////////////////////////////////////////////
 * types
 */


// the xml writer - StAX
typedef struct __tb_xml_writer_t
{
	// the stream
	tb_gstream_t* 			gst;

	// the attributes
	tb_xml_attribute_t 		attributes[TB_XML_WRITER_ATTRIBUTES_MAX];
	tb_size_t 				attributes_n;

}tb_xml_writer_t;


/* /////////////////////////////////////////////////////////
 * interfaces
 */

// open & close
tb_xml_writer_t* 			tb_xml_writer_open(tb_gstream_t* gst);
tb_void_t 					tb_xml_writer_close(tb_xml_writer_t* writer);

// document
tb_void_t 					tb_xml_writer_document_beg(tb_xml_writer_t* writer, tb_char_t const* version, tb_char_t const* encoding);
tb_void_t 					tb_xml_writer_document_end(tb_xml_writer_t* writer);

// element
tb_void_t 					tb_xml_writer_element_beg(tb_xml_writer_t* writer, tb_char_t const* name);
tb_void_t 					tb_xml_writer_element_end(tb_xml_writer_t* writer, tb_char_t const* name);
tb_void_t 					tb_xml_writer_element_empty(tb_xml_writer_t* writer, tb_char_t const* name);

// attributes
tb_void_t 					tb_xml_writer_attributes_clear(tb_xml_writer_t* writer);
tb_void_t 					tb_xml_writer_attributes_add_string(tb_xml_writer_t* writer, tb_char_t const* name, tb_pstring_t const* value);
tb_void_t 					tb_xml_writer_attributes_add_c_string(tb_xml_writer_t* writer, tb_char_t const* name, tb_char_t const* value);
tb_void_t 					tb_xml_writer_attributes_add_int(tb_xml_writer_t* writer, tb_char_t const* name, tb_int_t value);
tb_void_t 					tb_xml_writer_attributes_add_bool(tb_xml_writer_t* writer, tb_char_t const* name, tb_bool_t value);
tb_void_t 					tb_xml_writer_attributes_add_format(tb_xml_writer_t* writer, tb_char_t const* name, tb_char_t const* fmt, ...);

#ifdef TB_CONFIG_TYPE_FLOAT
tb_void_t 					tb_xml_writer_attributes_add_float(tb_xml_writer_t* writer, tb_char_t const* name, tb_float_t value);
#endif

// cdata
tb_void_t 					tb_xml_writer_cdata(tb_xml_writer_t* writer, tb_char_t const* data);

// text
tb_void_t 					tb_xml_writer_text(tb_xml_writer_t* writer, tb_char_t const* text);

// comment
tb_void_t 					tb_xml_writer_comment(tb_xml_writer_t* writer, tb_char_t const* comment);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
