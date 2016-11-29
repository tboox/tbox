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
 *
 */
#ifndef TB_COROUTINE_IMPL_STACKLESS_SCHEDULER_H
#define TB_COROUTINE_IMPL_STACKLESS_SCHEDULER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "coroutine.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// get the running coroutine
#define tb_lo_scheduler_running(scheduler)             ((scheduler)->running)

// get the ready coroutines count
#define tb_lo_scheduler_ready_count(scheduler)         tb_list_entry_size(&(scheduler)->coroutines_ready)

// get the suspended coroutines count
#define tb_lo_scheduler_suspend_count(scheduler)       tb_list_entry_size(&(scheduler)->coroutines_suspend)

// get the io scheduler
#define tb_lo_scheduler_io(scheduler)                  ((scheduler)->scheduler_io)

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the io scheduler type
struct __tb_lo_scheduler_io_t;

/// the stackless coroutine scheduler type
typedef struct __tb_lo_scheduler_t
{
    // is stopped
    tb_bool_t                       stopped;

    // the running coroutine
    tb_lo_coroutine_t*              running;

    // the io scheduler
    struct __tb_lo_scheduler_io_t*  scheduler_io;

    // the dead coroutines
    tb_list_entry_head_t            coroutines_dead;

    /* the ready coroutines
     * 
     * ready: head -> ready -> .. -> running -> .. -> ready -> ..->
     *         |                                                   |
     *          ---------------------------<-----------------------
     */
    tb_list_entry_head_t            coroutines_ready;

    // the suspend coroutines
    tb_list_entry_head_t            coroutines_suspend;

}tb_lo_scheduler_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! start coroutine 
 *
 * @param scheduler     the scheduler 
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 * @param free          the user private data free function
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_lo_scheduler_start(tb_lo_scheduler_t* scheduler, tb_lo_coroutine_func_t func, tb_cpointer_t priv, tb_lo_coroutine_free_t free);

/*! resume the given coroutine
 *
 * @param scheduler     the scheduler
 * @param coroutine     the coroutine 
 */
tb_void_t               tb_lo_scheduler_resume(tb_lo_scheduler_t* scheduler, tb_lo_coroutine_t* coroutine);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
