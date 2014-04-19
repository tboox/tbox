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
 * @file		zip.c
 * @ingroup 	filter
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "filter.h"
#include "../zip/zip.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the zip filter type
typedef struct __tb_filter_zip_t
{
	// the filter base
	tb_filter_t 			base;

	// the zip 
	tb_zip_t* 				zip;

}tb_filter_zip_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_filter_zip_t* tb_filter_zip_cast(tb_filter_t* filter)
{
	// check
	tb_assert_and_check_return_val(filter && filter->type == TB_FILTER_TYPE_ZIP, tb_null);
	return (tb_filter_zip_t*)filter;
}
static tb_long_t tb_filter_zip_spak(tb_filter_t* filter, tb_static_stream_t* istream, tb_static_stream_t* ostream, tb_long_t sync)
{
	// check
	tb_filter_zip_t* zfilter = tb_filter_zip_cast(filter);
	tb_assert_and_check_return_val(zfilter && zfilter->zip && istream && ostream, -1);

	// spak it
	return tb_zip_spak(zfilter->zip, istream, ostream, sync);
}
static tb_void_t tb_filter_zip_cler(tb_filter_t* filter)
{
	// check
	tb_filter_zip_t* zfilter = tb_filter_zip_cast(filter);
	tb_assert_and_check_return(zfilter);

	// clear zip
	if (zfilter->zip) 
	{
		// the algo 
		tb_size_t algo = zfilter->zip->algo;

		// the action
		tb_size_t action = zfilter->zip->action;

		// exit zip
		tb_zip_exit(zfilter->zip);

		// init zip
		zfilter->zip = tb_zip_init(algo, action);
		tb_assert_and_check_return(zfilter->zip);
	}
}
static tb_void_t tb_filter_zip_exit(tb_filter_t* filter)
{
	// check
	tb_filter_zip_t* zfilter = tb_filter_zip_cast(filter);
	tb_assert_and_check_return(zfilter);

	// exit zip
	if (zfilter->zip) tb_zip_exit(zfilter->zip);
	zfilter->zip = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_filter_t* tb_filter_init_from_zip(tb_size_t algo, tb_size_t action)
{
	// done
	tb_bool_t 			ok = tb_false;
	tb_filter_zip_t* 	filter = tb_null;
	do
	{
		// make filter
		filter = (tb_filter_zip_t*)tb_malloc0(sizeof(tb_filter_zip_t));
		tb_assert_and_check_break(filter);

		// init filter 
		if (!tb_filter_init((tb_filter_t*)filter, TB_FILTER_TYPE_ZIP)) break;
		filter->base.spak = tb_filter_zip_spak;
		filter->base.cler = tb_filter_zip_cler;
		filter->base.exit = tb_filter_zip_exit;

		// init zip
		filter->zip = tb_zip_init(algo, action);
		tb_assert_and_check_break(filter->zip);

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

