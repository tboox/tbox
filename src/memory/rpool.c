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
 * @file		rpool.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rpool.h"
#include "fpool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_RPOOL_CHUNK_GROW 			(8)
#else
# 	define TB_RPOOL_CHUNK_GROW 			(16)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the pool chunk type
typedef struct __tb_rpool_chunk_t
{
	// the fpool
	tb_handle_t 	pool;

	// the chunk
	tb_byte_t* 		data;
	tb_size_t 		size;

}tb_rpool_chunk_t;

// the rpool info type
#ifdef TB_DEBUG
typedef struct __tb_rpool_info_t
{
	// the pred count
	tb_size_t 			pred;

	// the aloc count
	tb_size_t 			aloc;

}tb_rpool_info_t;
#endif

// the small or string pool
typedef struct __tb_rpool_t
{
	// the pools align
	tb_size_t 			align;

	// the size
	tb_size_t 			size;

	// the chunk pools
	tb_rpool_chunk_t* 	pools;
	tb_size_t 			pooln;
	tb_size_t 			poolm;

	// the chunk step
	tb_size_t 			step;

	// the chunk grow
	tb_size_t 			grow;

	// the chunk pred
	tb_size_t 			pred;

	// the info
#ifdef TB_DEBUG
	tb_rpool_info_t 	info;
#endif

}tb_rpool_t;


/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_rpool_init(tb_size_t grow, tb_size_t step, tb_size_t align)
{
	tb_assert_and_check_return_val(grow && step, TB_NULL);

	// init rpool
	tb_rpool_t* rpool = (tb_rpool_t*)tb_malloc0(sizeof(tb_rpool_t));
	tb_assert_and_check_return_val(rpool, TB_NULL);

	// init pools size
	rpool->size = 0;

	// init pools align
	rpool->align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
	rpool->align = tb_max(rpool->align, TB_CPU_BITBYTE);

	// init chunk step
	rpool->step = step;

	// init chunk grow
	rpool->grow = tb_align_pow2((sizeof(tb_size_t) << 3) + grow * step);

	// init chunk pools
	rpool->pooln = 0;
	rpool->poolm = TB_RPOOL_CHUNK_GROW;
	rpool->pools = (tb_handle_t*)tb_nalloc0(TB_RPOOL_CHUNK_GROW, sizeof(tb_rpool_chunk_t));
	tb_assert_and_check_goto(rpool->pools, fail);

	// init chunk pred
	rpool->pred = 0;

	// init info
#ifdef TB_DEBUG
	rpool->info.pred = 0;
	rpool->info.aloc = 0;
#endif

	// ok
	return (tb_handle_t)rpool;

fail:
	if (rpool) tb_rpool_exit(rpool);
	return TB_NULL;
}

tb_void_t tb_rpool_exit(tb_handle_t handle)
{
	// check 
	tb_rpool_t* rpool = (tb_rpool_t*)handle;
	tb_assert_and_check_return(rpool);

	// clear
	tb_rpool_clear(handle);

	// free pools
	if (rpool->pools) 
	{	
		tb_size_t i = 0;
		tb_size_t n = rpool->pooln;
		for (i = 0; i < n; i++)
		{
			if (rpool->pools[i].pool) 
				tb_fpool_exit(rpool->pools[i].pool);

			if (rpool->pools[i].data) 
				tb_free(rpool->pools[i].data);
		}

		tb_free(rpool->pools);
	}

	// free rpool
	tb_free(rpool);
}
tb_void_t tb_rpool_clear(tb_handle_t handle)
{
	// check 
	tb_rpool_t* rpool = (tb_rpool_t*)handle;
	tb_assert_and_check_return(rpool && rpool->pools);

	// clear pools
	tb_size_t i = 0;
	tb_size_t n = rpool->pooln;
	for (i = 0; i < n; i++)
	{
		if (rpool->pools[i].pool) 
			tb_fpool_clear(rpool->pools[i].pool);
	}
	
	// reinit size
	rpool->size = 0;

	// reinit grow
	rpool->grow = 0;

	// reinit pred
	rpool->pred = 0;

	// reinit info
#ifdef TB_DEBUG
	rpool->info.pred = 0;
	rpool->info.aloc = 0;
#endif

}
tb_size_t tb_rpool_size(tb_handle_t handle)
{
	// check 
	tb_rpool_t* rpool = (tb_rpool_t*)handle;
	tb_assert_and_check_return_val(rpool, 0);

	return rpool->size;
}
tb_pointer_t tb_rpool_malloc(tb_handle_t handle)
{
	// check 
	tb_rpool_t* rpool = (tb_rpool_t*)handle;
	tb_assert_and_check_return_val(rpool && rpool->pools, TB_NULL);

	// aloc++
#ifdef TB_DEBUG
	rpool->info.aloc++;
#endif

	// allocate it from the predicted pool first
	if (rpool->pred)
	{
		// check
		tb_assert_and_check_return_val(rpool->pred <= rpool->pooln, TB_NULL);

		// the predicted pool
		tb_handle_t fpool = rpool->pools[rpool->pred - 1].pool;
		if (fpool) 
		{
			// try allocating it
			tb_pointer_t p = tb_fpool_malloc(fpool);

			// ok
			if (p) 
			{
				// size++
				rpool->size++;

				// pred++
#ifdef TB_DEBUG
				rpool->info.pred++;
#endif
				return p;
			}
		}
	}

	// allocate it from the existing pool
	if (rpool->pooln)
	{
		// allocate it from the last pool
		tb_size_t n = rpool->pooln;
		while (n--)
		{
			tb_handle_t fpool = rpool->pools[n].pool;
			if (fpool) 
			{
				// try allocating it
				tb_pointer_t p = tb_fpool_malloc(fpool);

				// ok
				if (p) 
				{
					// size++
					rpool->size++;

					// pred
					rpool->pred = n + 1;
					return p;
				}
			}
		}
	}

	// grow pools if not enough
	if (rpool->pooln >= rpool->poolm)
	{
		// grow
		rpool->poolm += TB_RPOOL_CHUNK_GROW;
		rpool->pools = (tb_handle_t*)tb_ralloc(rpool->pools, rpool->poolm * sizeof(tb_rpool_chunk_t));
		tb_assert_and_check_return_val(rpool->pools, TB_NULL);
	}
	
	// append a new pool for allocation
	tb_rpool_chunk_t* chunk = &rpool->pools[rpool->pooln];
	do
	{
		// clear the chunk
		tb_memset(chunk, 0, sizeof(tb_rpool_chunk_t));

		// alloc chunk data
		chunk->size = rpool->grow;
		chunk->data = tb_malloc(chunk->size);

		// no space?
		tb_check_break(chunk->data);

		// init chunk pool
		chunk->pool = tb_fpool_init(chunk->data, chunk->size, rpool->step, rpool->align);
		tb_assert_and_check_break(chunk->pool);

		// try allocating it
		tb_pointer_t p = tb_fpool_malloc(chunk->pool);

		// ok
		if (p) 
		{
			// size++
			rpool->size++;

			// pred
			rpool->pred = ++rpool->pooln;
			return p;
		}
		
		tb_assert_message(0, "the chunk size may be too small: %lu < %lu", chunk->size, rpool->step);

	} while (0);

	// clean chunk
	if (chunk->pool) tb_fpool_exit(chunk->pool);
	if (chunk->data) tb_free(chunk->data);
	tb_memset(chunk, 0, sizeof(tb_rpool_chunk_t));

	// fail
	return TB_NULL;
}

tb_pointer_t tb_rpool_malloc0(tb_handle_t handle)
{
	// check 
	tb_rpool_t* rpool = (tb_rpool_t*)handle;
	tb_assert_and_check_return_val(rpool && rpool->pools, TB_NULL);

	// malloc
	tb_byte_t* p = tb_rpool_malloc(handle);

	// clear
	if (p) tb_memset(p, 0, rpool->step);

	// ok?
	return p;
}

tb_bool_t tb_rpool_free(tb_handle_t handle, tb_pointer_t data)
{
	// check 
	tb_rpool_t* rpool = (tb_rpool_t*)handle;
	tb_assert_and_check_return_val(rpool && rpool->pools && rpool->size, TB_FALSE);

	// no data?
	tb_check_return_val(data, TB_TRUE);
	
	// free it from the predicted pool first
	if (rpool->pred)
	{
		// check
		tb_assert_and_check_return_val(rpool->pred <= rpool->pooln, TB_NULL);

		// the predicted pool
		tb_handle_t fpool = rpool->pools[rpool->pred - 1].pool;
		if (fpool) 
		{
			// try allocating it
			tb_bool_t r = tb_fpool_free(fpool, data);

			// ok
			if (r) 
			{
				// size--
				rpool->size--;
				return TB_TRUE;
			}
		}
	}

	// free it from the existing pool
	tb_size_t n = rpool->pooln;
	while (n--)
	{
		tb_handle_t fpool = rpool->pools[n].pool;
		if (fpool) 
		{
			// try free it
			tb_bool_t r = tb_fpool_free(fpool, data);

			// ok
			if (r) 
			{
				// size--
				rpool->size--;
		
				// pred
				rpool->pred = n + 1;
				return TB_TRUE;
			}
		}
	}

	// fail
	tb_assert_message(0, "invalid free data address: %p", data);
	return TB_FALSE;
}

tb_pointer_t tb_rpool_memdup(tb_handle_t handle, tb_pointer_t data)
{
	// check 
	tb_rpool_t* rpool = (tb_rpool_t*)handle;
	tb_assert_and_check_return_val(rpool && data, TB_NULL);

	tb_size_t 	n = rpool->step;
	tb_char_t* 	p = tb_rpool_malloc(handle);
	if (p) tb_memcpy(p, data, n);
	return p;
}

#ifdef TB_DEBUG
tb_void_t tb_rpool_dump(tb_handle_t handle)
{
	// check 
	tb_rpool_t* rpool = (tb_rpool_t*)handle;
	tb_assert_and_check_return(rpool && rpool->pools);

	tb_print("======================================================================");
	tb_print("rpool: align: %lu", 	rpool->align);
	tb_print("rpool: pooln: %lu", 	rpool->pooln);
	tb_print("rpool: poolm: %lu", 	rpool->poolm);
	tb_print("rpool: size: %lu", 	rpool->size);
	tb_print("rpool: grow: %lu", 	rpool->grow);
	tb_print("rpool: pred: %lu%%", 	rpool->info.aloc? ((rpool->info.pred * 100) / rpool->info.aloc) : 0);

	tb_size_t i = 0;
	tb_size_t n = rpool->pooln;
	for (i = 0; i < n; i++)
	{
		tb_handle_t fpool = rpool->pools[i].pool;
		if (fpool) tb_fpool_dump(fpool);
	}
}
#endif
