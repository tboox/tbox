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
 * @file        poll.c
 * @ingroup     coroutine
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the poll scheduler type
typedef struct __tb_scheduler_poll_t
{
    // the scheduler base
    tb_scheduler_t          base;

}tb_scheduler_poll_t, *tb_scheduler_poll_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_scheduler_poll_ref_t tb_scheduler_poll_cast(tb_scheduler_t* scheduler)
{
    // check
    tb_assert(scheduler && scheduler->type == TB_SCHEDULER_TYPE_FIFO);

    // cast it
    return (tb_scheduler_poll_ref_t)scheduler;
}
static tb_bool_t tb_scheduler_poll_start(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine)
{
    // check
    tb_scheduler_poll_ref_t scheduler_poll = tb_scheduler_poll_cast(scheduler);
    tb_assert_and_check_return_val(scheduler_poll, tb_false);

    return tb_false;
}
static tb_void_t tb_scheduler_poll_yield(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine)
{
    // check
    tb_scheduler_poll_ref_t scheduler_poll = tb_scheduler_poll_cast(scheduler);
    tb_assert_and_check_return(scheduler_poll);

}
static tb_void_t tb_scheduler_poll_sleep(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine, tb_size_t interval)
{
    // check
    tb_scheduler_poll_ref_t scheduler_poll = tb_scheduler_poll_cast(scheduler);
    tb_assert_and_check_return(scheduler_poll);

}
static tb_bool_t tb_scheduler_poll_ctrl(tb_scheduler_t* scheduler, tb_size_t ctrl, tb_va_list_t args)
{ 
    // check
    tb_scheduler_poll_ref_t scheduler_poll = tb_scheduler_poll_cast(scheduler);
    tb_assert_and_check_return_val(scheduler_poll, tb_false);

    return tb_false;
}
static tb_void_t tb_scheduler_poll_loop(tb_scheduler_t* scheduler)
{
    // check
    tb_scheduler_poll_ref_t scheduler_poll = tb_scheduler_poll_cast(scheduler);
    tb_assert_and_check_return(scheduler_poll);

}
static tb_void_t tb_scheduler_poll_exit(tb_scheduler_t* scheduler)
{
    // check
    tb_scheduler_poll_ref_t scheduler_poll = tb_scheduler_poll_cast(scheduler);
    tb_assert_and_check_return(scheduler_poll);

    // exit it
    tb_free(scheduler_poll);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_scheduler_ref_t tb_scheduler_init_with_poll()
{
    // done
    tb_bool_t               ok = tb_false;
    tb_scheduler_poll_t*    scheduler = tb_null;
    do
    {
        // make scheduler
        scheduler = tb_malloc0_type(tb_scheduler_poll_t);
        tb_assert_and_check_break(scheduler);

        // init base
        scheduler->base.type    = TB_SCHEDULER_TYPE_FIFO;
        scheduler->base.ctrl    = tb_scheduler_poll_ctrl;
        scheduler->base.loop    = tb_scheduler_poll_loop;
        scheduler->base.exit    = tb_scheduler_poll_exit;
        scheduler->base.start   = tb_scheduler_poll_start;
        scheduler->base.yield   = tb_scheduler_poll_yield;
        scheduler->base.sleep   = tb_scheduler_poll_sleep;
        
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

