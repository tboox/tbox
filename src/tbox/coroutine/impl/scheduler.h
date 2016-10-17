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

// get the running coroutine
#define tb_co_scheduler_running(scheduler)             ((scheduler)->running)

// get the ready coroutines count
#define tb_co_scheduler_ready_count(scheduler)         tb_list_entry_size(&(scheduler)->coroutines_ready)

// get the suspended coroutines count
#define tb_co_scheduler_suspend_count(scheduler)       tb_list_entry_size(&(scheduler)->coroutines_suspend)

// get the io scheduler
#define tb_co_scheduler_io(scheduler)                  ((scheduler)->scheduler_io)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the io scheduler type
struct __tb_co_scheduler_io_t;

// the scheduler type
typedef struct __tb_co_scheduler_t
{   
    /* the original coroutine (in main loop)
     *
     * coroutine->scheduler == (tb_co_scheduler_ref_t)coroutine
     */
    tb_coroutine_t                  original;

    // is stopped
    tb_bool_t                       stopped;

    // the running coroutine
    tb_coroutine_t*                 running;

    // the io scheduler
    struct __tb_co_scheduler_io_t*  scheduler_io;

    // the dead coroutines
    tb_list_entry_head_t            coroutines_dead;

    // the ready coroutines
    tb_list_entry_head_t            coroutines_ready;

    // the suspend coroutines
    tb_list_entry_head_t            coroutines_suspend;

}tb_co_scheduler_t;

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
tb_bool_t                   tb_co_scheduler_start(tb_co_scheduler_t* scheduler, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize);

/* yield the current coroutine
 *
 * @param scheduler         the scheduler
 *
 * @return                  tb_true(yield ok) or tb_false(yield failed, no more coroutines)
 */
tb_bool_t                   tb_co_scheduler_yield(tb_co_scheduler_t* scheduler);

/*! resume the given coroutine (suspended)
 *
 * @param scheduler         the scheduler
 * @param coroutine         the suspended coroutine
 * @param priv              the user private data as the return value of suspend() or sleep()
 */
tb_void_t                   tb_co_scheduler_resume(tb_co_scheduler_t* scheduler, tb_coroutine_t* coroutine, tb_cpointer_t priv);

/*! suspend the current coroutine
 *
 * @param scheduler         the scheduler
 *
 * @return                  the user private data from resume(priv)
 */
tb_cpointer_t               tb_co_scheduler_suspend(tb_co_scheduler_t* scheduler);

/* finish the current coroutine
 *
 * @param scheduler         the scheduler
 */
tb_void_t                   tb_co_scheduler_finish(tb_co_scheduler_t* scheduler);

/* sleep the current coroutine
 *
 * @param scheduler         the scheduler
 * @param interval          the interval (ms), infinity: -1
 *
 * @return                  the user private data from resume(priv)
 */
tb_cpointer_t               tb_co_scheduler_sleep(tb_co_scheduler_t* scheduler, tb_long_t interval);

/* switch to the given coroutine
 *
 * @param scheduler         the scheduler
 * @param coroutine         the coroutine
 */
tb_void_t                   tb_co_scheduler_switch(tb_co_scheduler_t* scheduler, tb_coroutine_t* coroutine);

/*! wait io events 
 *
 * @param scheduler         the scheduler
 * @param sock              the socket
 * @param events            the waited events
 * @param timeout           the timeout, infinity: -1
 *
 * @return                  > 0: the events, 0: timeout, -1: failed
 */
tb_long_t                   tb_co_scheduler_wait(tb_co_scheduler_t* scheduler, tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
