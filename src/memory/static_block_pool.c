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
 * @file		static_block_pool.c
 * @ingroup 	memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_block_pool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_STATIC_BLOCK_POOL_MAGIC 							(0xdead)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

#ifdef __tb_debug__
/// the pool info type
typedef struct __tb_static_block_pool_info_t
{
	// the used size
	tb_size_t 					used;

	// the peak size
	tb_size_t 					peak;

	// the need size
	tb_size_t 					need;

	// the real size
	tb_size_t 					real;

	// the fail count
	tb_size_t 					fail;

	// the pred count
	tb_size_t 					pred;

	// the aloc count
	tb_size_t 					aloc;

}tb_static_block_pool_info_t;
#endif

/// the pool block type
typedef struct __tb_static_block_pool_block_t
{
#ifdef __tb_debug__

	/// the magic
	tb_uint16_t 				magic;

	/// the line 
	tb_uint16_t 				line;

	/// the real 
	tb_uint32_t 				real;

	/// the file
	tb_char_t const* 			file;

	/// the func
	tb_char_t const* 			func;

	/// the frames
	tb_pointer_t 				frames[16];

#endif

	/// the block size
	tb_uint32_t 				size 	: 31;

	/// is free?
	tb_uint32_t 				free 	: 1;

}tb_static_block_pool_block_t;

/*!the variable pool type
 *
 * <pre>
 * |-----------||||||---------------------|||||||----------------------------------|
 *     head    [head         data         ]
 *                     block0 							block1 ...
 * </pre>
 */
typedef struct __tb_static_block_pool_t
{
	// the magic 
	tb_uint16_t 				magic;

	// the align
	tb_uint16_t 				align;

	// the nhead
	tb_uint16_t 				nhead;

	// the pred
	tb_byte_t* 					pred;

	// the data
	tb_byte_t* 					data;

	// the size
	tb_size_t 					size;

	// the full
	tb_size_t 					full;
	
	// the info 
#ifdef __tb_debug__
	tb_static_block_pool_info_t info;
#endif

}tb_static_block_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef __tb_debug__
// dump backtrace
static tb_void_t tb_static_block_pool_dump_backtrace(tb_char_t const* prefix, tb_static_block_pool_block_t* block)
{
	if (block)
	{
		// the frames count
		tb_size_t nframe = 0;
		while (nframe < tb_arrayn(block->frames) && block->frames[nframe]) nframe++;

		// dump backtrace
		tb_backtrace_dump(prefix, block->frames, nframe);
	}
}
static tb_void_t tb_static_block_pool_dump_data(tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(data && size);

	// dump head
	tb_tracef_i("");
	tb_size_t i = 0;
	tb_size_t n = 147;
	for (i = 0; i < n; i++) tb_tracet_i("=");
	tb_tracet_i(__tb_newline__);

	// walk
	tb_byte_t const* p = data;
	tb_byte_t const* e = data + size;
	while (p < e)
	{
		// full line?
		if (p + 0x20 <= e)
		{
			// dump offset
			tb_tracef_i("");
			tb_tracet_i("%08X ", p - data);

			// dump data
			for (i = 0; i < 0x20; i++)
			{
				if (!(i & 3)) tb_tracet_i(" ");
				tb_tracet_i(" %02X", p[i]);
			}

			// dump spaces
			tb_tracet_i("  ");

			// dump characters
			for (i = 0; i < 0x20; i++)
			{
				tb_tracet_i("%c", tb_isgraph(p[i])? p[i] : '.');
			}

			// dump new line
			tb_tracet_i(__tb_newline__);

			// update p
			p += 0x20;
		}
		// has left?
		else if (p < e)
		{
			// init padding
			tb_size_t padding = n - 0x20;

			// dump offset
			tb_tracef_i("");
			tb_tracet_i("%08X ", p - data); 
			if (padding >= 9) padding -= 9;

			// dump data
			tb_size_t left = e - p;
			for (i = 0; i < left; i++)
			{
				if (!(i & 3)) 
				{
					tb_tracet_i(" ");
					if (padding) padding--;
				}

				tb_tracet_i(" %02X", p[i]);
				if (padding >= 3) padding -= 3;
			}

			// dump spaces
			while (padding--) tb_tracet_i(" ");
				
			// dump characters
			for (i = 0; i < left; i++)
			{
				tb_tracet_i("%c", tb_isgraph(p[i])? p[i] : '.');
			}

			// dump new line
			tb_tracet_i(__tb_newline__);

			// update p
			p += left;
		}
		// end
		else break;
	}
}
static tb_static_block_pool_block_t* tb_static_block_pool_overflow_find(tb_static_block_pool_t* pool)
{
	// check
	tb_assert_and_check_return_val(pool, tb_null);

	// walk
	tb_byte_t* 			pb = pool->data;
	tb_byte_t* 			pe = pb + pool->size;
	tb_size_t 			nhead = pool->nhead;
	tb_static_block_pool_block_t* 	prev = tb_null;
	while (pb + nhead < pe)
	{
		// the block
		tb_static_block_pool_block_t* block = (tb_static_block_pool_block_t*)pb;

		// overflow?
		if (block->magic != TB_STATIC_BLOCK_POOL_MAGIC || !block->size || block->size >= pool->size)
			return prev;	

		// next
		pb += nhead + block->size;
		prev = block;
	}

	// no find
	return tb_null;
}
#endif

#ifdef __tb_debug__
static tb_bool_t tb_static_block_pool_overflow_check(tb_static_block_pool_t* pool, tb_static_block_pool_block_t* block, tb_static_block_pool_block_t* prev)
{
	// overflow?
	if (block->magic != TB_STATIC_BLOCK_POOL_MAGIC || !block->size || block->size >= pool->size) 
	{
		// find the previous block if null
		if (!prev) prev = tb_static_block_pool_overflow_find(pool);

		// dump backtrace
		if (prev)
		{
			// free?
			if (prev->free)
			{
				// dump
				tb_trace_i("pool: overflow: magic: data: %p size: %lu free: %lu"
						, (tb_byte_t const*)prev + pool->nhead
						, prev->real
						, prev->free
						);
				
				// dump data
				tb_static_block_pool_dump_data((tb_byte_t const*)prev, pool->nhead + prev->size);
				tb_static_block_pool_dump_data((tb_byte_t const*)prev + pool->nhead + prev->size, pool->nhead);
			}
			else
			{
				// dump
				tb_trace_i("pool: overflow: magic: data: %p size: %lu free: %lu at %s(): %d, file: %s"
						, (tb_byte_t const*)prev + pool->nhead
						, prev->real
						, prev->free
						, prev->func
						, prev->line
						, prev->file
						);

				// dump backtrace
				tb_static_block_pool_dump_backtrace("pool:     ", prev);

				// dump data
				tb_static_block_pool_dump_data((tb_byte_t const*)prev, pool->nhead + prev->size);
				tb_static_block_pool_dump_data((tb_byte_t const*)prev + pool->nhead + prev->size, pool->nhead);
			}
		}
		else tb_trace_i("pool: overflow");

		// abort
		return tb_false;
	}

	// has padding data?
	if (!block->free && block->real < block->size)
	{
		// find 0xcc
		tb_byte_t const* 	p = (tb_byte_t const*)block + pool->nhead + block->real;
		tb_byte_t const* 	e = (tb_byte_t const*)block + pool->nhead + block->size;
		if (p + 4 < e) e = p + 4; while (p < e && *p == 0xcc) p++;
		
		// no 0xcc? overflow?
		if (p < e)
		{
			// dump
			tb_trace_i("pool: overflow: fill: data: %p size: %lu free: %lu at %s(): %d, file: %s"
					, (tb_byte_t const*)block + pool->nhead
					, block->real
					, block->free
					, block->func
					, block->line
					, block->file
					);

			// dump backtrace
			tb_static_block_pool_dump_backtrace("pool:     ", block);

			// dump data
			tb_static_block_pool_dump_data((tb_byte_t const*)block, pool->nhead + block->size);

			// abort
			return tb_false;
		}
	}
	// ok
	return tb_true;
}
#else
static __tb_inline__ tb_bool_t tb_static_block_pool_overflow_check(tb_static_block_pool_t* pool, tb_static_block_pool_block_t* block, tb_static_block_pool_block_t* prev)
{
	// check
	tb_check_return_val(block->size < pool->size, tb_false);

	// ok
	return tb_true;
}
#endif

// malloc from the given data address
static tb_pointer_t tb_static_block_pool_malloc_from(tb_static_block_pool_t* pool, tb_byte_t* data, tb_size_t size, tb_size_t tryn)
{
	// pb & pe
	tb_byte_t* 	pb = pool->data;
	tb_byte_t* 	pe = pb + pool->size;

	// the data
	tb_byte_t* 	p = data;
	tb_check_return_val(p, tb_null);
	tb_assert_and_check_return_val(p >= pb && p < pe, tb_null);

	// is pred?
	tb_bool_t 	bpred = tryn == 1? tb_true : tb_false;

	// the nhead
	tb_size_t 	nhead = pool->nhead;

	// is align?
	tb_assert(!(size & (pool->align - 1)));
	tb_assert(!(nhead & (pool->align - 1)));
	tb_assert(!((tb_size_t)data & (pool->align - 1)));

	// find the free block
	tb_size_t 			maxn = 1;
	tb_byte_t* 			pred = tb_null;
	while (p + nhead <= pe && tryn)
	{
		// the block
		tb_static_block_pool_block_t* 	block = ((tb_static_block_pool_block_t*)p);
		tb_size_t 			bsize = ((tb_static_block_pool_block_t*)p)->size;
			
		// overflow?
		tb_check_abort(tb_static_block_pool_overflow_check(pool, block, tb_null));

		// allocate if the block is free
		if (block->free)
		{
			// predict the max free block
			if (bsize > maxn) 
			{
				maxn = bsize;
				pred = p;
			}

			// is enough?			
			if (bsize >= size)
			{
				// skip head
				p += nhead;

				// split it if the free block is too large
				if (bsize > nhead + size)
				{
					// split block
					tb_static_block_pool_block_t* next = (tb_static_block_pool_block_t*)(p + size);
					next->size = bsize - size - nhead;
					next->free = 1;
				#ifdef __tb_debug__
					next->real = 0;
					next->magic = TB_STATIC_BLOCK_POOL_MAGIC;
				#endif
					block->size = size;

					// predict the next free block
					pool->pred = p + block->size;
				}
				// use the whole block
				else block->size = bsize;

				// alloc the block
				block->free = 0;

				// reset the predicted block
				if (pool->pred == (tb_byte_t*)block || pool->pred == pe)
					pool->pred = tb_null;

				// reset full
				pool->full = 0;

				// return data address
				return p;
			}
			else // attempt to merge next free block if the free block is too small
			{
				// the next block
				tb_static_block_pool_block_t* next = (tb_static_block_pool_block_t*)(p + nhead + bsize);
			
				// break if doesn't exist next block
				if ((tb_size_t)next + nhead >= (tb_size_t)pe) break;

 				// the next block is free?
				if (next->free)
				{
					// merge next block
					block->size += nhead + next->size;

					// reset the next predicted block
					if (pool->pred == (tb_byte_t*)next)
						pool->pred = tb_null;

					// reset full
					pool->full = 0;

					// continue handle this block
					continue ;
				}
			}
		}

		// tryn--
		tryn--;
	
		// skip it if the block is non-free or too small
		p += nhead + bsize;
	}

	// predict the max free block
	pool->pred = pred;

	// full => pred => max free block size or 1
	if (!bpred) pool->full = maxn;

	// fail
	return tb_null;
}
static tb_pointer_t tb_static_block_pool_malloc_skip_frame(tb_handle_t handle, tb_size_t size __tb_debug_decl__)
{
	// check
	tb_static_block_pool_t* pool = (tb_static_block_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_BLOCK_POOL_MAGIC, tb_null);

	// no size?
	tb_check_return_val(size, tb_null);

	// align size
	tb_size_t asize = tb_align(size, pool->align);

	// full?
	tb_check_return_val(!pool->full || asize <= pool->full, tb_null);

	// one tryn
	tb_size_t tryn = 1;

	// try allocating from the predicted block
	tb_byte_t* p = tb_static_block_pool_malloc_from(pool, pool->pred, asize, tryn);

	// ok?
	tb_check_goto(!p, end);

	// no tryn
	tryn = -1;

	// alloc it from the first block
	p = tb_static_block_pool_malloc_from(pool, pool->data, asize, tryn);

end:

	// update the info
#ifdef __tb_debug__
	if (p) 
	{			
		// the block
		tb_static_block_pool_block_t* block = (tb_static_block_pool_block_t*)(p - pool->nhead);

		// set magic
		block->magic = TB_STATIC_BLOCK_POOL_MAGIC;

		// set line
		block->line = line_;

		// set file
		block->file = file_;

		// set func
		block->func = func_;

		// set real
		block->real = size;

		// fill 0xcc
		if (block->real < block->size) tb_memset(p + block->real, 0xcc, block->size - block->real);

		// set frames
		tb_size_t nframe = tb_backtrace_frames(block->frames, tb_arrayn(block->frames), 5);
		if (nframe < tb_arrayn(block->frames)) tb_memset(block->frames + nframe, 0, (tb_arrayn(block->frames) - nframe) * sizeof(tb_cpointer_t));

		// update the used size
		pool->info.used += block->size;

		// update the need size
		pool->info.need += size;

		// update the real size		
		pool->info.real += pool->nhead + block->size;

		// update the peak size
		if (pool->info.used > pool->info.peak) pool->info.peak = pool->info.used;
		
		// pred++
		if (tryn == 1) pool->info.pred++;
	}
	// fail++
	else pool->info.fail++;
	
	// aloc++
	pool->info.aloc++;

#endif

	// ok?
	return p;
}
tb_pointer_t tb_static_block_pool_ralloc_fast(tb_static_block_pool_t* pool, tb_pointer_t data, tb_size_t size, tb_size_t* osize);
tb_pointer_t tb_static_block_pool_ralloc_fast(tb_static_block_pool_t* pool, tb_pointer_t data, tb_size_t size, tb_size_t* osize)
{
	// check
	tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_BLOCK_POOL_MAGIC, tb_null);

	// no data?
	tb_check_return_val(data, tb_null);

	// pb & pe
	tb_byte_t* 	pb = pool->data;
	tb_byte_t* 	pe = pb + pool->size;

	// the data
	tb_byte_t* 	p = data;
	tb_check_return_val(p && p >= pb && p < pe, tb_null);

	// the nhead
	tb_size_t 	nhead = pool->nhead;
	
	// the block
	tb_static_block_pool_block_t* 	block = ((tb_static_block_pool_block_t*)(p - nhead));
#ifdef __tb_debug__
	tb_size_t 			bsize = block->size;
#endif

	// check block
	tb_assert_return_val(block->magic == TB_STATIC_BLOCK_POOL_MAGIC, tb_null);
	tb_assert_and_check_return_val(block->size < pool->size, tb_null);
	tb_assert_and_check_return_val(!block->free, tb_null);

	// osize
	if (osize) *osize = block->size;

	// ok?
	if (size <= block->size) 
	{
#ifdef __tb_debug__
		// update the real size
		block->real = size;
#endif
		return data; 
	}

	// align size
	tb_size_t asize = tb_align(size, pool->align);

	// merge it if the next block is free and the space is enough
	tb_static_block_pool_block_t* next = (tb_static_block_pool_block_t*)(p + block->size);
	if (next->free && block->size + nhead + next->size >= asize) 
	{
		// merge it
		block->size += nhead + next->size;

		// split it if the free block is too large
		if (block->size > asize + nhead)
		{
			// split block
			next = (tb_static_block_pool_block_t*)(p + asize);
			next->size = block->size - asize - nhead;
			next->free = 1;
#ifdef __tb_debug__
			next->real = 0;
			next->magic = TB_STATIC_BLOCK_POOL_MAGIC;
#endif
			block->size = asize;

			// predict the next free block
			pool->pred = (tb_byte_t*)next;
		}
		// reset the predicted block
		else if (pool->pred == (tb_byte_t*)block || pool->pred == (tb_byte_t*)next || pool->pred == pe)
			pool->pred = tb_null;

		// reset full
		pool->full = 0;

		// osize
		if (osize) *osize = block->size;

#ifdef __tb_debug__

		// update the used size
		pool->info.used += block->size - bsize;

		// update the need size
		pool->info.need += block->size - bsize;

		// update the real size		
		pool->info.real += block->size - bsize;

		// update the peak size
		if (pool->info.used > pool->info.peak) pool->info.peak = pool->info.used;
		
		// update the real size
		block->real = size;

		// fill 0xcc
		if (block->real < block->size) tb_memset((tb_byte_t*)data + block->real, 0xcc, block->size - block->real);
#endif

		// ok
		return data;
	}

	return tb_null;
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_static_block_pool_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	// check
	tb_assert_and_check_return_val(data && size, tb_null);

	// align
	align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
	align = tb_max(align, TB_CPU_BITBYTE);

	// align data
	tb_size_t byte = (tb_size_t)((tb_hize_t)tb_align((tb_hize_t)(tb_size_t)data, (tb_hize_t)align) - (tb_hize_t)(tb_size_t)data);
	tb_assert_and_check_return_val(size >= byte, tb_null);
	size -= byte;
	data += byte;

	// init data
	tb_memset(data, 0, size);

	// init pool
	tb_static_block_pool_t* pool = (tb_static_block_pool_t*)data;

	// init magic
	pool->magic = TB_STATIC_BLOCK_POOL_MAGIC;

	// init align
	pool->align = align;

	// init nhead
	pool->nhead = tb_align(sizeof(tb_static_block_pool_block_t), pool->align);

	// init data
	pool->data = (tb_byte_t*)(tb_size_t)tb_align((tb_hize_t)(tb_size_t)&pool[1], (tb_hize_t)pool->align);
	tb_assert_and_check_return_val(data + size > pool->data, tb_null);

	// init size
	pool->size = (tb_byte_t*)data + size - pool->data;
	tb_assert_and_check_return_val(pool->size > pool->nhead, tb_null);

	// init block, only one free block now.
	((tb_static_block_pool_block_t*)pool->data)->free = 1;
	((tb_static_block_pool_block_t*)pool->data)->size = pool->size - pool->nhead;
#ifdef __tb_debug__
	((tb_static_block_pool_block_t*)pool->data)->real = 0;
	((tb_static_block_pool_block_t*)pool->data)->magic = TB_STATIC_BLOCK_POOL_MAGIC;
#endif

	// init pred
	pool->pred = pool->data;

	// init full
	pool->full = 0;

	// init info
#ifdef __tb_debug__
	pool->info.used = 0;
	pool->info.peak = 0;
	pool->info.need = 0;
	pool->info.real = 0;
	pool->info.fail = 0;
	pool->info.pred = 0;
	pool->info.aloc = 0;
	if (pool->nhead < pool->size) tb_memset(pool->data + pool->nhead, 0xcc, pool->size - pool->nhead);
#endif

	// ok
	return ((tb_handle_t)pool);
}
tb_void_t tb_static_block_pool_exit(tb_handle_t handle)
{
	// check 
	tb_static_block_pool_t* pool = (tb_static_block_pool_t*)handle;
	tb_assert_and_check_return(pool && pool->magic == TB_STATIC_BLOCK_POOL_MAGIC);

	// clear body
	tb_static_block_pool_clear(handle);

	// clear head
	tb_memset(pool, 0, sizeof(tb_static_block_pool_t));	
}
tb_void_t tb_static_block_pool_clear(tb_handle_t handle)
{
	// check 
	tb_static_block_pool_t* pool = (tb_static_block_pool_t*)handle;
	tb_assert_and_check_return(pool && pool->magic == TB_STATIC_BLOCK_POOL_MAGIC);

	// init block, only one free block now.
	((tb_static_block_pool_block_t*)pool->data)->free = 1;
	((tb_static_block_pool_block_t*)pool->data)->size = pool->size - pool->nhead;
#ifdef __tb_debug__
	((tb_static_block_pool_block_t*)pool->data)->real = 0;
	((tb_static_block_pool_block_t*)pool->data)->magic = TB_STATIC_BLOCK_POOL_MAGIC;
#endif

	// reinit pred
	pool->pred = pool->data;

	// reinit full
	pool->full = 0;

	// reinit info
#ifdef __tb_debug__
	pool->info.used = 0;
	pool->info.peak = 0;
	pool->info.need = 0;
	pool->info.real = 0;
	pool->info.fail = 0;
	pool->info.pred = 0;
	pool->info.aloc = 0;
	if (pool->nhead < pool->size) tb_memset(pool->data + pool->nhead, 0xcc, pool->size - pool->nhead);
#endif
}

tb_pointer_t tb_static_block_pool_malloc_(tb_handle_t handle, tb_size_t size __tb_debug_decl__)
{
	// malloc
	return tb_static_block_pool_malloc_skip_frame(handle, size __tb_debug_args__);
}

tb_pointer_t tb_static_block_pool_malloc0_(tb_handle_t handle, tb_size_t size __tb_debug_decl__)
{
	// malloc
	tb_byte_t* p = tb_static_block_pool_malloc_skip_frame(handle, size __tb_debug_args__);

	// clear
	if (p && size) tb_memset(p, 0, size);

	// ok?
	return p;
}

tb_pointer_t tb_static_block_pool_nalloc_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// malloc
	return tb_static_block_pool_malloc_skip_frame(handle, item * size __tb_debug_args__);
}
tb_pointer_t tb_static_block_pool_nalloc0_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// malloc
	tb_pointer_t p = tb_static_block_pool_malloc_skip_frame(handle, item * size __tb_debug_args__);

	// clear
	if (p && (item * size)) tb_memset(p, 0, item * size);

	// ok?
	return p;
}

tb_pointer_t tb_static_block_pool_ralloc_(tb_handle_t handle, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
	// check
	tb_static_block_pool_t* pool = (tb_static_block_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_BLOCK_POOL_MAGIC, tb_null);

	// free it if no size
	if (!size)
	{
		tb_static_block_pool_free_(pool, data __tb_debug_args__);
		return tb_null;
	}

	// alloc it if no data?
	if (!data) return tb_static_block_pool_malloc_skip_frame(pool, size __tb_debug_args__);
	
	// ralloc it with fast mode if enough
	tb_size_t 		osize = 0;
	tb_pointer_t 	pdata = tb_static_block_pool_ralloc_fast(pool, data, size, &osize);
	tb_check_return_val(!pdata, pdata);
	tb_assert_and_check_return_val(osize && osize < size, tb_null);

	// malloc it
	pdata = tb_static_block_pool_malloc_skip_frame(pool, size __tb_debug_args__);
	tb_check_return_val(pdata, tb_null);
	tb_assert_and_check_return_val(pdata != data, pdata);

	// copy data
	tb_memcpy(pdata, data, osize);
	
	// free it
	tb_static_block_pool_free_(pool, data __tb_debug_args__);

	// ok
	return pdata;
}
tb_bool_t tb_static_block_pool_free_(tb_handle_t handle, tb_pointer_t data __tb_debug_decl__)
{
	// check
	tb_static_block_pool_t* pool = (tb_static_block_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->magic == TB_STATIC_BLOCK_POOL_MAGIC, tb_false);

	// no data?
	tb_check_return_val(data, tb_true);

	// pb & pe
	tb_byte_t* 	pb = pool->data;
	tb_byte_t* 	pe = pb + pool->size;

	// the data
	tb_byte_t* 	p = data;
	tb_check_return_val(p >= pb && p < pe, tb_false);

	// the nhead
	tb_size_t 	nhead = pool->nhead;
	
	// the block
	tb_static_block_pool_block_t* 	block = ((tb_static_block_pool_block_t*)(p - nhead));
#ifdef __tb_debug__
	tb_size_t 			bsize = block->size;
#endif

	// check overflow
	tb_check_abort(tb_static_block_pool_overflow_check(pool, block, tb_null));

	// double free?
	if (block->free)
	{
		// trace
		tb_trace_d("pool: double free: at %s(): %d, file: %s", block->func, block->line, block->file);

#ifdef __tb_debug__
		// dump backtrace
		tb_static_block_pool_dump_backtrace("pool:     ", block);
#endif

		return tb_true;
	}
	tb_assert_and_check_return_val(!block->free, tb_true);

	// check the next block
	tb_static_block_pool_block_t* next = (tb_static_block_pool_block_t*)(p + block->size);
	if ((tb_byte_t const*)next + nhead < pe)
	{
		// check overflow
		tb_check_abort(tb_static_block_pool_overflow_check(pool, next, block));

		// merge it if the next block is free
		if (next->free) block->size += nhead + next->size;
	}

	// free it
	block->free = 1;

	// predict the next free block
	pool->pred = (tb_byte_t*)block;

	// reinit full
	pool->full = 0;

#ifdef __tb_debug__
	// update the used
	pool->info.used -= bsize;

	// clear the real
	block->real = 0;

	// fill 0xcc
	if (bsize + nhead < block->size) tb_memset(p, 0xcc, bsize + nhead);
	else if (bsize) tb_memset(p, 0xcc, bsize);
#endif

	// ok
	return tb_true;
}
#ifdef __tb_debug__
tb_size_t tb_static_block_pool_data_size(tb_handle_t handle, tb_cpointer_t data)
{
	// check
	tb_static_block_pool_t* pool = (tb_static_block_pool_t*)handle;
	tb_assert_and_check_return_val(pool, 0);
	tb_check_return_val(data, 0);

	// is this pool?
	tb_check_return_val((tb_byte_t const*)data >= (tb_byte_t const*)pool->data + pool->nhead && (tb_byte_t const*)data < (tb_byte_t const*)pool->data + pool->size, 0);

	// the data block
	tb_static_block_pool_block_t const* block = ((tb_static_block_pool_block_t const*)((tb_byte_t const*)data - pool->nhead));

	// check magic
	tb_check_return_val(block->magic == TB_STATIC_BLOCK_POOL_MAGIC && !block->free, 0);

	// the real size
	return block->real;
}
tb_void_t tb_static_block_pool_data_dump(tb_handle_t handle, tb_cpointer_t data, tb_char_t const* prefix)
{
	// check
	tb_static_block_pool_t* pool = (tb_static_block_pool_t*)handle;
	tb_assert_and_check_return(pool);
	tb_check_return(data);

	// is this pool?
	tb_check_return((tb_byte_t*)data >= (tb_byte_t*)pool->data + pool->nhead && (tb_byte_t*)data < (tb_byte_t*)pool->data + pool->size);

	// the data block
	tb_static_block_pool_block_t* block = ((tb_static_block_pool_block_t*)((tb_byte_t*)data - pool->nhead));

	// check magic
	tb_check_return(block->magic == TB_STATIC_BLOCK_POOL_MAGIC && !block->free);

	// dump
	tb_trace_i("%s: data: %p size: %lu at %s(): %d, file: %s"
			, prefix
			, data
			, block->real
			, block->func
			, block->line
			, block->file
			);

	// dump backtrace
	tb_char_t tag[1024] = {0};
	tb_snprintf(tag, sizeof(tag) - 1, "%s:     ", prefix);
	tb_static_block_pool_dump_backtrace(tag, block);

	// dump data
	tb_static_block_pool_dump_data(data, tb_min(block->real, 4096));
}
tb_void_t tb_static_block_pool_dump(tb_handle_t handle, tb_char_t const* prefix)
{
	// check
	tb_static_block_pool_t* pool = (tb_static_block_pool_t*)handle;
	tb_assert_and_check_return(pool);

	// prefix
	if (!prefix) prefix = "pool";

	// dump
	tb_trace_i("======================================================================");

	// walk
	tb_byte_t* 						pb = pool->data;
	tb_byte_t* 						pe = pb + pool->size;
	tb_size_t 						nhead = pool->nhead;
	tb_static_block_pool_block_t* 	prev = tb_null;
	tb_bool_t 						ok = tb_true;
	tb_size_t 						frag = 0;
	while (pb + nhead < pe)
	{
		// the block
		tb_static_block_pool_block_t* block = (tb_static_block_pool_block_t*)pb;

		// check overflow
		tb_check_break((ok = tb_static_block_pool_overflow_check(pool, block, prev)));

		// no free?
		if (!block->free)
		{
			// dump leak
			tb_trace_i("%s: leak: data: %p size: %lu at %s(): %d, file: %s"
					, prefix
					, pb + nhead
					, block->real
					, block->func
					, block->line
					, block->file
					);

			// dump frames
			{
				// the backtrace prefix 
				tb_char_t backtrace_prefix[64] = {0};
				tb_snprintf(backtrace_prefix, 63, "%s:     ", prefix);

				// dump backtrace
				tb_static_block_pool_dump_backtrace(backtrace_prefix, block);
			}

			// leak
			ok = tb_false;
		}

		// next
		pb += nhead + block->size;
		prev = block;
		frag++;
	}

	// dump 
	if (!ok) tb_trace_i("");
//	tb_trace_i("%s: magic: %#lx",		prefix, pool->magic);
//	tb_trace_i("%s: nhead: %lu", 		prefix, pool->nhead);
	tb_trace_i("%s: align: %lu", 		prefix, pool->align);
//	tb_trace_i("%s: head: %lu", 		prefix, pool->data - (tb_byte_t*)pool);
//	tb_trace_i("%s: data: %p", 			prefix, pool->data);
	tb_trace_i("%s: size: %lu", 		prefix, pool->size);
	tb_trace_i("%s: full: %s", 			prefix, pool->full? "true" : "false");
	tb_trace_i("%s: used: %lu", 		prefix, pool->info.used);
	tb_trace_i("%s: peak: %lu", 		prefix, pool->info.peak);
	tb_trace_i("%s: wast: %lu%%", 		prefix, (pool->info.real + (pool->data - (tb_byte_t*)pool) - pool->info.need) * 100 / (pool->info.real + (pool->data - (tb_byte_t*)pool)));
	tb_trace_i("%s: fail: %lu", 		prefix, pool->info.fail);
	tb_trace_i("%s: pred: %lu%%", 		prefix, pool->info.aloc? ((pool->info.pred * 100) / pool->info.aloc) : 0);
	tb_trace_i("%s: frag: %lu", 		prefix, frag);
}
#endif
