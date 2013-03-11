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
 * @file		tpool.c
 * @ingroup 	memory
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tpool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_TPOOL_MAGIC 							(0xdead)

// the align maxn
#define TB_TPOOL_ALIGN_MAXN 					(64)

// the block maxn in the chunk
#define TB_TPOOL_BLOCK_MAXN 					(sizeof(tb_size_t) << 3)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

#ifdef TB_DEBUG
// the tpool info type
typedef struct __tb_tpool_info_t
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

}tb_tpool_info_t;
#endif

/*!the tiny pool type
 *
 * <pre>
 * pool: |---------|-----------------|-----------------------------------------------|
 *           head          used                            data                         
 *                 |--------|--------|
 *                    body     last
 *
 * used:
 * last: |---------------------------|-----------------------------|--- ... ---------|
 *             chunk0(32|64 bits)              chunk1                  chunki
 *       |---------------------------|
 *             sizeof(tb_size_t)
 *       |------|----|---------|-----|
 *                blocki..   block0   <= for little endian 
 *
 * 
 * body: |---------------------------|-----------------------------|--- ... ---------|
 *             chunk0(32|64 bits)              chunk1                  chunki
 *       |---------------------------|
 *             sizeof(tb_size_t)
 *       |------||||||||||||---||||--|
 *                blocki..   block0   <= for little endian 
 *
 *
 * data: |----------------------------------------------------|----|--- ... ---------|
 *                         chunk0(32|64 blocks)                         chunki
 *       |--------------|                       |-------------|
 *            block0                                blocki          
 *       |-----|-----|------|--------- ... -----|------|------|----|--- ... ---------|
 *        step0 step1 step2                       stepi   ...
 *
 * pred:
 * pred[i]:       0               : no pred
 * pred[0]:       1 x step        : |----------------------|-------------------------|---...   -------|
 * pred[1]:       2 x step        : -|----------------------|-------------------------|---...   -------|
 * pred[2]:       3 x step        : --|----------------------|-------------------------|---...   -------|
 * pred[3]:       4 x step        : ---|----------------------|-------------------------|---...   -------|
 * pred[4]:       5 x step        : ----|----------------------|-------------------------|---...   -------|
 * pred[5]:       6 x step        : -----|----------------------|-------------------------|---...   -------|
 * pred[6]:       7 x step        : ------|----------------------|-------------------------|---...   -------|
 * ...
 * pred[31|63]:   (32|64) x step
 *
 * note:
 * 1. align bytes <= 64
 * 2. alloc bytes <= (32|64) * 16 == 512|1024 for one chunk
 * 3. step bytes == max(align, 16)
 * </pre>
 */
typedef struct __tb_tpool_t
{
	/// the magic 
	tb_size_t 			magic 	: 16;

	/// the align
	tb_size_t 			align 	: 7;

	/// the step
	tb_size_t 			step 	: 7;

	/// the full
	tb_size_t 			full 	: 1;

	/// the body
	tb_size_t* 			body;

	/// the last
	tb_size_t* 			last;

	/// the maxn
	tb_size_t 			maxn;

	/// the data
	tb_byte_t* 			data;

	/// the pred
	tb_size_t 			pred[TB_TPOOL_BLOCK_MAXN];

	/// the info
#ifdef TB_DEBUG
	tb_tpool_info_t 	info;
#endif

}tb_tpool_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_size_t tb_tpool_find_free(tb_size_t body, tb_size_t bits, tb_size_t bitn)
{
#if 0
	tb_size_t 	blkn = TB_TPOOL_BLOCK_MAXN;
	tb_size_t 	blks = ~body;
	while (((((blks >> (TB_TPOOL_BLOCK_MAXN - blkn)) & bits) != bits) && blkn--)) ;
	return TB_TPOOL_BLOCK_MAXN - blkn;
#elif 0
	tb_size_t 	blki = 0;
	tb_size_t 	blkn = TB_TPOOL_BLOCK_MAXN;
	tb_size_t 	blks = ~body;
	while (blki < TB_TPOOL_BLOCK_MAXN)
	{
		if (((blks >> (blki + 0)) & bits) == bits) { blki += 0; break; }
		if (((blks >> (blki + 1)) & bits) == bits) { blki += 1; break; }
		if (((blks >> (blki + 2)) & bits) == bits) { blki += 2; break; }
		if (((blks >> (blki + 3)) & bits) == bits) { blki += 3; break; }
		if (((blks >> (blki + 4)) & bits) == bits) { blki += 4; break; }
		if (((blks >> (blki + 5)) & bits) == bits) { blki += 5; break; }
		if (((blks >> (blki + 6)) & bits) == bits) { blki += 6; break; }
		if (((blks >> (blki + 7)) & bits) == bits) { blki += 7; break; }
		blki += 8;
	}
	return blki;
#else
	tb_size_t 	blki = 0;
	tb_size_t 	blkn = TB_TPOOL_BLOCK_MAXN;
	tb_size_t 	blks = ~body;
	tb_size_t 	blkb = 0;
	while (blki < TB_TPOOL_BLOCK_MAXN)
	{
		blkb = (blks >> blki) & bits;
		if (blkb != bits)
		{
			if (!blkb) blki += bitn;
			else if (blkb == 1) blki++;
			else blki += TB_TPOOL_BLOCK_MAXN - tb_bits_fb0_be(blkb | ~bits);
		}
		else break;
	}
	return blki;
#endif
}
static tb_pointer_t tb_tpool_malloc_pred(tb_tpool_t* tpool, tb_size_t size, tb_size_t bits, tb_size_t bitn)
{	
	// no pred?
	tb_check_return_val(tpool->pred[bitn - 1], tb_null);

	// init
	tb_size_t 	maxn = tpool->maxn;
	tb_size_t 	pred = tpool->pred[bitn - 1] - 1;
	tb_size_t* 	body = tpool->body + pred;
	tb_size_t* 	last = tpool->last + pred;
		
	// find 
	tb_size_t 	blki = tb_tpool_find_free(*body, bits, bitn);
	tb_check_return_val(blki < TB_TPOOL_BLOCK_MAXN, tb_null);

	// alloc it
	tb_byte_t* 	data = tpool->data + (pred * TB_TPOOL_BLOCK_MAXN + blki) * tpool->step;
	*body |= bits << blki;
	*last |= (tb_size_t)1 << (blki + bitn - 1);
	
	// full? no the next free block space
	if (blki + bitn + bitn > TB_TPOOL_BLOCK_MAXN)
	{
		// the next
		if (tpool->pred[bitn - 1] + TB_TPOOL_BLOCK_MAXN <= maxn) tpool->pred[bitn - 1] += TB_TPOOL_BLOCK_MAXN;
		// no pred
		else tpool->pred[bitn - 1] = 0;
	}

	// update the info
#ifdef TB_DEBUG
	tpool->info.pred++;
#endif

	// ok
	return data;
}
static tb_pointer_t tb_tpool_malloc_find(tb_tpool_t* tpool, tb_size_t size, tb_size_t bits, tb_size_t bitn)
{
	// init
	tb_size_t* 	body = tpool->body;
	tb_size_t* 	last = tpool->last;
	tb_size_t 	maxn = tpool->maxn;
	tb_byte_t* 	data = tb_null;

	// find the free bit index for the enough space in the little-endian sort
	tb_size_t 	blki = TB_TPOOL_BLOCK_MAXN;
	tb_size_t* 	tail = body + maxn;
#if 1
	tb_size_t 	i = 0;	
	while (body + 7 < tail)
	{
		// find chunk
		do 
		{
			if (body[0] + 1 && tb_bits_cb0(body[0]) >= bitn) { i = 0; break; }
			if (body[1] + 1 && tb_bits_cb0(body[1]) >= bitn) { i = 1; break; }
			if (body[2] + 1 && tb_bits_cb0(body[2]) >= bitn) { i = 2; break; }
			if (body[3] + 1 && tb_bits_cb0(body[3]) >= bitn) { i = 3; break; }
			if (body[4] + 1 && tb_bits_cb0(body[4]) >= bitn) { i = 4; break; }
			if (body[5] + 1 && tb_bits_cb0(body[5]) >= bitn) { i = 5; break; }
			if (body[6] + 1 && tb_bits_cb0(body[6]) >= bitn) { i = 6; break; }
			if (body[7] + 1 && tb_bits_cb0(body[7]) >= bitn) { i = 7; break; }
			i = 8;

		} while (0);

		// if (*body != 0xffffffff)
		// if (*body != 0xffffffffffffffffL)
		if (i < 8)
		{
			// this chunk
			body += i;
			
			// find 
			blki = tb_tpool_find_free(*body, bits, bitn);

			// ok?
			if (blki < TB_TPOOL_BLOCK_MAXN)
			{
				last += (body - tpool->body);
				break;
			}
			
			// next chunk
			body++;
		}
		else body += 8;
	}
#endif
	if (blki == TB_TPOOL_BLOCK_MAXN)
	{
		while (body < tail) 
		{
			// if (*body != 0xffffffff)
			// if (*body != 0xffffffffffffffffL)
			if (*body + 1 && tb_bits_cb0(*body) >= bitn)
			{
				// find 
				blki = tb_tpool_find_free(*body, bits, bitn);

				// ok?
				if (blki < TB_TPOOL_BLOCK_MAXN)
				{
					last += (body - tpool->body);
					break;
				}
			}

			// next chunk
			body++;
		}
	}

	// no space?
	tb_check_return_val(body < tail && blki < TB_TPOOL_BLOCK_MAXN, tb_null);

	// alloc it
	data = tpool->data + ((body - tpool->body) * TB_TPOOL_BLOCK_MAXN + blki) * tpool->step;
	*body |= bits << blki;
	*last |= (tb_size_t)1 << (blki + bitn - 1);
	
	// predict the next
	tpool->pred[bitn - 1] = (body - tpool->body) + 1;
	// full? no the next free block space
	if (blki + bitn + bitn > TB_TPOOL_BLOCK_MAXN)
	{
		// the next
		if (tpool->pred[bitn - 1] + TB_TPOOL_BLOCK_MAXN <= maxn) tpool->pred[bitn - 1] += TB_TPOOL_BLOCK_MAXN;
		// no pred
		else tpool->pred[bitn - 1] = 0;
	}

	// ok
	return data;
}
tb_pointer_t tb_tpool_ralloc_fast(tb_tpool_t* tpool, tb_pointer_t data, tb_size_t size, tb_size_t* osize);
tb_pointer_t tb_tpool_ralloc_fast(tb_tpool_t* tpool, tb_pointer_t data, tb_size_t size, tb_size_t* osize)
{
	// check
	tb_assert_and_check_return_val(tpool && tpool->magic == TB_TPOOL_MAGIC, tb_null);

	// check data
	tb_check_return_val(data, tb_null);
	tb_check_return_val(data >= tpool->data && data < tpool->data + tpool->maxn * tpool->step * TB_TPOOL_BLOCK_MAXN, tb_null);
	tb_check_return_val(!(((tb_size_t)data) & (tpool->align - 1)), tb_null);
	tb_check_return_val(!(((tb_byte_t*)data - tpool->data) % tpool->step), tb_null);

	// the block bit index
	tb_size_t 	blki = ((tb_byte_t*)data - tpool->data) / tpool->step;

	// the body & last
	tb_size_t* 	body = tpool->body + blki / TB_TPOOL_BLOCK_MAXN;
	tb_size_t* 	last = tpool->last + (body - tpool->body);
	blki &= TB_TPOOL_BLOCK_MAXN - 1;

	// the block bit number
	tb_size_t 	bitn = tb_bits_fb1_le(*last >> blki) + 1;	
	tb_size_t 	bits = ((tb_size_t)1 << bitn) - 1;	
	tb_assert_and_check_return_val(bitn <= TB_TPOOL_BLOCK_MAXN, tb_null);

	// osize
	if (osize) *osize = bitn * tpool->step;

	// ok?
	return (tb_align(size, tpool->step) <= bitn * tpool->step)? data : tb_null;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_tpool_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	// check
	tb_assert_and_check_return_val(data && size, tb_null);
	tb_assert_static(TB_TPOOL_BLOCK_MAXN == sizeof(tb_size_t) << 3);

	// align
	align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
	align = tb_max(align, TB_CPU_BITBYTE);
	tb_assert_and_check_return_val(align <= TB_TPOOL_ALIGN_MAXN, tb_null);

	// align data
	tb_size_t byte = (tb_size_t)tb_align((tb_size_t)data, align) - (tb_size_t)data;
	tb_assert_and_check_return_val(size >= byte, tb_null);
	size -= byte;
	data += byte;
	tb_assert_and_check_return_val(size, tb_null);

	// init data
	tb_memset(data, 0, size);

	// init tpool
	tb_tpool_t* tpool = data;

	// init magic
	tpool->magic = TB_TPOOL_MAGIC;

	// init align
	tpool->align = align;

	// init step
	tpool->step = tb_max(tpool->align, 16);

	// init full
	tpool->full = 0;

	// init body
	tpool->body = (tb_size_t*)tb_align((tb_size_t)&tpool[1], tpool->align);
	tb_assert_and_check_return_val(data + size > (tb_byte_t*)tpool->body, tb_null);
	tb_assert_and_check_return_val(!(((tb_size_t)tpool->body) & (TB_CPU_BITBYTE - 1)), tb_null);

	/*!init maxn
	 *
	 * <pre>
	 * body + last + data < left
	 * sizeof(tb_size_t) * maxn * 2 + maxn * sizeof(tb_size_t) * 8 * step < left
	 * sizeof(tb_size_t) * maxn * 2 * (1 + 4 * step) < left
	 * maxn < left / ((1 + 4 * step) * 2 * sizeof(tb_size_t))
	 * </pre>
	 */
	tpool->maxn = (data + size - (tb_byte_t*)tpool->body) / ((1 + (tpool->step << 2)) * (sizeof(tb_size_t) << 1));
	tb_assert_and_check_return_val(tpool->maxn >= TB_TPOOL_BLOCK_MAXN, tb_null);

	// init last
	tpool->last = tpool->body + tpool->maxn;
	tb_assert_and_check_return_val(data + size > (tb_byte_t*)tpool->last, tb_null);
	tb_assert_and_check_return_val(!(((tb_size_t)tpool->last) & (TB_CPU_BITBYTE - 1)), tb_null);

	// init data
	tpool->data = (tb_byte_t*)tb_align((tb_size_t)(tpool->last + tpool->maxn), tpool->align);
	tb_assert_and_check_return_val(data + size > tpool->data, tb_null);
	tb_assert_and_check_return_val(tpool->maxn * tpool->step * TB_TPOOL_BLOCK_MAXN <= (data + size - tpool->data), tb_null);

	// init pred => the first chunk index at used
	tb_size_t n = TB_TPOOL_BLOCK_MAXN; while (n--) tpool->pred[n] = n + 1;

	// init info
#ifdef TB_DEBUG
	tpool->info.used = 0;
	tpool->info.peak = 0;
	tpool->info.need = 0;
	tpool->info.real = 0;
	tpool->info.fail = 0;
	tpool->info.pred = 0;
	tpool->info.aloc = 0;
#endif

	// ok
	return ((tb_handle_t)tpool);
}
tb_void_t tb_tpool_exit(tb_handle_t handle)
{
	// check 
	tb_tpool_t* tpool = (tb_tpool_t*)handle;
	tb_assert_and_check_return(tpool && tpool->magic == TB_TPOOL_MAGIC);

	// clear body
	tb_tpool_clear(handle);

	// clear last
	tb_memset(tpool, 0, sizeof(tb_tpool_t));
}
tb_void_t tb_tpool_clear(tb_handle_t handle)
{
	// check 
	tb_tpool_t* tpool = (tb_tpool_t*)handle;
	tb_assert_and_check_return(tpool && tpool->magic == TB_TPOOL_MAGIC);

	// clear data
	if (tpool->data) tb_memset(tpool->data, 0, tpool->maxn * tpool->step * TB_TPOOL_BLOCK_MAXN);

	// clear body
	if (tpool->body) tb_memset(tpool->body, 0, tpool->maxn * sizeof(tb_size_t));
	
	// clear last
	if (tpool->last) tb_memset(tpool->last, 0, tpool->maxn * sizeof(tb_size_t));

	// reinit pred
	tb_size_t n = TB_TPOOL_BLOCK_MAXN; while (n--) tpool->pred[n] = n + 1;
	
	// reinit full
	tpool->full = 0;
	
	// reinit info
#ifdef TB_DEBUG
	tpool->info.used = 0;
	tpool->info.peak = 0;
	tpool->info.need = 0;
	tpool->info.real = 0;
	tpool->info.fail = 0;
	tpool->info.pred = 0;
	tpool->info.aloc = 0;
#endif
}

tb_size_t tb_tpool_limit(tb_handle_t handle)
{
	// check
	tb_tpool_t* tpool = (tb_tpool_t*)handle;
	tb_assert_and_check_return_val(tpool && tpool->magic == TB_TPOOL_MAGIC, 0);

	return (TB_TPOOL_BLOCK_MAXN * tpool->step);
}
tb_pointer_t tb_tpool_malloc(tb_handle_t handle, tb_size_t size)
{
	// check
	tb_tpool_t* tpool = (tb_tpool_t*)handle;
	tb_assert_and_check_return_val(tpool && tpool->magic == TB_TPOOL_MAGIC, tb_null);

	// no size?
	tb_check_return_val(size, tb_null);

	// too large?
	tb_check_return_val(size <= tpool->step * TB_TPOOL_BLOCK_MAXN, tb_null);
	
	// larger than limit, maybe slower
	tb_assert(size <= tb_tpool_limit(tpool));

	// full?
	tb_check_return_val(!tpool->full, tb_null);

	// the free block bit in the chunk
	// e.g. 3 blocks => bits: 111
	tb_size_t 	bitn = tb_align(size, tpool->step) / tpool->step;
	tb_size_t 	bits = ((tb_size_t)1 << bitn) - 1;
	tb_assert_and_check_return_val(bitn && bitn <= TB_TPOOL_BLOCK_MAXN, tb_null);

	// predict it?
//	tb_pointer_t data = tb_null;
	tb_pointer_t data = tb_tpool_malloc_pred(tpool, size, bits, bitn);

	// find the free block
	if (!data) data = tb_tpool_malloc_find(tpool, size, bits, bitn);

	// update info
#ifdef TB_DEBUG
	if (data)
	{
		// update the used size
		tpool->info.used += tb_align(size, tpool->step);

		// update the need size
		tpool->info.need += size;

		// update the real size		
		tpool->info.real += tb_align(size, tpool->step);

		// update the peak size
		if (tpool->info.used > tpool->info.peak) tpool->info.peak = tpool->info.used;
		
	}
	// fail++
	else tpool->info.fail++;
	
	// aloc++
	tpool->info.aloc++;
#endif

	// full?
	if (!data) tpool->full = 1;

	// ok?
	return data;
}

tb_pointer_t tb_tpool_malloc0(tb_handle_t handle, tb_size_t size)
{
	// malloc
	tb_byte_t* p = tb_tpool_malloc(handle, size);

	// clear
	if (p && size) tb_memset(p, 0, size);

	// ok?
	return p;
}

tb_pointer_t tb_tpool_nalloc(tb_handle_t handle, tb_size_t item, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// malloc
	return tb_tpool_malloc(handle, item * size);
}

tb_pointer_t tb_tpool_nalloc0(tb_handle_t handle, tb_size_t item, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// malloc
	return tb_tpool_malloc0(handle, item * size);
}

tb_pointer_t tb_tpool_ralloc(tb_handle_t handle, tb_pointer_t data, tb_size_t size)
{
	// check
	tb_tpool_t* tpool = (tb_tpool_t*)handle;
	tb_assert_and_check_return_val(tpool && tpool->magic == TB_TPOOL_MAGIC, tb_null);

	// free it if no size
	if (!size)
	{
		tb_tpool_free(tpool, data);
		return tb_null;
	}

	// alloc it if no data?
	if (!data) return tb_tpool_malloc(tpool, size);
		
	// larger than limit, maybe slower
	tb_assert(size <= tb_tpool_limit(tpool));

	// ralloc it with fast mode if enough
	tb_size_t 		osize = 0;
	tb_pointer_t 	pdata = tb_tpool_ralloc_fast(tpool, data, size, &osize);
	tb_check_return_val(!pdata, pdata);
	tb_assert_and_check_return_val(osize && osize < size, tb_null);

	// malloc it
	pdata = tb_tpool_malloc(tpool, size);
	tb_check_return_val(pdata, tb_null);
	tb_assert_and_check_return_val(pdata != data, pdata);

	// copy data
	tb_memcpy(pdata, data, osize);
	
	// free it
	tb_tpool_free(tpool, data);

	// ok
	return pdata;
}

tb_bool_t tb_tpool_free(tb_handle_t handle, tb_pointer_t data)
{
	// check
	tb_tpool_t* tpool = (tb_tpool_t*)handle;
	tb_assert_and_check_return_val(tpool && tpool->magic == TB_TPOOL_MAGIC, tb_false);

	// no data?
	tb_check_return_val(data, tb_true);

	// check data
	tb_check_return_val(data >= tpool->data && data < tpool->data + tpool->maxn * tpool->step * TB_TPOOL_BLOCK_MAXN, tb_false);
	tb_check_return_val(!(((tb_size_t)data) & (tpool->align - 1)), tb_false);
	tb_check_return_val(!(((tb_byte_t*)data - tpool->data) % tpool->step), tb_false);

	// the block bit index
	tb_size_t 	blki = ((tb_byte_t*)data - tpool->data) / tpool->step;

	// the body & last
	tb_size_t* 	body = tpool->body + blki / TB_TPOOL_BLOCK_MAXN;
	tb_size_t* 	last = tpool->last + (body - tpool->body);
	blki &= TB_TPOOL_BLOCK_MAXN - 1;

	// the block bit number
	tb_size_t 	bitn = tb_bits_fb1_le(*last >> blki) + 1;	
	tb_size_t 	bits = ((tb_size_t)1 << bitn) - 1;	
	tb_assert_and_check_return_val(bitn <= TB_TPOOL_BLOCK_MAXN, tb_false);

	// double free? valid?
	tb_assert_and_check_return_val(((*body >> blki) & bits) == bits, tb_false);	

	// free it
	*body &= ~(bits << blki);
	*last &= ~((tb_size_t)1 << (blki + bitn - 1));
		
	// predict it
	tpool->pred[bitn - 1] = (body - tpool->body) + 1;

	// null? no full
	if (!*body) tpool->full = 0;

	// update the info
#ifdef TB_DEBUG
	tpool->info.used -= bitn * tpool->step;
#endif

	// ok
	return tb_true;
}


#ifdef TB_DEBUG
tb_void_t tb_tpool_dump(tb_handle_t handle)
{
	tb_tpool_t* tpool = (tb_tpool_t*)handle;
	tb_assert_and_check_return(tpool);

	tb_print("======================================================================");
	tb_print("tpool: magic: %#lx",	tpool->magic);
	tb_print("tpool: align: %lu", 	tpool->align);
	tb_print("tpool: limit: %lu", 	tb_tpool_limit(tpool));
	tb_print("tpool: step: %lu", 	tpool->step);
	tb_print("tpool: head: %lu", 	tpool->data - (tb_byte_t*)tpool);
	tb_print("tpool: data: %p", 	tpool->data);
	tb_print("tpool: size: %lu", 	tpool->maxn * tpool->step * TB_TPOOL_BLOCK_MAXN);
	tb_print("tpool: full: %lu", 	tpool->full);
	tb_print("tpool: used: %lu", 	tpool->info.used);
	tb_print("tpool: peak: %lu", 	tpool->info.peak);
	tb_print("tpool: wast: %lu%%", 	(tpool->info.real + (tpool->data - (tb_byte_t*)tpool) - tpool->info.need) * 100 / (tpool->info.real + (tpool->data - (tb_byte_t*)tpool)));
	tb_print("tpool: fail: %lu", 	tpool->info.fail);
	tb_print("tpool: pred: %lu%%", 	tpool->info.aloc? ((tpool->info.pred * 100) / tpool->info.aloc) : 0);

	tb_print("");
	tb_size_t 	i = 0;
	tb_size_t 	m = TB_TPOOL_BLOCK_MAXN;
	for (i = 0; i < m; i++)
	{
		tb_size_t pred = tpool->pred[i];
#if TB_CPU_BIT64
		tb_print("tpool: [%lu]: pred: %lu, body: %064lb, last: %064lb", i, pred, pred? tpool->body[pred - 1] : 0, pred? tpool->last[pred - 1] : 0);
#elif TB_CPU_BIT32
		tb_print("tpool: [%lu]: pred: %lu, body: %032lb, last: %032lb", i, pred, pred? tpool->body[pred - 1] : 0, pred? tpool->last[pred - 1] : 0);
#endif
	}

	tb_print("");
	m = tpool->maxn;
	for (i = 0; i < m; i++)
	{
#if TB_CPU_BIT64
		if (tpool->body[i]) tb_print("tpool: [%lu]: body: %064lb, last: %064lb", i, tpool->body[i], tpool->last[i]);
#elif TB_CPU_BIT32
		if (tpool->body[i]) tb_print("tpool: [%lu]: body: %032lb, last: %032lb", i, tpool->body[i], tpool->last[i]);
#endif
	}

}
#endif
