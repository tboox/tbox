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
 * @file		aico.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aico.h"
#include "aicp.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_aico_init_sock(tb_handle_t aicp, tb_handle_t handle)
{
	// addo
	return tb_aicp_addo(aicp, handle, TB_AICO_TYPE_SOCK);
}
tb_handle_t tb_aico_init_file(tb_handle_t aicp, tb_handle_t handle)
{
	// addo
	return tb_aicp_addo(aicp, handle, TB_AICO_TYPE_FILE);
}
tb_handle_t tb_aico_init_task(tb_handle_t aicp, tb_bool_t bltimer)
{
	// addo, hack: handle != null? using higher precision timer for being compatible with sock/file aico task
	return tb_aicp_addo(aicp, (tb_handle_t)(tb_size_t)!bltimer, TB_AICO_TYPE_TASK);
}
tb_void_t tb_aico_kill(tb_handle_t haico)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return(aico && aico->aicp);

	// kilo
	tb_aicp_kilo(aico->aicp, haico);
}
tb_void_t tb_aico_exit(tb_handle_t haico, tb_bool_t bcalling)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return(aico && aico->aicp);

	// delo
	tb_aicp_delo(aico->aicp, haico, bcalling);
}
tb_handle_t tb_aico_aicp(tb_handle_t haico)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico, tb_null);

	// the aico aicp
	return aico->aicp;
}
tb_size_t tb_aico_type(tb_handle_t haico)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico, TB_AICO_TYPE_NONE);

	// the aico type
	return aico->type;
}
tb_handle_t tb_aico_pool(tb_handle_t haico)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico, tb_null);

	// the pool handle
	return aico->pool;
}
#ifdef __tb_debug__
tb_char_t const* tb_aico_func(tb_handle_t haico)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico, tb_null);

	// the func
	return aico->func;
}
tb_char_t const* tb_aico_file(tb_handle_t haico)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico, tb_null);

	// the file
	return aico->file;
}
tb_size_t tb_aico_line(tb_handle_t haico)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico, 0);

	// the line
	return aico->line;
}
#endif
tb_handle_t tb_aico_handle(tb_handle_t haico)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico, tb_null);

	// the aico handle
	return aico->handle;
}
tb_long_t tb_aico_timeout(tb_handle_t haico, tb_size_t type)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && type < tb_arrayn(aico->timeout), -1);

	// the aico timeout
	return tb_atomic_get((tb_atomic_t*)(aico->timeout + type));
}
tb_void_t tb_aico_timeout_set(tb_handle_t haico, tb_size_t type, tb_long_t timeout)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return(aico && type < tb_arrayn(aico->timeout));

	// set the aico timeout
	tb_atomic_set((tb_atomic_t*)(aico->timeout + type), (tb_atomic_t)timeout);
}
tb_bool_t tb_aico_acpt_impl(tb_handle_t haico, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_ACPT;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_conn_impl(tb_handle_t haico, tb_ipv4_t const* addr, tb_size_t port, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_CONN;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.conn.port 		= port;
	aice.u.conn.addr 		= *addr;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_recv_impl(tb_handle_t haico, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_RECV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.recv.data 		= data;
	aice.u.recv.size 		= size;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_send_impl(tb_handle_t haico, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_SEND;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.send.data 		= data;
	aice.u.send.size 		= size;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_urecv_impl(tb_handle_t haico, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_URECV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.urecv.data 		= data;
	aice.u.urecv.size 		= size;
	aice.u.urecv.port 		= port;
	aice.u.urecv.addr 		= *addr;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_usend_impl(tb_handle_t haico, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_USEND;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.usend.data 		= data;
	aice.u.usend.size 		= size;
	aice.u.usend.port 		= port;
	aice.u.usend.addr 		= *addr;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_recvv_impl(tb_handle_t haico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_RECVV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.recvv.list 		= list;
	aice.u.recvv.size 		= size;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_sendv_impl(tb_handle_t haico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_SENDV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.sendv.list 		= list;
	aice.u.sendv.size 		= size;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_urecvv_impl(tb_handle_t haico, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_URECVV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.urecvv.list 		= list;
	aice.u.urecvv.size 		= size;
	aice.u.urecvv.port 		= port;
	aice.u.urecvv.addr 		= *addr;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_usendv_impl(tb_handle_t haico, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_USENDV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.usendv.list 		= list;
	aice.u.usendv.size 		= size;
	aice.u.usendv.port 		= port;
	aice.u.usendv.addr 		= *addr;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_sendfile_impl(tb_handle_t haico, tb_handle_t file, tb_hize_t seek, tb_hize_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && file, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_SENDFILE;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.sendfile.file 	= file;
	aice.u.sendfile.seek 	= seek;
	aice.u.sendfile.size 	= size;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_read_impl(tb_handle_t haico, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_READ;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.read.seek 		= seek;
	aice.u.read.data 		= data;
	aice.u.read.size 		= size;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_writ_impl(tb_handle_t haico, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_WRIT;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.writ.seek 		= seek;
	aice.u.writ.data 		= data;
	aice.u.writ.size 		= size;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_readv_impl(tb_handle_t haico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_READV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.readv.seek 		= seek;
	aice.u.readv.list 		= list;
	aice.u.readv.size 		= size;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_writv_impl(tb_handle_t haico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_WRITV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.writv.seek 		= seek;
	aice.u.writv.list 		= list;
	aice.u.writv.size 		= size;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_fsync_impl(tb_handle_t haico, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_FSYNC;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_acpt_after_impl(tb_handle_t haico, tb_size_t delay, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_ACPT;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_conn_after_impl(tb_handle_t haico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_CONN;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.conn.port 		= port;
	aice.u.conn.addr 		= *addr;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_recv_after_impl(tb_handle_t haico, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_RECV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.recv.data 		= data;
	aice.u.recv.size 		= size;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_send_after_impl(tb_handle_t haico, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_SEND;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.send.data 		= data;
	aice.u.send.size 		= size;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_urecv_after_impl(tb_handle_t haico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_URECV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.urecv.data 		= data;
	aice.u.urecv.size 		= size;
	aice.u.urecv.port 		= port;
	aice.u.urecv.addr 		= *addr;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_usend_after_impl(tb_handle_t haico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_USEND;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.usend.data 		= data;
	aice.u.usend.size 		= size;
	aice.u.usend.port 		= port;
	aice.u.usend.addr 		= *addr;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_recvv_after_impl(tb_handle_t haico, tb_size_t delay, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_RECVV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.recvv.list 		= list;
	aice.u.recvv.size 		= size;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_sendv_after_impl(tb_handle_t haico, tb_size_t delay, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_SENDV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.sendv.list 		= list;
	aice.u.sendv.size 		= size;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_urecvv_after_impl(tb_handle_t haico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_URECVV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.urecvv.list 		= list;
	aice.u.urecvv.size 		= size;
	aice.u.urecvv.port 		= port;
	aice.u.urecvv.addr 		= *addr;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_usendv_after_impl(tb_handle_t haico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && addr && addr->u32 && port && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_USENDV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.usendv.list 		= list;
	aice.u.usendv.size 		= size;
	aice.u.usendv.port 		= port;
	aice.u.usendv.addr 		= *addr;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_sendfile_after_impl(tb_handle_t haico, tb_size_t delay, tb_handle_t file, tb_hize_t seek, tb_hize_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && file, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_SENDFILE;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.sendfile.file 	= file;
	aice.u.sendfile.seek 	= seek;
	aice.u.sendfile.size 	= size;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_read_after_impl(tb_handle_t haico, tb_size_t delay, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_READ;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.read.seek 		= seek;
	aice.u.read.data 		= data;
	aice.u.read.size 		= size;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_writ_after_impl(tb_handle_t haico, tb_size_t delay, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_WRIT;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.writ.seek 		= seek;
	aice.u.writ.data 		= data;
	aice.u.writ.size 		= size;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_readv_after_impl(tb_handle_t haico, tb_size_t delay, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_READV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.readv.seek 		= seek;
	aice.u.readv.list 		= list;
	aice.u.readv.size 		= size;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_writv_after_impl(tb_handle_t haico, tb_size_t delay, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_WRITV;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.writv.seek 		= seek;
	aice.u.writv.list 		= list;
	aice.u.writv.size 		= size;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_fsync_after_impl(tb_handle_t haico, tb_size_t delay, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_FSYNC;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;

	// post
	return tb_aicp_post_after_impl(aico->aicp, delay, &aice __tb_debug_args__);
}

tb_bool_t tb_aico_task_run_impl(tb_handle_t haico, tb_size_t delay, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__)
{
	// check
	tb_aico_t* aico = (tb_aico_t*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_RUNTASK;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.runtask.when 	= tb_ctime_time() + delay;

	// post
	return tb_aicp_post_impl(aico->aicp, &aice __tb_debug_args__);
}

