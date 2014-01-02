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
 * @file		astream.c
 * @ingroup 	stream
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 				"astream"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "astream.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_astream_t* tb_astream_init_from_url(tb_aicp_t* aicp, tb_char_t const* url)
{
	return tb_null;
}
tb_void_t tb_astream_exit(tb_astream_t* ast)
{
}
tb_void_t tb_astream_kill(tb_astream_t* ast)
{
}
tb_bool_t tb_astream_open(tb_astream_t* ast, tb_astream_open_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_astream_read(tb_astream_t* ast, tb_astream_read_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_astream_writ(tb_astream_t* ast, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_astream_save(tb_astream_t* ast, tb_astream_t* ost, tb_astream_save_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_astream_seek(tb_astream_t* ast, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_astream_sync(tb_astream_t* ast, tb_astream_sync_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_astream_try_open(tb_astream_t* ast)
{
	return tb_false;
}
tb_bool_t tb_astream_try_seek(tb_astream_t* ast, tb_hize_t offset)
{
	return tb_false;
}
tb_aicp_t* tb_astream_aicp(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return_val(ast, tb_null);

	// the aicp
	return ast->aicp;
}
tb_size_t tb_astream_type(tb_astream_t const* ast)
{
	// check
	tb_assert_and_check_return_val(ast, TB_ASTREAM_TYPE_NONE);

	// the type
	return ast->type;
}
tb_hize_t tb_astream_size(tb_astream_t const* ast)
{
	return 0;
}
tb_hize_t tb_astream_left(tb_astream_t const* ast)
{
	return 0;
}
tb_hize_t tb_astream_offset(tb_astream_t const* ast)
{
	return 0;
}
tb_size_t tb_astream_timeout(tb_astream_t const* ast)
{
	return 0;
}
tb_char_t const* tb_astream_state_cstr(tb_size_t state)
{
	return tb_null;
}
tb_bool_t tb_astream_ctrl(tb_astream_t* ast, tb_size_t ctrl, ...)
{
	return tb_false;
}
