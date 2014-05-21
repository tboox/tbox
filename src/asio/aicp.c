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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        aicp.c
 * @ingroup     asio
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "aicp"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"
#include "aioo.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp);

/* //////////////////////////////////////////////////////////////////////////////////////
 * aico implementation
 */
static tb_aico_t* tb_aicp_aico_init(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type, tb_aico_exit_func_t exit, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(aicp && aicp->pool && type, tb_null);

    // enter 
    tb_spinlock_enter(&aicp->lock);

    // make aico
    tb_aico_t* aico = (tb_aico_t*)tb_fixed_pool_malloc0(aicp->pool);

    // init aico
    if (aico)
    {
        aico->aicp      = aicp;
        aico->type      = type;
        aico->handle    = handle;
        aico->pool      = tb_null;
        aico->exit      = exit;
        aico->priv      = priv;
        aico->state     = TB_STATE_OK;

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
    tb_assert_and_check_return(aico && TB_STATE_EXITING == tb_atomic_get(&aico->state));

    // enter 
    tb_spinlock_enter(&aicp->lock);

    // done
    if (aico) 
    {
        // trace
        tb_trace_d("exit: aico[%p]: type: %lu, handle: %p, state: %s", aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(tb_atomic_get(&aico->state)));
        
        // exit pool
        if (aico->pool) tb_block_pool_exit(aico->pool);
        aico->pool = tb_null;

        // exit it
        tb_fixed_pool_free(aicp->pool, aico);
    }

    // leave 
    tb_spinlock_leave(&aicp->lock);
}
static tb_bool_t tb_aicp_aico_walk(tb_pointer_t item, tb_pointer_t data)
{
    // check
    tb_aico_t* aico = (tb_aico_t*)item;
    tb_assert_and_check_return_val(aico, tb_false);

    // trace
    tb_trace_w("exit: left: aico[%p]: type: %lu, handle: %p, state: %s", aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(tb_atomic_get(&aico->state)));

    // done exit
    if (aico->exit) aico->exit(aico, aico->priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_aicp_post_after_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the posted aice
    tb_aice_t* posted_aice = (tb_aice_t*)aice->priv;
    tb_assert_and_check_return_val(posted_aice, tb_false);

    // the aicp
    tb_aicp_t* aicp = (tb_aicp_t*)tb_aico_aicp(aice->aico);
    tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);

    // ok?
    tb_bool_t ok = tb_true;
    tb_bool_t posted = tb_true;
    if (aice->state == TB_STATE_OK)
    {
        // post it  
#ifdef __tb_debug__
        if (!tb_aicp_post_(aicp, posted_aice, aice->aico->func, aice->aico->line, aice->aico->file))
#else
        if (!tb_aicp_post_(aicp, posted_aice))
#endif
        {
            // not posted
            posted = tb_false;

            // failed
            posted_aice->state = TB_STATE_FAILED;
        }
    }
    // failed?
    else 
    {
        // not posted
        posted = tb_false;

        // save state
        posted_aice->state = aice->state;
    }

    // not posted? done func now
    if (!posted)
    {
        // done func: notify failed
        if (posted_aice->func && !posted_aice->func(posted_aice)) ok = tb_false;
    }

    // exit the posted aice
    tb_aico_pool_free(aice->aico, posted_aice);

    // ok?
    return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_pointer_t tb_aicp_instance_loop(tb_cpointer_t priv)
{
    // aicp
    tb_handle_t aicp = (tb_handle_t)priv;

    // trace
    tb_trace_d("loop: init");

    // loop aicp
    if (aicp) tb_aicp_loop(aicp);
    
    // trace
    tb_trace_d("loop: exit");

    // exit
    tb_thread_return(tb_null);
    return tb_null;
}
static tb_handle_t tb_aicp_instance_init(tb_cpointer_t* ppriv)
{
    // check
    tb_assert_and_check_return_val(ppriv, tb_null);

    // done
    tb_bool_t   ok = tb_false;
    tb_aicp_t*  aicp = tb_null;
    do
    {
        // init aicp
        aicp = tb_aicp_init(0);
        tb_assert_and_check_break(aicp);

        // init loop
        *ppriv = (tb_cpointer_t)tb_thread_init(tb_null, tb_aicp_instance_loop, aicp, 0);
        tb_assert_and_check_break(*ppriv);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit aicp
        if (aicp) tb_aicp_exit(aicp);
        aicp = tb_null;
    }

    // ok?
    return (tb_handle_t)aicp;
}
static tb_void_t tb_aicp_instance_exit(tb_handle_t handle, tb_cpointer_t priv)
{
    // exit loop
    tb_handle_t loop = (tb_handle_t)priv;
    if (loop)
    {
        // wait it
        tb_long_t wait = 0;
        if (!(wait = tb_thread_wait(loop, 5000)))
        {
            // trace
            tb_trace_e("loop[%p]: wait failed: %ld!", loop, wait);
        }

        // exit it
        tb_thread_exit(loop);
    }

    // exit it
    tb_aicp_exit((tb_aicp_t*)handle);
}
static tb_void_t tb_aicp_instance_kill(tb_handle_t handle, tb_cpointer_t priv)
{
    // kill it
    tb_aicp_kill((tb_aicp_t*)handle);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_aicp_t* tb_aicp()
{
    return (tb_aicp_t*)tb_singleton_instance(TB_SINGLETON_TYPE_AICP, tb_aicp_instance_init, tb_aicp_instance_exit, tb_aicp_instance_kill);
}
tb_aicp_t* tb_aicp_init(tb_size_t maxn)
{
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

    // done
    tb_aicp_t*  aicp = tb_null;
    tb_bool_t   ok = tb_false;
    do
    {
        // make aicp
        aicp = tb_malloc0(sizeof(tb_aicp_t));
        tb_assert_and_check_break(aicp);

        // init aicp
        aicp->maxn = maxn? maxn : (1 << 16);
        aicp->kill = 0;

        // init lock
        if (!tb_spinlock_init(&aicp->lock)) break;

        // init proactor
        aicp->ptor = tb_aicp_proactor_init(aicp);
        tb_assert_and_check_break(aicp->ptor && aicp->ptor->step >= sizeof(tb_aico_t));

        // init aico pool
        aicp->pool = tb_fixed_pool_init((aicp->maxn >> 2) + 16, aicp->ptor->step, 0);
        tb_assert_and_check_break(aicp->pool);

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&aicp->lock, TB_TRACE_MODULE_NAME);
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok) 
    {
        // exit aicp
        if (aicp) tb_aicp_exit(aicp);
        aicp = tb_null;
    }

    // ok?
    return aicp;
}
tb_void_t tb_aicp_exit(tb_aicp_t* aicp)
{
    // check
    tb_assert_and_check_return(aicp);
    
    // kill it first
    tb_aicp_kill(aicp);

    // wait workers exiting 
    tb_hong_t time = tb_mclock();
    while (tb_atomic_get(&aicp->work) && (tb_mclock() < time + 5000)) tb_msleep(500);

    // exit all left aico first
    tb_spinlock_enter(&aicp->lock);
    if (aicp->pool) tb_fixed_pool_walk(aicp->pool, tb_aicp_aico_walk, tb_null);
    tb_spinlock_leave(&aicp->lock);

    // exit proactor
    if (aicp->ptor)
    {
        tb_assert(aicp->ptor && aicp->ptor->exit);
        aicp->ptor->exit(aicp->ptor);
        aicp->ptor = tb_null;
    }

    // exit aico pool
    tb_spinlock_enter(&aicp->lock);
    if (aicp->pool) tb_fixed_pool_exit(aicp->pool);
    aicp->pool = tb_null;
    tb_spinlock_leave(&aicp->lock);

    // exit lock
    tb_spinlock_exit(&aicp->lock);

    // free aicp
    tb_free(aicp);
}
tb_handle_t tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type, tb_aico_exit_func_t exit, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->addo && type, tb_null);

    // done
    tb_bool_t   ok = tb_false;
    tb_aico_t*  aico = tb_null;
    do
    {
        // init aico
        aico = tb_aicp_aico_init(aicp, handle, type, exit, priv);
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
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t aico)
{
    // check
    tb_assert_and_check_return(aicp && aicp->ptor && aicp->ptor->delo && aico);

    // no pending and killing? exit it directly
    if (TB_STATE_OK == tb_atomic_fetch_and_set(&((tb_aico_t*)aico)->state, TB_STATE_EXITING))
    {
        // done exit
        if (((tb_aico_t*)aico)->exit) ((tb_aico_t*)aico)->exit(aico, ((tb_aico_t*)aico)->priv);

        // exit it
        if (aicp->ptor->delo(aicp->ptor, aico)) tb_aicp_aico_exit(aicp, aico);
    }
}
tb_void_t tb_aicp_kilo(tb_aicp_t* aicp, tb_handle_t aico)
{
    // check
    tb_assert_and_check_return(aicp && aicp->ptor && aicp->ptor->kilo && aico);

    // the aicp is killed and not worked?
    tb_check_return(!tb_atomic_get(&aicp->kill) || tb_atomic_get(&aicp->work));

    // pending? kill it
    if (TB_STATE_PENDING == tb_atomic_fetch_and_pset(&((tb_aico_t*)aico)->state, TB_STATE_PENDING, TB_STATE_KILLING)) 
    {
        // trace
        tb_trace_d("kill: pending aico[%p]: type: %lu, handle: %p: ..", aico, tb_aico_type(aico), tb_aico_handle(aico));

        // kill it
        aicp->ptor->kilo(aicp->ptor, aico);
    }
}
tb_bool_t tb_aicp_post_(tb_aicp_t* aicp, tb_aice_t const* aice __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);
    tb_assert_and_check_return_val(aice && aice->aico, tb_false);

    // killed?
    tb_check_return_val(!tb_atomic_get(&aicp->kill), tb_false);

    // the aico
    tb_aico_t* aico = (tb_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico, tb_false);

    // ok? pending it
    tb_size_t state = tb_atomic_fetch_and_pset(&aice->aico->state, TB_STATE_OK, TB_STATE_PENDING);
    if (state != TB_STATE_OK)
    {
        // trace
#ifdef __tb_debug__
        tb_trace_e("post aice[%lu] failed, the aico[%p]: type: %lu, handle: %p, state: %s for func: %s, line: %lu, file: %s", aice->code, aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(state), func_, line_, file_);
#else
        tb_trace_e("post aice[%lu] failed, the aico[%p]: type: %lu, handle: %p, state: %s", aice->code, aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(state));
#endif
        return tb_false;
    }

    // save debug info
#ifdef __tb_debug__
    aico->func = func_;
    aico->file = file_;
    aico->line = line_;
#endif

    // post aice
    return aicp->ptor->post(aicp->ptor, aice);
}
tb_bool_t tb_aicp_post_after_(tb_aicp_t* aicp, tb_size_t delay, tb_aice_t const* aice __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);
    tb_assert_and_check_return_val(aice && aice->aico, tb_false);

    // killed?
    tb_check_return_val(!tb_atomic_get(&aicp->kill), tb_false);

    // no delay?
    if (!delay) return tb_aicp_post_(aicp, aice __tb_debug_args__);

    // make the posted aice
    tb_aice_t* posted_aice = (tb_aice_t*)tb_aico_pool_malloc0(aice->aico, sizeof(tb_aice_t));
    tb_assert_and_check_return_val(posted_aice, tb_false);

    // init the posted aice
    *posted_aice = *aice;

    // run the delay task
    return tb_aico_task_run_(aice->aico, delay, tb_aicp_post_after_func, posted_aice __tb_debug_args__);
}
tb_void_t tb_aicp_loop(tb_aicp_t* aicp)
{
    tb_aicp_loop_util(aicp, tb_null, tb_null);  
}
tb_void_t tb_aicp_loop_util(tb_aicp_t* aicp, tb_bool_t (*stop)(tb_cpointer_t priv), tb_cpointer_t priv)
{   
    // check
    tb_assert_and_check_return(aicp);

    // the ptor 
    tb_aicp_proactor_t* ptor = aicp->ptor;
    tb_assert_and_check_return(ptor && ptor->delo && ptor->loop_init && ptor->loop_exit && ptor->loop_spak);

    // the loop spak
    tb_long_t (*loop_spak)(tb_aicp_proactor_t* , tb_handle_t, tb_aice_t* , tb_long_t ) = ptor->loop_spak;

    // worker++
    tb_atomic_fetch_and_inc(&aicp->work);

    // init loop
    tb_handle_t loop = ptor->loop_init(ptor);
    tb_assert_and_check_return(loop);

    // loop
    while (1)
    {
        // spak
        tb_aice_t   resp = {0};
        tb_long_t   ok = loop_spak(ptor, loop, &resp, -1);
        tb_check_break(ok >= 0);

        // timeout?
        tb_check_continue(ok);

        // check aico
        tb_assert_and_check_continue(resp.aico);

        // kill? force updating state to be killing
        tb_size_t state = TB_STATE_OK;
        if (tb_atomic_get(&aicp->kill))
        {
            resp.state = TB_STATE_KILLED;
            state = TB_STATE_KILLING;
        }

        // pending? clear state
        state = tb_atomic_fetch_and_pset(&resp.aico->state, TB_STATE_PENDING, state);

        // done func
        if (resp.func && !resp.func(&resp)) 
        {
            // trace
#ifdef __tb_debug__
            tb_trace_e("done aice func failed at line: %lu, func: %s, file: %s!", resp.aico->line, resp.aico->func, resp.aico->file);
#else
            tb_trace_e("done aice func failed!");
#endif
        }

        // exiting? exit it
        if (state == TB_STATE_EXITING)
        {
            // done exit
            if (resp.aico->exit) resp.aico->exit(resp.aico, resp.aico->priv);

            // exit it
            if (ptor->delo(ptor, resp.aico)) tb_aicp_aico_exit(aicp, resp.aico);
        }

        // stop it?
        if (stop && stop(priv)) tb_aicp_kill(aicp);
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
    return tb_cache_time_mclock();
}
