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
 * @file		nstream.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "filter.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the encoding stream type
typedef struct __tb_nstream_t
{
	// the stream base
	tb_gstream_filter_t 			base;

}tb_nstream_t;

/* ///////////////////////////////////////////////////////////////////////
 * implements
 */

static __tb_inline__ tb_nstream_t* tb_nstream_cast(tb_gstream_t* gstream)
{
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gstream);
	tb_assert_and_check_return_val(filter && filter->type == TB_GSTREAM_FLTR_TYPE_NONE, tb_null);
	return (tb_nstream_t*)filter;
}
static tb_long_t tb_nstream_spak(tb_gstream_t* gstream, tb_long_t sync)
{
	tb_nstream_t* nst = tb_nstream_cast(gstream);
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gstream);
	tb_assert_and_check_return_val(nst && filter, -1);

	// the input
	tb_assert_and_check_return_val(filter->ip, -1);
	tb_byte_t const* 	ib = filter->ip;
	tb_byte_t const* 	ip = filter->ip;
	tb_byte_t const* 	ie = ip + filter->in;
	tb_check_return_val(ip < ie, 0);

	// the output
	tb_assert_and_check_return_val(filter->op, -1);
	tb_byte_t* 			ob = filter->op;
	tb_byte_t* 			op = filter->op;
	tb_byte_t const* 	oe = filter->ob + TB_GSTREAM_FLTR_CACHE_MAXN;
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
	filter->in -= ip - ib;
	filter->ip = ip;

	// update output
	filter->on += op - ob;

	// ok
	return (op - ob);
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_t* tb_gstream_init_filter_null()
{
	// make stream
	tb_gstream_t* gstream = (tb_gstream_t*)tb_malloc0(sizeof(tb_nstream_t));
	tb_assert_and_check_return_val(gstream, tb_null);

	// init stream
	if (!tb_gstream_init(gstream, TB_GSTREAM_TYPE_FLTR)) goto fail;

	// init func
	gstream->open 	= tb_gstream_filter_open;
	gstream->read 	= tb_gstream_filter_read;
	gstream->clos	= tb_gstream_filter_clos;
	gstream->wait	= tb_gstream_filter_wait;
	gstream->ctrl	= tb_gstream_filter_ctrl;

	// init filter
	((tb_gstream_filter_t*)gstream)->type 	= TB_GSTREAM_FLTR_TYPE_NONE;
	((tb_gstream_filter_t*)gstream)->spak = tb_nstream_spak;

	// ok
	return gstream;

fail:
	if (gstream) tb_gstream_exit(gstream);
	return tb_null;
}

tb_gstream_t* tb_gstream_init_filter_from_null(tb_gstream_t* gstream)
{
	tb_assert_and_check_return_val(gstream, tb_null);

	// init the null stream
	tb_gstream_t* nst = tb_gstream_init_filter_null();
	tb_assert_and_check_return_val(nst, tb_null);

	// set gstream
	if (!tb_gstream_ctrl(nst, TB_GSTREAM_CTRL_FLTR_SET_GSTREAM, gstream)) goto fail;
	
	return nst;

fail:
	if (nst) tb_gstream_exit(nst);
	return tb_null;
}
