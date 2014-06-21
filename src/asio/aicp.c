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
static tb_aico_t* tb_aicp_aico_init(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type)
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
static tb_bool_t tb_aicp_aico_wait(tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_aico_t* aico = (tb_aico_t*)item;
    tb_assert_and_check_return_val(aico, tb_false);

    // trace
#ifdef __tb_debug__
    tb_trace_e("aico[%p]: wait exited failed, type: %lu, handle: %p, state: %s for func: %s, line: %lu, file: %s", aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(tb_atomic_get(&aico->state)), aico->func, aico->line, aico->file);
#else
    tb_trace_e("aico[%p]: wait exited failed, type: %lu, handle: %p, state: %s", aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(tb_atomic_get(&aico->state)));
#endif

    // ok
    return tb_true;
}
static tb_bool_t tb_aicp_aico_kill(tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_aico_t* aico = (tb_aico_t*)item;
    tb_aicp_t* aicp = (tb_aicp_t*)priv;
    tb_assert_and_check_return_val(aico && aicp, tb_false);

    // kill it
    tb_aicp_kilo(aicp, aico);

    // ok
    return tb_true;
}
static tb_void_t tb_aicp_aico_exit_func(tb_handle_t aico, tb_cpointer_t priv)
{
    // check
    tb_atomic_t* wait = (tb_atomic_t*)priv;
    tb_assert_and_check_return(wait);

    // exit ok
    tb_atomic_set(wait, 1);
}
static tb_bool_t tb_aicp_post_after_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the posted aice
    tb_aice_t* posted_aice = (tb_aice_t*)aice->priv;
    tb_assert_and_check_return_val(posted_aice && posted_aice->aico, tb_false);

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
    tb_aicp_t* aicp = (tb_aicp_t*)priv;

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
    tb_assert_and_check_return_val(tb_memberof_eq(tb_aice_recv_t, real, tb_aice_sendf_t, real), tb_null);
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
        aicp = (tb_aicp_t*)tb_malloc0(sizeof(tb_aicp_t));
        tb_assert_and_check_break(aicp);

        // init aicp
#ifdef __tb_small__
        aicp->maxn = maxn? maxn : (1 << 8);
#else
        aicp->maxn = maxn? maxn : (1 << 16);
#endif

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
tb_bool_t tb_aicp_exit(tb_aicp_t* aicp)
{
    // check
    tb_assert_and_check_return_val(aicp, tb_false);

    // kill all first
    tb_aicp_kill_all(aicp);

    // wait all exiting
    if (tb_aicp_wait_all(aicp, 5000) <= 0) 
    {
        // wait failed, trace left aicos
        tb_spinlock_enter(&aicp->lock);
        if (aicp->pool) tb_fixed_pool_walk(aicp->pool, tb_aicp_aico_wait, tb_null);
        tb_spinlock_leave(&aicp->lock);
        return tb_false;
    }
   
    // kill loop
    tb_aicp_kill(aicp);

    // wait workers exiting 
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
    if (aicp->pool) tb_fixed_pool_exit(aicp->pool);
    aicp->pool = tb_null;
    tb_spinlock_leave(&aicp->lock);

    // exit lock
    tb_spinlock_exit(&aicp->lock);

    // free aicp
    tb_free(aicp);

    // ok
    return tb_true;
}
tb_handle_t tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type)
{
    // check
    tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->addo && type, tb_null);

    // done
    tb_bool_t   ok = tb_false;
    tb_aico_t*  aico = tb_null;
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
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t aico, tb_aico_exit_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(aicp && aicp->ptor && aicp->ptor->delo && aico);

    // no func? wait exiting
    tb_atomic_t wait = 0;
    if (!func)
    {
        func = tb_aicp_aico_exit_func;
        priv = (tb_cpointer_t)&wait;
    }
    else tb_atomic_set(&wait, 1);

    // save func
    ((tb_aico_t*)aico)->exit = func;
    ((tb_aico_t*)aico)->priv = priv;

    // trace
    tb_trace_d("exit: aico[%p]: type: %lu, handle: %p: state: %s: ..", aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(tb_atomic_get(&((tb_aico_t*)aico)->state)));

    // exit it directly if be ok or killed, otherwise wait exiting
    tb_size_t state = TB_STATE_OK;
    if (TB_STATE_OK == (state = tb_atomic_fetch_and_set(&((tb_aico_t*)aico)->state, TB_STATE_EXITING)) || (state == TB_STATE_KILLED))
    {
        // done exit
        if (((tb_aico_t*)aico)->exit) ((tb_aico_t*)aico)->exit(aico, ((tb_aico_t*)aico)->priv);

        // clear func
        ((tb_aico_t*)aico)->exit = tb_null;
        ((tb_aico_t*)aico)->priv = tb_null;
 
        // trace
        tb_trace_d("exit: aico[%p]: type: %lu, handle: %p: ok", aico, tb_aico_type(aico), tb_aico_handle(aico));

        // exit it
        if (aicp->ptor->delo(aicp->ptor, (tb_aico_t*)aico)) tb_aicp_aico_exit(aicp, (tb_aico_t*)aico);
    }

    // wait exiting
    tb_size_t tryn = 20;
    while (!tb_atomic_get(&wait) && tryn--)
    {
        // trace
        tb_trace_d("exit: aico[%p]: waiting: ..", aico);

        // wait it
        tb_msleep(200);
    }

    // check
    if (!tb_atomic_get(&wait))
    {
        // trace
        tb_trace_e("exit: aico[%p]: failed", aico);
    }
}
tb_void_t tb_aicp_kilo(tb_aicp_t* aicp, tb_handle_t aico)
{
    // check
    tb_assert_and_check_return(aicp && aicp->ptor && aicp->ptor->kilo && aico);

    // the aicp is killed and not worked?
    tb_check_return(!tb_atomic_get(&aicp->kill) || tb_atomic_get(&aicp->work));

    // trace
    tb_trace_d("kill: aico[%p]: type: %lu, handle: %p: state: %s: ..", aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(tb_atomic_get(&((tb_aico_t*)aico)->state)));

    // ok? killed
    if (TB_STATE_OK == tb_atomic_fetch_and_pset(&((tb_aico_t*)aico)->state, TB_STATE_OK, TB_STATE_KILLED))
    { 
        // trace
        tb_trace_d("kill: aico[%p]: type: %lu, handle: %p: ok", aico, tb_aico_type(aico), tb_aico_handle(aico));
    }
    // pending? kill it
    else if (TB_STATE_PENDING == tb_atomic_fetch_and_pset(&((tb_aico_t*)aico)->state, TB_STATE_PENDING, TB_STATE_KILLING)) 
    {
        // kill it
        aicp->ptor->kilo(aicp->ptor, (tb_aico_t*)aico);

        // trace
        tb_trace_d("kill: aico[%p]: type: %lu, handle: %p: state: pending: ok", aico, tb_aico_type(aico), tb_aico_handle(aico));
    }
}
tb_bool_t tb_aicp_post_(tb_aicp_t* aicp, tb_aice_t const* aice __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);
    tb_assert_and_check_return_val(aice && aice->aico, tb_false);

    // killed?
    tb_check_return_val(!tb_atomic_get(&aicp->kill_all), tb_false);

    // the aico
    tb_aico_t* aico = (tb_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico, tb_false);

    // ok? pending it
    tb_size_t state = tb_atomic_fetch_and_pset(&aice->aico->state, TB_STATE_OK, TB_STATE_PENDING);
    if (state != TB_STATE_OK)
    {
        // pending? error
        if (state == TB_STATE_PENDING)
        {
            // trace
#ifdef __tb_debug__
            tb_trace_e("post aice[%lu] failed, the aico[%p]: type: %lu, handle: %p, state: %s for func: %s, line: %lu, file: %s", aice->code, aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(state), func_, line_, file_);
#else
            tb_trace_e("post aice[%lu] failed, the aico[%p]: type: %lu, handle: %p, state: %s", aice->code, aico, tb_aico_type(aico), tb_aico_handle(aico), tb_state_cstr(state));
#endif

            // abort it
            tb_assert_abort(0);
        }
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
    tb_check_return_val(!tb_atomic_get(&aicp->kill_all), tb_false);

    // no delay?
    if (!delay) return tb_aicp_post_(aicp, aice __tb_debug_args__);

    // the aico
    tb_aico_t* aico = (tb_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico, tb_false);

    // make the posted aice
    tb_aice_t* posted_aice = (tb_aice_t*)tb_aico_pool_malloc0(aico, sizeof(tb_aice_t));
    tb_assert_and_check_return_val(posted_aice, tb_false);

    // init the posted aice
    *posted_aice = *aice;

    // run the delay task
    return tb_aico_task_run_(aico, delay, tb_aicp_post_after_func, posted_aice __tb_debug_args__);
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
    tb_assert_and_check_return(ptor && ptor->delo && ptor->loop_spak);

    // the loop spak
    tb_long_t (*loop_spak)(tb_aicp_proactor_t* , tb_handle_t, tb_aice_t* , tb_long_t ) = ptor->loop_spak;

    // worker++
    tb_atomic_fetch_and_inc(&aicp->work);

    // init loop
    tb_handle_t loop = ptor->loop_init? ptor->loop_init(ptor) : tb_null;
 
    // trace
    tb_trace_d("loop[%p]: init", loop);

    // spak ctime
    tb_cache_time_spak();

    // loop
    while (1)
    {
        // spak
        tb_aice_t   resp = {0};
        tb_long_t   ok = loop_spak(ptor, loop, &resp, -1);

        // spak ctime
        tb_cache_time_spak();

        // failed?
        tb_check_break(ok >= 0);

        // timeout?
        tb_check_continue(ok);

        // check aico
        tb_assert_and_check_continue(resp.aico);

        // trace
        tb_trace_d("loop[%p]: spak: code: %lu, aico: %p, state: %s: %ld", loop, resp.code, resp.aico, resp.aico? tb_state_cstr(tb_atomic_get(&resp.aico->state)) : "null", ok);

        // pending? clear state
        tb_size_t state = TB_STATE_OK;
        state = tb_atomic_fetch_and_pset(&resp.aico->state, TB_STATE_PENDING, state);

        // killed? update the aice state 
        if ((state == TB_STATE_KILLING) || (state == TB_STATE_EXITING) || (state == TB_STATE_KILLED))
            resp.state = TB_STATE_KILLED;

        // done func, @note maybe the aico exit will be called
        if (resp.func && !resp.func(&resp)) 
        {
            // trace
#ifdef __tb_debug__
            tb_trace_e("loop[%p]: done aice func failed with code: %lu at line: %lu, func: %s, file: %s!", loop, resp.code, resp.aico->line, resp.aico->func, resp.aico->file);
#else
            tb_trace_e("loop[%p]: done aice func failed with code: %lu!", loop, resp.code);
#endif
        }

        // exiting? exit it
        if (state == TB_STATE_EXITING || (state == TB_STATE_KILLING && tb_atomic_get(&resp.aico->state) == TB_STATE_EXITING))
        {
            // done exit
            if (resp.aico->exit) resp.aico->exit(resp.aico, resp.aico->priv);

            // clear func
            resp.aico->exit = tb_null;
            resp.aico->priv = tb_null;
 
            // trace
            tb_trace_d("loop[%p]: exit: aico[%p]: type: %lu, handle: %p: ok", loop, resp.aico, tb_aico_type(resp.aico), tb_aico_handle(resp.aico));

            // exit it
            if (ptor->delo(ptor, resp.aico)) tb_aicp_aico_exit(aicp, resp.aico);
        }
        // killing? change state to be killed
        else tb_atomic_fetch_and_pset(&resp.aico->state, TB_STATE_KILLING, TB_STATE_KILLED);

        // stop it?
        if (stop && stop(priv)) tb_aicp_kill(aicp);
    }

    // exit loop
    if (ptor->loop_exit) ptor->loop_exit(ptor, loop);

    // worker--
    tb_atomic_fetch_and_dec(&aicp->work);

    // trace
    tb_trace_d("loop[%p]: exit", loop);
}
tb_void_t tb_aicp_kill(tb_aicp_t* aicp)
{
    // check
    tb_assert_and_check_return(aicp);
        
    // trace
    tb_trace_d("kill: ..");

    // kill all
    tb_aicp_kill_all(aicp);

    // kill it
    if (!tb_atomic_fetch_and_set(&aicp->kill, 1))
    {
        // kill proactor
        if (aicp->ptor && aicp->ptor->kill) aicp->ptor->kill(aicp->ptor);
    }
}
tb_void_t tb_aicp_kill_all(tb_aicp_t* aicp)
{
    // check
    tb_assert_and_check_return(aicp);

    // trace
    tb_trace_d("kill: all: ..");

    // kill all
    if (!tb_atomic_fetch_and_set(&aicp->kill_all, 1))
    {
        tb_spinlock_enter(&aicp->lock);
        if (aicp->pool) tb_fixed_pool_walk(aicp->pool, tb_aicp_aico_kill, aicp);
        tb_spinlock_leave(&aicp->lock);
    }
}
tb_long_t tb_aicp_wait_all(tb_aicp_t* aicp, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(aicp, -1);

    // trace
    tb_trace_d("wait: all: ..");

    // wait it
    tb_size_t size = 0;
    tb_hong_t time = tb_cache_time_spak();
    while ((timeout < 0 || tb_cache_time_spak() < time + timeout))
    {
        // enter
        tb_spinlock_enter(&aicp->lock);

        // the aico count
        size = aicp->pool? tb_fixed_pool_size(aicp->pool) : 0;

        // trace
        tb_trace_d("wait: count: %lu: ..", size);

        // leave
        tb_spinlock_leave(&aicp->lock);

        // ok?
        tb_check_break(size);

        // wait some time
        tb_msleep(200);
    }

    // ok?
    return !size? 1 : 0;
}
tb_hong_t tb_aicp_time(tb_aicp_t* aicp)
{
    return tb_cache_time_mclock();
}
