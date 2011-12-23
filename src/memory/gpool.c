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
 * \file		gpool.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gpool.h"
#include "../libc/libc.h"

/*!structure
 *
 * gpool->data: |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 *                                                                                                gpool->size
 *
 *                   						regular blocks        		                                   non-regular blocks                                                  for regular blocks
 * gpool->data: |----------------|----------------|---------------|-------   ...  --------|-----------------------------------------------------------------|--------------------------------------------------|
 *                  chunks[0]        chunks[1]        chunks[2]              ...              chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX] : large gpool chunk                blocks_info: size == align(blocks_n, 8) / 8
 *
 * non-regular block size:
 * chunks[0]: <= gpool->size - (chunks[0].size + chunks[2].size + ... + chunks[TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT - 1].size) - align(blocks_n, 8) / 8
 *
 * regular block size:
 *
 * chunks[0]: (16 << 0) = 16 bytes
 * chunks[1]: (16 << 1) = 32 bytes
 * chunks[2]: (16 << 2) = 64 bytes
 * chunks[3]: (16 << 3) = 128 bytes
 * chunks[4]: (16 << 4) = 256 bytes
 * chunks[5]: (16 << 5) = 512 bytes
 */

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#if TB_CPU_BITSIZE == 32
# 	define TB_GPOOL_MAGIC 							(0xdeadbeef)
#elif TB_CPU_BITSIZE == 64
# 	define TB_GPOOL_MAGIC 							(0xdeadbeefdeadbeef)
#else
# 	error the bitsize of cpu is not supported.
#endif

// align boundary
#define TB_GPOOL_ALIGN_BOUNDARY 					(TB_CPU_BITBYTE)

// praticle size
#define TB_GPOOL_PRATICLE_SIZE 						(16)

// the regular chunk maximum count
#define TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT 	 		(7)
// regular block maximum size
#define TB_GPOOL_REGULAR_BLOCK_MAX_SIZE 			(TB_GPOOL_PRATICLE_SIZE << (TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT - 1))

// the non-regular chunk minimum size
#define TB_GPOOL_NON_REGULAR_CHUNCK_MIN_SIZE 		(128)
// non-regular chunk index
#define TB_GPOOL_NON_REGULAR_CHUNCK_INDEX 			(TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT)

// alloc & free without lock
#ifndef TB_DEBUG
# 	define TB_GPOOL_ALLOCATE(gpool, size, func, line, file) 									tb_gpool_allocate(gpool, size)
# 	define TB_GPOOL_TRY_ALLOCATING_FROM_PRED_RBLOCK(gpool, size, chunk_i, func, line, file) 	tb_gpool_try_allocating_from_pred_rblock(gpool, size, chunk_i)
# 	define TB_GPOOL_TRY_ALLOCATING_FROM_PRED_NRBLOCK(gpool, size, asize, func, line, file) 		tb_gpool_try_allocating_from_pred_nrblock(gpool, size, asize)
#else
# 	define TB_GPOOL_ALLOCATE(gpool, size, func, line, file) 									tb_gpool_allocate(gpool, size, func, line, file)
# 	define TB_GPOOL_TRY_ALLOCATING_FROM_PRED_RBLOCK(gpool, size, chunk_i, func, line, file) 	tb_gpool_try_allocating_from_pred_rblock(gpool, size, chunk_i, func, line, file)
# 	define TB_GPOOL_TRY_ALLOCATING_FROM_PRED_NRBLOCK(gpool, size, asize, func, line, file) 		tb_gpool_try_allocating_from_pred_nrblock(gpool, size, asize, func, line, file)
#endif

// prediction
#define TB_GPOOL_PRED_ENABLE
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_GPOOL_PRED_RBLOCKS_MAX 		(128)
#else
# 	define TB_GPOOL_PRED_RBLOCKS_MAX 		(256)
#endif

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

// the non-regular block head type
typedef struct __tb_gpool_nrblock_head_t
{
	// the bounds magic
#ifdef TB_DEBUG
	tb_size_t 			magic_b;
#endif

	tb_size_t 			block_size 	: (TB_CPU_BITSIZE - 1);
	tb_size_t 			is_free 	: 1;

#ifdef TB_DEBUG

	// the bounds magic
	tb_size_t 			magic_e;

	// the trace info
	tb_char_t const* 	code_file;
	tb_char_t const* 	code_func;
	tb_size_t 			code_line;

#endif

}tb_gpool_nrblock_head_t;

// the chunk type
typedef struct __tb_gpool_chunk_t
{
	// the chunk data
	tb_byte_t* 		data;
	tb_size_t 		size;

	// \note only used for regular chunk
	tb_size_t 		start_block;
	tb_size_t 		block_n;
	tb_size_t 		block_size;
	tb_size_t 		free_block_n;

}tb_gpool_chunk_t;

#ifdef TB_DEBUG
// the gpool status type
typedef struct __tb_gpool_status_t
{
	// total size
	tb_size_t 			total_size;
	// current used size
	tb_size_t 			used_size;
	// maximum used size
	tb_size_t 			peak_size;

	// for computing wasted rate
	// waste rate = 1 - need_used_size / real_used_size
	tb_size_t 			need_used_size;
	tb_size_t 			real_used_size;

	// the number of allocation failure
	tb_size_t 			alloc_failed;

	// the number of prediction failure
	tb_size_t 			pred_failed;
	tb_size_t 			alloc_total;

	// regular chuncks info
	tb_size_t 			rblock_max_n[TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT];

	// non-regular chunk info
	tb_size_t 			nrblock_max_size;

}tb_gpool_status_t;
#endif

// the gpool type
typedef struct __tb_gpool_t
{
	// the magic number
	tb_size_t 			magic;

	// the total gpool data
	tb_byte_t* 			data;
	tb_size_t 			size;

	// the info of used regular blocks
	// 0 1 0 1 1 1 ...
	// 1: the block is non-free
	// 0: the block is free
	tb_byte_t* 			blocks_info;
	tb_size_t 			blocks_n;

#ifdef TB_DEBUG
	// the gpool info 
	tb_gpool_status_t 	status;
#endif

	// the gpool chunks
	// some regular chunks + one non-regular chunk
	tb_gpool_chunk_t 	chunks[TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT + 1];

	// the predicted block
#ifdef TB_GPOOL_PRED_ENABLE
	tb_size_t 			pred_rblocks[TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT][TB_GPOOL_PRED_RBLOCKS_MAX];
	tb_size_t 			pred_rblocks_n[TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT];
	tb_byte_t* 			pred_nrblock;
#endif

}tb_gpool_t;

/* ////////////////////////////////////////////////////////////////////////
 * the globals
 */

// head + info + safe
tb_size_t const g_gpool_size_min = tb_align8(sizeof(tb_gpool_t) + ((TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT * 1024) >> 3) + 256);

/* ////////////////////////////////////////////////////////////////////////
 * the details
 */

#ifdef TB_DEBUG
static __tb_inline__ tb_void_t tb_gpool_blocks_info_set(tb_byte_t* blocks_info, tb_size_t block_i)
{
	tb_assert(blocks_info);
	blocks_info[block_i >> 3] |= (0x1 << (block_i & 0x7));
}
static __tb_inline__ tb_void_t tb_gpool_blocks_info_reset(tb_byte_t* blocks_info, tb_size_t block_i)
{
	tb_assert(blocks_info);
	blocks_info[block_i >> 3] &= ~(0x1 << (block_i & 0x7));
}
static __tb_inline__ tb_size_t tb_gpool_blocks_info_isset(tb_byte_t* blocks_info, tb_size_t block_i)
{
	tb_assert(blocks_info);
	return (blocks_info[block_i >> 3] & (0x1 << (block_i & 0x7)));
}
#else
# 	define tb_gpool_blocks_info_set(info, i) 	do {(info)[(i) >> 3] |= (0x1 << ((i) & 0x7));} while (0)
# 	define tb_gpool_blocks_info_reset(info, i) 	do {(info)[(i) >> 3] &= ~(0x1 << ((i) & 0x7));} while (0)
# 	define tb_gpool_blocks_info_isset(info, i) 	((info)[(i) >> 3] & (0x1 << ((i) & 0x7)))
#endif

static __tb_inline__ tb_size_t tb_block_size_to_chunk_i(tb_gpool_chunk_t* chunks, tb_size_t block_size)
{
	tb_size_t i = 0;
	// find the chunk index of the first free regular-block
	for (i = 0; i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT; ++i) 
		if (chunks[i].free_block_n && block_size <= chunks[i].block_size) return i;

	// regular chunks is full 
	return i;
}

// prediction
#ifdef TB_GPOOL_PRED_ENABLE

// try allocating it from the prediction rblock
#ifndef TB_DEBUG
static tb_pointer_t tb_gpool_try_allocating_from_pred_rblock(tb_gpool_t* gpool, tb_size_t size, tb_size_t chunk_i)
#else
static tb_pointer_t tb_gpool_try_allocating_from_pred_rblock(tb_gpool_t* gpool, tb_size_t size, tb_size_t chunk_i, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// no predicted block
	if (!gpool->pred_rblocks_n[chunk_i]) return TB_NULL;

	tb_size_t block_i = gpool->pred_rblocks[chunk_i][--gpool->pred_rblocks_n[chunk_i]];
	tb_size_t block_n = gpool->chunks[chunk_i].block_n;
	tb_size_t block_b = gpool->chunks[chunk_i].start_block;
	if (block_i >= block_b && block_i < block_b + block_n)
	{
		tb_size_t i = block_i - block_b;

		// check
		tb_assert(gpool->chunks[chunk_i].free_block_n && size <= gpool->chunks[chunk_i].block_size);

		// set used flag: the block is non-free
		tb_gpool_blocks_info_set(gpool->blocks_info, block_i);
		// update the count of free blocks in the chunk
		--gpool->chunks[chunk_i].free_block_n;

 		// predict next, pred_n must be null, otherwise exists repeat item
		if (!gpool->pred_rblocks_n[chunk_i])
		{
			if ((i + 1 < block_n) && !tb_gpool_blocks_info_isset(gpool->blocks_info, block_i)) 
			{
				gpool->pred_rblocks[chunk_i][0] = block_i + 1;
				gpool->pred_rblocks_n[chunk_i] = 1;
			}
		}

	#ifdef TB_DEBUG
		// update gpool status info
		gpool->status.used_size += gpool->chunks[chunk_i].block_size;
		if (gpool->status.used_size > gpool->status.peak_size) gpool->status.peak_size = gpool->status.used_size;
		gpool->status.need_used_size += size;
		gpool->status.real_used_size += gpool->chunks[chunk_i].block_size;
		{
			tb_size_t block_n = gpool->chunks[chunk_i].block_n - gpool->chunks[chunk_i].free_block_n;
			gpool->status.rblock_max_n[chunk_i] = block_n > gpool->status.rblock_max_n[chunk_i]? block_n : gpool->status.rblock_max_n[chunk_i];
		}
	#endif

		// return data address
		return (gpool->chunks[chunk_i].data + i * gpool->chunks[chunk_i].block_size);
	}

	return TB_NULL;
}

// try allocating it from the prediction nrblock
#ifndef TB_DEBUG
static tb_pointer_t tb_gpool_try_allocating_from_pred_nrblock(tb_gpool_t* gpool, tb_size_t size, tb_size_t asize)
#else
static tb_pointer_t tb_gpool_try_allocating_from_pred_nrblock(tb_gpool_t* gpool, tb_size_t size, tb_size_t asize, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	tb_byte_t* 	pb = gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].data;
	tb_byte_t* 	pe = pb + gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].size;
	tb_byte_t* 	p = gpool->pred_nrblock;

	// is null?
	if (!p || p < pb || p >= pe) return TB_NULL;

	// find free block
	while (p + sizeof(tb_gpool_nrblock_head_t) < pe)
	{
		// get the head of current block
		tb_gpool_nrblock_head_t* phead = ((tb_gpool_nrblock_head_t*)p);
		if (phead->is_free) // allocate if the block is free
		{
			// is enough?
			tb_size_t osize = ((tb_gpool_nrblock_head_t*)p)->block_size;
			if (osize >= asize)
			{
				// skip head
				p += sizeof(tb_gpool_nrblock_head_t);

				// update head of the next free block if the free block is splited
				if (osize > asize + sizeof(tb_gpool_nrblock_head_t))
				{
					// split block
					tb_gpool_nrblock_head_t* pnext_head = (tb_gpool_nrblock_head_t*)(p + asize);
					pnext_head->block_size = osize - asize - sizeof(tb_gpool_nrblock_head_t);
					pnext_head->is_free = 1;

					// update the head of block
					phead->block_size = asize;	// set block size
					phead->is_free = 0; 		// now it is non-free
				}
				// use the whole block if the free block is not splited
				else 
				{
					// update the head of block
					phead->block_size = osize; 	// use the whole block
					phead->is_free = 0; 		// now it is non-free
				}

				// check address
				tb_assert(p + phead->block_size <= pe);
				tb_assert(!(((tb_size_t)p) & (TB_GPOOL_ALIGN_BOUNDARY - 1)));

				// predict the next free block
			#ifdef TB_GPOOL_PRED_ENABLE
				gpool->pred_nrblock = p + phead->block_size;
			#endif

			#ifdef TB_DEBUG
				// set bounds magic
				phead->magic_b = TB_GPOOL_MAGIC;
				phead->magic_e = TB_GPOOL_MAGIC;

				// save the trace info
				phead->code_line = line;
				phead->code_file = file;
				phead->code_func = func;

				// update gpool status info
				gpool->status.used_size += phead->block_size;
				if (gpool->status.used_size > gpool->status.peak_size) gpool->status.peak_size = gpool->status.used_size;
				gpool->status.need_used_size += size;
				gpool->status.real_used_size += phead->block_size;
				gpool->status.nrblock_max_size = phead->block_size > gpool->status.nrblock_max_size? phead->block_size : gpool->status.nrblock_max_size;
			#endif

				// return data address
				return p;
			}
#if 1
			else // attempt to merge next free block if current free block is too small
			{
				tb_gpool_nrblock_head_t* pnext_head = (tb_gpool_nrblock_head_t*)(p + sizeof(tb_gpool_nrblock_head_t) + osize);
			
				// break if doesn't exist next block
				if ((tb_size_t)pnext_head + sizeof(tb_gpool_nrblock_head_t) >= (tb_size_t)gpool->blocks_info) break;

 				// next block is free?
				if (pnext_head->is_free)
				{
					// merge next block
					phead->block_size += pnext_head->block_size + sizeof(tb_gpool_nrblock_head_t);

					// continue handle this block
					continue ;
				}
			}
#endif
		}
		break;
	}	

	return TB_NULL;
}
#endif

#ifdef TB_DEBUG
static tb_bool_t tb_gpool_allocate_try(tb_gpool_t* gpool, tb_size_t size)
{
	tb_assert_and_check_return_val(gpool && size, TB_FALSE);

	// allocate it from regular-chunks if block is small
	if (size <= TB_GPOOL_REGULAR_BLOCK_MAX_SIZE)
	{
		// find the chunk index of the first free regular-block
		tb_size_t chunk_i = tb_block_size_to_chunk_i(gpool->chunks, size);

		// if exists free block
		if (chunk_i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT)
		{
			// allocate a free block
			tb_size_t i = 0;
			for (i = 0; i < gpool->chunks[chunk_i].block_n; ++i)
			{
				tb_size_t block_i = gpool->chunks[chunk_i].start_block + i;
				if (!tb_gpool_blocks_info_isset(gpool->blocks_info, block_i)) // is free?
				{
					return TB_TRUE;
				}
			}
		}
	}

	// allocate it from non-regular chunk
	tb_byte_t* 	p = gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].data;
	tb_byte_t* 	pe = p + gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].size;

	// find free block
	while (p + sizeof(tb_gpool_nrblock_head_t) < pe)
	{
		// get the head of current block
		tb_gpool_nrblock_head_t* 	phead = ((tb_gpool_nrblock_head_t*)p);
		tb_size_t 					osize = ((tb_gpool_nrblock_head_t*)p)->block_size;

		if (phead->is_free) // allocate if the block is free
		{
			// align size
			tb_size_t asize = tb_align(size, TB_GPOOL_ALIGN_BOUNDARY);
			if (osize >= asize) // enough?
			{
				return TB_TRUE;
			}
			else // attempt to merge next free block if current free block is too small
			{
				tb_gpool_nrblock_head_t* pnext_head = (tb_gpool_nrblock_head_t*)(p + sizeof(tb_gpool_nrblock_head_t) + osize);
			
				// break if doesn't exist next block
				if ((tb_size_t)pnext_head + sizeof(tb_gpool_nrblock_head_t) >= (tb_size_t)gpool->blocks_info) break;

				if (pnext_head->is_free) // next block is free?
				{
					// merge next block
					phead->block_size += pnext_head->block_size + sizeof(tb_gpool_nrblock_head_t);

					// reset the next predicted block
				#ifdef TB_GPOOL_PRED_ENABLE
					if (gpool->pred_nrblock == (tb_byte_t*)pnext_head)
						gpool->pred_nrblock = TB_NULL;
				#endif

					// continue handle this block
					continue ;
				}
			}
		}

		// skip it if the block is non-free or block_size is too small
		p += sizeof(tb_gpool_nrblock_head_t) + osize;
	}	

	return TB_FALSE;
}
#endif

/* ////////////////////////////////////////////////////////////////////////
 * the implemention
 */
tb_handle_t tb_gpool_init(tb_pointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(data && size, TB_NULL);
	tb_assert_static(tb_ispow2(TB_GPOOL_ALIGN_BOUNDARY));
	tb_assert_static(!(sizeof(tb_gpool_nrblock_head_t) & (TB_GPOOL_ALIGN_BOUNDARY - 1)));
	
	// init regular block 
	tb_size_t rblockn[TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT] = {0, 0, 0, 0, 0, 0, 0};
#ifndef TB_DEBUG
	rblockn[0] = size >> 14;
	rblockn[1] = rblockn[0] >> 1;
	rblockn[2] = rblockn[1] >> 1;
	rblockn[3] = rblockn[2] >> 1;
	rblockn[4] = rblockn[3] >> 1;
	rblockn[5] = rblockn[4] >> 1;
	rblockn[6] = rblockn[5] >> 1;
#endif

	// init data
	tb_memset(data, 0, size);

	// init gpool
	tb_gpool_t* gpool = data;
	gpool->magic = TB_GPOOL_MAGIC;

	// attach data
	gpool->data = &gpool[1];
	gpool->data = (tb_byte_t*)tb_align((tb_size_t)gpool->data, TB_GPOOL_ALIGN_BOUNDARY);

	tb_assert((tb_size_t)data + size > (tb_size_t)gpool->data);
	gpool->size = (tb_size_t)((tb_size_t)data + size - (tb_size_t)gpool->data);

	// init regular chunks
	tb_size_t i = 0;
	gpool->blocks_n = 0;
	for (i = 0; i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT; ++i)
	{
		// init regular chunk
		if (i == 0) gpool->chunks[i].data = gpool->data;
		else gpool->chunks[i].data = gpool->chunks[i - 1].data + gpool->chunks[i - 1].size;

		gpool->chunks[i].block_n = rblockn[i];
		gpool->chunks[i].free_block_n = gpool->chunks[i].block_n;
		gpool->chunks[i].block_size = TB_GPOOL_PRATICLE_SIZE << i;
		gpool->chunks[i].size = gpool->chunks[i].block_size * gpool->chunks[i].block_n;

		// stats total blocks count
		gpool->chunks[i].start_block = gpool->blocks_n;
		gpool->blocks_n += gpool->chunks[i].block_n;

		// init predicted info
#ifdef TB_GPOOL_PRED_ENABLE
		tb_size_t m = rblockn[i] < TB_GPOOL_PRED_RBLOCKS_MAX? rblockn[i] : TB_GPOOL_PRED_RBLOCKS_MAX;
		tb_size_t n = m;
		while (n--) gpool->pred_rblocks[i][n] = gpool->chunks[i].start_block + m - n - 1;
		gpool->pred_rblocks_n[i] = m;
#endif
	}

	// init blocks info
	gpool->blocks_info = gpool->data + gpool->size - (tb_align(gpool->blocks_n, 8) >> 3);
	gpool->blocks_info -= ((tb_size_t)gpool->blocks_info) & (TB_GPOOL_ALIGN_BOUNDARY - 1);
	tb_assert((tb_size_t)gpool->blocks_info > (tb_size_t)gpool->data);
  
	// init non-regular chunk
	tb_assert(i >= 1 && i == TB_GPOOL_NON_REGULAR_CHUNCK_INDEX);
	gpool->chunks[i].data = gpool->chunks[i - 1].data + gpool->chunks[i - 1].size;
	gpool->chunks[i].start_block = 0;
	gpool->chunks[i].block_n = 0;
	gpool->chunks[i].size = (tb_size_t)(gpool->blocks_info - gpool->chunks[i].data);
	tb_assert(!(gpool->chunks[i].size & (TB_GPOOL_ALIGN_BOUNDARY - 1)));
	tb_assert(gpool->chunks[i].size >= TB_GPOOL_NON_REGULAR_CHUNCK_MIN_SIZE);

	// check non-regular chunk size & data address
	tb_assert(!(gpool->chunks[i].size & (TB_GPOOL_ALIGN_BOUNDARY - 1)));
	tb_assert_message((tb_size_t)gpool->blocks_info == (tb_size_t)gpool->chunks[i].data + gpool->chunks[i].size, "the size of non-regular chunk is too small");

	((tb_gpool_nrblock_head_t*)gpool->chunks[i].data)->block_size = gpool->chunks[i].size - sizeof(tb_gpool_nrblock_head_t); // block size == the whole chunk size - the head size
	((tb_gpool_nrblock_head_t*)gpool->chunks[i].data)->is_free = 1; // is free? = 1

#ifdef TB_DEBUG
	// init gpool status info
	tb_assert(gpool->data + gpool->size >= gpool->blocks_info);
	gpool->status.total_size = gpool->size;
	gpool->status.used_size = (tb_size_t)(gpool->data + gpool->size - gpool->blocks_info);
	gpool->status.peak_size = gpool->status.used_size;
	gpool->status.need_used_size = 0;
	gpool->status.real_used_size = gpool->status.used_size;
	gpool->status.alloc_failed = 0;
	gpool->status.nrblock_max_size = 0;
	tb_memset(gpool->status.rblock_max_n, 0, TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT);
#endif
	return ((tb_handle_t)gpool);
}
tb_void_t tb_gpool_exit(tb_handle_t hpool)
{
	// check 
	tb_gpool_t* gpool = (tb_gpool_t*)hpool;
	tb_assert_and_check_return(gpool && gpool->magic == TB_GPOOL_MAGIC);

	// clear
	if (gpool->data) tb_memset(gpool->data, 0, gpool->size);
	tb_memset(gpool, 0, sizeof(tb_gpool_t));
}


#ifndef TB_DEBUG
tb_pointer_t tb_gpool_allocate(tb_handle_t hpool, tb_size_t size)
#else
tb_pointer_t tb_gpool_allocate(tb_handle_t hpool, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_gpool_t* gpool = (tb_gpool_t*)hpool;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC, TB_NULL);
	tb_check_return_val(size, TB_NULL);

#ifdef TB_DEBUG
	gpool->status.alloc_total++;
#endif

	// allocate it from regular-chunks if block is small
	if (size <= TB_GPOOL_REGULAR_BLOCK_MAX_SIZE)
	{
		// find the chunk index of the first free regular-block
		tb_size_t chunk_i = tb_block_size_to_chunk_i(gpool->chunks, size);

		// if exists free block
		if (chunk_i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT)
		{
		#ifdef TB_GPOOL_PRED_ENABLE
			// try allocating it from predicted rblock
			tb_byte_t* p = TB_GPOOL_TRY_ALLOCATING_FROM_PRED_RBLOCK(gpool, size, chunk_i, func, line, file);
			if (p) return p;

		# 	ifdef TB_DEBUG
			if (!p) gpool->status.pred_failed++;
		# 	endif

		#endif

			//allocate a free block
			tb_size_t i = 0;
			tb_size_t block_n = gpool->chunks[chunk_i].block_n;
			for (i = 0; i < block_n; ++i)
			{
				tb_size_t block_i = gpool->chunks[chunk_i].start_block + i;
				if (!tb_gpool_blocks_info_isset(gpool->blocks_info, block_i)) // is free?
				{
					// set used flag: the block is non-free
					tb_gpool_blocks_info_set(gpool->blocks_info, block_i);
					// update the count of free blocks in the chunk
					--gpool->chunks[chunk_i].free_block_n;

					// predict next, pred_n must be null, otherwise exists repeat item
				#ifdef TB_GPOOL_PRED_ENABLE
					if (!gpool->pred_rblocks_n[chunk_i])
					{
						if ((i + 1 < block_n) && !tb_gpool_blocks_info_isset(gpool->blocks_info, block_i)) 
						{
							gpool->pred_rblocks[chunk_i][0] = block_i + 1;
							gpool->pred_rblocks_n[chunk_i] = 1;
						}
					}
				#endif

				#ifdef TB_DEBUG
					// update gpool status info
					gpool->status.used_size += gpool->chunks[chunk_i].block_size;
					if (gpool->status.used_size > gpool->status.peak_size) gpool->status.peak_size = gpool->status.used_size;
					gpool->status.need_used_size += size;
					gpool->status.real_used_size += gpool->chunks[chunk_i].block_size;
					{
						tb_size_t block_n = gpool->chunks[chunk_i].block_n - gpool->chunks[chunk_i].free_block_n;
						gpool->status.rblock_max_n[chunk_i] = block_n > gpool->status.rblock_max_n[chunk_i]? block_n : gpool->status.rblock_max_n[chunk_i];
					}
				#endif

					// return data address
					return (gpool->chunks[chunk_i].data + i * gpool->chunks[chunk_i].block_size);
				}
			}
		}
	}

	// align size
	tb_byte_t* 	p = TB_NULL;
	tb_size_t 	asize = tb_align(size, TB_GPOOL_ALIGN_BOUNDARY);

	// try allocating it from predicted nrblock
#ifdef TB_GPOOL_PRED_ENABLE
	p = TB_GPOOL_TRY_ALLOCATING_FROM_PRED_NRBLOCK(gpool, size, asize, func, line, file);
	if (p) return p;

# 	ifdef TB_DEBUG
	if (!p) gpool->status.pred_failed++;
# 	endif

#endif

	// allocate it from non-regular chunk
	tb_byte_t* 	pb = gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].data;
	tb_byte_t* 	pe = pb + gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].size;

	// find free block
	for (p = pb; p + sizeof(tb_gpool_nrblock_head_t) < pe;)
	{
		// get the head of current block
		tb_gpool_nrblock_head_t* 	phead = ((tb_gpool_nrblock_head_t*)p);
		tb_size_t 					osize = ((tb_gpool_nrblock_head_t*)p)->block_size;

		if (phead->is_free) // allocate if the block is free
		{
			// is enough?
			if (osize >= asize)
			{
				// skip head
				p += sizeof(tb_gpool_nrblock_head_t);

				// update head of the next free block if the free block is splited
				if (osize > asize + sizeof(tb_gpool_nrblock_head_t))
				{
					// split block
					tb_gpool_nrblock_head_t* pnext_head = (tb_gpool_nrblock_head_t*)(p + asize);
					pnext_head->block_size = osize - asize - sizeof(tb_gpool_nrblock_head_t);
					pnext_head->is_free = 1;

					// update the head of block
					phead->block_size = asize;	// set block size
					phead->is_free = 0; 		// now it is non-free
				}
				// use the whole block if the free block is not splited
				else 
				{
					// update the head of block
					phead->block_size = osize; 	// use the whole block
					phead->is_free = 0; 		// now it is non-free
				}

				// check address
				tb_assert(p + phead->block_size <= pe);
				tb_assert(!(((tb_size_t)p) & (TB_GPOOL_ALIGN_BOUNDARY - 1)));

				// predict the next free block
			#ifdef TB_GPOOL_PRED_ENABLE
				gpool->pred_nrblock = p + phead->block_size;
			#endif

			#ifdef TB_DEBUG
				// set bounds magic
				phead->magic_b = TB_GPOOL_MAGIC;
				phead->magic_e = TB_GPOOL_MAGIC;

				// save the trace info
				phead->code_line = line;
				phead->code_file = file;
				phead->code_func = func;

				// update gpool status info
				gpool->status.used_size += phead->block_size;
				if (gpool->status.used_size > gpool->status.peak_size) gpool->status.peak_size = gpool->status.used_size;
				gpool->status.need_used_size += size;
				gpool->status.real_used_size += phead->block_size;
				gpool->status.nrblock_max_size = phead->block_size > gpool->status.nrblock_max_size? phead->block_size : gpool->status.nrblock_max_size;
			#endif

				// return data address
				return p;
			}
			else // attempt to merge next free block if current free block is too small
			{
				tb_gpool_nrblock_head_t* pnext_head = (tb_gpool_nrblock_head_t*)(p + sizeof(tb_gpool_nrblock_head_t) + osize);
			
				// break if doesn't exist next block
				if ((tb_size_t)pnext_head + sizeof(tb_gpool_nrblock_head_t) >= (tb_size_t)gpool->blocks_info) break;

				// next block is free?
				if (pnext_head->is_free) 
				{
					// merge next block
					phead->block_size += pnext_head->block_size + sizeof(tb_gpool_nrblock_head_t);

					// reset the next predicted block
				#ifdef TB_GPOOL_PRED_ENABLE
					if (gpool->pred_nrblock == (tb_byte_t*)pnext_head)
						gpool->pred_nrblock = TB_NULL;
				#endif

					// continue handle this block
					continue ;
				}
			}
		}

		// skip it if the block is non-free or block_size is too small
		p += sizeof(tb_gpool_nrblock_head_t) + osize;
	}	
	
#ifdef TB_DEBUG
	// update the failed count
	gpool->status.alloc_failed++;
#endif
	
	return TB_NULL;
}
#ifndef TB_DEBUG
tb_bool_t tb_gpool_deallocate(tb_handle_t hpool, tb_pointer_t data)
#else
tb_bool_t tb_gpool_deallocate(tb_handle_t hpool, tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_gpool_t* gpool = (tb_gpool_t*)hpool;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC, TB_FALSE);
	tb_check_return_val(data, TB_TRUE);

	// get data address
	// |--------------------------------------------|-----------------|
	// addr_begin           addr             non-regular addr       end_addr
	//
	tb_size_t addr = (tb_size_t)data;
	tb_size_t addr_begin = (tb_size_t)gpool->data;
	tb_size_t addr_end = (tb_size_t)gpool->blocks_info;

	// check data address
	if (addr < addr_begin) return TB_FALSE;
	//tb_assert_message(addr >= addr_begin && addr < addr_end, "invalid data address:%x", addr);

	// deallocate it to regular chunks if the block is regular
	if (addr < gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].data)
	{
		// find chunk index
		tb_size_t chunk_i = 0;
		for (chunk_i = 0; chunk_i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT; ++chunk_i)
			if (addr < gpool->chunks[chunk_i].data + gpool->chunks[chunk_i].size) break;
		tb_assert(chunk_i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT);

		// compute block index
		tb_assert(addr >= (tb_size_t)gpool->chunks[chunk_i].data);
		tb_size_t offset = addr - (tb_size_t)gpool->chunks[chunk_i].data;
		tb_size_t block_i = gpool->chunks[chunk_i].start_block + offset / gpool->chunks[chunk_i].block_size;

		// check data address
		tb_assert(!(offset % gpool->chunks[chunk_i].block_size));

		// reset used flag: the block is free
		tb_gpool_blocks_info_reset(gpool->blocks_info, block_i);

		// predict the next free block
	#ifdef TB_GPOOL_PRED_ENABLE
		if (gpool->pred_rblocks_n[chunk_i] < TB_GPOOL_PRED_RBLOCKS_MAX)
			gpool->pred_rblocks[chunk_i][gpool->pred_rblocks_n[chunk_i]++] = block_i;
	#endif

		// update the count of free blocks in the chunk
		++gpool->chunks[chunk_i].free_block_n;

	#ifdef TB_DEBUG
		// update gpool status info
		gpool->status.used_size -= gpool->chunks[chunk_i].block_size;
		if (gpool->status.used_size > gpool->status.peak_size) gpool->status.peak_size = gpool->status.used_size;
	#endif
		return TB_TRUE;
	}
	// deallocate it to non-regular chunk if the block is non-regular
	else if (addr < addr_end)
	{
		tb_byte_t* p = (tb_byte_t*)addr;

		// get the head of the block
		tb_gpool_nrblock_head_t* phead = ((tb_gpool_nrblock_head_t*)(p - sizeof(tb_gpool_nrblock_head_t)));

		// check this block
		tb_assert_message(!phead->is_free, "double free data:%x at %s(): %d, file: %s", addr, func? func : "null", line, file? file : "null");
		tb_assert_message(phead->block_size < gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].size, "invalid data address at free():%x", addr);

	#ifdef TB_DEBUG
		// update gpool status info
		gpool->status.used_size -= phead->block_size;
		if (gpool->status.used_size > gpool->status.peak_size) gpool->status.peak_size = gpool->status.used_size;
	#endif

		// merge it if next block is free
		tb_gpool_nrblock_head_t* pnext_head = (tb_gpool_nrblock_head_t*)(p + phead->block_size);
		// block size : current block size + next block head + next block size
		if (pnext_head->is_free) phead->block_size += sizeof(tb_gpool_nrblock_head_t) + pnext_head->block_size;

		phead->is_free = 1;

		// predict the next free block
	#ifdef TB_GPOOL_PRED_ENABLE
		gpool->pred_nrblock = (tb_byte_t*)phead;
	#endif

		return TB_TRUE;
	}

	return TB_FALSE;
}

#ifndef TB_DEBUG
tb_pointer_t tb_gpool_callocate(tb_handle_t hpool, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_gpool_callocate(tb_handle_t hpool, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	tb_check_return_val(item && size, TB_NULL);
	tb_pointer_t p = TB_GPOOL_ALLOCATE(hpool, item * size, func, line, file);
	if (p) tb_memset(p, 0, item * size);
	return p;
}

#ifndef TB_DEBUG
tb_pointer_t tb_gpool_reallocate(tb_handle_t hpool, tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_gpool_reallocate(tb_handle_t hpool, tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check
	tb_gpool_t* gpool = (tb_gpool_t*)hpool;
	tb_assert_and_check_return_val(gpool && gpool->magic == TB_GPOOL_MAGIC, TB_NULL);

	// alloc it if is null?
	if (!data) return TB_GPOOL_ALLOCATE(hpool, size, func, line, file);

	/* get data address
	 * |--------------------------------------------|-----------------|
	 * addr_begin           addr             non-regular addr       end_addr
	 *
	 */
	tb_size_t addr = (tb_size_t)data;
	tb_size_t addr_begin = (tb_size_t)gpool->data;
	tb_size_t addr_end = (tb_size_t)gpool->blocks_info;

	// check data address
	if (addr < addr_begin) return TB_NULL;
	//tb_assert_message(addr >= addr_begin && addr < addr_end, "invalid data address:%x", addr);

	// reallocate it to regular chunks if the block is regular
	if (addr < (tb_size_t)gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].data)
	{
		// find chunk index
		tb_size_t chunk_i = 0;
		for (chunk_i = 0; chunk_i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT; ++chunk_i)
			if (addr < gpool->chunks[chunk_i].data + gpool->chunks[chunk_i].size) break;
		tb_assert(chunk_i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT);

		// if enough?
		if (size <= gpool->chunks[chunk_i].block_size) return data;
		else // reallocate
		{
			// compute block index
			tb_size_t offset = addr - (tb_size_t)gpool->chunks[chunk_i].data;
			tb_size_t block_i = gpool->chunks[chunk_i].start_block + offset / gpool->chunks[chunk_i].block_size;

			// check data address
			tb_assert(!(offset % gpool->chunks[chunk_i].block_size));

			// allocate new buffer
			tb_byte_t* p = TB_GPOOL_ALLOCATE(gpool, size, func, line, file);
			tb_memcpy(p, data, gpool->chunks[chunk_i].block_size);

			// reset used flag: the block is free
			tb_gpool_blocks_info_reset(gpool->blocks_info, block_i);
			// update the count of free blocks in the chunk
			++gpool->chunks[chunk_i].free_block_n;

			// predict the next free block
		#ifdef TB_GPOOL_PRED_ENABLE
			if (gpool->pred_rblocks_n[chunk_i] < TB_GPOOL_PRED_RBLOCKS_MAX)
				gpool->pred_rblocks[chunk_i][gpool->pred_rblocks_n[chunk_i]++] = block_i;
		#endif

		#ifdef TB_DEBUG
			// update gpool status info
			gpool->status.used_size -= gpool->chunks[chunk_i].block_size;
			if (gpool->status.used_size > gpool->status.peak_size) gpool->status.peak_size = gpool->status.used_size;
		#endif

			return p;
		}
	}
	// reallocate it to non-regular chunk if the block is non-regular
	else if (addr < addr_end)
	{
		// get the head of the block
		tb_byte_t* 			p = (tb_byte_t*)addr;
		tb_gpool_nrblock_head_t* 	phead = ((tb_gpool_nrblock_head_t*)(p - sizeof(tb_gpool_nrblock_head_t)));
		tb_size_t 			osize = phead->block_size;

		// check this block
		tb_assert_message(!phead->is_free, "the buffer has been free:%x", addr);
		tb_assert_message(phead->block_size < gpool->chunks[TB_GPOOL_NON_REGULAR_CHUNCK_INDEX].size, "invalid data address at realloc():%x", addr);

		// if enough?
		if (size <= osize) return data;
		else // reallocate
		{
			// merge it if next block is free
			tb_gpool_nrblock_head_t* pnext_head = (tb_gpool_nrblock_head_t*)(p + phead->block_size);
			if (pnext_head->is_free)
			{
				// align size
				tb_size_t asize = tb_align(size, TB_GPOOL_ALIGN_BOUNDARY);

				// block size : current block size + next block head + next block size
				phead->block_size += sizeof(tb_gpool_nrblock_head_t) + pnext_head->block_size;

				// reset the next predicted block
			#ifdef TB_GPOOL_PRED_ENABLE
				if (gpool->pred_nrblock == (tb_byte_t*)pnext_head)
					gpool->pred_nrblock = TB_NULL;
			#endif

				// if enough?
				if (asize <= phead->block_size) 
				{
					// update head of the next free block if the free block is splited
					if (phead->block_size > asize + sizeof(tb_gpool_nrblock_head_t))
					{
						// split block
						pnext_head = (tb_gpool_nrblock_head_t*)(p + asize);
						pnext_head->block_size = phead->block_size - asize - sizeof(tb_gpool_nrblock_head_t);
						pnext_head->is_free = 1;

						// update the head of block
						phead->block_size = asize;	// set block size
						phead->is_free = 0; 				// now it is non-free
					}
					// use the whole block if the free block is not splited
					else ; 

					// predict the next free block
				#ifdef TB_GPOOL_PRED_ENABLE
					gpool->pred_nrblock = p + phead->block_size;
				#endif

				#ifdef TB_DEBUG
					// set bounds magic
					phead->magic_b = TB_GPOOL_MAGIC;
					phead->magic_e = TB_GPOOL_MAGIC;

					// save the trace info
					phead->code_line = line;
					phead->code_file = file;
					phead->code_func = func;

					// update gpool status info
					gpool->status.used_size += phead->block_size - osize;
					if (gpool->status.used_size > gpool->status.peak_size) gpool->status.peak_size = gpool->status.used_size;
				#endif
					return p;
				}
			}

			// allocate new buffer
			p = TB_GPOOL_ALLOCATE(gpool, size, func, line, file);
			if (p) tb_memcpy(p, data, osize);
			else return TB_NULL;

			// free it to allocate a new block if not enough
			phead->is_free = 1;

			// predict the next free block
		#ifdef TB_GPOOL_PRED_ENABLE
			gpool->pred_nrblock = (tb_byte_t*)phead;
		#endif

		#ifdef TB_DEBUG
			// update gpool status info
			gpool->status.used_size -= osize;
			if (gpool->status.used_size > gpool->status.peak_size) gpool->status.peak_size = gpool->status.used_size;
		#endif
			return p;
		}
	}
	return TB_NULL;
}


#ifdef TB_DEBUG
tb_void_t tb_gpool_dump(tb_handle_t hpool)
{
	tb_size_t i = 0, j = 0;
	tb_byte_t* p = TB_NULL;
	tb_byte_t* pe = TB_NULL;

	tb_gpool_t* gpool = (tb_gpool_t*)hpool;
	tb_assert_and_check_return(gpool);

	tb_trace("=============================================");
	tb_trace("memory gpool info:");
	tb_trace("gpool data addr:%x", (tb_size_t)gpool->data);
	tb_trace("gpool data size:%u", gpool->size);
	tb_trace("gpool blocks_n:%u\n", gpool->blocks_n);
	tb_trace("regular chunks info:");
	for (i = 0; i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT; ++i)
	{
		tb_trace("chunk(%d):", gpool->chunks[i].block_size);
		tb_trace("\tdata:%x size:%u", (tb_size_t)gpool->chunks[i].data, gpool->chunks[i].size);
		tb_trace("\tstart block:%d block_n:%u free_block_n:%u", gpool->chunks[i].start_block, gpool->chunks[i].block_n, gpool->chunks[i].free_block_n);

		// dump allocated blocks info
		{
			tb_char_t str[4 * 8 + 4];
			p = (tb_byte_t*)str;
			for (j = 0; j < gpool->chunks[i].block_n; ++j)
			{
				if (!(j & 31)) 
				{
					*p = '\0';
					tb_trace("\t%s", str);
					p = (tb_byte_t*)str;
				}
				if ((j & 31) && !(j & 0x7)) *p++ = ' ';
				if (tb_gpool_blocks_info_isset(gpool->blocks_info, gpool->chunks[i].start_block + j)) *p = '1';
				else *p = '0';
				++p;
			}
			*p = '\0';
			tb_trace("\t%s", str);
		}
	}
	tb_trace("non-regular chunks info:");
	tb_trace("\tdata:%x size:%u", (tb_size_t)gpool->chunks[i].data, gpool->chunks[i].size);
	p = gpool->chunks[i].data;
	pe = p + gpool->chunks[i].size;
	i = 0;
	while (p + sizeof(tb_gpool_nrblock_head_t) < pe)
	{
		tb_gpool_nrblock_head_t* phead = (tb_gpool_nrblock_head_t*)p;

		if (!phead->is_free)
		{
			tb_trace("\tblock(%d) data:%x size:%u head_size:%d is_free:%s, b: 0x%x, e: 0x%x, at %s(): %d, file: %s"
				, i++
				, (tb_size_t)(p + sizeof(tb_gpool_nrblock_head_t))
				, phead->block_size
				, sizeof(tb_gpool_nrblock_head_t)
				, "false"
				, phead->magic_b
				, phead->magic_e
				, phead->code_func
				, phead->code_line
				, phead->code_file
				);
		}
		else
		{
			tb_trace("\tblock(%d) data:%x size:%u head_size:%d is_free:%s"
				, i++
				, (tb_size_t)(p + sizeof(tb_gpool_nrblock_head_t))
				, phead->block_size
				, sizeof(tb_gpool_nrblock_head_t)
				, "true"
				);
		}
		p += sizeof(tb_gpool_nrblock_head_t) + phead->block_size;
	}

	tb_trace("blocks info:");
	tb_trace("\tdata:%x size:%u", (tb_size_t)gpool->blocks_info, (tb_size_t)(gpool->data + gpool->size - gpool->blocks_info));

	tb_trace("=============================================");
	tb_trace("gpool status info:");
	tb_trace("=============================================");
	tb_trace("total:%u", gpool->status.total_size);
	tb_trace("used:%u", gpool->status.used_size);
	tb_trace("peak:%u", gpool->status.peak_size);
	tb_trace("wast:%u%%", gpool->status.real_used_size? (gpool->status.real_used_size - gpool->status.need_used_size) * 100 / gpool->status.real_used_size : 0);
	tb_trace("fail:%u", gpool->status.alloc_failed);
	tb_trace("pred:%02d%%, fail: %d, total: %d", gpool->status.alloc_total? ((gpool->status.alloc_total - gpool->status.pred_failed) * 100 / gpool->status.alloc_total) : -1, gpool->status.pred_failed, gpool->status.alloc_total);

	for (i = 0; i < TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT; ++i)
		tb_trace("regular chunk:%d total_block_n:%u block_max_n:%u used rate:%u%% recommend_block_n:%u", gpool->chunks[i].block_size, gpool->chunks[i].block_n, gpool->status.rblock_max_n[i], gpool->chunks[i].block_n? gpool->status.rblock_max_n[i] * 100 / gpool->chunks[i].block_n : 0, 2 + gpool->status.rblock_max_n[i] * 100 / 80);
	tb_trace("nrblock_max_size:%u", gpool->status.nrblock_max_size);

	if (!tb_gpool_check(hpool))
		tb_trace("the gpool maybe exists error!");
}
tb_bool_t tb_gpool_check(tb_handle_t hpool)
{	
	tb_gpool_t* gpool = (tb_gpool_t*)hpool;
	tb_assert_and_check_return_val(gpool, TB_FALSE);

	// check magic
	if (gpool->magic != TB_GPOOL_MAGIC) return TB_FALSE;

	// check chunk
	if (!gpool->chunks) return TB_FALSE;

	//check non-regular chunks
	tb_byte_t const* p = gpool->chunks[TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT].data;
	tb_byte_t const* pe = p + gpool->chunks[TB_GPOOL_REGULAR_CHUNCK_MAX_COUNT].size;
	if (!p || !pe || p >= pe) return TB_FALSE;
	while (p + sizeof(tb_gpool_nrblock_head_t) < pe)
	{
		tb_gpool_nrblock_head_t* phead = (tb_gpool_nrblock_head_t*)p;
		if (!phead->is_free)
		{
			if (!phead->block_size
				|| phead->magic_b != TB_GPOOL_MAGIC
				|| phead->magic_e != TB_GPOOL_MAGIC
				) 
			{
				return TB_FALSE;
			}
		}
		p += sizeof(tb_gpool_nrblock_head_t) + phead->block_size;
	}

	// is enough?
	if (!tb_gpool_allocate_try(gpool, 10)) return TB_FALSE;

	return TB_TRUE;
}

#endif
