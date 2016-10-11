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
 * @file        scheduler.h
 * @ingroup     scheduler
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
#include "impl/impl.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the self scheduler local 
static tb_thread_local_t s_scheduler_self = TB_THREAD_LOCAL_INIT;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_scheduler_free(tb_list_entry_head_ref_t coroutines)
{
    // check
    tb_assert(coroutines);

    // free all coroutines
    while (tb_list_entry_size(coroutines))
    {
        // get the next entry from head
        tb_list_entry_ref_t entry = tb_list_entry_head(coroutines);
        tb_assert(entry);

        // remove it from the ready coroutines
        tb_list_entry_remove_head(coroutines);

        // exit this coroutine
        tb_coroutine_exit((tb_coroutine_t*)tb_list_entry(coroutines, entry));
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_scheduler_ref_t tb_scheduler_init()
{
    // done
    tb_bool_t       ok = tb_false;
    tb_scheduler_t* scheduler = tb_null;
    do
    {
        // make scheduler
        scheduler = tb_malloc0_type(tb_scheduler_t);
        tb_assert_and_check_break(scheduler);

        // init dead coroutines
        tb_list_entry_init(&scheduler->coroutines_dead, tb_coroutine_t, entry, tb_null);

        // init ready coroutines
        tb_list_entry_init(&scheduler->coroutines_ready, tb_coroutine_t, entry, tb_null);

        // init suspend coroutines
        tb_list_entry_init(&scheduler->coroutines_suspend, tb_coroutine_t, entry, tb_null);

        // init original coroutine
        scheduler->original.scheduler = (tb_scheduler_ref_t)scheduler;

        // init running
        scheduler->running = &scheduler->original;
        tb_coroutine_state_set(scheduler->running, TB_STATE_RUNNING);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (scheduler) tb_scheduler_exit((tb_scheduler_ref_t)scheduler);
        scheduler = tb_null;
    }

    // ok?
    return (tb_scheduler_ref_t)scheduler;
}
tb_void_t tb_scheduler_exit(tb_scheduler_ref_t self)
{
    // check
    tb_scheduler_t* scheduler = (tb_scheduler_t*)self;
    tb_assert_and_check_return(scheduler);

    // must be stopped
    tb_assert(scheduler->stopped);

    // exit io scheduler first
    if (scheduler->scheduler_io) tb_scheduler_io_exit(scheduler->scheduler_io);
    scheduler->scheduler_io = tb_null;

    // clear running
    scheduler->running = tb_null;

    // check coroutines
    tb_assert(!tb_list_entry_size(&scheduler->coroutines_ready));
    tb_assert(!tb_list_entry_size(&scheduler->coroutines_suspend));

    // free all dead coroutines 
    tb_scheduler_free(&scheduler->coroutines_dead);

    // free all ready coroutines 
    tb_scheduler_free(&scheduler->coroutines_ready);

    // free all suspend coroutines 
    tb_scheduler_free(&scheduler->coroutines_suspend);

    // exit dead coroutines
    tb_list_entry_exit(&scheduler->coroutines_dead);

    // exit ready coroutines
    tb_list_entry_exit(&scheduler->coroutines_ready);

    // exit suspend coroutines
    tb_list_entry_exit(&scheduler->coroutines_suspend);

    // exit the scheduler
    tb_free(scheduler);
}
tb_void_t tb_scheduler_kill(tb_scheduler_ref_t self)
{
    // check
    tb_scheduler_t* scheduler = (tb_scheduler_t*)self;
    tb_assert_and_check_return(scheduler);

    // stop it
    scheduler->stopped = tb_true;

    // kill the io scheduler
    if (scheduler->scheduler_io) tb_scheduler_io_kill(scheduler->scheduler_io);
}
tb_void_t tb_scheduler_loop(tb_scheduler_ref_t self)
{
    // check
    tb_scheduler_t* scheduler = (tb_scheduler_t*)self;
    tb_assert_and_check_return(scheduler);
 
    // init self scheduler local
    if (!tb_thread_local_init(&s_scheduler_self, tb_null)) return ;
 
    // update and overide the current scheduler
    tb_thread_local_set(&s_scheduler_self, self);

    // schedule all ready coroutines
    while (tb_list_entry_size(&scheduler->coroutines_ready)) 
    {
        // get the next entry from head
        tb_list_entry_ref_t entry = tb_list_entry_head(&scheduler->coroutines_ready);
        tb_assert(entry);

        // remove it from the ready coroutines
        tb_list_entry_remove_head(&scheduler->coroutines_ready);

        // switch to the next coroutine 
        tb_scheduler_switch(scheduler, (tb_coroutine_t*)tb_list_entry(&scheduler->coroutines_ready, entry));

        // trace
        tb_trace_d("[loop]: ready %lu", tb_list_entry_size(&scheduler->coroutines_ready));
    }

    // stop it
    scheduler->stopped = tb_true;
 
    // clear the current scheduler
    tb_thread_local_set(&s_scheduler_self, tb_null);
}
tb_scheduler_ref_t tb_scheduler_self()
{ 
    // get self scheduler on the current thread
    return (tb_scheduler_ref_t)tb_thread_local_get(&s_scheduler_self);
}
