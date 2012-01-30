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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		fpool.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fpool.h"
#include "../libc/libc.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#define TB_POOL_MAX_SIZE 				(1 << 30)

#define tb_pool_info_set(info, idx) 	do {(info)[(idx) >> 3] |= (0x1 << ((idx) & 7));} while (0)
#define tb_pool_info_reset(info, idx) 	do {(info)[(idx) >> 3] &= ~(0x1 << ((idx) & 7));} while (0)
#define tb_pool_info_isset(info, idx) 	((info)[(idx) >> 3] & (0x1 << ((idx) & 7)))

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_fpool_t* tb_fpool_init(tb_size_t size, tb_size_t grow, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(size && grow, TB_NULL);
	tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.copy, TB_NULL);

	// alloc fpool
	tb_fpool_t* fpool = (tb_fpool_t*)tb_calloc(1, sizeof(tb_fpool_t));
	tb_assert_and_check_return_val(fpool, TB_NULL);

	// init fpool
	fpool->grow = tb_align(grow, TB_CPU_BITBYTE);
	fpool->size = 0;
	fpool->maxn = tb_align(size, TB_CPU_BITBYTE);
	fpool->func = func;
	fpool->func.size = tb_align(func.size, TB_CPU_BITBYTE);

	// alloc data
	fpool->data = tb_calloc(fpool->maxn, fpool->func.size);
	tb_assert_and_check_goto(fpool->data, fail);

	// alloc info
	fpool->info = tb_calloc(1, fpool->maxn >> 3);
	tb_assert_and_check_goto(fpool->info, fail);

	// init predicted info
#ifdef TB_FPOOL_PRED_ENABLE
	tb_size_t m = tb_min(fpool->maxn, TB_FPOOL_PRED_MAX);
	tb_size_t n = m;
	while (n--) fpool->pred[n] = m - n;
	fpool->pred_n = m;
#endif

	return fpool;
fail:
	if (fpool) tb_fpool_exit(fpool);
	return TB_NULL;
}

tb_void_t tb_fpool_exit(tb_fpool_t* fpool)
{
	if (fpool)
	{
		tb_fpool_clear(fpool);

		// free data
		if (fpool->data) tb_free(fpool->data);

		// free info
		if (fpool->info) tb_free(fpool->info);

		// free it
		tb_free(fpool);
	}
}
tb_size_t tb_fpool_put(tb_fpool_t* fpool, tb_cpointer_t data)
{
	tb_assert_and_check_return_val(fpool, 0);

	tb_size_t itor = 0;

	// try allocating from the predicted itor
#ifdef TB_FPOOL_PRED_ENABLE
# 	ifdef TB_DEBUG
	fpool->alloc_total++;
	if (!fpool->pred_n) fpool->pred_failed++;
# 	endif
	if (fpool->pred_n) itor = fpool->pred[--fpool->pred_n];
#endif

	// is enough?
	if (!itor && fpool->size < fpool->maxn)
	{
		// find free op node and skip the index: 0
		tb_size_t i = 0;
		tb_size_t n = fpool->maxn;
		for (i = 0; i < n; ++i)
		{
			// is free?
			if (!tb_pool_info_isset(fpool->info, i))
			{
				// get op index
				itor = 1 + i;
				break;
			}
		}
	}
	// the fpool is full?
	if (!itor)
	{
		// adjust max size
		tb_assert_abort(fpool->size == fpool->maxn);
		fpool->maxn += fpool->grow;
		tb_assert_and_check_return_val(fpool->maxn < TB_POOL_MAX_SIZE, 0);

		// realloc data
		fpool->data = (tb_byte_t*)tb_realloc(fpool->data, fpool->maxn * fpool->func.size);
		tb_assert_and_check_return_val(fpool->data, 0);
		tb_memset(fpool->data + fpool->size * fpool->func.size, 0, fpool->grow * fpool->func.size);

		// realloc info
		fpool->info = (tb_byte_t*)tb_realloc(fpool->info, fpool->maxn >> 3);
		tb_assert_and_check_return_val(fpool->info, 0);
		tb_memset(fpool->info + (fpool->size >> 3), 0, fpool->grow >> 3);

		// get the index of itor
		itor = 1 + fpool->size;
	}

	if (!itor) return 0;

	// set info
	tb_assert_abort(!tb_pool_info_isset(fpool->info, itor - 1));
	tb_pool_info_set(fpool->info, itor - 1);

#ifdef TB_FPOOL_PRED_ENABLE
 	// predict next, pred_n must be null, otherwise exists repeat itor
	if (!fpool->pred_n) 
	{
		// itor + 1 - 1 < maxn
		if (itor < fpool->maxn && !tb_pool_info_isset(fpool->info, itor))
		{
			fpool->pred[0] = itor + 1;
			fpool->pred_n = 1;
		}
	}

#endif

	// update size
	fpool->size++;
	tb_assert(itor && itor < 1 + fpool->maxn);
	if (itor > fpool->maxn) itor = 0;

	// duplicate data
	if (itor) fpool->func.dupl(&fpool->func, fpool->data + (itor - 1) * fpool->func.size, data);
	return itor;
}
tb_pointer_t tb_fpool_get(tb_fpool_t* fpool, tb_size_t itor)
{
	//tb_trace("%d %d %d %d", fpool->func.size, fpool->size, itor, fpool->maxn);
	tb_assert_and_check_return_val(fpool && fpool->size && itor > 0 && itor < 1 + fpool->maxn, TB_NULL);
	tb_assert_and_check_return_val(tb_pool_info_isset(fpool->info, itor - 1), TB_NULL);

	return fpool->func.data(&fpool->func, fpool->data + (itor - 1) * fpool->func.size);
}
tb_void_t tb_fpool_set(tb_fpool_t* fpool, tb_size_t itor, tb_cpointer_t data)
{
	tb_assert_and_check_return(fpool && fpool->size && itor > 0 && itor < 1 + fpool->maxn);
	tb_assert_and_check_return(tb_pool_info_isset(fpool->info, itor - 1));

	// copy data
	fpool->func.copy(&fpool->func, fpool->data + (itor - 1) * fpool->func.size, data);
}
tb_void_t tb_fpool_clr(tb_fpool_t* fpool, tb_size_t itor)
{
	tb_assert_and_check_return(fpool && fpool->size && itor > 0 && itor < 1 + fpool->maxn);
	tb_assert_and_check_return(tb_pool_info_isset(fpool->info, itor - 1));

	// get item
	tb_pointer_t item = fpool->data + (itor - 1) * fpool->func.size;

	// free item
	if (fpool->func.free) fpool->func.free(&fpool->func, item);

	// clear data
	tb_memset(item, 0, fpool->func.size);
}
tb_void_t tb_fpool_del(tb_fpool_t* fpool, tb_size_t itor)
{
	tb_assert_and_check_return(fpool && fpool->size && itor > 0 && itor < 1 + fpool->maxn);
	tb_assert_and_check_return(tb_pool_info_isset(fpool->info, itor - 1));

	// get item
	tb_pointer_t item = fpool->data + (itor - 1) * fpool->func.size;

	// free item
	if (fpool->func.free) fpool->func.free(&fpool->func, item);

	// reset info
	tb_pool_info_reset(fpool->info, itor - 1);

	// predict next
#ifdef TB_FPOOL_PRED_ENABLE
	if (fpool->pred_n < TB_FPOOL_PRED_MAX)
		fpool->pred[fpool->pred_n++] = itor;
#endif
	// update fpool size
	fpool->size--;
}
tb_void_t tb_fpool_clear(tb_fpool_t* fpool)
{
	// free items
	if (fpool->func.free && fpool->data) 
	{
		tb_int_t i = 0;
		for (i = 0; i < fpool->maxn; ++i)
		{
			if (tb_pool_info_isset(fpool->info, i))
				fpool->func.free(&fpool->func, fpool->data + i * fpool->func.size);
		}
	}

	// clear info
	fpool->size = 0;
	if (fpool->info) tb_memset(fpool->info, 0, fpool->maxn >> 3);
	if (fpool->data) tb_memset(fpool->data, 0, fpool->maxn * fpool->func.size);

#ifdef TB_FPOOL_PRED_ENABLE
	tb_size_t m = tb_min(fpool->maxn, TB_FPOOL_PRED_MAX);
	tb_size_t n = m;
	while (n--) fpool->pred[n] = m - n;
	fpool->pred_n = m;
#endif
}
tb_pointer_t tb_fpool_itor_at(tb_fpool_t* fpool, tb_size_t itor)
{
	return tb_fpool_get(fpool, itor);
}
tb_cpointer_t tb_fpool_itor_const_at(tb_fpool_t const* fpool, tb_size_t itor)
{
	return tb_fpool_get((tb_fpool_t*)fpool, itor);
}
tb_size_t tb_fpool_itor_head(tb_fpool_t const* fpool)
{
	tb_assert_abort(fpool);
	if (fpool->size)
	{
		// find the first item
		tb_size_t i = 0;
		tb_size_t n = fpool->maxn;
		for ( ; i < n; ++i)
		{
			// is non-free?
			if (tb_pool_info_isset(fpool->info, i))
				return (1 + i);
		}
	}
	return fpool->maxn;
}
tb_size_t tb_fpool_itor_tail(tb_fpool_t const* fpool)
{
	tb_assert_abort(fpool);
	return (fpool->maxn);
}
tb_size_t tb_fpool_itor_next(tb_fpool_t const* fpool, tb_size_t itor)
{
	tb_assert_abort(fpool && itor);
	if (fpool->size)
	{
		// find the next item
		tb_size_t i = itor;
		tb_size_t n = fpool->maxn;
		for ( ; i < n; ++i)
		{
			// is non-free?
			if (tb_pool_info_isset(fpool->info, i))
				return (1 + i);
		}
	}
	return fpool->maxn;
}

tb_size_t tb_fpool_size(tb_fpool_t const* fpool)
{
	tb_assert_and_check_return_val(fpool, 0);
	return fpool->size;
}
tb_size_t tb_fpool_maxn(tb_fpool_t const* fpool)
{
	tb_assert_and_check_return_val(fpool, 0);
	return fpool->maxn;
}
tb_size_t tb_fpool_step(tb_fpool_t const* fpool)
{
	tb_assert_and_check_return_val(fpool, 0);
	return fpool->func.size;
}
#ifdef TB_DEBUG
tb_void_t tb_fpool_dump(tb_fpool_t* fpool)
{
	tb_trace("size: %d", fpool->size);
	tb_trace("maxn: %d", fpool->maxn);
	tb_trace("step: %d", fpool->func.size);
	tb_trace("grow: %d", fpool->grow);
	tb_trace("pred: %02d%%, fail: %d, total: %d", fpool->alloc_total? ((fpool->alloc_total - fpool->pred_failed) * 100 / fpool->alloc_total) : -1, fpool->pred_failed, fpool->alloc_total);
}
#endif
