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
 * @file        thread.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../thread.h"
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_thread_ref_t tb_thread_init(tb_char_t const* name, tb_thread_func_t func, tb_cpointer_t priv, tb_size_t stack)
{
    // done
    pthread_t       thread;
    pthread_attr_t  attr;
    tb_bool_t       ok = tb_false;
    tb_value_ref_t  args = tb_null;
    do
    {
        // init attr
        if (stack)
        {
            if (pthread_attr_init(&attr)) break;
            pthread_attr_setstacksize(&attr, stack);
        }

        // init arguments
        args = tb_nalloc0_type(2, tb_value_t);
        tb_assert_and_check_break(args);

        // save function and private data
        args[0].ptr = (tb_pointer_t)func;
        args[1].ptr = (tb_pointer_t)priv;

        // init thread
        if (pthread_create(&thread, stack? &attr : tb_null, tb_thread_func, args)) break;

        // ok
        ok = tb_true;

    } while (0);

    // exit attr
    if (stack) pthread_attr_destroy(&attr);

    // exit arguments if failed
    if (!ok)
    {
        if (args) tb_free(args);
        args = tb_null;
    }

    // ok?
    return ok? ((tb_thread_ref_t)thread) : tb_null;
}
tb_void_t tb_thread_exit(tb_thread_ref_t thread)
{
    // check 
    tb_long_t ok = -1;
    if ((ok = pthread_kill(((pthread_t)thread), 0)) && ok != ESRCH)
    {
        // trace
        tb_trace_e("thread[%p]: not exited: %ld, errno: %d", thread, ok, errno);
    }
}
tb_long_t tb_thread_wait(tb_thread_ref_t thread, tb_long_t timeout, tb_int_t* retval)
{
    // check
    tb_assert_and_check_return_val(thread, -1);

    // wait
    tb_long_t       ok = -1;
    tb_pointer_t    ret = tb_null;
    if ((ok = pthread_join(((pthread_t)thread), &ret)) && ok != ESRCH)
    {
        // trace
        tb_trace_e("thread[%p]: wait failed: %ld, errno: %d", thread, ok, errno);
        return -1;
    
    }

    // save the return value
    if (retval) *retval = tb_p2s32(ret);

    // ok
    return 1;
}
tb_void_t tb_thread_return(tb_int_t value)
{
    pthread_exit((tb_pointer_t)(tb_long_t)value);
}
tb_bool_t tb_thread_suspend(tb_thread_ref_t thread)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_thread_resume(tb_thread_ref_t thread)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_size_t tb_thread_self()
{
    return (tb_size_t)pthread_self();
}

