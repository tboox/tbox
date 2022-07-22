/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
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

// get scheduler
#define tb_coroutine_scheduler(coroutine)           ((coroutine)->scheduler)

// is original?
#define tb_coroutine_is_original(coroutine)         ((coroutine)->scheduler == (tb_co_scheduler_ref_t)(coroutine))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the coroutine function type
typedef struct __tb_coroutine_rs_func_t
{
    // the function
    tb_coroutine_func_t             func;

    // the user private data as the argument of function
    tb_cpointer_t                   priv;

}tb_coroutine_rs_func_t;

// the coroutine wait type
typedef struct __tb_coroutine_rs_wait_t
{
    // the waited poller object
    tb_poller_object_t              object;

    // the timer task pointer for ltimer or timer
    tb_cpointer_t                   task;

    // the object event, (process status or fwatcher event)
    tb_long_t                       object_event;

    // has pending process status?
    tb_uint16_t                     object_pending  : 1;

    // waiting process?
    tb_uint16_t                     object_waiting  : 1;

    // is ltimer?
    tb_uint16_t                     is_ltimer     : 1;

}tb_coroutine_rs_wait_t;

// the coroutine type
typedef struct __tb_coroutine_t
{
    /* the list entry for ready, suspend and dead lists
     *
     * be placed in the head for optimization
     */
    tb_list_entry_t                 entry;

    // the scheduler
    tb_co_scheduler_ref_t           scheduler;

    // the context
    tb_context_ref_t                context;

    // the stack base (top)
    tb_byte_t*                      stackbase;

    // the stack size
    tb_size_t                       stacksize;

    // the passed user private data between priv = resume(priv) and priv = suspend(priv)
    tb_cpointer_t                   rs_priv;

    // the passed private data between resume() and suspend()
    union
    {
        // the function
        tb_coroutine_rs_func_t      func;

        // the arguments for wait()
        tb_coroutine_rs_wait_t      wait;

        // the single entry
        tb_single_list_entry_t      single_entry;

    }                               rs;

    // the guard
    tb_uint16_t                     guard;

#if defined(__tb_valgrind__) && defined(TB_CONFIG_VALGRIND_HAVE_VALGRIND_STACK_REGISTER)
    // the valgrind stack id, helo valgrind to understand coroutine
    tb_uint_t                       valgrind_stack_id;
#endif

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
tb_coroutine_t*         tb_coroutine_init(tb_co_scheduler_ref_t scheduler, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize);

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
