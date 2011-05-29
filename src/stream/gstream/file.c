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
#include "../../string/string.h"
#include "../../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_FSTREAM_URL_MAX 		(4096)

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
	tb_size_t 			size;

	// the file offset
	tb_size_t 			offset;

	// the file flags
	tb_size_t 			flags;

	// the url
	tb_char_t 			url[TB_FSTREAM_URL_MAX];

}tb_fstream_t;


/* /////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_fstream_t* tb_fstream_cast(tb_gstream_t* gst)
{
	TB_ASSERT_RETURN_VAL(gst && gst->type == TB_GSTREAM_TYPE_FILE, TB_NULL);
	return (tb_fstream_t*)gst;
}
static tb_bool_t tb_fstream_open(tb_gstream_t* gst)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	TB_ASSERT_RETURN_VAL(fst, TB_FALSE);
	TB_ASSERT(!fst->file);

	// open file
	fst->file = tb_file_open(fst->url, fst->flags);
	TB_ASSERT_RETURN_VAL(fst->file, TB_FALSE);

	// init size
	fst->size = (tb_size_t)tb_file_seek(fst->file, -1, TB_FILE_SEEK_SIZE);
	fst->offset = 0;
	
	return TB_TRUE;
}
static void tb_fstream_close(tb_gstream_t* gst)
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
static tb_int_t tb_fstream_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	TB_ASSERT_RETURN_VAL(fst && fst->file && data, -1);
	TB_IF_FAIL_RETURN_VAL(size, 0);

	// read data
	tb_int_t ret = (tb_int_t)tb_file_read(fst->file, (tb_byte_t*)data, (tb_size_t)size);

	// update offset
	if (ret > 0) fst->offset += ret;
	return ret;
}
static tb_int_t tb_fstream_write(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	TB_ASSERT_RETURN_VAL(fst && fst->file && data, -1);
	TB_IF_FAIL_RETURN_VAL(size, 0);

	// write
	tb_int_t ret = (tb_int_t)tb_file_write(fst->file, (tb_byte_t*)data, (tb_size_t)size);

	// update offset
	if (ret > 0) fst->offset += ret;
	return ret;
}
static tb_bool_t tb_fstream_seek(tb_gstream_t* gst, tb_int_t offset, tb_gstream_seek_t flag)
{
	TB_NOT_IMPLEMENT();
	return TB_FALSE;
}
static tb_size_t tb_fstream_size(tb_gstream_t* gst)
{	
	tb_fstream_t* fst = tb_fstream_cast(gst);
	TB_ASSERT_RETURN_VAL(fst && fst->file, 0);
	return fst->size;
}
static tb_size_t tb_fstream_offset(tb_gstream_t* gst)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	TB_ASSERT_RETURN_VAL(fst && fst->file, 0);
	return fst->offset;
}
static tb_bool_t tb_fstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, void* arg1)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	TB_ASSERT_RETURN_VAL(fst, TB_FALSE);

	switch (cmd)
	{
	case TB_GSTREAM_CMD_SET_URL:
		{
			TB_ASSERT_RETURN_VAL(arg1, TB_FALSE);
			tb_cstring_ncopy(fst->url, (tb_char_t const*)arg1, TB_FSTREAM_URL_MAX);
			fst->url[TB_FSTREAM_URL_MAX - 1] = '\0';
			return TB_TRUE;
		}
	case TB_GSTREAM_CMD_GET_URL:
		{
			tb_char_t const** purl = (tb_char_t const**)arg1;
			TB_ASSERT_RETURN_VAL(purl, TB_FALSE);
			*purl = fst->url;
			return TB_TRUE;
		}
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
	TB_ASSERT_RETURN_VAL(gst, TB_NULL);

	// init stream
	tb_fstream_t* fst = (tb_fstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_FILE;
	gst->open 	= tb_fstream_open;
	gst->close 	= tb_fstream_close;
	gst->read 	= tb_fstream_read;
	gst->write 	= tb_fstream_write;
	gst->size 	= tb_fstream_size;
	gst->offset = tb_fstream_offset;
	gst->seek 	= tb_fstream_seek;
	gst->ioctl1 = tb_fstream_ioctl1;
	fst->file 	= TB_NULL;
	fst->flags 	= TB_FILE_RO | TB_FILE_BINARY;
	fst->url[0] = '\0';

	return gst;
}

