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
	tb_assert_and_check_return_val(zst && !zst->zip, -1);

	// open zip
	zst->zip = tb_zip_init(zst->algo, zst->action);
	tb_assert_and_check_return_val(zst->zip, -1);

	// open tstream
	return tb_tstream_aopen(gst);
}
static tb_long_t tb_zstream_aclose(tb_gstream_t* gst)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_assert_and_check_return_val(zst, -1);

	// close zip
	if (zst->zip) 
	{
		tb_zip_exit(zst->zip);
		zst->zip = TB_NULL;
	}

	// close tstream
	return tb_tstream_aclose(gst);
}
static tb_bool_t tb_zstream_ctrl(tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_assert_and_check_return_val(zst, TB_FALSE);

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
static tb_long_t tb_zstream_spak(tb_gstream_t* gst)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(zst && zst->zip && tst, -1);

	// the input
	tb_assert_and_check_return_val(tst->ip, -1);
	tb_byte_t const* 	ib = tst->ip;
	tb_byte_t const* 	ip = tst->ip;
	tb_byte_t const* 	ie = ip + tst->in;
	tb_check_return_val(ip < ie, 0);

	// the output
	tb_assert_and_check_return_val(tst->op, -1);
	tb_byte_t* 			ob = tst->op;
	tb_byte_t* 			op = tst->op;
	tb_byte_t const* 	oe = tst->ob + TB_TSTREAM_CACHE_MAXN;
	tb_check_return_val(op < oe, 0);

	// attach bstream
	tb_bstream_t ist, ost;
	tb_bstream_attach(&ist, ip, ie - ip);
	tb_bstream_attach(&ost, op, oe - op);

	// spak it
	tb_long_t r = tb_zip_spak(zst->zip, &ist, &ost);
	tb_check_return_val(r >= 0, -1);

	// update pointer
	ip = ist.p;
	op = ost.p;

	// check
	tb_assert_and_check_return_val(ip >= ib && ip <= ie, -1);
	tb_assert_and_check_return_val(op >= ob && op <= oe, -1);

	// update input
	tst->in -= ip - ib;
	tst->ip = ip;

	// update output
	tst->on += op - ob;

	// ok
	return (op - ob);
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_t* tb_gstream_init_zip()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_zstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init gstream
	gst->type 	= TB_GSTREAM_TYPE_TRAN;
	gst->aopen 	= tb_zstream_aopen;
	gst->aread 	= tb_tstream_aread;
	gst->awrit 	= tb_tstream_awrit;
	gst->aclose	= tb_zstream_aclose;
	gst->bare	= tb_tstream_bare;
	gst->wait	= tb_tstream_wait;
	gst->ctrl 	= tb_zstream_ctrl;

	// init tstream
	((tb_tstream_t*)gst)->type 	= TB_TSTREAM_TYPE_ZIP;
	((tb_tstream_t*)gst)->spak = tb_zstream_spak;

	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
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
	
	// ok
	return zst;

fail:
	if (zst) tb_gstream_exit(zst);
	return TB_NULL;
}

