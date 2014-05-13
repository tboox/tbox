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
# 	define TB_BLOOM_FILTER_DATA_MAXN 			(1 << 24)
#else
# 	define TB_BLOOM_FILTER_DATA_MAXN 			(1 << 28)
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
# 		error
#endif
		
		// init size
		filter->size = tb_align8(m) >> 3;
		tb_assert_and_check_break(filter->size && filter->size < TB_BLOOM_FILTER_DATA_MAXN);
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

