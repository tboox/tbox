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
 * macros
 */

// the stack guard magic
#define TB_COROUTINE_STACK_GUARD            (0xbeef)

// the default stack size
#define TB_COROUTINE_STACK_DEFSIZE          (8192 << 1)

// the debuginfo stack size
#define TB_COROUTINE_STACK_DBGSIZE          (8192)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_coroutine_entry(tb_context_from_t from)
{
    // get the from-coroutine 
    tb_coroutine_t* coroutine_from = (tb_coroutine_t*)from.priv;
    tb_assert(coroutine_from && from.context);

    // update the context
    coroutine_from->context = from.context;
    tb_assert(from.context);

    // get the current coroutine
    tb_coroutine_t* coroutine = (tb_coroutine_t*)tb_coroutine_self();
    tb_assert(coroutine && coroutine->func);

#ifdef __tb_debug__
    // check it
    tb_coroutine_check(coroutine);
#endif

    // trace
    tb_trace_d("entry: %p from coroutine(%p)", coroutine, coroutine_from);

    // call the coroutine function
    coroutine->func(coroutine->func_priv);

    // finish the current coroutine and switch to the other coroutine
    tb_scheduler_finish((tb_scheduler_t*)tb_scheduler_self());
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_coroutine_t* tb_coroutine_init(tb_scheduler_ref_t scheduler, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize)
{
    // check
    tb_assert_and_check_return_val(scheduler && func, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_coroutine_t* coroutine = tb_null;
    do
    {
        // init stack size
        if (!stacksize) stacksize = TB_COROUTINE_STACK_DEFSIZE;

#ifdef __tb_debug__
        // patch debug stack size for (assert, trace ..)
        stacksize += TB_COROUTINE_STACK_DBGSIZE;
#endif

        /* make coroutine
         *
         * TODO: 
         *
         * - segment stack 
         *
         *  -----------------------------------------------
         * | coroutine | guard | ... stacksize ... | guard |
         *  -----------------------------------------------
         */
        coroutine = (tb_coroutine_t*)tb_malloc_bytes(sizeof(tb_coroutine_t) + stacksize + sizeof(tb_uint16_t));
        tb_assert_and_check_break(coroutine);

        // save scheduler
        coroutine->scheduler = scheduler;

        // init stack
        coroutine->stackbase = (tb_byte_t*)&(coroutine[1]) + stacksize;
        coroutine->stacksize = stacksize;

        // fill guard
        coroutine->guard = TB_COROUTINE_STACK_GUARD;
        tb_bits_set_u16_ne(coroutine->stackbase, TB_COROUTINE_STACK_GUARD);

        // init function and user private data
        coroutine->func      = func;
        coroutine->func_priv = priv;

        // init state
        coroutine->state = TB_STATE_READY;

        // make context
        coroutine->context = tb_context_make(coroutine->stackbase - stacksize, stacksize, tb_coroutine_entry);
        tb_assert_and_check_break(coroutine->context);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (coroutine) tb_coroutine_exit(coroutine); 
        coroutine = tb_null;
    }

    // trace
    tb_trace_d("init %p", coroutine);

    // ok?
    return coroutine;
}
tb_coroutine_t* tb_coroutine_reinit(tb_coroutine_t* coroutine, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize)
{
    // check
    tb_assert_and_check_return_val(coroutine && func, tb_null);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // init stack size
        if (!stacksize) stacksize = TB_COROUTINE_STACK_DEFSIZE;

        // remake coroutine
        if (stacksize > coroutine->stacksize)
            coroutine = (tb_coroutine_t*)tb_ralloc_bytes(coroutine, sizeof(tb_coroutine_t) + stacksize + sizeof(tb_uint16_t));
        else stacksize = coroutine->stacksize;
        tb_assert_and_check_break(coroutine && coroutine->scheduler);

        // init stack
        coroutine->stackbase = (tb_byte_t*)&(coroutine[1]) + stacksize;
        coroutine->stacksize = stacksize;

        // fill guard
        coroutine->guard = TB_COROUTINE_STACK_GUARD;
        tb_bits_set_u16_ne(coroutine->stackbase, TB_COROUTINE_STACK_GUARD);

        // init function and user private data
        coroutine->func         = func;
        coroutine->func_priv    = priv;

        // init state
        coroutine->state = TB_STATE_READY;

        // make context
        coroutine->context = tb_context_make(coroutine->stackbase - stacksize, stacksize, tb_coroutine_entry);
        tb_assert_and_check_break(coroutine->context);

        // ok
        ok = tb_true;

    } while (0);

    // failed? reset it
    if (!ok) coroutine = tb_null;

    // trace
    tb_trace_d("reinit %p", coroutine);

    // ok?
    return coroutine;
}
tb_void_t tb_coroutine_exit(tb_coroutine_t* coroutine)
{
    // check
    tb_assert_and_check_return(coroutine);

    // trace
    tb_trace_d("exit: %p", coroutine);

#ifdef __tb_debug__
    // check it
    tb_coroutine_check(coroutine);
#endif

    // check state: must be dead now
    tb_assertf(tb_coroutine_is_dead(coroutine), "this coroutine state(%s) is invalid!", tb_coroutine_state_cstr(coroutine));

    // exit it
    tb_free(coroutine);
}
#ifdef __tb_debug__
tb_void_t tb_coroutine_check(tb_coroutine_t* coroutine)
{
    // check
    tb_assert(coroutine && coroutine->context);

    // this coroutine is original for scheduler?
    tb_check_return(!tb_coroutine_is_original(coroutine));

    // check stack underflow
    if (coroutine->guard != TB_COROUTINE_STACK_GUARD)
    {
        // trace
        tb_trace_e("this coroutine stack is underflow!");

        // dump stack
        tb_dump_data(coroutine->stackbase - coroutine->stacksize, coroutine->stacksize);

        // abort
        tb_abort();
    }

    // check stack overflow
    if (tb_bits_get_u16_ne(coroutine->stackbase) != TB_COROUTINE_STACK_GUARD)
    {
        // trace
        tb_trace_e("this coroutine stack is overflow!");

        // dump stack
        tb_dump_data(coroutine->stackbase - coroutine->stacksize, coroutine->stacksize);

        // abort
        tb_abort();
    }
}
#endif

