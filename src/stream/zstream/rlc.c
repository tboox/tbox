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
#include "rlc.h"


/* /////////////////////////////////////////////////////////
 * inflate
 */
static tb_bstream_t* tb_zstream_inflate_rlc_transform(tb_tstream_t* st)
{
	tb_rlc_inflate_zstream_t* zst = (tb_rlc_inflate_zstream_t*)st;
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// get dst
	tb_byte_t* dp = st->dst.p;
	tb_byte_t* de = st->dst.e;
	TB_ASSERT(dp && de);
	if (!dp || !de) return TB_NULL;

	// get src
	tb_bstream_t* src = tb_tstream_src(st);

	// get vlc
	tb_zstream_vlc_t* vlc = zst->vlc;
	TB_ASSERT(vlc && vlc->get);

	// vlc callback
	tb_zstream_vlc_get_t vlc_get = vlc->get;

	// inflate 
	while (tb_bstream_left_bits(src) > 8 && (dp < de))
	{
		// get flag
		if (tb_bstream_get_u1(src))
		{
			// get repeat
			tb_size_t repeat = vlc_get(vlc, src);

			// get value
			tb_byte_t val = tb_bstream_get_ubits(src, 8);

			//TB_DBG("repeat(0x%02x): %d", val, repeat);

			// set value
			while (repeat--) *dp++ = val;
	
		}
		else *dp++ = tb_bstream_get_ubits(src, 8);
	}

	// update position
	st->dst.p = dp;
	return tb_tstream_dst(st);
}
static void tb_zstream_inflate_rlc_close(tb_tstream_t* st)
{
	tb_rlc_inflate_zstream_t* zst = (tb_rlc_inflate_zstream_t*)st;
	if (zst) 
	{
		// close vlc
		if (zst->vlc && zst->vlc->close) zst->vlc->close(zst->vlc); 

		// reset it
		memset(zst, 0, sizeof(tb_rlc_inflate_zstream_t));
	}
}
/* /////////////////////////////////////////////////////////
 * deflate
 */
static tb_bstream_t* tb_zstream_deflate_rlc_transform(tb_tstream_t* st)
{
	tb_rlc_deflate_zstream_t* zst = (tb_rlc_deflate_zstream_t*)st;
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// get src
	tb_byte_t* sp = st->src.p;
	tb_byte_t* se = st->src.e;
	TB_ASSERT(sp && se);
	if (!sp || !se) return TB_NULL;

	// get dst
	tb_bstream_t* dst = tb_tstream_dst(st);

	// get vlc
	tb_zstream_vlc_t* vlc = zst->vlc;
	TB_ASSERT(vlc && vlc->set);

	// vlc callback
	tb_zstream_vlc_set_t vlc_set = vlc->set;

	// deflate 
	tb_byte_t last = *sp++;
	tb_size_t repeat = 1;
	for (; sp <= se; sp++)
	{
		// update repeat
		if (*sp != last || sp == se) 
		{
			if (repeat > 1)
			{
				// set flag
				tb_bstream_set_u1(dst, 1);

				// set repeat
				vlc_set(vlc, repeat, dst);
				
				// set value
				tb_bstream_set_ubits(dst, last, 8);

				//TB_DBG("repeat(0x%02x): %d", last, repeat);
			}
			else
			{
				TB_ASSERT(repeat == 1);

				// set flag
				tb_bstream_set_u1(dst, 0);

				// set value
				tb_bstream_set_ubits(dst, last, 8);
			}

			// is end?
			if (sp == se) break;

			// reset
			last = *sp;
			repeat = 1;
		}
		else repeat++;
	}

	// sync 
	tb_bstream_sync(dst);

	// update position
	st->src.p = sp;
	return dst;
}
static void tb_zstream_deflate_rlc_close(tb_tstream_t* st)
{
	tb_rlc_deflate_zstream_t* zst = (tb_rlc_deflate_zstream_t*)st;
	if (zst) 
	{
		// close vlc
		if (zst->vlc && zst->vlc->close) zst->vlc->close(zst->vlc); 

		// reset it
		memset(zst, 0, sizeof(tb_rlc_deflate_zstream_t));
	}
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_tstream_t* tb_zstream_open_rlc_inflate(tb_rlc_inflate_zstream_t* zst)
{
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// init 
	memset(zst, 0, sizeof(tb_rlc_inflate_zstream_t));

	// init tstream
	((tb_tstream_t*)zst)->transform = tb_zstream_inflate_rlc_transform;
	((tb_tstream_t*)zst)->close 	= tb_zstream_inflate_rlc_close;

	// init zstream
	((tb_zstream_t*)zst)->algo 		= TB_ZSTREAM_ALGO_RLC;
	((tb_zstream_t*)zst)->action 	= TB_ZSTREAM_ACTION_INFLATE;

	// open vlc
#if TB_RLC_VLC_TYPE_GOLOMB
	zst->vlc = tb_zstream_vlc_golomb_open(&(((tb_zstream_t*)zst)->vlc), 4);
#elif TB_RLC_VLC_TYPE_GAMMA
	zst->vlc = tb_zstream_vlc_gamma_open(&(((tb_zstream_t*)zst)->vlc));
#else
	zst->vlc = tb_zstream_vlc_fixed_open(&(((tb_zstream_t*)zst)->vlc), 16);
#endif

	// check vlc
	TB_ASSERT(zst->vlc);
	if (!zst->vlc) return TB_NULL;

	return ((tb_tstream_t*)zst);
}
tb_tstream_t* tb_zstream_open_rlc_deflate(tb_rlc_deflate_zstream_t* zst)
{
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// init 
	memset(zst, 0, sizeof(tb_rlc_deflate_zstream_t));

	// init tstream
	((tb_tstream_t*)zst)->transform = tb_zstream_deflate_rlc_transform;
	((tb_tstream_t*)zst)->close 	= tb_zstream_deflate_rlc_close;

	// init zstream
	((tb_zstream_t*)zst)->algo 		= TB_ZSTREAM_ALGO_RLC;
	((tb_zstream_t*)zst)->action 	= TB_ZSTREAM_ACTION_DEFLATE;

	// open vlc
#if TB_RLC_VLC_TYPE_GOLOMB
	zst->vlc = tb_zstream_vlc_golomb_open(&(((tb_zstream_t*)zst)->vlc), 4);
#elif TB_RLC_VLC_TYPE_GAMMA
	zst->vlc = tb_zstream_vlc_gamma_open(&(((tb_zstream_t*)zst)->vlc));
#else
	zst->vlc = tb_zstream_vlc_fixed_open(&(((tb_zstream_t*)zst)->vlc), 16);
#endif

	// check vlc
	TB_ASSERT(zst->vlc);
	if (!zst->vlc) return TB_NULL;


	return ((tb_tstream_t*)zst);
}

tb_tstream_t* tb_zstream_open_rlc(tb_rlc_zstream_t* zst, tb_size_t action)
{
	if (action == TB_ZSTREAM_ACTION_INFLATE)
		return tb_zstream_open_rlc_inflate((tb_rlc_inflate_zstream_t*)zst);
	else if (action == TB_ZSTREAM_ACTION_DEFLATE)
		return tb_zstream_open_rlc_deflate((tb_rlc_deflate_zstream_t*)zst);
	else return TB_NULL;
}
