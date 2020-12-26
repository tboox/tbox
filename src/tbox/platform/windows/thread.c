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
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../thread.h"
#include <process.h>

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
    // done
    tb_bool_t       ok = tb_false;
    HANDLE          thread = INVALID_HANDLE_VALUE;
    tb_value_ref_t  args = tb_null;
    do
    {
        // init arguments
        args = tb_nalloc0_type(2, tb_value_t);
        tb_assert_and_check_break(args);

        // save function and private data
        args[0].ptr = (tb_pointer_t)func;
        args[1].ptr = (tb_pointer_t)priv;

        // init thread
#if defined(TB_COMPILER_IS_MSVC) || defined(_beginthreadex)
        thread = (HANDLE)_beginthreadex(NULL, (DWORD)stack, tb_thread_func, (LPVOID)args, 0, NULL);
#else
        thread = CreateThread(NULL, (DWORD)stack, (LPTHREAD_START_ROUTINE)tb_thread_func, (LPVOID)args, 0, NULL);
#endif
        tb_assert_and_check_break(thread != INVALID_HANDLE_VALUE);

        // ok
        ok = tb_true;

    } while (0);

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
    if (thread) CloseHandle((HANDLE)thread);
}
tb_long_t tb_thread_wait(tb_thread_ref_t thread, tb_long_t timeout, tb_int_t* retval)
{
    // wait
    tb_long_t r = WaitForSingleObject((HANDLE)thread, (DWORD)(timeout >= 0? timeout : INFINITE));
    tb_assert_and_check_return_val(r != WAIT_FAILED, -1);

    // timeout?
    tb_check_return_val(r != WAIT_TIMEOUT, 0);

    // error?
    tb_check_return_val(r >= WAIT_OBJECT_0, -1);

    // get the return value
    if (retval)
    {
        DWORD code = 0;
        *retval = GetExitCodeThread((HANDLE)thread, &code)? (tb_int_t)code : 0;
    }

    // ok
    return 1;
}
tb_void_t tb_thread_return(tb_int_t value)
{
    // free all thread local data on the current thread
    tb_thread_local_clear_atexit();

    // exit thread and return value
    ExitThread(value);
}
tb_bool_t tb_thread_suspend(tb_thread_ref_t thread)
{
    if (thread) return ((DWORD)-1 != SuspendThread((HANDLE)thread))? tb_true : tb_false;
    return tb_false;
}
tb_bool_t tb_thread_resume(tb_thread_ref_t thread)
{
    if (thread) return ((DWORD)-1 != ResumeThread((HANDLE)thread))? tb_true : tb_false;
    return tb_false;
}
tb_size_t tb_thread_self()
{
    return (tb_size_t)GetCurrentThreadId();
}
