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
 * \file		pool.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "pool.h"
#include "../libc/libc.h"

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

tb_pool_t* tb_pool_create(tb_size_t step, tb_size_t size, tb_size_t grow)
{
	tb_pool_t* pool = (tb_pool_t*)tb_calloc(1, sizeof(tb_pool_t));
	TB_ASSERT_RETURN_VAL(pool, TB_NULL);

	pool->step = tb_align(step, (TB_CPU_BITSIZE >> 3));
	pool->grow = tb_align(grow, 8); // align by 8-byte for info
	pool->size = 0;
	pool->maxn = tb_align(size, 8); // align by 8-byte for info

	pool->data = tb_calloc(pool->maxn, pool->step);
	TB_ASSERT_GOTO(pool->data, fail);

	pool->info = tb_calloc(1, pool->maxn >> 3);
	TB_ASSERT_GOTO(pool->info, fail);

#ifdef TB_MEMORY_POOL_PRED_ENABLE
	tb_size_t m = tb_min(pool->maxn, TB_MEMORY_POOL_PRED_MAX);
	tb_size_t n = m;
	while (n--) pool->pred[n] = m - n;
	pool->pred_n = m;
#endif

	return pool;
fail:
	if (pool) tb_pool_destroy(pool);
	return TB_NULL;
}

tb_void_t tb_pool_destroy(tb_pool_t* pool)
{
	if (pool)
	{
#if 0 // discarded
		// free items
		if (pool->free)
		{
			tb_int_t i = 0;
			for (i = 0; i < pool->maxn; i++)
			{
				// is free?
				if (TB_POOL_INFO_ISSET(pool->info, i))
					tb_pool_free(pool, i + 1);
			}
		}
#endif

		// free data
		if (pool->data) tb_free(pool->data);

		// free info
		if (pool->info) tb_free(pool->info);

		// free it
		tb_free(pool);
	}
}
tb_size_t tb_pool_alloc(tb_pool_t* pool)
{
	TB_ASSERT_RETURN_VAL(pool, 0);

	tb_size_t item = 0;

	// try allocating from the predicted item
#ifdef TB_MEMORY_POOL_PRED_ENABLE
# 	ifdef TB_DEBUG
	pool->alloc_total++;
	if (!pool->pred_n) pool->pred_failed++;
# 	endif
	if (pool->pred_n) item = pool->pred[--pool->pred_n];
#endif

	// is enough?
	if (!item && pool->size < pool->maxn)
	{
		 // find free op node and skip the index: 0
		tb_size_t i = 0;
		tb_size_t n = pool->maxn;
		for (i = 0; i < n; ++i)
		{
			// is free?
			if (!TB_POOL_INFO_ISSET(pool->info, i))
			{
				// get op index
				item = 1 + i;
				break;
			}
		}
	}
	// the pool is full?
	if (!item)
	{
		// adjust max size
		TB_ASSERTA(pool->size == pool->maxn);
		pool->maxn += pool->grow;
		TB_ASSERT_RETURN_VAL(pool->maxn < TB_POOL_MAX_SIZE, 0);

		// realloc data
		pool->data = (tb_byte_t*)tb_realloc(pool->data, pool->maxn * pool->step);
		TB_ASSERT_RETURN_VAL(pool->data, 0);
		tb_memset(pool->data + pool->size * pool->step, 0, pool->grow * pool->step);

		// realloc info
		pool->info = (tb_byte_t*)tb_realloc(pool->info, pool->maxn >> 3);
		TB_ASSERT_RETURN_VAL(pool->info, 0);
		tb_memset(pool->info + (pool->size >> 3), 0, pool->grow >> 3);

		// get the index of item
		item = 1 + pool->size;
	}

	if (!item) return 0;

	// set info
	TB_ASSERTA(!TB_POOL_INFO_ISSET(pool->info, item - 1));
	TB_POOL_INFO_SET(pool->info, item - 1);

#ifdef TB_MEMORY_POOL_PRED_ENABLE
 	// predict next, pred_n must be null, otherwise exists repeat item
	if (!pool->pred_n) 
	{
		// item + 1 - 1 < maxn
		if (item < pool->maxn && !TB_POOL_INFO_ISSET(pool->info, item))
		{
			pool->pred[0] = item + 1;
			pool->pred_n = 1;
		}
	}

#endif

	// update size
	pool->size++;
	TB_ASSERT(item && item < 1 + pool->maxn);
	if (item > pool->maxn) item = 0;
	return item;
}
tb_void_t tb_pool_free(tb_pool_t* pool, tb_size_t item)
{
	TB_ASSERT(pool && pool->size && item > 0 && item < 1 + pool->maxn);
	TB_ASSERT(TB_POOL_INFO_ISSET(pool->info, item - 1));
	if (pool && pool->size && item > 0 && item < 1 + pool->maxn)
	{

#if 0 // discarded
		// free item
		if (pool->free) pool->free(pool->priv, tb_pool_get(pool, item));
#endif

		// set info
		TB_POOL_INFO_RESET(pool->info, item - 1);

		// predict next
#ifdef TB_MEMORY_POOL_PRED_ENABLE
		if (pool->pred_n < TB_MEMORY_POOL_PRED_MAX)
			pool->pred[pool->pred_n++] = item;
#endif
		// update pool size
		pool->size--;
	}
}
tb_void_t tb_pool_clear(tb_pool_t* pool)
{
#if 0 // discarded
	// free private data
	if (pool->free && pool->data) 
	{
		tb_int_t i = 0;
		for (i = 0; i < pool->maxn; ++i)
		{
			if (TB_POOL_INFO_ISSET(pool->info, i))
				pool->free(pool->priv, pool->data + i * pool->step);
		}
	}
#endif

	// clear info
	pool->size = 0;
	if (pool->info) tb_memset(pool->info, 0, pool->maxn >> 3);
	if (pool->data) tb_memset(pool->data, 0, pool->maxn);

#ifdef TB_MEMORY_POOL_PRED_ENABLE
	tb_size_t m = tb_min(pool->maxn, TB_MEMORY_POOL_PRED_MAX);
	tb_size_t n = m;
	while (n--) pool->pred[n] = m - n;
	pool->pred_n = m;
#endif
}
#ifdef TB_DEBUG
tb_byte_t* tb_pool_get(tb_pool_t* pool, tb_size_t item)
{
	//TB_DBG("%d %d %d %d", pool->step, pool->size, item, pool->maxn);
	TB_ASSERT(pool && pool->size && item > 0 && item < 1 + pool->maxn);
	TB_ASSERT(TB_POOL_INFO_ISSET(pool->info, item - 1));

	if (pool && pool->size && item > 0 && item < 1 + pool->maxn)
		return (pool->data + (item - 1) * pool->step);
	else return TB_NULL;
}
tb_void_t tb_pool_dump(tb_pool_t* pool)
{
	TB_DBG("size: %d", pool->size);
	TB_DBG("maxn: %d", pool->maxn);
	TB_DBG("step: %d", pool->step);
	TB_DBG("grow: %d", pool->grow);
	TB_DBG("pred: %02d%%, fail: %d, total: %d", pool->alloc_total? ((pool->alloc_total - pool->pred_failed) * 100 / pool->alloc_total) : -1, pool->pred_failed, pool->alloc_total);
}
#endif
