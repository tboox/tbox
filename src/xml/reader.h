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
 * @author		ruki
 * @file		reader.h
 * @ingroup 	xml
 *
 */
#ifndef TB_XML_READER_H
#define TB_XML_READER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "node.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the reader event type for iterator
typedef enum __tb_xml_reader_event_t
{
	TB_XML_READER_EVENT_NONE 					= 0
, 	TB_XML_READER_EVENT_DOCUMENT_TYPE 			= 1
, 	TB_XML_READER_EVENT_DOCUMENT 				= 2
, 	TB_XML_READER_EVENT_ELEMENT_BEG 			= 3
, 	TB_XML_READER_EVENT_ELEMENT_END 			= 4
, 	TB_XML_READER_EVENT_ELEMENT_EMPTY 			= 5
, 	TB_XML_READER_EVENT_COMMENT					= 6
, 	TB_XML_READER_EVENT_TEXT					= 7
, 	TB_XML_READER_EVENT_CDATA					= 8

}tb_xml_reader_event_t;


/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */


/*! init the xml reader
 *
 * @param gst 			the stream
 * @return 				the reader handle
 */
tb_handle_t 			tb_xml_reader_init(tb_basic_stream_t* gst);

/*! exit the xml reader
 *
 * @param reader 		the xml reader
 */
tb_void_t 				tb_xml_reader_exit(tb_handle_t reader);

/*! clear the xml reader, @note the stream will be reseted
 *
 * @param reader 		the xml reader
 */
tb_void_t 				tb_xml_reader_clear(tb_handle_t reader);

/*! the next iterator for the xml reader
 *
 * @param reader 		the xml reader
 * @return 				the iterator event 
 *
 * @code
 *
 *	// init stream
 *	tb_basic_stream_t* gst = tb_basic_stream_init_from_url(argv[1]);
 *	if (gst && tb_basic_stream_open(gst))
 *	{
 *		// init reader
 *		tb_handle_t reader = tb_xml_reader_init(gst);
 *		if (reader)
 *		{
 *			// walk
 *			tb_size_t e = TB_XML_READER_EVENT_NONE;
 *			while (e = tb_xml_reader_next(reader))
 *			{
 *				switch (e)
 *				{
 *				case TB_XML_READER_EVENT_DOCUMENT: 
 *					{
 *						tb_printf("<?xml version = \"%s\" encoding = \"%s\" ?>\n"
 *							, tb_xml_reader_version(reader), tb_xml_reader_encoding(reader));
 *					}
 *					break;
 *				case TB_XML_READER_EVENT_DOCUMENT_TYPE: 
 *					{
 *						tb_printf("<!DOCTYPE>\n");
 *					}
 *					break;
 *				case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
 *					{
 *						tb_char_t const* 		name = tb_xml_reader_element(reader);
 *						tb_xml_node_t const* 	attr = tb_xml_reader_attributes(reader);
 *						if (!attr) tb_printf("<%s/>\n", name);
 *						else
 *						{
 *							tb_printf("<%s", name);
 *							for (; attr; attr = attr->next)
 *								tb_printf(" %s = \"%s\"", tb_scoped_string_cstr(&attr->name), tb_scoped_string_cstr(&attr->data));
 *							tb_printf("/>\n");
 *						}
 *					}
 *					break;
 *				case TB_XML_READER_EVENT_ELEMENT_BEG: 
 *					{
 *						tb_char_t const* 		name = tb_xml_reader_element(reader);
 *						tb_xml_node_t const* 	attr = tb_xml_reader_attributes(reader);
 *						if (!attr) tb_printf("<%s>\n", name);
 *						else
 *						{
 *							tb_printf("<%s", name);
 *							for (; attr; attr = attr->next)
 *								tb_printf(" %s = \"%s\"", tb_scoped_string_cstr(&attr->name), tb_scoped_string_cstr(&attr->data));
 *							tb_printf(">\n");
 *						}
 *					}
 *					break;
 *				case TB_XML_READER_EVENT_ELEMENT_END: 
 *					{
 *						tb_printf("</%s>\n", tb_xml_reader_element(reader));
 *					}
 *				break;
 *				case TB_XML_READER_EVENT_TEXT: 
 *					{
 *						tb_printf("%s", tb_xml_reader_text(reader));
 *					}
 *					break;
 *				case TB_XML_READER_EVENT_CDATA: 
 *					{
 *						tb_printf("<![CDATA[%s]]>", tb_xml_reader_cdata(reader));
 *					}
 *					break;
 *				case TB_XML_READER_EVENT_COMMENT: 
 *					{
 *						tb_printf("<!--%s-->", tb_xml_reader_comment(reader));
 *					}
 *					break;
 *				default:
 *					break;
 *				}
 *			}
 *
 *			// exit reader
 *			tb_xml_reader_exit(reader);
 *		}
 *	
 *		// exit stream
 *		tb_basic_stream_exit(gst);
 *	}
 * @endcode
 */
tb_size_t 				tb_xml_reader_next(tb_handle_t reader);

/*! the xml stream
 *
 * @param reader 		the xml reader
 * @return 				the xml stream
 */
tb_basic_stream_t* 			tb_xml_reader_stream(tb_handle_t reader);

/*! the xml level
 *
 * @param reader 		the xml reader
 * @return 				the xml level for tab spaces
 */
tb_size_t 				tb_xml_reader_level(tb_handle_t reader);

/*! seek to the given node for xml, .e.g /root/node/item
 *
 * @param reader 		the reader handle
 * @param path 			the xml path
 * @return 				tb_true or tb_false
 *
 * @note the stream will be reseted
 */
tb_bool_t 				tb_xml_reader_goto(tb_handle_t reader, tb_char_t const* path);

/*! load the xml 
 *
 * @param reader 		the xml reader
 * @return 				the xml root node
 */
tb_xml_node_t* 			tb_xml_reader_load(tb_handle_t reader);

/*! the xml version
 *
 * @param reader 		the xml reader
 * @return 				the xml version
 */
tb_char_t const* 		tb_xml_reader_version(tb_handle_t reader);

/*! the xml charset
 *
 * @param reader 		the xml reader
 * @return 				the xml charset
 */
tb_char_t const* 		tb_xml_reader_charset(tb_handle_t reader);

/*! the current xml element name
 *
 * @param reader 		the xml reader
 * @return 				the current xml element name
 */
tb_char_t const* 		tb_xml_reader_element(tb_handle_t reader);

/*! the current xml node text
 *
 * @param reader 		the xml reader
 * @return 				the current xml node text
 */
tb_char_t const* 		tb_xml_reader_text(tb_handle_t reader);

/*! the current xml node cdata
 *
 * @param reader 		the xml reader
 * @return 				the current xml node cdata
 */
tb_char_t const* 		tb_xml_reader_cdata(tb_handle_t reader);

/*! the current xml node comment
 *
 * @param reader 		the xml reader
 * @return 				the current xml node comment
 */
tb_char_t const* 		tb_xml_reader_comment(tb_handle_t reader);

/*! the xml document type
 *
 * @param reader 		the xml reader
 * @return 				the xml document type
 */
tb_char_t const* 		tb_xml_reader_doctype(tb_handle_t reader);

/*! the current xml node attributes
 *
 * @param reader 		the xml reader
 * @return 				the current xml node attributes
 */
tb_xml_node_t const* 	tb_xml_reader_attributes(tb_handle_t reader);

#endif
