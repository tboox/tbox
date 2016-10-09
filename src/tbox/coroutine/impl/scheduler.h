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
 * @ingroup     coroutine
 *
 */
#ifndef TB_COROUTINE_IMPL_SCHEDULER_H
#define TB_COROUTINE_IMPL_SCHEDULER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "coroutine.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// get running coroutine
#define tb_scheduler_running(scheduler)           ((scheduler)->running)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the scheduler type
typedef struct __tb_scheduler_t
{   
    /* the original coroutine (in main loop)
     *
     * coroutine->scheduler == (tb_scheduler_ref_t)coroutine
     */
    tb_coroutine_t              original;

    // the running coroutine
    tb_coroutine_t*             running;

    // the dead coroutines
    tb_list_entry_head_t        coroutines_dead;

    // the ready coroutines
    tb_list_entry_head_t        coroutines_ready;

    // the suspend coroutines
    tb_list_entry_head_t        coroutines_suspend;

}tb_scheduler_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* start the coroutine function 
 *
 * @param scheduler         the scheduler, uses the default scheduler if be null
 * @param func              the coroutine function
 * @param priv              the passed user private data as the argument of function
 * @param stacksize         the stack size
 *
 * @return                  tb_true or tb_false
 */
tb_bool_t                   tb_scheduler_start(tb_scheduler_t* scheduler, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize);

/* yield the current coroutine
 *
 * @param scheduler         the scheduler
 *
 * @return                  tb_true(yield ok) or tb_false(yield failed, no more coroutines)
 */
tb_bool_t                   tb_scheduler_yield(tb_scheduler_t* scheduler);

/*! resume the given coroutine (suspended)
 *
 * @param scheduler         the scheduler
 * @param coroutine         the suspended coroutine
 * @param priv              the user private data as the return value of suspend()
 */
tb_void_t                   tb_scheduler_resume(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine, tb_cpointer_t priv);

/*! suspend the current coroutine
 *
 * @param scheduler         the scheduler
 *
 * @return                  the user private data from resume(priv)
 */
tb_cpointer_t               tb_scheduler_suspend(tb_scheduler_t* scheduler);

/* finish the current coroutine
 *
 * @param scheduler         the scheduler
 */
tb_void_t                   tb_scheduler_finish(tb_scheduler_t* scheduler);

/* sleep the current coroutine
 *
 * @param scheduler         the scheduler
 * @param interval          the interval (ms)
 */
tb_void_t                   tb_scheduler_sleep(tb_scheduler_t* scheduler, tb_size_t interval);

/* switch to the given coroutine
 *
 * @param scheduler         the scheduler
 * @param coroutine         the coroutine
 */
tb_void_t                   tb_scheduler_switch(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
