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

tb_handle_t tb_filter_init_from_zip(tb_size_t algo, tb_size_t action)
{
	return tb_null;
}
tb_handle_t tb_filter_init_from_charset(tb_size_t fr, tb_size_t to)
{
	return tb_null;
}
tb_handle_t tb_filter_init_from_chunked(tb_bool_t dechunked)
{
	return tb_null;
}
tb_void_t tb_filter_cler(tb_handle_t filter)
{
}
tb_void_t tb_filter_exit(tb_handle_t filter)
{
}
tb_long_t tb_filter_spak(tb_handle_t filter, tb_byte_t const* data, tb_size_t size, tb_byte_t const** pdata, tb_long_t sync)
{
	return -1;
}


