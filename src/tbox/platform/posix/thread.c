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
 * types
 */

// the thread type
typedef struct __tb_thread_t
{
    // the pthread, @note We must put it at the beginning, because posix/mach/thread_affinity will use it.
    pthread_t   pthread;

    // is joined?
    tb_bool_t   is_joined;

}tb_thread_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c_enter__
tb_void_t tb_thread_local_clear_atexit();
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_thread_ref_t tb_thread_init(tb_char_t const* name, tb_thread_func_t func, tb_cpointer_t priv, tb_size_t stack)
{
    // check
    tb_assert_and_check_return_val(func, tb_null);

    pthread_attr_t  attr;
    tb_bool_t       ok = tb_false;
    tb_value_ref_t  args = tb_null;
    tb_thread_t*    thread = tb_null;
    do
    {
        // init thread
        thread = tb_malloc0_type(tb_thread_t);
        tb_assert_and_check_break(thread);

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
        if (pthread_create(&thread->pthread, stack? &attr : tb_null, tb_thread_func, args)) break;

        // ok
        ok = tb_true;

    } while (0);

    // exit attr
    if (stack) pthread_attr_destroy(&attr);

    // failed
    if (!ok)
    {
        // exit arguments
        if (args) tb_free(args);
        args = tb_null;

        // exit thread
        if (thread) tb_free(thread);
        thread = tb_null;
    }

    // ok?
    return ok? ((tb_thread_ref_t)thread) : tb_null;
}
tb_void_t tb_thread_exit(tb_thread_ref_t self)
{
    // check
    tb_thread_t* thread = (tb_thread_t*)self;
    tb_assert_and_check_return(thread);

    // detach thread if not joined
    if (!thread->is_joined)
        pthread_detach(thread->pthread);
    tb_free(thread);
}
tb_long_t tb_thread_wait(tb_thread_ref_t self, tb_long_t timeout, tb_int_t* retval)
{
    // check
    tb_thread_t* thread = (tb_thread_t*)self;
    tb_assert_and_check_return_val(thread, -1);

    // wait thread
    tb_long_t       ok = -1;
    tb_pointer_t    ret = tb_null;
    if ((ok = pthread_join(thread->pthread, &ret)) && ok != ESRCH)
    {
        // trace
        tb_trace_e("thread[%p]: wait failed: %ld, errno: %d", thread, ok, errno);

        // join failed? detach it
        pthread_detach(thread->pthread);
        return -1;

    }

    // join ok
    thread->is_joined = tb_true;

    // save the return value
    if (retval) *retval = tb_p2s32(ret);

    // ok
    return 1;
}
tb_void_t tb_thread_return(tb_int_t value)
{
    // free all thread local data on the current thread
    tb_thread_local_clear_atexit();

    // exit thread and return value
    pthread_exit((tb_pointer_t)(tb_long_t)value);
}
tb_bool_t tb_thread_suspend(tb_thread_ref_t self)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_thread_resume(tb_thread_ref_t self)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_size_t tb_thread_self()
{
    return (tb_size_t)pthread_self();
}

