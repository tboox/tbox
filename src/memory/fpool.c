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
 * \file		fpool.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fpool.h"
#include "../libc/libc.h"
#include "../math/math.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_FPOOL_MAGIC 							(0xdead)

// the used sets
#define tb_fpool_used_set1(used, i) 			do {(used)[(i) >> 3] |= (0x1 << ((i) & 7));} while (0)
#define tb_fpool_used_set0(used, i) 			do {(used)[(i) >> 3] &= ~(0x1 << ((i) & 7));} while (0)
#define tb_fpool_used_bset(used, i) 			((used)[(i) >> 3] & (0x1 << ((i) & 7)))

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the fpool info type
#ifdef TB_DEBUG
typedef struct __tb_fpool_info_t
{
	// the peak size
	tb_size_t 			peak;

	// the fail count
	tb_size_t 			fail;

	// the pred count
	tb_size_t 			pred;

	// the aloc count
	tb_size_t 			aloc;

}tb_fpool_info_t;
#endif

/* the fixed pool type
 *
 * |---------|-----------------|-----------------------------------------------|
 *    head          used                            data                         
 */
typedef struct __tb_fpool_t
{
	// the magic 
	tb_size_t 			magic 	: 16;

	// the align
	tb_size_t 			align 	: 16;

	// the step
	tb_size_t 			step;

	// the maxn
	tb_size_t 			maxn;

	// the size
	tb_size_t 			size;

	// the data
	tb_byte_t* 			data;

	// the used
	tb_byte_t* 			used;

	// the pred
	tb_byte_t* 			pred;

	// the info
#ifdef TB_DEBUG
	tb_fpool_info_t 	info;
#endif

}tb_fpool_t;

/* ///////////////////////////////////////////////////////////////////////
 * the implemention
 */
tb_handle_t tb_fpool_init(tb_byte_t* data, tb_size_t size, tb_size_t step, tb_size_t align)
{
	// check
	tb_assert_and_check_return_val(data && step && size, TB_NULL);

	// align
	align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;

	// align data
	tb_size_t byte = (tb_size_t)tb_align((tb_size_t)data, align) - (tb_size_t)data;
	tb_assert_and_check_return_val(size >= byte, TB_NULL);
	size -= byte;
	data += byte;

	// init data
	tb_memset(data, 0, size);

	// init fpool
	tb_fpool_t* fpool = data;

	// init magic
	fpool->magic = TB_FPOOL_MAGIC;

	// init align
	fpool->align = align;

	// init step
	fpool->step = tb_align(step, fpool->align);

	// init used
	fpool->used = (tb_byte_t*)tb_align((tb_size_t)&fpool[1], fpool->align);
	tb_assert_and_check_return_val(data + size > fpool->used, TB_NULL);

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
	fpool->maxn = ((((tb_byte_t*)data + size - fpool->used) << 3) - 7) / (1 + (fpool->step << 3));
	tb_assert_and_check_return_val(fpool->maxn, TB_NULL);

	// init data
	fpool->data = (tb_byte_t*)tb_align((tb_size_t)fpool->used + (tb_align8(fpool->maxn) >> 3), fpool->align);
	tb_assert_and_check_return_val(data + size > fpool->data, TB_NULL);
	tb_assert_and_check_return_val(fpool->maxn * fpool->step == (data + size - fpool->data), TB_NULL);

	// init size
	fpool->size = 0;

	// init pred
	fpool->pred = fpool->data;

	// init info
#ifdef TB_DEBUG
	fpool->info.peak = 0;
	fpool->info.fail = 0;
	fpool->info.pred = 0;
	fpool->info.aloc = 0;
#endif

	// ok
	return ((tb_handle_t)fpool);
}
tb_void_t tb_fpool_exit(tb_handle_t handle)
{
	// check 
	tb_fpool_t* fpool = (tb_fpool_t*)handle;
	tb_assert_and_check_return(fpool && fpool->magic == TB_FPOOL_MAGIC);

	// clear body
	tb_fpool_clear(handle);

	// clear head
	tb_memset(fpool, 0, sizeof(tb_fpool_t));
}
tb_void_t tb_fpool_clear(tb_handle_t handle)
{
	// check 
	tb_fpool_t* fpool = (tb_fpool_t*)handle;
	tb_assert_and_check_return(fpool && fpool->magic == TB_FPOOL_MAGIC);

	// clear data
	if (fpool->data) tb_memset(fpool->data, 0, fpool->size);
	
	// clear used
	if (fpool->used) tb_memset(fpool->used, 0, (tb_align8(fpool->maxn) >> 3));

	// reinit size
	fpool->size = 0;
	
	// reinit pred
	fpool->pred = fpool->data;
	
	// reinit info
#ifdef TB_DEBUG
	fpool->info.peak = 0;
	fpool->info.fail = 0;
	fpool->info.pred = 0;
	fpool->info.aloc = 0;
#endif
}

tb_pointer_t tb_fpool_malloc(tb_handle_t handle)
{
	// check 
	tb_fpool_t* fpool = (tb_fpool_t*)handle;
	tb_assert_and_check_return_val(fpool && fpool->magic == TB_FPOOL_MAGIC, TB_NULL);
	tb_assert_and_check_return_val(fpool->step && fpool->size < fpool->maxn, TB_NULL);

	// predict it?
	tb_pointer_t data = TB_NULL;
	if (fpool->pred)
	{
		tb_size_t i = ((tb_byte_t*)fpool->pred - fpool->data) / fpool->step;
		tb_assert_and_check_return_val(!(((tb_byte_t*)fpool->pred - fpool->data) % fpool->step), TB_NULL);
		if (!tb_fpool_used_bset(fpool->used, i)) 
		{
			// ok
			data = fpool->pred;
			tb_fpool_used_set1(fpool->used, i);

			// predict the next block
			if (i + 1 < fpool->maxn && !tb_fpool_used_bset(fpool->used, i + 1))
				fpool->pred = (i + 1) * fpool->step;

#ifdef TB_DEBUG
			// pred++
			fpool->info.pred++;
#endif
		}
	}

	// find the free block
	if (!data)
	{
		tb_size_t 	i = 0;
		tb_size_t 	m = fpool->maxn;
		tb_byte_t* 	p = fpool->used;
		tb_byte_t 	u = *p;
		tb_byte_t 	b = 0;
		for (i = 0; i < m; ++i)
		{
			// bit
			b = i & 0x07;

			// u++
			if (!b) u = *p++;

			// is free?
			// if (!tb_fpool_used_bset(fpool->used, i))
			if (!(u & (0x01 << b)))
			{
				// ok
				data = fpool->data + i * fpool->step;
				// tb_fpool_used_set1(fpool->used, i);
				*(p - 1) |= (0x01 << b);

				// predict the next block
				if (i + 1 < m && !tb_fpool_used_bset(fpool->used, i + 1))
					fpool->pred = (i + 1) * fpool->step;

				break;
			}
		}
	}

	// size++
	if (data) fpool->size++;

	// update info
#ifdef TB_DEBUG
	if (fpool->size > fpool->info.peak) fpool->info.peak = fpool->size;
	fpool->info.fail += data? 0 : 1;
	fpool->info.aloc++;
#endif

	// ok?
	return data;
}
tb_pointer_t tb_fpool_malloc0(tb_handle_t handle)
{
	// check 
	tb_fpool_t* fpool = (tb_fpool_t*)handle;
	tb_assert_and_check_return_val(fpool && fpool->magic == TB_FPOOL_MAGIC, TB_NULL);

	// malloc
	tb_pointer_t p = tb_fpool_malloc(handle);

	// clear
	if (p) tb_memset(p, 0, fpool->step);

	// ok?
	return p;
}
tb_bool_t tb_fpool_free(tb_handle_t handle, tb_pointer_t data)
{
	// check 
	tb_fpool_t* fpool = (tb_fpool_t*)handle;
	tb_assert_and_check_return_val(fpool && fpool->magic == TB_FPOOL_MAGIC, TB_FALSE);
	tb_assert_and_check_return_val(fpool->step && fpool->size, TB_FALSE);

	// check data
	tb_assert_and_check_return_val(!(((tb_size_t)data) & (fpool->align - 1)), TB_FALSE);
	tb_assert_and_check_return_val(data >= fpool->data && (tb_byte_t*)data + fpool->step <= fpool->data + fpool->size, TB_FALSE);

	// free it
	tb_size_t i = ((tb_byte_t*)data - fpool->data) / fpool->step;
	tb_assert_and_check_return_val(!(((tb_byte_t*)data - fpool->data) % fpool->step), TB_FALSE);
	tb_fpool_used_set0(fpool->used, i);
	
	// predict it
	fpool->pred = data;

	// size--
	fpool->size--;

	// ok
	return TB_TRUE;
}
#ifdef TB_DEBUG
tb_void_t tb_fpool_dump(tb_handle_t handle)
{
	tb_fpool_t* fpool = (tb_fpool_t*)handle;
	tb_assert_and_check_return(fpool);

	tb_print("======================================================================");
	tb_print("fpool: magic: 0x%lx",	fpool->magic);
	tb_print("fpool: align: %lu", 	fpool->align);
	tb_print("fpool: data: %p", 	fpool->data);
	tb_print("fpool: size: %lu", 	fpool->size);
	tb_print("fpool: step: %lu", 	fpool->step);
	tb_print("fpool: maxn: %lu", 	fpool->maxn);
	tb_print("fpool: peak: %lu", 	fpool->info.peak);
	tb_print("fpool: fail: %lu", 	fpool->info.fail);
	tb_print("fpool: pred: %lu%%", 	fpool->info.aloc? ((fpool->info.pred * 100) / fpool->info.aloc) : 0);

	tb_size_t 	i = 0;
	tb_size_t 	m = fpool->maxn;
	for (i = 0; i < m; ++i)
	{
		tb_print("\tfpool: block[%lu]: data: %p free: %lu"
				, i
				, fpool->data + i * fpool->step
				, (tb_size_t)tb_fpool_used_bset(fpool->used, i)
				);
	}
}
#endif
