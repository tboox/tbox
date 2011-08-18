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
 * \file		gzip.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "gzip.h"

/* /////////////////////////////////////////////////////////
 * implements
 */
static __tb_inline__ tb_zip_gzip_t* tb_zip_gzip_cast(tb_zip_t* zip)
{
	TB_ASSERT_RETURN_VAL(zip && zip->algo == TB_ZIP_ALGO_GZIP, TB_NULL);
	return (tb_zip_gzip_t*)zip;
}
static tb_void_t tb_zip_gzip_close(tb_zip_t* zip)
{
	tb_zip_gzip_t* gzip = tb_zip_gzip_cast(zip);
	if (gzip) 
	{
		// close zst
		if (zip->action == TB_ZIP_ACTION_INFLATE) inflateEnd(&(gzip->zst));
		else if (zip->action == TB_ZIP_ACTION_DEFLATE) deflateEnd(&(gzip->zst));

		// reset it
		tb_memset(gzip, 0, sizeof(tb_zip_gzip_t));
	}
}
static tb_zip_status_t tb_zip_gzip_spank_deflate(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost)
{
	tb_zip_gzip_t* gzip = tb_zip_gzip_cast(zip);
	TB_ASSERT_RETURN_VAL(gzip && ist && ost, TB_ZIP_STATUS_FAIL);

	// the input stream
	tb_byte_t* ip = ist->p;
	tb_byte_t* ie = ist->e;
	TB_ASSERT_RETURN_VAL(ip && ie, TB_ZIP_STATUS_FAIL);

	// the output stream
	tb_byte_t* op = ost->p;
	tb_byte_t* oe = ost->e;
	TB_ASSERT_RETURN_VAL(op && oe, TB_ZIP_STATUS_FAIL);

	// attach zst
	gzip->zst.next_in = (Bytef*)ip;
	gzip->zst.avail_in = (uInt)(ie - ip);

	gzip->zst.next_out = (Bytef*)op;
	gzip->zst.avail_out = (uInt)(oe - op);

	// deflate 
	tb_int_t ret = deflate(&gzip->zst, Z_NO_FLUSH);
	TB_ASSERT_RETURN_VAL(ret == Z_OK || ret == Z_STREAM_END, TB_ZIP_STATUS_FAIL);
	//TB_DBG("deflate: %d", gzip->zst.total_out);

	// update 
	ist->p = (tb_byte_t*)gzip->zst.next_in;
	ost->p = (tb_byte_t*)gzip->zst.next_out;

	// ok?
	return (ret == Z_STREAM_END)? TB_ZIP_STATUS_END : TB_ZIP_STATUS_OK;
}
static tb_zip_status_t tb_zip_gzip_spank_inflate(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost)
{
	tb_zip_gzip_t* gzip = tb_zip_gzip_cast(zip);
	TB_ASSERT_RETURN_VAL(gzip && ist && ost, TB_ZIP_STATUS_FAIL);

	// the input stream
	tb_byte_t* ip = ist->p;
	tb_byte_t* ie = ist->e;
	TB_ASSERT_RETURN_VAL(ip && ie, TB_ZIP_STATUS_FAIL);

	// the output stream
	tb_byte_t* op = ost->p;
	tb_byte_t* oe = ost->e;
	TB_ASSERT_RETURN_VAL(op && oe, TB_ZIP_STATUS_FAIL);

	// attach zst
	gzip->zst.next_in = (Bytef*)ip;
	gzip->zst.avail_in = (uInt)(ie - ip);

	gzip->zst.next_out = (Bytef*)op;
	gzip->zst.avail_out = (uInt)(oe - op);

	// inflate 
	tb_int_t ret = inflate(&gzip->zst, Z_NO_FLUSH);
	TB_ASSERT_RETURN_VAL(ret == Z_OK || ret == Z_STREAM_END, TB_ZIP_STATUS_FAIL);
	//TB_DBG("inflate: %d", gzip->zst.total_out);

	// update 
	ist->p = (tb_byte_t*)gzip->zst.next_in;
	ost->p = (tb_byte_t*)gzip->zst.next_out;

	// ok?
	return (ret == Z_STREAM_END)? TB_ZIP_STATUS_END : TB_ZIP_STATUS_OK;
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_zip_t* tb_zip_gzip_open(tb_zip_gzip_t* gzip, tb_zip_action_t action)
{
	tb_zip_t* zip = (tb_zip_t*)gzip;
	TB_ASSERT_RETURN_VAL(zip, TB_NULL);
	
	// init zip
	tb_memset(zip, 0, sizeof(tb_zip_gzip_t));
	zip->algo 		= TB_ZIP_ALGO_GZIP;
	zip->action 	= action;
	zip->close 		= tb_zip_gzip_close;

	// open zst
	switch (action)
	{
	case TB_ZIP_ACTION_INFLATE:
		{
			zip->spank = tb_zip_gzip_spank_inflate;
			if (inflateInit2(&gzip->zst, 47) != Z_OK) return TB_NULL;
		}
		break;
	case TB_ZIP_ACTION_DEFLATE:
		{
			zip->spank = tb_zip_gzip_spank_deflate;

			// only gzip data, no gzip header, no crc32
			if (deflateInit2(&gzip->zst, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) return TB_NULL;
		}
		break;
	default:
		break;
	}

	return zip;
}
