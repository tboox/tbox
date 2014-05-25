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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
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

tb_handle_t tb_thread_init(tb_char_t const* name, tb_pointer_t (*func)(tb_cpointer_t), tb_cpointer_t priv, tb_size_t stack)
{
    // done
    pthread_t       handle;
    pthread_attr_t  attr;
    tb_bool_t       ok = tb_false;
    do
    {
        // init attr
        if (stack)
        {
            if (pthread_attr_init(&attr)) break;
            pthread_attr_setstacksize(&attr, stack);
        }

        // init thread
        if (pthread_create(&handle, stack? &attr : tb_null, (tb_pointer_t (*)(tb_pointer_t))func, (tb_pointer_t)priv)) break;

        // ok
        ok = tb_true;

    } while (0);

    // exit attr
    if (stack) pthread_attr_destroy(&attr);
    
    // ok?
    return ok? ((tb_handle_t)handle) : tb_null;
}
tb_void_t tb_thread_exit(tb_handle_t handle)
{
    // check 
    tb_long_t ok = -1;
    if ((ok = pthread_kill(((pthread_t)handle), 0)) && ok != ESRCH)
    {
        // trace
        tb_trace_e("thread[%p]: not exited: %ld, errno: %d", handle, ok, errno);
    }
}
tb_long_t tb_thread_wait(tb_handle_t handle, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(handle, -1);

    // wait
    tb_long_t ok = -1;
    if ((ok = pthread_join(((pthread_t)handle), tb_null)))
    {
        // trace
        tb_trace_e("thread[%p]: wait failed: %ld, errno: %d", handle, ok, errno);
        return -1;
    
    }

    // ok
    return 1;
}
tb_void_t tb_thread_return(tb_pointer_t value)
{
    pthread_exit(value);
}
tb_bool_t tb_thread_suspend(tb_handle_t handle)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_thread_resume(tb_handle_t handle)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_size_t tb_thread_self()
{
    return (tb_size_t)pthread_self();
}

