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
 *
 */
#ifndef TB_COROUTINE_IMPL_STACKLESS_COROUTINE_H
#define TB_COROUTINE_IMPL_STACKLESS_COROUTINE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the coroutine wait type
typedef struct __tb_lo_coroutine_rs_wait_t
{
#ifndef TB_CONFIG_MICRO_ENABLE
    /* the timer task pointer for ltimer or timer
     *
     * for ltimer:  task
     * for timer:   task & 0x1
     */
    tb_cpointer_t               task;
#endif

    // the socket
    tb_socket_ref_t             sock;

    // the waiting events
    tb_sint32_t                 events          : 6;

    // the cached events
    tb_sint32_t                 events_cache    : 6;

    // the events result (may be -1)
    tb_sint32_t                 events_result   : 6;

    // is waiting?
    tb_sint32_t                 waiting         : 1;

}tb_lo_coroutine_rs_wait_t;

/// the stackless coroutine type
typedef struct __tb_lo_coroutine_t
{
    // the coroutine core
    tb_lo_core_t                core;

    // the list entry
    tb_list_entry_t             entry;

    // the coroutine function
    tb_lo_coroutine_func_t      func;

    // the user private data of the coroutine function
    tb_cpointer_t               priv;

    // the user private data free function
    tb_lo_coroutine_free_t      free;

    // the scheduler
    tb_lo_scheduler_ref_t       scheduler;

    // the passed private data between resume() and suspend()
    union 
    {
        // the arguments for wait()
        tb_lo_coroutine_rs_wait_t   wait;

    }                               rs;

}tb_lo_coroutine_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init coroutine 
 *
 * @param scheduler     the scheduler
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 * @param free          the user private data free function
 *
 * @return              the coroutine 
 */
tb_lo_coroutine_t*      tb_lo_coroutine_init(tb_lo_scheduler_ref_t scheduler, tb_lo_coroutine_func_t func, tb_cpointer_t priv, tb_lo_coroutine_free_t free);

/* reinit the given coroutine 
 *
 * @param coroutine     the coroutine
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 * @param free          the user private data free function
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_lo_coroutine_reinit(tb_lo_coroutine_t* coroutine, tb_lo_coroutine_func_t func, tb_cpointer_t priv, tb_lo_coroutine_free_t free);

/* exit coroutine
 *
 * @param coroutine     the coroutine
 */
tb_void_t               tb_lo_coroutine_exit(tb_lo_coroutine_t* coroutine);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
