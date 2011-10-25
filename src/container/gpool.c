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
	TB_ASSERT_RETURN_VAL(gpool, TB_NULL);

	gpool->step = tb_align(step, (TB_CPU_BITSIZE >> 3));
	gpool->grow = tb_align(grow, 8); // align by 8-byte for info
	gpool->size = 0;
	gpool->maxn = tb_align(size, 8); // align by 8-byte for info
	if (func) gpool->func = *func;

	gpool->data = tb_calloc(gpool->maxn, gpool->step);
	TB_ASSERT_GOTO(gpool->data, fail);

	gpool->info = tb_calloc(1, gpool->maxn >> 3);
	TB_ASSERT_GOTO(gpool->info, fail);

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
tb_size_t tb_gpool_alloc(tb_gpool_t* gpool)
{
	TB_ASSERT_RETURN_VAL(gpool, 0);

	tb_size_t item = 0;

	// try allocating from the predicted item
#ifdef TB_GPOOL_PRED_ENABLE
# 	ifdef TB_DEBUG
	gpool->alloc_total++;
	if (!gpool->pred_n) gpool->pred_failed++;
# 	endif
	if (gpool->pred_n) item = gpool->pred[--gpool->pred_n];
#endif

	// is enough?
	if (!item && gpool->size < gpool->maxn)
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
				item = 1 + i;
				break;
			}
		}
	}
	// the gpool is full?
	if (!item)
	{
		// adjust max size
		TB_ASSERTA(gpool->size == gpool->maxn);
		gpool->maxn += gpool->grow;
		TB_ASSERT_RETURN_VAL(gpool->maxn < TB_POOL_MAX_SIZE, 0);

		// realloc data
		gpool->data = (tb_byte_t*)tb_realloc(gpool->data, gpool->maxn * gpool->step);
		TB_ASSERT_RETURN_VAL(gpool->data, 0);
		tb_memset(gpool->data + gpool->size * gpool->step, 0, gpool->grow * gpool->step);

		// realloc info
		gpool->info = (tb_byte_t*)tb_realloc(gpool->info, gpool->maxn >> 3);
		TB_ASSERT_RETURN_VAL(gpool->info, 0);
		tb_memset(gpool->info + (gpool->size >> 3), 0, gpool->grow >> 3);

		// get the index of item
		item = 1 + gpool->size;
	}

	if (!item) return 0;

	// set info
	TB_ASSERTA(!TB_POOL_INFO_ISSET(gpool->info, item - 1));
	TB_POOL_INFO_SET(gpool->info, item - 1);

#ifdef TB_GPOOL_PRED_ENABLE
 	// predict next, pred_n must be null, otherwise exists repeat item
	if (!gpool->pred_n) 
	{
		// item + 1 - 1 < maxn
		if (item < gpool->maxn && !TB_POOL_INFO_ISSET(gpool->info, item))
		{
			gpool->pred[0] = item + 1;
			gpool->pred_n = 1;
		}
	}

#endif

	// update size
	gpool->size++;
	TB_ASSERT(item && item < 1 + gpool->maxn);
	if (item > gpool->maxn) item = 0;
	return item;
}
tb_void_t tb_gpool_free(tb_gpool_t* gpool, tb_size_t item)
{
	TB_ASSERT(gpool && gpool->size && item > 0 && item < 1 + gpool->maxn);
	TB_ASSERT(TB_POOL_INFO_ISSET(gpool->info, item - 1));
	if (gpool && gpool->size && item > 0 && item < 1 + gpool->maxn)
	{
		// free item
		if (gpool->func.free) gpool->func.free(tb_gpool_get(gpool, item), gpool->func.priv);

		// set info
		TB_POOL_INFO_RESET(gpool->info, item - 1);

		// predict next
#ifdef TB_GPOOL_PRED_ENABLE
		if (gpool->pred_n < TB_GPOOL_PRED_MAX)
			gpool->pred[gpool->pred_n++] = item;
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

#ifdef TB_DEBUG
tb_byte_t* tb_gpool_get(tb_gpool_t* gpool, tb_size_t item)
{
	//TB_DBG("%d %d %d %d", gpool->step, gpool->size, item, gpool->maxn);
	TB_ASSERT(gpool && gpool->size && item > 0 && item < 1 + gpool->maxn);
	TB_ASSERT(TB_POOL_INFO_ISSET(gpool->info, item - 1));

	if (gpool && gpool->size && item > 0 && item < 1 + gpool->maxn)
		return (gpool->data + (item - 1) * gpool->step);
	else return TB_NULL;
}
tb_void_t tb_gpool_dump(tb_gpool_t* gpool)
{
	TB_DBG("size: %d", gpool->size);
	TB_DBG("maxn: %d", gpool->maxn);
	TB_DBG("step: %d", gpool->step);
	TB_DBG("grow: %d", gpool->grow);
	TB_DBG("pred: %02d%%, fail: %d, total: %d", gpool->alloc_total? ((gpool->alloc_total - gpool->pred_failed) * 100 / gpool->alloc_total) : -1, gpool->pred_failed, gpool->alloc_total);
}
#endif
