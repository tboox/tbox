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
#include "../impl/impl.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_lo_scheduler_free(tb_list_entry_head_ref_t coroutines)
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
        tb_lo_coroutine_exit((tb_lo_coroutine_t*)tb_list_entry(coroutines, entry));
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_lo_scheduler_ref_t tb_lo_scheduler_init()
{
    // done
    tb_bool_t           ok = tb_false;
    tb_lo_scheduler_t*  scheduler = tb_null;
    do
    {
        // make scheduler
        scheduler = tb_malloc0_type(tb_lo_scheduler_t);
        tb_assert_and_check_break(scheduler);

        // init dead coroutines
        tb_list_entry_init(&scheduler->coroutines_dead, tb_lo_coroutine_t, entry, tb_null);

        // init ready coroutines
        tb_list_entry_init(&scheduler->coroutines_ready, tb_lo_coroutine_t, entry, tb_null);

        // init suspend coroutines
        tb_list_entry_init(&scheduler->coroutines_suspend, tb_lo_coroutine_t, entry, tb_null);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (scheduler) tb_lo_scheduler_exit((tb_lo_scheduler_ref_t)scheduler);
        scheduler = tb_null;
    }

    // ok?
    return (tb_lo_scheduler_ref_t)scheduler;
}
tb_void_t tb_lo_scheduler_exit(tb_lo_scheduler_ref_t self)
{
    // check
    tb_lo_scheduler_t* scheduler = (tb_lo_scheduler_t*)self;
    tb_assert_and_check_return(scheduler);

    // must be stopped
    tb_assert(scheduler->stopped);
    
    // check coroutines
    tb_assert(!tb_list_entry_size(&scheduler->coroutines_ready));
    tb_assert(!tb_list_entry_size(&scheduler->coroutines_suspend));

    // free all dead coroutines 
    tb_lo_scheduler_free(&scheduler->coroutines_dead);

    // free all ready coroutines 
    tb_lo_scheduler_free(&scheduler->coroutines_ready);

    // free all suspend coroutines 
    tb_lo_scheduler_free(&scheduler->coroutines_suspend);

    // exit dead coroutines
    tb_list_entry_exit(&scheduler->coroutines_dead);

    // exit ready coroutines
    tb_list_entry_exit(&scheduler->coroutines_ready);

    // exit suspend coroutines
    tb_list_entry_exit(&scheduler->coroutines_suspend);

    // exit the scheduler
    tb_free(scheduler);
}
tb_void_t tb_lo_scheduler_kill(tb_lo_scheduler_ref_t self)
{
    // check
    tb_lo_scheduler_t* scheduler = (tb_lo_scheduler_t*)self;
    tb_assert_and_check_return(scheduler);

    // stop it
    scheduler->stopped = tb_true;
}
tb_void_t tb_lo_scheduler_loop(tb_lo_scheduler_ref_t self)
{
    // check
    tb_lo_scheduler_t* scheduler = (tb_lo_scheduler_t*)self;
    tb_assert_and_check_return(scheduler);

    // schedule all ready coroutines
    while (tb_list_entry_size(&scheduler->coroutines_ready)) 
    {
        // get the next entry from head
//        tb_list_entry_ref_t entry = tb_list_entry_head(&scheduler->coroutines_ready);
//        tb_assert(entry);

        // TODO
        // switch to the next coroutine 
        //tb_lo_scheduler_switch(scheduler, (tb_lo_coroutine_t*)tb_list_entry0(entry));

        // trace
        tb_trace_d("[loop]: ready %lu", tb_list_entry_size(&scheduler->coroutines_ready));
    }

    // stop it
    scheduler->stopped = tb_true;
}

