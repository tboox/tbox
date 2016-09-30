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
 * includes
 */
#include "scheduler.h"
#include "impl/impl.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the self scheduler local 
static tb_thread_local_t s_scheduler_self = TB_THREAD_LOCAL_INIT;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_scheduler_exit(tb_scheduler_ref_t self)
{
    // check
    tb_scheduler_t* scheduler = (tb_scheduler_t*)self;
    tb_assert_and_check_return(scheduler);

    // exit the scheduler
    if (scheduler->exit) scheduler->exit(scheduler);
}
tb_void_t tb_scheduler_loop(tb_scheduler_ref_t self)
{
    // check
    tb_scheduler_t* scheduler = (tb_scheduler_t*)self;
    tb_assert_and_check_return(scheduler && scheduler->loop);
 
    // init self scheduler local
    if (!tb_thread_local_init(&s_scheduler_self, tb_null)) return ;
 
    // update and overide the current scheduler
    tb_thread_local_set(&s_scheduler_self, self);

    // run loop
    scheduler->loop(scheduler);
}
tb_size_t tb_scheduler_type(tb_scheduler_ref_t self)
{
    // check
    tb_scheduler_t* scheduler = (tb_scheduler_t*)self;
    tb_assert_and_check_return_val(scheduler, TB_SCHEDULER_TYPE_NONE);

    // get it
    return scheduler->type;
}
tb_scheduler_ref_t tb_scheduler_self()
{
    // get self scheduler on the current thread
    return (tb_scheduler_ref_t)tb_thread_local_get(&s_scheduler_self);
}
tb_bool_t tb_scheduler_ctrl(tb_scheduler_ref_t self, tb_size_t ctrl, ...)
{
    // check
    tb_scheduler_t* scheduler = (tb_scheduler_t*)self;
    tb_assert_and_check_return_val(scheduler && scheduler->ctrl, tb_false);

    // init args
    tb_va_list_t args;
    tb_va_start(args, ctrl);

    // ctrl it
    tb_bool_t ok = scheduler->ctrl(scheduler, ctrl, args);

    // exit args
    tb_va_end(args);

    // ok?
    return ok;
}
