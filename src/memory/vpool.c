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
 * \file		vpool.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "vpool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_VPOOL_MAGIC 							(0xdead)

// the align maxn
#define TB_VPOOL_ALIGN_MAXN 					(128)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the vpool info type
#ifdef TB_DEBUG
typedef struct __tb_vpool_info_t
{
	// the used size
	tb_size_t 			used;

	// the peak size
	tb_size_t 			peak;

	// the need size
	tb_size_t 			need;

	// the real size
	tb_size_t 			real;

	// the fail count
	tb_size_t 			fail;

	// the pred count
	tb_size_t 			pred;

	// the aloc count
	tb_size_t 			aloc;

}tb_vpool_info_t;
#endif

// the vpool block type
typedef struct __tb_vpool_block_t
{
	// the block size
	tb_size_t 			size 	: (TB_CPU_BITSIZE - 1);

	// is free?
	tb_size_t 			free 	: 1;

#ifdef TB_DEBUG

	// the magic
	tb_size_t 			magic 	: 16;

	// the line 
	tb_size_t 			line 	: 16;

	// the file
	tb_char_t const* 	file;

	// the func
	tb_char_t const* 	func;

#endif

}tb_vpool_block_t;

/* the variable pool type
 *
 * |-----------||||||---------------------|||||||----------------------------------|
 *     head    [head         data         ]
 *                     block0 							block1 ...
 *
 */
typedef struct __tb_vpool_t
{
	// the magic 
	tb_size_t 			magic 	: 16;

	// the align
	tb_size_t 			align 	: 8;

	// the nhead
	tb_size_t 			nhead 	: 8;

	// the pred
	tb_byte_t* 			pred;

	// the data
	tb_byte_t* 			data;

	// the size
	tb_size_t 			size;

	// the full
	tb_size_t 			full;
	
	// the info 
#ifdef TB_DEBUG
	tb_vpool_info_t 	info;
#endif

}tb_vpool_t;

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
// malloc from the given data address
static tb_pointer_t tb_vpool_malloc_from(tb_vpool_t* vpool, tb_byte_t* data, tb_size_t size, tb_size_t tryn)
{
	// pb & pe
	tb_byte_t* 	pb = vpool->data;
	tb_byte_t* 	pe = pb + vpool->size;

	// the data
	tb_byte_t* 	p = data;
	tb_check_return_val(p, TB_NULL);
	tb_assert_and_check_return_val(p >= pb && p < pe, TB_NULL);

	// is pred?
	tb_bool_t 	bpred = tryn == 1? TB_TRUE : TB_FALSE;

	// the nhead
	tb_size_t 	nhead = vpool->nhead;

	// is align?
	tb_assert(!(size & (vpool->align - 1)));
	tb_assert(!(nhead & (vpool->align - 1)));
	tb_assert(!((tb_size_t)data & (vpool->align - 1)));

	// find the free block
	tb_size_t 	maxn = 1;
	tb_byte_t* 	pred = TB_NULL;
	while (p + nhead < pe && tryn)
	{
		// the block
		tb_vpool_block_t* 	block = ((tb_vpool_block_t*)p);
		tb_size_t 			bsize = ((tb_vpool_block_t*)p)->size;

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
					tb_vpool_block_t* next = (tb_vpool_block_t*)(p + size);
					next->size = bsize - size - nhead;
					next->free = 1;
					block->size = size;

					// predict the next free block
					vpool->pred = p + block->size;
				}
				// use the whole block
				else block->size = bsize;

				// alloc the block
				block->free = 0;

				// reset the predicted block
				if (vpool->pred == (tb_byte_t*)block || vpool->pred == pe)
					vpool->pred = TB_NULL;

				// reset full
				vpool->full = 0;

				// return data address
				return p;
			}
			else // attempt to merge next free block if the free block is too small
			{
				// the next block
				tb_vpool_block_t* next = (tb_vpool_block_t*)(p + nhead + bsize);
			
				// break if doesn't exist next block
				if ((tb_size_t)next + nhead >= (tb_size_t)pe) break;

 				// the next block is free?
				if (next->free)
				{
					// merge next block
					block->size += nhead + next->size;

					// reset the next predicted block
					if (vpool->pred == (tb_byte_t*)next)
						vpool->pred = TB_NULL;

					// reset full
					vpool->full = 0;

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
	vpool->pred = pred;

	// full => pred => max free block size or 1
	if (!bpred) vpool->full = maxn;

	// fail
	return TB_NULL;
}
tb_pointer_t tb_vpool_ralloc_fast(tb_vpool_t* vpool, tb_pointer_t data, tb_size_t size, tb_size_t* osize)
{
	// check
	tb_assert_and_check_return_val(vpool && vpool->magic == TB_VPOOL_MAGIC, TB_NULL);

	// no data?
	tb_check_return_val(data, TB_NULL);

	// pb & pe
	tb_byte_t* 	pb = vpool->data;
	tb_byte_t* 	pe = pb + vpool->size;

	// the data
	tb_byte_t* 	p = data;
	tb_check_return_val(p && p >= pb && p < pe, TB_NULL);

	// the nhead
	tb_size_t 	nhead = vpool->nhead;
	
	// the block
	tb_vpool_block_t* 	block = ((tb_vpool_block_t*)(p - nhead));
	tb_size_t 			bsize = block->size;

	// check block
	tb_assert_return_val(block->magic == TB_VPOOL_MAGIC, TB_NULL);
	tb_assert_and_check_return_val(block->size < vpool->size, TB_NULL);
	tb_assert_and_check_return_val(!block->free, TB_NULL);

	// osize
	if (osize) *osize = block->size;

	// ok?
	if (block->size >= size) return data; 

	// align size
	tb_size_t asize = tb_align(size, vpool->align);

	// merge it if the next block is free and the space is enough
	tb_vpool_block_t* next = (tb_vpool_block_t*)(p + block->size);
	if (next->free && block->size + nhead + next->size >= asize) 
	{
		// merge it
		block->size += nhead + next->size;

		// split it if the free block is too large
		if (block->size > nhead + asize)
		{
			// split block
			next = (tb_vpool_block_t*)(p + asize);
			next->size = block->size - asize - nhead;
			next->free = 1;
			block->size = asize;

			// predict the next free block
			vpool->pred = p + block->size;
		}

		// reset the predicted block
		if (vpool->pred == (tb_byte_t*)block || vpool->pred == pe)
			vpool->pred = TB_NULL;

		// reset full
		vpool->full = 0;

		// osize
		if (osize) *osize = block->size;

		// update the info
#ifdef TB_DEBUG

		// update the used size
		vpool->info.used += block->size - bsize;

		// update the need size
		vpool->info.need += block->size - bsize;

		// update the real size		
		vpool->info.real += block->size - bsize;

		// update the peak size
		if (vpool->info.used > vpool->info.peak) vpool->info.peak = vpool->info.used;
		
#endif

		// ok
		return data;
	}

	return TB_NULL;
}


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_vpool_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	// check
	tb_assert_and_check_return_val(data && size, TB_NULL);

	// align
	align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
	align = tb_max(align, TB_CPU_BITBYTE);
	tb_assert_and_check_return_val(align <= TB_VPOOL_ALIGN_MAXN, TB_NULL);

	// align data
	tb_size_t byte = (tb_size_t)tb_align((tb_size_t)data, align) - (tb_size_t)data;
	tb_assert_and_check_return_val(size >= byte, TB_NULL);
	size -= byte;
	data += byte;

	// init data
	tb_memset(data, 0, size);

	// init vpool
	tb_vpool_t* vpool = data;

	// init magic
	vpool->magic = TB_VPOOL_MAGIC;

	// init align
	vpool->align = align;

	// init nhead
	vpool->nhead = tb_align(sizeof(tb_vpool_block_t), vpool->align);

	// init data
	vpool->data = (tb_byte_t*)tb_align((tb_size_t)&vpool[1], vpool->align);
	tb_assert_and_check_return_val(data + size > vpool->data, TB_NULL);

	// init size
	vpool->size = (tb_byte_t*)data + size - vpool->data;
	tb_assert_and_check_return_val(vpool->size > vpool->nhead, TB_NULL);

	// init block, only one free block now.
	((tb_vpool_block_t*)vpool->data)->free = 1;
	((tb_vpool_block_t*)vpool->data)->size = vpool->size - vpool->nhead;

	// init pred
	vpool->pred = vpool->data;

	// init full
	vpool->full = 0;

	// init info
#ifdef TB_DEBUG
	vpool->info.used = 0;
	vpool->info.peak = 0;
	vpool->info.need = 0;
	vpool->info.real = 0;
	vpool->info.fail = 0;
	vpool->info.pred = 0;
	vpool->info.aloc = 0;
#endif

	// ok
	return ((tb_handle_t)vpool);
}
tb_void_t tb_vpool_exit(tb_handle_t handle)
{
	// check 
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return(vpool && vpool->magic == TB_VPOOL_MAGIC);

	// clear body
	tb_vpool_clear(handle);

	// clear head
	tb_memset(vpool, 0, sizeof(tb_vpool_t));	
}
tb_void_t tb_vpool_clear(tb_handle_t handle)
{
	// check 
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return(vpool && vpool->magic == TB_VPOOL_MAGIC);

	// clear body
	if (vpool->data) tb_memset(vpool->data, 0, vpool->size);
	
	// init block, only one free block now.
	((tb_vpool_block_t*)vpool->data)->free = 1;
	((tb_vpool_block_t*)vpool->data)->size = vpool->size - vpool->nhead;

	// reinit pred
	vpool->pred = vpool->data;

	// reinit full
	vpool->full = 0;

	// reinit info
#ifdef TB_DEBUG
	vpool->info.used = 0;
	vpool->info.peak = 0;
	vpool->info.need = 0;
	vpool->info.real = 0;
	vpool->info.fail = 0;
	vpool->info.pred = 0;
	vpool->info.aloc = 0;
#endif
}

#ifndef TB_DEBUG
tb_pointer_t tb_vpool_malloc_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_vpool_malloc_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return_val(vpool && vpool->magic == TB_VPOOL_MAGIC, TB_NULL);

	// no size?
	tb_check_return_val(size, TB_NULL);

	// align size
	tb_size_t asize = tb_align(size, vpool->align);

	// full?
	tb_check_return_val(!vpool->full || asize <= vpool->full, TB_NULL);

	// one tryn
	tb_size_t tryn = 1;

	// try allocating from the predicted block
	tb_byte_t* p = tb_vpool_malloc_from(vpool, vpool->pred, asize, tryn);

	// ok?
	tb_check_goto(!p, end);

	// no tryn
	tryn = -1;

	// alloc it from the first block
	p = tb_vpool_malloc_from(vpool, vpool->data, asize, tryn);

end:

	// update the info
#ifdef TB_DEBUG
	if (p) 
	{			
		// the block
		tb_vpool_block_t* block = (tb_vpool_block_t*)(p - vpool->nhead);

		// set magic
		block->magic = TB_VPOOL_MAGIC;

		// set line
		block->line = line;

		// set file
		block->file = file;

		// set func
		block->func = func;

		// update the used size
		vpool->info.used += block->size;

		// update the need size
		vpool->info.need += size;

		// update the real size		
		vpool->info.real += vpool->nhead + block->size;

		// update the peak size
		if (vpool->info.used > vpool->info.peak) vpool->info.peak = vpool->info.used;
		
		// pred++
		if (tryn == 1) vpool->info.pred++;
	}
	// fail++
	else vpool->info.fail++;
	
	// aloc++
	vpool->info.aloc++;

#endif

	// ok?
	return p;
}

#ifndef TB_DEBUG
tb_pointer_t tb_vpool_malloc0_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_vpool_malloc0_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// malloc
#ifndef TB_DEBUG
	tb_byte_t* p = tb_vpool_malloc_impl(handle, size);
#else
	tb_byte_t* p = tb_vpool_malloc_impl(handle, size, func, line, file);
#endif

	// clear
	if (p && size) tb_memset(p, 0, size);

	// ok?
	return p;
}

#ifndef TB_DEBUG
tb_pointer_t tb_vpool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_vpool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_assert_and_check_return_val(item, TB_NULL);

	// malloc
#ifndef TB_DEBUG
	return tb_vpool_malloc_impl(handle, item * size);
#else
	return tb_vpool_malloc_impl(handle, item * size, func, line, file);
#endif
}

#ifndef TB_DEBUG
tb_pointer_t tb_vpool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_vpool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_assert_and_check_return_val(item, TB_NULL);

	// malloc
#ifndef TB_DEBUG
	return tb_vpool_malloc0_impl(handle, item * size);
#else
	return tb_vpool_malloc0_impl(handle, item * size, func, line, file);
#endif
}

#ifndef TB_DEBUG
tb_pointer_t tb_vpool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_vpool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return_val(vpool && vpool->magic == TB_VPOOL_MAGIC, TB_NULL);

	// free it if no size
	if (!size)
	{
#ifndef TB_DEBUG
		tb_vpool_free_impl(vpool, data);
#else
		tb_vpool_free_impl(vpool, data, func, line, file);
#endif
		return TB_NULL;
	}

	// alloc it if no data?
#ifndef TB_DEBUG
	if (!data) return tb_vpool_malloc_impl(vpool, size);
#else
	if (!data) return tb_vpool_malloc_impl(vpool, size, func, line, file);
#endif
	
	// ralloc it with fast mode if enough
	tb_size_t 		osize = 0;
	tb_pointer_t 	pdata = tb_vpool_ralloc_fast(vpool, data, size, &osize);
	tb_check_return_val(!pdata, pdata);
	tb_assert_and_check_return_val(osize && osize < size, TB_NULL);

	// malloc it
#ifndef TB_DEBUG
	pdata = tb_vpool_malloc_impl(vpool, size);
#else
	pdata = tb_vpool_malloc_impl(vpool, size, func, line, file);
#endif
	tb_check_return_val(pdata, TB_NULL);
	tb_assert_and_check_return_val(pdata != data, pdata);

	// copy data
	tb_memcpy(pdata, data, osize);
	
	// free it
#ifndef TB_DEBUG
	tb_vpool_free_impl(vpool, data);
#else
	tb_vpool_free_impl(vpool, data, func, line, file);
#endif

	// ok
	return pdata;
}

#ifndef TB_DEBUG
tb_bool_t tb_vpool_free_impl(tb_handle_t handle, tb_pointer_t data)
#else
tb_bool_t tb_vpool_free_impl(tb_handle_t handle, tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return_val(vpool && vpool->magic == TB_VPOOL_MAGIC, TB_FALSE);

	// no data?
	tb_check_return_val(data, TB_TRUE);

	// pb & pe
	tb_byte_t* 	pb = vpool->data;
	tb_byte_t* 	pe = pb + vpool->size;

	// the data
	tb_byte_t* 	p = data;
	tb_check_return_val(p >= pb && p < pe, TB_FALSE);

	// the nhead
	tb_size_t 	nhead = vpool->nhead;
	
	// the block
	tb_vpool_block_t* 	block = ((tb_vpool_block_t*)(p - nhead));
	tb_size_t 			bsize = block->size;

	// check block
	tb_assert_return_val(block->magic == TB_VPOOL_MAGIC, TB_FALSE);
	tb_assert_and_check_return_val(block->size < vpool->size, TB_FALSE);
	tb_assert_and_check_return_val(!block->free, TB_TRUE);

	// merge it if the next block is free
	tb_vpool_block_t* next = (tb_vpool_block_t*)(p + block->size);
	if (next->free) block->size += nhead + next->size;

	// free it
	block->free = 1;

	// predict the next free block
	vpool->pred = (tb_byte_t*)block;

	// reinit full
	vpool->full = 0;

	// update the info
#ifdef TB_DEBUG
	vpool->info.used -= bsize;
#endif

	// ok
	return TB_TRUE;
}


#ifdef TB_DEBUG
tb_void_t tb_vpool_dump(tb_handle_t handle)
{
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return(vpool);

	tb_print("======================================================================");
	tb_print("vpool: magic: %#lx",	vpool->magic);
	tb_print("vpool: nhead: %lu", 	vpool->nhead);
	tb_print("vpool: align: %lu", 	vpool->align);
	tb_print("vpool: head: %lu", 	vpool->data - (tb_byte_t*)vpool);
	tb_print("vpool: data: %p", 	vpool->data);
	tb_print("vpool: size: %lu", 	vpool->size);
	tb_print("vpool: full: %lu", 	vpool->full);
	tb_print("vpool: used: %lu", 	vpool->info.used);
	tb_print("vpool: peak: %lu", 	vpool->info.peak);
	tb_print("vpool: wast: %lu%%", 	(vpool->info.real + (vpool->data - (tb_byte_t*)vpool) - vpool->info.need) * 100 / (vpool->info.real + (vpool->data - (tb_byte_t*)vpool)));
	tb_print("vpool: fail: %lu", 	vpool->info.fail);
	tb_print("vpool: pred: %lu%%", 	vpool->info.aloc? ((vpool->info.pred * 100) / vpool->info.aloc) : 0);

	tb_size_t 	i = 0;
	tb_byte_t* 	pb = vpool->data;
	tb_byte_t* 	pe = pb + vpool->size;
	tb_size_t 	nhead = vpool->nhead;
	while (pb + nhead < pe)
	{
		tb_vpool_block_t* block = (tb_vpool_block_t*)pb;

		if (!block->free)
		{
			tb_print("\tvpool: block[%lu]: data: %p size: %lu free: %lu, magic: 0x%x, at %s(): %d, file: %s"
					, i++
					, pb + nhead
					, block->size
					, 0
					, block->magic
					, block->func
					, block->line
					, block->file
					);
		}
		else
		{
			tb_print("\tvpool: block[%lu]: data: %p size: %lu free: %lu"
					, i++
					, pb + nhead
					, block->size
					, 1
					);
		}
		pb += nhead + block->size;
	}
}
#endif
