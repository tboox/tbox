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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		block_pool.c
 * @ingroup 	memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "memory.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__
# 	define TB_BLOCK_POOL_CHUNK_GROW 			(8)
#else
# 	define TB_BLOCK_POOL_CHUNK_GROW 			(16)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the pool chunk type
typedef struct __tb_block_pool_chunk_t
{
	// the static block pool
	tb_handle_t 			pool;

	// the chunk
	tb_byte_t* 				data;
	tb_size_t 				size;

}tb_block_pool_chunk_t;

/// the pool info type
#ifdef __tb_debug__
typedef struct __tb_block_pool_info_t
{
	// the pred count
	tb_size_t 				pred;

	// the aloc count
	tb_size_t 				aloc;

}tb_block_pool_info_t;
#endif

/// the small or string pool
typedef struct __tb_block_pool_t
{
	// the pools align
	tb_size_t 				align;

	// the chunk pools
	tb_block_pool_chunk_t* 	pools;

	// the chunk pool count
	tb_size_t 				pooln;

	// the chunk pool maxn
	tb_size_t 				poolm;

	// the chunk grow
	tb_size_t 				grow;

	// the chunk pred
	tb_size_t 				pred;

	// the info
#ifdef __tb_debug__
	tb_block_pool_info_t 	info;
#endif

}tb_block_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_pointer_t tb_static_block_pool_ralloc_fast(tb_pointer_t bpool, tb_pointer_t data, tb_size_t size, tb_size_t* osize);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_pointer_t tb_block_pool_ralloc_fast(tb_block_pool_t* pool, tb_pointer_t data, tb_size_t size, tb_size_t* osize)
{
	// check 
	tb_assert_and_check_return_val(pool && pool->pools, tb_null);

	// no size?
	tb_check_return_val(size, tb_null);

	// check osize
	tb_assert_and_check_return_val(osize && !*osize, tb_null);
	
	// reallocate it from the predicted pool first
	if (pool->pred)
	{
		// check
		tb_assert_and_check_return_val(pool->pred <= pool->pooln, tb_null);

		// the predicted pool
		tb_handle_t bpool = pool->pools[pool->pred - 1].pool;
		if (bpool) 
		{
			// try reallocating it
			tb_pointer_t p = tb_static_block_pool_ralloc_fast(bpool, data, size, osize);

			// ok?
			tb_check_return_val(!p, p);

			// no space?
			tb_check_return_val(!*osize, tb_null);
		}
	}

	// reallocate it from the existing pool
	if (pool->pooln)
	{
		// allocate it from the last pool
		tb_size_t n = pool->pooln;
		while (n--)
		{
			tb_handle_t bpool = pool->pools[n].pool;
			if (bpool) 
			{
				// try reallocating it
				tb_pointer_t p = tb_static_block_pool_ralloc_fast(bpool, data, size, osize);

				// ok?
				tb_check_return_val(!p, p);

				// no space?
				tb_check_return_val(!*osize, tb_null);
			}
		}
	}

	return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_block_pool_init(tb_size_t grow, tb_size_t align)
{
	tb_assert_and_check_return_val(grow, tb_null);

	// init pool
	tb_block_pool_t* pool = (tb_block_pool_t*)tb_malloc0(sizeof(tb_block_pool_t));
	tb_assert_and_check_return_val(pool, tb_null);

	// init pools align
	pool->align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
	pool->align = tb_max(pool->align, TB_CPU_BITBYTE);

	// init chunk grow
	pool->grow = grow;

	// init chunk pools
	pool->pooln = 0;
	pool->poolm = TB_BLOCK_POOL_CHUNK_GROW;
	pool->pools = (tb_block_pool_chunk_t*)tb_nalloc0(TB_BLOCK_POOL_CHUNK_GROW, sizeof(tb_block_pool_chunk_t));
	tb_assert_and_check_goto(pool->pools, fail);

	// init chunk pred
	pool->pred = 0;

	// init info
#ifdef __tb_debug__
	pool->info.pred = 0;
	pool->info.aloc = 0;
#endif

	// ok
	return (tb_handle_t)pool;

fail:
	if (pool) tb_block_pool_exit(pool);
	return tb_null;
}

tb_void_t tb_block_pool_exit(tb_handle_t handle)
{
	// check 
	tb_block_pool_t* pool = (tb_block_pool_t*)handle;
	tb_assert_and_check_return(pool);

	// clear
	tb_block_pool_clear(handle);

	// free pools
	if (pool->pools) 
	{	
		tb_size_t i = 0;
		tb_size_t n = pool->pooln;
		for (i = 0; i < n; i++)
		{
			if (pool->pools[i].pool) 
				tb_static_block_pool_exit(pool->pools[i].pool);

			if (pool->pools[i].data) 
				tb_free(pool->pools[i].data);
		}

		tb_free(pool->pools);
	}

	// free pool
	tb_free(pool);
}
tb_void_t tb_block_pool_clear(tb_handle_t handle)
{
	// check 
	tb_block_pool_t* pool = (tb_block_pool_t*)handle;
	tb_assert_and_check_return(pool && pool->pools);

	// clear pools
	tb_size_t i = 0;
	tb_size_t n = pool->pooln;
	for (i = 0; i < n; i++)
	{
		if (pool->pools[i].pool) 
			tb_static_block_pool_clear(pool->pools[i].pool);
	}
	
	// reinit grow
	pool->grow = 0;

	// reinit pred
	pool->pred = 0;

	// reinit info
#ifdef __tb_debug__
	pool->info.pred = 0;
	pool->info.aloc = 0;
#endif

}
tb_pointer_t tb_block_pool_malloc_(tb_handle_t handle, tb_size_t size __tb_debug_decl__)
{
	// check 
	tb_block_pool_t* pool = (tb_block_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->pools, tb_null);

	// no size?
	tb_check_return_val(size, tb_null);
	
	// aloc++
#ifdef __tb_debug__
	pool->info.aloc++;
#endif

	// allocate it from the predicted pool first
	if (pool->pred)
	{
		// check
		tb_assert_and_check_return_val(pool->pred <= pool->pooln, tb_null);

		// the predicted pool
		tb_handle_t bpool = pool->pools[pool->pred - 1].pool;
		if (bpool) 
		{
			// try allocating it
			tb_pointer_t p = tb_static_block_pool_malloc_(bpool, size __tb_debug_args__);

			// ok
			if (p) 
			{
				// pred++
#ifdef __tb_debug__
				pool->info.pred++;
#endif
				return p;
			}
		}
	}

	// allocate it from the existing pool
	if (pool->pooln)
	{
		// allocate it from the last pool
		tb_size_t n = pool->pooln;
		while (n--)
		{
			tb_handle_t bpool = pool->pools[n].pool;
			if (bpool) 
			{
				// try allocating it
				tb_pointer_t p = tb_static_block_pool_malloc_(bpool, size __tb_debug_args__);

				// ok
				if (p) 
				{
					pool->pred = n + 1;
					return p;
				}
			}
		}
	}

	// grow pools if not enough
	if (pool->pooln >= pool->poolm)
	{
		// grow
		pool->poolm += TB_BLOCK_POOL_CHUNK_GROW;
		pool->pools = (tb_block_pool_chunk_t*)tb_ralloc(pool->pools, pool->poolm * sizeof(tb_block_pool_chunk_t));
		tb_assert_and_check_return_val(pool->pools, tb_null);
	}
	
	// append a new pool for allocation
	tb_block_pool_chunk_t* chunk = &pool->pools[pool->pooln];
	do
	{
		// clear the chunk
		tb_memset(chunk, 0, sizeof(tb_block_pool_chunk_t));

		// alloc chunk data
		chunk->size = pool->grow;
		chunk->data = tb_malloc(chunk->size);

		// no space
		tb_check_break(chunk->data);

		// init chunk pool
		chunk->pool = tb_static_block_pool_init(chunk->data, chunk->size, pool->align);
		tb_assert_and_check_break(chunk->pool);

		// try allocating it
		tb_pointer_t p = tb_static_block_pool_malloc_(chunk->pool, size __tb_debug_args__);

		// ok
		if (p) 
		{
			pool->pred = ++pool->pooln;
			return p;
		}
		
		tb_assert_message(0, "the chunk size may be too small: %lu < %lu", chunk->size, size);

	} while (0);

	// clean chunk
	if (chunk->pool) tb_static_block_pool_exit(chunk->pool);
	if (chunk->data) tb_free(chunk->data);
	tb_memset(chunk, 0, sizeof(tb_block_pool_chunk_t));

	// fail
	return tb_null;
}
tb_pointer_t tb_block_pool_malloc0_(tb_handle_t handle, tb_size_t size __tb_debug_decl__)
{
	// malloc
	tb_byte_t* p = tb_block_pool_malloc_(handle, size __tb_debug_args__);

	// clear
	if (p && size) tb_memset(p, 0, size);

	// ok?
	return p;
}
tb_pointer_t tb_block_pool_nalloc_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// malloc
	return tb_block_pool_malloc_(handle, item * size __tb_debug_args__);
}

tb_pointer_t tb_block_pool_nalloc0_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// malloc
	return tb_block_pool_malloc0_(handle, item * size __tb_debug_args__);
}

tb_pointer_t tb_block_pool_ralloc_(tb_handle_t handle, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
	// check
	tb_block_pool_t* pool = (tb_block_pool_t*)handle;
	tb_assert_and_check_return_val(pool, tb_null);

	// free it if no size
	if (!size)
	{
		tb_block_pool_free_(pool, data __tb_debug_args__);
		return tb_null;
	}

	// alloc it if no data?
	if (!data) return tb_block_pool_malloc_(pool, size __tb_debug_args__);
	
	// ralloc it with fast mode if enough
	tb_size_t 		osize = 0;
	tb_pointer_t 	pdata = tb_block_pool_ralloc_fast(pool, data, size, &osize);
	tb_check_return_val(!pdata, pdata);
	tb_assert_and_check_return_val(osize < size, tb_null);

	// malloc it
	pdata = tb_block_pool_malloc_(pool, size __tb_debug_args__);
	tb_check_return_val(pdata, tb_null);
	tb_assert_and_check_return_val(pdata != data, pdata);

	// copy data
	tb_memcpy(pdata, data, osize);
	
	// free it
	tb_block_pool_free_(pool, data __tb_debug_args__);

	// ok
	return pdata;
}
tb_char_t* tb_block_pool_strdup_(tb_handle_t handle, tb_char_t const* data __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(handle && data, tb_null);

	tb_size_t 	n = tb_strlen(data);
	tb_char_t* 	p = tb_block_pool_malloc_(handle, n + 1 __tb_debug_args__);
	if (p)
	{
		tb_memcpy(p, data, n);
		p[n] = '\0';
	}

	return p;
}

tb_char_t* tb_block_pool_strndup_(tb_handle_t handle, tb_char_t const* data, tb_size_t size __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(handle && data, tb_null);

	size = tb_strnlen(data, size);
	tb_char_t* 	p = tb_block_pool_malloc_(handle, size + 1 __tb_debug_args__);
	if (p)
	{
		tb_memcpy(p, data, size);
		p[size] = '\0';
	}

	return p;
}
tb_bool_t tb_block_pool_free_(tb_handle_t handle, tb_pointer_t data __tb_debug_decl__)
{
	// check 
	tb_block_pool_t* pool = (tb_block_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->pools, tb_false);

	// no data?
	tb_check_return_val(data, tb_true);
	
	// free it from the predicted pool first
	if (pool->pred)
	{
		// check
		tb_assert_and_check_return_val(pool->pred <= pool->pooln, tb_false);

		// the predicted pool
		tb_handle_t bpool = pool->pools[pool->pred - 1].pool;
		if (bpool) 
		{
			// try allocating it
			tb_bool_t r = tb_static_block_pool_free_(bpool, data __tb_debug_args__);

			// ok
			if (r) return r;
		}
	}

	// free it from the existing pool
	tb_size_t n = pool->pooln;
	while (n--)
	{
		tb_handle_t bpool = pool->pools[n].pool;
		if (bpool) 
		{
			// try free it
			tb_bool_t r = tb_static_block_pool_free_(bpool, data __tb_debug_args__);

			// ok
			if (r) 
			{
				pool->pred = n + 1;
				return r;
			}
		}
	}

	// fail
	tb_assert_message(0, "invalid free data address: %p", data);
	return tb_false;
}

#ifdef __tb_debug__
tb_void_t tb_block_pool_dump(tb_handle_t handle, tb_char_t const* prefix)
{
	// check 
	tb_block_pool_t* pool = (tb_block_pool_t*)handle;
	tb_assert_and_check_return(pool && pool->pools);

	// prefix
	if (!prefix) prefix = "pool";

	// trace
#if 0
	tb_trace_i("======================================================================");
	tb_trace_i("%s: align: %lu", 	prefix, pool->align);
	tb_trace_i("%s: pooln: %lu", 	prefix, pool->pooln);
	tb_trace_i("%s: poolm: %lu", 	prefix, pool->poolm);
	tb_trace_i("%s: grow: %lu", 	prefix, pool->grow);
	tb_trace_i("%s: pred: %lu%%", 	prefix, pool->info.aloc? ((pool->info.pred * 100) / pool->info.aloc) : 0);
#endif

	// dump
	tb_size_t i = 0;
	tb_size_t n = pool->pooln;
	for (i = 0; i < n; i++)
	{
		tb_handle_t bpool = pool->pools[i].pool;
		if (bpool) tb_static_block_pool_dump(bpool, prefix);
	}
}
#endif
