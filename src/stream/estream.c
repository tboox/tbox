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
 * \file		estream.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "estream.h"

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_bstream_t* tb_estream_transform(tb_tstream_t* st)
{
	tb_estream_t* est = (tb_estream_t*)st;
	TB_ASSERT(est);
	if (!est) return TB_NULL;

	// get the encoding converters
	tb_encoding_converter_t const* src_c = est->src_c;
	tb_encoding_converter_t const* dst_c = est->dst_c;
	TB_ASSERT(src_c && dst_c && src_c->get && dst_c->set);

	// get src & dst
	tb_byte_t* sp = st->src.p;
	tb_byte_t* se = st->src.e;

	tb_byte_t* dp = st->dst.p;
	tb_byte_t* de = st->dst.e;

	// check data
	TB_ASSERT(sp && se && dp && de);
	if (!sp || !se || !dp || !de) return TB_NULL;

	// convert
	while (sp < se && dp < de) dst_c->set(src_c->get(&sp), &dp);

	// update position
	st->src.p = sp;
	st->dst.p = dp;

	return tb_tstream_dst(st);
}
static void tb_estream_close(tb_tstream_t* st)
{
	if (st) memset(st, 0, sizeof(tb_estream_t));
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_tstream_t* tb_estream_open(tb_estream_t* est, tb_encoding_t src_e, tb_encoding_t dst_e)
{
	TB_ASSERT(est);
	if (!est) return TB_NULL;

	// init 
	memset(est, 0, sizeof(tb_estream_t));

	// get converter
	est->src_c = tb_encoding_get_converter(src_e);
	est->dst_c = tb_encoding_get_converter(dst_e);

	// check converter
	TB_ASSERT(est->src_c && est->dst_c);
	if (!est->src_c || !est->dst_c) return TB_NULL;

	TB_ASSERT(est->src_c->get && est->dst_c->set);
	if (!est->src_c->get || !est->dst_c->set) return TB_NULL;

	// attach callback
	est->base.transform = tb_estream_transform;
	est->base.close 	= tb_estream_close;

	return ((tb_tstream_t*)est);
}

