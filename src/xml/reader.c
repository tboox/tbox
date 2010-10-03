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
 * \file		reader.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"


/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_xml_reader_t* tb_xml_reader_open_from_stream(tb_stream_t* st)
{
	TB_ASSERT(st);
	if (!st) return TB_NULL;

	// alloc reader
	tb_xml_reader_t* reader = (tb_xml_reader_t*)tb_malloc(sizeof(tb_xml_reader_t));
	if (!reader) return TB_NULL;

	// init it
	memset(reader, 0, sizeof(tb_xml_reader_t));
	reader->st = st;
	reader->st_owner = TB_FALSE;

	return reader;
}
tb_xml_reader_t* tb_xml_reader_open_from_data(tb_byte_t const* data, tb_size_t size)
{
	TB_ASSERT(data && size);
	if (!data || !size) return TB_NULL;

	tb_data_stream_t dst;
	tb_xml_reader_t* reader = tb_xml_reader_open_from_stream(tb_stream_open_from_data(&dst, data, size, TB_STREAM_FLAG_IS_BLOCK));
	if (reader) reader->st_owner = TB_TRUE;
	return reader;
}
tb_xml_reader_t* tb_xml_reader_open_from_file(tb_char_t const* url)
{
	TB_ASSERT(url);
	if (!url) return TB_NULL;

	tb_file_stream_t fst;
	tb_xml_reader_t* reader = tb_xml_reader_open_from_stream(tb_stream_open_from_file(&fst, url, TB_STREAM_FLAG_IS_BLOCK));
	if (reader) reader->st_owner = TB_TRUE;
	return reader;
}
tb_xml_reader_t* tb_xml_reader_open_from_http(tb_char_t const* url)
{
	TB_ASSERT(url);
	if (!url) return TB_NULL;

	tb_http_stream_t hst;
	tb_xml_reader_t* reader = tb_xml_reader_open_from_stream(tb_stream_open_from_http(&hst, url, TB_STREAM_FLAG_IS_BLOCK));
	if (reader) reader->st_owner = TB_TRUE;
	return reader;
}
tb_xml_reader_t* tb_xml_reader_open_from_url(tb_char_t const* url)
{
	TB_ASSERT(url);
	if (!url) return TB_NULL;

	tb_generic_stream_t gst;
	tb_xml_reader_t* reader = tb_xml_reader_open_from_stream(tb_stream_open(&gst, url, TB_NULL, 0, TB_STREAM_FLAG_IS_BLOCK));
	if (reader) reader->st_owner = TB_TRUE;
	return reader;
}
void tb_xml_reader_close(tb_xml_reader_t* reader)
{
	if (reader)
	{
		// close stream
		if (reader->st && reader->st_owner == TB_TRUE) 
			tb_stream_close(reader->st);

		// free it
		tb_free(reader);
	}
}


