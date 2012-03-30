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
 * \author		ruki
 * \file		spool.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "spool.h"
#include "vpool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_SPOOL_CHUNK_GROW 			(8)
#else
# 	define TB_SPOOL_CHUNK_GROW 			(16)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the pool chunk type
typedef struct __tb_spool_chunk_t
{
	// the vpool
	tb_handle_t 	pool;

	// the chunk
	tb_byte_t* 		data;
	tb_size_t 		size;

}tb_spool_chunk_t;

// the spool info type
#ifdef TB_DEBUG
typedef struct __tb_spool_info_t
{
	// the pred count
	tb_size_t 			pred;

	// the aloc count
	tb_size_t 			aloc;

}tb_spool_info_t;
#endif

// the small or string pool
typedef struct __tb_spool_t
{
	// the pools align
	tb_size_t 			align;

	// the chunk pools
	tb_spool_chunk_t* 	pools;
	tb_size_t 			pooln;
	tb_size_t 			poolm;

	// the chunk grow
	tb_size_t 			grow;

	// the chunk pred
	tb_size_t 			pred;

	// the info
#ifdef TB_DEBUG
	tb_spool_info_t 	info;
#endif

}tb_spool_t;

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_pointer_t tb_vpool_ralloc_fast(tb_pointer_t vpool, tb_pointer_t data, tb_size_t size, tb_size_t* osize);

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
static tb_pointer_t tb_spool_ralloc_fast(tb_spool_t* spool, tb_pointer_t data, tb_size_t size, tb_size_t* osize)
{
	// check 
	tb_assert_and_check_return_val(spool && spool->pools, TB_NULL);

	// no size?
	tb_check_return_val(size, TB_NULL);

	// check osize
	tb_assert_and_check_return_val(osize && !*osize, TB_NULL);
	
	// reallocate it from the predicted pool first
	if (spool->pred)
	{
		// check
		tb_assert_and_check_return_val(spool->pred <= spool->pooln, TB_NULL);

		// the predicted pool
		tb_handle_t vpool = spool->pools[spool->pred - 1].pool;
		if (vpool) 
		{
			// try reallocating it
			tb_pointer_t p = tb_vpool_ralloc_fast(vpool, data, size, osize);

			// ok?
			tb_check_return_val(!p, p);

			// no space?
			tb_check_return_val(!*osize, TB_NULL);
		}
	}

	// reallocate it from the existing pool
	if (spool->pooln)
	{
		// allocate it from the last pool
		tb_size_t n = spool->pooln;
		while (n--)
		{
			tb_handle_t vpool = spool->pools[n].pool;
			if (vpool) 
			{
				// try reallocating it
				tb_pointer_t p = tb_vpool_ralloc_fast(vpool, data, size, osize);

				// ok?
				tb_check_return_val(!p, p);

				// no space?
				tb_check_return_val(!*osize, TB_NULL);
			}
		}
	}

	return TB_NULL;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_spool_init(tb_size_t grow, tb_size_t align)
{
	tb_assert_and_check_return_val(grow, TB_NULL);

	// init spool
	tb_spool_t* spool = (tb_spool_t*)tb_malloc0(sizeof(tb_spool_t));
	tb_assert_and_check_return_val(spool, TB_NULL);

	// init pools align
	spool->align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
	spool->align = tb_max(spool->align, TB_CPU_BITBYTE);

	// init chunk grow
	spool->grow = grow;

	// init chunk pools
	spool->pooln = 0;
	spool->poolm = TB_SPOOL_CHUNK_GROW;
	spool->pools = (tb_handle_t*)tb_nalloc0(TB_SPOOL_CHUNK_GROW, sizeof(tb_spool_chunk_t));
	tb_assert_and_check_goto(spool->pools, fail);

	// init chunk pred
	spool->pred = 0;

	// init info
#ifdef TB_DEBUG
	spool->info.pred = 0;
	spool->info.aloc = 0;
#endif

	// ok
	return (tb_handle_t)spool;

fail:
	if (spool) tb_spool_exit(spool);
	return TB_NULL;
}

tb_void_t tb_spool_exit(tb_handle_t handle)
{
	// check 
	tb_spool_t* spool = (tb_spool_t*)handle;
	tb_assert_and_check_return(spool);

	// clear
	tb_spool_clear(handle);

	// free pools
	if (spool->pools) 
	{	
		tb_size_t i = 0;
		tb_size_t n = spool->pooln;
		for (i = 0; i < n; i++)
		{
			if (spool->pools[i].pool) 
				tb_vpool_exit(spool->pools[i].pool);

			if (spool->pools[i].data) 
				tb_free(spool->pools[i].data);
		}

		tb_free(spool->pools);
	}

	// free spool
	tb_free(spool);
}
tb_void_t tb_spool_clear(tb_handle_t handle)
{
	// check 
	tb_spool_t* spool = (tb_spool_t*)handle;
	tb_assert_and_check_return(spool && spool->pools);

	// clear pools
	tb_size_t i = 0;
	tb_size_t n = spool->pooln;
	for (i = 0; i < n; i++)
	{
		if (spool->pools[i].pool) 
			tb_vpool_clear(spool->pools[i].pool);
	}
	
	// reinit grow
	spool->grow = 0;

	// reinit pred
	spool->pred = 0;

	// reinit info
#ifdef TB_DEBUG
	spool->info.pred = 0;
	spool->info.aloc = 0;
#endif

}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_malloc_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_spool_malloc_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_spool_t* spool = (tb_spool_t*)handle;
	tb_assert_and_check_return_val(spool && spool->pools, TB_NULL);

	// no size?
	tb_check_return_val(size, TB_NULL);
	
	// the align size
	tb_size_t asize = tb_align(size, spool->align);

	// aloc++
#ifdef TB_DEBUG
	spool->info.aloc++;
#endif

	// allocate it from the predicted pool first
	if (spool->pred)
	{
		// check
		tb_assert_and_check_return_val(spool->pred <= spool->pooln, TB_NULL);

		// the predicted pool
		tb_handle_t vpool = spool->pools[spool->pred - 1].pool;
		if (vpool) 
		{
			// try allocating it
#ifndef TB_DEBUG
			tb_pointer_t p = tb_vpool_malloc_impl(vpool, size);
#else
			tb_pointer_t p = tb_vpool_malloc_impl(vpool, size, func, line, func);
#endif

			// ok
			if (p) 
			{
				// pred++
#ifdef TB_DEBUG
				spool->info.pred++;
#endif
				return p;
			}
		}
	}

	// allocate it from the existing pool
	if (spool->pooln)
	{
		// allocate it from the last pool
		tb_size_t n = spool->pooln;
		while (n--)
		{
			tb_handle_t vpool = spool->pools[n].pool;
			if (vpool) 
			{
				// try allocating it
#ifndef TB_DEBUG
				tb_pointer_t p = tb_vpool_malloc_impl(vpool, size);
#else
				tb_pointer_t p = tb_vpool_malloc_impl(vpool, size, func, line, func);
#endif
				// ok
				if (p) 
				{
					spool->pred = n + 1;
					return p;
				}
			}
		}
	}

	// grow pools if not enough
	if (spool->pooln >= spool->poolm)
	{
		// grow
		spool->poolm += TB_SPOOL_CHUNK_GROW;
		spool->pools = (tb_handle_t*)tb_ralloc(spool->pools, spool->poolm * sizeof(tb_spool_chunk_t));
		tb_assert_and_check_return_val(spool->pools, TB_NULL);
	}
	
	// append a new pool for allocation
	tb_spool_chunk_t* chunk = &spool->pools[spool->pooln];
	do
	{
		// clear the chunk
		tb_memset(chunk, 0, sizeof(tb_spool_chunk_t));

		// alloc chunk data
		chunk->size = spool->grow;
		chunk->data = tb_malloc(chunk->size);

		// no space
		tb_check_break(chunk->data);

		// init chunk pool
		chunk->pool = tb_vpool_init(chunk->data, chunk->size, spool->align);
		tb_assert_and_check_break(chunk->pool);

		// try allocating it
#ifndef TB_DEBUG
		tb_pointer_t p = tb_vpool_malloc_impl(chunk->pool, size);
#else
		tb_pointer_t p = tb_vpool_malloc_impl(chunk->pool, size, func, line, func);
#endif

		// ok
		if (p) 
		{
			spool->pred = ++spool->pooln;
			return p;
		}
		
		tb_assert_message(0, "the chunk size may be too small: %lu < %lu", chunk->size, size);

	} while (0);

	// clean chunk
	if (chunk->pool) tb_vpool_exit(chunk->pool);
	if (chunk->data) tb_free(chunk->data);
	tb_memset(chunk, 0, sizeof(tb_spool_chunk_t));

	// fail
	return TB_NULL;
}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_malloc0_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_spool_malloc0_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// malloc
#ifndef TB_DEBUG
	tb_byte_t* p = tb_spool_malloc_impl(handle, size);
#else
	tb_byte_t* p = tb_spool_malloc_impl(handle, size, func, line, file);
#endif

	// clear
	if (p && size) tb_memset(p, 0, size);

	// ok?
	return p;
}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_spool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_assert_and_check_return_val(item, TB_NULL);

	// malloc
#ifndef TB_DEBUG
	return tb_spool_malloc_impl(handle, item * size);
#else
	return tb_spool_malloc_impl(handle, item * size, func, line, file);
#endif
}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_spool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_assert_and_check_return_val(item, TB_NULL);

	// malloc
#ifndef TB_DEBUG
	return tb_spool_malloc0_impl(handle, item * size);
#else
	return tb_spool_malloc0_impl(handle, item * size, func, line, file);
#endif
}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_spool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_spool_t* spool = (tb_spool_t*)handle;
	tb_assert_and_check_return_val(spool, TB_NULL);

	// free it if no size
	if (!size)
	{
#ifndef TB_DEBUG
		tb_spool_free_impl(spool, data);
#else
		tb_spool_free_impl(spool, data, func, line, file);
#endif
		return TB_NULL;
	}

	// alloc it if no data?
#ifndef TB_DEBUG
	if (!data) return tb_spool_malloc_impl(spool, size);
#else
	if (!data) return tb_spool_malloc_impl(spool, size, func, line, file);
#endif
	
	// ralloc it with fast mode if enough
	tb_size_t 		osize = 0;
	tb_pointer_t 	pdata = tb_spool_ralloc_fast(spool, data, size, &osize);
	tb_check_return_val(!pdata, pdata);
	tb_assert_and_check_return_val(osize < size, TB_NULL);

	// malloc it
#ifndef TB_DEBUG
	pdata = tb_spool_malloc_impl(spool, size);
#else
	pdata = tb_spool_malloc_impl(spool, size, func, line, file);
#endif
	tb_check_return_val(pdata, TB_NULL);
	tb_assert_and_check_return_val(pdata != data, pdata);

	// copy data
	tb_memcpy(pdata, data, osize);
	
	// free it
#ifndef TB_DEBUG
	tb_spool_free_impl(spool, data);
#else
	tb_spool_free_impl(spool, data, func, line, file);
#endif

	// ok
	return pdata;
}

#ifndef TB_DEBUG
tb_char_t* tb_spool_strdup_impl(tb_handle_t handle, tb_char_t const* data)
#else
tb_char_t* tb_spool_strdup_impl(tb_handle_t handle, tb_char_t const* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	tb_assert_and_check_return_val(handle && data, TB_NULL);

	tb_size_t 	n = tb_strlen(data);
#ifndef TB_DEBUG
	tb_char_t* 	p = tb_spool_malloc_impl(handle, n + 1);
#else
	tb_char_t* 	p = tb_spool_malloc_impl(handle, n + 1, func, line, file);
#endif
	if (p)
	{
		tb_memcpy(p, data, n);
		p[n] = '\0';
	}

	return p;}

#ifndef TB_DEBUG
tb_char_t* tb_spool_strndup_impl(tb_handle_t handle, tb_char_t const* data, tb_size_t size)
#else
tb_char_t* tb_spool_strndup_impl(tb_handle_t handle, tb_char_t const* data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	tb_assert_and_check_return_val(handle && data, TB_NULL);

	size = tb_strnlen(data, size);
#ifndef TB_DEBUG
	tb_char_t* 	p = tb_spool_malloc_impl(handle, size + 1);
#else
	tb_char_t* 	p = tb_spool_malloc_impl(handle, size + 1, func, line, file);
#endif
	if (p)
	{
		tb_memcpy(p, data, size);
		p[size] = '\0';
	}

	return p;
}

#ifndef TB_DEBUG
tb_bool_t tb_spool_free_impl(tb_handle_t handle, tb_pointer_t data)
#else
tb_bool_t tb_spool_free_impl(tb_handle_t handle, tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_spool_t* spool = (tb_spool_t*)handle;
	tb_assert_and_check_return_val(spool && spool->pools, TB_FALSE);

	// no data?
	tb_check_return_val(data, TB_TRUE);
	
	// free it from the predicted pool first
	if (spool->pred)
	{
		// check
		tb_assert_and_check_return_val(spool->pred <= spool->pooln, TB_FALSE);

		// the predicted pool
		tb_handle_t vpool = spool->pools[spool->pred - 1].pool;
		if (vpool) 
		{
			// try allocating it
#ifndef TB_DEBUG
			tb_bool_t r = tb_vpool_free_impl(vpool, data);
#else
			tb_bool_t r = tb_vpool_free_impl(vpool, data, func, line, func);
#endif
			// ok
			if (r) return r;
		}
	}

	// free it from the existing pool
	tb_size_t n = spool->pooln;
	while (n--)
	{
		tb_handle_t vpool = spool->pools[n].pool;
		if (vpool) 
		{
			// try free it
#ifndef TB_DEBUG
			tb_bool_t r = tb_vpool_free_impl(vpool, data);
#else
			tb_bool_t r = tb_vpool_free_impl(vpool, data, func, line, func);
#endif
			// ok
			if (r) 
			{
				spool->pred = n + 1;
				return r;
			}
		}
	}

	// fail
	tb_assert_message(0, "invalid free data address: %p", data);
	return TB_FALSE;
}

#ifdef TB_DEBUG
tb_void_t tb_spool_dump(tb_handle_t handle)
{
	// check 
	tb_spool_t* spool = (tb_spool_t*)handle;
	tb_assert_and_check_return(spool && spool->pools);

	tb_print("======================================================================");
	tb_print("spool: align: %lu", 	spool->align);
	tb_print("spool: pooln: %lu", 	spool->pooln);
	tb_print("spool: poolm: %lu", 	spool->poolm);
	tb_print("spool: grow: %lu", 	spool->grow);
	tb_print("spool: pred: %lu%%", 	spool->info.aloc? ((spool->info.pred * 100) / spool->info.aloc) : 0);

	tb_size_t i = 0;
	tb_size_t n = spool->pooln;
	for (i = 0; i < n; i++)
	{
		tb_handle_t vpool = spool->pools[i].pool;
		if (vpool) tb_vpool_dump(vpool);
	}
}
#endif
