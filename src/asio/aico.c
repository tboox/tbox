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
 * addr
 */
static tb_bool_t tb_aico_conn_addr_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_ADDR, tb_false);
	tb_assert_and_check_return_val(aice->aico && aice->aico->aicp, tb_false);
	
	// the aicb & port
	tb_aicb_t aicb = (tb_aicb_t)aice->u.addr.priv[0];
	tb_size_t port = (tb_size_t)aice->u.addr.priv[1];
	tb_assert_and_check_return_val(aicb && port, tb_false);

	// init
	tb_aice_t 				conn = {0};
	conn.code 				= TB_AICE_CODE_CONN;
	conn.state 				= aice->state == TB_AICE_STATE_OK? TB_AICE_STATE_PENDING : aice->state;
	conn.aicb 				= aicb;
	conn.data 				= aice->data;
	conn.aico 				= aice->aico;
	conn.u.conn.port 		= port;
	conn.u.conn.addr 		= aice->u.addr.addr;

	// post conn
	return tb_aicp_post(aice->aico->aicp, &conn);
}
static tb_bool_t tb_aico_urecv_addr_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_ADDR, tb_false);
	tb_assert_and_check_return_val(aice->aico && aice->aico->aicp, tb_false);
	
	// the aicb & port & data & size
	tb_aicb_t 	aicb = (tb_aicb_t)aice->u.addr.priv[0];
	tb_size_t 	port = (tb_size_t)aice->u.addr.priv[1];
	tb_byte_t* 	data = (tb_byte_t*)aice->u.addr.priv[2];
	tb_size_t 	size = (tb_size_t)aice->u.addr.priv[3];
	tb_assert_and_check_return_val(aicb && port && data && size, tb_false);

	// init
	tb_aice_t 				urecv = {0};
	urecv.code 				= TB_AICE_CODE_URECV;
	urecv.state 			= aice->state == TB_AICE_STATE_OK? TB_AICE_STATE_PENDING : aice->state;
	urecv.aicb 				= aicb;
	urecv.data 				= aice->data;
	urecv.aico 				= aice->aico;
	urecv.u.urecv.port 		= port;
	urecv.u.urecv.addr 		= aice->u.addr.addr;
	urecv.u.urecv.data 		= data;
	urecv.u.urecv.size 		= size;

	// post conn
	return tb_aicp_post(aice->aico->aicp, &urecv);
}
static tb_bool_t tb_aico_usend_addr_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_ADDR, tb_false);
	tb_assert_and_check_return_val(aice->aico && aice->aico->aicp, tb_false);

	// the aicb & port & data & size
	tb_aicb_t 	aicb = (tb_aicb_t)aice->u.addr.priv[0];
	tb_size_t 	port = (tb_size_t)aice->u.addr.priv[1];
	tb_byte_t* 	data = (tb_byte_t*)aice->u.addr.priv[2];
	tb_size_t 	size = (tb_size_t)aice->u.addr.priv[3];
	tb_assert_and_check_return_val(aicb && port && data && size, tb_false);

	// init
	tb_aice_t 				usend = {0};
	usend.code 				= TB_AICE_CODE_USEND;
	usend.state 			= aice->state == TB_AICE_STATE_OK? TB_AICE_STATE_PENDING : aice->state;
	usend.aicb 				= aicb;
	usend.data 				= aice->data;
	usend.aico 				= aice->aico;
	usend.u.usend.port 		= port;
	usend.u.usend.addr 		= aice->u.addr.addr;
	usend.u.usend.data 		= data;
	usend.u.usend.size 		= size;

	// post conn
	return tb_aicp_post(aice->aico->aicp, &usend);
}

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
tb_handle_t tb_aico_init_task(tb_handle_t aicp)
{
	// addo
	return tb_aicp_addo(aicp, tb_null, TB_AICO_TYPE_TASK);
}
tb_void_t tb_aico_exit(tb_handle_t haico)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return(aico && aico->aicp);

	// delo
	tb_aicp_delo(aico->aicp, haico);
}
tb_handle_t tb_aico_aicp(tb_handle_t haico)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return_val(aico, tb_null);

	// the aico aicp
	return aico->aicp;
}
tb_size_t tb_aico_type(tb_handle_t haico)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return_val(aico, TB_AICO_TYPE_NONE);

	// the aico type
	return aico->type;
}
tb_handle_t tb_aico_handle(tb_handle_t haico)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return_val(aico, tb_null);

	// the aico handle
	return aico->handle;
}
tb_long_t tb_aico_timeout(tb_handle_t haico, tb_size_t type)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
tb_bool_t tb_aico_addr(tb_handle_t haico, tb_char_t const* host, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && host, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_ADDR;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.addr.host 		= host;

	// post
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_acpt(tb_handle_t haico, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_ACPT;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;

	// post
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_conn(tb_handle_t haico, tb_char_t const* host, tb_size_t port, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && host && port, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_CONN;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.conn.port 		= port;

	// name? get the address first
	if (!tb_ipv4_set(&aice.u.conn.addr, host))
	{
		// post addr and conn it
		aice.code 			= TB_AICE_CODE_ADDR;
		aice.aicb 			= tb_aico_conn_addr_func;
		aice.data 			= aicb_data;
		aice.u.addr.priv[0] = (tb_pointer_t)aicb_func;
		aice.u.addr.priv[1] = (tb_pointer_t)port;
		aice.u.addr.host 	= host;
	}

	// post
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_recv(tb_handle_t haico, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_send(tb_handle_t haico, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_urecv(tb_handle_t haico, tb_char_t const* host, tb_size_t port, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && host && port && data && size, tb_false);

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

	// name? get the address first
	if (!tb_ipv4_set(&aice.u.urecv.addr, host))
	{
		// post addr and urecv it
		aice.code 			= TB_AICE_CODE_ADDR;
		aice.aicb 			= tb_aico_urecv_addr_func;
		aice.data 			= aicb_data;
		aice.u.addr.priv[0] = (tb_pointer_t)aicb_func;
		aice.u.addr.priv[1] = (tb_pointer_t)port;
		aice.u.addr.priv[2] = (tb_pointer_t)data;
		aice.u.addr.priv[3] = (tb_pointer_t)size;
		aice.u.addr.host 	= host;
	}

	// post
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_usend(tb_handle_t haico, tb_char_t const* host, tb_size_t port, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp && host && port && data && size, tb_false);

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

	// name? get the address first
	if (!tb_ipv4_set(&aice.u.usend.addr, host))
	{
		// post addr and urecv it
		aice.code 			= TB_AICE_CODE_ADDR;
		aice.aicb 			= tb_aico_usend_addr_func;
		aice.data 			= aicb_data;
		aice.u.addr.priv[0] = (tb_pointer_t)aicb_func;
		aice.u.addr.priv[1] = (tb_pointer_t)port;
		aice.u.addr.priv[2] = (tb_pointer_t)data;
		aice.u.addr.priv[3] = (tb_pointer_t)size;
		aice.u.addr.host 	= host;
	}

	// post
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_recvv(tb_handle_t haico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_sendv(tb_handle_t haico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_sendfile(tb_handle_t haico, tb_handle_t file, tb_hize_t seek, tb_hize_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_read(tb_handle_t haico, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_writ(tb_handle_t haico, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_readv(tb_handle_t haico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_writv(tb_handle_t haico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_fsync(tb_handle_t haico, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
	tb_assert_and_check_return_val(aico && aico->aicp, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_FSYNC;
	aice.state 				= TB_AICE_STATE_PENDING;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;

	// post
	return tb_aicp_post(aico->aicp, &aice);
}
tb_bool_t tb_aico_task_run(tb_handle_t haico, tb_size_t delay, tb_aicb_t aicb_func, tb_pointer_t aicb_data)
{
	// check
	tb_aico_t const* aico = (tb_aico_t const*)haico;
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
	return tb_aicp_post(aico->aicp, &aice);
}

