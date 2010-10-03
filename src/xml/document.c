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
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_bool_t tb_xml_document_load(tb_xml_document_t* document, tb_xml_reader_t* reader)
{
	return TB_FALSE;
}
static tb_bool_t tb_xml_document_store(tb_xml_document_t* document, tb_xml_writer_t* writer)
{
	return TB_FALSE;
}

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

tb_bool_t tb_xml_document_load_stream(tb_xml_document_t* document, tb_stream_t* st)
{
	return tb_xml_document_load(document, tb_xml_reader_open_from_stream(st));
}
tb_bool_t tb_xml_document_load_data(tb_xml_document_t* document,tb_byte_t const* data, tb_size_t size)
{
	return tb_xml_document_load(document, tb_xml_reader_open_from_data(st, data, size));
}
tb_bool_t tb_xml_document_load_file(tb_xml_document_t* document,tb_char_t const* url)
{
	return tb_xml_document_load(document, tb_xml_reader_open_from_file(st, url));
}
tb_bool_t tb_xml_document_load_http(tb_xml_document_t* document,tb_char_t const* url)
{
	return tb_xml_document_load(document, tb_xml_reader_open_from_http(st, url));
}
tb_bool_t tb_xml_document_load_url(tb_xml_document_t* document,tb_char_t const* url)
{
	return tb_xml_document_load(document, tb_xml_reader_open_from_url(st, url));
}

tb_bool_t tb_xml_document_store_stream(tb_xml_document_t* document, tb_stream_t* st)
{
	return tb_xml_document_store(document, tb_xml_writer_open_from_stream(st));
}
tb_bool_t tb_xml_document_store_data(tb_xml_document_t* document,tb_byte_t const* data, tb_size_t size)
{
	return tb_xml_document_store(document, tb_xml_writer_open_from_data(st, data, size));
}
tb_bool_t tb_xml_document_store_file(tb_xml_document_t* document,tb_char_t const* url)
{
	return tb_xml_document_store(document, tb_xml_writer_open_from_file(st, url));
}
tb_bool_t tb_xml_document_store_http(tb_xml_document_t* document,tb_char_t const* url)
{
	return tb_xml_document_store(document, tb_xml_writer_open_from_http(st, url));
}
tb_bool_t tb_xml_document_store_url(tb_xml_document_t* document,tb_char_t const* url)
{
	return tb_xml_document_store(document, tb_xml_writer_open_from_url(st, url));
}

void tb_xml_document_clear(tb_xml_document_t* document)
{
	TB_ASSERT(document);
	if (!document) return ;

	memset(document, 0, sizeof(tb_xml_document_t));
}
