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
 * @file		sock.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 				"asock"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the sock stream type
typedef struct __tb_astream_sock_t
{
	// the base
	tb_astream_t 				base;

	// the sock handle
	tb_handle_t 				sock;

	// the aico
	tb_handle_t 				aico;

	// the sock type
	tb_size_t 					type : 23;

	// the sock bref
	tb_size_t 					bref : 1;

	// the sock data
	tb_byte_t 					data[8192];

	// the func
	union
	{
		tb_astream_read_func_t 	read;
		tb_astream_writ_func_t 	writ;

	} 							func;

	// the priv
	tb_pointer_t 				priv;

}tb_astream_sock_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_astream_sock_t* tb_astream_sock_cast(tb_astream_t* ast)
{
	tb_assert_and_check_return_val(ast && ast->type == TB_ASTREAM_TYPE_SOCK, tb_null);
	return (tb_astream_sock_t*)ast;
}
static tb_bool_t tb_astream_sock_open(tb_astream_t* ast, tb_astream_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sst = tb_astream_sock_cast(ast);
	tb_assert_and_check_return_val(sst && func, tb_false);


	// ok
	return tb_true;
}
static tb_bool_t tb_astream_sock_read_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RECV, tb_false);

	// the stream
	tb_astream_sock_t* sst = (tb_astream_sock_t*)aice->data;
	tb_assert_and_check_return_val(sst && sst->func.read, tb_false);

	// done state
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	switch (aice->state)
	{
		// ok
	case TB_AICE_STATE_OK:
		tb_assert_and_check_break(aice->u.recv.real && aice->u.recv.real <= sizeof(sst->data));
		state = TB_ASTREAM_STATE_OK;
		break;
		// closed
	case TB_AICE_STATE_CLOSED:
		state = TB_ASTREAM_STATE_CLOSED;
		break;
		// timeout?
	case TB_AICE_STATE_TIMEOUT:
		state = TB_ASTREAM_SOCK_STATE_RECV_TIMEOUT;
		break;
	default:
		tb_trace_impl("read: unknown state: %s", tb_aice_state_cstr(aice));
		break;
	}

	// done func
	if (sst->func.read((tb_astream_t*)sst, state, aice->u.recv.data, aice->u.recv.real, sst->priv))
	{
		// continue?
		if (aice->state == TB_AICE_STATE_OK)
		{
			// continue to post read
			tb_aico_recv(aice->aico, sst->data, sizeof(sst->data), tb_astream_sock_read_func, (tb_astream_t*)sst);
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_sock_read(tb_astream_t* ast, tb_astream_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sst = tb_astream_sock_cast(ast);
	tb_assert_and_check_return_val(sst && sst->sock && sst->aico && func, tb_false);

	// save func and priv
	sst->priv 		= priv;
	sst->func.read 	= func;

	// post read
	return tb_aico_recv(sst->aico, sst->data, sizeof(sst->data), tb_astream_sock_read_func, ast);
}
static tb_bool_t tb_astream_sock_writ_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_SEND, tb_false);

	// the stream
	tb_astream_sock_t* sst = (tb_astream_sock_t*)aice->data;
	tb_assert_and_check_return_val(sst && sst->func.writ, tb_false);

	// done state
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	switch (aice->state)
	{
		// ok
	case TB_AICE_STATE_OK:
		tb_assert_and_check_break(aice->u.send.data && aice->u.send.real && aice->u.send.real <= aice->u.send.size);
		state = TB_ASTREAM_STATE_OK;
		break;
		// closed
	case TB_AICE_STATE_CLOSED:
		state = TB_ASTREAM_STATE_CLOSED;
		break;
		// timeout?
	case TB_AICE_STATE_TIMEOUT:
		state = TB_ASTREAM_SOCK_STATE_SEND_TIMEOUT;
		break;
	default:
		tb_trace_impl("writ: unknown state: %s", tb_aice_state_cstr(aice));
		break;
	}

	// done func
	if (sst->func.writ((tb_astream_t*)sst, state, aice->u.send.real, aice->u.send.size, sst->priv))
	{
		// continue?
		if (aice->state == TB_AICE_STATE_OK && aice->u.send.real < aice->u.send.size)
		{
			// continue to post writ
			tb_aico_send(aice->aico, aice->u.send.data + aice->u.send.real, aice->u.send.size - aice->u.send.real, tb_astream_sock_writ_func, (tb_astream_t*)sst);
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_sock_writ(tb_astream_t* ast, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sst = tb_astream_sock_cast(ast);
	tb_assert_and_check_return_val(sst && sst->sock && sst->aico && data && size && func, tb_false);

	// save func and priv
	sst->priv 		= priv;
	sst->func.writ 	= func;

	// post writ
	return tb_aico_send(sst->aico, data, size, tb_astream_sock_writ_func, ast);
}
static tb_bool_t tb_astream_sock_save(tb_astream_t* ast, tb_astream_t* ost, tb_astream_save_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
static tb_bool_t tb_astream_sock_seek(tb_astream_t* ast, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sst = tb_astream_sock_cast(ast);
	tb_assert_and_check_return_val(sst && func, tb_false);

	// done func
	func(ast, TB_ASTREAM_STATE_NOT_SUPPORTED, priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_sock_sync(tb_astream_t* ast, tb_astream_sync_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sst = tb_astream_sock_cast(ast);
	tb_assert_and_check_return_val(sst && sst->sock && sst->aico && func, tb_false);

	// done func
	func(ast, TB_ASTREAM_STATE_OK, priv);

	// ok
	return tb_true;
}
static tb_void_t tb_astream_sock_kill(tb_astream_t* ast)
{	
	// check
	tb_astream_sock_t* sst = tb_astream_sock_cast(ast);
	tb_assert_and_check_return(sst);

	// is pending?
	if (sst->aico && tb_aico_pending(sst->aico))
	{
		// kill it
		if (!sst->bref && sst->sock) tb_socket_kill(sst->sock, TB_SOCKET_KILL_RW);
	}
}
static tb_void_t tb_astream_sock_exit(tb_astream_t* ast)
{	
	// check
	tb_astream_sock_t* sst = tb_astream_sock_cast(ast);
	tb_assert_and_check_return(sst);

	// exit aico
	if (sst->aico) tb_aico_exit(sst->aico);
	sst->aico = tb_null;

	// exit it
	if (!sst->bref && sst->sock) tb_socket_close(sst->sock);
	sst->sock = tb_null;
	sst->bref = 0;
}
static tb_bool_t tb_astream_sock_ctrl(tb_astream_t* ast, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_astream_sock_t* sst = tb_astream_sock_cast(ast);
	tb_assert_and_check_return_val(sst, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_ASTREAM_CTRL_SOCK_SET_TYPE:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened), tb_false);

			// set type
			tb_size_t type = (tb_size_t)tb_va_arg(args, tb_size_t);
			tb_assert_and_check_return_val(type == TB_SOCKET_TYPE_TCP || type == TB_SOCKET_TYPE_UDP, tb_false);
			sst->type = type;
			return tb_true;
		}
	case TB_ASTREAM_CTRL_SOCK_SET_HANDLE:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened), tb_false);

			// set handle
			tb_handle_t handle = (tb_handle_t)tb_va_arg(args, tb_handle_t);
			sst->sock = handle;
			sst->bref = handle? 1 : 0;
			return tb_true;
		}
	case TB_ASTREAM_CTRL_SOCK_GET_HANDLE:
		{
			// get handle
			tb_handle_t* phandle = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
			tb_assert_and_check_return_val(phandle, tb_false);
			*phandle = sst->sock;
			return tb_true;
		}
	default:
		break;
	}
	return tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_astream_t* tb_astream_init_sock(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp, tb_null);

	// make stream
	tb_astream_sock_t* ast = (tb_astream_sock_t*)tb_malloc0(sizeof(tb_astream_sock_t));
	tb_assert_and_check_return_val(ast, tb_null);

	// init stream
	if (!tb_astream_init((tb_astream_t*)ast, aicp, TB_ASTREAM_TYPE_SOCK)) goto fail;
	ast->base.open 		= tb_astream_sock_open;
	ast->base.read 		= tb_astream_sock_read;
	ast->base.writ 		= tb_astream_sock_writ;
	ast->base.save 		= tb_astream_sock_save;
	ast->base.seek 		= tb_astream_sock_seek;
	ast->base.sync 		= tb_astream_sock_sync;
	ast->base.kill 		= tb_astream_sock_kill;
	ast->base.exit 		= tb_astream_sock_exit;
	ast->base.ctrl 		= tb_astream_sock_ctrl;

	// ok
	return (tb_astream_t*)ast;

fail:
	if (ast) tb_astream_exit((tb_astream_t*)ast);
	return tb_null;
}
tb_astream_t* tb_astream_init_from_sock(tb_aicp_t* aicp, tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
	// check
	tb_assert_and_check_return_val(aicp && host && port, tb_null);

	// ssl is not supported now.
	tb_assert_and_check_return_val(!bssl, tb_null);

	// init stream
	tb_astream_t* ast = tb_astream_init_sock(aicp);
	tb_assert_and_check_return_val(ast, tb_null);

	// ctrl
	if (!tb_astream_ctrl(ast, TB_ASTREAM_CTRL_SET_HOST, host)) goto fail;
	if (!tb_astream_ctrl(ast, TB_ASTREAM_CTRL_SET_PORT, port)) goto fail;
	if (!tb_astream_ctrl(ast, TB_ASTREAM_CTRL_SOCK_SET_TYPE, type)) goto fail;
	
	// ok
	return ast;
fail:
	if (ast) tb_astream_exit(ast);
	return tb_null;
}
