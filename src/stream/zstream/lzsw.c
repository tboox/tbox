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
#include "lzsw.h"
#include "../../math/math.h"

/* /////////////////////////////////////////////////////////
 * types
 */
// the window type
typedef struct __tb_zstream_lzsw_window_t
{
	// the window range
	tb_byte_t const* 	e;
	tb_size_t 			n;
	tb_size_t 			b;

}tb_zstream_lzsw_window_t;

/* /////////////////////////////////////////////////////////
 * details
 */

tb_size_t tb_zstream_lzsw_window_find_max_match(tb_zstream_lzsw_window_t* window, tb_byte_t const* sp, tb_byte_t const* se, tb_size_t* p)
{
	tb_byte_t const* wb = window->e - window->n;
	tb_byte_t const* wp = wb;
	tb_byte_t const* we = window->e;
	tb_byte_t const* mp = wp;
	tb_byte_t const* me = wp;
	for (; wp < we; wp++)
	{
		tb_byte_t const* wq = wp;
		tb_byte_t const* sq = sp;
		for (; wq < we && sq < se && *wq == *sq; wq++, sq++);
		if ((wq - wp) > (me - mp))
		{
			mp = wp;
			me = wq;
		}
	}

	*p = mp - wb;
	return (me - mp);
}

/* /////////////////////////////////////////////////////////
 * inflate
 */
static tb_bstream_t* tb_zstream_inflate_lzsw_transform(tb_tstream_t* st)
{
	tb_lzsw_inflate_zstream_t* zst = (tb_lzsw_inflate_zstream_t*)st;
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// get dst
	tb_byte_t* dp = st->dst.p;
	tb_byte_t* de = st->dst.e;
	tb_byte_t* db = dp;
	TB_ASSERT(dp && de);
	if (!dp || !de) return TB_NULL;

	// get src
	tb_bstream_t* src = tb_tstream_src(st);

	// get vlc
	tb_zstream_vlc_t* vlc = zst->vlc;
	TB_ASSERT(vlc && vlc->get);

	// vlc callback
	tb_zstream_vlc_get_t vlc_get = vlc->get;

	// init window
	tb_zstream_lzsw_window_t window;
	window.e = dp;
	window.n = 0;
	window.b = 0;

	// inflate 
	while (tb_bstream_left_bits(src) > 8 && (dp < de))
	{
		// get flag
		if (tb_bstream_get_u1(src))
		{
			// set position
			tb_size_t p = tb_bstream_get_ubits(src, window.b);
			
			// get size
			tb_size_t n = vlc_get(vlc, src);

			//TB_DBG("%d %d", p, n);
#if 1 
			// fill data
			// \note: address maybe overlap
			memcpy(dp, window.e + p - window.n, n);

			// update dp
			dp += n;
#else
			tb_byte_t const* wp = window.e + p - window.n;
			while (n--) *dp++ = *wp++;
#endif

		}
		else 
		{
			*dp++ = tb_bstream_get_ubits(src, 8);
			*dp++ = tb_bstream_get_ubits(src, 8);
		}

		// update window
		window.e = dp;
		if (dp - db <= TB_LZSW_WINDOW_SIZE_MAX) 
		{
			window.n = dp - db;
			window.b = TB_MATH_ICLOG2I(window.n);
		}
	}

	// update position
	st->dst.p = dp;
	return tb_tstream_dst(st);
}
static void tb_zstream_inflate_lzsw_close(tb_tstream_t* st)
{
	tb_lzsw_inflate_zstream_t* zst = (tb_lzsw_inflate_zstream_t*)st;
	if (zst) 
	{
		// close vlc
		if (zst->vlc && zst->vlc->close) zst->vlc->close(zst->vlc); 

		// reset it
		memset(zst, 0, sizeof(tb_lzsw_inflate_zstream_t));
	}
}
/* /////////////////////////////////////////////////////////
 * deflate
 */
static tb_bstream_t* tb_zstream_deflate_lzsw_transform(tb_tstream_t* st)
{
	tb_lzsw_deflate_zstream_t* zst = (tb_lzsw_deflate_zstream_t*)st;
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// get src
	tb_byte_t* sp = st->src.p;
	tb_byte_t* se = st->src.e;
	tb_byte_t* sb = sp;
	TB_ASSERT(sp && se);
	if (!sp || !se) return TB_NULL;

	// get dst
	tb_bstream_t* dst = tb_tstream_dst(st);

	// get vlc
	tb_zstream_vlc_t* vlc = zst->vlc;
	TB_ASSERT(vlc && vlc->set);

	// vlc callback
	tb_zstream_vlc_set_t vlc_set = vlc->set;

	// init window
	tb_zstream_lzsw_window_t window;
	window.e = sp;
	window.n = 0;
	window.b = 0;

	// deflate 
	while (sp < se)
	{
		tb_size_t p = 0;
		tb_size_t n = tb_zstream_lzsw_window_find_max_match(&window, sp, se, &p);
		if (n > 2)
		{
			// set flag
			tb_bstream_set_u1(dst, 1);

			// set position
			tb_bstream_set_ubits(dst, p, window.b);
			
			// set size
			vlc_set(vlc, n, dst);

			// update sp
			sp += n;

			//TB_DBG("%d %d", p, n);
		}
		else
		{
			// set flag
			tb_bstream_set_u1(dst, 0);

			// set value
			tb_bstream_set_ubits(dst, *sp++, 8);
			tb_bstream_set_ubits(dst, *sp++, 8);
		}

		// update window
		window.e = sp;
		if (sp - sb <= TB_LZSW_WINDOW_SIZE_MAX) 
		{
			window.n = sp - sb;
			window.b = TB_MATH_ICLOG2I(window.n);
		}
	}

	// sync 
	tb_bstream_sync(dst);

	// update position
	st->src.p = sp;
	return dst;
}
static void tb_zstream_deflate_lzsw_close(tb_tstream_t* st)
{
	tb_lzsw_deflate_zstream_t* zst = (tb_lzsw_deflate_zstream_t*)st;
	if (zst) 
	{
		// close vlc
		if (zst->vlc && zst->vlc->close) zst->vlc->close(zst->vlc); 

		// reset it
		memset(zst, 0, sizeof(tb_lzsw_deflate_zstream_t));
	}
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_tstream_t* tb_zstream_open_lzsw_inflate(tb_lzsw_inflate_zstream_t* zst)
{
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// init 
	memset(zst, 0, sizeof(tb_lzsw_inflate_zstream_t));

	// init tstream
	((tb_tstream_t*)zst)->transform = tb_zstream_inflate_lzsw_transform;
	((tb_tstream_t*)zst)->close 	= tb_zstream_inflate_lzsw_close;

	// init zstream
	((tb_zstream_t*)zst)->algo 		= TB_ZSTREAM_ALGO_LZSW;
	((tb_zstream_t*)zst)->action 	= TB_ZSTREAM_ACTION_INFLATE;

	// open vlc
#if TB_LZSW_VLC_TYPE_GOLOMB
	zst->vlc = tb_zstream_vlc_golomb_open(&(((tb_zstream_t*)zst)->vlc), 4);
#elif TB_LZSW_VLC_TYPE_GAMMA
	zst->vlc = tb_zstream_vlc_gamma_open(&(((tb_zstream_t*)zst)->vlc));
#else
	zst->vlc = tb_zstream_vlc_fixed_open(&(((tb_zstream_t*)zst)->vlc), 16);
#endif

	return ((tb_tstream_t*)zst);
}
tb_tstream_t* tb_zstream_open_lzsw_deflate(tb_lzsw_deflate_zstream_t* zst)
{
	TB_ASSERT(zst);
	if (!zst) return TB_NULL;

	// init 
	memset(zst, 0, sizeof(tb_lzsw_deflate_zstream_t));

	// init tstream
	((tb_tstream_t*)zst)->transform = tb_zstream_deflate_lzsw_transform;
	((tb_tstream_t*)zst)->close 	= tb_zstream_deflate_lzsw_close;

	// init zstream
	((tb_zstream_t*)zst)->algo 		= TB_ZSTREAM_ALGO_LZSW;
	((tb_zstream_t*)zst)->action 	= TB_ZSTREAM_ACTION_DEFLATE;

	// open vlc
#if TB_LZSW_VLC_TYPE_GOLOMB
	zst->vlc = tb_zstream_vlc_golomb_open(&(((tb_zstream_t*)zst)->vlc), 4);
#elif TB_LZSW_VLC_TYPE_GAMMA
	zst->vlc = tb_zstream_vlc_gamma_open(&(((tb_zstream_t*)zst)->vlc));
#else
	zst->vlc = tb_zstream_vlc_fixed_open(&(((tb_zstream_t*)zst)->vlc), 16);
#endif

	return ((tb_tstream_t*)zst);
}

tb_tstream_t* tb_zstream_open_lzsw(tb_lzsw_zstream_t* zst, tb_size_t action)
{
	if (action == TB_ZSTREAM_ACTION_INFLATE)
		return tb_zstream_open_lzsw_inflate((tb_lzsw_inflate_zstream_t*)zst);
	else if (action == TB_ZSTREAM_ACTION_DEFLATE)
		return tb_zstream_open_lzsw_deflate((tb_lzsw_deflate_zstream_t*)zst);
	else return TB_NULL;
}
