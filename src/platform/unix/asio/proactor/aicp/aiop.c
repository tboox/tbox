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

// the aiop proactor type
typedef struct __tb_aicp_proactor_aiop_t
{
	// the proactor base
	tb_aicp_proactor_t 					base;

	// the unix proactor
	tb_aicp_proactor_t* 				uptr;

	// the proactor indx
	tb_size_t 							indx;

	// the aiop
	tb_aiop_t* 							aiop;

	// the aice pool
	tb_handle_t 						pool;
	
	// the pool mutx
	tb_handle_t 						mutx;

	// the spak loop
	tb_handle_t 						loop;

}tb_aicp_proactor_aiop_t;

/* ///////////////////////////////////////////////////////////////////////
 * aice
 */
static tb_aice_t* tb_aiop_aice_init(tb_aicp_proactor_aiop_t* ptor)
{
	// check
	tb_assert_and_check_return_val(ptor, tb_null);

	// enter 
	if (ptor->mutx) tb_mutex_enter(ptor->mutx);

	// make data
	tb_pointer_t data = ptor->pool? tb_rpool_malloc0(ptor->pool) : tb_null;

	// leave 
	if (ptor->mutx) tb_mutex_leave(ptor->mutx);
	
	// ok?
	return (tb_aice_t*)data;
}
static tb_void_t tb_aiop_aice_exit(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* data)
{
	// check
	tb_assert_and_check_return(ptor);

	// enter 
	if (ptor->mutx) tb_mutex_enter(ptor->mutx);

	// free data
	if (data) tb_rpool_free(ptor->pool, data);

	// leave 
	if (ptor->mutx) tb_mutex_leave(ptor->mutx);
}

/* ///////////////////////////////////////////////////////////////////////
 * post
 */
static tb_bool_t tb_aiop_post_acpt(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice)
{	
	// check
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_ACPT, tb_false);

	// ok?
	return tb_false;
}
static tb_bool_t tb_aiop_post_conn(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice)
{	
	// check
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_CONN, tb_false);
	tb_assert_and_check_return_val(aice->u.conn.host && aice->u.conn.port, tb_false);

	// try to connect it
	tb_long_t ok = tb_socket_connect(((tb_long_t)aice->handle) - 1, aice->u.conn.host, aice->u.conn.port);

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

			// TODO

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
static tb_bool_t tb_aiop_post_recv(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice)
{	
	// check
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_RECV, tb_false);
	tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size, tb_false);

	// ok?
	return tb_false;
}
static tb_bool_t tb_aiop_post_send(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice)
{	
	// check
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_SEND, tb_false);
	tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size, tb_false);

	// ok?
	return tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * spak 
 */
static tb_bool_t tb_aiop_spak_resp(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* resp, tb_size_t events)
{
	// check?
	tb_assert_and_check_return_val(ptor && resp, tb_false);

	// init spak
	static tb_bool_t (*s_spak[])(tb_aicp_proactor_aiop_t* , tb_aice_t const* , tb_size_t) = 
	{
		tb_null
	,	tb_null
	,	tb_null
	,	tb_null
	,	tb_null
	,	tb_null
	,	tb_null
	};
	tb_assert_and_check_return_val(resp->code < tb_arrayn(s_spak), tb_false);

	// done spak 
	return (s_spak[resp->code])? s_spak[resp->code](ptor, resp, events) : tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * loop
 */
static tb_pointer_t tb_aiop_spak_loop(tb_pointer_t data)
{
	// check
	tb_aicp_proactor_aiop_t* 	ptor = (tb_aicp_proactor_aiop_t*)data;
	tb_aicp_t* 					aicp = ptor? ptor->base.aicp : tb_null;
	tb_assert_and_check_goto(ptor && aicp, end);

	// trace
	tb_trace_impl("loop: init");

#if 0
	// loop 
	while (!tb_atomic_get(&aicp->kill))
	{
		// wait events
		tb_long_t evtn = aiop_wait(ptor->epfd, ptor->evts, ptor->evtm, -1);
		tb_assert_and_check_break(evtn >= 0 && evtn <= ptor->evtm);
		
		// timeout?
		tb_check_continue(evtn);
	
		// walk events
		tb_size_t i = 0;
		for (i = 0; i < evtn; i++)
		{
			// the aice
			tb_aice_t const* aice = (tb_aice_t const*)ptor->evts[i].data.u64;
			tb_assert_and_check_goto(aice, end);

			// spak resp
			if (!tb_aiop_spak_resp(ptor, aice, ptor->evts[i].events)) goto end;
		}
	}
#endif
	
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
	tb_assert_and_check_return_val(ptor && ptro->aiop && handle && type == TB_AICO_TYPE_SOCK, tb_false);

	// addo
	return tb_aiop_addo(ptor->aiop, handle, TB_AIOE_CODE_NONE, tb_null);
}
static tb_bool_t tb_aicp_proactor_aiop_delo(tb_aicp_proactor_t* proactor, tb_handle_t handle)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && handle, tb_false);

	// delo
	return tb_aiop_delo(ptor->aiop, handle);
}
static tb_bool_t tb_aicp_proactor_aiop_post(tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && list && size, tb_false);

	// walk list
	tb_size_t i = 0;
	for (i = 0; i < size; i++)
	{
		// the aice
		tb_aice_t const* aice = &list[i];
	
		// init post
		static tb_long_t (*s_post[])(tb_aicp_proactor_t* , tb_aice_t const*) = 
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
		if (!s_post[aice->code](proactor, aice)) return tb_false;
	}

	// ok
	return tb_true;
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

		// exit pool
		if (ptor->mutx) tb_mutex_enter(ptor->mutx);
		if (ptor->pool) tb_rpool_exit(ptor->pool);
		ptor->pool = tb_null;
		if (ptor->mutx) tb_mutex_leave(ptor->mutx);

		// exit aiop
		if (ptor->aiop) tb_aiop_exit(ptor->aiop);
		ptor->aiop = tb_null;

		// exit mutx
		if (ptor->mutx) tb_mutex_exit(ptor->mutx);
		ptor->mutx = tb_null;

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

	// init mutx
	ptor->mutx = tb_mutex_init(tb_null);
	tb_assert_and_check_goto(ptor->mutx, fail);

	// init aiop
	ptor->aiop = tb_aiop_init(uptr->base.aicp->maxn);
	tb_assert_and_check_goto(ptor->aiop, fail);

	// init pool
	ptor->pool = tb_rpool_init((uptr->base.aicp->maxn << 1) + 16, sizeof(tb_aice_t), 0);
	tb_assert_and_check_goto(ptor->pool, fail);

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

