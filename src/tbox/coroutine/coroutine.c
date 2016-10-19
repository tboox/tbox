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
 * @file        coroutine.h
 * @ingroup     coroutine
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "coroutine"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "coroutine.h"
#include "scheduler.h"
#include "impl/impl.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_coroutine_start(tb_co_scheduler_ref_t scheduler, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize)
{
    // check
    tb_assert_and_check_return_val(func, tb_false);

    // start it
    return tb_co_scheduler_start((tb_co_scheduler_t*)scheduler, func, priv, stacksize);
}
tb_bool_t tb_coroutine_yield()
{
    // get current scheduler
    tb_co_scheduler_t* scheduler = (tb_co_scheduler_t*)tb_co_scheduler_self();

    // yield the current coroutine
    return scheduler? tb_co_scheduler_yield(scheduler) : tb_false;
}
tb_cpointer_t tb_coroutine_resume(tb_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // get current scheduler
    tb_co_scheduler_t* scheduler = (tb_co_scheduler_t*)tb_co_scheduler_self();
        
    // resume the given coroutine
    return scheduler? tb_co_scheduler_resume(scheduler, (tb_coroutine_t*)coroutine, priv) : tb_null;
}
tb_cpointer_t tb_coroutine_suspend(tb_cpointer_t priv)
{
    // get current scheduler
    tb_co_scheduler_t* scheduler = (tb_co_scheduler_t*)tb_co_scheduler_self();

    // suspend the current coroutine
    return scheduler? tb_co_scheduler_suspend(scheduler, priv) : tb_null;
}
tb_cpointer_t tb_coroutine_sleep(tb_long_t interval)
{
    // get current scheduler
    tb_co_scheduler_t* scheduler = (tb_co_scheduler_t*)tb_co_scheduler_self();

    // sleep the current coroutine
    return scheduler? tb_co_scheduler_sleep(scheduler, interval) : tb_null;
}
tb_long_t tb_coroutine_wait(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    // get current scheduler
    tb_co_scheduler_t* scheduler = (tb_co_scheduler_t*)tb_co_scheduler_self();

    // wait events
    return scheduler? tb_co_scheduler_wait(scheduler, sock, events, timeout) : -1;
}
tb_coroutine_ref_t tb_coroutine_self()
{
    // get coroutine
    tb_co_scheduler_t* scheduler = (tb_co_scheduler_t*)tb_co_scheduler_self();

    // get running coroutine
    return scheduler? (tb_coroutine_ref_t)tb_co_scheduler_running(scheduler) : tb_null;
}

