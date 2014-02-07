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
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 				"aicp"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"
#include "aioo.h"
#include "addr.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp);

/* ///////////////////////////////////////////////////////////////////////
 * aico
 */
static tb_aico_t* tb_aicp_aico_init(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->pool && type, tb_null);

	// enter 
	tb_spinlock_enter(&aicp->lock);

	// make aico
	tb_aico_t* aico = (tb_aico_t*)tb_rpool_malloc0(aicp->pool);

	// init aico
	if (aico)
	{
		aico->aicp 		= aicp;
		aico->type 		= type;
		aico->handle 	= handle;
		aico->pool 		= tb_null;

		// init timeout 
		tb_size_t i = 0;
		tb_size_t n = tb_arrayn(aico->timeout);
		for (i = 0; i < n; i++) aico->timeout[i] = -1;
	}

	// leave 
	tb_spinlock_leave(&aicp->lock);
	
	// ok?
	return aico;
}
static tb_void_t tb_aicp_aico_exit(tb_aicp_t* aicp, tb_aico_t* aico)
{
	// check
	tb_assert_and_check_return(aicp && aicp->pool);

	// enter 
	tb_spinlock_enter(&aicp->lock);

	if (aico) 
	{
		// exit pool
		if (aico->pool) tb_spool_exit(aico->pool);
		aico->pool = tb_null;

		// exit it
		tb_rpool_free(aicp->pool, aico);
	}

	// leave 
	tb_spinlock_leave(&aicp->lock);
}
static tb_bool_t tb_aicp_post_after_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

	// the posted aice
	tb_aice_t* posted_aice = (tb_aice_t*)aice->data;
	tb_assert_and_check_return_val(posted_aice, tb_false);

	// the aicp
	tb_aicp_t* aicp = (tb_aicp_t*)tb_aico_aicp(aice->aico);
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);

	// ok?
	tb_bool_t ok = tb_true;
	tb_bool_t posted = tb_true;
	if (aice->state == TB_AICE_STATE_OK)
	{
		// post it	
#ifdef __tb_debug__
		if (!tb_aicp_post_impl(aicp, posted_aice, aice->aico->func, aice->aico->line, aice->aico->file))
#else
		if (!tb_aicp_post_impl(aicp, posted_aice))
#endif
		{
			// not posted
			posted = tb_false;

			// failed
			posted_aice->state = TB_AICE_STATE_FAILED;
		}
	}
	// failed?
	else 
	{
		// not posted
		posted = tb_false;

		// killed?
		if (tb_atomic_get(&aicp->kill) || tb_atomic_get(&aice->aico->killed))
			posted_aice->state = TB_AICE_STATE_KILLED;
		// other error state
		else posted_aice->state = aice->state;
	}

	// not posted? done aicb now
	if (!posted)
	{
		// calling++
		tb_atomic_fetch_and_inc(&aice->aico->calling);

		// done aicb
		if (posted_aice->aicb && !posted_aice->aicb(posted_aice)) ok = tb_false;

		// calling--
		tb_atomic_fetch_and_dec(&aice->aico->calling);
	}

	// exit the posted aice
	tb_aico_pool_free(aice->aico, posted_aice);

	// ok?
	return ok;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_aicp_t* tb_aicp_init(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, tb_null);

	// check iovec
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, data, tb_iovec_t, data), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, size, tb_iovec_t, size), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_send_t, data, tb_iovec_t, data), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_send_t, size, tb_iovec_t, size), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_read_t, data, tb_iovec_t, data), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_read_t, size, tb_iovec_t, size), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_writ_t, data, tb_iovec_t, data), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_writ_t, size, tb_iovec_t, size), tb_null);

	// check real
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, real, tb_aice_send_t, real), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, real, tb_aice_read_t, real), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, real, tb_aice_writ_t, real), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, real, tb_aice_sendv_t, real), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, real, tb_aice_recvv_t, real), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, real, tb_aice_readv_t, real), tb_null);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, real, tb_aice_writv_t, real), tb_null);

	// alloc aicp
	tb_aicp_t* aicp = tb_malloc0(sizeof(tb_aicp_t));
	tb_assert_and_check_return_val(aicp, tb_null);

	// init aicp
	aicp->maxn = maxn;
	aicp->kill = 0;

	// init lock
	if (!tb_spinlock_init(&aicp->lock)) goto fail;

	// init proactor
	aicp->ptor = tb_aicp_proactor_init(aicp);
	tb_assert_and_check_goto(aicp->ptor && aicp->ptor->step >= sizeof(tb_aico_t), fail);

	// init aico pool
	aicp->pool = tb_rpool_init((maxn >> 2) + 16, aicp->ptor->step, 0);
	tb_assert_and_check_goto(aicp->pool, fail);

	// ok
	return aicp;

fail:
	if (aicp) tb_aicp_exit(aicp);
	return tb_null;
}
tb_void_t tb_aicp_exit(tb_aicp_t* aicp)
{
	if (aicp)
	{
		// kill all
		if (!tb_atomic_get(&aicp->kill)) tb_aicp_kill(aicp);
	
		// wait workers 
		tb_hong_t time = tb_mclock();
		while (tb_atomic_get(&aicp->work) && (tb_mclock() < time + 5000)) tb_msleep(500);

		// exit proactor
		if (aicp->ptor)
		{
			tb_assert(aicp->ptor && aicp->ptor->exit);
			aicp->ptor->exit(aicp->ptor);
			aicp->ptor = tb_null;
		}

		// exit aico pool
		tb_spinlock_enter(&aicp->lock);
		if (aicp->pool) tb_rpool_exit(aicp->pool);
		aicp->pool = tb_null;
		tb_spinlock_leave(&aicp->lock);

		// exit lock
		tb_spinlock_exit(&aicp->lock);

		// free aicp
		tb_free(aicp);
	}
}
tb_handle_t tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->addo && type, tb_null);

	// done
	tb_bool_t 	ok = tb_false;
	tb_aico_t* 	aico = tb_null;
	do
	{
		// init aico
		aico = tb_aicp_aico_init(aicp, handle, type);
		tb_assert_and_check_break(aico);

		// addo aico
		if (!aicp->ptor->addo(aicp->ptor, aico)) break;

		// ok
		ok = tb_true;

	} while (0);

	// failed? remove aico
	if (!ok && aico) 
	{
		tb_aicp_aico_exit(aicp, aico);
		aico = tb_null;
	}

	// ok?
	return (tb_handle_t)aico;
}
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t aico, tb_bool_t bcalling)
{
	// check
	tb_assert_and_check_return(aicp && aicp->ptor && aicp->ptor->delo && aico);

	// the aicp have been actived?
	if (!tb_atomic_get(&aicp->kill) || tb_atomic_get(&aicp->work))
	{
		// wait pending 
		tb_size_t tryn = 10;
		while (tb_atomic_get(&((tb_aico_t*)aico)->pending) && tryn--) 
		{
			// trace
			tb_trace_impl("delo: aico: %p, type: %lu: wait pending", aico, tb_aico_type(aico));

			// wait it
			tb_msleep(500);
		}
		if (tb_atomic_get(&((tb_aico_t*)aico)->pending))
		{
			// trace
			tb_trace("[aicp]: delo failed, the aico is pending for func: %s, line: %lu, file: %s", ((tb_aico_t*)aico)->func, ((tb_aico_t*)aico)->line, ((tb_aico_t*)aico)->file);
			return ;
		}

		// wait calling if be not at the self callback
		if (!bcalling)
		{
			tryn = 10;
			while (tb_atomic_get(&((tb_aico_t*)aico)->calling) && tryn--) 
			{
				// trace
				tb_trace_impl("delo: aico: %p, type: %lu: wait calling", aico, tb_aico_type(aico));

				// wait it
				tb_msleep(500);
			}
			if (tb_atomic_get(&((tb_aico_t*)aico)->calling))
			{
				// trace
				tb_trace("[aicp]: delo failed, the aico is calling for func: %s, line: %lu, file: %s", ((tb_aico_t*)aico)->func, ((tb_aico_t*)aico)->line, ((tb_aico_t*)aico)->file);
				return ;
			}
		}
	}

	// delo
	if (aicp->ptor->delo(aicp->ptor, aico)) tb_aicp_aico_exit(aicp, aico);
}
tb_void_t tb_aicp_kilo(tb_aicp_t* aicp, tb_handle_t aico)
{
	// check
	tb_assert_and_check_return(aicp && aicp->ptor && aicp->ptor->kilo && aico);

	// the aicp is killed and not worked?
	tb_check_return(!tb_atomic_get(&aicp->kill) || tb_atomic_get(&aicp->work));

	// pending and not killed? kill it
	if (tb_atomic_get(&((tb_aico_t*)aico)->pending) && !tb_atomic_get(&((tb_aico_t*)aico)->killed)) 
	{
		// killed
		tb_atomic_set(&((tb_aico_t*)aico)->killed, 1);

		// kill it
		aicp->ptor->kilo(aicp->ptor, aico);
	}
}
tb_bool_t tb_aicp_post_impl(tb_aicp_t* aicp, tb_aice_t const* aice __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);
	tb_assert_and_check_return_val(aice && aice->aico, tb_false);

	// killed?
	tb_check_return_val(!tb_atomic_get(&aicp->kill), tb_false);

	// is pending?
	tb_size_t pending = tb_atomic_fetch_and_inc(&aice->aico->pending);
	if (pending)
	{
		// trace
		tb_trace("[aicp]: post aice[%lu] failed, the aico is pending for func: %s, line: %lu, file: %s", aice->code, func_, line_, file_);
		return tb_false;
	}

	// save debug info
#ifdef __tb_debug__
	if (aice->aico)
	{
		aice->aico->func = func_;
		aice->aico->file = file_;
		aice->aico->line = line_;
	}
#endif

	// post aice
	return aicp->ptor->post(aicp->ptor, aice);
}
tb_bool_t tb_aicp_post_after_impl(tb_aicp_t* aicp, tb_size_t delay, tb_aice_t const* aice __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);
	tb_assert_and_check_return_val(aice && aice->aico, tb_false);

	// killed?
	tb_check_return_val(!tb_atomic_get(&aicp->kill), tb_false);

	// no delay?
	if (!delay) return tb_aicp_post_impl(aicp, aice __tb_debug_args__);

	// make the posted aice
	tb_aice_t* posted_aice = (tb_aice_t*)tb_aico_pool_malloc0(aice->aico, sizeof(tb_aice_t));
	tb_assert_and_check_return_val(posted_aice, tb_false);

	// init the posted aice
	*posted_aice = *aice;

	// run the delay task
	return tb_aico_task_run_impl(aice->aico, delay, tb_aicp_post_after_func, posted_aice __tb_debug_args__);
}
tb_void_t tb_aicp_loop(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return(aicp);

	// the ptor 
	tb_aicp_proactor_t* ptor = aicp->ptor;
	tb_assert_and_check_return(ptor && ptor->loop_init && ptor->loop_exit && ptor->loop_spak);

	// the loop spak
	tb_long_t (*loop_spak)(tb_aicp_proactor_t* , tb_handle_t, tb_aice_t* , tb_long_t ) = ptor->loop_spak;

	// worker++
	tb_atomic_fetch_and_inc(&aicp->work);

	// init loop
	tb_handle_t loop = ptor->loop_init(ptor);
	tb_assert_and_check_return(loop);

	// loop
	while (!tb_atomic_get(&aicp->kill))
	{
		// spak
		tb_aice_t 	resp = {0};
		tb_long_t	ok = loop_spak(ptor, loop, &resp, -1);

		// failed? exit all loops
		if (ok < 0) tb_aicp_kill(aicp);

		// killed? break it
		tb_check_break(!tb_atomic_get(&aicp->kill));
		
		// timeout?
		tb_check_continue(ok);

		// check aico
		tb_assert_and_check_continue(resp.aico);

		// check pending
		tb_size_t pending = tb_atomic_fetch_and_set0(&resp.aico->pending);
		tb_assert_and_check_continue(pending == 1);

		// calling++
		tb_atomic_fetch_and_inc(&resp.aico->calling);

		// done aicb
		if (resp.aicb && !resp.aicb(&resp)) 
		{
			// calling--
			tb_atomic_fetch_and_dec(&resp.aico->calling);

			// exit all loops
			tb_aicp_kill(aicp);
			break;
		}

		// calling--
		tb_atomic_fetch_and_dec(&resp.aico->calling);
	}

	// exit loop
	ptor->loop_exit(ptor, loop);

	// worker--
	tb_atomic_fetch_and_dec(&aicp->work);
}
tb_void_t tb_aicp_kill(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return(aicp);

	// kill it
	if (!tb_atomic_fetch_and_set(&aicp->kill, 1))
	{
		// kill proactor
		if (aicp->ptor && aicp->ptor->kill) aicp->ptor->kill(aicp->ptor);
	}
}
tb_hong_t tb_aicp_time(tb_aicp_t* aicp)
{
	return tb_ctime_time();
}
