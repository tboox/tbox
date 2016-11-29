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
#include "../impl/impl.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
tb_lo_coroutine_t* tb_lo_coroutine_init(tb_lo_scheduler_ref_t scheduler, tb_lo_coroutine_func_t func, tb_cpointer_t priv, tb_lo_coroutine_free_t free)
{
    // check
    tb_assert_and_check_return_val(scheduler && func, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_lo_coroutine_t*  coroutine = tb_null;
    do
    {
        // make coroutine
        coroutine = tb_malloc0_type(tb_lo_coroutine_t);
        tb_assert_and_check_break(coroutine);

        // init core
        tb_lo_core_init(&coroutine->core);

        // save scheduler
        coroutine->scheduler = scheduler;

        // init function and user private data
        coroutine->func = func;
        coroutine->priv = priv;
        coroutine->free = free;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (coroutine) tb_lo_coroutine_exit(coroutine); 
        coroutine = tb_null;
    }

    // trace
    tb_trace_d("init %p", coroutine);

    // ok?
    return coroutine;
}
tb_bool_t tb_lo_coroutine_reinit(tb_lo_coroutine_t* coroutine, tb_lo_coroutine_func_t func, tb_cpointer_t priv, tb_lo_coroutine_free_t free)
{
    // check
    tb_assert_and_check_return_val(coroutine && func, tb_false);
    tb_assert_and_check_return_val(coroutine->scheduler && tb_lo_core_state(coroutine) == TB_STATE_END, tb_false);

    // init core
    tb_lo_core_init(&coroutine->core);

    // init function and user private data
    coroutine->func = func;
    coroutine->priv = priv;
    coroutine->free = free;

    // ok
    return tb_true;
}
tb_void_t tb_lo_coroutine_exit(tb_lo_coroutine_t* coroutine)
{
    // check
    tb_assert_and_check_return(coroutine && tb_lo_core_state(coroutine) == TB_STATE_END);

    // trace
    tb_trace_d("exit: %p", coroutine);

    // exit it
    tb_free(coroutine);
}
tb_lo_scheduler_ref_t tb_lo_coroutine_scheduler_(tb_lo_coroutine_ref_t self)
{
    // check
    tb_lo_coroutine_t* coroutine = (tb_lo_coroutine_t*)self;
    tb_assert(coroutine);

    // get scheduler 
    return coroutine->scheduler;
}
tb_void_t tb_lo_coroutine_sleep_(tb_lo_coroutine_ref_t self, tb_long_t interval)
{
    // check
    tb_lo_coroutine_t* coroutine = (tb_lo_coroutine_t*)self;
    tb_assert(coroutine);

    // get scheduler
    tb_lo_scheduler_t* scheduler = (tb_lo_scheduler_t*)coroutine->scheduler;
    tb_assert(scheduler);
    
    // init io scheduler first
    if (!scheduler->scheduler_io) scheduler->scheduler_io = tb_lo_scheduler_io_init(scheduler);
    tb_assert(scheduler->scheduler_io);

    // sleep it
    tb_lo_scheduler_io_sleep(scheduler->scheduler_io, interval);
}
tb_bool_t tb_lo_coroutine_wait_(tb_lo_coroutine_ref_t self, tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_lo_coroutine_t* coroutine = (tb_lo_coroutine_t*)self;
    tb_assert(coroutine);

    // get scheduler
    tb_lo_scheduler_t* scheduler = (tb_lo_scheduler_t*)coroutine->scheduler;
    tb_assert(scheduler);
    
    // init io scheduler first
    if (!scheduler->scheduler_io) scheduler->scheduler_io = tb_lo_scheduler_io_init(scheduler);
    tb_assert(scheduler->scheduler_io);

    // wait it
    return tb_lo_scheduler_io_wait(scheduler->scheduler_io, sock, events, timeout);
}
tb_long_t tb_lo_coroutine_events_(tb_lo_coroutine_ref_t self)
{
    // check
    tb_lo_coroutine_t* coroutine = (tb_lo_coroutine_t*)self;
    tb_assert(coroutine);

    // get events
    return coroutine->rs.wait.events;
}
tb_void_t tb_lo_coroutine_pass_free_(tb_cpointer_t priv)
{
    if (priv) tb_free(priv);
}
tb_pointer_t tb_lo_coroutine_pass1_make_(tb_size_t type_size, tb_cpointer_t value, tb_size_t offset, tb_size_t size)
{
    // check
    tb_assert(type_size && value && offset + size <= type_size);

    // make data
    tb_byte_t* data = tb_malloc0(type_size);
    if (data) tb_memcpy(data + offset, value, size);

    // ok?
    return data;
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * public implementation
 */
tb_bool_t tb_lo_coroutine_start(tb_lo_scheduler_ref_t self, tb_lo_coroutine_func_t func, tb_cpointer_t priv, tb_lo_coroutine_free_t free)
{
    return tb_lo_scheduler_start((tb_lo_scheduler_t*)self, func, priv, free);
}
tb_void_t tb_lo_coroutine_resume(tb_lo_coroutine_ref_t self)
{
    tb_lo_scheduler_resume((tb_lo_scheduler_t*)tb_lo_coroutine_scheduler_(self), (tb_lo_coroutine_t*)self);
}
