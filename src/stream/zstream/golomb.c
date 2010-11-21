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
 * \file		zstream.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "lz77.h"

/* /////////////////////////////////////////////////////////
 * inflate
 */
static tb_bstream_t* tb_zstream_inflate_lz77_transform(tb_tstream_t* st)
{
	tb_lz77_inflate_zstream_t* zst = (tb_lz77_inflate_zstream_t*)st;
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// get src & dst
	tb_byte_t* sp = st->src.p;
	tb_byte_t* se = st->src.e;

	tb_byte_t* dp = st->dst.p;
	tb_byte_t* de = st->dst.e;

	// check data
	TB_ASSERT(sp && se && dp && de);
	if (!sp || !se || !dp || !de) return TB_NULL;














	// update position
	st->src.p = sp;
	st->dst.p = dp;

	return tb_tstream_dst(st);
}
static void tb_zstream_inflate_lz77_close(tb_tstream_t* st)
{
	if (st) memset(st, 0, sizeof(tb_lz77_inflate_zstream_t));
}
/* /////////////////////////////////////////////////////////
 * deflate
 */
static tb_bstream_t* tb_zstream_deflate_lz77_transform(tb_tstream_t* st)
{
	tb_lz77_deflate_zstream_t* zst = (tb_lz77_deflate_zstream_t*)st;
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// get src & dst
	tb_byte_t* sp = st->src.p;
	tb_byte_t* se = st->src.e;

	tb_byte_t* dp = st->dst.p;
	tb_byte_t* de = st->dst.e;

	// check data
	TB_ASSERT(sp && se && dp && de);
	if (!sp || !se || !dp || !de) return TB_NULL;











	// update position
	st->src.p = sp;
	st->dst.p = dp;

	return tb_tstream_dst(st);
}
static void tb_zstream_deflate_lz77_close(tb_tstream_t* st)
{
	if (st) memset(st, 0, sizeof(tb_lz77_deflate_zstream_t));
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_tstream_t* tb_zstream_open_lz77_inflate(tb_lz77_inflate_zstream_t* zst)
{
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// init 
	memset(zst, 0, sizeof(tb_lz77_inflate_zstream_t));

	// init tstream
	((tb_tstream_t*)zst)->transform = tb_zstream_inflate_lz77_transform;
	((tb_tstream_t*)zst)->close 	= tb_zstream_inflate_lz77_close;

	// init zstream
	((tb_zstream_t*)zst)->algo 		= TB_ZSTREAM_ALGO_LZ77;
	((tb_zstream_t*)zst)->action 	= TB_ZSTREAM_ACTION_INFLATE;


	return ((tb_tstream_t*)zst);
}
tb_tstream_t* tb_zstream_open_lz77_deflate(tb_lz77_deflate_zstream_t* zst)
{
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// init 
	memset(zst, 0, sizeof(tb_lz77_deflate_zstream_t));

	// init tstream
	((tb_tstream_t*)zst)->transform = tb_zstream_deflate_lz77_transform;
	((tb_tstream_t*)zst)->close 	= tb_zstream_deflate_lz77_close;

	// init zstream
	((tb_zstream_t*)zst)->algo 		= TB_ZSTREAM_ALGO_LZ77;
	((tb_zstream_t*)zst)->action 	= TB_ZSTREAM_ACTION_DEFLATE;

	return ((tb_tstream_t*)zst);
}

tb_tstream_t* tb_zstream_open_lz77(tb_lz77_zstream_t* zst, tb_size_t action)
{
	if (action == TB_ZSTREAM_ACTION_INFLATE)
		return tb_zstream_open_lz77_inflate((tb_lz77_inflate_zstream_t*)zst);
	else if (action == TB_ZSTREAM_ACTION_DEFLATE)
		return tb_zstream_open_lz77_deflate((tb_lz77_deflate_zstream_t*)zst);
	else return TB_NULL;
}
