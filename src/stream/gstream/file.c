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
 * @file		file.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../asio/asio.h"
#include "../../string/string.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the file cache maxn
#define TB_GSTREAM_FILE_CACHE_MAXN 			TB_FILE_DIRECT_CSIZE

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the file stream type
typedef struct __tb_gstream_file_t
{
	// the base
	tb_gstream_t 		base;

	// the file handle
	tb_handle_t 		file;

	// the file handle is referenced? need not exit it
	tb_size_t 			bref;

	// the file mode
	tb_size_t 			mode;

}tb_gstream_file_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_gstream_file_t* tb_gstream_file_cast(tb_gstream_t* gstream)
{
	tb_assert_and_check_return_val(gstream && gstream->type == TB_GSTREAM_TYPE_FILE, tb_null);
	return (tb_gstream_file_t*)gstream;
}
static tb_long_t tb_gstream_file_open(tb_gstream_t* gstream)
{
	// check
	tb_gstream_file_t* fstream = tb_gstream_file_cast(gstream);
	tb_assert_and_check_return_val(fstream && !fstream->file, -1);

	// no reference?
	tb_check_return_val(!(fstream->file && fstream->bref), 1);

	// url
	tb_char_t const* url = tb_url_get(&gstream->url);
	tb_assert_and_check_return_val(url, -1);

	// open file
	fstream->file = tb_file_init(url, fstream->mode);
	tb_assert_and_check_return_val(fstream->file, -1);

	// ok
	return 1;
}
static tb_long_t tb_gstream_file_clos(tb_gstream_t* gstream)
{
	// check
	tb_gstream_file_t* fstream = tb_gstream_file_cast(gstream);
	tb_assert_and_check_return_val(fstream, -1);

	// has file?
	if (fstream->file)
	{
		// exit file
		if (!fstream->bref) if (!tb_file_exit(fstream->file)) return 0;

		// reset
		fstream->file = tb_null;
		fstream->bref = 0;
	}

	// ok
	return 1;
}
static tb_long_t tb_gstream_file_read(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	// check
	tb_gstream_file_t* fstream = tb_gstream_file_cast(gstream);
	tb_assert_and_check_return_val(fstream && fstream->file, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// read 
	return tb_file_read(fstream->file, data, size);
}
static tb_long_t tb_gstream_file_writ(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size, tb_bool_t sync)
{
	// check
	tb_gstream_file_t* fstream = tb_gstream_file_cast(gstream);
	tb_assert_and_check_return_val(fstream && fstream->file, -1);

	// has data
	if (data)
	{
		// check
		tb_check_return_val(size, 0);

		// writ
		return tb_file_writ(fstream->file, data, size);
	}
	
end:
	// sync data
	if (sync) if (!tb_file_sync(fstream->file)) return -1;

	// end?
	return -1;
}
static tb_long_t tb_gstream_file_seek(tb_gstream_t* gstream, tb_hize_t offset)
{
	// check
	tb_gstream_file_t* fstream = tb_gstream_file_cast(gstream);
	tb_assert_and_check_return_val(fstream && fstream->file, -1);

	// seek
	return (tb_file_seek(fstream->file, offset, TB_FILE_SEEK_BEG) == offset)? 1 : -1;
}
static tb_long_t tb_gstream_file_wait(tb_gstream_t* gstream, tb_size_t wait, tb_long_t timeout)
{
	// check
	tb_gstream_file_t* fstream = tb_gstream_file_cast(gstream);
	tb_assert_and_check_return_val(fstream && fstream->file, -1);

	// wait 
	tb_long_t aioe = 0;
	if (tb_gstream_left(gstream))
	{
		if (wait & TB_GSTREAM_WAIT_READ) aioe |= TB_GSTREAM_WAIT_READ;
		if (wait & TB_GSTREAM_WAIT_WRIT) aioe |= TB_GSTREAM_WAIT_WRIT;
	}

	// ok?
	return aioe;
}
static tb_bool_t tb_gstream_file_ctrl(tb_gstream_t* gstream, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_gstream_file_t* fstream = tb_gstream_file_cast(gstream);
	tb_assert_and_check_return_val(fstream, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_GSTREAM_CTRL_GET_SIZE:
		{
			tb_hize_t* psize = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(psize, tb_false);
			*psize = fstream->file? tb_file_size(fstream->file) : 0;
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FILE_SET_MODE:
		{
			fstream->mode = (tb_size_t)tb_va_arg(args, tb_size_t);
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FILE_SET_HANDLE:
		{
			tb_handle_t handle = (tb_handle_t)tb_va_arg(args, tb_handle_t);
			fstream->file = handle;
			fstream->bref = handle? 1 : 0;
			return tb_true;
		}
	case TB_GSTREAM_CTRL_FILE_GET_HANDLE:
		{
			tb_handle_t* phandle = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
			tb_assert_and_check_return_val(phandle, tb_false);
			*phandle = fstream->file;
			return tb_true;
		}
	default:
		break;
	}
	return tb_false;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_t* tb_gstream_init_file()
{
	// make stream
	tb_gstream_file_t* gstream = (tb_gstream_file_t*)tb_malloc0(sizeof(tb_gstream_file_t));
	tb_assert_and_check_return_val(gstream, tb_null);

	// init base
	if (!tb_gstream_init((tb_gstream_t*)gstream, TB_GSTREAM_TYPE_FILE)) goto fail;

	// init stream
	gstream->base.open		= tb_gstream_file_open;
	gstream->base.clos 		= tb_gstream_file_clos;
	gstream->base.read 		= tb_gstream_file_read;
	gstream->base.writ 		= tb_gstream_file_writ;
	gstream->base.seek 		= tb_gstream_file_seek;
	gstream->base.wait 		= tb_gstream_file_wait;
	gstream->base.ctrl 		= tb_gstream_file_ctrl;
	gstream->file 			= tb_null;
	gstream->mode 			= TB_FILE_MODE_RO | TB_FILE_MODE_BINARY;

	// resize file cache
	if (!tb_gstream_ctrl((tb_gstream_t*)gstream, TB_GSTREAM_CTRL_SET_CACHE, TB_GSTREAM_FILE_CACHE_MAXN)) goto fail;

	// ok
	return (tb_gstream_t*)gstream;

fail:
	if (gstream) tb_gstream_exit((tb_gstream_t*)gstream);
	return tb_null;
}

tb_gstream_t* tb_gstream_init_from_file(tb_char_t const* path, tb_size_t mode)
{
	// check
	tb_assert_and_check_return_val(path, tb_null);

	// init file stream
	tb_gstream_t* gstream = tb_gstream_init_file();
	tb_assert_and_check_return_val(gstream, tb_null);

	// set path
	if (!tb_gstream_ctrl(gstream, TB_GSTREAM_CTRL_SET_URL, path)) goto fail;
	
	// set mode
	if (mode) if (!tb_gstream_ctrl(gstream, TB_GSTREAM_CTRL_FILE_SET_MODE, mode)) goto fail;
	
	// ok
	return gstream;
fail:
	if (gstream) tb_gstream_exit(gstream);
	return tb_null;
}
