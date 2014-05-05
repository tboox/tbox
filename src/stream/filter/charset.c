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
 * @file		charset.c
 * @ingroup 	stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "filter.h"
#include "../../charset/charset.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the charset filter type
typedef struct __tb_stream_filter_charset_t
{
	// the filter base
	tb_stream_filter_t 			base;

	// the from type
	tb_size_t 				ftype;

	// the to type
	tb_size_t 				ttype;

}tb_stream_filter_charset_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_stream_filter_charset_t* tb_stream_filter_charset_cast(tb_stream_filter_t* filter)
{
	// check
	tb_assert_and_check_return_val(filter && filter->type == TB_STREAM_FILTER_TYPE_CHARSET, tb_null);
	return (tb_stream_filter_charset_t*)filter;
}
static tb_long_t tb_stream_filter_charset_spak(tb_stream_filter_t* filter, tb_static_stream_t* istream, tb_static_stream_t* ostream, tb_long_t sync)
{
	// check
	tb_stream_filter_charset_t* cfilter = tb_stream_filter_charset_cast(filter);
	tb_assert_and_check_return_val(cfilter && TB_CHARSET_TYPE_OK(cfilter->ftype) && TB_CHARSET_TYPE_OK(cfilter->ttype) && istream && ostream, -1);

	// spak it
	tb_long_t real = tb_charset_conv_bst(cfilter->ftype, cfilter->ttype, istream, ostream);

	// no data and sync end? end it
	if (!real && sync < 0) real = -1;

	// ok?
	return real;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_stream_filter_t* tb_stream_filter_init_from_charset(tb_size_t fr, tb_size_t to)
{
	// done
	tb_bool_t 				ok = tb_false;
	tb_stream_filter_charset_t* 	filter = tb_null;
	do
	{
		// make filter
		filter = (tb_stream_filter_charset_t*)tb_malloc0(sizeof(tb_stream_filter_charset_t));
		tb_assert_and_check_break(filter);

		// init filter 
		if (!tb_stream_filter_init((tb_stream_filter_t*)filter, TB_STREAM_FILTER_TYPE_CHARSET)) break;
		filter->base.spak = tb_stream_filter_charset_spak;

		// init the from and to charset
		filter->ftype = fr;
		filter->ttype = to;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit filter
		tb_stream_filter_exit((tb_stream_filter_t*)filter);
		filter = tb_null;
	}

	// ok?
	return (tb_stream_filter_t*)filter;
}

