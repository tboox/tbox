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
 * @file		cache.c
 * @ingroup 	filter
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 			"cache"
#define TB_TRACE_MODULE_DEBUG 			(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "filter.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the cache filter type
typedef struct __tb_filter_cache_t
{
	// the filter base
	tb_filter_t 			base;

}tb_filter_cache_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_filter_cache_t* tb_filter_cache_cast(tb_filter_t* filter)
{
	// check
	tb_assert_and_check_return_val(filter && filter->type == TB_FILTER_TYPE_CACHE, tb_null);
	return (tb_filter_cache_t*)filter;
}
static tb_long_t tb_filter_cache_spak(tb_filter_t* filter, tb_bstream_t* istream, tb_bstream_t* ostream, tb_long_t sync)
{
	// check
	tb_filter_cache_t* cfilter = tb_filter_cache_cast(filter);
	tb_assert_and_check_return_val(cfilter && istream && ostream, -1);
	tb_assert_and_check_return_val(tb_bstream_valid(istream) && tb_bstream_valid(ostream), -1);

	// the idata
	tb_byte_t const* 	ip = tb_bstream_pos(istream);
	tb_byte_t const* 	ie = tb_bstream_end(istream);

	// the odata
	tb_byte_t* 			op = (tb_byte_t*)tb_bstream_pos(ostream);
	tb_byte_t* 			oe = (tb_byte_t*)tb_bstream_end(ostream);
	tb_byte_t* 			ob = op;

	// the need 
	tb_size_t 			need = tb_min(ie - ip, oe - op);

	// copy data
	if (need) tb_memcpy(op, ip, need);
	ip += need;
	op += need;

	// update stream
	tb_bstream_goto(istream, (tb_byte_t*)ip);
	tb_bstream_goto(ostream, (tb_byte_t*)op);

	// no data and sync end? end
	if (sync < 0 && op == ob && !tb_bstream_left(istream)) return -1;

	// ok
	return (op - ob);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_filter_t* tb_filter_init_from_cache(tb_size_t size)
{
	// done
	tb_bool_t 			ok = tb_false;
	tb_filter_cache_t* 	filter = tb_null;
	do
	{
		// make filter
		filter = (tb_filter_cache_t*)tb_malloc0(sizeof(tb_filter_cache_t));
		tb_assert_and_check_break(filter);

		// init filter 
		if (!tb_filter_init((tb_filter_t*)filter, TB_FILTER_TYPE_CACHE)) break;
		filter->base.spak = tb_filter_cache_spak;

		// init the cache size
		if (size) tb_queue_buffer_resize(&filter->base.odata, size);

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit filter
		tb_filter_exit((tb_filter_t*)filter);
		filter = tb_null;
	}

	// ok?
	return (tb_filter_t*)filter;
}

