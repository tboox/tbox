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
 * \file		nstream.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "tstream.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the encoding stream type
typedef struct __tb_nstream_t
{
	// the stream base
	tb_tstream_t 			base;

}tb_nstream_t;

/* ///////////////////////////////////////////////////////////////////////
 * implements
 */

static __tb_inline__ tb_nstream_t* tb_nstream_cast(tb_gstream_t* gst)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(tst && tst->type == TB_TSTREAM_TYPE_NULL, TB_NULL);
	return (tb_nstream_t*)tst;
}
static tb_long_t tb_nstream_spak(tb_gstream_t* gst, tb_bool_t sync)
{
	tb_nstream_t* nst = tb_nstream_cast(gst);
	tb_tstream_t* tst = tb_tstream_cast(gst);
	tb_assert_and_check_return_val(nst && tst, -1);

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

	// spak it
	tb_size_t n = tb_min(ie - ip, oe - op);
	tb_memcpy(op, ip, n);
	ip += n;
	op += n;

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
tb_gstream_t* tb_gstream_init_null()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_nstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init gstream
	gst->type 	= TB_GSTREAM_TYPE_TRAN;
	gst->aopen 	= tb_tstream_aopen;
	gst->aread 	= tb_tstream_aread;
	gst->aclose	= tb_tstream_aclose;
	gst->wait	= tb_tstream_wait;
	gst->ctrl	= tb_tstream_ctrl;

	// init tstream
	((tb_tstream_t*)gst)->type 	= TB_TSTREAM_TYPE_NULL;
	((tb_tstream_t*)gst)->spak = tb_nstream_spak;

	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
}

tb_gstream_t* tb_gstream_init_from_null(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init the null stream
	tb_gstream_t* nst = tb_gstream_init_null();
	tb_assert_and_check_return_val(nst, TB_NULL);

	// set gstream
	if (!tb_gstream_ctrl(nst, TB_TSTREAM_CMD_SET_GSTREAM, gst)) goto fail;
	
	return nst;

fail:
	if (nst) tb_gstream_exit(nst);
	return TB_NULL;
}
