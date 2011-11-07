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
 * \file		zlib.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "zlib.h"

/* /////////////////////////////////////////////////////////
 * implements
 */
static __tb_inline__ tb_zip_zlib_t* tb_zip_zlib_cast(tb_zip_t* zip)
{
	tb_assert_and_check_return_val(zip && zip->algo == TB_ZIP_ALGO_ZLIB, TB_NULL);
	return (tb_zip_zlib_t*)zip;
}
static tb_void_t tb_zip_zlib_close(tb_zip_t* zip)
{
	tb_zip_zlib_t* zlib = tb_zip_zlib_cast(zip);
	if (zlib) 
	{
		// close zst
		if (zip->action == TB_ZIP_ACTION_INFLATE) inflateEnd(&(zlib->zst));
		else if (zip->action == TB_ZIP_ACTION_DEFLATE) deflateEnd(&(zlib->zst));

		// reset it
		tb_memset(zlib, 0, sizeof(tb_zip_zlib_t));
	}
}
static tb_zip_status_t tb_zip_zlib_spank_deflate(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost)
{
	tb_zip_zlib_t* zlib = tb_zip_zlib_cast(zip);
	tb_assert_and_check_return_val(zlib && ist && ost, TB_ZIP_STATUS_FAIL);

	// the input stream
	tb_byte_t* ip = ist->p;
	tb_byte_t* ie = ist->e;
	tb_assert_and_check_return_val(ip && ie, TB_ZIP_STATUS_FAIL);

	// the output stream
	tb_byte_t* op = ost->p;
	tb_byte_t* oe = ost->e;
	tb_assert_and_check_return_val(op && oe, TB_ZIP_STATUS_FAIL);

	// attach zst
	zlib->zst.next_in = (Bytef*)ip;
	zlib->zst.avail_in = (uInt)(ie - ip);

	zlib->zst.next_out = (Bytef*)op;
	zlib->zst.avail_out = (uInt)(oe - op);

	// deflate 
	tb_int_t ret = deflate(&zlib->zst, Z_NO_FLUSH);
	tb_assert_and_check_return_val(ret == Z_OK || ret == Z_STREAM_END, TB_ZIP_STATUS_FAIL);
	//tb_trace("deflate: %d", zlib->zst.total_out);

	// update 
	ist->p = (tb_byte_t*)zlib->zst.next_in;
	ost->p = (tb_byte_t*)zlib->zst.next_out;

	// ok?
	return (ret == Z_STREAM_END)? TB_ZIP_STATUS_END : TB_ZIP_STATUS_OK;
}
static tb_zip_status_t tb_zip_zlib_spank_inflate(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost)
{
	tb_zip_zlib_t* zlib = tb_zip_zlib_cast(zip);
	tb_assert_and_check_return_val(zlib && ist && ost, TB_ZIP_STATUS_FAIL);

	// the input stream
	tb_byte_t* ip = ist->p;
	tb_byte_t* ie = ist->e;
	tb_assert_and_check_return_val(ip && ie, TB_ZIP_STATUS_FAIL);

	// the output stream
	tb_byte_t* op = ost->p;
	tb_byte_t* oe = ost->e;
	tb_assert_and_check_return_val(op && oe, TB_ZIP_STATUS_FAIL);

	// attach zst
	zlib->zst.next_in = (Bytef*)ip;
	zlib->zst.avail_in = (uInt)(ie - ip);

	zlib->zst.next_out = (Bytef*)op;
	zlib->zst.avail_out = (uInt)(oe - op);

	// inflate 
	tb_int_t ret = inflate(&zlib->zst, Z_NO_FLUSH);
	tb_assert_and_check_return_val(ret == Z_OK || ret == Z_STREAM_END, TB_ZIP_STATUS_FAIL);
	//tb_trace("inflate: %d", zlib->zst.total_out);

	// update 
	ist->p = (tb_byte_t*)zlib->zst.next_in;
	ost->p = (tb_byte_t*)zlib->zst.next_out;

	// ok?
	return (ret == Z_STREAM_END)? TB_ZIP_STATUS_END : TB_ZIP_STATUS_OK;
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_zip_t* tb_zip_zlib_open(tb_zip_zlib_t* zlib, tb_zip_action_t action)
{
	tb_zip_t* zip = (tb_zip_t*)zlib;
	tb_assert_and_check_return_val(zip, TB_NULL);
	
	// init zip
	tb_memset(zip, 0, sizeof(tb_zip_zlib_t));
	zip->algo 		= TB_ZIP_ALGO_ZLIB;
	zip->action 	= action;
	zip->close 		= tb_zip_zlib_close;

	// open zst
	switch (action)
	{
	case TB_ZIP_ACTION_INFLATE:
		{
			zip->spank = tb_zip_zlib_spank_inflate;

			// skip zlib header, only inflate raw data
			if (inflateInit2(&zlib->zst, -MAX_WBITS) != Z_OK) return TB_NULL;
		}
		break;
	case TB_ZIP_ACTION_DEFLATE:
		{
			zip->spank = tb_zip_zlib_spank_deflate;
			if (deflateInit(&zlib->zst, Z_DEFAULT_COMPRESSION) != Z_OK) return TB_NULL;
		}
		break;
	default:
		break;
	}

	return zip;
}
