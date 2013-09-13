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
 * @file		vpool.c
 * @ingroup 	memory
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "vpool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

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

#ifdef __tb_debug__
/// the vpool info type
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

/// the vpool block type
typedef struct __tb_vpool_block_t
{
#ifdef __tb_debug__

	/// the magic
	tb_uint16_t 		magic;

	/// the line 
	tb_uint16_t 		line;

	/// the real 
	tb_uint32_t 		real;

	/// the file
	tb_char_t const* 	file;

	/// the func
	tb_char_t const* 	func;

	/// the frames
	tb_cpointer_t 		frames[16];

#endif

	/// the block size
	tb_uint32_t 		size 	: 31;

	/// is free?
	tb_uint32_t 		free 	: 1;

}tb_vpool_block_t;

/*!the variable pool type
 *
 * <pre>
 * |-----------||||||---------------------|||||||----------------------------------|
 *     head    [head         data         ]
 *                     block0 							block1 ...
 * </pre>
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
#ifdef __tb_debug__
	tb_vpool_info_t 	info;
#endif

}tb_vpool_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef __tb_debug__
// dump backtrace
static tb_void_t tb_vpool_dump_backtrace(tb_char_t const* prefix, tb_vpool_block_t* block)
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
static tb_void_t tb_vpool_dump_data(tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(data && size);

	// dump head
	tb_size_t i = 0;
	tb_size_t n = 147;
	for (i = 0; i < n; i++) tb_printf("=");
	tb_printf("\n");

	// walk
	tb_byte_t const* p = data;
	tb_byte_t const* e = data + size;
	while (p < e)
	{
		// full line?
		if (p + 0x20 <= e)
		{
			// dump offset
			tb_printf("%08X ", p - data);

			// dump data
			for (i = 0; i < 0x20; i++)
			{
				if (!(i & 3)) tb_printf(" ");
				tb_printf(" %02X", p[i]);
			}

			// dump spaces
			tb_printf("  ");

			// dump characters
			for (i = 0; i < 0x20; i++)
			{
				tb_printf("%c", tb_isgraph(p[i])? p[i] : '.');
			}

			// dump new line
			tb_printf("\n");

			// update p
			p += 0x20;
		}
		// has left?
		else if (p < e)
		{
			// init padding
			tb_size_t padding = n - 0x20;

			// dump offset
			tb_printf("%08X ", p - data); 
			if (padding >= 9) padding -= 9;

			// dump data
			tb_size_t left = e - p;
			for (i = 0; i < left; i++)
			{
				if (!(i & 3)) 
				{
					tb_printf(" ");
					if (padding) padding--;
				}

				tb_printf(" %02X", p[i]);
				if (padding >= 3) padding -= 3;
			}

			// dump spaces
			while (padding--) tb_printf(" ");
				
			// dump characters
			for (i = 0; i < left; i++)
			{
				tb_printf("%c", tb_isgraph(p[i])? p[i] : '.');
			}

			// dump new line
			tb_printf("\n");

			// update p
			p += left;
		}
		// end
		else break;
	}
}
static tb_vpool_block_t* tb_vpool_overflow_find(tb_vpool_t* vpool)
{
	// check
	tb_assert_and_check_return(vpool);

	// walk
	tb_byte_t* 			pb = vpool->data;
	tb_byte_t* 			pe = pb + vpool->size;
	tb_size_t 			nhead = vpool->nhead;
	tb_vpool_block_t* 	prev = tb_null;
	while (pb + nhead < pe)
	{
		// the block
		tb_vpool_block_t* block = (tb_vpool_block_t*)pb;

		// overflow?
		if (block->magic != TB_VPOOL_MAGIC || !block->size || block->size >= vpool->size)
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
static tb_bool_t tb_vpool_overflow_check(tb_vpool_t* vpool, tb_vpool_block_t* block, tb_vpool_block_t* prev)
{
	// overflow?
	if (block->magic != TB_VPOOL_MAGIC || !block->size || block->size >= vpool->size) 
	{
		// find the previous block if null
		if (!prev) prev = tb_vpool_overflow_find(vpool);

		// dump backtrace
		if (prev)
		{
			// free?
			if (prev->free)
			{
				// dump
				tb_print("vpool: overflow: magic: data: %p size: %lu free: %lu"
						, (tb_byte_t const*)prev + vpool->nhead
						, prev->real
						, prev->free
						);
				
				// dump data
				tb_vpool_dump_data((tb_byte_t const*)prev, vpool->nhead + prev->size);
				tb_vpool_dump_data((tb_byte_t const*)prev + vpool->nhead + prev->size, vpool->nhead);
			}
			else
			{
				// dump
				tb_print("vpool: overflow: magic: data: %p size: %lu free: %lu at %s(): %d, file: %s"
						, (tb_byte_t const*)prev + vpool->nhead
						, prev->real
						, prev->free
						, prev->func
						, prev->line
						, prev->file
						);

				// dump backtrace
				tb_vpool_dump_backtrace("vpool:     ", prev);

				// dump data
				tb_vpool_dump_data((tb_byte_t const*)prev, vpool->nhead + prev->size);
				tb_vpool_dump_data((tb_byte_t const*)prev + vpool->nhead + prev->size, vpool->nhead);
			}
		}
		else tb_print("vpool: overflow");

		// abort
		return tb_false;
	}

	// has padding data?
	if (!block->free && block->real < block->size)
	{
		// find 0xcc
		tb_byte_t const* 	p = (tb_byte_t const*)block + vpool->nhead + block->real;
		tb_byte_t const* 	e = (tb_byte_t const*)block + vpool->nhead + block->size;
		if (p + 4 < e) e = p + 4; while (p < e && *p == 0xcc) p++;
		
		// no 0xcc? overflow?
		if (p < e)
		{
			// dump
			tb_print("vpool: overflow: fill: data: %p size: %lu free: %lu at %s(): %d, file: %s"
					, (tb_byte_t const*)block + vpool->nhead
					, block->real
					, block->free
					, block->func
					, block->line
					, block->file
					);

			// dump backtrace
			tb_vpool_dump_backtrace("vpool:     ", block);

			// dump data
			tb_vpool_dump_data((tb_byte_t const*)block, vpool->nhead + block->size);

			// abort
			return tb_false;
		}
	}
	// ok
	return tb_true;
}
#else
static __tb_inline__ tb_bool_t tb_vpool_overflow_check(tb_vpool_t* vpool, tb_vpool_block_t* block, tb_vpool_block_t* prev)
{
	// check
	tb_check_return_val(block->size < vpool->size, tb_false);

	// ok
	return tb_true;
}
#endif

// malloc from the given data address
static tb_pointer_t tb_vpool_malloc_from(tb_vpool_t* vpool, tb_byte_t* data, tb_size_t size, tb_size_t tryn)
{
	// pb & pe
	tb_byte_t* 	pb = vpool->data;
	tb_byte_t* 	pe = pb + vpool->size;

	// the data
	tb_byte_t* 	p = data;
	tb_check_return_val(p, tb_null);
	tb_assert_and_check_return_val(p >= pb && p < pe, tb_null);

	// is pred?
	tb_bool_t 	bpred = tryn == 1? tb_true : tb_false;

	// the nhead
	tb_size_t 	nhead = vpool->nhead;

	// is align?
	tb_assert(!(size & (vpool->align - 1)));
	tb_assert(!(nhead & (vpool->align - 1)));
	tb_assert(!((tb_size_t)data & (vpool->align - 1)));

	// find the free block
	tb_size_t 			maxn = 1;
	tb_byte_t* 			pred = tb_null;
	while (p + nhead <= pe && tryn)
	{
		// the block
		tb_vpool_block_t* 	block = ((tb_vpool_block_t*)p);
		tb_size_t 			bsize = ((tb_vpool_block_t*)p)->size;
			
		// overflow?
		tb_check_abort(tb_vpool_overflow_check(vpool, block, tb_null));

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
				#ifdef __tb_debug__
					next->real = 0;
					next->magic = TB_VPOOL_MAGIC;
				#endif
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
					vpool->pred = tb_null;

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
						vpool->pred = tb_null;

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
	return tb_null;
}

#ifndef __tb_debug__
static tb_pointer_t tb_vpool_malloc_skip_frame(tb_handle_t handle, tb_size_t size)
#else
static tb_pointer_t tb_vpool_malloc_skip_frame(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return_val(vpool && vpool->magic == TB_VPOOL_MAGIC, tb_null);

	// no size?
	tb_check_return_val(size, tb_null);

	// align size
	tb_size_t asize = tb_align(size, vpool->align);

	// full?
	tb_check_return_val(!vpool->full || asize <= vpool->full, tb_null);

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
#ifdef __tb_debug__
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

		// set real
		block->real = size;

		// fill 0xcc
		if (block->real < block->size) tb_memset(p + block->real, 0xcc, block->size - block->real);

		// set frames
		tb_size_t nframe = tb_backtrace_frames(block->frames, tb_arrayn(block->frames), 5);
		if (nframe < tb_arrayn(block->frames)) tb_memset(block->frames + nframe, 0, (tb_arrayn(block->frames) - nframe) * sizeof(tb_cpointer_t));

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
tb_pointer_t tb_vpool_ralloc_fast(tb_vpool_t* vpool, tb_pointer_t data, tb_size_t size, tb_size_t* osize);
tb_pointer_t tb_vpool_ralloc_fast(tb_vpool_t* vpool, tb_pointer_t data, tb_size_t size, tb_size_t* osize)
{
	// check
	tb_assert_and_check_return_val(vpool && vpool->magic == TB_VPOOL_MAGIC, tb_null);

	// no data?
	tb_check_return_val(data, tb_null);

	// pb & pe
	tb_byte_t* 	pb = vpool->data;
	tb_byte_t* 	pe = pb + vpool->size;

	// the data
	tb_byte_t* 	p = data;
	tb_check_return_val(p && p >= pb && p < pe, tb_null);

	// the nhead
	tb_size_t 	nhead = vpool->nhead;
	
	// the block
	tb_vpool_block_t* 	block = ((tb_vpool_block_t*)(p - nhead));
#ifdef __tb_debug__
	tb_size_t 			bsize = block->size;
#endif

	// check block
	tb_assert_return_val(block->magic == TB_VPOOL_MAGIC, tb_null);
	tb_assert_and_check_return_val(block->size < vpool->size, tb_null);
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
	tb_size_t asize = tb_align(size, vpool->align);

	// merge it if the next block is free and the space is enough
	tb_vpool_block_t* next = (tb_vpool_block_t*)(p + block->size);
	if (next->free && block->size + nhead + next->size >= asize) 
	{
		// merge it
		block->size += nhead + next->size;

		// split it if the free block is too large
		if (block->size > asize + nhead)
		{
			// split block
			next = (tb_vpool_block_t*)(p + asize);
			next->size = block->size - asize - nhead;
			next->free = 1;
#ifdef __tb_debug__
			next->real = 0;
			next->magic = TB_VPOOL_MAGIC;
#endif
			block->size = asize;

			// predict the next free block
			vpool->pred = next;
		}
		// reset the predicted block
		else if (vpool->pred == (tb_byte_t*)block || vpool->pred == (tb_byte_t*)next || vpool->pred == pe)
			vpool->pred = tb_null;

		// reset full
		vpool->full = 0;

		// osize
		if (osize) *osize = block->size;

#ifdef __tb_debug__

		// update the used size
		vpool->info.used += block->size - bsize;

		// update the need size
		vpool->info.need += block->size - bsize;

		// update the real size		
		vpool->info.real += block->size - bsize;

		// update the peak size
		if (vpool->info.used > vpool->info.peak) vpool->info.peak = vpool->info.used;
		
		// update the real size
		block->real = size;

		// fill 0xcc
		if (block->real < block->size) tb_memset(data + block->real, 0xcc, block->size - block->real);
#endif

		// ok
		return data;
	}

	return tb_null;
}


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_vpool_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	// check
	tb_assert_and_check_return_val(data && size, tb_null);

	// align
	align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
	align = tb_max(align, TB_CPU_BITBYTE);
	tb_assert_and_check_return_val(align <= TB_VPOOL_ALIGN_MAXN, tb_null);

	// align data
	tb_size_t byte = (tb_size_t)tb_align((tb_size_t)data, align) - (tb_size_t)data;
	tb_assert_and_check_return_val(size >= byte, tb_null);
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
	tb_assert_and_check_return_val(data + size > vpool->data, tb_null);

	// init size
	vpool->size = (tb_byte_t*)data + size - vpool->data;
	tb_assert_and_check_return_val(vpool->size > vpool->nhead, tb_null);

	// init block, only one free block now.
	((tb_vpool_block_t*)vpool->data)->free = 1;
	((tb_vpool_block_t*)vpool->data)->size = vpool->size - vpool->nhead;
#ifdef __tb_debug__
	((tb_vpool_block_t*)vpool->data)->real = 0;
	((tb_vpool_block_t*)vpool->data)->magic = TB_VPOOL_MAGIC;
#endif

	// init pred
	vpool->pred = vpool->data;

	// init full
	vpool->full = 0;

	// init info
#ifdef __tb_debug__
	vpool->info.used = 0;
	vpool->info.peak = 0;
	vpool->info.need = 0;
	vpool->info.real = 0;
	vpool->info.fail = 0;
	vpool->info.pred = 0;
	vpool->info.aloc = 0;
	if (vpool->nhead < vpool->size) tb_memset(vpool->data + vpool->nhead, 0xcc, vpool->size - vpool->nhead);
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

	// init block, only one free block now.
	((tb_vpool_block_t*)vpool->data)->free = 1;
	((tb_vpool_block_t*)vpool->data)->size = vpool->size - vpool->nhead;
#ifdef __tb_debug__
	((tb_vpool_block_t*)vpool->data)->real = 0;
	((tb_vpool_block_t*)vpool->data)->magic = TB_VPOOL_MAGIC;
#endif

	// reinit pred
	vpool->pred = vpool->data;

	// reinit full
	vpool->full = 0;

	// reinit info
#ifdef __tb_debug__
	vpool->info.used = 0;
	vpool->info.peak = 0;
	vpool->info.need = 0;
	vpool->info.real = 0;
	vpool->info.fail = 0;
	vpool->info.pred = 0;
	vpool->info.aloc = 0;
	if (vpool->nhead < vpool->size) tb_memset(vpool->data + vpool->nhead, 0xcc, vpool->size - vpool->nhead);
#endif
}

#ifndef __tb_debug__
tb_pointer_t tb_vpool_malloc_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_vpool_malloc_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// malloc
#ifndef __tb_debug__
	return tb_vpool_malloc_skip_frame(handle, size);
#else
	return tb_vpool_malloc_skip_frame(handle, size, func, line, file);
#endif
}

#ifndef __tb_debug__
tb_pointer_t tb_vpool_malloc0_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_vpool_malloc0_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// malloc
#ifndef __tb_debug__
	tb_byte_t* p = tb_vpool_malloc_skip_frame(handle, size);
#else
	tb_byte_t* p = tb_vpool_malloc_skip_frame(handle, size, func, line, file);
#endif

	// clear
	if (p && size) tb_memset(p, 0, size);

	// ok?
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_vpool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_vpool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// malloc
#ifndef __tb_debug__
	return tb_vpool_malloc_skip_frame(handle, item * size);
#else
	return tb_vpool_malloc_skip_frame(handle, item * size, func, line, file);
#endif
}

#ifndef __tb_debug__
tb_pointer_t tb_vpool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_vpool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// malloc
#ifndef __tb_debug__
	tb_pointer_t p = tb_vpool_malloc_skip_frame(handle, item * size);
#else
	tb_pointer_t p = tb_vpool_malloc_skip_frame(handle, item * size, func, line, file);
#endif

	// clear
	if (p && (item * size)) tb_memset(p, 0, item * size);

	// ok?
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_vpool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_vpool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return_val(vpool && vpool->magic == TB_VPOOL_MAGIC, tb_null);

	// free it if no size
	if (!size)
	{
#ifndef __tb_debug__
		tb_vpool_free_impl(vpool, data);
#else
		tb_vpool_free_impl(vpool, data, func, line, file);
#endif
		return tb_null;
	}

	// alloc it if no data?
#ifndef __tb_debug__
	if (!data) return tb_vpool_malloc_skip_frame(vpool, size);
#else
	if (!data) return tb_vpool_malloc_skip_frame(vpool, size, func, line, file);
#endif
	
	// ralloc it with fast mode if enough
	tb_size_t 		osize = 0;
	tb_pointer_t 	pdata = tb_vpool_ralloc_fast(vpool, data, size, &osize);
	tb_check_return_val(!pdata, pdata);
	tb_assert_and_check_return_val(osize && osize < size, tb_null);

	// malloc it
#ifndef __tb_debug__
	pdata = tb_vpool_malloc_skip_frame(vpool, size);
#else
	pdata = tb_vpool_malloc_skip_frame(vpool, size, func, line, file);
#endif
	tb_check_return_val(pdata, tb_null);
	tb_assert_and_check_return_val(pdata != data, pdata);

	// copy data
	tb_memcpy(pdata, data, osize);
	
	// free it
#ifndef __tb_debug__
	tb_vpool_free_impl(vpool, data);
#else
	tb_vpool_free_impl(vpool, data, func, line, file);
#endif

	// ok
	return pdata;
}

#ifndef __tb_debug__
tb_bool_t tb_vpool_free_impl(tb_handle_t handle, tb_pointer_t data)
#else
tb_bool_t tb_vpool_free_impl(tb_handle_t handle, tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return_val(vpool && vpool->magic == TB_VPOOL_MAGIC, tb_false);

	// no data?
	tb_check_return_val(data, tb_true);

	// pb & pe
	tb_byte_t* 	pb = vpool->data;
	tb_byte_t* 	pe = pb + vpool->size;

	// the data
	tb_byte_t* 	p = data;
	tb_check_return_val(p >= pb && p < pe, tb_false);

	// the nhead
	tb_size_t 	nhead = vpool->nhead;
	
	// the block
	tb_vpool_block_t* 	block = ((tb_vpool_block_t*)(p - nhead));
#ifdef __tb_debug__
	tb_size_t 			bsize = block->size;
#endif

	// check overflow
	tb_check_abort(tb_vpool_overflow_check(vpool, block, tb_null));

	// double free?
	if (block->free)
	{
		// trace
		tb_trace("vpool: double free: at %s(): %d, file: %s", block->func, block->line, block->file);

#ifdef __tb_debug__
		// dump backtrace
		tb_vpool_dump_backtrace("vpool:     ", block);
#endif

		return tb_true;
	}
	tb_assert_and_check_return_val(!block->free, tb_true);

	// check the next block
	tb_vpool_block_t* next = (tb_vpool_block_t*)(p + block->size);
	if ((tb_byte_t const*)next + nhead < pe)
	{
		// check overflow
		tb_check_abort(tb_vpool_overflow_check(vpool, next, block));

		// merge it if the next block is free
		if (next->free) block->size += nhead + next->size;
	}

	// free it
	block->free = 1;

	// predict the next free block
	vpool->pred = (tb_byte_t*)block;

	// reinit full
	vpool->full = 0;

#ifdef __tb_debug__
	// update the used
	vpool->info.used -= bsize;

	// clear the real
	block->real = 0;

	// fill 0xcc
	if (bsize) tb_memset(p, 0xcc, bsize);
	if (bsize < block->size) tb_memset(p + bsize, 0xcc, nhead);
#endif

	// ok
	return tb_true;
}


#ifdef __tb_debug__
tb_void_t tb_vpool_dump(tb_handle_t handle, tb_char_t const* prefix)
{
	tb_vpool_t* vpool = (tb_vpool_t*)handle;
	tb_assert_and_check_return(vpool);

	// prefix
	if (!prefix) prefix = "vpool";

	// dump
	tb_print("======================================================================");

	// walk
	tb_size_t 			i = 0;
	tb_byte_t* 			pb = vpool->data;
	tb_byte_t* 			pe = pb + vpool->size;
	tb_size_t 			nhead = vpool->nhead;
	tb_vpool_block_t* 	prev = tb_null;
	tb_size_t 			prev_i = 0;
	tb_bool_t 			ok = tb_true;
	tb_size_t 			frag = 0;
	while (pb + nhead < pe)
	{
		// the block
		tb_vpool_block_t* block = (tb_vpool_block_t*)pb;

		// check overflow
		tb_check_break((ok = tb_vpool_overflow_check(vpool, block, prev)));

		// no free?
		if (!block->free)
		{
			// dump leak
			tb_print("%s: leak: data: %p size: %lu at %s(): %d, file: %s"
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
				tb_vpool_dump_backtrace(backtrace_prefix, block);
			}

			// leak
			ok = tb_false;			
		}

		// next
		pb += nhead + block->size;
		prev = block;
		prev_i = i - 1;
		frag++;
	}

	// dump 
	if (!ok) tb_print("");
//	tb_print("%s: magic: %#lx",		prefix, vpool->magic);
//	tb_print("%s: nhead: %lu", 		prefix, vpool->nhead);
	tb_print("%s: align: %lu", 		prefix, vpool->align);
//	tb_print("%s: head: %lu", 		prefix, vpool->data - (tb_byte_t*)vpool);
//	tb_print("%s: data: %p", 		prefix, vpool->data);
	tb_print("%s: size: %lu", 		prefix, vpool->size);
	tb_print("%s: full: %s", 		prefix, vpool->full? "true" : "false");
	tb_print("%s: used: %lu", 		prefix, vpool->info.used);
	tb_print("%s: peak: %lu", 		prefix, vpool->info.peak);
	tb_print("%s: wast: %lu%%", 	prefix, (vpool->info.real + (vpool->data - (tb_byte_t*)vpool) - vpool->info.need) * 100 / (vpool->info.real + (vpool->data - (tb_byte_t*)vpool)));
	tb_print("%s: fail: %lu", 		prefix, vpool->info.fail);
	tb_print("%s: pred: %lu%%", 	prefix, vpool->info.aloc? ((vpool->info.pred * 100) / vpool->info.aloc) : 0);
	tb_print("%s: frag: %lu", 		prefix, frag);
}
#endif
