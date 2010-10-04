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
 * \file		document.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "document.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#ifndef TPLAT_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO
#if 1
# 	define TB_DOCUMENT_DBG(fmt, arg...) 			TB_DBG("[xml]: " fmt, ##arg)
#else
# 	define TB_DOCUMENT_DBG(fmt, arg...)
#endif

#else
# 	define TB_DOCUMENT_DBG
#endif

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_xml_document_t* tb_xml_document_create()
{
	// alloc document
	tb_xml_document_t* document = (tb_xml_document_t*)tb_malloc(sizeof(tb_xml_document_t));
	if (!document) return TB_NULL;

	// clear it
	tb_xml_document_clear(document);

	return document;
}
void tb_xml_document_destroy(tb_xml_document_t* document)
{
	if (document)
	{
		// free it
		tb_free(document);
	}
}

#ifdef TB_DEBUG

tb_bool_t tb_xml_document_load_dump(tb_xml_document_t* document, tb_stream_t* st)
{
	TB_ASSERT(document);
	if (!document) return TB_FALSE;

	// open reader
	tb_xml_reader_t* reader = tb_xml_reader_open(st);
	TB_ASSERT(reader);
	if (!reader) return TB_FALSE;

	// init return
	tb_bool_t ret = TB_TRUE;

	// has event?
	while (TB_TRUE == tb_xml_reader_has_next(reader))
	{
		// get event type
		tb_size_t event = tb_xml_reader_get_event(reader);
		switch (event)
		{
		case TB_XML_READER_EVENT_DOCUMENT_BEG: 
			{
				tb_char_t const* version = tb_xml_reader_get_version(reader);
				tb_char_t const* encoding = tb_xml_reader_get_encoding(reader);
				tplat_printf("<?xml version = \"%s\" encoding = \"%s\" ?>\n", version? version : "", encoding? encoding : "");
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
			{
				tb_char_t const* name = tb_xml_reader_get_element_name(reader);
				tplat_printf("<%s>", name? name : "");
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				tb_char_t const* name = tb_xml_reader_get_element_name(reader);
				tplat_printf("</%s>", name? name : "");
			}
			break;
		case TB_XML_READER_EVENT_CHARACTERS: 
			{
				tb_char_t const* text = tb_xml_reader_get_characters_text(reader);
				tplat_printf("%s", text? text : "");
			}
			break;
		case TB_XML_READER_EVENT_COMMENT: 
			{
				tb_char_t const* text = tb_xml_reader_get_comment_text(reader);
				tplat_printf("<!--%s-->", text? text : "");
			}
			break;
		default: break;
		}

		// next event
		tb_xml_reader_next(reader);
	}

	tplat_printf("\n");

end:
	// close reader
	if (reader) tb_xml_reader_close(reader);

	return ret;
}
#endif

tb_bool_t tb_xml_document_load(tb_xml_document_t* document, tb_stream_t* st)
{
	TB_ASSERT(document);
	if (!document) return TB_FALSE;

	// open reader
	tb_xml_reader_t* reader = tb_xml_reader_open(st);
	TB_ASSERT(reader);
	if (!reader) return TB_FALSE;

	// init return
	tb_bool_t ret = TB_TRUE;


	return ret;
}

tb_bool_t tb_xml_document_store(tb_xml_document_t* document, tb_stream_t* st)
{
	return TB_FALSE;
}

void tb_xml_document_clear(tb_xml_document_t* document)
{
	TB_ASSERT(document);
	if (!document) return ;

	memset(document, 0, sizeof(tb_xml_document_t));
}
