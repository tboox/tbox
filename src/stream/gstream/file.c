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
 * \author		ruki
 * \file		file.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../aio/aio.h"
#include "../../string/string.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// need larger cache for performance
#define TB_FSTREAM_MCACHE_DEFAULT 		(8192 << 2)

/* ///////////////////////////////////////////////////////////////////////
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
	tb_hize_t 			size;

	// the wait event
	tb_long_t 			wait;

	// the file flags
	tb_size_t 			flags;

}tb_fstream_t;


/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_fstream_t* tb_fstream_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_FILE, TB_NULL);
	return (tb_fstream_t*)gst;
}
static tb_long_t tb_fstream_aopen(tb_gstream_t* gst)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && !fst->file, -1);

	// url
	tb_char_t const* url = tb_url_get(&gst->url);
	tb_assert_and_check_return_val(url, -1);

	// open file
	fst->file = tb_file_init(url, fst->flags);
	tb_assert_and_check_return_val(fst->file, -1);

	// init size
	fst->size = tb_file_size(fst->file);
	fst->wait = 0;
	
	// ok
	return 1;
}
static tb_long_t tb_fstream_aclose(tb_gstream_t* gst)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst, -1);

	if (fst->file)
	{
		// close file
		if (!tb_file_exit(fst->file)) return 0;

		// clear 
		fst->file = TB_NULL;
		fst->size = 0;
		fst->wait = 0;
	}

	// ok
	return 1;
}
static tb_long_t tb_fstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// read 
	tb_long_t r = tb_file_read(fst->file, data, size);
	tb_check_return_val(r >= 0, -1);

	// abort?
	if (!r && fst->wait > 0 && (fst->wait & TB_AIOO_ETYPE_READ)) return -1;

	// clear wait
	if (r > 0) fst->wait = 0;

	// ok?
	return r;
}
static tb_long_t tb_fstream_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file, -1);

	// has data
	if (data)
	{
		// check
		tb_check_return_val(size, 0);

		// writ
		tb_long_t r = tb_file_writ(fst->file, data, size);
		tb_check_goto(r >= 0, end);

		// abort?
		if (!r && fst->wait > 0 && (fst->wait & TB_AIOO_ETYPE_WRIT)) goto end;

		// clear wait
		if (r > 0) fst->wait = 0;

		// ok?
		return r;
	}
	
end:
	// sync data
	if (sync) tb_file_sync(fst->file);

	// end?
	return -1;
}
static tb_long_t tb_fstream_aseek(tb_gstream_t* gst, tb_hize_t offset)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file, -1);

	// seek
	return (tb_file_seek(fst->file, offset))? 1 : -1;
}
static tb_hize_t tb_fstream_size(tb_gstream_t* gst)
{	
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file, 0);
	return fst->size;
}
static tb_long_t tb_fstream_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst && fst->file, -1);

	// aioo
	tb_aioo_t o;
	tb_aioo_seto(&o, fst->file, TB_AIOO_OTYPE_FILE, etype, TB_NULL);

	// wait
	fst->wait = tb_aioo_wait(&o, timeout);

	// ok?
	return fst->wait;
}
static tb_bool_t tb_fstream_ctrl(tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args)
{
	tb_fstream_t* fst = tb_fstream_cast(gst);
	tb_assert_and_check_return_val(fst, TB_FALSE);

	switch (cmd)
	{
	case TB_FSTREAM_CMD_SET_FLAGS:
		fst->flags = (tb_size_t)tb_va_arg(args, tb_size_t);
		return TB_TRUE;
	default:
		break;
	}
	return TB_FALSE;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_t* tb_gstream_init_file()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_malloc0(sizeof(tb_fstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init stream
	tb_fstream_t* fst = (tb_fstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_FILE;
	gst->aopen 	= tb_fstream_aopen;
	gst->aclose = tb_fstream_aclose;
	gst->aread 	= tb_fstream_aread;
	gst->awrit 	= tb_fstream_awrit;
	gst->aseek 	= tb_fstream_aseek;
	gst->size 	= tb_fstream_size;
	gst->wait 	= tb_fstream_wait;
	gst->ctrl 	= tb_fstream_ctrl;
	fst->file 	= TB_NULL;
	fst->flags 	= TB_FILE_RO | TB_FILE_BINARY;

	// resize file cache
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_CACHE, TB_FSTREAM_MCACHE_DEFAULT)) goto fail;

	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
}

tb_gstream_t* tb_gstream_init_from_file(tb_char_t const* path)
{
	tb_assert_and_check_return_val(path, TB_NULL);

	// init file stream
	tb_gstream_t* gst = tb_gstream_init_file();
	tb_assert_and_check_return_val(gst, TB_NULL);

	// set path
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_URL, path)) goto fail;
	
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
}
