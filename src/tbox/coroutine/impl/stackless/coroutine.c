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

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_lo_coroutine_t* tb_lo_coroutine_init(tb_lo_scheduler_ref_t scheduler, tb_lo_coroutine_func_t func, tb_cpointer_t priv)
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
tb_bool_t tb_lo_coroutine_reinit(tb_lo_coroutine_t* coroutine, tb_lo_coroutine_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(coroutine && func, tb_false);
    tb_assert_and_check_return_val(coroutine->scheduler && tb_lo_core_state(coroutine) == TB_STATE_DEAD, tb_false);

    // init core
    tb_lo_core_init(&coroutine->core);

    // init function and user private data
    coroutine->func = func;
    coroutine->priv = priv;

    // ok
    return tb_true;
}
tb_void_t tb_lo_coroutine_exit(tb_lo_coroutine_t* coroutine)
{
    // check
    tb_assert_and_check_return(coroutine && tb_lo_core_state(coroutine) == TB_STATE_DEAD);

    // trace
    tb_trace_d("exit: %p", coroutine);

    // exit it
    tb_free(coroutine);
}

