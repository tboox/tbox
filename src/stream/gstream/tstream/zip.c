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
 * @file		zstream.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "tstream.h"
#include "../../bstream.h"
#include "../../../zip/zip.h"

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
	// check
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst && tst->type == TB_TSTREAM_TYPE_ZIP, tb_null);
	return (tb_zstream_t*)tst;
}
static tb_long_t tb_zstream_aopen(tb_gstream_t* gst)
{
	// check
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
	// check
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_assert_and_check_return_val(zst, -1);

	// close zip
	if (zst->zip) 
	{
		tb_zip_exit(zst->zip);
		zst->zip = tb_null;
	}

	// close tstream
	return tb_tstream_aclose(gst);
}
static tb_bool_t tb_zstream_ctrl(tb_gstream_t* gst, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_assert_and_check_return_val(zst, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_ZSTREAM_CTRL_GET_ALGO:
		{
			tb_size_t* pa = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pa, tb_false);
			*pa = zst->algo;
			return tb_true;
		}
	case TB_ZSTREAM_CTRL_GET_ACTION:
		{
			tb_size_t* pa = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pa, tb_false);
			*pa = zst->action;
			return tb_true;
		}
	case TB_ZSTREAM_CTRL_SET_ALGO:
		{
			zst->algo = (tb_size_t)tb_va_arg(args, tb_size_t);
			return tb_true;
		}
	case TB_ZSTREAM_CTRL_SET_ACTION:
		{
			zst->action = (tb_size_t)tb_va_arg(args, tb_size_t);
			return tb_true;
		}
	default:
		break;
	}

	// routine to tstream 
	return tb_tstream_ctrl(gst, ctrl, args);
}
static tb_long_t tb_zstream_spak(tb_gstream_t* gst, tb_bool_t sync)
{
	// check
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(zst && zst->zip && tst, -1);

	// the input
	tb_assert_and_check_return_val(tst->ip, -1);
	tb_byte_t const* 	ib = tst->ip;
	tb_byte_t const* 	ip = tst->ip;
	tb_byte_t const* 	ie = ip + tst->in;
	tb_check_return_val(ip < ie || sync, 0);

	// the output
	tb_assert_and_check_return_val(tst->op, -1);
	tb_byte_t* 			ob = tst->op;
	tb_byte_t* 			op = tst->op;
	tb_byte_t const* 	oe = tst->ob + TB_TSTREAM_CACHE_MAXN;
	tb_check_return_val(op < oe, 0);

	// attach bstream
	tb_bstream_t ist, ost;
	tb_bstream_init(&ist, ip, ie - ip);
	tb_bstream_init(&ost, op, oe - op);

	// spak it
	tb_long_t r = tb_zip_spak(zst->zip, &ist, &ost, sync);
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
	// check
	tb_gstream_t* gst = (tb_gstream_t*)tb_malloc0(sizeof(tb_zstream_t));
	tb_assert_and_check_return_val(gst, tb_null);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init gstream
	gst->type 	= TB_GSTREAM_TYPE_TRAN;
	gst->aopen 	= tb_zstream_aopen;
	gst->aread 	= tb_tstream_aread;
	gst->aclose	= tb_zstream_aclose;
	gst->wait	= tb_tstream_wait;
	gst->ctrl 	= tb_zstream_ctrl;

	// init tstream
	((tb_tstream_t*)gst)->type 	= TB_TSTREAM_TYPE_ZIP;
	((tb_tstream_t*)gst)->spak = tb_zstream_spak;

	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return tb_null;
}
tb_gstream_t* tb_gstream_init_from_zip(tb_gstream_t* gst, tb_size_t algo, tb_size_t action)
{
	// check
	tb_assert_and_check_return_val(gst, tb_null);

	// create encoding stream
	tb_gstream_t* zst = tb_gstream_init_zip();
	tb_assert_and_check_return_val(zst, tb_null);

	// set gstream
	if (!tb_gstream_ctrl(zst, TB_TSTREAM_CTRL_SET_GSTREAM, gst)) goto fail;
		
	// set zip algorithm
	if (!tb_gstream_ctrl(zst, TB_ZSTREAM_CTRL_SET_ALGO, algo)) goto fail;
		
	// set zip action
	if (!tb_gstream_ctrl(zst, TB_ZSTREAM_CTRL_SET_ACTION, action)) goto fail;
	
	// ok
	return zst;

fail:
	if (zst) tb_gstream_exit(zst);
	return tb_null;
}

