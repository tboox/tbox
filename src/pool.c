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
 * along with TGraphic; 
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
#include "math.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* /////////////////////////////////////////////////////////
 * details
 */

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
		if (pool->data) tb_free(pool->data);
		if (pool->info) tb_free(pool->info);
		tb_free(pool);
	}
}
tb_size_t tb_pool_alloc(tb_pool_t* pool)
{
	TB_ASSERT(pool);

	tb_size_t item = 0;

	// try allocating from the predicted item
#ifdef TB_MEMORY_POOL_PREDICTION_ENABLE
	if (pool->pred && !tb_pool_info_isset(pool->info, pool->pred - 1)) 
		item = pool->pred;
#endif

	// is enough?
	if (!item && pool->size < pool->maxn)
	{
		 // find free op node and skip the index: 0
		tb_int_t i = 0;
		for (i = 0; i < pool->maxn; ++i)
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
	tb_pool_info_set(pool->info, item - 1);

	// predict next
#ifdef TB_MEMORY_POOL_PREDICTION_ENABLE
	// item + 1 - 1 < maxn
	if (item < pool->maxn) pool->pred = item + 1;
	else pool->pred = 0;
#endif

	// update size
	pool->size++;
	TB_ASSERT(item && item < 1 + pool->maxn);
	return item;
}
void tb_pool_free(tb_pool_t* pool, tb_size_t item)
{
	TB_ASSERT(pool && pool->size && item > 0 && item < 1 + pool->maxn);
	TB_ASSERT(tb_pool_info_isset(pool->info, item - 1));
	if (pool && pool->size && item > 0 && item < 1 + pool->maxn)
	{
		// set info
		tb_pool_info_reset(pool->info, item - 1);

		// predict next
#ifdef TB_MEMORY_POOL_PREDICTION_ENABLE
		pool->pred = item;
#endif
		// update pool size
		pool->size--;
	}
}
tb_byte_t* tb_pool_get(tb_pool_t* pool, tb_size_t item)
{
	//TB_DBG("%d %d %d %d", pool->step, pool->size, item, pool->maxn);
	TB_ASSERT(pool && pool->size && item > 0 && item < 1 + pool->maxn);
	if (!item || !tb_pool_info_isset(pool->info, item - 1)) return TB_NULL;

	if (pool && pool->size && item > 0 && item < 1 + pool->maxn)
		return (pool->data + (item - 1) * pool->step);
	else return TB_NULL;
}
tb_byte_t* tb_pool_put(tb_pool_t* pool, tb_size_t item)
{
	TB_ASSERT(pool && item > 0);
	if (item <= 0) return TB_NULL;

	// grow pool if not enough
	if (item >= 1 + pool->maxn)
	{
		// adjust max size
		pool->maxn = TB_MATH_ALIGN(item + 1, pool->grow);
		if (pool->maxn > TB_POOL_MAX_SIZE) return TB_NULL;

		// tb_realloc data
		pool->data = (tb_byte_t*)tb_realloc(pool->data, pool->maxn * pool->step);
		if (!pool->data) return TB_NULL;
		memset(pool->data + pool->size * pool->step, 0, pool->grow * pool->step);

		// tb_realloc info
		pool->info = (tb_byte_t*)tb_realloc(pool->info, pool->maxn >> 3);
		if (!pool->info) return 0;
		memset(pool->info + (pool->size >> 3), 0, pool->grow >> 3);
	}
	TB_ASSERT(item < 1 + pool->maxn);

	// create item
	if (!tb_pool_info_isset(pool->info, item - 1))
	{
		tb_pool_info_set(pool->info, item - 1);
		pool->size++;
	}
	return (pool->data + (item - 1) * pool->step);
}
void tb_pool_reset(tb_pool_t* pool)
{
	if (pool)
	{
		pool->size = 0;
		memset(pool->info, 0, pool->maxn >> 3);

		// reset the predicted item
#ifdef TB_MEMORY_POOL_PREDICTION_ENABLE
		pool->pred = 0;
#endif
	}
}
tb_size_t tb_pool_maxn(tb_pool_t* pool)
{
	if (pool) return pool->maxn;
	else return 0;
}
tb_bool_t tb_pool_exists(tb_pool_t* pool, tb_size_t item)
{
	if (pool && pool->size && item > 0 && item < 1 + pool->maxn)
		return tb_pool_info_isset(pool->info, item - 1)? TB_TRUE : TB_FALSE;
	else return TB_FALSE;
}
