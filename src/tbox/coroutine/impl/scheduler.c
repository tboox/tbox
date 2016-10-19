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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        scheduler.c
 * @ingroup     coroutine
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "scheduler"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scheduler.h"
#include "coroutine.h"
#include "scheduler_io.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the dead cache maximum count
#ifdef __tb_small__
#   define TB_SCHEDULER_DEAD_CACHE_MAXN     (64)
#else
#   define TB_SCHEDULER_DEAD_CACHE_MAXN     (256)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static __tb_inline__ tb_bool_t tb_co_scheduler_need_io(tb_co_scheduler_t* scheduler)
{
    // check
    tb_assert(scheduler);

    // init io scheduler first
    if (!scheduler->scheduler_io) scheduler->scheduler_io = tb_co_scheduler_io_init(scheduler);
    tb_assert(scheduler->scheduler_io);

    // ok?
    return scheduler->scheduler_io != tb_null;
}
static tb_void_t tb_co_scheduler_make_dead(tb_co_scheduler_t* scheduler, tb_coroutine_t* coroutine)
{
    // check
    tb_assert(scheduler && coroutine);

    // trace
    tb_trace_d("dead coroutine(%p)", coroutine);

    // cannot be original coroutine
    tb_assert(!tb_coroutine_is_original(coroutine));

    // mark this coroutine as dead
    tb_coroutine_state_set(coroutine, TB_STATE_DEAD);

    // append this coroutine to dead coroutines
    tb_list_entry_insert_tail(&scheduler->coroutines_dead, &coroutine->entry);
}
static tb_void_t tb_co_scheduler_make_ready(tb_co_scheduler_t* scheduler, tb_coroutine_t* coroutine)
{
    // check
    tb_assert(scheduler && coroutine);

    // trace
    tb_trace_d("ready coroutine(%p)", coroutine);

    // mark this coroutine as ready
    tb_coroutine_state_set(coroutine, TB_STATE_READY);

    // append this coroutine to ready coroutines
    tb_list_entry_insert_tail(&scheduler->coroutines_ready, &coroutine->entry);
}
static tb_void_t tb_co_scheduler_make_suspend(tb_co_scheduler_t* scheduler, tb_coroutine_t* coroutine)
{
    // check
    tb_assert(scheduler && coroutine);

    // trace
    tb_trace_d("suspend coroutine(%p)", coroutine);

    // mark this coroutine as suspend
    tb_coroutine_state_set(coroutine, TB_STATE_SUSPEND);

    // append this coroutine to suspend coroutines
    tb_list_entry_insert_tail(&scheduler->coroutines_suspend, &coroutine->entry);
}
static tb_coroutine_t* tb_co_scheduler_next_ready(tb_co_scheduler_t* scheduler)
{
    // check
    tb_assert(scheduler);
 
    // no more?
    if (!tb_list_entry_size(&scheduler->coroutines_ready)) return tb_null;

    // get the next entry from head
    tb_list_entry_ref_t entry = tb_list_entry_head(&scheduler->coroutines_ready);
    tb_assert(entry);

    // remove it from the ready coroutines
    tb_list_entry_remove_head(&scheduler->coroutines_ready);

    // trace
    tb_trace_d("get next coroutine(%p)", tb_list_entry(&scheduler->coroutines_ready, entry));

    // return this coroutine
    return (tb_coroutine_t*)tb_list_entry(&scheduler->coroutines_ready, entry);
}
static tb_void_t tb_co_scheduler_switch_next(tb_co_scheduler_t* scheduler)
{
    // check
    tb_assert(scheduler);

    // switch to other coroutine? 
    if (tb_list_entry_size(&scheduler->coroutines_ready))
    {
        // get the next coroutine 
        tb_coroutine_t* coroutine = tb_co_scheduler_next_ready(scheduler);
        tb_assert(coroutine);

        // switch to the next coroutine
        tb_co_scheduler_switch(scheduler, coroutine);
    }
    // no more ready coroutines? 
    else
    {
        // trace
        tb_trace_d("switch to original from coroutine(%p)", tb_coroutine_self());

        // switch to the original coroutine
        tb_co_scheduler_switch(scheduler, &scheduler->original);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_co_scheduler_start(tb_co_scheduler_t* scheduler, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize)
{
    // check
    tb_assert(func);

    // done
    tb_bool_t       ok = tb_false;
    tb_coroutine_t* coroutine = tb_null;
    do
    {
        // trace
        tb_trace_d("start ..");

        // uses the current scheduler if be null
        if (!scheduler) scheduler = (tb_co_scheduler_t*)tb_co_scheduler_self();
        tb_assert_and_check_break(scheduler);

        // have been stopped? do not continue to start new coroutines
        tb_check_break(!scheduler->stopped);

        // reuses dead coroutines in init function
        if (tb_list_entry_size(&scheduler->coroutines_dead))
        {
            // get the next entry from head
            tb_list_entry_ref_t entry = tb_list_entry_head(&scheduler->coroutines_dead);
            tb_assert_and_check_break(entry);

            // remove it from the ready coroutines
            tb_list_entry_remove_head(&scheduler->coroutines_dead);

            // get the dead coroutine
            tb_coroutine_t* coroutine_dead = (tb_coroutine_t*)tb_list_entry(&scheduler->coroutines_dead, entry);

            // reinit this coroutine
            coroutine = tb_coroutine_reinit(coroutine_dead, func, priv, stacksize);

            // failed? exit this coroutine
            if (!coroutine) tb_coroutine_exit(coroutine_dead);
        }

        // init coroutine
        if (!coroutine) coroutine = tb_coroutine_init((tb_co_scheduler_ref_t)scheduler, func, priv, stacksize);
        tb_assert_and_check_break(coroutine);

        // ready coroutine
        tb_co_scheduler_make_ready(scheduler, coroutine);

        // the dead coroutines is too much? free some coroutines
        while (tb_list_entry_size(&scheduler->coroutines_dead) > TB_SCHEDULER_DEAD_CACHE_MAXN)
        {
            // get the next entry from head
            tb_list_entry_ref_t entry = tb_list_entry_head(&scheduler->coroutines_dead);
            tb_assert(entry);

            // remove it from the ready coroutines
            tb_list_entry_remove_head(&scheduler->coroutines_dead);

            // exit this coroutine
            tb_coroutine_exit((tb_coroutine_t*)tb_list_entry(&scheduler->coroutines_dead, entry));
        }

        // ok
        ok = tb_true;

    } while (0);

    // trace
    tb_trace_d("start %s", ok? "ok" : "no");

    // ok?
    return ok;
}
tb_bool_t tb_co_scheduler_yield(tb_co_scheduler_t* scheduler)
{
    // check
    tb_assert(scheduler && scheduler->running);
    tb_assert(tb_coroutine_is_running(scheduler->running));
    tb_assert(scheduler->running == (tb_coroutine_t*)tb_coroutine_self());

    // trace
    tb_trace_d("yield coroutine(%p)", scheduler->running);

    // exists other ready coroutines?
    if (tb_list_entry_size(&scheduler->coroutines_ready))
    {
        // make the running coroutine as ready
        tb_co_scheduler_make_ready(scheduler, scheduler->running);

        // get the next coroutine 
        tb_coroutine_t* coroutine = tb_co_scheduler_next_ready(scheduler);
        tb_assert(coroutine);

        // switch to the next coroutine
        tb_co_scheduler_switch(scheduler, coroutine);

        // ok
        return tb_true;
    }

    // trace
    tb_trace_d("continue to run current coroutine(%p)", tb_coroutine_self());

    // no more ready coroutines? return it directly and continue to run this coroutine
    return tb_false;
}
tb_cpointer_t tb_co_scheduler_resume(tb_co_scheduler_t* scheduler, tb_coroutine_t* coroutine, tb_cpointer_t priv)
{
    // check
    tb_assert(scheduler && coroutine);
    tb_assert(tb_coroutine_is_suspend(coroutine) || tb_coroutine_is_ready(coroutine));

    // trace
    tb_trace_d("resume coroutine(%p)", coroutine);

    // this coroutine is suspended?
    tb_cpointer_t retval = tb_null;
    if (tb_coroutine_is_suspend(coroutine))
    {
        // remove it from the suspend coroutines
        tb_list_entry_remove(&scheduler->coroutines_suspend, &coroutine->entry);

        // get the passed private data from suspend(priv)
        retval = coroutine->rs_priv;

        // pass the user private data to suspend()
        coroutine->rs_priv = priv;

        // make it as ready
        tb_co_scheduler_make_ready(scheduler, coroutine);
    }

    // return it
    return retval;
}
tb_cpointer_t tb_co_scheduler_suspend(tb_co_scheduler_t* scheduler, tb_cpointer_t priv)
{
    // check
    tb_assert(scheduler && scheduler->running);
    tb_assert(tb_coroutine_is_running(scheduler->running));
    tb_assert(scheduler->running == (tb_coroutine_t*)tb_coroutine_self());

    // have been stopped? return it directly
    tb_check_return_val(!scheduler->stopped, tb_null);

    // trace
    tb_trace_d("suspend coroutine(%p)", scheduler->running);

    // pass the private data to resume() first
    scheduler->running->rs_priv = priv;

    // make the running coroutine as suspend
    tb_co_scheduler_make_suspend(scheduler, scheduler->running);

    // switch to next coroutine 
    tb_co_scheduler_switch_next(scheduler);

    // check
    tb_assert(scheduler->running);

    // return the user private data from resume(priv)
    return scheduler->running->rs_priv;
}
tb_void_t tb_co_scheduler_finish(tb_co_scheduler_t* scheduler)
{
    // check
    tb_assert(scheduler && scheduler->running);
    tb_assert(tb_coroutine_is_running(scheduler->running));
    tb_assert(scheduler->running == (tb_coroutine_t*)tb_coroutine_self());

    // trace
    tb_trace_d("finish coroutine(%p)", scheduler->running);

    // make the running coroutine as dead
    tb_co_scheduler_make_dead(scheduler, scheduler->running);

    // switch to next coroutine 
    tb_co_scheduler_switch_next(scheduler);
}
tb_cpointer_t tb_co_scheduler_sleep(tb_co_scheduler_t* scheduler, tb_long_t interval)
{
    // check
    tb_assert(scheduler && scheduler->running);
    tb_assert(tb_coroutine_is_running(scheduler->running));
    tb_assert(scheduler->running == (tb_coroutine_t*)tb_coroutine_self());

    // have been stopped? return it directly
    tb_check_return_val(!scheduler->stopped, tb_null);

    // need io scheduler
    if (!tb_co_scheduler_need_io(scheduler)) return tb_null;

    // sleep it
    return tb_co_scheduler_io_sleep(scheduler->scheduler_io, interval);
}
tb_void_t tb_co_scheduler_switch(tb_co_scheduler_t* scheduler, tb_coroutine_t* coroutine)
{
    // check
    tb_assert(scheduler && scheduler->running);
    tb_assert(coroutine && coroutine->context);

    // the current running coroutine
    tb_coroutine_t* running = scheduler->running;

    // mark the given coroutine as running
    tb_coroutine_state_set(coroutine, TB_STATE_RUNNING);
    scheduler->running = coroutine;

    // trace
    tb_trace_d("switch to coroutine(%p) from coroutine(%p)", coroutine, running);

    // jump to the given coroutine
    tb_context_from_t from = tb_context_jump(coroutine->context, running);

    // the from-coroutine 
    tb_coroutine_t* coroutine_from = (tb_coroutine_t*)from.priv;
    tb_assert(coroutine_from && from.context);

#ifdef __tb_debug__
    // check it
    tb_coroutine_check(coroutine_from);
#endif

    // update the context
    coroutine_from->context = from.context;
}
tb_long_t tb_co_scheduler_wait(tb_co_scheduler_t* scheduler, tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_assert(scheduler && scheduler->running);
    tb_assert(tb_coroutine_is_running(scheduler->running));
    tb_assert(scheduler->running == (tb_coroutine_t*)tb_coroutine_self());

    // have been stopped? return it directly
    tb_check_return_val(!scheduler->stopped, -1);

    // need io scheduler
    if (!tb_co_scheduler_need_io(scheduler)) return -1;

    // sleep it
    return tb_co_scheduler_io_wait(scheduler->scheduler_io, sock, events, timeout);
}
