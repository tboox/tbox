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
 * @file		trace.c
 * @ingroup 	utils
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "trace.h"

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */


/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_trace_init()
{
	return tb_false;
}
tb_void_t tb_trace_exit()
{
}
tb_size_t tb_trace_mode()
{
	return 0;
}
tb_bool_t tb_trace_mode_set(tb_size_t mode)
{
	return tb_false;
}
tb_handle_t tb_trace_file()
{
	return tb_null;
}
tb_bool_t tb_trace_file_set(tb_handle_t file)
{
	return tb_false;
}
tb_bool_t tb_trace_file_set_path(tb_char_t const* path, tb_bool_t bappend)
{
	return tb_false;
}
tb_void_t tb_trace_debug(tb_char_t const* prefix, tb_char_t const* module, tb_char_t const* format, ...)
{
}
tb_void_t tb_trace_info(tb_char_t const* prefix, tb_char_t const* module, tb_char_t const* format, ...)
{
}
tb_void_t tb_trace_error(tb_char_t const* prefix, tb_char_t const* module, tb_char_t const* format, ...)
{
}
