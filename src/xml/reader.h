/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		reader.h
 *
 */
#ifndef TB_XML_READER_H
#define TB_XML_READER_H

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

#ifdef TB_MEMORY_MODE_SMALL
# 	define TB_XML_READER_ATTRIBUTES_MAX 		(256)
#else
# 	define TB_XML_READER_ATTRIBUTES_MAX 		(512)
#endif


#ifdef TB_DEBUG
# 	define TB_XML_READER_DUMP(r) 				tb_xml_reader_dump(r)
#else 
# 	define TB_XML_READER_DUMP(r)
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// the reader event type
typedef enum __tb_xml_reader_event_t
{
	TB_XML_READER_EVENT_NULL 					= 0
, 	TB_XML_READER_EVENT_DOCUMENT_BEG 			= 1
, 	TB_XML_READER_EVENT_DOCUMENT_END 			= 2
, 	TB_XML_READER_EVENT_ELEMENT_BEG 			= 3
, 	TB_XML_READER_EVENT_ELEMENT_END 			= 4
, 	TB_XML_READER_EVENT_ELEMENT_EMPTY 			= 5
, 	TB_XML_READER_EVENT_COMMENT					= 6
, 	TB_XML_READER_EVENT_TEXT					= 7
, 	TB_XML_READER_EVENT_CDATA					= 8

}tb_xml_reader_event_t;

// the xml reader - StAX
typedef struct __tb_xml_reader_t
{
	// the reference to stream
	tb_stream_t* 			st;

	// the event
	tb_size_t 				event;

	// the cache character
	tb_char_t 				cache;

	// the version
	tb_string_t 			version;

	// the encoding
	tb_string_t 			encoding;

	// the element
	tb_string_t 			element;

	// the element name
	tb_string_t 			name;

	// the text
	tb_string_t 			text;

	// the attributes
	tb_xml_attribute_t 		attributes[TB_XML_READER_ATTRIBUTES_MAX];
	tb_size_t 				attributes_n;

}tb_xml_reader_t;


/* /////////////////////////////////////////////////////////
 * interfaces
 */

// open & close
tb_xml_reader_t* 		tb_xml_reader_open(tb_stream_t* st);
void 					tb_xml_reader_close(tb_xml_reader_t* reader);

// iterator
tb_bool_t 				tb_xml_reader_has_next(tb_xml_reader_t* reader);
tb_size_t 				tb_xml_reader_next(tb_xml_reader_t* reader);

// seek: /root/node/item
tb_bool_t 				tb_xml_reader_seek(tb_xml_reader_t* reader, tb_char_t const* path);

// debug
void 					tb_xml_reader_dump(tb_xml_reader_t* reader);

// getter
tb_size_t 				tb_xml_reader_get_event(tb_xml_reader_t* reader);
tb_string_t const* 		tb_xml_reader_get_version(tb_xml_reader_t* reader);
tb_string_t const* 		tb_xml_reader_get_encoding(tb_xml_reader_t* reader);
tb_string_t const* 		tb_xml_reader_get_text(tb_xml_reader_t* reader);
tb_string_t const* 		tb_xml_reader_get_comment(tb_xml_reader_t* reader);
tb_string_t const* 		tb_xml_reader_get_cdata(tb_xml_reader_t* reader);
tb_string_t const* 		tb_xml_reader_get_element_name(tb_xml_reader_t* reader);
tb_size_t 				tb_xml_reader_get_attribute_count(tb_xml_reader_t* reader);
tb_string_t const* 		tb_xml_reader_get_attribute_name(tb_xml_reader_t* reader, tb_int_t index);
tb_string_t const* 		tb_xml_reader_get_attribute_value_by_index(tb_xml_reader_t* reader, tb_int_t index);
tb_string_t const* 		tb_xml_reader_get_attribute_value_by_name(tb_xml_reader_t* reader, tb_char_t const* name);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
