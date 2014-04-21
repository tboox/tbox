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
 * @file		static_fixed_pool.c
 * @ingroup 	memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_fixed_pool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_STATIC_FIXED_POOL_MAGIC 							(0xdeaf)

// the align maxn
#define TB_STATIC_FIXED_POOL_ALIGN_MAXN 					(128)

// the used sets
#define tb_static_fixed_pool_used_set1(used, i) 			do {(used)[(i) >> 3] |= (0x1 << ((i) & 7));} while (0)
#define tb_static_fixed_pool_used_set0(used, i) 			do {(used)[(i) >> 3] &= ~(0x1 << ((i) & 7));} while (0)
#define tb_static_fixed_pool_used_bset(used, i) 			((used)[(i) >> 3] & (0x1 << ((i) & 7)))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

#ifdef __tb_debug__
/// the static fixed pool info type
typedef struct __tb_static_fixed_pool_info_t
{
	/// the peak size
	tb_size_t 					peak;

	/// the fail count
	tb_size_t 					fail;

	/// the pred count
	tb_size_t 					pred;

	/// the aloc count
	tb_size_t 					aloc;

}tb_static_fixed_pool_info_t;
#endif

/*!the static fixed pool type
 *
 * <pre>
 * |---------|-----------------|-----------------------------------------------|
 *    head          used                            data     
 * </pre>
 */
typedef struct __tb_static_fixed_pool_t
{
	/// the magic 
	tb_uint16_t 				magic;

	/// the align
	tb_uint16_t 				align;

	/// the step
	tb_size_t 					step;

	/// the maxn
	tb_size_t 					maxn;

	/// the size
	tb_size_t 					size;

	/// the data
	tb_byte_t* 					data;

	/// the used
	tb_byte_t* 					used;

	/// the pred
	tb_byte_t* 					pred;

#ifdef __tb_debug__
	/// the info
	tb_static_fixed_pool_info_t info;
#endif

}tb_static_fixed_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_pointer_t tb_static_fixed_pool_malloc_pred(tb_static_fixed_pool_t* pool)
{
	// init data
	tb_pointer_t data = tb_null;

	// has pred?
	if (pool->pred)
	{
		// init index
		tb_size_t i = (pool->pred - pool->data) / pool->step;
		tb_assert_and_check_return_val(!((pool->pred - pool->data) % pool->step), tb_null);

		// is free?
		if (!tb_static_fixed_pool_used_bset(pool->used, i)) 
		{
			// ok
			data = pool->pred;
			tb_static_fixed_pool_used_set1(pool->used, i);

			// predict the next block
			if (i + 1 < pool->maxn && !tb_static_fixed_pool_used_bset(pool->used, i + 1))
				pool->pred = pool->data + (i + 1) * pool->step;

#ifdef __tb_debug__
			// pred++
			pool->info.pred++;
#endif
		}
	}

	// ok?
	return data;
}

#if 1
static tb_pointer_t tb_static_fixed_pool_malloc_find(tb_static_fixed_pool_t* pool)
{
	tb_size_t 	i = 0;
#if TB_CPU_BIT64
	tb_size_t 	m = tb_align(pool->maxn, 64) >> 6;
#elif TB_CPU_BIT32
	tb_size_t 	m = tb_align(pool->maxn, 32) >> 5;
#endif
	tb_size_t* 	p = (tb_size_t*)pool->used;
	tb_size_t* 	e = (tb_size_t*)pool->used + m;
	tb_byte_t* 	d = tb_null;

	// check align
	tb_assert_and_check_return_val(!(((tb_size_t)p) & (TB_CPU_BITBYTE - 1)), tb_null);

	// find the free chunk, step * 32|64 items
#if 0
//	while (p < e && *p == 0xffffffff) p++;
//	while (p < e && *p == 0xffffffffffffffffL) p++;
	while (p < e && !(*p + 1)) p++;
#else
	while (p + 7 < e)
	{
		if (p[0] + 1) { p += 0; break; }
		if (p[1] + 1) { p += 1; break; }
		if (p[2] + 1) { p += 2; break; }
		if (p[3] + 1) { p += 3; break; }
		if (p[4] + 1) { p += 4; break; }
		if (p[5] + 1) { p += 5; break; }
		if (p[6] + 1) { p += 6; break; }
		if (p[7] + 1) { p += 7; break; }
		p += 8;
	}
	while (p < e && !(*p + 1)) p++;	
#endif
	tb_check_return_val(p < e, tb_null);

	// find the free bit index
	m = pool->maxn;
	i = (((tb_byte_t*)p - pool->used) << 3) + tb_bits_fb0_le(*p);
	tb_check_return_val(i < m, tb_null);

	// alloc it
	d = pool->data + i * pool->step;
	tb_static_fixed_pool_used_set1(pool->used, i);

	// predict the next block
	if (i + 1 < m && !tb_static_fixed_pool_used_bset(pool->used, i + 1))
		pool->pred = pool->data + (i + 1) * pool->step;

	// ok?
	return d;
}
#else
static tb_pointer_t tb_static_fixed_pool_malloc_find(tb_static_fixed_pool_t* pool)
{
	tb_size_t 	i = 0;
	tb_size_t 	m = pool->maxn;
	tb_byte_t* 	p = pool->used;
	tb_byte_t 	u = *p;
	tb_byte_t 	b = 0;
	tb_byte_t* 	d = tb_null;
	for (i = 0; i < m; ++i)
	{
		// bit
		b = i & 0x07;

		// u++
		if (!b) 
		{
			u = *p++;
				
			// skip the non-free byte
			//if (u == 0xff)
			if (!(u + 1))
			{
				i += 7;
				continue ;
			}
		}

		// is free?
		// if (!tb_static_fixed_pool_used_bset(pool->used, i))
		if (!(u & (0x01 << b)))
		{
			// ok
			d = pool->data + i * pool->step;
			// tb_static_fixed_pool_used_set1(pool->used, i);
			*(p - 1) |= (0x01 << b);

			// predict the next block
			if (i + 1 < m && !tb_static_fixed_pool_used_bset(pool->used, i + 1))
				pool->pred = pool->data + (i + 1) * pool->step;

			break;
		}
	}

	// ok?
	return d;
}
#endif


/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_static_fixed_pool_init(tb_byte_t* data, tb_size_t size, tb_size_t step, tb_size_t align)
{
	// check
	tb_assert_and_check_return_val(data && step && size, tb_null);

	// align
	align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
	align = tb_max(align, TB_CPU_BITBYTE);
	tb_assert_and_check_return_val(align <= TB_STATIC_FIXED_POOL_ALIGN_MAXN, tb_null);

	// align data
	tb_size_t byte = (tb_size_t)((tb_hize_t)tb_align((tb_hize_t)(tb_size_t)data, align) - (tb_hize_t)(tb_size_t)data);
	tb_assert_and_check_return_val(size >= byte, tb_null);
	size -= byte;
	data += byte;
	tb_assert_and_check_return_val(size, tb_null);

	// init data
	tb_memset(data, 0, size);

	// init pool
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)data;

	// init magic
	pool->magic = TB_STATIC_FIXED_POOL_MAGIC;

	// init align
	pool->align = align;

	// init step
	pool->step = tb_align(step, pool->align);

	// init used
	pool->used = (tb_byte_t*)(tb_size_t)tb_align((tb_hize_t)(tb_size_t)&pool[1], (tb_hize_t)pool->align);
	tb_assert_and_check_return_val(data + size > pool->used, tb_null);

	/* init maxn
	 *
	 * used + maxn * step < left
	 * align8(maxn) / 8 + maxn * step < left
	 * (maxn + 7) / 8 + maxn * step < left
	 * (maxn / 8) + (7 / 8) + maxn * step < left
	 * maxn * (1 / 8 + step) < left - (7 / 8)
	 * maxn < (left - (7 / 8)) / (1 / 8 + step)
	 * maxn < (left * 8 - 7) / (1 + step * 8)
	 */
	pool->maxn = (((data + size - pool->used) << 3) - 7) / (1 + (pool->step << 3));
	tb_assert_and_check_return_val(pool->maxn, tb_null);

	// init data
	pool->data = (tb_byte_t*)(tb_size_t)tb_align((tb_hize_t)(tb_size_t)pool->used + (tb_align8(pool->maxn) >> 3), (tb_hize_t)pool->align);
	tb_assert_and_check_return_val(data + size > pool->data, tb_null);
	tb_assert_and_check_return_val(pool->maxn * pool->step <= (data + size - pool->data), tb_null);

	// init size
	pool->size = 0;

	// init pred
	pool->pred = pool->data;

	// init info
#ifdef __tb_debug__
	pool->info.peak = 0;
	pool->info.fail = 0;
	pool->info.pred = 0;
	pool->info.aloc = 0;
#endif

	// ok
	return ((tb_handle_t)pool);
}
tb_void_t tb_static_fixed_pool_exit(tb_handle_t handle)
{
	// check 
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)handle;
	tb_assert_and_check_return(pool && pool->magic == TB_STATIC_FIXED_POOL_MAGIC);

	// clear body
	tb_static_fixed_pool_clear(handle);

	// clear head
	tb_memset(pool, 0, sizeof(tb_static_fixed_pool_t));
}

tb_void_t tb_static_fixed_pool_clear(tb_handle_t handle)
{
	// check 
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)handle;
	tb_assert_and_check_return(pool && pool->magic == TB_STATIC_FIXED_POOL_MAGIC);

	// clear data
	if (pool->data) tb_memset(pool->data, 0, pool->maxn * pool->step);
	
	// clear used
	if (pool->used) tb_memset(pool->used, 0, (tb_align8(pool->maxn) >> 3));

	// reinit size
	pool->size = 0;
	
	// reinit pred
	pool->pred = pool->data;
	
	// reinit info
#ifdef __tb_debug__
	pool->info.peak = 0;
	pool->info.fail = 0;
	pool->info.pred = 0;
	pool->info.aloc = 0;
#endif
}
tb_size_t tb_static_fixed_pool_size(tb_handle_t handle)
{
	// check 
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_FIXED_POOL_MAGIC, 0);

	// size
	return pool->size;
}
tb_pointer_t tb_static_fixed_pool_malloc(tb_handle_t handle)
{
	// check 
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_FIXED_POOL_MAGIC, tb_null);
	tb_assert_and_check_return_val(pool->step, tb_null);

	// no space?
	tb_check_return_val(pool->size < pool->maxn, tb_null);

	// predict it?
//	tb_pointer_t data = tb_null;
	tb_pointer_t data = tb_static_fixed_pool_malloc_pred(pool);

	// find the free block
	if (!data) data = tb_static_fixed_pool_malloc_find(pool);

	// size++
	if (data) pool->size++;

	// update info
#ifdef __tb_debug__
	if (pool->size > pool->info.peak) pool->info.peak = pool->size;
	pool->info.fail += data? 0 : 1;
	pool->info.aloc++;
#endif

	// ok?
	return data;
}
tb_pointer_t tb_static_fixed_pool_malloc0(tb_handle_t handle)
{
	// check 
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_FIXED_POOL_MAGIC, tb_null);

	// malloc
	tb_pointer_t p = tb_static_fixed_pool_malloc(handle);

	// clear
	if (p) tb_memset(p, 0, pool->step);

	// ok?
	return p;
}
tb_pointer_t tb_static_fixed_pool_memdup(tb_handle_t handle, tb_cpointer_t data)
{
	// check 
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)handle;
	tb_assert_and_check_return_val(pool && data, tb_null);

	// init
	tb_size_t 	n = pool->step;
	tb_char_t* 	p = tb_static_fixed_pool_malloc(handle);

	// copy
	if (p) tb_memcpy(p, data, n);

	// ok?
	return p;
}
tb_bool_t tb_static_fixed_pool_free(tb_handle_t handle, tb_pointer_t data)
{
	// check 
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_FIXED_POOL_MAGIC && pool->step, tb_false);

	// check size
	tb_check_return_val(pool->size, tb_false);

	// check data
	tb_check_return_val((tb_byte_t*)data >= pool->data && (tb_byte_t*)data + pool->step <= pool->data + pool->maxn * pool->step, tb_false);	
	tb_check_return_val(!(((tb_size_t)data) & (pool->align - 1)), tb_false);
	tb_check_return_val(!(((tb_byte_t*)data - pool->data) % pool->step), tb_false);

	// item
	tb_size_t i = ((tb_byte_t*)data - pool->data) / pool->step;

	// double free?
	tb_assert_return_val(tb_static_fixed_pool_used_bset(pool->used, i), tb_true);

	// free it
	tb_static_fixed_pool_used_set0(pool->used, i);
	
	// predict it
	pool->pred = data;

	// size--
	pool->size--;

	// ok
	return tb_true;
}
tb_void_t tb_static_fixed_pool_walk(tb_handle_t handle, tb_bool_t (*func)(tb_pointer_t item, tb_pointer_t data), tb_pointer_t data)
{
	// check 
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)handle;
	tb_assert_and_check_return(pool && pool->magic == TB_STATIC_FIXED_POOL_MAGIC && pool->maxn && pool->step && func);

	// walk
	tb_size_t 	i = 0;
	tb_size_t 	m = pool->maxn;
	tb_byte_t* 	p = pool->used;
	tb_byte_t 	u = *p;
	tb_byte_t 	b = 0;
	for (i = 0; i < m; ++i)
	{
		// bit
		b = i & 0x07;

		// u++
		if (!b) 
		{
			u = *p++;
				
			// this byte is all occupied?
			//if (u == 0xff)
			if (!(u + 1))
			{
				// done func
				func(pool->data + (i + 0) * pool->step, data);
				func(pool->data + (i + 1) * pool->step, data);
				func(pool->data + (i + 2) * pool->step, data);
				func(pool->data + (i + 3) * pool->step, data);
				func(pool->data + (i + 4) * pool->step, data);
				func(pool->data + (i + 5) * pool->step, data);
				func(pool->data + (i + 6) * pool->step, data);
				func(pool->data + (i + 7) * pool->step, data);

				// skip this byte and continue it
				i += 7;
				continue ;
			}
		}

		// is occupied?
		// if (tb_static_fixed_pool_used_bset(pool->used, i))
		if ((u & (0x01 << b)))
		{
			// done func
			func(pool->data + i * pool->step, data);
		}
	}
}

#ifdef __tb_debug__
tb_void_t tb_static_fixed_pool_dump(tb_handle_t handle)
{
	tb_static_fixed_pool_t* pool = (tb_static_fixed_pool_t*)handle;
	tb_assert_and_check_return(pool);

	tb_trace_i("======================================================================");
	tb_trace_i("pool: magic: %#lx",	pool->magic);
	tb_trace_i("pool: align: %lu", 	pool->align);
	tb_trace_i("pool: head: %lu", 	pool->data - (tb_byte_t*)pool);
	tb_trace_i("pool: data: %p", 	pool->data);
	tb_trace_i("pool: size: %lu", 	pool->size);
	tb_trace_i("pool: step: %lu", 	pool->step);
	tb_trace_i("pool: maxn: %lu", 	pool->maxn);
	tb_trace_i("pool: peak: %lu", 	pool->info.peak);
	tb_trace_i("pool: wast: %lu%%",	(pool->data - (tb_byte_t*)pool) * 100 / (pool->maxn * pool->step));
	tb_trace_i("pool: fail: %lu", 	pool->info.fail);
	tb_trace_i("pool: pred: %lu%%", pool->info.aloc? ((pool->info.pred * 100) / pool->info.aloc) : 0);

	tb_size_t 	i = 0;
	tb_size_t 	m = pool->maxn;
	for (i = 0; i < m; ++i)
	{
		if (!(i & 0x7) && pool->used[i >> 3]) 
			tb_trace_i("\tpool: block[%lu]: %08b", i >> 3, pool->used[i >> 3]);
	}
}
#endif
