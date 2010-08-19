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
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		file.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_int_t tb_file_stream_read(tb_stream_t* st, tb_byte_t* data, tb_size_t size)
{
	tb_file_stream_t* fst = st;
	TB_ASSERT(data && size);
	if (fst) return (tb_int_t)tplat_file_read(fst->hfile, (tplat_byte_t*)data, (tplat_size_t)size);
	else return -1;
}
static void tb_file_stream_close(tb_stream_t* st)
{
	tb_file_stream_t* fst = st;
	if (fst && fst->hfile != TPLAT_INVALID_HANDLE)
		tplat_file_close(fst->hfile);
}
static tb_size_t tb_file_stream_size(tb_stream_t* st)
{
	tb_file_stream_t* fst = st;
	if (fst && fst->hfile != TPLAT_INVALID_HANDLE)
		return (tb_size_t)tplat_file_seek(fst->hfile, -1, TPLAT_FILE_SEEK_SIZE);
	else return 0;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_stream_t* tb_stream_open_from_file(tb_file_stream_t* st, tb_char_t const* url, tb_stream_flag_t flag)
{
	TB_ASSERT(st && url);
	if (!st || !url) return TB_NULL;

	// open file
	tplat_handle_t hfile = tplat_file_open(url, TPLAT_FILE_RW);
	if (hfile == TPLAT_INVALID_HANDLE) return TB_NULL;

	// init stream
	memset(st, 0, sizeof(tb_file_stream_t));
	st->base.flag = flag;
	st->base.head = st->base.data;
	st->base.size = 0;
	st->base.offset = 0;

	// init file stream
	st->base.read = tb_file_stream_read;
	st->base.close = tb_file_stream_close;
	st->base.ssize = tb_file_stream_size;
	st->hfile = hfile;

	// is hzlib?
	if (flag & TB_STREAM_FLAG_IS_ZLIB)
	{
		st->base.hzlib = tb_zlib_create();
		if (st->base.hzlib == TB_INVALID_HANDLE) goto fail;
	}

	return ((tb_stream_t*)st);

fail:
	if (hfile != TPLAT_INVALID_HANDLE) tplat_file_close(hfile);
	return TB_NULL;
}
