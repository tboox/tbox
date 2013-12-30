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
 * types
 */

// the aicp file type
typedef struct __tb_aicp_astream_t
{

}tb_aicp_astream_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_astream_t* tb_astream_init_file(tb_aicp_t* aicp, tb_astream_func_t const* func)
{
	return tb_null;
}
tb_astream_t* tb_astream_init_sock(tb_aicp_t* aicp, tb_astream_func_t const* func)
{
	return tb_null;
}
tb_astream_t* tb_astream_init_http(tb_aicp_t* aicp, tb_astream_func_t const* func)
{
	return tb_null;
}
tb_void_t tb_astream_exit(tb_astream_t* ast)
{
}

tb_astream_t* tb_astream_init_from_url(tb_aicp_t* aicp, tb_astream_func_t const* func, tb_char_t const* url)
{
	return tb_null;
}
tb_astream_t* tb_astream_init_from_file(tb_aicp_t* aicp, tb_astream_func_t const* func, tb_char_t const* path)
{
	return tb_null;
}
tb_astream_t* tb_astream_init_from_sock(tb_aicp_t* aicp, tb_astream_func_t const* func, tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
	return tb_null;
}
tb_astream_t* tb_astream_init_from_http(tb_aicp_t* aicp, tb_astream_func_t const* func, tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl)
{
	return tb_null;
}
tb_void_t tb_astream_kill(tb_astream_t* ast)
{
}
tb_long_t tb_astream_open(tb_astream_t* ast)
{
	return 0;
}
tb_long_t tb_astream_read(tb_astream_t* ast)
{
	return 0;
}
tb_long_t tb_astream_writ(tb_astream_t* ast, tb_byte_t const* data, tb_size_t size)
{
	return 0;
}
tb_long_t tb_astream_seek(tb_astream_t* ast, tb_hize_t offset)
{
	return 0;
}
tb_aicp_t* tb_astream_aicp(tb_astream_t* ast)
{
	return tb_null;
}
tb_size_t tb_astream_type(tb_astream_t const* ast)
{
	return 0;
}
tb_hize_t tb_astream_size(tb_astream_t const* ast)
{
	return 0;
}
tb_bool_t tb_astream_ctrl(tb_astream_t* ast, tb_size_t ctrl, ...)
{
	return tb_false;
}
