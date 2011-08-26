/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		mpool.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "mpool.h"
#include "memops.h"

/*!structure
 *
 * mpool->data: |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 *                                                                                                mpool->size
 *
 *                   						regular blocks        		                                   non-regular blocks                                                  for regular blocks
 * mpool->data: |----------------|----------------|---------------|-------   ...  --------|-----------------------------------------------------------------|--------------------------------------------------|
 *                  chunks[0]        chunks[1]        chunks[2]              ...              chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX] : large mpool chunk                blocks_info: size == align(blocks_n, 8) / 8
 *
 * non-regular block size:
 * chunks[0]: <= mpool->size - (chunks[0].size + chunks[2].size + ... + chunks[TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT - 1].size) - align(blocks_n, 8) / 8
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
# 	define TB_MPOOL_MAGIC 							(0xdeadbeef)
#elif TB_CPU_BITSIZE == 64
# 	define TB_MPOOL_MAGIC 							(0xdeadbeefdeadbeef)
#else
# 	error the bitsize of cpu is not supported.
#endif

// align boundary
#define TB_MPOOL_ALIGN_BOUNDARY 					(TB_CPU_BITBYTE)

// praticle size
#define TB_MPOOL_PRATICLE_SIZE 						(16)

// the regular chunk maximum count
#define TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT 	 		(7)
// regular block maximum size
#define TB_MPOOL_REGULAR_BLOCK_MAX_SIZE 			(TB_MPOOL_PRATICLE_SIZE << (TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT - 1))

// the non-regular chunk minimum size
#define TB_MPOOL_NON_REGULAR_CHUNCK_MIN_SIZE 		(128)
// non-regular chunk index
#define TB_MPOOL_NON_REGULAR_CHUNCK_INDEX 			(TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT)

// alloc & free without lock
#ifndef TB_DEBUG
# 	define TB_MPOOL_ALLOCATE_NO_LOCK(mpool, size, func, line, file) 							tb_mpool_allocate_no_lock(mpool, size)
# 	define TB_MPOOL_REALLOCATE_NO_LOCK(mpool, data, size, func, line, file) 					tb_mpool_reallocate_no_lock(mpool, data, size)
# 	define TB_MPOOL_CALLOCATE_NO_LOCK(mpool, item, size, func, line, file) 						tb_mpool_callocate_no_lock(mpool, item, size)
# 	define TB_MPOOL_DEALLOCATE_NO_LOCK(mpool, data, func, line, file) 							tb_mpool_deallocate_no_lock(mpool, data)
# 	define TB_MPOOL_TRY_ALLOCATING_FROM_PRED_RBLOCK(mpool, size, chunk_i, func, line, file) 	tb_mpool_try_allocating_from_pred_rblock(mpool, size, chunk_i)
# 	define TB_MPOOL_TRY_ALLOCATING_FROM_PRED_NRBLOCK(mpool, size, asize, func, line, file) 		tb_mpool_try_allocating_from_pred_nrblock(mpool, size, asize)
#else
# 	define TB_MPOOL_ALLOCATE_NO_LOCK(mpool, size, func, line, file) 							tb_mpool_allocate_no_lock(mpool, size, func, line, file)
# 	define TB_MPOOL_REALLOCATE_NO_LOCK(mpool, data, size, func, line, file) 					tb_mpool_reallocate_no_lock(mpool, data, size, func, line, file)
# 	define TB_MPOOL_CALLOCATE_NO_LOCK(mpool, item, size, func, line, file) 						tb_mpool_callocate_no_lock(mpool, item, size, func, line, file)
# 	define TB_MPOOL_DEALLOCATE_NO_LOCK(mpool, data, func, line, file) 							tb_mpool_deallocate_no_lock(mpool, data, func, line, file)
# 	define TB_MPOOL_TRY_ALLOCATING_FROM_PRED_RBLOCK(mpool, size, chunk_i, func, line, file) 	tb_mpool_try_allocating_from_pred_rblock(mpool, size, chunk_i, func, line, file)
# 	define TB_MPOOL_TRY_ALLOCATING_FROM_PRED_NRBLOCK(mpool, size, asize, func, line, file) 		tb_mpool_try_allocating_from_pred_nrblock(mpool, size, asize, func, line, file)
#endif

// prediction
#define TB_MPOOL_PRED_ENABLE
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_MEMORY_MPOOL_PRED_RBLOCKS_MAX 		(128)
#else
# 	define TB_MEMORY_MPOOL_PRED_RBLOCKS_MAX 		(256)
#endif

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

// the non-regular block head type
typedef struct __tb_mpool_nrblock_head_t
{
	// the bounds magic
#ifdef TB_DEBUG
	tb_size_t 			magic_b;
#endif

	// the block info
	tb_size_t 			block_size;
	tb_size_t 			is_free;
	tb_size_t 			data;

#ifdef TB_DEBUG

	// the bounds magic
	tb_size_t 			magic_e;

	// the trace info
	tb_char_t const* 	code_file;
	tb_char_t const* 	code_func;
	tb_size_t 			code_line;

#endif

}tb_mpool_nrblock_head_t;

// the chunk type
typedef struct __tb_mpool_chunk_t
{
	tb_byte_t* 		data;
	tb_size_t 		size;

	// \note only used for regular chunk
	tb_size_t 		start_block;
	tb_size_t 		block_n;
	tb_size_t 		block_size;
	tb_size_t 		free_block_n;

}tb_mpool_chunk_t;

// the mpool status type
typedef struct __tb_mpool_status_t
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
	tb_size_t 			rblock_max_n[TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT];

	// non-regular chunk info
	tb_size_t 			nrblock_max_size;

}tb_mpool_status_t;

// the mpool type
typedef struct __tb_mpool_t
{
	// the magic number
	tb_size_t 			magic;

	// the total mpool data
	tb_byte_t* 			data;
	tb_size_t 			size;

	// the info of used regular blocks
	// 0 1 0 1 1 1 ...
	// 1: the block is non-free
	// 0: the block is free
	tb_byte_t* 			blocks_info;
	tb_size_t 			blocks_n;

#ifdef TB_DEBUG
	// the mpool info 
	tb_mpool_status_t 	status;
#endif

	// the mutex
	tb_handle_t 		hmutex; 

	// the mpool chunks
	// some regular chunks + one non-regular chunk
	tb_mpool_chunk_t 	chunks[TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT + 1];

	// the predicted block
#ifdef TB_MPOOL_PRED_ENABLE
	tb_size_t 			pred_rblocks[TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT][TB_MEMORY_MPOOL_PRED_RBLOCKS_MAX];
	tb_size_t 			pred_rblocks_n[TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT];
	tb_byte_t* 			pred_nrblock;
#endif

}tb_mpool_t;

/* ////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_mpool_t* 		g_mpool = TB_NULL;

/* ////////////////////////////////////////////////////////////////////////
 * the inner implemention
 */

#ifdef TB_DEBUG
static __tb_inline__ tb_void_t tb_mpool_blocks_info_set(tb_byte_t* blocks_info, tb_size_t block_i)
{
	TB_ASSERT(blocks_info);
	blocks_info[block_i >> 3] |= (0x1 << (block_i % 8));
}
static __tb_inline__ tb_void_t tb_mpool_blocks_info_reset(tb_byte_t* blocks_info, tb_size_t block_i)
{
	TB_ASSERT(blocks_info);
	blocks_info[block_i >> 3] &= ~(0x1 << (block_i % 8));
}
static __tb_inline__ tb_size_t tb_mpool_blocks_info_isset(tb_byte_t* blocks_info, tb_size_t block_i)
{
	TB_ASSERT(blocks_info);
	return (blocks_info[block_i >> 3] & (0x1 << (block_i % 8)));
}
#else
# 	define tb_mpool_blocks_info_set(info, i) 		do {(info)[(i) >> 3] |= (0x1 << ((i) % 8));} while (0)
# 	define tb_mpool_blocks_info_reset(info, i) 	do {(info)[(i) >> 3] &= ~(0x1 << ((i) % 8));} while (0)
# 	define tb_mpool_blocks_info_isset(info, i) 	((info)[(i) >> 3] & (0x1 << ((i) % 8)))
#endif

static __tb_inline__ tb_size_t tb_block_size_to_chunk_i(tb_mpool_chunk_t* chunks, tb_size_t block_size)
{
	tb_size_t i = 0;
	// find the chunk index of the first free regular-block
	for (i = 0; i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT; ++i) 
		if (chunks[i].free_block_n && block_size <= chunks[i].block_size) return i;

	// regular chunks is full 
	return i;
}
/* ////////////////////////////////////////////////////////////////////////
 * the interface implemention
 */
tb_bool_t tb_mpool_init(tb_void_t* data, tb_size_t size)
{
	// check
	TB_ASSERT_RETURN_VAL(!g_mpool && data && size, TB_FALSE);
	TB_STATIC_ASSERT(!(sizeof(tb_mpool_nrblock_head_t) % TB_MPOOL_ALIGN_BOUNDARY));
	
	// init regular block 
	tb_size_t rblockn[TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT] = {0, 0, 0, 0, 0, 0, 0};
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

	// init mpool
	g_mpool = data;
	g_mpool->magic = TB_MPOOL_MAGIC;

	// create mutex
	g_mpool->hmutex = tb_mutex_create("the memory pool");
	if (!g_mpool->hmutex) return TB_FALSE;

	// attach data
	g_mpool->data = &g_mpool[1];
	g_mpool->data = (tb_byte_t*)tb_align((tb_size_t)g_mpool->data, TB_MPOOL_ALIGN_BOUNDARY);

	TB_ASSERT((tb_size_t)data + size > (tb_size_t)g_mpool->data);
	g_mpool->size = (tb_size_t)((tb_size_t)data + size - (tb_size_t)g_mpool->data);

	// init regular chunks
	tb_size_t i = 0;
	g_mpool->blocks_n = 0;
	for (i = 0; i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT; ++i)
	{
		// init regular chunk
		if (i == 0) g_mpool->chunks[i].data = g_mpool->data;
		else g_mpool->chunks[i].data = g_mpool->chunks[i - 1].data + g_mpool->chunks[i - 1].size;

		g_mpool->chunks[i].block_n = rblockn[i];
		g_mpool->chunks[i].free_block_n = g_mpool->chunks[i].block_n;
		g_mpool->chunks[i].block_size = TB_MPOOL_PRATICLE_SIZE << i;
		g_mpool->chunks[i].size = g_mpool->chunks[i].block_size * g_mpool->chunks[i].block_n;

		// stats total blocks count
		g_mpool->chunks[i].start_block = g_mpool->blocks_n;
		g_mpool->blocks_n += g_mpool->chunks[i].block_n;

		// init predicted info
#ifdef TB_MPOOL_PRED_ENABLE
		tb_size_t m = rblockn[i] < TB_MEMORY_MPOOL_PRED_RBLOCKS_MAX? rblockn[i] : TB_MEMORY_MPOOL_PRED_RBLOCKS_MAX;
		tb_size_t n = m;
		while (n--) g_mpool->pred_rblocks[i][n] = g_mpool->chunks[i].start_block + m - n - 1;
		g_mpool->pred_rblocks_n[i] = m;
#endif
	}

	// init blocks info
	g_mpool->blocks_info = g_mpool->data + g_mpool->size - (tb_align(g_mpool->blocks_n, 8) >> 3);
	g_mpool->blocks_info -= ((tb_size_t)g_mpool->blocks_info) % TB_MPOOL_ALIGN_BOUNDARY;
	TB_ASSERT((tb_size_t)g_mpool->blocks_info > (tb_size_t)g_mpool->data);
  
	// init non-regular chunk
	TB_ASSERT(i >= 1 && i == TB_MPOOL_NON_REGULAR_CHUNCK_INDEX);
	g_mpool->chunks[i].data = g_mpool->chunks[i - 1].data + g_mpool->chunks[i - 1].size;
	g_mpool->chunks[i].start_block = 0;
	g_mpool->chunks[i].block_n = 0;
	g_mpool->chunks[i].size = (tb_size_t)(g_mpool->blocks_info - g_mpool->chunks[i].data);
	TB_ASSERT(!(g_mpool->chunks[i].size % TB_MPOOL_ALIGN_BOUNDARY));
	TB_ASSERT(g_mpool->chunks[i].size >= TB_MPOOL_NON_REGULAR_CHUNCK_MIN_SIZE);

	// check non-regular chunk size & data address
	TB_ASSERT(!(g_mpool->chunks[i].size % TB_MPOOL_ALIGN_BOUNDARY));
	TB_ASSERTM((tb_size_t)g_mpool->blocks_info == (tb_size_t)g_mpool->chunks[i].data + g_mpool->chunks[i].size, "the size of non-regular chunk is too small");

	((tb_mpool_nrblock_head_t*)g_mpool->chunks[i].data)->block_size = g_mpool->chunks[i].size - sizeof(tb_mpool_nrblock_head_t); // block size == the whole chunk size - the head size
	((tb_mpool_nrblock_head_t*)g_mpool->chunks[i].data)->is_free = 1; // is free? = 1

#ifdef TB_DEBUG
	// init mpool status info
	TB_ASSERT(g_mpool->data + g_mpool->size >= g_mpool->blocks_info);
	g_mpool->status.total_size = g_mpool->size;
	g_mpool->status.used_size = (tb_size_t)(g_mpool->data + g_mpool->size - g_mpool->blocks_info);
	g_mpool->status.peak_size = g_mpool->status.used_size;
	g_mpool->status.need_used_size = 0;
	g_mpool->status.real_used_size = g_mpool->status.used_size;
	g_mpool->status.alloc_failed = 0;
	g_mpool->status.nrblock_max_size = 0;
	tb_memset(g_mpool->status.rblock_max_n, 0, TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT);
#endif
	return TB_TRUE;
}
tb_void_t tb_mpool_exit()
{
	// check 
	TB_ASSERT_RETURN(g_mpool && g_mpool->magic == TB_MPOOL_MAGIC);

	// destroy mutex
	if (g_mpool->hmutex) tb_mutex_destroy(g_mpool->hmutex);
	g_mpool->hmutex = TB_NULL;

	// clear
	if (g_mpool->data) tb_memset(g_mpool->data, 0, g_mpool->size);
	tb_memset(g_mpool, 0, sizeof(tb_mpool_t));

	// free
	g_mpool = TB_NULL;
}

// prediction
#ifdef TB_MPOOL_PRED_ENABLE

// try allocating it from the prediction rblock
#ifndef TB_DEBUG
static tb_void_t* tb_mpool_try_allocating_from_pred_rblock(tb_mpool_t* mpool, tb_size_t size, tb_size_t chunk_i)
#else
static tb_void_t* tb_mpool_try_allocating_from_pred_rblock(tb_mpool_t* mpool, tb_size_t size, tb_size_t chunk_i, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// no predicted block
	if (!mpool->pred_rblocks_n[chunk_i]) return TB_NULL;

	tb_size_t block_i = mpool->pred_rblocks[chunk_i][--mpool->pred_rblocks_n[chunk_i]];
	tb_size_t block_n = mpool->chunks[chunk_i].block_n;
	tb_size_t block_b = mpool->chunks[chunk_i].start_block;
	if (block_i >= block_b && block_i < block_b + block_n)
	{
		tb_size_t i = block_i - block_b;

		// check
		TB_ASSERT(mpool->chunks[chunk_i].free_block_n && size <= mpool->chunks[chunk_i].block_size);

		// set used flag: the block is non-free
		tb_mpool_blocks_info_set(mpool->blocks_info, block_i);
		// update the count of free blocks in the chunk
		--mpool->chunks[chunk_i].free_block_n;

 		// predict next, pred_n must be null, otherwise exists repeat item
		if (!mpool->pred_rblocks_n[chunk_i])
		{
			if ((i + 1 < block_n) && !tb_mpool_blocks_info_isset(mpool->blocks_info, block_i)) 
			{
				mpool->pred_rblocks[chunk_i][0] = block_i + 1;
				mpool->pred_rblocks_n[chunk_i] = 1;
			}
		}

	#ifdef TB_DEBUG
		// update mpool status info
		mpool->status.used_size += mpool->chunks[chunk_i].block_size;
		if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
		mpool->status.need_used_size += size;
		mpool->status.real_used_size += mpool->chunks[chunk_i].block_size;
		{
			tb_size_t block_n = mpool->chunks[chunk_i].block_n - mpool->chunks[chunk_i].free_block_n;
			mpool->status.rblock_max_n[chunk_i] = block_n > mpool->status.rblock_max_n[chunk_i]? block_n : mpool->status.rblock_max_n[chunk_i];
		}
	#endif

		// return data address
		return (mpool->chunks[chunk_i].data + i * mpool->chunks[chunk_i].block_size);
	}

	return TB_NULL;
}

// try allocating it from the prediction nrblock
#ifndef TB_DEBUG
static tb_void_t* tb_mpool_try_allocating_from_pred_nrblock(tb_mpool_t* mpool, tb_size_t size, tb_size_t asize)
#else
static tb_void_t* tb_mpool_try_allocating_from_pred_nrblock(tb_mpool_t* mpool, tb_size_t size, tb_size_t asize, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
#if 0
	// get the head of this block
	tb_byte_t* 			p = mpool->nrblock_next;
	tb_mpool_nrblock_head_t* 	phead = ((tb_mpool_nrblock_head_t*)p);

	// is free?
	if (phead && phead->is_free)
	{
		// is enough?
		tb_size_t osize = ((tb_mpool_nrblock_head_t*)p)->block_size;
		if (osize >= asize)
		{
			// skip head
			p += sizeof(tb_mpool_nrblock_head_t);

			// update head of the next free block if the free block is splited
			if (osize > asize + sizeof(tb_mpool_nrblock_head_t))
			{
				// split block
				tb_mpool_nrblock_head_t* pnext_head = (tb_mpool_nrblock_head_t*)(p + asize);
				pnext_head->block_size = osize - asize - sizeof(tb_mpool_nrblock_head_t);
				pnext_head->is_free = 1;

				// update the head of block
				phead->block_size = asize;	// set block size
				phead->is_free = 0; 				// now it is non-free
			}
			// use the whole block if the free block is not splited
			else 
			{
				// update the head of block
				phead->block_size = osize; 	// use the whole block
				phead->is_free = 0; 						// now it is non-free
			}

			// check address
			TB_ASSERT(!(((tb_size_t)p) % 4));

			// predict the next free block
		#ifdef TB_MPOOL_PRED_ENABLE
			mpool->nrblock_next = p + phead->block_size;
		#endif

		#ifdef TB_DEBUG
			// set bounds magic
			phead->magic_b = TB_MPOOL_MAGIC;
			phead->magic_e = TB_MPOOL_MAGIC;

			// save the trace info
			phead->code_line = line;
			phead->code_file = file;
			phead->code_func = func;

			// update mpool status info
			mpool->status.used_size += phead->block_size;
			if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
			mpool->status.need_used_size += asize;
			mpool->status.real_used_size += phead->block_size;
			mpool->status.nrblock_max_size = phead->block_size > mpool->status.nrblock_max_size? phead->block_size : mpool->status.nrblock_max_size;
		#endif
			// return data address
			return p;
		}
	}
#else
	tb_byte_t* 	pb = mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].data;
	tb_byte_t* 	pe = pb + mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].size;
	tb_byte_t* 	p = mpool->pred_nrblock;

	// is null?
	if (!p || p < pb || p >= pe) return TB_NULL;

	// find free block
	while (p + sizeof(tb_mpool_nrblock_head_t) < pe)
	{
		// get the head of current block
		tb_mpool_nrblock_head_t* phead = ((tb_mpool_nrblock_head_t*)p);
		if (phead->is_free) // allocate if the block is free
		{
			// is enough?
			tb_size_t osize = ((tb_mpool_nrblock_head_t*)p)->block_size;
			if (osize >= asize)
			{
				// skip head
				p += sizeof(tb_mpool_nrblock_head_t);

				// update head of the next free block if the free block is splited
				if (osize > asize + sizeof(tb_mpool_nrblock_head_t))
				{
					// split block
					tb_mpool_nrblock_head_t* pnext_head = (tb_mpool_nrblock_head_t*)(p + asize);
					pnext_head->block_size = osize - asize - sizeof(tb_mpool_nrblock_head_t);
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
				TB_ASSERT(p + phead->block_size < pe);
				TB_ASSERT(!(((tb_size_t)p) % TB_MPOOL_ALIGN_BOUNDARY));

				// predict the next free block
			#ifdef TB_MPOOL_PRED_ENABLE
				mpool->pred_nrblock = p + phead->block_size;
			#endif

			#ifdef TB_DEBUG
				// set bounds magic
				phead->magic_b = TB_MPOOL_MAGIC;
				phead->magic_e = TB_MPOOL_MAGIC;

				// save the trace info
				phead->code_line = line;
				phead->code_file = file;
				phead->code_func = func;

				// update mpool status info
				mpool->status.used_size += phead->block_size;
				if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
				mpool->status.need_used_size += size;
				mpool->status.real_used_size += phead->block_size;
				mpool->status.nrblock_max_size = phead->block_size > mpool->status.nrblock_max_size? phead->block_size : mpool->status.nrblock_max_size;
			#endif

				// return data address
				return p;
			}
#if 1
			else // attempt to merge next free block if current free block is too small
			{
				tb_mpool_nrblock_head_t* pnext_head = (tb_mpool_nrblock_head_t*)(p + sizeof(tb_mpool_nrblock_head_t) + osize);
			
				// break if doesn't exist next block
				if ((tb_size_t)pnext_head + sizeof(tb_mpool_nrblock_head_t) >= (tb_size_t)mpool->blocks_info) break;

 				// next block is free?
				if (pnext_head->is_free)
				{
					// merge next block
					phead->block_size += pnext_head->block_size + sizeof(tb_mpool_nrblock_head_t);

					// continue handle this block
					continue ;
				}
			}
#endif
		}
		break;
	}	
#endif

	return TB_NULL;
}

#endif

#ifndef TB_DEBUG
static tb_void_t* tb_mpool_allocate_no_lock(tb_mpool_t* mpool, tb_size_t size)
#else
static tb_void_t* tb_mpool_allocate_no_lock(tb_mpool_t* mpool, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	TB_ASSERT(mpool && size);
	if (!mpool || !size) return TB_NULL;

#ifdef TB_DEBUG
	mpool->status.alloc_total++;
#endif

	// allocate it from regular-chunks if block is small
	if (size <= TB_MPOOL_REGULAR_BLOCK_MAX_SIZE)
	{
		// find the chunk index of the first free regular-block
		tb_size_t chunk_i = tb_block_size_to_chunk_i(mpool->chunks, size);

		// if exists free block
		if (chunk_i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT)
		{
		#ifdef TB_MPOOL_PRED_ENABLE
			// try allocating it from predicted rblock
			tb_byte_t* p = TB_MPOOL_TRY_ALLOCATING_FROM_PRED_RBLOCK(mpool, size, chunk_i, func, line, file);
			if (p) return p;

		# 	ifdef TB_DEBUG
			if (!p) mpool->status.pred_failed++;
		# 	endif

		#endif

			//allocate a free block
			tb_size_t i = 0;
			tb_size_t block_n = mpool->chunks[chunk_i].block_n;
			for (i = 0; i < block_n; ++i)
			{
				tb_size_t block_i = mpool->chunks[chunk_i].start_block + i;
				if (!tb_mpool_blocks_info_isset(mpool->blocks_info, block_i)) // is free?
				{
					// set used flag: the block is non-free
					tb_mpool_blocks_info_set(mpool->blocks_info, block_i);
					// update the count of free blocks in the chunk
					--mpool->chunks[chunk_i].free_block_n;

					// predict next, pred_n must be null, otherwise exists repeat item
				#ifdef TB_MPOOL_PRED_ENABLE
					if (!mpool->pred_rblocks_n[chunk_i])
					{
						if ((i + 1 < block_n) && !tb_mpool_blocks_info_isset(mpool->blocks_info, block_i)) 
						{
							mpool->pred_rblocks[chunk_i][0] = block_i + 1;
							mpool->pred_rblocks_n[chunk_i] = 1;
						}
					}
				#endif

				#ifdef TB_DEBUG
					// update mpool status info
					mpool->status.used_size += mpool->chunks[chunk_i].block_size;
					if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
					mpool->status.need_used_size += size;
					mpool->status.real_used_size += mpool->chunks[chunk_i].block_size;
					{
						tb_size_t block_n = mpool->chunks[chunk_i].block_n - mpool->chunks[chunk_i].free_block_n;
						mpool->status.rblock_max_n[chunk_i] = block_n > mpool->status.rblock_max_n[chunk_i]? block_n : mpool->status.rblock_max_n[chunk_i];
					}
				#endif

					// return data address
					return (mpool->chunks[chunk_i].data + i * mpool->chunks[chunk_i].block_size);
				}
			}
		}
	}

	// align size
	tb_byte_t* 	p = TB_NULL;
	tb_size_t 	asize = tb_align(size, TB_MPOOL_ALIGN_BOUNDARY);

	// try allocating it from predicted nrblock
#ifdef TB_MPOOL_PRED_ENABLE
	p = TB_MPOOL_TRY_ALLOCATING_FROM_PRED_NRBLOCK(mpool, size, asize, func, line, file);
	if (p) return p;

# 	ifdef TB_DEBUG
	if (!p) mpool->status.pred_failed++;
# 	endif

#endif

	// allocate it from non-regular chunk
	tb_byte_t* 	pb = mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].data;
	tb_byte_t* 	pe = pb + mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].size;

	// find free block
	for (p = pb; p + sizeof(tb_mpool_nrblock_head_t) < pe;)
	{
		// get the head of current block
		tb_mpool_nrblock_head_t* 	phead = ((tb_mpool_nrblock_head_t*)p);
		tb_size_t 					osize = ((tb_mpool_nrblock_head_t*)p)->block_size;

		if (phead->is_free) // allocate if the block is free
		{
			// is enough?
			if (osize >= asize)
			{
				// skip head
				p += sizeof(tb_mpool_nrblock_head_t);

				// update head of the next free block if the free block is splited
				if (osize > asize + sizeof(tb_mpool_nrblock_head_t))
				{
					// split block
					tb_mpool_nrblock_head_t* pnext_head = (tb_mpool_nrblock_head_t*)(p + asize);
					pnext_head->block_size = osize - asize - sizeof(tb_mpool_nrblock_head_t);
					pnext_head->is_free = 1;

					// update the head of block
					phead->block_size = asize;	// set block size
					phead->is_free = 0; 				// now it is non-free
				}
				// use the whole block if the free block is not splited
				else 
				{
					// update the head of block
					phead->block_size = osize; 	// use the whole block
					phead->is_free = 0; 						// now it is non-free
				}

				// check address
				TB_ASSERT(p + phead->block_size < pe);
				TB_ASSERT(!(((tb_size_t)p) % TB_MPOOL_ALIGN_BOUNDARY));

				// predict the next free block
			#ifdef TB_MPOOL_PRED_ENABLE
				mpool->pred_nrblock = p + phead->block_size;
			#endif

			#ifdef TB_DEBUG
				// set bounds magic
				phead->magic_b = TB_MPOOL_MAGIC;
				phead->magic_e = TB_MPOOL_MAGIC;

				// save the trace info
				phead->code_line = line;
				phead->code_file = file;
				phead->code_func = func;

				// update mpool status info
				mpool->status.used_size += phead->block_size;
				if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
				mpool->status.need_used_size += size;
				mpool->status.real_used_size += phead->block_size;
				mpool->status.nrblock_max_size = phead->block_size > mpool->status.nrblock_max_size? phead->block_size : mpool->status.nrblock_max_size;
			#endif

				// return data address
				return p;
			}
			else // attempt to merge next free block if current free block is too small
			{
				tb_mpool_nrblock_head_t* pnext_head = (tb_mpool_nrblock_head_t*)(p + sizeof(tb_mpool_nrblock_head_t) + osize);
			
				// break if doesn't exist next block
				if ((tb_size_t)pnext_head + sizeof(tb_mpool_nrblock_head_t) >= (tb_size_t)mpool->blocks_info) break;

				// next block is free?
				if (pnext_head->is_free) 
				{
					// merge next block
					phead->block_size += pnext_head->block_size + sizeof(tb_mpool_nrblock_head_t);

					// reset the next predicted block
				#ifdef TB_MPOOL_PRED_ENABLE
					if (mpool->pred_nrblock == (tb_byte_t*)pnext_head)
						mpool->pred_nrblock = TB_NULL;
				#endif

					// continue handle this block
					continue ;
				}
			}
		}

		// skip it if the block is non-free or block_size is too small
		p += sizeof(tb_mpool_nrblock_head_t) + osize;
	}	
	
#ifdef TB_DEBUG
	// update the failed count
	mpool->status.alloc_failed++;
#endif
	
	return TB_NULL;
}
#ifndef TB_DEBUG
static tb_bool_t tb_mpool_deallocate_no_lock(tb_mpool_t* mpool, tb_void_t* data)
#else
static tb_bool_t tb_mpool_deallocate_no_lock(tb_mpool_t* mpool, tb_void_t* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	TB_ASSERT(mpool && data);
	if (!mpool || !data) return TB_FALSE;

	// get data address
	// |--------------------------------------------|-----------------|
	// addr_begin           addr             non-regular addr       end_addr
	//
	tb_size_t addr = (tb_size_t)data;
	tb_size_t addr_begin = (tb_size_t)mpool->data;
	tb_size_t addr_end = (tb_size_t)mpool->blocks_info;

	// check data address
	if (addr < addr_begin) return TB_FALSE;
	//TB_ASSERTM(addr >= addr_begin && addr < addr_end, "invalid data address:%x", addr);

	// deallocate it to regular chunks if the block is regular
	if (addr < mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].data)
	{
		// find chunk index
		tb_size_t chunk_i = 0;
		for (chunk_i = 0; chunk_i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT; ++chunk_i)
			if (addr < mpool->chunks[chunk_i].data + mpool->chunks[chunk_i].size) break;
		TB_ASSERT(chunk_i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT);

		// compute block index
		TB_ASSERT(addr >= (tb_size_t)mpool->chunks[chunk_i].data);
		tb_size_t offset = addr - (tb_size_t)mpool->chunks[chunk_i].data;
		tb_size_t block_i = mpool->chunks[chunk_i].start_block + offset / mpool->chunks[chunk_i].block_size;

		// check data address
		TB_ASSERT(!(offset % mpool->chunks[chunk_i].block_size));

		// reset used flag: the block is free
		tb_mpool_blocks_info_reset(mpool->blocks_info, block_i);

		// predict the next free block
	#ifdef TB_MPOOL_PRED_ENABLE
		if (mpool->pred_rblocks_n[chunk_i] < TB_MEMORY_MPOOL_PRED_RBLOCKS_MAX)
			mpool->pred_rblocks[chunk_i][mpool->pred_rblocks_n[chunk_i]++] = block_i;
	#endif

		// update the count of free blocks in the chunk
		++mpool->chunks[chunk_i].free_block_n;

	#ifdef TB_DEBUG
		// update mpool status info
		mpool->status.used_size -= mpool->chunks[chunk_i].block_size;
		if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
	#endif
		return TB_TRUE;
	}
	// deallocate it to non-regular chunk if the block is non-regular
	else if (addr < addr_end)
	{
		tb_byte_t* p = (tb_byte_t*)addr;

		// get the head of the block
		tb_mpool_nrblock_head_t* phead = ((tb_mpool_nrblock_head_t*)(p - sizeof(tb_mpool_nrblock_head_t)));

		// check this block
		TB_ASSERTM(!phead->is_free, "double free data:%x at %s(): %d, file: %s", addr, func? func : "null", line, file? file : "null");
		TB_ASSERTM(phead->block_size < mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].size, "invalid data address at free():%x", addr);

	#ifdef TB_DEBUG
		// update mpool status info
		mpool->status.used_size -= phead->block_size;
		if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
	#endif

		// merge it if next block is free
		tb_mpool_nrblock_head_t* pnext_head = (tb_mpool_nrblock_head_t*)(p + phead->block_size);
		// block size : current block size + next block head + next block size
		if (pnext_head->is_free) phead->block_size += sizeof(tb_mpool_nrblock_head_t) + pnext_head->block_size;

		phead->is_free = 1;

		// predict the next free block
	#ifdef TB_MPOOL_PRED_ENABLE
		mpool->pred_nrblock = (tb_byte_t*)phead;
	#endif

		return TB_TRUE;
	}
	//TB_ASSERTM(0, "invalid data address:%x at free()", addr);

	return TB_FALSE;
}
#ifndef TB_DEBUG
static tb_void_t* tb_mpool_reallocate_no_lock(tb_mpool_t* mpool, tb_void_t* data, tb_size_t size)
#else
static tb_void_t* tb_mpool_reallocate_no_lock(tb_mpool_t* mpool, tb_void_t* data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	TB_ASSERT(mpool && data);
	if (!mpool || !data) return TB_MPOOL_ALLOCATE_NO_LOCK(mpool, size, func, line, file);

	/* get data address
	 * |--------------------------------------------|-----------------|
	 * addr_begin           addr             non-regular addr       end_addr
	 *
	 */
	tb_size_t addr = (tb_size_t)data;
	tb_size_t addr_begin = (tb_size_t)mpool->data;
	tb_size_t addr_end = (tb_size_t)mpool->blocks_info;

	// check data address
	if (addr < addr_begin) return TB_NULL;
	//TB_ASSERTM(addr >= addr_begin && addr < addr_end, "invalid data address:%x", addr);

	// reallocate it to regular chunks if the block is regular
	if (addr < (tb_size_t)mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].data)
	{
		// find chunk index
		tb_size_t chunk_i = 0;
		for (chunk_i = 0; chunk_i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT; ++chunk_i)
			if (addr < mpool->chunks[chunk_i].data + mpool->chunks[chunk_i].size) break;
		TB_ASSERT(chunk_i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT);

		// if enough?
		if (size <= mpool->chunks[chunk_i].block_size) return data;
		else // reallocate
		{
			// compute block index
			tb_size_t offset = addr - (tb_size_t)mpool->chunks[chunk_i].data;
			tb_size_t block_i = mpool->chunks[chunk_i].start_block + offset / mpool->chunks[chunk_i].block_size;

			// check data address
			TB_ASSERT(!(offset % mpool->chunks[chunk_i].block_size));

			// allocate new buffer
			tb_byte_t* p = TB_MPOOL_ALLOCATE_NO_LOCK(mpool, size, func, line, file);
			tb_memcpy(p, data, mpool->chunks[chunk_i].block_size);

			// reset used flag: the block is free
			tb_mpool_blocks_info_reset(mpool->blocks_info, block_i);
			// update the count of free blocks in the chunk
			++mpool->chunks[chunk_i].free_block_n;

			// predict the next free block
		#ifdef TB_MPOOL_PRED_ENABLE
			if (mpool->pred_rblocks_n[chunk_i] < TB_MEMORY_MPOOL_PRED_RBLOCKS_MAX)
				mpool->pred_rblocks[chunk_i][mpool->pred_rblocks_n[chunk_i]++] = block_i;
		#endif

		#ifdef TB_DEBUG
			// update mpool status info
			mpool->status.used_size -= mpool->chunks[chunk_i].block_size;
			if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
		#endif

			return p;
		}
	}
	// reallocate it to non-regular chunk if the block is non-regular
	else if (addr < addr_end)
	{
		// get the head of the block
		tb_byte_t* 			p = (tb_byte_t*)addr;
		tb_mpool_nrblock_head_t* 	phead = ((tb_mpool_nrblock_head_t*)(p - sizeof(tb_mpool_nrblock_head_t)));
		tb_size_t 			osize = phead->block_size;

		// check this block
		TB_ASSERTM(!phead->is_free, "the buffer has been free:%x", addr);
		TB_ASSERTM(phead->block_size < mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].size, "invalid data address at realloc():%x", addr);

		// if enough?
		if (size <= osize) return data;
		else // reallocate
		{
			// merge it if next block is free
			tb_mpool_nrblock_head_t* pnext_head = (tb_mpool_nrblock_head_t*)(p + phead->block_size);
			if (pnext_head->is_free)
			{
				// align size
				tb_size_t asize = tb_align(size, TB_MPOOL_ALIGN_BOUNDARY);

				// block size : current block size + next block head + next block size
				phead->block_size += sizeof(tb_mpool_nrblock_head_t) + pnext_head->block_size;

				// reset the next predicted block
			#ifdef TB_MPOOL_PRED_ENABLE
				if (mpool->pred_nrblock == (tb_byte_t*)pnext_head)
					mpool->pred_nrblock = TB_NULL;
			#endif

				// if enough?
				if (asize <= phead->block_size) 
				{
					// update head of the next free block if the free block is splited
					if (phead->block_size > asize + sizeof(tb_mpool_nrblock_head_t))
					{
						// split block
						pnext_head = (tb_mpool_nrblock_head_t*)(p + asize);
						pnext_head->block_size = phead->block_size - asize - sizeof(tb_mpool_nrblock_head_t);
						pnext_head->is_free = 1;

						// update the head of block
						phead->block_size = asize;	// set block size
						phead->is_free = 0; 				// now it is non-free
					}
					// use the whole block if the free block is not splited
					else ; 

					// predict the next free block
				#ifdef TB_MPOOL_PRED_ENABLE
					mpool->pred_nrblock = p + phead->block_size;
				#endif

				#ifdef TB_DEBUG
					// set bounds magic
					phead->magic_b = TB_MPOOL_MAGIC;
					phead->magic_e = TB_MPOOL_MAGIC;

					// save the trace info
					phead->code_line = line;
					phead->code_file = file;
					phead->code_func = func;

					// update mpool status info
					mpool->status.used_size += phead->block_size - osize;
					if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
				#endif
					return p;
				}
			}

			// allocate new buffer
			p = TB_MPOOL_ALLOCATE_NO_LOCK(mpool, size, func, line, file);
			if (p) tb_memcpy(p, data, osize);
			else return TB_NULL;

			// free it to allocate a new block if not enough
			phead->is_free = 1;

			// predict the next free block
		#ifdef TB_MPOOL_PRED_ENABLE
			mpool->pred_nrblock = (tb_byte_t*)phead;
		#endif

		#ifdef TB_DEBUG
			// update mpool status info
			mpool->status.used_size -= osize;
			if (mpool->status.used_size > mpool->status.peak_size) mpool->status.peak_size = mpool->status.used_size;
		#endif
			return p;
		}
	}
	//TB_ASSERTM(0, "invalid data address:%x at realloc()", addr);
	return TB_NULL;
}
#ifndef TB_DEBUG
tb_void_t* tb_mpool_allocate(tb_size_t size)
#else
tb_void_t* tb_mpool_allocate(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	TB_IF_FAIL_RETURN_VAL(size, TB_NULL);
	TB_ASSERT_RETURN_VAL(g_mpool && g_mpool->magic == TB_MPOOL_MAGIC, TB_NULL);

	// lock
	if (TB_FALSE == tb_mutex_lock(g_mpool->hmutex)) return TB_NULL;
	tb_byte_t* p = TB_MPOOL_ALLOCATE_NO_LOCK(g_mpool, size, func, line, file);
	tb_mutex_unlock(g_mpool->hmutex);

	// align by 4 bytes
	TB_ASSERT(!(((tb_size_t)p) & TB_CPU_BITALIGN));

	TB_ASSERTM(p, "cannot alloc at %s(): %d, file: %s", func? func : "null", line, file? file : "null");
	return p;
}


#ifndef TB_DEBUG
tb_void_t* tb_mpool_callocate(tb_size_t item, tb_size_t size)
{
	tb_void_t* p = tb_mpool_allocate(item * size);
	if (p) tb_memset(p, 0, item * size);
	return p;
}
#else
tb_void_t* tb_mpool_callocate(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
{
	tb_void_t* p = tb_mpool_allocate(item * size, func, line, file);
	if (p) tb_memset(p, 0, item * size);
	return p;
}
#endif


#ifndef TB_DEBUG
tb_void_t* tb_mpool_reallocate(tb_void_t* data, tb_size_t size)
#else
tb_void_t* tb_mpool_reallocate(tb_void_t* data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	TB_IF_FAIL_RETURN_VAL(size, TB_NULL);
	TB_ASSERT_RETURN_VAL(g_mpool && g_mpool->magic == TB_MPOOL_MAGIC, TB_NULL);

	// lock
	if (TB_FALSE == tb_mutex_lock(g_mpool->hmutex)) return TB_NULL;
	tb_byte_t* p = TB_MPOOL_REALLOCATE_NO_LOCK(g_mpool, data, size, func, line, file);
	tb_mutex_unlock(g_mpool->hmutex);

	// align by 4 bytes
	TB_ASSERT(!(((tb_size_t)p) & TB_CPU_BITALIGN));

	TB_ASSERTM(p, "invalid realloc data address:%x at %s(): %d, file: %s", data, func? func : "null", line, file? file : "null");
	return p;
}
#ifndef TB_DEBUG
tb_void_t tb_mpool_deallocate(tb_void_t* data)
#else
tb_void_t tb_mpool_deallocate(tb_void_t* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	TB_ASSERT_RETURN(g_mpool && g_mpool->magic == TB_MPOOL_MAGIC);

	// lock
	if (TB_FALSE == tb_mutex_lock(g_mpool->hmutex)) return ;
	tb_bool_t ret = TB_MPOOL_DEALLOCATE_NO_LOCK(g_mpool, data, func, line, file);
	tb_mutex_unlock(g_mpool->hmutex);

	TB_ASSERTM(ret, "invalid free data address:%x at %s(): %d, file: %s", data, func? func : "null", line, file? file : "null");
}

#ifdef TB_DEBUG

static tb_bool_t tb_mpool_allocate_try(tb_mpool_t* mpool, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(mpool && size, TB_FALSE);

	// allocate it from regular-chunks if block is small
	if (size <= TB_MPOOL_REGULAR_BLOCK_MAX_SIZE)
	{
		// find the chunk index of the first free regular-block
		tb_size_t chunk_i = tb_block_size_to_chunk_i(mpool->chunks, size);

		// if exists free block
		if (chunk_i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT)
		{
			// allocate a free block
			tb_size_t i = 0;
			for (i = 0; i < mpool->chunks[chunk_i].block_n; ++i)
			{
				tb_size_t block_i = mpool->chunks[chunk_i].start_block + i;
				if (!tb_mpool_blocks_info_isset(mpool->blocks_info, block_i)) // is free?
				{
					return TB_TRUE;
				}
			}
		}
	}

	// allocate it from non-regular chunk
	tb_byte_t* 	p = mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].data;
	tb_byte_t* 	pe = p + mpool->chunks[TB_MPOOL_NON_REGULAR_CHUNCK_INDEX].size;

	// find free block
	while (p + sizeof(tb_mpool_nrblock_head_t) < pe)
	{
		// get the head of current block
		tb_mpool_nrblock_head_t* 	phead = ((tb_mpool_nrblock_head_t*)p);
		tb_size_t 					osize = ((tb_mpool_nrblock_head_t*)p)->block_size;

		if (phead->is_free) // allocate if the block is free
		{
			// align size
			tb_size_t asize = tb_align(size, TB_MPOOL_ALIGN_BOUNDARY);
			if (osize >= asize) // enough?
			{
				return TB_TRUE;
			}
			else // attempt to merge next free block if current free block is too small
			{
				tb_mpool_nrblock_head_t* pnext_head = (tb_mpool_nrblock_head_t*)(p + sizeof(tb_mpool_nrblock_head_t) + osize);
			
				// break if doesn't exist next block
				if ((tb_size_t)pnext_head + sizeof(tb_mpool_nrblock_head_t) >= (tb_size_t)mpool->blocks_info) break;

				if (pnext_head->is_free) // next block is free?
				{
					// merge next block
					phead->block_size += pnext_head->block_size + sizeof(tb_mpool_nrblock_head_t);

					// reset the next predicted block
				#ifdef TB_MPOOL_PRED_ENABLE
					if (mpool->pred_nrblock == (tb_byte_t*)pnext_head)
						mpool->pred_nrblock = TB_NULL;
				#endif

					// continue handle this block
					continue ;
				}
			}
		}

		// skip it if the block is non-free or block_size is too small
		p += sizeof(tb_mpool_nrblock_head_t) + osize;
	}	

	return TB_FALSE;
}
tb_void_t tb_mpool_dump()
{
	tb_size_t i = 0, j = 0;
	tb_byte_t* p = TB_NULL;
	tb_byte_t* pe = TB_NULL;

	TB_ASSERT_RETURN(g_mpool);

	TB_DBG("=============================================");
	TB_DBG("memory mpool info:");
	TB_DBG("mpool data addr:%x", (tb_size_t)g_mpool->data);
	TB_DBG("mpool data size:%u", g_mpool->size);
	TB_DBG("mpool blocks_n:%u\n", g_mpool->blocks_n);
	TB_DBG("regular chunks info:");
	for (i = 0; i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT; ++i)
	{
		TB_DBG("chunk(%d):", g_mpool->chunks[i].block_size);
		TB_DBG("\tdata:%x size:%u", (tb_size_t)g_mpool->chunks[i].data, g_mpool->chunks[i].size);
		TB_DBG("\tstart block:%d block_n:%u free_block_n:%u", g_mpool->chunks[i].start_block, g_mpool->chunks[i].block_n, g_mpool->chunks[i].free_block_n);

		// dump allocated blocks info
		{
			tb_char_t str[4 * 8 + 4];
			p = (tb_byte_t*)str;
			for (j = 0; j < g_mpool->chunks[i].block_n; ++j)
			{
				if (!(j % 32)) 
				{
					*p = '\0';
					TB_DBG("\t%s", str);
					p = (tb_byte_t*)str;
				}
				if ((j % 32) && !(j % 8)) *p++ = ' ';
				if (tb_mpool_blocks_info_isset(g_mpool->blocks_info, g_mpool->chunks[i].start_block + j)) *p = '1';
				else *p = '0';
				++p;
			}
			*p = '\0';
			TB_DBG("\t%s", str);
		}
	}
	TB_DBG("non-regular chunks info:");
	TB_DBG("\tdata:%x size:%u", (tb_size_t)g_mpool->chunks[i].data, g_mpool->chunks[i].size);
	p = g_mpool->chunks[i].data;
	pe = p + g_mpool->chunks[i].size;
	i = 0;
	while (p + sizeof(tb_mpool_nrblock_head_t) < pe)
	{
		tb_mpool_nrblock_head_t* phead = (tb_mpool_nrblock_head_t*)p;

		if (!phead->is_free)
		{
			TB_DBG("\tblock(%d) data:%x size:%u head_size:%d is_free:%s, b: 0x%x, e: 0x%x, at %s(): %d, file: %s"
				, i++
				, (tb_size_t)(p + sizeof(tb_mpool_nrblock_head_t))
				, phead->block_size
				, sizeof(tb_mpool_nrblock_head_t)
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
			TB_DBG("\tblock(%d) data:%x size:%u head_size:%d is_free:%s"
				, i++
				, (tb_size_t)(p + sizeof(tb_mpool_nrblock_head_t))
				, phead->block_size
				, sizeof(tb_mpool_nrblock_head_t)
				, "true"
				);
		}
		p += sizeof(tb_mpool_nrblock_head_t) + phead->block_size;
	}

	TB_DBG("blocks info:");
	TB_DBG("\tdata:%x size:%u", (tb_size_t)g_mpool->blocks_info, (tb_size_t)(g_mpool->data + g_mpool->size - g_mpool->blocks_info));

	TB_DBG("=============================================");
	TB_DBG("mpool status info:");
	TB_DBG("=============================================");
	TB_DBG("total:%u", g_mpool->status.total_size);
	TB_DBG("used:%u", g_mpool->status.used_size);
	TB_DBG("peak:%u", g_mpool->status.peak_size);
	TB_DBG("wast:%u%%", g_mpool->status.real_used_size? (g_mpool->status.real_used_size - g_mpool->status.need_used_size) * 100 / g_mpool->status.real_used_size : 0);
	TB_DBG("fail:%u", g_mpool->status.alloc_failed);
	TB_DBG("pred:%02d%%, fail: %d, total: %d", g_mpool->status.alloc_total? ((g_mpool->status.alloc_total - g_mpool->status.pred_failed) * 100 / g_mpool->status.alloc_total) : -1, g_mpool->status.pred_failed, g_mpool->status.alloc_total);

	for (i = 0; i < TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT; ++i)
		TB_DBG("regular chunk:%d total_block_n:%u block_max_n:%u used rate:%u%% recommend_block_n:%u", g_mpool->chunks[i].block_size, g_mpool->chunks[i].block_n, g_mpool->status.rblock_max_n[i], g_mpool->chunks[i].block_n? g_mpool->status.rblock_max_n[i] * 100 / g_mpool->chunks[i].block_n : 0, 2 + g_mpool->status.rblock_max_n[i] * 100 / 80);
	TB_DBG("nrblock_max_size:%u", g_mpool->status.nrblock_max_size);

	if (!tb_mpool_check())
		TB_DBG("the mpool maybe exists error!");
}
tb_bool_t tb_mpool_check()
{
	// check magic
	if (!g_mpool || g_mpool->magic != TB_MPOOL_MAGIC) return TB_FALSE;

	// check chunk
	if (!g_mpool->chunks) return TB_FALSE;

	//check non-regular chunks
	tb_byte_t const* p = g_mpool->chunks[TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT].data;
	tb_byte_t const* pe = p + g_mpool->chunks[TB_MPOOL_REGULAR_CHUNCK_MAX_COUNT].size;
	if (!p || !pe || p >= pe) return TB_FALSE;
	while (p + sizeof(tb_mpool_nrblock_head_t) < pe)
	{
		tb_mpool_nrblock_head_t* phead = (tb_mpool_nrblock_head_t*)p;
		if (!phead->is_free)
		{
			if (!phead->block_size
				|| phead->magic_b != TB_MPOOL_MAGIC
				|| phead->magic_e != TB_MPOOL_MAGIC
				) 
			{
				return TB_FALSE;
			}
		}
		p += sizeof(tb_mpool_nrblock_head_t) + phead->block_size;
	}

	// is enough?
	if (TB_FALSE == tb_mpool_allocate_try(g_mpool, 10)) return TB_FALSE;

	return TB_TRUE;
}

#endif
