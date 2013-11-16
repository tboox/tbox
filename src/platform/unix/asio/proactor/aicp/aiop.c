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

// the aiop aico type
typedef struct __tb_aiop_aico_t
{
	// the aioo
	tb_aioo_t const* 			aioo;

	// the wait
	tb_bool_t 					wait;

}tb_aiop_aico_t;

// the aiop mutx type
typedef struct __tb_aiop_mutx_t
{
	// the pool mutx
	tb_handle_t 				pool;

	// the spak mutx
	tb_handle_t 				spak;

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

	// the aice spak
	tb_queue_t* 				spak;
	
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
static tb_aice_t* tb_aiop_aice_init(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* init)
{
	// check
	tb_assert_and_check_return_val(ptor && ptor->pool, tb_null);

	// enter 
	if (ptor->mutx.pool) tb_mutex_enter(ptor->mutx.pool);

	// make aice
	tb_aice_t* aice = (tb_aice_t*)tb_rpool_malloc0(ptor->pool);

	// init aice
	if (aice && init) *aice = *init;

	// leave 
	if (ptor->mutx.pool) tb_mutex_leave(ptor->mutx.pool);
	
	// ok?
	return aice;
}
static tb_void_t tb_aiop_aice_exit(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return(ptor && ptor->pool);

	// enter 
	if (ptor->mutx.pool) tb_mutex_enter(ptor->mutx.pool);

	// exit aice
	if (aice) tb_rpool_free(ptor->pool, aice);

	// leave 
	if (ptor->mutx.pool) tb_mutex_leave(ptor->mutx.pool);
}
static tb_bool_t tb_aiop_aice_wait(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice)
{	
	// check
	tb_assert_and_check_return_val(ptor && ptor->uptr && ptor->aiop && aice, tb_false);

	// the aico
	tb_aico_t* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// the aiop aico
	tb_aiop_aico_t* aiop_aico = tb_aicp_proactor_unix_getp(ptor->uptr, ptor->indx, aico);
	tb_assert_and_check_return_val(aiop_aico, tb_false);

	// the aioe code
	static tb_size_t s_code[] =
	{
		TB_AIOE_CODE_NONE
	, 	TB_AIOE_CODE_ACPT
	, 	TB_AIOE_CODE_CONN
	, 	TB_AIOE_CODE_RECV
	, 	TB_AIOE_CODE_SEND
	, 	TB_AIOE_CODE_NONE
	, 	TB_AIOE_CODE_NONE
	};
	tb_assert_and_check_return_val(aice->code && aice->code < tb_arrayn(s_code) && s_code[aice->code], tb_false);

	// done
	tb_bool_t 	ok = tb_false;
	tb_aice_t* 	wait = tb_null;
	do
	{
		// make wait
		wait = tb_aiop_aice_init(ptor, aice);
		tb_assert_and_check_break(wait);

		// have aioo?
		if (!aiop_aico->aioo) 
		{
			// addo wait
			if (!(aiop_aico->aioo = tb_aiop_addo(ptor->aiop, aico->handle, s_code[aice->code] | TB_AIOE_CODE_ONESHOT, wait))) break;
		}
		else
		{
			// sete wait
			if (!tb_aiop_sete(ptor->aiop, aiop_aico->aioo, s_code[aice->code] | TB_AIOE_CODE_ONESHOT, wait)) break;
		}

		// wait it
		aiop_aico->wait = tb_true;

		// ok
		ok = tb_true;

	} while (0);

	// error?
	if (!ok)
	{
		if (wait) tb_aiop_aice_exit(ptor, wait);
	}

	// ok?
	return ok;
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

		// killed?
		tb_check_break(real >= 0);

		// error? out of range
		tb_assert_and_check_break(real <= ptor->maxn);
		
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

		// enter 
		if (ptor->mutx.spak) tb_mutex_enter(ptor->mutx.spak);

		// walk aioe list
		tb_size_t i = 0;
		for (i = 0; i < real; i++)
		{
			// the aioe
			tb_aioe_t const* aioe = &ptor->list[i];
			tb_assert_and_check_goto(aioe, end);

			// the aice
			tb_aice_t const* aice = aioe->data;
			tb_assert_and_check_goto(aice, end);

			// post aice
			if (!tb_queue_full(ptor->spak)) 
			{
				// put
				tb_queue_put(ptor->spak, aice);

				// trace
				tb_trace_impl("wait: code: %lu, size: %lu", aice->code, tb_queue_size(ptor->spak));
			}
			else
			{
				// assert
				tb_assert(0);
			}

		}
			
		// leave 
		if (ptor->mutx.spak) tb_mutex_leave(ptor->mutx.spak);

		// work it for all workers
		tb_aicp_proactor_unix_work(ptor->uptr);
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
 * spak
 */
static tb_long_t tb_aiop_spak_acpt(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, -1);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_ACPT, -1);

	// the aico
	tb_aico_t* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, -1);

	// the aiop aico
	tb_aiop_aico_t* aiop_aico = tb_aicp_proactor_unix_getp(ptor->uptr, ptor->indx, aico);
	tb_assert_and_check_return_val(aiop_aico, -1);

	// accept it
	tb_handle_t sock = tb_socket_accept(aico->handle);

	// trace
	tb_trace_impl("acpt[%p]: %p", aico->handle, sock);

	// no accepted? wait it
	if (!sock) 
	{
		// wait it
		if (!aiop_aico->wait) return tb_aiop_aice_wait(ptor, aice)? 0 : -1;
		// closed
		else aice->state = TB_AICE_STATE_CLOSED;
	}

	// save it
	aice->state = TB_AICE_STATE_OK;
	aice->u.acpt.sock = sock;
	
	// reset wait
	aiop_aico->wait = tb_false;

	// ok
	return 1;
}
static tb_long_t tb_aiop_spak_conn(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, -1);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_CONN, -1);
	tb_assert_and_check_return_val(aice->u.conn.host && aice->u.conn.port, -1);

	// the aico
	tb_aico_t* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, -1);

	// the aiop aico
	tb_aiop_aico_t* aiop_aico = tb_aicp_proactor_unix_getp(ptor->uptr, ptor->indx, aico);
	tb_assert_and_check_return_val(aiop_aico, -1);

	// try to connect it
	tb_long_t ok = tb_socket_connect(aico->handle, aice->u.conn.host, aice->u.conn.port);

	// trace
	tb_trace_impl("conn[%p]: %ld", aico->handle, ok);

	// no connected? wait it
	if (!ok) 
	{
		// wait it
		if (!aiop_aico->wait) return tb_aiop_aice_wait(ptor, aice)? 0 : -1;
		// closed
		else aice->state = TB_AICE_STATE_FAILED;
	}

	// save it
	aice->state = ok > 0? TB_AICE_STATE_OK : TB_AICE_STATE_FAILED;
	
	// reset wait
	aiop_aico->wait = tb_false;

	// ok
	return 1;
}
static tb_long_t tb_aiop_spak_recv(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, -1);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_RECV, -1);
	tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size, -1);

	// the aico
	tb_aico_t* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, -1);

	// the aiop aico
	tb_aiop_aico_t* aiop_aico = tb_aicp_proactor_unix_getp(ptor->uptr, ptor->indx, aico);
	tb_assert_and_check_return_val(aiop_aico, -1);

	// try to recv it
	tb_size_t recv = 0;
	tb_long_t real = 0;
	while (recv < aice->u.recv.size)
	{
		// recv it
		real = tb_socket_recv(aico->handle, aice->u.recv.data + recv, aice->u.recv.size - recv);
		
		// save recv
		if (real > 0) recv += real;
		else break;
	}

	// trace
	tb_trace_impl("recv[%p]: %lu", aico->handle, recv);

	// no recv? 
	if (!recv) 
	{
		// wait it
		if (!aiop_aico->wait) return tb_aiop_aice_wait(ptor, aice)? 0 : -1;
		// closed
		else aice->state = TB_AICE_STATE_CLOSED;
	}
	else
	{
		// ok or closed?
		aice->state = real < 0? TB_AICE_STATE_CLOSED : TB_AICE_STATE_OK;

		// save the recv size
		aice->u.recv.real = recv;
	}
	
	// reset wait
	aiop_aico->wait = tb_false;

	// ok
	return 1;
}
static tb_long_t tb_aiop_spak_send(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, -1);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_SEND, -1);
	tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size, -1);

	// the aico
	tb_aico_t* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, -1);

	// the aiop aico
	tb_aiop_aico_t* aiop_aico = tb_aicp_proactor_unix_getp(ptor->uptr, ptor->indx, aico);
	tb_assert_and_check_return_val(aiop_aico, -1);

	// try to send it
	tb_size_t send = 0;
	tb_long_t real = 0;
	while (send < aice->u.send.size)
	{
		// send it
		real = tb_socket_send(aico->handle, aice->u.send.data + send, aice->u.send.size - send);
		
		// save send
		if (real > 0) send += real;
		else break;
	}

	// trace
	tb_trace_impl("send[%p]: %lu", aico->handle, send);

	// no send? 
	if (!send) 
	{
		// wait it
		if (!aiop_aico->wait) return tb_aiop_aice_wait(ptor, aice)? 0 : -1;
		// closed
		else aice->state = TB_AICE_STATE_CLOSED;
	}
	else
	{
		// ok or closed?
		aice->state = real < 0? TB_AICE_STATE_CLOSED : TB_AICE_STATE_OK;

		// save the send size
		aice->u.send.real = send;
	}
	
	// reset wait
	aiop_aico->wait = tb_false;

	// ok
	return 1;
}
static tb_long_t tb_aiop_spak_done(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, -1);

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
	tb_assert_and_check_return_val(aice->code < tb_arrayn(s_spak) && s_spak[aice->code], -1);

	// done spak 
	return s_spak[aice->code](ptor, aice);
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_proactor_aiop_addo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && aico && aico->type == TB_AICO_TYPE_SOCK, tb_false);

	// the aiop aico
	tb_aiop_aico_t* aiop_aico = tb_aicp_proactor_unix_getp(ptor->uptr, ptor->indx, aico);
	tb_assert_and_check_return_val(aiop_aico, tb_false);

	// init aiop aico and addo it later
	aiop_aico->aioo = tb_null;

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_proactor_aiop_delo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && aico && aico->type == TB_AICO_TYPE_SOCK, tb_false);

	// the aiop aico
	tb_aiop_aico_t* aiop_aico = tb_aicp_proactor_unix_getp(ptor->uptr, ptor->indx, aico);
	tb_assert_and_check_return_val(aiop_aico && aiop_aico->aioo, tb_false);

	// delo
	tb_aiop_delo(ptor->aiop, aiop_aico->aioo);

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_proactor_aiop_post(tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && list && size, tb_false);
	
	// enter 
	if (ptor->mutx.spak) tb_mutex_enter(ptor->mutx.spak);

	// walk list
	tb_size_t i = 0;
	tb_bool_t ok = tb_true;
	for (i = 0; i < size && ok; i++)
	{
		// the aice
		tb_aice_t const* aice = &list[i];

		// post aice
		if (!tb_queue_full(ptor->spak)) 
		{
			// put
			tb_queue_put(ptor->spak, aice);

			// trace
			tb_trace_impl("post: code: %lu, size: %lu", aice->code, tb_queue_size(ptor->spak));
		}
		else
		{
			// failed
			ok = tb_false;

			// assert
			tb_assert(0);
		}

	}

	// leave 
	if (ptor->mutx.spak) tb_mutex_leave(ptor->mutx.spak);

	// work it for all workers
	tb_aicp_proactor_unix_work(ptor->uptr);

	// ok?
	return ok;
}
static tb_long_t tb_aicp_proactor_aiop_spak(tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && resp, -1);

	// spak it
	tb_long_t ok = 0;
	tb_bool_t null = tb_false;
	while (!ok && !null)
	{
		// enter 
		if (ptor->mutx.spak) tb_mutex_enter(ptor->mutx.spak);

		// spak aice
		if (!(null = tb_queue_null(ptor->spak))) 
		{
			// get resp
			tb_aice_t const* aice = tb_queue_get(ptor->spak);
			if (aice) 
			{
				// save resp
				*resp = *aice;

				// trace
				tb_trace_impl("spak[%u]: code: %lu, size: %lu", (tb_uint16_t)tb_thread_self(), aice->code, tb_queue_size(ptor->spak));

				// pop it
				tb_queue_pop(ptor->spak);

				// ok
				ok = 1;
			}
		}

		// leave 
		if (ptor->mutx.spak) tb_mutex_leave(ptor->mutx.spak);

		// ok? done it
		if (ok) ok = tb_aiop_spak_done(ptor, resp);
	}

	// ok?
	return ok;
}
static tb_void_t tb_aicp_proactor_aiop_kill(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return(ptor && ptor->aiop);

	// trace
	tb_trace_impl("kill");

	// kill it
	tb_aiop_kill(ptor->aiop);
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

		// exit spak
		if (ptor->mutx.spak) tb_mutex_enter(ptor->mutx.spak);
		if (ptor->spak) tb_queue_exit(ptor->spak);
		ptor->spak = tb_null;
		if (ptor->mutx.spak) tb_mutex_leave(ptor->mutx.spak);

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
		if (ptor->mutx.spak) tb_mutex_exit(ptor->mutx.spak);
		ptor->mutx.pool = tb_null;
		ptor->mutx.spak = tb_null;

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
	ptor->base.step = sizeof(tb_aiop_aico_t);
	ptor->base.aicp = uptr->base.aicp;
	ptor->base.kill = tb_aicp_proactor_aiop_kill;
	ptor->base.exit = tb_aicp_proactor_aiop_exit;
	ptor->base.addo = tb_aicp_proactor_aiop_addo;
	ptor->base.delo = tb_aicp_proactor_aiop_delo;
	ptor->base.post = tb_aicp_proactor_aiop_post;
	ptor->base.spak = tb_aicp_proactor_aiop_spak;

	// init mutx
	ptor->mutx.pool = tb_mutex_init();
	ptor->mutx.spak = tb_mutex_init();
	tb_assert_and_check_goto(ptor->mutx.pool && ptor->mutx.spak, fail);

	// init aiop
	ptor->aiop = tb_aiop_init(ptor->base.aicp->maxn);
	tb_assert_and_check_goto(ptor->aiop, fail);

	// init pool
	ptor->pool = tb_rpool_init((ptor->base.aicp->maxn << 1) + 16, sizeof(tb_aice_t), 0);
	tb_assert_and_check_goto(ptor->pool, fail);

	// init spak
	ptor->spak = tb_queue_init((ptor->base.aicp->maxn << 2) + 16, tb_item_func_ifm(sizeof(tb_aice_t), tb_null, tb_null));
	tb_assert_and_check_goto(ptor->spak, fail);

	// init aioe list
	ptor->maxn = tb_align8((ptor->base.aicp->maxn >> 3) + 1);
	ptor->list = tb_nalloc0(ptor->maxn, sizeof(tb_aioe_t));
	tb_assert_and_check_goto(ptor->list, fail);
	
	// init loop
	ptor->loop = tb_thread_init(tb_null, tb_aiop_spak_loop, ptor, 0);
	tb_assert_and_check_goto(ptor->loop, fail);
	
	// add this proactor to the unix proactor list
	ptor->indx = uptr->ptor_size++;
	uptr->ptor_list[ptor->indx] = (tb_aicp_proactor_t*)ptor;

	// attach index to some aice post
	if (!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_ACPT]) uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_ACPT] = ptor->indx + 1;
	if (!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_CONN]) uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_CONN] = ptor->indx + 1;
	if (!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_RECV]) uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_RECV] = ptor->indx + 1;
	if (!uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_SEND]) uptr->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_SEND] = ptor->indx + 1;

	// ok
	return ;

fail:
	if (ptor) tb_aicp_proactor_aiop_exit(ptor);
}

