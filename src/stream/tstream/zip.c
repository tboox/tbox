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
 * \file		zstream.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "tstream.h"
#include "../bstream.h"
#include "../../zip/zip.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the encoding stream type
typedef struct __tb_zstream_t
{
	// the stream base
	tb_tstream_t 			base;

	// the zip package
	tb_zip_package_t 		package;

	// the zip algorithm
	tb_size_t 				algo;

	// the zip action
	tb_size_t 				action;

	// the zip 
	tb_zip_t* 				zip;

}tb_zstream_t;

/* ///////////////////////////////////////////////////////////////////////
 * implements
 */

static __tb_inline__ tb_zstream_t* tb_zstream_cast(tb_gstream_t* gst)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst && tst->type == TB_TSTREAM_TYPE_ZIP, TB_NULL);
	return (tb_zstream_t*)tst;
}
static tb_long_t tb_zstream_aopen(tb_gstream_t* gst)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_assert_and_check_return_val(zst, -1);

	// open zip
	zst->zip = tb_zip_open(&zst->package, zst->algo, zst->action);
	tb_assert_and_check_return_val(zst->zip, -1);

	// open tstream
	return tb_tstream_aopen(gst);
}
static tb_long_t tb_zstream_aclose(tb_gstream_t* gst)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_assert_and_check_return_val(zst, -1);

	// close zip
	if (zst->zip) tb_zip_close(zst->zip);

	// close tstream
	return tb_tstream_aclose(gst);
}
static tb_bool_t tb_zstream_ctrl(tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_assert_and_check_return_val(zst, TB_FALSE);

	// handle it
	switch (cmd)
	{
	case TB_ZSTREAM_CMD_GET_ALGO:
		{
			tb_size_t* pa = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pa, TB_FALSE);
			*pa = zst->algo;
			return TB_TRUE;
		}
	case TB_ZSTREAM_CMD_GET_ACTION:
		{
			tb_size_t* pa = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pa, TB_FALSE);
			*pa = zst->action;
			return TB_TRUE;
		}
	case TB_ZSTREAM_CMD_SET_ALGO:
		{
			zst->algo = (tb_size_t)tb_va_arg(args, tb_size_t);
			return TB_TRUE;
		}
	case TB_ZSTREAM_CMD_SET_ACTION:
		{
			zst->action = (tb_size_t)tb_va_arg(args, tb_size_t);
			return TB_TRUE;
		}
	default:
		break;
	}

	// routine to tstream 
	return tb_tstream_ctrl(gst, cmd, args);
}
static tb_bool_t tb_zstream_spank(tb_gstream_t* gst)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(zst && tst, TB_FALSE);

	// get zip
	tb_zip_t* zip = zst->zip;
	tb_assert_and_check_return_val(zip && zip->spank, TB_FALSE);

	// get input
	tb_assert_and_check_return_val(tst->ip && tst->in, TB_FALSE);
	tb_byte_t const* ip = tst->ip;
	tb_byte_t const* ie = ip + tst->in;

	// get output
	tb_assert_and_check_return_val(tst->op, TB_FALSE);
	tb_byte_t* op = tst->op;
	tb_byte_t* oe = tst->ob + TB_TSTREAM_CACHE_MAXN;

	// attach bstream
	tb_bstream_t ist, ost;
	tb_bstream_attach(&ist, ip, ie - ip);
	tb_bstream_attach(&ost, op, oe - op);

	// spank it
	tb_zip_status_t ret = tb_zip_spank(zip, &ist, &ost);
	if (ret == TB_ZIP_STATUS_FAIL) return TB_FALSE;
	tb_assert_and_check_return_val(ret == TB_ZIP_STATUS_OK || ret == TB_ZIP_STATUS_END, TB_FALSE);
	//while (ip < ie && op < oe) *op++ = *ip++;

	// update pointer
	ip = ist.p;
	op = ost.p;

	// check
	tb_assert_and_check_return_val(ip >= tst->ip && ip <= ie, TB_FALSE);
	tb_assert_and_check_return_val(op >= tst->op && op <= oe, TB_FALSE);

	// update input
	tst->in -= ip - tst->ip;
	tst->ip = ip;

	// update output
	tst->on += op - tst->op;

	// update state
	if (ret == TB_ZIP_STATUS_END) tst->state = TB_TSTREAM_STATE_END;

	return TB_TRUE;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_t* tb_gstream_init_zip()
{
	// create stream
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_zstream_t));
	tb_tstream_t* tst = (tb_tstream_t*)gst;
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init gstream
	gst->type 	= TB_GSTREAM_TYPE_TRAN;
	gst->aopen 	= tb_zstream_aopen;
	gst->aread 	= tb_tstream_aread;
	gst->aclose	= tb_zstream_aclose;
	gst->bare	= tb_tstream_bare;
	gst->wait	= tb_tstream_wait;
	gst->ctrl 	= tb_zstream_ctrl;

	// init tstream
	tst->type 	= TB_TSTREAM_TYPE_ZIP;
	tst->spank 	= tb_zstream_spank;

	return gst;
}
tb_gstream_t* tb_gstream_init_from_zip(tb_gstream_t* gst, tb_size_t algo, tb_size_t action)
{
	tb_assert_and_check_return_val(gst, TB_NULL);

	// create encoding stream
	tb_gstream_t* zst = tb_gstream_init_zip();
	tb_assert_and_check_return_val(zst, TB_NULL);

	// set gstream
	if (!tb_gstream_ctrl(zst, TB_TSTREAM_CMD_SET_GSTREAM, gst)) goto fail;
		
	// set zip algorithm
	if (!tb_gstream_ctrl(zst, TB_ZSTREAM_CMD_SET_ALGO, algo)) goto fail;
		
	// set zip action
	if (!tb_gstream_ctrl(zst, TB_ZSTREAM_CMD_SET_ACTION, action)) goto fail;
	
	return zst;

fail:
	if (zst) tb_gstream_exit(zst);
	return TB_NULL;
}

