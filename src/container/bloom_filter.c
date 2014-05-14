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
 * @file		bloom_filter.c
 * @ingroup 	container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 				"bloom_filter"
#define TB_TRACE_MODULE_DEBUG 				(1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bloom_filter.h"
#include "../libc/libc.h"
#include "../libm/libm.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
// the data size maxn
#ifdef __tb_small__
# 	define TB_BLOOM_FILTER_DATA_MAXN 			(1 << 28)
#else
# 	define TB_BLOOM_FILTER_DATA_MAXN 			(1 << 30)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the bloom filter impl type
typedef struct __tb_bloom_filter_impl_t
{
	// the probability
	tb_size_t 			probability;

	// the hash count
	tb_size_t 			hash_count;

	// the maxn
	tb_size_t 			maxn;

	// the func
	tb_item_func_t 		func;

	// the size
	tb_size_t 			size;

	// the data
	tb_byte_t* 			data;

}tb_bloom_filter_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bloom_filter_t* tb_bloom_filter_init(tb_size_t probability, tb_size_t hash_count, tb_size_t maxn, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(func.hash, tb_null);

	// done
	tb_bool_t 				ok = tb_false;
	tb_bloom_filter_impl_t* filter = tb_null;
	do
	{
		// check 
		tb_assert_and_check_break(probability && probability < 32);
		tb_assert_and_check_break(hash_count && hash_count < 16);
		tb_assert_and_check_break(maxn && maxn < TB_MAXU32);

		// make filter
		filter = (tb_bloom_filter_impl_t*)tb_malloc0(sizeof(tb_bloom_filter_impl_t));
		tb_assert_and_check_break(filter);
	
		// init filter
		filter->func 		= func;
		filter->maxn 		= maxn;
		filter->hash_count 	= hash_count;
		filter->probability = probability;

		/* compute the storage space
		 *
		 * c = p^(1/k)
 		 * s = m / n = 2k / (2c + c * c)
		 */
#ifdef TB_CONFIG_TYPE_FLOAT
		tb_double_t k = (tb_double_t)hash_count;
		tb_double_t p = 1. / (tb_double_t)(1 << probability);
		tb_double_t c = tb_pow(p, 1 / k);
		tb_double_t s = (k + k) / (c + c + c * c);
		tb_size_t 	n = maxn;
		tb_size_t 	m = n * tb_round(s);
		tb_trace_d("k: %lf, p: %lf, c: %lf, s: %lf => p: %lf, m: %lu, n: %lu", k, p, c, s, tb_pow((1 - tb_exp(-k / s)), k), m, n);
#else

#if 0
		// make scale table
		tb_size_t i = 0;
		for (i = 1; i < 16; i++)
		{
			tb_printf(", { ");
			tb_size_t j = 0;
			for (j = 0; j < 31; j++)
			{
				tb_double_t k = (tb_double_t)i;
				tb_double_t p = 1. / (tb_double_t)(1 << j);
				tb_double_t c = tb_pow(p, 1 / k);
				tb_double_t s = (k + k) / (c + c + c * c);
				if (j != 30) tb_printf("%lu, ", tb_round(s));
				else tb_printf("%lu }\n", tb_round(s));
			}
		}
#endif
		// the scale
		static tb_size_t s_scale[15][31] =
		{
		  { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824 }
		, { 1, 2, 3, 5, 7, 10, 15, 22, 31, 44, 63, 90, 127, 180, 255, 361, 511, 723, 1023, 1447, 2047, 2895, 4095, 5792, 8191, 11584, 16383, 23169, 32767, 46340, 65535 }
		, { 2, 3, 4, 5, 6, 8, 11, 14, 18, 23, 29, 37, 47, 59, 75, 95, 119, 151, 191, 240, 303, 383, 482, 608, 767, 966, 1218, 1535, 1934, 2437, 3071 }
		, { 3, 3, 4, 5, 6, 8, 10, 12, 14, 17, 21, 25, 30, 36, 43, 52, 62, 74, 89, 106, 126, 150, 179, 213, 254, 302, 360, 429, 510, 607, 722 }
		, { 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 18, 21, 24, 28, 32, 38, 44, 50, 58, 67, 78, 89, 103, 119, 137, 158, 181, 209, 240, 276, 318 }
		, { 4, 5, 5, 6, 7, 8, 10, 11, 13, 14, 16, 19, 21, 24, 28, 31, 35, 40, 45, 51, 58, 65, 73, 83, 93, 105, 118, 133, 149, 168, 189 }
		, { 5, 5, 6, 7, 8, 9, 10, 11, 13, 14, 16, 18, 20, 22, 25, 28, 31, 34, 38, 43, 47, 53, 59, 65, 72, 80, 89, 98, 109, 120, 133 }
		, { 5, 6, 7, 7, 8, 9, 10, 12, 13, 14, 16, 17, 19, 21, 23, 26, 28, 31, 34, 38, 42, 46, 50, 55, 60, 66, 72, 79, 87, 95, 104 }
		, { 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 27, 29, 32, 35, 38, 41, 45, 49, 53, 58, 62, 68, 74, 80, 86 }
		, { 7, 7, 8, 9, 10, 10, 11, 12, 14, 15, 16, 17, 19, 20, 22, 24, 26, 28, 30, 33, 36, 38, 41, 45, 48, 52, 56, 60, 65, 70, 75 }
		, { 7, 8, 9, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 22, 24, 25, 27, 29, 32, 34, 36, 39, 42, 45, 48, 52, 55, 59, 63, 68 }
		, { 8, 9, 9, 10, 11, 12, 13, 13, 14, 16, 17, 18, 19, 21, 22, 24, 25, 27, 29, 31, 33, 35, 38, 40, 43, 45, 48, 52, 55, 59, 62 }
		, { 9, 9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 18, 20, 21, 22, 24, 25, 27, 28, 30, 32, 34, 36, 39, 41, 44, 46, 49, 52, 55, 58 }
		, { 9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 27, 28, 30, 32, 34, 36, 38, 40, 42, 45, 47, 50, 53, 56 }
		, { 10, 11, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 27, 28, 30, 32, 33, 35, 37, 39, 41, 43, 46, 48, 51, 53 }
		};
		tb_size_t m = maxn * s_scale[hash_count - 1][probability];
#endif
		
		// init size
		filter->size = tb_align8(m) >> 3;
		tb_assert_and_check_break(filter->size);
		if (filter->size > TB_BLOOM_FILTER_DATA_MAXN)
		{
			tb_trace_e("the need space too large, size: %lu, please decrease hash count and probability!", filter->size);
			break;
		}
		tb_trace_d("size: %lu", filter->size);

		// init data
		filter->data = (tb_byte_t*)tb_malloc0(filter->size);
		tb_assert_and_check_break(filter->data);

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit it
		if (filter) tb_bloom_filter_exit((tb_bloom_filter_t*)filter);
		filter = tb_null;
	}

	// ok?
	return (tb_bloom_filter_t*)filter;
}
tb_void_t tb_bloom_filter_exit(tb_bloom_filter_t* handle)
{
	// check
	tb_bloom_filter_impl_t* filter = (tb_bloom_filter_impl_t*)handle;
	tb_assert_and_check_return(filter);

	// exit data
	if (filter->data) tb_free(filter->data);
	filter->data = tb_null;

	// exit it
	tb_free(filter);
}
tb_void_t tb_bloom_filter_clear(tb_bloom_filter_t* handle)
{
	// check
	tb_bloom_filter_impl_t* filter = (tb_bloom_filter_impl_t*)handle;
	tb_assert_and_check_return(filter);

	// clear it
	if (filter->data && filter->size) tb_memset(filter->data, 0, filter->size);
}
tb_bool_t tb_bloom_filter_set(tb_bloom_filter_t* handle, tb_cpointer_t data)
{
	// check
	tb_bloom_filter_impl_t* filter = (tb_bloom_filter_impl_t*)handle;
	tb_assert_and_check_return_val(filter, tb_false);

	return tb_false;
}
tb_bool_t tb_bloom_filter_get(tb_bloom_filter_t* handle, tb_cpointer_t data)
{
	// check
	tb_bloom_filter_impl_t* filter = (tb_bloom_filter_impl_t*)handle;
	tb_assert_and_check_return_val(filter, tb_false);

	return tb_false;
}

