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
 * @file		filter.c
 * @ingroup 	filter
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "filter.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_filter_cler(tb_filter_t* filter)
{
	// check
	tb_assert_and_check_return(filter);
	
	// clear it
	if (filter->cler) filter->cler(filter);

	// exit idata
	tb_pbuffer_clear(&filter->idata);

	// exit odata
	tb_pbuffer_clear(&filter->odata);
}
tb_void_t tb_filter_exit(tb_filter_t* filter)
{
	// check
	tb_assert_and_check_return(filter);
	
	// exit it
	if (filter->exit) filter->exit(filter);

	// exit idata
	tb_pbuffer_exit(&filter->idata);

	// exit odata
	tb_pbuffer_exit(&filter->odata);

	// free it
	tb_free(filter);
}
tb_long_t tb_filter_spak(tb_filter_t* filter, tb_byte_t const* data, tb_size_t size, tb_byte_t const** pdata, tb_size_t need, tb_long_t sync)
{
	// check
	tb_assert_and_check_return_val(filter && filter->spak && pdata, -1);

	// ok?
	return -1;
}


