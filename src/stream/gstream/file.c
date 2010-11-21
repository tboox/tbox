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
static tb_int_t tb_fstream_read(tb_gstream_t* st, tb_byte_t* data, tb_size_t size)
{
	tb_fstream_t* fst = st;
	TB_ASSERT(data && size);
	if (fst) return (tb_int_t)tplat_file_read(fst->hfile, (tplat_byte_t*)data, (tplat_size_t)size);
	else return -1;
}
static tb_int_t tb_fstream_write(tb_gstream_t* st, tb_byte_t* data, tb_size_t size)
{
	tb_fstream_t* fst = st;
	TB_ASSERT(data && size);
	if (fst) return (tb_int_t)tplat_file_write(fst->hfile, (tplat_byte_t*)data, (tplat_size_t)size);
	else return -1;
}
static void tb_fstream_close(tb_gstream_t* st)
{
	tb_fstream_t* fst = st;
	if (fst && fst->hfile != TPLAT_INVALID_HANDLE)
		tplat_file_close(fst->hfile);
}
static tb_size_t tb_fstream_size(tb_gstream_t* st)
{
	tb_fstream_t* fst = st;
	if (fst && fst->hfile != TPLAT_INVALID_HANDLE && !(st->flag & TB_GSTREAM_FLAG_ZLIB))
		return (tb_size_t)tplat_file_seek(fst->hfile, -1, TPLAT_FILE_SEEK_SIZE);
	else return 0;
}
static tb_bool_t tb_fstream_seek(tb_gstream_t* st, tb_int_t offset, tb_gstream_seek_t flag)
{
	tb_fstream_t* fst = st;
	if (fst && !(st->flag & TB_GSTREAM_FLAG_ZLIB))
	{
		tb_int_t ret = -1;

		// adjust offset
		if (st->size)
		{
			if (flag == TB_GSTREAM_SEEK_CUR && offset >= 0 && offset <= st->size) 
			{
				st->head += offset;
				st->size -= offset;
				st->offset += offset;
				return TB_TRUE;
			}

			offset -= st->size;
			st->head = st->data;
			st->size = 0;
		}

		// seek
		if (flag == TB_GSTREAM_SEEK_BEG) ret = tplat_file_seek(fst->hfile, offset, TPLAT_FILE_SEEK_BEG);
		else if (flag == TB_GSTREAM_SEEK_CUR) ret = tplat_file_seek(fst->hfile, offset, TPLAT_FILE_SEEK_CUR);
		else if (flag == TB_GSTREAM_SEEK_END) ret = tplat_file_seek(fst->hfile, offset, TPLAT_FILE_SEEK_END);

		// update offset
		if (ret >= 0) 
		{
			st->offset = ret;
			return TB_TRUE;
		}
		else return TB_FALSE;
	}
	else return TB_FALSE;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_t* tb_gstream_open_from_file(tb_fstream_t* st, tb_char_t const* url, tb_gstream_flag_t flag)
{
	TB_ASSERT(st && url);
	if (!st || !url) return TB_NULL;

	// file flags
	tplat_size_t fflags = TPLAT_FILE_BINARY;
	if (flag & TB_GSTREAM_FLAG_RO) fflags |= TPLAT_FILE_RO;
	if (flag & TB_GSTREAM_FLAG_WO) fflags |= TPLAT_FILE_WO;
	if (flag & TB_GSTREAM_FLAG_TRUNC) fflags |= TPLAT_FILE_TRUNC;

	// { open file
	tplat_handle_t hfile = tplat_file_open(url, fflags);
	if (hfile == TPLAT_INVALID_HANDLE) 
	{
		// exists arguments: ?=...
		tb_char_t const* p = url;
		for (; *p != '?' && *p; p++) ;
		if (!*p) return TB_NULL;

		// try open url without arguments
		tb_char_t s[4096];
		tb_size_t n = p - url;
		TB_ASSERT(n < 4096);
		if (n >= 4096) return TB_NULL;
		strncpy(s, url, n);
		s[n] = '\0';

		hfile = tplat_file_open(s, fflags);
		if (hfile == TPLAT_INVALID_HANDLE) return TB_NULL;
	}

	// init stream
	memset(st, 0, sizeof(tb_fstream_t));
	st->base.flag = flag;
	st->base.head = st->base.data;
	st->base.size = 0;
	st->base.offset = 0;

	// init url
	tb_string_init(&st->base.url);
	tb_string_assign_c_string(&st->base.url, url);

	// init file stream
	st->base.read = tb_fstream_read;
	st->base.write = tb_fstream_write;
	st->base.close = tb_fstream_close;
	st->base.ssize = tb_fstream_size;
	st->base.seek = tb_fstream_seek;
	st->hfile = hfile;

#ifdef TB_CONFIG_ZLIB
	// is hzlib?
	if (flag & TB_GSTREAM_FLAG_ZLIB)
	{
		st->base.hzlib = tb_zlib_create();
		if (st->base.hzlib == TB_INVALID_HANDLE) goto fail;
	}
#endif
	return ((tb_gstream_t*)st);

fail:
	if (hfile != TPLAT_INVALID_HANDLE) tplat_file_close(hfile);
	return TB_NULL;
	// }
}
