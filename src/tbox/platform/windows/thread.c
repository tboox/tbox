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
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../thread.h"

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
        thread = CreateThread(NULL, (DWORD)stack, (LPTHREAD_START_ROUTINE)tb_thread_func, (LPVOID)args, 0, NULL);
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
    DWORD code = 0;
    if (retval && GetExitCodeThread((HANDLE)thread, &code))
        *retval = (tb_int_t)code;
    else *retval = 0;

    // ok
    return 1;
}
tb_void_t tb_thread_return(tb_int_t value)
{
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
