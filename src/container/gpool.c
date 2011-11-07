/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		gpool.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "gpool.h"
#include "../libc/libc.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

#define TB_POOL_MAX_SIZE 				(1 << 30)

#define TB_POOL_INFO_SET(info, idx) 	do {(info)[(idx) >> 3] |= (0x1 << ((idx) & 7));} while (0)
#define TB_POOL_INFO_RESET(info, idx) 	do {(info)[(idx) >> 3] &= ~(0x1 << ((idx) & 7));} while (0)
#define TB_POOL_INFO_ISSET(info, idx) 	((info)[(idx) >> 3] & (0x1 << ((idx) & 7)))

/* /////////////////////////////////////////////////////////
 * implemention
 */

tb_gpool_t* tb_gpool_init(tb_size_t step, tb_size_t size, tb_size_t grow, tb_gpool_item_func_t const* func)
{
	tb_gpool_t* gpool = (tb_gpool_t*)tb_calloc(1, sizeof(tb_gpool_t));
	tb_assert_and_check_return_val(gpool, TB_NULL);

	gpool->step = tb_align(step, (TB_CPU_BITSIZE >> 3));
	gpool->grow = tb_align(grow, 8); // align by 8-byte for info
	gpool->size = 0;
	gpool->maxn = tb_align(size, 8); // align by 8-byte for info
	if (func) gpool->func = *func;

	gpool->data = tb_calloc(gpool->maxn, gpool->step);
	tb_assert_and_check_goto(gpool->data, fail);

	gpool->info = tb_calloc(1, gpool->maxn >> 3);
	tb_assert_and_check_goto(gpool->info, fail);

#ifdef TB_GPOOL_PRED_ENABLE
	tb_size_t m = tb_min(gpool->maxn, TB_GPOOL_PRED_MAX);
	tb_size_t n = m;
	while (n--) gpool->pred[n] = m - n;
	gpool->pred_n = m;
#endif

	return gpool;
fail:
	if (gpool) tb_gpool_exit(gpool);
	return TB_NULL;
}

tb_void_t tb_gpool_exit(tb_gpool_t* gpool)
{
	if (gpool)
	{
		tb_gpool_clear(gpool);

		// free data
		if (gpool->data) tb_free(gpool->data);

		// free info
		if (gpool->info) tb_free(gpool->info);

		// free it
		tb_free(gpool);
	}
}
tb_size_t tb_gpool_put(tb_gpool_t* gpool, tb_void_t const* item)
{
	tb_assert_and_check_return_val(gpool, 0);

	tb_size_t itor = 0;

	// try allocating from the predicted itor
#ifdef TB_GPOOL_PRED_ENABLE
# 	ifdef TB_DEBUG
	gpool->alloc_total++;
	if (!gpool->pred_n) gpool->pred_failed++;
# 	endif
	if (gpool->pred_n) itor = gpool->pred[--gpool->pred_n];
#endif

	// is enough?
	if (!itor && gpool->size < gpool->maxn)
	{
		// find free op node and skip the index: 0
		tb_size_t i = 0;
		tb_size_t n = gpool->maxn;
		for (i = 0; i < n; ++i)
		{
			// is free?
			if (!TB_POOL_INFO_ISSET(gpool->info, i))
			{
				// get op index
				itor = 1 + i;
				break;
			}
		}
	}
	// the gpool is full?
	if (!itor)
	{
		// adjust max size
		tb_assert_abort(gpool->size == gpool->maxn);
		gpool->maxn += gpool->grow;
		tb_assert_and_check_return_val(gpool->maxn < TB_POOL_MAX_SIZE, 0);

		// realloc data
		gpool->data = (tb_byte_t*)tb_realloc(gpool->data, gpool->maxn * gpool->step);
		tb_assert_and_check_return_val(gpool->data, 0);
		tb_memset(gpool->data + gpool->size * gpool->step, 0, gpool->grow * gpool->step);

		// realloc info
		gpool->info = (tb_byte_t*)tb_realloc(gpool->info, gpool->maxn >> 3);
		tb_assert_and_check_return_val(gpool->info, 0);
		tb_memset(gpool->info + (gpool->size >> 3), 0, gpool->grow >> 3);

		// get the index of itor
		itor = 1 + gpool->size;
	}

	if (!itor) return 0;

	// set info
	tb_assert_abort(!TB_POOL_INFO_ISSET(gpool->info, itor - 1));
	TB_POOL_INFO_SET(gpool->info, itor - 1);

#ifdef TB_GPOOL_PRED_ENABLE
 	// predict next, pred_n must be null, otherwise exists repeat itor
	if (!gpool->pred_n) 
	{
		// itor + 1 - 1 < maxn
		if (itor < gpool->maxn && !TB_POOL_INFO_ISSET(gpool->info, itor))
		{
			gpool->pred[0] = itor + 1;
			gpool->pred_n = 1;
		}
	}

#endif

	// update size
	gpool->size++;
	tb_assert(itor && itor < 1 + gpool->maxn);
	if (itor > gpool->maxn) itor = 0;

	// update data
	if (itor) 
	{
		if (item) tb_memcpy(gpool->data + (itor - 1) * gpool->step, item, gpool->step);
		else tb_memset(gpool->data + (itor - 1) * gpool->step, 0, gpool->step);
	}
	return itor;
}

tb_void_t tb_gpool_set(tb_gpool_t* gpool, tb_size_t itor, tb_void_t const* item)
{
	tb_assert_and_check_return(gpool && itor);
	tb_byte_t* data = tb_gpool_itor_at(gpool, itor);
	if (data)
	{
		if (item) tb_memcpy(data, item, gpool->step);
		else tb_memset(data, 0, gpool->step);
	}
}
tb_void_t tb_gpool_del(tb_gpool_t* gpool, tb_size_t itor)
{
	tb_assert(gpool && gpool->size && itor > 0 && itor < 1 + gpool->maxn);
	tb_assert(TB_POOL_INFO_ISSET(gpool->info, itor - 1));
	if (gpool && gpool->size && itor > 0 && itor < 1 + gpool->maxn)
	{
		// free itor
		if (gpool->func.free) gpool->func.free(gpool->data + (itor - 1) * gpool->step, gpool->func.priv);

		// set info
		TB_POOL_INFO_RESET(gpool->info, itor - 1);

		// predict next
#ifdef TB_GPOOL_PRED_ENABLE
		if (gpool->pred_n < TB_GPOOL_PRED_MAX)
			gpool->pred[gpool->pred_n++] = itor;
#endif
		// update gpool size
		gpool->size--;
	}
}
tb_void_t tb_gpool_clear(tb_gpool_t* gpool)
{
	// free items
	if (gpool->func.free && gpool->data) 
	{
		tb_int_t i = 0;
		for (i = 0; i < gpool->maxn; ++i)
		{
			if (TB_POOL_INFO_ISSET(gpool->info, i))
				gpool->func.free(gpool->data + i * gpool->step, gpool->func.priv);
		}
	}

	// clear info
	gpool->size = 0;
	if (gpool->info) tb_memset(gpool->info, 0, gpool->maxn >> 3);
	if (gpool->data) tb_memset(gpool->data, 0, gpool->maxn);

#ifdef TB_GPOOL_PRED_ENABLE
	tb_size_t m = tb_min(gpool->maxn, TB_GPOOL_PRED_MAX);
	tb_size_t n = m;
	while (n--) gpool->pred[n] = m - n;
	gpool->pred_n = m;
#endif
}
tb_void_t* tb_gpool_itor_at(tb_gpool_t* gpool, tb_size_t itor)
{
	return (tb_void_t*)tb_gpool_itor_const_at(gpool, itor);
}
tb_void_t const* tb_gpool_itor_const_at(tb_gpool_t const* gpool, tb_size_t itor)
{
	//tb_trace("%d %d %d %d", gpool->step, gpool->size, itor, gpool->maxn);
	tb_assert(gpool && gpool->size && itor > 0 && itor < 1 + gpool->maxn);
	tb_assert(TB_POOL_INFO_ISSET(gpool->info, itor - 1));

	if (gpool && gpool->size && itor > 0 && itor < 1 + gpool->maxn)
		return (gpool->data + (itor - 1) * gpool->step);
	else return TB_NULL;
}
tb_size_t tb_gpool_itor_head(tb_gpool_t const* gpool)
{
	tb_assert_abort(gpool);
	if (gpool->size)
	{
		// find the first item
		tb_size_t i = 0;
		tb_size_t n = gpool->maxn;
		for ( ; i < n; ++i)
		{
			// is non-free?
			if (TB_POOL_INFO_ISSET(gpool->info, i))
				return (1 + i);
		}
	}
	return gpool->maxn;
}
tb_size_t tb_gpool_itor_tail(tb_gpool_t const* gpool)
{
	tb_assert_abort(gpool);
	return (gpool->maxn);
}
tb_size_t tb_gpool_itor_next(tb_gpool_t const* gpool, tb_size_t itor)
{
	tb_assert_abort(gpool && itor);
	if (gpool->size)
	{
		// find the next item
		tb_size_t i = itor;
		tb_size_t n = gpool->maxn;
		for ( ; i < n; ++i)
		{
			// is non-free?
			if (TB_POOL_INFO_ISSET(gpool->info, i))
				return (1 + i);
		}
	}
	return gpool->maxn;
}

tb_size_t tb_gpool_size(tb_gpool_t const* gpool)
{
	tb_assert_and_check_return_val(gpool, 0);
	return gpool->size;
}
tb_size_t tb_gpool_maxn(tb_gpool_t const* gpool)
{
	tb_assert_and_check_return_val(gpool, 0);
	return gpool->maxn;
}

#ifdef TB_DEBUG
tb_void_t tb_gpool_dump(tb_gpool_t* gpool)
{
	tb_trace("size: %d", gpool->size);
	tb_trace("maxn: %d", gpool->maxn);
	tb_trace("step: %d", gpool->step);
	tb_trace("grow: %d", gpool->grow);
	tb_trace("pred: %02d%%, fail: %d, total: %d", gpool->alloc_total? ((gpool->alloc_total - gpool->pred_failed) * 100 / gpool->alloc_total) : -1, gpool->pred_failed, gpool->alloc_total);
}
#endif
