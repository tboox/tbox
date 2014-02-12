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

	// the addr
	tb_handle_t 				addr;

	// the sock type
	tb_size_t 					type : 23;

	// the sock bref
	tb_size_t 					bref : 1;

	// the sock data
	tb_byte_t* 					data;

	// the sock maxn
	tb_size_t 					maxn;

	// the func
	union
	{
		tb_astream_open_func_t 	open;
		tb_astream_read_func_t 	read;
		tb_astream_writ_func_t 	writ;
		tb_astream_task_func_t 	task;

	} 							func;

	// the priv
	tb_pointer_t 				priv;

}tb_astream_sock_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_astream_sock_t* tb_astream_sock_cast(tb_astream_t* astream)
{
	tb_assert_and_check_return_val(astream && astream->type == TB_ASTREAM_TYPE_SOCK, tb_null);
	return (tb_astream_sock_t*)astream;
}
static tb_bool_t tb_astream_sock_conn_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_CONN, tb_false);

	// the stream
	tb_astream_sock_t* sstream = tb_astream_sock_cast(aice->data);
	tb_assert_and_check_return_val(sstream && sstream->func.open, tb_false);

	// done
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	switch (aice->state)
	{
		// ok
	case TB_AICE_STATE_OK:
		{
			// opened
			tb_atomic_set(&sstream->base.opened, 1);

			// ok
			state = TB_ASTREAM_STATE_OK;
		}
		break;
		// timeout
	case TB_AICE_STATE_TIMEOUT:
		state = TB_ASTREAM_SOCK_STATE_CONNECT_TIMEOUT;
		break;
		// killed
	case TB_AICE_STATE_KILLED:
		state = TB_ASTREAM_STATE_KILLED;
		break;
		// failed
	default:
		state = TB_ASTREAM_SOCK_STATE_CONNECT_FAILED;
		break;
	}

	// done func
	sstream->func.open((tb_astream_t*)sstream, state, sstream->priv);

	// ok
	return tb_true;
}
static tb_void_t tb_astream_sock_addr_func(tb_handle_t haddr, tb_ipv4_t const* addr, tb_pointer_t data)
{
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(data);
	tb_assert_and_check_return(haddr && sstream && sstream->func.open);

	// done
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	do
	{
		// addr ok?
		if (addr)
		{
			// trace
			tb_trace_impl("addr[%s]: %u.%u.%u.%u", tb_aicp_addr_host(haddr), tb_ipv4_u8x4(*addr));

			// init sock
			if (!sstream->sock) 
			{
				sstream->sock = tb_socket_open(sstream->type);
				sstream->bref = 0;
			}
			tb_assert_and_check_break(sstream->sock);

			// init maxn
			if (!sstream->maxn) sstream->maxn = tb_socket_recv_buffer_size(sstream->sock);
			tb_assert_and_check_break(sstream->maxn);

			// init data
			if (!sstream->data) sstream->data = tb_malloc0(sstream->maxn);
			tb_assert_and_check_break(sstream->data);

			// init aico
			if (!sstream->aico) sstream->aico = tb_aico_init_sock(sstream->base.aicp, sstream->sock);
			tb_assert_and_check_break(sstream->aico);

			// init timeout
			tb_aico_timeout_set(sstream->aico, TB_AICO_TIMEOUT_CONN, sstream->base.timeout);
			tb_aico_timeout_set(sstream->aico, TB_AICO_TIMEOUT_RECV, sstream->base.timeout);
			tb_aico_timeout_set(sstream->aico, TB_AICO_TIMEOUT_SEND, sstream->base.timeout);

			// port
			tb_size_t port = tb_url_port_get(&sstream->base.url);
			tb_assert_and_check_break(port);
			
			// done conn
			if (!tb_aico_conn(sstream->aico, addr, port, tb_astream_sock_conn_func, sstream)) break;

			// ok
			state = TB_ASTREAM_STATE_OK;
		}
		// timeout or failed?
		else
		{
			// trace
			tb_trace_impl("addr[%s]: failed", tb_aicp_addr_host(haddr));

			// dns failed
			state = TB_ASTREAM_SOCK_STATE_DNS_FAILED;
		}

	} while (0);

	// done func if failed
	if (state != TB_ASTREAM_STATE_OK) sstream->func.open((tb_astream_t*)sstream, state, sstream->priv);
}
static tb_bool_t tb_astream_sock_open(tb_astream_t* astream, tb_astream_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return_val(sstream && sstream->type && func, tb_false);

	// get the host from url
	tb_char_t const* host = tb_url_host_get(&astream->url);
	tb_assert_and_check_return_val(host, tb_false);

	// init addr
	if (!sstream->addr) sstream->addr = tb_aicp_addr_init(astream->aicp, sstream->base.timeout, tb_astream_sock_addr_func, astream);
	tb_assert_and_check_return_val(sstream->addr, tb_false);

	// save func and priv
	sstream->priv 		= priv;
	sstream->func.open 	= func;

	// done addr
	return tb_aicp_addr_done(sstream->addr, host);
}
static tb_bool_t tb_astream_sock_read_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RECV, tb_false);

	// the stream
	tb_astream_sock_t* sstream = (tb_astream_sock_t*)aice->data;
	tb_assert_and_check_return_val(sstream && sstream->maxn && sstream->func.read, tb_false);
 
	// done state
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	switch (aice->state)
	{
		// ok
	case TB_AICE_STATE_OK:
		tb_assert_and_check_break(aice->u.recv.real <= sstream->maxn);
		state = TB_ASTREAM_STATE_OK;
		break;
		// closed
	case TB_AICE_STATE_CLOSED:
		state = TB_ASTREAM_STATE_CLOSED;
		break;
		// killed
	case TB_AICE_STATE_KILLED:
		state = TB_ASTREAM_STATE_KILLED;
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
	if (sstream->func.read((tb_astream_t*)sstream, state, aice->u.recv.data, aice->u.recv.real, aice->u.recv.size, sstream->priv))
	{
		// continue?
		if (aice->state == TB_AICE_STATE_OK)
		{
			// continue to post read
			tb_aico_recv(aice->aico, sstream->data, aice->u.recv.size, tb_astream_sock_read_func, (tb_astream_t*)sstream);
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_sock_read(tb_astream_t* astream, tb_size_t delay, tb_size_t maxn, tb_astream_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return_val(sstream && sstream->sock && sstream->maxn && sstream->aico && func, tb_false);

	// save func and priv
	sstream->priv 		= priv;
	sstream->func.read 	= func;
	if (!maxn || maxn > sstream->maxn) maxn = sstream->maxn;

	// post read
	return tb_aico_recv_after(sstream->aico, delay, sstream->data, maxn, tb_astream_sock_read_func, astream);
}
static tb_bool_t tb_astream_sock_writ_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_SEND, tb_false);

	// the stream
	tb_astream_sock_t* sstream = (tb_astream_sock_t*)aice->data;
	tb_assert_and_check_return_val(sstream && sstream->func.writ, tb_false);

	// done state
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	switch (aice->state)
	{
		// ok
	case TB_AICE_STATE_OK:
		tb_assert_and_check_break(aice->u.send.data && aice->u.send.real <= aice->u.send.size);
		state = TB_ASTREAM_STATE_OK;
		break;
		// closed
	case TB_AICE_STATE_CLOSED:
		state = TB_ASTREAM_STATE_CLOSED;
		break;
		// killed
	case TB_AICE_STATE_KILLED:
		state = TB_ASTREAM_STATE_KILLED;
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
	if (sstream->func.writ((tb_astream_t*)sstream, state, aice->u.send.data, aice->u.send.real, aice->u.send.size, sstream->priv))
	{
		// continue?
		if (aice->state == TB_AICE_STATE_OK && aice->u.send.real < aice->u.send.size)
		{
			// continue to post writ
			tb_aico_send(aice->aico, aice->u.send.data + aice->u.send.real, aice->u.send.size - aice->u.send.real, tb_astream_sock_writ_func, (tb_astream_t*)sstream);
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_sock_writ(tb_astream_t* astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return_val(sstream && sstream->sock && sstream->aico && data && size && func, tb_false);

	// save func and priv
	sstream->priv 		= priv;
	sstream->func.writ 	= func;

	// post writ
	return tb_aico_send_after(sstream->aico, delay, data, size, tb_astream_sock_writ_func, astream);
}
static tb_bool_t tb_astream_sock_seek(tb_astream_t* astream, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return_val(sstream && func, tb_false);

	// done func
	func(astream, TB_ASTREAM_STATE_NOT_SUPPORTED, 0, priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_sock_sync(tb_astream_t* astream, tb_bool_t bclosing, tb_astream_sync_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return_val(sstream && sstream->sock && sstream->aico && func, tb_false);

	// done func
	func(astream, TB_ASTREAM_STATE_OK, priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_sock_task_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

	// the stream
	tb_astream_sock_t* sstream = (tb_astream_sock_t*)aice->data;
	tb_assert_and_check_return_val(sstream && sstream->func.task, tb_false);

	// done func
	tb_bool_t ok = sstream->func.task((tb_astream_t*)sstream, aice->state == TB_AICE_STATE_OK? TB_ASTREAM_STATE_OK : TB_ASTREAM_STATE_UNKNOWN_ERROR, sstream->priv);

	// ok and continue?
	if (ok && aice->state == TB_AICE_STATE_OK)
	{
		// post task
		tb_aico_task_run(aice->aico, aice->u.runtask.delay, tb_astream_sock_task_func, sstream);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_sock_task(tb_astream_t* astream, tb_size_t delay, tb_astream_task_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return_val(sstream && sstream->sock && sstream->aico && func, tb_false);

	// save func and priv
	sstream->priv 		= priv;
	sstream->func.task 	= func;

	// post task
	return tb_aico_task_run(sstream->aico, delay, tb_astream_sock_task_func, astream);
}
static tb_void_t tb_astream_sock_kill(tb_astream_t* astream)
{	
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return(sstream);

	// kill it
	if (sstream->aico) tb_aico_kill(sstream->aico);
}
static tb_void_t tb_astream_sock_clos(tb_astream_t* astream, tb_bool_t bcalling)
{	
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return(sstream);

	// exit aico
	if (sstream->aico) tb_aico_exit(sstream->aico, bcalling);
	sstream->aico = tb_null;

	// exit addr
	if (sstream->addr) tb_aicp_addr_exit(sstream->addr, bcalling);
	sstream->addr = tb_null;

	// exit it
	if (!sstream->bref && sstream->sock) tb_socket_clos(sstream->sock);
	sstream->sock = tb_null;
	sstream->bref = 0;
}
static tb_void_t tb_astream_sock_exit(tb_astream_t* astream, tb_bool_t bcalling)
{	
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return(sstream);

	// exit data
	if (sstream->data) tb_free(sstream->data);
	sstream->data = tb_null;
}
static tb_bool_t tb_astream_sock_ctrl(tb_astream_t* astream, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_astream_sock_t* sstream = tb_astream_sock_cast(astream);
	tb_assert_and_check_return_val(sstream, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_ASTREAM_CTRL_SOCK_SET_TYPE:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&astream->opened), tb_false);

			// the type
			tb_size_t type = (tb_size_t)tb_va_arg(args, tb_size_t);
			tb_assert_and_check_return_val(type == TB_SOCKET_TYPE_TCP || type == TB_SOCKET_TYPE_UDP, tb_false);

			// changed? exit the old sock
			if (sstream->type != type)
			{
				// exit aico
				if (sstream->aico) tb_aico_exit(sstream->aico, tb_false);
				sstream->aico = tb_null;

				// exit it
				if (!sstream->bref && sstream->sock) tb_socket_clos(sstream->sock);
				sstream->sock = tb_null;
				sstream->bref = 0;
			}

			// set type
			sstream->type = type;

			// ok
			return tb_true;
		}
	case TB_ASTREAM_CTRL_SOCK_SET_HANDLE:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&astream->opened), tb_false);

			// the sock
			tb_handle_t sock = (tb_handle_t)tb_va_arg(args, tb_handle_t);

			// changed? exit the old sock
			if (sstream->sock != sock)
			{
				// exit aico
				if (sstream->aico) tb_aico_exit(sstream->aico, tb_false);
				sstream->aico = tb_null;

				// exit it
				if (!sstream->bref && sstream->sock) tb_socket_clos(sstream->sock);
			}

			// set sock
			sstream->sock = sock;
			sstream->bref = sock? 1 : 0;

			// ok
			return tb_true;
		}
	case TB_ASTREAM_CTRL_SOCK_GET_HANDLE:
		{
			// get handle
			tb_handle_t* phandle = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
			tb_assert_and_check_return_val(phandle, tb_false);
			*phandle = sstream->sock;
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
	tb_astream_sock_t* sstream = (tb_astream_sock_t*)tb_malloc0(sizeof(tb_astream_sock_t));
	tb_assert_and_check_return_val(sstream, tb_null);

	// init stream
	if (!tb_astream_init((tb_astream_t*)sstream, aicp, TB_ASTREAM_TYPE_SOCK)) goto fail;
	sstream->base.open 		= tb_astream_sock_open;
	sstream->base.read 		= tb_astream_sock_read;
	sstream->base.writ 		= tb_astream_sock_writ;
	sstream->base.seek 		= tb_astream_sock_seek;
	sstream->base.sync 		= tb_astream_sock_sync;
	sstream->base.task 		= tb_astream_sock_task;
	sstream->base.kill 		= tb_astream_sock_kill;
	sstream->base.clos 		= tb_astream_sock_clos;
	sstream->base.exit 		= tb_astream_sock_exit;
	sstream->base.ctrl 		= tb_astream_sock_ctrl;
	sstream->type 			= TB_SOCKET_TYPE_TCP;

	// ok
	return (tb_astream_t*)sstream;

fail:
	if (sstream) tb_astream_exit((tb_astream_t*)sstream, tb_false);
	return tb_null;
}
tb_astream_t* tb_astream_init_from_sock(tb_aicp_t* aicp, tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
	// check
	tb_assert_and_check_return_val(aicp && host && port, tb_null);

	// ssl is not supported now.
	tb_assert_and_check_return_val(!bssl, tb_null);

	// init stream
	tb_astream_t* sstream = tb_astream_init_sock(aicp);
	tb_assert_and_check_return_val(sstream, tb_null);

	// ctrl
	if (!tb_astream_ctrl(sstream, TB_ASTREAM_CTRL_SET_HOST, host)) goto fail;
	if (!tb_astream_ctrl(sstream, TB_ASTREAM_CTRL_SET_PORT, port)) goto fail;
	if (!tb_astream_ctrl(sstream, TB_ASTREAM_CTRL_SOCK_SET_TYPE, type)) goto fail;
	
	// ok
	return sstream;
fail:
	if (sstream) tb_astream_exit(sstream, tb_false);
	return tb_null;
}
