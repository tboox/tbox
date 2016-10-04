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
#ifndef TB_COROUTINE_IMPL_COROUTINE_H
#define TB_COROUTINE_IMPL_COROUTINE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// get coroutine state
#define tb_coroutine_state(coroutine)               ((coroutine)->state)

// set coroutine state
#define tb_coroutine_state_set(coroutine, value)    do { ((coroutine)->state) = (value); } while (0)

// get coroutine state c-string
#define tb_coroutine_state_cstr(coroutine)          tb_state_cstr(((coroutine)->state))

// get scheduler
#define tb_coroutine_scheduler(coroutine)           ((coroutine)->scheduler)

// is dead?
#define tb_coroutine_is_dead(coroutine)             (tb_coroutine_state(coroutine) == TB_STATE_DEAD)

// is ready?
#define tb_coroutine_is_ready(coroutine)            (tb_coroutine_state(coroutine) == TB_STATE_READY)

// is running?
#define tb_coroutine_is_running(coroutine)          (tb_coroutine_state(coroutine) == TB_STATE_RUNNING)

// is original?
#define tb_coroutine_is_original(coroutine)         ((coroutine)->scheduler == (tb_scheduler_ref_t)(coroutine))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the coroutine type
typedef struct __tb_coroutine_t
{
    // the scheduler
    tb_scheduler_ref_t      scheduler;

    // the context 
    tb_context_ref_t        context;

    // the stack base (top)
    tb_byte_t*              stackbase;

    // the stack size
    tb_size_t               stacksize;

    /* the state
     * 
     * - TB_STATE_READY
     * - TB_STATE_RUNNING
     * - TB_STATE_DEAD
     */
    tb_size_t               state;

    // the function 
    tb_coroutine_func_t     func;

    // the user private data
    tb_cpointer_t           priv;

    // the single list entry
    tb_single_list_entry_t  entry;

    // the guard
    tb_uint16_t             guard;

}tb_coroutine_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init coroutine 
 *
 * @param scheduler     the scheduler
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 * @param stacksize     the stack size, uses the default stack size if be zero
 *
 * @return              the coroutine 
 */
tb_coroutine_t*         tb_coroutine_init(tb_scheduler_ref_t scheduler, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize);

/* reinit the given coroutine 
 *
 * @param coroutine     the coroutine
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 * @param stacksize     the stack size, uses the default stack size if be zero
 *
 * @return              the coroutine
 */
tb_coroutine_t*         tb_coroutine_reinit(tb_coroutine_t* coroutine, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize);

/* exit coroutine
 *
 * @param coroutine     the coroutine
 */
tb_void_t               tb_coroutine_exit(tb_coroutine_t* coroutine);

#ifdef __tb_debug__
/* check coroutine
 *
 * @param coroutine     the coroutine
 */
tb_void_t               tb_coroutine_check(tb_coroutine_t* coroutine);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
