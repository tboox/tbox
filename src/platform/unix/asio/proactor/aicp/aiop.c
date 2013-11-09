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
 * @file		aiop.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#undef TB_TRACE_IMPL_TAG
#define TB_TRACE_IMPL_TAG 				"aiop"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aiop mutx type
typedef struct __tb_aiop_mutx_t
{
	// the pool mutx
	tb_handle_t 				pool;

	// the resp mutx
	tb_handle_t 				resp;

}tb_aiop_mutx_t;

// the aiop proactor type
typedef struct __tb_aicp_proactor_aiop_t
{
	// the proactor base
	tb_aicp_proactor_t 			base;

	// the unix proactor
	tb_aicp_proactor_t* 		uptr;

	// the proactor indx
	tb_size_t 					indx;

	// the aiop
	tb_aiop_t* 					aiop;

	// the mutx
	tb_aiop_mutx_t 				mutx;

	// the aice pool
	tb_handle_t 				pool;

	// the aice resp
	tb_queue_t* 				resp;
	
	// the spak loop
	tb_handle_t 				loop;

	// the aioe list
	tb_aioe_t* 					list;

	// the aioe size
	tb_size_t 					maxn;

}tb_aicp_proactor_aiop_t;

/* ///////////////////////////////////////////////////////////////////////
 * aice
 */
static tb_aice_t* tb_aiop_aice_init(tb_aicp_proactor_aiop_t* ptor)
{
	// check
	tb_assert_and_check_return_val(ptor && ptor->pool, tb_null);

	// enter 
	if (ptor->mutx.pool) tb_mutex_enter(ptor->mutx.pool);

	// make data
	tb_pointer_t data = tb_rpool_malloc0(ptor->pool);

	// leave 
	if (ptor->mutx.pool) tb_mutex_leave(ptor->mutx.pool);
	
	// ok?
	return (tb_aice_t*)data;
}
static tb_void_t tb_aiop_aice_exit(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* data)
{
	// check
	tb_assert_and_check_return(ptor && ptor->pool);

	// enter 
	if (ptor->mutx.pool) tb_mutex_enter(ptor->mutx.pool);

	// free data
	if (data) tb_rpool_free(ptor->pool, data);

	// leave 
	if (ptor->mutx.pool) tb_mutex_leave(ptor->mutx.pool);
}

/* ///////////////////////////////////////////////////////////////////////
 * post
 */
static tb_bool_t tb_aiop_post_acpt(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice, tb_aioe_t* aioe)
{	
	// check
	tb_assert_and_check_return_val(ptor && aice && aioe, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice->handle && aice->code == TB_AICE_CODE_ACPT, tb_false);

	// done
	tb_bool_t 	ok = tb_false;
	tb_aice_t* 	reqt = tb_null;
	do
	{
		// make reqt
		reqt = tb_aiop_aice_init(ptor);
		tb_assert_and_check_break(reqt);

		// init reqt
		*reqt = *aice;

		// init aioe
		aioe->code = TB_AIOE_CODE_ACPT | TB_AIOE_CODE_ONESHOT;
		aioe->data = reqt;
		aioe->handle = aice->handle;

		// ok
		ok = tb_true;

	} while (0);

	// error?
	if (!ok)
	{
		if (reqt) tb_aiop_aice_exit(ptor, reqt);
	}

	// ok?
	return ok;
}
static tb_bool_t tb_aiop_post_conn(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice, tb_aioe_t* aioe)
{	
	// check
	tb_assert_and_check_return_val(ptor && aice && aioe, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice->handle && aice->code == TB_AICE_CODE_CONN, tb_false);
	tb_assert_and_check_return_val(aice->u.conn.host && aice->u.conn.port, tb_false);

	// try to connect it
	tb_long_t ok = tb_socket_connect(aice->handle, aice->u.conn.host, aice->u.conn.port);

	// trace
	tb_trace_impl("conn[%p]: %ld", aice->handle, ok);

	// ok?
	if (ok > 0)
	{
		// post resp
		tb_aicp_proactor_unix_resp(ptor->uptr, aice);
	}
	// pending? continue it
	else if (!ok)
	{
		// done
		tb_bool_t 	done = tb_false;
		tb_aice_t* 	reqt = tb_null;
		do
		{
			// make reqt
			reqt = tb_aiop_aice_init(ptor);
			tb_assert_and_check_break(reqt);

			// init reqt
			*reqt = *aice;

			// init aioe
			aioe->code = TB_AIOE_CODE_CONN | TB_AIOE_CODE_ONESHOT;
			aioe->data = reqt;
			aioe->handle = aice->handle;

			// ok
			done = tb_true;

		} while (0);

		// error?
		if (!done)
		{
			if (reqt) tb_aiop_aice_exit(ptor, reqt);
			return tb_false;
		}
	}
	// failed?
	else
	{
		// post failed
		tb_aice_t resp = *aice;
		resp.state = TB_AICE_STATE_FAILED;
		tb_aicp_proactor_unix_resp(ptor->uptr, &resp);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_post_recv(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice, tb_aioe_t* aioe)
{	
	// check
	tb_assert_and_check_return_val(ptor && aice && aioe, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice->handle && aice->code == TB_AICE_CODE_RECV, tb_false);
	tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size, tb_false);

	// done
	tb_bool_t 	ok = tb_false;
	tb_aice_t* 	reqt = tb_null;
	do
	{
		// make reqt
		reqt = tb_aiop_aice_init(ptor);
		tb_assert_and_check_break(reqt);

		// init reqt
		*reqt = *aice;

		// init aioe
		aioe->code = TB_AIOE_CODE_RECV | TB_AIOE_CODE_ONESHOT;
		aioe->data = reqt;
		aioe->handle = aice->handle;

		// ok
		ok = tb_true;

	} while (0);

	// error?
	if (!ok)
	{
		if (reqt) tb_aiop_aice_exit(ptor, reqt);
	}

	// ok?
	return ok;
}
static tb_bool_t tb_aiop_post_send(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice, tb_aioe_t* aioe)
{	
	// check
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_SEND, tb_false);
	tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size, tb_false);

	// try to send it
	tb_long_t real = tb_socket_send(aice->handle, aice->u.send.data, aice->u.send.size);

	// trace
	tb_trace_impl("send[%p]: %ld", aice->handle, real);

	// ok?
	if (real > 0)
	{
		// post resp
		tb_aice_t resp = *aice;
		resp.u.send.real = real;
		tb_aicp_proactor_unix_resp(ptor->uptr, &resp);
	}
	// pending? continue it
	else if (!real)
	{
		// done
		tb_bool_t 	done = tb_false;
		tb_aice_t* 	reqt = tb_null;
		do
		{
			// make reqt
			reqt = tb_aiop_aice_init(ptor);
			tb_assert_and_check_break(reqt);

			// init reqt
			*reqt = *aice;

			// init aioe
			aioe->code = TB_AIOE_CODE_SEND | TB_AIOE_CODE_ONESHOT;
			aioe->data = reqt;
			aioe->handle = aice->handle;

			// ok
			done = tb_true;

		} while (0);

		// error?
		if (!done)
		{
			if (reqt) tb_aiop_aice_exit(ptor, reqt);
			return tb_false;
		}
	}
	// failed?
	else
	{
		// post failed
		tb_aice_t resp = *aice;
		resp.state = TB_AICE_STATE_FAILED;
		tb_aicp_proactor_unix_resp(ptor->uptr, &resp);
	}

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * spak 
 */
static tb_bool_t tb_aiop_spak_acpt(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, tb_false);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_ACPT, tb_false);

	// accept
	tb_handle_t sock = tb_socket_accept(aice->handle);

	// trace
	tb_trace_impl("acpt[%p]: %p", aice->handle, sock);

	// ok?
	if (sock) 
	{
		aice->u.acpt.sock = sock;
		aice->state = TB_AICE_STATE_OK;
	}
	// failed?
	else aice->state = TB_AICE_STATE_FAILED;

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_spak_conn(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, tb_false);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_CONN, tb_false);
	tb_assert_and_check_return_val(aice->u.conn.host && aice->u.conn.port, tb_false);

	// connect it again
	tb_long_t ok = tb_socket_connect(aice->handle, aice->u.conn.host, aice->u.conn.port);

	// trace
	tb_trace_impl("conn[%p]: %ld", aice->handle, ok);

	// ok?
	if (ok > 0) aice->state = TB_AICE_STATE_OK;
	// failed?
	else aice->state = TB_AICE_STATE_FAILED;

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_spak_recv(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, tb_false);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_RECV, tb_false);
	tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size, tb_false);

	// recv
	tb_long_t real = tb_socket_recv(aice->handle, aice->u.recv.data, aice->u.recv.size);

	// trace
	tb_trace_impl("recv[%p]: %ld", aice->handle, real);

	// ok?
	if (real > 0) 
	{
		aice->u.recv.real = real;
		aice->state = TB_AICE_STATE_OK;
	}
	// closed?
	else if (!real) aice->state = TB_AICE_STATE_CLOSED;
	// failed?
	else aice->state = TB_AICE_STATE_FAILED;

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_spak_send(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, tb_false);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_SEND, tb_false);
	tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size, tb_false);

	// send
	tb_long_t real = tb_socket_send(aice->handle, aice->u.send.data, aice->u.send.size);

	// trace
	tb_trace_impl("send[%p]: %ld", aice->handle, real);

	// ok?
	if (real > 0) 
	{
		aice->u.send.real = real;
		aice->state = TB_AICE_STATE_OK;
	}
	// closed?
	else if (!real) aice->state = TB_AICE_STATE_CLOSED;
	// failed?
	else aice->state = TB_AICE_STATE_FAILED;

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_spak_resp(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check?
	tb_assert_and_check_return_val(ptor && aice, tb_false);

	// init spak
	static tb_bool_t (*s_spak[])(tb_aicp_proactor_aiop_t* , tb_aice_t*) = 
	{
		tb_null
	,	tb_aiop_spak_acpt
	,	tb_aiop_spak_conn
	,	tb_aiop_spak_recv
	,	tb_aiop_spak_send
	,	tb_null
	,	tb_null
	};
	tb_assert_and_check_return_val(aice->code < tb_arrayn(s_spak) && s_spak[aice->code], tb_false);

	// done spak 
	return s_spak[aice->code](ptor, aice);
}

/* ///////////////////////////////////////////////////////////////////////
 * loop
 */
static tb_pointer_t tb_aiop_spak_loop(tb_pointer_t data)
{
	// check
	tb_aicp_proactor_aiop_t* 	ptor = (tb_aicp_proactor_aiop_t*)data;
	tb_aicp_t* 					aicp = ptor? ptor->base.aicp : tb_null;
	tb_assert_and_check_goto(ptor && ptor->aiop && ptor->list && ptor->uptr && aicp, end);

	// trace
	tb_trace_impl("loop: init");

	// loop 
	while (!tb_atomic_get(&aicp->kill))
	{
		// wait aioe
		tb_long_t real = tb_aiop_wait(ptor->aiop, ptor->list, ptor->maxn, -1);
		tb_assert_and_check_break(real >= 0 && real <= ptor->maxn);
		
		// timeout?
		tb_check_continue(real);
	
		// grow it if aioe is full
		if (real == ptor->maxn)
		{
			// grow size
			ptor->maxn += tb_align8((ptor->base.aicp->maxn >> 3) + 1);
			if (ptor->maxn > ptor->base.aicp->maxn) ptor->maxn = ptor->base.aicp->maxn;

			// grow list
			ptor->list = tb_ralloc(ptor->list, ptor->maxn * sizeof(tb_aioe_t));
			tb_assert_and_check_break(ptor->list);
		}

		// walk aioe list
		tb_size_t i = 0;
		for (i = 0; i < real; i++)
		{
			// the aioe
			tb_aioe_t const* aioe = &ptor->list[i];
			tb_assert_and_check_goto(aioe->handle, end);

			// the aice
			tb_aice_t const* aice = aioe->data;
			tb_assert_and_check_goto(aice && aice->handle && aice->handle == aioe->handle, end);

			// enter 
			if (ptor->mutx.resp) tb_mutex_enter(ptor->mutx.resp);

			// post aice
			if (!tb_queue_full(ptor->resp)) 
			{
				// put
				tb_queue_put(ptor->resp, aice);

				// trace
				tb_trace_impl("resp: code: %lu, size: %lu", aice->code, tb_queue_size(ptor->resp));

				// only post wait
				tb_aicp_proactor_unix_resp(ptor->uptr, tb_null);
			}
			else
			{
				// assert
				tb_assert(0);
			}

			// leave 
			if (ptor->mutx.resp) tb_mutex_leave(ptor->mutx.resp);
		}
	}
	
end:
	// trace
	tb_trace_impl("loop: exit");

	// kill
	tb_atomic_set(&aicp->kill, 1);

	// exit
	tb_thread_return(tb_null);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_proactor_aiop_addo(tb_aicp_proactor_t* proactor, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && handle && type == TB_AICO_TYPE_SOCK, tb_false);

	// addo
	return tb_aiop_addo(ptor->aiop, handle, TB_AIOE_CODE_NONE, tb_null);
}
static tb_void_t tb_aicp_proactor_aiop_delo(tb_aicp_proactor_t* proactor, tb_handle_t handle)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return(ptor && ptor->aiop && handle);

	// delo
	tb_aiop_delo(ptor->aiop, handle);
}
static tb_bool_t tb_aicp_proactor_aiop_post(tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && list && size, tb_false);

	// init aioe list
	tb_size_t aioe_size = 0;
	tb_aioe_t aioe_list[TB_AICP_PROACTOR_UNIX_MAXN] = {0};

	// walk list
	tb_size_t i = 0;
	for (i = 0; i < size && aioe_size < TB_AICP_PROACTOR_UNIX_MAXN; i++)
	{
		// the aice
		tb_aice_t const* aice = &list[i];
	
		// init post
		static tb_bool_t (*s_post[])(tb_aicp_proactor_t* , tb_aice_t const*, tb_aioe_t* ) = 
		{
			tb_null
		,	tb_aiop_post_acpt
		,	tb_aiop_post_conn
		,	tb_aiop_post_recv
		,	tb_aiop_post_send
		,	tb_null
		,	tb_null
		};
		tb_assert_and_check_return_val(aice->code < tb_arrayn(s_post) && s_post[aice->code], tb_false);

		// post aice
		tb_aioe_t aioe = {0};
		if (!s_post[aice->code](proactor, aice, &aioe)) return tb_false;

		// save aioe
		if (aioe.code) aioe_list[aioe_size++] = aioe;
	}

	// post aioe list
	return aioe_size? tb_aiop_post(ptor->aiop, aioe_list, aioe_size) : tb_true;
}
static tb_long_t tb_aicp_proactor_aiop_spak(tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && resp, -1);

	// enter 
	if (ptor->mutx.resp) tb_mutex_enter(ptor->mutx.resp);

	// post aice
	tb_long_t ok = 0;
	if (!tb_queue_null(ptor->resp)) 
	{
		// get resp
		tb_aice_t const* aice = tb_queue_get(ptor->resp);
		if (aice) 
		{
			// save resp
			*resp = *aice;

			// trace
			tb_trace_impl("spak[%u]: code: %lu, size: %lu", (tb_uint16_t)tb_thread_self(), aice->code, tb_queue_size(ptor->resp));

			// pop it
			tb_queue_pop(ptor->resp);

			// exit aice
			tb_aiop_aice_exit(ptor, aice);

			// ok
			ok = 1;
		}
	}

	// leave 
	if (ptor->mutx.resp) tb_mutex_leave(ptor->mutx.resp);

	// ok?
	if (ok) 
	{
		// spak it
		if (!tb_aiop_spak_resp(ptor, resp)) ok = -1;
	}

	// ok?
	return ok;
}
static tb_void_t tb_aicp_proactor_aiop_kill(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return(ptor);

	// trace
	tb_trace_impl("kill");
}
static tb_void_t tb_aicp_proactor_aiop_exit(tb_aicp_proactor_t* proactor)
{
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	if (ptor)
	{
		// trace
		tb_trace_impl("exit");

		// exit loop
		if (ptor->loop)
		{
			if (!tb_thread_wait(ptor->loop, 5000))
				tb_thread_kill(ptor->loop);
			tb_thread_exit(ptor->loop);
			ptor->loop = tb_null;
		}

		// exit resp
		if (ptor->mutx.resp) tb_mutex_enter(ptor->mutx.resp);
		if (ptor->resp) tb_queue_exit(ptor->resp);
		ptor->resp = tb_null;
		if (ptor->mutx.resp) tb_mutex_leave(ptor->mutx.resp);

		// exit pool
		if (ptor->mutx.pool) tb_mutex_enter(ptor->mutx.pool);
		if (ptor->pool) tb_rpool_exit(ptor->pool);
		ptor->pool = tb_null;
		if (ptor->mutx.pool) tb_mutex_leave(ptor->mutx.pool);

		// exit aiop
		if (ptor->aiop) tb_aiop_exit(ptor->aiop);
		ptor->aiop = tb_null;

		// exit list
		if (ptor->list) tb_free(ptor->list);
		ptor->list = tb_null;

		// exit mutx
		if (ptor->mutx.pool) tb_mutex_exit(ptor->mutx.pool);
		if (ptor->mutx.resp) tb_mutex_exit(ptor->mutx.resp);
		ptor->mutx.pool = tb_null;
		ptor->mutx.resp = tb_null;

		// exit it
		tb_free(ptor);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
static tb_void_t tb_aicp_proactor_aiop_init(tb_aicp_proactor_unix_t* uptr)
{
	// check
	tb_assert_and_check_return(uptr && uptr->base.aicp && uptr->base.aicp->maxn);

	// need this proactor?
	tb_check_return( 	!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_ACPT]
					|| 	!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_CONN]
					|| 	!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_RECV]
					|| 	!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_SEND]
					);

	// make proactor
	tb_aicp_proactor_aiop_t* ptor = tb_malloc0(sizeof(tb_aicp_proactor_aiop_t));
	tb_assert_and_check_return(ptor);

	// init base
	ptor->uptr = uptr;
	ptor->base.aicp = uptr->base.aicp;
	ptor->base.kill = tb_aicp_proactor_aiop_kill;
	ptor->base.exit = tb_aicp_proactor_aiop_exit;
	ptor->base.addo = tb_aicp_proactor_aiop_addo;
	ptor->base.delo = tb_aicp_proactor_aiop_delo;
	ptor->base.post = tb_aicp_proactor_aiop_post;
	ptor->base.spak = tb_aicp_proactor_aiop_spak;

	// init mutx
	ptor->mutx.pool = tb_mutex_init(tb_null);
	ptor->mutx.resp = tb_mutex_init(tb_null);
	tb_assert_and_check_goto(ptor->mutx.pool && ptor->mutx.resp, fail);

	// init aiop
	ptor->aiop = tb_aiop_init(ptor->base.aicp->maxn);
	tb_assert_and_check_goto(ptor->aiop, fail);

	// init pool
	ptor->pool = tb_rpool_init((ptor->base.aicp->maxn << 1) + 16, sizeof(tb_aice_t), 0);
	tb_assert_and_check_goto(ptor->pool, fail);

	// init resp
	ptor->resp = tb_queue_init((ptor->base.aicp->maxn << 2) + 16, tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_goto(ptor->resp, fail);

	// init aioe list
	ptor->maxn = tb_align8((ptor->base.aicp->maxn >> 3) + 1);
	ptor->list = tb_nalloc0(ptor->maxn, sizeof(tb_aioe_t));
	tb_assert_and_check_goto(ptor->list, fail);
	
	// init loop
	ptor->loop = tb_thread_init(tb_null, tb_aiop_spak_loop, ptor, 0);
	tb_assert_and_check_goto(ptor->loop, fail);
	
	// add this proactor to the unix proactor list
	uptr->ptor_list[uptr->ptor_size++] = (tb_aicp_proactor_t*)ptor;
	ptor->indx = uptr->ptor_size;

	// attach index to some aice post
	if (!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_ACPT]) uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_ACPT] = ptor->indx;
	if (!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_CONN]) uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_CONN] = ptor->indx;
	if (!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_RECV]) uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_RECV] = ptor->indx;
	if (!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_SEND]) uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_SEND] = ptor->indx;

	// ok
	return ;

fail:
	if (ptor) tb_aicp_proactor_aiop_exit(ptor);
}

