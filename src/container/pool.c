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
 * \file		pool.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "pool.h"
#include "../math/math.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
/* /////////////////////////////////////////////////////////
 * details
 */
#if 0
static __tplat_inline__ void tb_pool_info_set(tb_byte_t* info, tb_int_t idx)
{
	TB_ASSERT(info && idx < TB_POOL_MAX_SIZE);
	info[idx >> 3] |= (0x1 << (idx & 7));
}
static __tplat_inline__ void tb_pool_info_reset(tb_byte_t* info, tb_int_t idx)
{
	TB_ASSERT(info && idx < TB_POOL_MAX_SIZE);
	info[idx >> 3] &= ~(0x1 << (idx & 7));
}
static __tplat_inline__ tb_int_t tb_pool_info_isset(tb_byte_t* info, tb_int_t idx)
{
	TB_ASSERT(info && idx < TB_POOL_MAX_SIZE);
	return (info[idx >> 3] & (0x1 << (idx & 7)));
}
#else
# 	define tb_pool_info_set(info, idx) 		do {(info)[(idx) >> 3] |= (0x1 << ((idx) & 7));} while (0)
# 	define tb_pool_info_reset(info, idx) 	do {(info)[(idx) >> 3] &= ~(0x1 << ((idx) & 7));} while (0)
# 	define tb_pool_info_isset(info, idx) 	((info)[(idx) >> 3] & (0x1 << ((idx) & 7)))
#endif
/* /////////////////////////////////////////////////////////
 * internal implemention
 */

#if 1
tb_pool_t* tb_pool_create(tb_size_t step, tb_size_t size, tb_size_t grow)
{
	tb_pool_t* pool = (tb_pool_t*)tb_malloc(sizeof(tb_pool_t));
	if (!pool) return TB_NULL;
	memset(pool, 0, sizeof(tb_pool_t));

	// align by 8-byte for info
	TB_ASSERT(!(grow & 7));
	TB_ASSERT(!(size & 7));

	pool->step = step;
	pool->grow = grow;
	pool->size = 0;
	pool->maxn = size;

	pool->data = tb_malloc(pool->maxn * pool->step);
	if (!pool->data) goto fail;
	memset(pool->data, 0, pool->maxn * pool->step);

	pool->info = tb_malloc(pool->maxn >> 3);
	if (!pool->info) goto fail;
	memset(pool->info, 0, pool->maxn >> 3);

#ifdef TB_MEMORY_POOL_PRED_ENABLE
	tb_size_t m = TB_MATH_MIN(pool->maxn, TB_MEMORY_POOL_PRED_MAX);
	tb_size_t n = m;
	while (n--) pool->pred[n] = m - n;
	pool->pred_n = m;
#endif

	return pool;
fail:
	if (pool) tb_pool_destroy(pool);
	return TB_NULL;
}
#else
tb_pool_t* tb_pool_create_with_trace(tb_size_t step, tb_size_t size, tb_size_t grow, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
{
	tb_pool_t* pool = (tb_pool_t*)tplat_pool_allocate(TB_MEMORY_POOL_INDEX, sizeof(tb_pool_t), func, line, file);
	if (!pool) return TB_NULL;
	memset(pool, 0, sizeof(tb_pool_t));

	// align by 8-byte for info
	TB_ASSERT(!(grow & 7));
	TB_ASSERT(!(size & 7));

	pool->step = step;
	pool->grow = grow;
	pool->size = 0;
	pool->maxn = size;

	pool->data = tplat_pool_allocate(TB_MEMORY_POOL_INDEX, pool->maxn * pool->step, func, line, file);
	if (!pool->data) goto fail;
	memset(pool->data, 0, pool->maxn * pool->step);

	pool->info = tplat_pool_allocate(TB_MEMORY_POOL_INDEX, pool->maxn >> 3, func, line, file);
	if (!pool->info) goto fail;
	memset(pool->info, 0, pool->maxn >> 3);

	return pool;
fail:
	if (pool) tb_pool_destroy(pool);
	return TB_NULL;
}
#endif

void tb_pool_destroy(tb_pool_t* pool)
{
	if (pool)
	{
		// free items
		if (pool->free)
		{
			tb_int_t i = 0;
			for (i = 0; i < pool->maxn; i++)
			{
				// is free?
				if (tb_pool_info_isset(pool->info, i))
					tb_pool_free(pool, i + 1);
			}
		}

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
	TB_ASSERT(pool);

	// {
	tb_size_t item = 0;

	// try allocating from the predicted item
#ifdef TB_MEMORY_POOL_PRED_ENABLE
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
			if (!tb_pool_info_isset(pool->info, i))
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
		TB_ASSERT(pool->size == pool->maxn);
		pool->maxn += pool->grow;
		TB_ASSERT(pool->maxn <= TB_POOL_MAX_SIZE);
		if (pool->maxn > TB_POOL_MAX_SIZE) return 0;

		// realloc data
		pool->data = (tb_byte_t*)tb_realloc(pool->data, pool->maxn * pool->step);
		TB_ASSERT(pool->data);
		if (!pool->data) return 0;
		memset(pool->data + pool->size * pool->step, 0, pool->grow * pool->step);

		// realloc info
		pool->info = (tb_byte_t*)tb_realloc(pool->info, pool->maxn >> 3);
		TB_ASSERT(pool->info);
		if (!pool->info) return 0;
		memset(pool->info + (pool->size >> 3), 0, pool->grow >> 3);

		// get the index of item
		item = 1 + pool->size;
	}

	if (!item) return 0;

	// set info
	TB_ASSERT(!tb_pool_info_isset(pool->info, item - 1));
	tb_pool_info_set(pool->info, item - 1);

#ifdef TB_MEMORY_POOL_PRED_ENABLE
 	// predict next, pred_n must be null, otherwise exists repeat item
	if (!pool->pred_n) 
	{
		// item + 1 - 1 < maxn
		if (item < pool->maxn && !tb_pool_info_isset(pool->info, item))
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
	// }
}
void tb_pool_free(tb_pool_t* pool, tb_size_t item)
{
	TB_ASSERT(pool && pool->size && item > 0 && item < 1 + pool->maxn);
	TB_ASSERT(tb_pool_info_isset(pool->info, item - 1));
	if (pool && pool->size && item > 0 && item < 1 + pool->maxn)
	{
		// free item
		if (pool->free) pool->free(pool->priv, tb_pool_get(pool, item));

		// set info
		tb_pool_info_reset(pool->info, item - 1);

		// predict next
#ifdef TB_MEMORY_POOL_PRED_ENABLE
		if (pool->pred_n < TB_MEMORY_POOL_PRED_MAX)
			pool->pred[pool->pred_n++] = item;
#endif
		// update pool size
		pool->size--;
	}
}
void tb_pool_clear(tb_pool_t* pool)
{
	// free private data
	if (pool->free && pool->data) 
	{
		tb_int_t i = 0;
		for (i = 0; i < pool->maxn; ++i)
		{
			if (tb_pool_info_isset(pool->info, i))
				pool->free(pool->priv, pool->data + i * pool->step);
		}
	}

	// clear info
	pool->size = 0;
	if (pool->info) memset(pool->info, 0, pool->maxn >> 3);
	if (pool->data) memset(pool->data, 0, pool->maxn);

#ifdef TB_MEMORY_POOL_PRED_ENABLE
	tb_size_t m = TB_MATH_MIN(pool->maxn, TB_MEMORY_POOL_PRED_MAX);
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
	TB_ASSERT(tb_pool_info_isset(pool->info, item - 1));

	if (pool && pool->size && item > 0 && item < 1 + pool->maxn)
		return (pool->data + (item - 1) * pool->step);
	else return TB_NULL;
}
#endif
