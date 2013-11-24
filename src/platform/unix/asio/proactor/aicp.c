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
 * @file		aicp.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 				"aicp_aiop"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aiop aico type
typedef struct __tb_aiop_aico_t
{
	// the base
	tb_aico_t 					base;

	// the aioo
	tb_aioo_t const* 			aioo;

	// the aice
	tb_aice_t 					aice;

	// the wait
	tb_size_t 					wait;

}tb_aiop_aico_t;

// the aiop proactor type
typedef struct __tb_aicp_proactor_aiop_t
{
	// the proactor base
	tb_aicp_proactor_t 			base;

	// the file impl
	tb_handle_t 				file;

	// the wait aiop
	tb_aiop_t* 					aiop;

	// the aice spak
	tb_queue_t* 				spak;
	
	// the spak mutx
	tb_handle_t 				mutx;

	// the spak wait
	tb_handle_t 				wait;

	// the spak loop
	tb_handle_t 				loop;

	// the aioe list
	tb_aioe_t* 					list;

	// the aioe size
	tb_size_t 					maxn;

}tb_aicp_proactor_aiop_t;

/* ///////////////////////////////////////////////////////////////////////
 * file declaration
 */
static tb_handle_t 	tb_aicp_file_init(tb_aicp_proactor_aiop_t* ptor);
static tb_void_t 	tb_aicp_file_exit(tb_handle_t file);
static tb_bool_t 	tb_aicp_file_addo(tb_handle_t file, tb_aico_t* aico);
static tb_bool_t 	tb_aicp_file_delo(tb_handle_t file, tb_aico_t* aico);
static tb_bool_t 	tb_aicp_file_post(tb_handle_t file, tb_aice_t const* list, tb_size_t size);
static tb_long_t 	tb_aicp_file_spak(tb_handle_t file, tb_aice_t* aice);
static tb_void_t 	tb_aicp_file_kill(tb_handle_t file);
static tb_void_t 	tb_aicp_file_poll(tb_handle_t file);

/* ///////////////////////////////////////////////////////////////////////
 * spak
 */
static tb_void_t tb_aiop_spak_work(tb_aicp_proactor_aiop_t* ptor)
{
	// check
	tb_assert_and_check_return(ptor && ptor->wait && ptor->base.aicp);

	// the worker size
	tb_size_t work = tb_atomic_get(&ptor->base.aicp->work);

	// the semaphore value
	tb_long_t value = tb_semaphore_value(ptor->wait);

	// post wait
	if (value >= 0 && value < work) tb_semaphore_post(ptor->wait, work - value);
}
static tb_pointer_t tb_aiop_spak_loop(tb_pointer_t data)
{
	// check
	tb_aicp_proactor_aiop_t* 	ptor = (tb_aicp_proactor_aiop_t*)data;
	tb_aicp_t* 					aicp = ptor? ptor->base.aicp : tb_null;
	tb_assert_and_check_goto(ptor && ptor->aiop && ptor->list && aicp, end);

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
			ptor->maxn += tb_align8((aicp->maxn >> 3) + 1);
			if (ptor->maxn > aicp->maxn) ptor->maxn = aicp->maxn;

			// grow list
			ptor->list = tb_ralloc(ptor->list, ptor->maxn * sizeof(tb_aioe_t));
			tb_assert_and_check_break(ptor->list);
		}

		// enter 
		if (ptor->mutx) tb_mutex_enter(ptor->mutx);

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

			// the aico
			tb_aico_t const* aico = aice->aico;
			tb_assert_and_check_goto(aico, end);

			// have wait?
			tb_check_continue(aice->code);

			// trace
			tb_trace_impl("wait: code: %lu, size: %lu", aice->code, tb_queue_size(ptor->spak));

			// sock?
			if (aico->type == TB_AICO_TYPE_SOCK)
			{
				// spak aice
				if (!tb_queue_full(ptor->spak)) tb_queue_put(ptor->spak, aice);
				else tb_assert(0);
			}
			else if (aico->type == TB_AICO_TYPE_FILE)
			{
				// check
				tb_assert(ptor->file);

				// poll file
				if (ptor->file) tb_aicp_file_poll(ptor->file);
			}
			else tb_assert(0);
		}
			
		// leave 
		if (ptor->mutx) tb_mutex_leave(ptor->mutx);

		// work it
		tb_aiop_spak_work(ptor);
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
static tb_bool_t tb_aiop_spak_wait(tb_aicp_proactor_aiop_t* ptor, tb_aice_t const* aice)
{	
	// check
	tb_assert_and_check_return_val(ptor && ptor->aiop && aice, tb_false);

	// the aico
	tb_aiop_aico_t* aico = (tb_aiop_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// the aioe code
	static tb_size_t s_code[] =
	{
		TB_AIOE_CODE_NONE
	, 	TB_AIOE_CODE_ACPT
	, 	TB_AIOE_CODE_CONN
	, 	TB_AIOE_CODE_RECV
	, 	TB_AIOE_CODE_SEND
	, 	TB_AIOE_CODE_RECV
	, 	TB_AIOE_CODE_SEND
	, 	TB_AIOE_CODE_SEND
	, 	TB_AIOE_CODE_NONE
	, 	TB_AIOE_CODE_NONE
	, 	TB_AIOE_CODE_NONE
	, 	TB_AIOE_CODE_NONE
	, 	TB_AIOE_CODE_NONE
	};
	tb_assert_and_check_return_val(aice->code && aice->code < tb_arrayn(s_code) && s_code[aice->code], tb_false);
				
	// trace
	tb_trace_impl("wait: code: %lu: ..", aice->code);

	// done
	tb_bool_t ok = tb_false;
	tb_size_t wait = aico->wait;
	tb_aice_t prev = aico->aice;
	do
	{
		// wait it
		aico->aice = *aice;

		// wait++
		aico->wait++;

		// have aioo?
		if (!aico->aioo) 
		{
			// addo wait
			if (!(aico->aioo = tb_aiop_addo(ptor->aiop, aico->base.handle, s_code[aice->code] | TB_AIOE_CODE_ONESHOT, &aico->aice))) break;
		}
		else
		{
			// sete wait
			if (!tb_aiop_sete(ptor->aiop, aico->aioo, s_code[aice->code] | TB_AIOE_CODE_ONESHOT, &aico->aice)) break;
		}

		// ok
		ok = tb_true;

	} while (0);

	// failed? restore it
	if (!ok) 
	{
		// trace
		tb_trace_impl("wait: code: %lu: failed", aice->code);

		// restore it
		aico->aice = prev;
		aico->wait = wait;
	}

	// ok?
	return ok;
}
static tb_long_t tb_aiop_spak_acpt(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, -1);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_ACPT, -1);

	// the aico
	tb_aiop_aico_t* aico = (tb_aiop_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, -1);

	// check wait
	tb_assert_and_check_return_val(aico->wait < 2, -1);

	// accept it
	tb_handle_t sock = tb_socket_accept(aico->base.handle);

	// trace
	tb_trace_impl("acpt[%p]: %p", aico->base.handle, sock);

	// no accepted? wait it
	if (!sock) 
	{
		// wait it
		if (!aico->wait) return tb_aiop_spak_wait(ptor, aice)? 0 : -1;
		// closed
		else aice->state = TB_AICE_STATE_CLOSED;
	}

	// save it
	aice->state = TB_AICE_STATE_OK;
	aice->u.acpt.sock = sock;

	// reset wait
	if (aico->wait) aico->wait--;
	aico->aice.code = TB_AICE_CODE_NONE;

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
	tb_aiop_aico_t* aico = (tb_aiop_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, -1);

	// check wait
	tb_assert_and_check_return_val(aico->wait < 2, -1);

	// try to connect it
	tb_long_t ok = tb_socket_connect(aico->base.handle, aice->u.conn.host, aice->u.conn.port);

	// trace
	tb_trace_impl("conn[%p]: %ld", aico->base.handle, ok);

	// no connected? wait it
	if (!ok) 
	{
		// wait it
		if (!aico->wait) return tb_aiop_spak_wait(ptor, aice)? 0 : -1;
		// closed
		else aice->state = TB_AICE_STATE_FAILED;
	}

	// save it
	aice->state = ok > 0? TB_AICE_STATE_OK : TB_AICE_STATE_FAILED;
	
	// reset wait
	if (aico->wait) aico->wait--;
	aico->aice.code = TB_AICE_CODE_NONE;

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
	tb_aiop_aico_t* aico = (tb_aiop_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, -1);

	// check wait
	tb_assert_and_check_return_val(aico->wait < 2, -1);

	// try to recv it
	tb_size_t recv = 0;
	tb_long_t real = 0;
	while (recv < aice->u.recv.size)
	{
		// recv it
		real = tb_socket_recv(aico->base.handle, aice->u.recv.data + recv, aice->u.recv.size - recv);

		// save recv
		if (real > 0) recv += real;
		else break;
	}

	// trace
	tb_trace_impl("recv[%p]: %lu", aico->base.handle, recv);

	// no recv? 
	if (!recv) 
	{
		// wait it
		if (!real && !aico->wait) return tb_aiop_spak_wait(ptor, aice)? 0 : -1;
		// closed
		else aice->state = TB_AICE_STATE_CLOSED;
	}
	else
	{
		// ok or closed?
		aice->state = TB_AICE_STATE_OK;

		// save the recv size
		aice->u.recv.real = recv;
	}
	
	// reset wait
	if (aico->wait) aico->wait--;
	aico->aice.code = TB_AICE_CODE_NONE;

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
	tb_aiop_aico_t* aico = (tb_aiop_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, -1);

	// check wait
	tb_assert_and_check_return_val(aico->wait < 2, -1);

	// try to send it
	tb_size_t send = 0;
	tb_long_t real = 0;
	while (send < aice->u.send.size)
	{
		// send it
		real = tb_socket_send(aico->base.handle, aice->u.send.data + send, aice->u.send.size - send);
		
		// save send
		if (real > 0) send += real;
		else break;
	}

	// trace
	tb_trace_impl("send[%p]: %lu", aico->base.handle, send);

	// no send? 
	if (!send) 
	{
		// wait it
		if (!real && !aico->wait) return tb_aiop_spak_wait(ptor, aice)? 0 : -1;
		// closed
		else aice->state = TB_AICE_STATE_CLOSED;
	}
	else
	{
		// ok or closed?
		aice->state = TB_AICE_STATE_OK;

		// save the send size
		aice->u.send.real = send;
	}
	
	// reset wait
	if (aico->wait) aico->wait--;
	aico->aice.code = TB_AICE_CODE_NONE;

	// ok
	return 1;
}
static tb_long_t tb_aiop_spak_sendfile(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, -1);
	tb_assert_and_check_return_val(aice->code == TB_AICE_CODE_SENDFILE, -1);
	tb_assert_and_check_return_val(aice->u.sendfile.file && aice->u.sendfile.size, -1);

	// the aico
	tb_aiop_aico_t* aico = (tb_aiop_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, -1);

	// check wait
	tb_assert_and_check_return_val(aico->wait < 2, -1);

	// try to send it
	tb_long_t 	real = 0;
	tb_hize_t 	send = 0;
	tb_hize_t 	seek = aice->u.sendfile.seek;
	tb_hize_t 	size = aice->u.sendfile.size;
	tb_handle_t file = aice->u.sendfile.file;
	while (send < size)
	{
		// send it
		real = tb_socket_sendfile(aico->base.handle, file, seek + send, size - send);
		
		// save send
		if (real > 0) send += real;
		else break;
	}

	// trace
	tb_trace_impl("sendfile[%p]: %llu", aico->base.handle, send);

	// no send? 
	if (!send) 
	{
		// wait it
		if (!real && !aico->wait) return tb_aiop_spak_wait(ptor, aice)? 0 : -1;
		// closed
		else aice->state = TB_AICE_STATE_CLOSED;
	}
	else
	{
		// ok or closed?
		aice->state = TB_AICE_STATE_OK;

		// save the send size
		aice->u.sendfile.real = send;
	}
	
	// reset wait
	if (aico->wait) aico->wait--;
	aico->aice.code = TB_AICE_CODE_NONE;

	// ok
	return 1;
}
static tb_long_t tb_aiop_spak_done(tb_aicp_proactor_aiop_t* ptor, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(ptor && aice, -1);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico, -1);

	// done
	tb_long_t ok = -1;
	switch (aico->type)
	{
	case TB_AICO_TYPE_SOCK:
		{
			// init spak
			static tb_bool_t (*s_spak[])(tb_aicp_proactor_aiop_t* , tb_aice_t*) = 
			{
				tb_null
			,	tb_aiop_spak_acpt
			,	tb_aiop_spak_conn
			,	tb_aiop_spak_recv
			,	tb_aiop_spak_send
			,	tb_null // tb_aiop_spak_recvv
			,	tb_null // tb_aiop_spak_sendv
			,	tb_aiop_spak_sendfile
			,	tb_null
			,	tb_null
			,	tb_null
			,	tb_null
			,	tb_null
			};
			tb_assert_and_check_break(aice->code && aice->code < tb_arrayn(s_spak) && s_spak[aice->code]);

			// done spak 
			ok = s_spak[aice->code](ptor, aice);
		}
		break;
	case TB_AICO_TYPE_FILE:
		{
			// check
			tb_assert_and_check_break(ptor->file);

			// done spak
			ok = tb_aicp_file_spak(ptor->file, aice);
		}
		break;
	default:
		break;
	}

	// ok?
	return ok;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_proactor_aiop_addo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && aico, tb_false);

	// done
	tb_bool_t ok = tb_false;
	switch (aico->type)
	{
	case TB_AICO_TYPE_SOCK:
		{
			ok = tb_true;
		}
		break;
	case TB_AICO_TYPE_FILE:
		{
			// check
			tb_assert_and_check_break(ptor->file);

			// file: addo
			ok = tb_aicp_file_addo(ptor->file, aico);
		}
		break;
	default:
		break;
	}

	// ok?
	return ok;
}
static tb_bool_t tb_aicp_proactor_aiop_delo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && aico, tb_false);

	// done
	tb_bool_t ok = tb_false;
	switch (aico->type)
	{
	case TB_AICO_TYPE_SOCK:
		{
			// the aiop aico
			tb_aiop_aico_t* aiop_aico = (tb_aiop_aico_t*)aico;

			// delo
			if (aiop_aico->aioo) tb_aiop_delo(ptor->aiop, aiop_aico->aioo);
			aiop_aico->aioo = tb_null;

			// ok
			ok = tb_true;
		}
		break;
	case TB_AICO_TYPE_FILE:
		{
			// check
			tb_assert_and_check_break(ptor->file);

			// file: delo
			ok = tb_aicp_file_delo(ptor->file, aico);
		}
		break;
	default:
		break;
	}

	// ok?
	return ok;
}
static tb_bool_t tb_aicp_proactor_aiop_post(tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->file && ptor->spak && list && size, tb_false);
	
	// init
	tb_bool_t ok = tb_true;

	// only one?
	if (size == 1)
	{
		// the aico
		tb_aico_t* aico = list->aico;
		if (aico)
		{
			// done
			switch (aico->type)
			{
			case TB_AICO_TYPE_SOCK:
				{
					// enter 
					if (ptor->mutx) tb_mutex_enter(ptor->mutx);

					// post aice
					if (!tb_queue_full(ptor->spak)) 
					{
						// put
						tb_queue_put(ptor->spak, list);

						// trace
						tb_trace_impl("post: code: %lu, size: %lu", list->code, tb_queue_size(ptor->spak));
					}
					else
					{
						// failed
						ok = tb_false;

						// assert
						tb_assert(0);
					}

					// leave 
					if (ptor->mutx) tb_mutex_leave(ptor->mutx);
				}
				break;
			case TB_AICO_TYPE_FILE:
				{
					// post file
					ok = tb_aicp_file_post(ptor->file, list, 1);
				}
				break;
			default:
				ok = tb_false;
				break;
			}
		}
		else ok = tb_false;
	}
	else
	{
		// enter 
		if (ptor->mutx) tb_mutex_enter(ptor->mutx);

		// walk list
		tb_size_t i = 0;
		tb_aice_t file_list[TB_AICP_POST_MAXN]; 
		tb_size_t file_size = 0;
		for (i = 0; i < size && ok; i++)
		{
			// the aice
			tb_aice_t const* aice = &list[i];

			// the aico
			tb_aico_t* aico = aice->aico;
			tb_assert_and_check_continue(aico);

			// done
			switch (aico->type)
			{
			case TB_AICO_TYPE_SOCK:
				{
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
				break;
			case TB_AICO_TYPE_FILE:
				{
					// check
					tb_assert(file_size < TB_AICP_POST_MAXN);

					// save it
					if (file_size < TB_AICP_POST_MAXN) file_list[file_size++] = *aice;
					else ok = tb_false;
				}
				break;
			default:
				ok = tb_false;
				break;
			}
		}

		// leave 
		if (ptor->mutx) tb_mutex_leave(ptor->mutx);

		// post file
		if (ok && file_size) ok = tb_aicp_file_post(ptor->file, file_list, file_size);
	}

	// work it 
	tb_aiop_spak_work(ptor);

	// ok?
	return ok;
}
static tb_long_t tb_aicp_proactor_aiop_spak(tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && resp, -1);

	// enter 
	if (ptor->mutx) tb_mutex_enter(ptor->mutx);

	// spak aice
	tb_long_t ok = 0;
	tb_bool_t null = tb_false;
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
	if (ptor->mutx) tb_mutex_leave(ptor->mutx);

	// done it
	if (ok) ok = tb_aiop_spak_done(ptor, resp);

	// null?
	tb_check_return_val(!ok && null, ok);

	// trace
	tb_trace_impl("wait[%u]: ..", (tb_uint16_t)tb_thread_self());

	// wait some time
	if (tb_semaphore_wait(ptor->wait, timeout) < 0) return -1;

	// timeout 
	return 0;
}
static tb_void_t tb_aicp_proactor_aiop_kill(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return(ptor);

	// the worker size
	tb_size_t work = tb_atomic_get(&ptor->base.aicp->work);

	// trace
	tb_trace_impl("kill: %lu", work);

	// kill aiop
	if (ptor->aiop) tb_aiop_kill(ptor->aiop);

	// kill file
	if (ptor->file) tb_aicp_file_kill(ptor->file); 

	// post wait
	if (work) tb_semaphore_post(ptor->wait, work);
}
static tb_void_t tb_aicp_proactor_aiop_exit(tb_aicp_proactor_t* proactor)
{
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	if (ptor)
	{
		// trace
		tb_trace_impl("exit");

		// exit file
		if (ptor->file) tb_aicp_file_exit(ptor->file);
		ptor->file = tb_null;

		// exit loop
		if (ptor->loop)
		{
			if (!tb_thread_wait(ptor->loop, 5000))
				tb_thread_kill(ptor->loop);
			tb_thread_exit(ptor->loop);
			ptor->loop = tb_null;
		}

		// exit spak
		if (ptor->mutx) tb_mutex_enter(ptor->mutx);
		if (ptor->spak) tb_queue_exit(ptor->spak);
		ptor->spak = tb_null;
		if (ptor->mutx) tb_mutex_leave(ptor->mutx);

		// exit aiop
		if (ptor->aiop) tb_aiop_exit(ptor->aiop);
		ptor->aiop = tb_null;

		// exit list
		if (ptor->list) tb_free(ptor->list);
		ptor->list = tb_null;

		// exit wait
		if (ptor->wait) tb_semaphore_exit(ptor->wait);
		ptor->wait = tb_null;

		// exit mutx
		if (ptor->mutx) tb_mutex_exit(ptor->mutx);
		ptor->mutx = tb_null;

		// exit it
		tb_free(ptor);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * file implementation
 */
#if defined(TB_CONFIG_ASIO_HAVE_NAIO)
# 	include "file/naio.c"
#else
# 	include "file/file.c"
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp);
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->maxn, tb_null);

	// make proactor
	tb_aicp_proactor_aiop_t* ptor = tb_malloc0(sizeof(tb_aicp_proactor_aiop_t));
	tb_assert_and_check_return_val(ptor, tb_null);

	// init base
	ptor->base.aicp = aicp;
	ptor->base.step = sizeof(tb_aiop_aico_t);
	ptor->base.kill = tb_aicp_proactor_aiop_kill;
	ptor->base.exit = tb_aicp_proactor_aiop_exit;
	ptor->base.addo = tb_aicp_proactor_aiop_addo;
	ptor->base.delo = tb_aicp_proactor_aiop_delo;
	ptor->base.post = tb_aicp_proactor_aiop_post;
	ptor->base.spak = tb_aicp_proactor_aiop_spak;

	// init mutx
	ptor->mutx = tb_mutex_init();
	tb_assert_and_check_goto(ptor->mutx, fail);

	// init wait
	ptor->wait = tb_semaphore_init(0);
	tb_assert_and_check_goto(ptor->wait, fail);

	// init aiop
	ptor->aiop = tb_aiop_init(aicp->maxn);
	tb_assert_and_check_goto(ptor->aiop, fail);

	// init spak
	ptor->spak = tb_queue_init((aicp->maxn << 2) + 16, tb_item_func_ifm(sizeof(tb_aice_t), tb_null, tb_null));
	tb_assert_and_check_goto(ptor->spak, fail);

	// init file
	ptor->file = tb_aicp_file_init(ptor);
	tb_assert_and_check_goto(ptor->file, fail);

	// init list
	ptor->maxn = tb_align8((aicp->maxn >> 3) + 1);
	ptor->list = tb_nalloc0(ptor->maxn, sizeof(tb_aioe_t));
	tb_assert_and_check_goto(ptor->list, fail);

	// init loop
	ptor->loop = tb_thread_init(tb_null, tb_aiop_spak_loop, ptor, 0);
	tb_assert_and_check_goto(ptor->loop, fail);

	// ok
	return (tb_aicp_proactor_t*)ptor;

fail:
	if (ptor) tb_aicp_proactor_aiop_exit(ptor);
	return tb_null;
}

