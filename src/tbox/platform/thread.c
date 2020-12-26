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
#include "thread.h"
#include "atomic.h"
#include "time.h"
#include "thread_local.h"
#include "../utils/utils.h"
#include "impl/thread_local.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the return value type of the thread function
#ifdef TB_CONFIG_OS_WINDOWS
typedef tb_uint32_t     tb_thread_retval_t;
#else
typedef tb_pointer_t    tb_thread_retval_t;
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c_enter__
tb_void_t tb_thread_local_clear_atexit();
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
#ifndef TB_CONFIG_MICRO_ENABLE
static tb_bool_t tb_thread_local_free(tb_iterator_ref_t iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    // the local
    tb_thread_local_ref_t local = (tb_thread_local_ref_t)item;
    if (local)
    {
        // free the thread local data
        if (local->free && tb_thread_local_has(local))
        {
            // free it
            local->free(tb_thread_local_get(local));
        }
    }

    // ok
    return tb_true;
}
#endif

tb_void_t tb_thread_local_clear_atexit()
{
#ifndef TB_CONFIG_MICRO_ENABLE
    // free all thread local data on the current thread
    tb_thread_local_walk(tb_thread_local_free, tb_null);
#endif
}

#ifdef TB_CONFIG_OS_WINDOWS
static tb_thread_retval_t __tb_stdcall__ tb_thread_func(tb_pointer_t priv)
#else
static tb_thread_retval_t tb_thread_func(tb_pointer_t priv)
#endif
{
    // done
    tb_thread_retval_t  retval = (tb_thread_retval_t)0;
    tb_value_ref_t      args = (tb_value_ref_t)priv;
    do
    {
        // check
        tb_assert_and_check_break(args);

        // get the thread function
        tb_thread_func_t func = (tb_thread_func_t)args[0].ptr;
        tb_assert_and_check_break(func);

        // get the thread private data
        tb_cpointer_t priv = args[1].ptr;

        // free arguments before calling thread function
        if (args) tb_free(args);
        args = tb_null;

        // call the thread function
        retval = (tb_thread_retval_t)(tb_size_t)func(priv);

        // free all thread local data on the current thread
        tb_thread_local_clear_atexit();

    } while (0);

    // return the return value
    return retval;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/thread.c"
#elif defined(TB_CONFIG_POSIX_HAVE_PTHREAD_CREATE)
#   include "posix/thread.c"
#else
tb_thread_ref_t tb_thread_init(tb_char_t const* name, tb_thread_func_t func, tb_cpointer_t priv, tb_size_t stack)
{
    tb_used(tb_thread_func);
    tb_trace_noimpl();
    return tb_null;
}
tb_void_t tb_thread_exit(tb_thread_ref_t thread)
{
    tb_trace_noimpl();
}
tb_long_t tb_thread_wait(tb_thread_ref_t thread, tb_long_t timeout, tb_int_t* retval)
{
    tb_trace_noimpl();
    return -1;
}
tb_void_t tb_thread_return(tb_int_t value)
{
    tb_trace_noimpl();
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
    tb_trace_noimpl();
    return 0;
}
#endif

#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/thread_affinity.c"
#elif defined(TB_CONFIG_OS_MACOSX) || defined(TB_CONFIG_OS_IOS)
#   include "mach/thread_affinity.c"
#elif defined(TB_CONFIG_POSIX_HAVE_PTHREAD_SETAFFINITY_NP)
#   include "posix/thread_affinity.c"
#else
tb_bool_t tb_thread_setaffinity(tb_thread_ref_t thread, tb_cpuset_ref_t cpuset)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_thread_getaffinity(tb_thread_ref_t thread, tb_cpuset_ref_t cpuset)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif

tb_bool_t tb_thread_once(tb_atomic32_t* lock, tb_bool_t (*func)(tb_cpointer_t), tb_cpointer_t priv)
{
    // check
    tb_check_return_val(lock && func, tb_false);

    /* called?
     *
     * 0: have been not called
     * 1: be calling
     * 2: have been called and ok
     * -2: have been called and failed
     */
    tb_atomic32_t called = tb_atomic32_fetch_and_cmpset(lock, 0, 1);

    // called?
    if (called && called != 1) return called == 2;
    // have been not called? call it
    else if (!called)
    {
        // call the once function
        tb_bool_t ok = func(priv);

        // call ok
        tb_atomic32_set(lock, ok? 2 : -1);

        // ok?
        return ok;
    }
    // calling? wait it
    else
    {
        // try getting it
        tb_size_t tryn = 50;
        while ((1 == tb_atomic32_get(lock)) && tryn--)
        {
            // wait some time
            tb_msleep(100);
        }
    }

    // ok? 1: timeout, -2: failed, 2: ok
    return tb_atomic32_get(lock) == 2;
}
