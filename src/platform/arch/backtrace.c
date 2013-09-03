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
 * @file		backtrace.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_backtrace_frames(tb_cpointer_t* frames, tb_size_t nframe, tb_size_t nskip)
{
	tb_trace_noimpl();
	return 0;
}
tb_handle_t tb_backtrace_symbols_init(tb_cpointer_t* frames, tb_size_t nframe)
{
	tb_trace_noimpl();
	return tb_null;
}
tb_char_t const* tb_backtrace_symbols_name(tb_handle_t symbols, tb_cpointer_t* frames, tb_size_t nframe, tb_size_t iframe)
{
	tb_trace_noimpl();
	return tb_null;
}
tb_void_t tb_backtrace_symbols_exit(tb_handle_t symbols)
{
	tb_trace_noimpl();
}

