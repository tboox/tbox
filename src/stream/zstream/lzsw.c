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
#include "../../container/container.h"
#include "../../math/math.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* /////////////////////////////////////////////////////////
 * details
 */
#if TB_LZSW_WINDOW_FIND_HASH

/* circle offset => global offset
 *
 * global: 0, 1, 2, ...                                           TB_LZSW_WINDOW_MAX - 1
 * circle: 3, 4, 5, ...             TB_LZSW_WINDOW_MAX - 1, 0, 1, 2
 *                                                          |
 *                                                    window->base
 *
 */
static __tplat_inline__ tb_size_t tb_lzsw_window_goff(tb_lzsw_deflate_window_t* window, tb_size_t coff)
{
	return ((coff + window->base) % window->n);
}
static void tb_lzsw_window_insert(tb_lzsw_deflate_window_t* window, tb_size_t size)
{
	tb_pool_t* pool = window->pool;
	tb_byte_t const* wp = window->e - size;
	tb_byte_t const* we = window->e - 2;
	for (; wp < we; wp++)
	{
		// alloc node
		tb_size_t node = tb_pool_alloc(pool);
		if (node)
		{
			// init node
			tb_lzsw_node_t* onode = TB_POOL_GET(pool, node, tb_lzsw_node_t);
			TB_ASSERT(onode);
			onode->sign[0] = wp[0];
			onode->sign[1] = wp[1];
			onode->sign[2] = wp[2];

			// insert node to the head of hash 
			tb_size_t idx = wp[0] + wp[1] + wp[2];
			tb_size_t head = window->hash[idx];
			if (head)
			{
				tb_lzsw_node_t* onext = TB_POOL_GET(pool, head, tb_lzsw_node_t);
				TB_ASSERT(onext);

				tb_lzsw_node_t* oprev = TB_POOL_GET(pool, onext->prev, tb_lzsw_node_t);
				TB_ASSERT(oprev);

				onode->next = head;
				onode->prev = onext->prev;
				onext->prev = node;
				oprev->next = node;
			}
			else
			{
				onode->next = node;
				onode->prev = node;
			}
			window->hash[idx] = node;
		}
		else break;
	}
}
static void tb_lzsw_window_remove(tb_lzsw_deflate_window_t* window, tb_size_t size)
{
	tb_pool_t* pool = window->pool;
	tb_byte_t const* wp = window->e - size;
	tb_byte_t const* we = window->e - 2;
	for (; wp < we; wp++)
	{
		tb_size_t idx = wp[0] + wp[1] + wp[2];
		tb_size_t head = window->hash[idx];
		if (head)
		{
			// get the head
			tb_lzsw_node_t* ohead = TB_POOL_GET(pool, head, tb_lzsw_node_t);

			// remove from the tail
			tb_size_t node = ohead->prev;
			while (node != head)
			{
				// get node
				tb_lzsw_node_t* onode = TB_POOL_GET(pool, node, tb_lzsw_node_t);
				TB_ASSERT(onode);

				// get next & prev 
				tb_size_t next = onode->next;
				tb_size_t prev = onode->prev;
			
				// remove it
				if (1)
				{
					// detach it
					TB_POOL_SET_NEXT(pool, onode->prev, tb_lzsw_node_t, next);	
					TB_POOL_SET_PREV(pool, onode->next, tb_lzsw_node_t, prev);	

					// free it
					tb_pool_free(pool, node);
				}
			
				// prev
				node = prev;
			}

			// remove head
			if (1)
			{
				// detach it
				window->hash[idx] = 0;
			
				// free it
				tb_pool_free(pool, head);
			}
		}
	}
}
static tb_size_t tb_lzsw_window_find(tb_lzsw_deflate_window_t* window, tb_byte_t const* sp, tb_byte_t const* se, tb_size_t* p)
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
#else
// find the maximum matched data
static tb_size_t tb_lzsw_window_find(tb_lzsw_deflate_window_t* window, tb_byte_t const* sp, tb_byte_t const* se, tb_size_t* p)
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
#endif

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
	tb_lzsw_inflate_window_t* window = &zst->window;
	window->e = dp;

	// inflate 
	while (tb_bstream_left_bits(src) > 8 && (dp < de))
	{
		// get flag
		if (tb_bstream_get_u1(src))
		{
			// set position
			tb_size_t p = tb_bstream_get_ubits(src, window->b);
			
			// get size
			tb_size_t n = vlc_get(vlc, src) + 2;

			//TB_DBG("%d %d", p, n);
#if 1 
			// fill data
			// \note: address maybe overlap
			memcpy(dp, window->e + p - window->n, n);

			// update dp
			dp += n;
#else
			tb_byte_t const* wp = window->e + p - window->n;
			while (n--) *dp++ = *wp++;
#endif

		}
		else 
		{
			*dp++ = tb_bstream_get_ubits(src, 8);
			*dp++ = tb_bstream_get_ubits(src, 8);
		}

		// update window
		window->e = dp;
		if (dp - db <= TB_LZSW_WINDOW_SIZE_MAX) 
		{
			window->n = dp - db;
			window->b = TB_MATH_ICLOG2I(window->n);
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
	tb_lzsw_deflate_window_t* window = &zst->window;
	window->e = sp;

	// deflate 
	while (sp < se)
	{
		tb_size_t p = 0;
		tb_size_t n = tb_lzsw_window_find(window, sp, se, &p);
		if (n > 2)
		{
			// set flag
			tb_bstream_set_u1(dst, 1);

			// set position
			tb_bstream_set_ubits(dst, p, window->b);
			
			// set size
			vlc_set(vlc, n - 2, dst);

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

#if TB_LZSW_WINDOW_FIND_HASH
		// the old window
		tb_byte_t* 	owe = window->e;
		tb_size_t 	own = window->n;
		tb_byte_t* 	owb = owe - own;

		// the new window 
		tb_byte_t* 	we = sp;
		tb_size_t 	wn = ((sp - sb) > TB_LZSW_WINDOW_SIZE_MAX)? own : (sp - sb);
		tb_byte_t* 	wb = we - wn;

		// remove the old nodes
		//tb_lzsw_window_remove(window, 0);

		// slide window
		window->e = we;
		window->n = wn;
		window->b = ((sp - sb) > TB_LZSW_WINDOW_SIZE_MAX)? window->b : TB_MATH_ICLOG2I(wn);
		window->base = (window->base + wn - ((wb - owb) % TB_LZSW_WINDOW_MAX)) % wn;
		TB_DBG("lmove: %d, rmove: %d, base: %d, wb: %d", wb - owb, we - owe, window->base, wb - st->src.p);

		// insert the new nodes
		//tb_lzsw_window_insert(window, 0);
#else
		// update window
		window->e = sp;
		if (sp - sb <= TB_LZSW_WINDOW_SIZE_MAX) 
		{
			window->n = sp - sb;
			window->b = TB_MATH_ICLOG2I(window->n);
		}
#endif
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

		// free pool
#if TB_LZSW_WINDOW_FIND_HASH
		if (zst->window.pool) tb_pool_destroy(zst->window.pool);
#endif

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

	// create pool
#if TB_LZSW_WINDOW_FIND_HASH
	zst->window.pool = tb_pool_create(sizeof(tb_lzsw_node_t), TB_LZSW_WINDOW_SIZE_MAX - 2, 0);
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
