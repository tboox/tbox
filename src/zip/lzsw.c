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
 * @file		lzsw.c
 * @ingroup 	zip
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "lzsw.h"
#include "../../container/container.h"
#include "../../math/math.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/* ///////////////////////////////////////////////////////////////////////
 * details
 */
#if TB_LZSW_WINDOW_HASH_FIND

/* circle offset => global offset
 *
 * window: |-----------------------------------------------------------|
 * global: 0, 1, 2, ...                                           TB_LZSW_WINDOW_SIZE_MAX - 1
 * circle: 3, 4, 5, ...             TB_LZSW_WINDOW_SIZE_MAX - 1, 0, 1, 2
 *                                                               |
 *                                                         window->base
 *
 */
static __tb_inline__ tb_size_t tb_lzsw_window_goff(tb_size_t base, tb_size_t coff)
{
	return ((coff + base) % TB_LZSW_WINDOW_SIZE_MAX);
}
static __tb_inline__ tb_size_t tb_lzsw_window_coff(tb_size_t base, tb_size_t goff)
{
	return ((goff + TB_LZSW_WINDOW_SIZE_MAX - base) % TB_LZSW_WINDOW_SIZE_MAX);
}
static tb_void_t tb_lzsw_window_insert(tb_lzsw_deflate_window_t* window, tb_size_t size)
{
	tb_pool_t* pool = window->pool;
	tb_byte_t const* wb = window->we - window->wn;
	tb_byte_t const* wp = window->we - size;
	tb_byte_t const* we = window->we;
	for (; wp < we; wp++)
	{
		// alloc node
		tb_size_t node = tb_pool_alloc(pool);
		if (node)
		{
			// init node
			tb_lzsw_node_t* onode = TB_POOL_GET(pool, node, tb_lzsw_node_t);
			tb_assert(onode);
			onode->sign[0] = wp[0];
			onode->sign[1] = wp[1];
			onode->sign[2] = wp[2];
			onode->addr = wp;
			onode->coff = tb_lzsw_window_coff(window->base, wp - wb);

			// insert node to the head of hash 
			tb_size_t idx = wp[0] + wp[1] + wp[2];
			tb_size_t head = window->hash[idx];
			if (head)
			{
				tb_lzsw_node_t* onext = TB_POOL_GET(pool, head, tb_lzsw_node_t);
				tb_assert(onext);

				tb_lzsw_node_t* oprev = TB_POOL_GET(pool, onext->prev, tb_lzsw_node_t);
				tb_assert(oprev);

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

static tb_void_t tb_lzsw_window_remove(tb_lzsw_deflate_window_t* window, tb_size_t size)
{
	tb_pool_t* pool = window->pool;
#if 1
	if (size == TB_LZSW_WINDOW_SIZE_MAX)
	{
		tb_memset(window->hash, 0, sizeof(tb_size_t) * TB_LZSW_WINDOW_HASH_MAX);
		tb_pool_clear(pool);
	}
	else
#endif
	{
		tb_byte_t const* wb = window->we - window->wn;
		tb_byte_t const* wp = wb;
		tb_byte_t const* we = wb + size;
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
					tb_assert(onode);

					// get next & prev 
					tb_size_t next = onode->next;
					tb_size_t prev = onode->prev;
				
					// remove it
					if (onode->addr < we)
					{
						// detach it
						TB_POOL_SET_NEXT(pool, onode->prev, tb_lzsw_node_t, next);	
						TB_POOL_SET_PREV(pool, onode->next, tb_lzsw_node_t, prev);	

						// free it
						tb_pool_free(pool, node);
					}
					else break;
				
					// prev
					node = prev;
				}
					
				// remove head
				if (ohead->addr < we)
				{
					// detach it
					window->hash[idx] = 0;
				
					// free it
					tb_pool_free(pool, head);
				}
			}
		}
	}
}

#if 0
static tb_size_t tb_lzsw_window_find(tb_lzsw_deflate_window_t* window, tb_byte_t const* sp, tb_byte_t const* se, tb_size_t* p)
{
	tb_byte_t const* wb = window->we - window->wn;
	tb_byte_t const* wp = wb;
	tb_byte_t const* we = window->we;
	tb_byte_t const* mp = wp;
	tb_byte_t const* me = wp;
	for (; wp < we; wp++)
	{
		tb_byte_t const* wq = wp;
		tb_byte_t const* sq = sp;
		for (; /*wq < we && */sq < se && *wq == *sq; wq++, sq++);
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
static tb_size_t tb_lzsw_window_find(tb_lzsw_deflate_window_t* window, tb_byte_t const* sp, tb_byte_t const* se, tb_size_t* p)
{
	tb_byte_t const* wb = window->we - window->wn;
	tb_byte_t const* we = window->we;
	tb_byte_t const* mp = wb;
	tb_byte_t const* me = wb;

	tb_pool_t* pool = window->pool;
	tb_size_t idx = sp[0] + sp[1] + sp[2];
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
			tb_assert(onode);

			// find it
			if (onode->sign[0] == sp[0]
				&& onode->sign[1] == sp[1]
				&& onode->sign[2] == sp[2])
			{
				tb_byte_t const* wp = onode->addr;
				//tb_byte_t const* wp = wb + tb_lzsw_window_goff(window->base, onode->coff);
				tb_byte_t const* wq = wp + 3;
				tb_byte_t const* sq = sp + 3;

#if 1
				for (; /*wq < we && */sq < se && *wq == *sq; wq++, sq++);
#else
				for (; /*wq < we && */sq + 4 < se; wq += 4, sq += 4)
				{
					if (wq[0] != sq[0]) break;
					if (wq[1] != sq[1]) { wq++; sq++; break; }
					if (wq[2] != sq[2]) { wq += 2; sq += 2; break; }
					if (wq[3] != sq[3]) { wq += 3; sq += 3; break; }
				}
				for (; /*wq < we && */sq < se && *wq == *sq; wq++, sq++);
#endif
				if ((wq - wp) > (me - mp))
				{
					mp = wp;
					me = wq;
					//if (me > we) break;
					//if (me - mp > we - wp) break;
				}
			}

			// prev
			node = onode->prev;
		}

		// find it in the head last
		if (ohead->sign[0] == sp[0]
			&& ohead->sign[1] == sp[1]
			&& ohead->sign[2] == sp[2])
		{
			tb_byte_t const* wp = ohead->addr;
			//tb_byte_t const* wp = wb + tb_lzsw_window_goff(window->base, ohead->coff);
			tb_byte_t const* wq = wp + 3;
			tb_byte_t const* sq = sp + 3;

			for (; /*wq < we && */sq < se && *wq == *sq; wq++, sq++);
			if ((wq - wp) > (me - mp))
			{
				mp = wp;
				me = wq;
			}
		}
	}

	*p = mp - wb;
	return (me - mp);
}
#endif
#else
// find the maximum matched data
static tb_size_t tb_lzsw_window_find(tb_lzsw_deflate_window_t* window, tb_byte_t const* sp, tb_byte_t const* se, tb_size_t* p)
{
	tb_byte_t const* wb = window->we - window->wn;
	tb_byte_t const* wp = wb;
	tb_byte_t const* we = window->we;
	tb_byte_t const* mp = wp;
	tb_byte_t const* me = wp;
	for (; wp < we; wp++)
	{
		tb_byte_t const* wq = wp;
		tb_byte_t const* sq = sp;
		// @note: address maybe generate overlap
		for (; /*wq < we && */sq < se && *wq == *sq; wq++, sq++);
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

/* ///////////////////////////////////////////////////////////////////////
 * inflate
 */
static tb_bstream_t* tb_gstream_filter_zip_inflate_lzsw_transform(tb_gstream_filter_t* st)
{
	tb_lzsw_inflate_gstream_filter_zip_t* zst = (tb_lzsw_inflate_gstream_filter_zip_t*)st;
	tb_assert(zst);
	if (!zst) return tb_null;

	// get dst
	tb_byte_t* dp = st->dst.p;
	tb_byte_t* de = st->dst.e;
	tb_byte_t* db = dp;
	tb_assert(dp && de);
	if (!dp || !de) return tb_null;

	// get src
	tb_bstream_t* src = tb_gstream_filter_src(st);

	// get vlc
	tb_gstream_filter_zip_vlc_t* vlc = zst->vlc;
	tb_assert(vlc && vlc->get);

	// vlc callback
	tb_gstream_filter_zip_vlc_get_t vlc_get = vlc->get;

	// init window
	tb_lzsw_inflate_window_t* window = &zst->window;
	window->we = dp;

	// inflate 
	while (tb_bstream_left_bits(src) > 8 && (dp < de))
	{
		// get flag
		if (tb_bstream_get_u1(src))
		{
			// set position
			tb_size_t p = tb_bstream_get_ubits32(src, window->wb);
			
			// get size
			tb_size_t n = vlc_get(vlc, src) + 2;

			//tb_trace("%d %d", p, n);
#if 0 
			// fill data
			// @note: address maybe overlap
			tb_memcpy(dp, window->we + p - window->wn, n);

			// update dp
			dp += n;
#elif 0
			tb_byte_t const* wp = window->we + p - window->wn;
			while (n--) *dp++ = *wp++;
#else 
			tb_byte_t const* wp = window->we + p - window->wn;
			if (n < 32) while (n--) *dp++ = *wp++;
			else
			{
				tb_size_t l = n & 0x3;
				n -= l;

				tb_byte_t const* we = wp + n;
				while (wp < we)
				{
					dp[0] = wp[0];
					dp[1] = wp[1];
					dp[2] = wp[2];
					dp[3] = wp[3];
					dp += 4;
					wp += 4;
				}

				while (l--) *dp++ = *wp++;
			}
#endif

		}
		else 
		{
			*dp++ = tb_bstream_get_ubits32(src, 8);
			*dp++ = tb_bstream_get_ubits32(src, 8);
		}

		// update window
		window->we = dp;
		window->wn = TB_MATH_MIN((dp - db), TB_LZSW_WINDOW_SIZE_MAX);
		window->wb = (window->wn == TB_LZSW_WINDOW_SIZE_MAX)? window->mb : TB_MATH_ICLOG2I(window->wn);
	}

	// update position
	st->dst.p = dp;
	return tb_gstream_filter_dst(st);
}
static tb_void_t tb_gstream_filter_zip_inflate_lzsw_clos(tb_gstream_filter_t* st)
{
	tb_lzsw_inflate_gstream_filter_zip_t* zst = (tb_lzsw_inflate_gstream_filter_zip_t*)st;
	if (zst) 
	{
		// close vlc
		if (zst->vlc && zst->vlc->clos) zst->vlc->clos(zst->vlc); 

		// reset it
		tb_memset(zst, 0, sizeof(tb_lzsw_inflate_gstream_filter_zip_t));
	}
}
/* ///////////////////////////////////////////////////////////////////////
 * deflate
 */
static tb_bstream_t* tb_gstream_filter_zip_deflate_lzsw_transform(tb_gstream_filter_t* st)
{
	tb_lzsw_deflate_gstream_filter_zip_t* zst = (tb_lzsw_deflate_gstream_filter_zip_t*)st;
	tb_assert(zst);
	if (!zst) return tb_null;

	// get src
	tb_byte_t* sp = st->src.p;
	tb_byte_t* se = st->src.e;
	tb_byte_t* sb = sp;
	tb_assert(sp && se);
	if (!sp || !se) return tb_null;

	// get dst
	tb_bstream_t* dst = tb_gstream_filter_dst(st);

	// get vlc
	tb_gstream_filter_zip_vlc_t* vlc = zst->vlc;
	tb_assert(vlc && vlc->set);

	// vlc callback
	tb_gstream_filter_zip_vlc_set_t vlc_set = vlc->set;

	// init window
	tb_lzsw_deflate_window_t* window = &zst->window;
	window->we = sp;

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
			tb_bstream_set_ubits32(dst, p, window->wb);
			
			// set size
			vlc_set(vlc, n - 2, dst);

			// update sp
			sp += n;

			//tb_trace("%d %d", p, n);
		}
		else
		{
			// set flag
			tb_bstream_set_u1(dst, 0);

			// set value
			tb_bstream_set_ubits32(dst, *sp++, 8);
			tb_bstream_set_ubits32(dst, *sp++, 8);
		}

#if TB_LZSW_WINDOW_HASH_FIND
		// the old window
		tb_byte_t* 	owe = window->we;
		tb_size_t 	own = window->wn;
		tb_byte_t* 	owb = owe - own;

		// the new window 
		tb_byte_t* 	we = sp;
		tb_size_t 	wn = TB_MATH_MIN((sp - sb), TB_LZSW_WINDOW_SIZE_MAX);
		tb_byte_t* 	wb = we - wn;

		// the number of moving by left & right 
		tb_size_t 	ln = TB_MATH_MIN((wb - owb), TB_LZSW_WINDOW_SIZE_MAX);
		tb_size_t 	rn = TB_MATH_MIN((we - owe), TB_LZSW_WINDOW_SIZE_MAX);

		// remove the old nodes
		tb_lzsw_window_remove(window, ln);

		// slide window
		window->we = we;
		window->wn = wn;
		window->wb = (wn == TB_LZSW_WINDOW_SIZE_MAX)? window->mb : TB_MATH_ICLOG2I(wn);
		window->base = (window->base + TB_LZSW_WINDOW_SIZE_MAX - ln) % TB_LZSW_WINDOW_SIZE_MAX;
		//tb_trace("[window]: at: %d, base: %d, ln: %d, rn: %d, coff0: %d", wb - sb, window->base, ln, rn, tb_lzsw_window_coff(window->base, 0));


		// insert the new nodes
		tb_lzsw_window_insert(window, rn);
#else
		// update window
		window->we = sp;
		window->wn = TB_MATH_MIN((sp - sb), TB_LZSW_WINDOW_SIZE_MAX);
		window->wb = (window->wn == TB_LZSW_WINDOW_SIZE_MAX)? window->mb : TB_MATH_ICLOG2I(window->wn);
#endif
	}

	// sync 
	tb_bstream_sync(dst);

	//tb_pool_dump(window->pool);

	// update position
	st->src.p = sp;
	return dst;
}
static tb_void_t tb_gstream_filter_zip_deflate_lzsw_clos(tb_gstream_filter_t* st)
{
	tb_lzsw_deflate_gstream_filter_zip_t* zst = (tb_lzsw_deflate_gstream_filter_zip_t*)st;
	if (zst) 
	{
		// close vlc
		if (zst->vlc && zst->vlc->clos) zst->vlc->clos(zst->vlc); 

		// free pool
#if TB_LZSW_WINDOW_HASH_FIND
		if (zst->window.pool) tb_pool_exit(zst->window.pool);
#endif

		// reset it
		tb_memset(zst, 0, sizeof(tb_lzsw_deflate_gstream_filter_zip_t));
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_filter_t* tb_gstream_filter_zip_open_lzsw_inflate(tb_lzsw_inflate_gstream_filter_zip_t* zst)
{
	tb_assert(zst);
	if (!zst) return tb_null;

	// init 
	tb_memset(zst, 0, sizeof(tb_lzsw_inflate_gstream_filter_zip_t));

	// init filter
	((tb_gstream_filter_t*)zst)->transform 	= tb_gstream_filter_zip_inflate_lzsw_transform;
	((tb_gstream_filter_t*)zst)->clos 		= tb_gstream_filter_zip_inflate_lzsw_clos;

	// init zstream
	((tb_gstream_filter_zip_t*)zst)->algo 		= TB_ZSTREAM_ALGO_LZSW;
	((tb_gstream_filter_zip_t*)zst)->action 	= TB_ZSTREAM_ACTION_INFLATE;

	// open vlc
#if TB_LZSW_VLC_TYPE_GOLOMB
	zst->vlc = tb_gstream_filter_zip_vlc_golomb_open(&(((tb_gstream_filter_zip_t*)zst)->vlc), 4);
#elif TB_LZSW_VLC_TYPE_GAMMA
	zst->vlc = tb_gstream_filter_zip_vlc_gamma_open(&(((tb_gstream_filter_zip_t*)zst)->vlc));
#else
	zst->vlc = tb_gstream_filter_zip_vlc_fixed_open(&(((tb_gstream_filter_zip_t*)zst)->vlc), 16);
#endif

	// init window
	zst->window.mb = TB_MATH_ICLOG2I(TB_LZSW_WINDOW_SIZE_MAX);

	return ((tb_gstream_filter_t*)zst);
}
tb_gstream_filter_t* tb_gstream_filter_zip_open_lzsw_deflate(tb_lzsw_deflate_gstream_filter_zip_t* zst)
{
	tb_assert(zst);
	if (!zst) return tb_null;

	// init 
	tb_memset(zst, 0, sizeof(tb_lzsw_deflate_gstream_filter_zip_t));

	// init filter
	((tb_gstream_filter_t*)zst)->transform = tb_gstream_filter_zip_deflate_lzsw_transform;
	((tb_gstream_filter_t*)zst)->clos 	= tb_gstream_filter_zip_deflate_lzsw_clos;

	// init zstream
	((tb_gstream_filter_zip_t*)zst)->algo 		= TB_ZSTREAM_ALGO_LZSW;
	((tb_gstream_filter_zip_t*)zst)->action 	= TB_ZSTREAM_ACTION_DEFLATE;

	// open vlc
#if TB_LZSW_VLC_TYPE_GOLOMB
	zst->vlc = tb_gstream_filter_zip_vlc_golomb_open(&(((tb_gstream_filter_zip_t*)zst)->vlc), 4);
#elif TB_LZSW_VLC_TYPE_GAMMA
	zst->vlc = tb_gstream_filter_zip_vlc_gamma_open(&(((tb_gstream_filter_zip_t*)zst)->vlc));
#else
	zst->vlc = tb_gstream_filter_zip_vlc_fixed_open(&(((tb_gstream_filter_zip_t*)zst)->vlc), 16);
#endif

	// init window
	zst->window.mb = TB_MATH_ICLOG2I(TB_LZSW_WINDOW_SIZE_MAX);
#if TB_LZSW_WINDOW_HASH_FIND
	zst->window.pool = tb_pool_init(sizeof(tb_lzsw_node_t), TB_LZSW_WINDOW_SIZE_MAX, 0, tb_null, tb_null);
#endif

	return ((tb_gstream_filter_t*)zst);
}

tb_gstream_filter_t* tb_gstream_filter_zip_open_lzsw(tb_lzsw_gstream_filter_zip_t* zst, tb_size_t action)
{
	if (action == TB_ZSTREAM_ACTION_INFLATE)
		return tb_gstream_filter_zip_open_lzsw_inflate((tb_lzsw_inflate_gstream_filter_zip_t*)zst);
	else if (action == TB_ZSTREAM_ACTION_DEFLATE)
		return tb_gstream_filter_zip_open_lzsw_deflate((tb_lzsw_deflate_gstream_filter_zip_t*)zst);
	else return tb_null;
}
