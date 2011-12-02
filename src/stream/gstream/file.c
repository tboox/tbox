/*!The Tiny Box Library
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
 * \file		file.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../string/string.h"
#include "../../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// need larger cache for performance
#define TB_FSTREAM_CACHE_MAXN 					(TB_GSTREAM_CACHE_MAXN << 2)

/* /////////////////////////////////////////////////////////
 * types
 */

// the file stream type
typedef struct __tb_fstream_t
{
	// the base
	tb_gstream_t 		base;

	// the file handle
	tb_handle_t 		file;

	// the file size
	tb_uint64_t 		size;

	// the file offset
	tb_uint64_t 		offset;

	// the file flags
	tb_size_t 			flags;

}tb_fstream_t;


/* /////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_fstream_t* tb_fstream_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_FILE, TB_NULL);
	return (tb_fstream_t*)gst;
}
static tb_bool_t tb_fstream_open(tb_gstream_t* gst)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && !fst->file && gst->url, TB_FALSE);

	// open file
	fst->file = tb_file_open(gst->url, fst->flags);
	tb_assert_and_check_return_val(fst->file, TB_FALSE);

	// init size
	fst->size = tb_file_size(fst->file);
	fst->offset = 0;
	
	return TB_TRUE;
}
static tb_void_t tb_fstream_close(tb_gstream_t* gst)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	if (fst && fst->file)
	{
		tb_file_close(fst->file);
		fst->file = TB_NULL;
		fst->size = 0;
		fst->offset = 0;
	}
}
static tb_long_t tb_fstream_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file && data, -1);
	tb_check_return_val(size, 0);

	// read data
	tb_long_t ret = tb_file_read(fst->file, data, size);

	// update offset
	if (ret > 0) fst->offset += ret;
	return ret;
}
static tb_long_t tb_fstream_writ(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file && data, -1);
	tb_check_return_val(size, 0);

	// writ
	tb_long_t ret = tb_file_writ(fst->file, (tb_byte_t*)data, (tb_size_t)size);
	
	// flush data
	tb_file_flush(fst->file);

	// update offset
	if (ret > 0) fst->offset += ret;
	return ret;
}
static tb_bool_t tb_fstream_seek(tb_gstream_t* gst, tb_int64_t offset, tb_gstream_seek_t flag)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file, TB_FALSE);

	// seek
	tb_int64_t ret = -1;
	switch (flag)
	{
	case TB_GSTREAM_SEEK_BEG:
		ret = tb_file_seek(fst->file, offset, TB_FILE_SEEK_BEG);
		break;
	case TB_GSTREAM_SEEK_CUR:
		ret = tb_file_seek(fst->file, offset, TB_FILE_SEEK_CUR);
		break;
	case TB_GSTREAM_SEEK_END:
		ret = tb_file_seek(fst->file, offset, TB_FILE_SEEK_END);
		break;
	default:
		break;
	}

	// offset
	if (ret >= 0) 
	{
		fst->offset = ret;
		return TB_TRUE;
	}

	return TB_FALSE;
}
static tb_uint64_t tb_fstream_size(tb_gstream_t* gst)
{	
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file, 0);
	return fst->size;
}
static tb_uint64_t tb_fstream_offset(tb_gstream_t* gst)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file, 0);
	return fst->offset;
}
static tb_bool_t tb_fstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst, TB_FALSE);

	switch (cmd)
	{
	case TB_FSTREAM_CMD_SET_FLAGS:
		fst->flags = (tb_size_t)arg1;
		return TB_TRUE;
	default:
		break;
	}
	return TB_FALSE;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_t* tb_gstream_create_file()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_fstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init stream
	tb_fstream_t* fst = (tb_fstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_FILE;
	gst->open 	= tb_fstream_open;
	gst->close 	= tb_fstream_close;
	gst->read 	= tb_fstream_read;
	gst->writ 	= tb_fstream_writ;
	gst->size 	= tb_fstream_size;
	gst->offset = tb_fstream_offset;
	gst->seek 	= tb_fstream_seek;
	gst->ioctl1 = tb_fstream_ioctl1;
	fst->file 	= TB_NULL;
	fst->flags 	= TB_FILE_RO | TB_FILE_BINARY;

	// need larger cache for performance
	gst->cache_maxn = TB_FSTREAM_CACHE_MAXN;

	return gst;
}

