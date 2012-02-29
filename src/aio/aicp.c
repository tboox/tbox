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
 * \author		ruki
 * \file		aicp.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"
#include "aioo.h"

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_aicp_t* tb_aicp_init(tb_size_t type, tb_size_t maxn)
{
	tb_trace_noimpl();
	return TB_NULL;
}
tb_void_t tb_aicp_exit(tb_aicp_t* aicp)
{
	tb_trace_noimpl();
}
tb_size_t tb_aicp_maxn(tb_aicp_t* aicp)
{
	tb_trace_noimpl();
	return 0;
}
tb_size_t tb_aicp_size(tb_aicp_t* aicp)
{
	tb_trace_noimpl();
	return 0;
}
tb_size_t tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata)
{
	tb_trace_noimpl();
	return 0;
}
tb_size_t tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata)
{
	tb_trace_noimpl();
	return 0;
}
tb_void_t tb_aicp_adde(tb_aicp_t* aicp, tb_handle_t handle, tb_aice_t const* aice)
{
	tb_trace_noimpl();
}
tb_void_t tb_aicp_dele(tb_aicp_t* aicp, tb_handle_t handle, tb_aice_t const* aice)
{
	tb_trace_noimpl();
}
tb_void_t tb_aicp_setp(tb_aicp_t* aicp, tb_handle_t handle, tb_pointer_t odata)
{
	tb_trace_noimpl();
}
tb_bool_t tb_aicp_read(tb_aicp_t* aicp, tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_trace_noimpl();
	return TB_FALSE;
}
tb_bool_t tb_aicp_writ(tb_aicp_t* aicp, tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_trace_noimpl();
	return TB_FALSE;
}
tb_bool_t tb_aicp_sync(tb_aicp_t* aicp, tb_handle_t handle)
{
	tb_trace_noimpl();
	return TB_FALSE;
}
tb_bool_t tb_aicp_seek(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t offset)
{
	tb_trace_noimpl();
	return TB_FALSE;
}
tb_bool_t tb_aicp_skip(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t size)
{
	tb_trace_noimpl();
	return TB_FALSE;
}
tb_long_t tb_aicp_wait(tb_aicp_t* aicp, tb_long_t timeout)
{
	tb_trace_noimpl();
	return -1;
}
tb_long_t tb_aicp_spak(tb_aicp_t* aicp)
{
	tb_trace_noimpl();
	return -1;
}
