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
 * @file		reader.h
 * @ingroup 	xml
 *
 */
#ifndef TB_XML_READER_H
#define TB_XML_READER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "node.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the reader event type
typedef enum __tb_xml_reader_event_t
{
	TB_XML_READER_EVENT_NONE 					= 0
, 	TB_XML_READER_EVENT_DOCUMENT_TYPE 			= 1
, 	TB_XML_READER_EVENT_DOCUMENT_BEG 			= 2
, 	TB_XML_READER_EVENT_DOCUMENT_END 			= 3
, 	TB_XML_READER_EVENT_ELEMENT_BEG 			= 4
, 	TB_XML_READER_EVENT_ELEMENT_END 			= 5
, 	TB_XML_READER_EVENT_ELEMENT_EMPTY 			= 6
, 	TB_XML_READER_EVENT_COMMENT					= 7
, 	TB_XML_READER_EVENT_TEXT					= 8
, 	TB_XML_READER_EVENT_CDATA					= 9

}tb_xml_reader_event_t;


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/// init
tb_handle_t 			tb_xml_reader_init(tb_gstream_t* gst);

/// exit
tb_void_t 				tb_xml_reader_exit(tb_handle_t reader);

/// next
tb_size_t 				tb_xml_reader_next(tb_handle_t reader);

/// stream
tb_gstream_t* 			tb_xml_reader_stream(tb_handle_t reader);

/// goto: /root/node/item
tb_bool_t 				tb_xml_reader_goto(tb_handle_t reader, tb_char_t const* path);

/// version
tb_char_t const* 		tb_xml_reader_version(tb_handle_t reader);

/// encoding
tb_char_t const* 		tb_xml_reader_encoding(tb_handle_t reader);

/// name
tb_char_t const* 		tb_xml_reader_name(tb_handle_t reader);

/// text
tb_char_t const* 		tb_xml_reader_text(tb_handle_t reader);

/// cdata
tb_char_t const* 		tb_xml_reader_cdata(tb_handle_t reader);

/// comment
tb_char_t const* 		tb_xml_reader_comment(tb_handle_t reader);

/// attributes
tb_xml_node_t const* 	tb_xml_reader_attributes(tb_handle_t reader);

#endif
