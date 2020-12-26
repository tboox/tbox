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
 * @file        exception.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "exception.h"
#include "../thread_local.h"
#include <setjmp.h>
#include <signal.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
tb_thread_local_t g_exception_local = TB_THREAD_LOCAL_INIT;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_exception_stack_exit(tb_cpointer_t priv)
{
    if (priv) tb_stack_exit((tb_stack_ref_t)priv);
}
static tb_void_t tb_exception_signal_func(tb_int_t sig)
{
    tb_stack_ref_t stack = (tb_stack_ref_t)tb_thread_local_get(&g_exception_local);
    if (stack && tb_stack_size(stack))
    {
#if defined(TB_CONFIG_LIBC_HAVE_SIGSETJMP)
        sigjmp_buf* jmpbuf = (sigjmp_buf*)tb_stack_top(stack);
        if (jmpbuf) siglongjmp(*jmpbuf, 1);
#else
        jmpbuf* jmpbuf = (jmpbuf*)tb_stack_top(stack);
        if (jmpbuf) longjmp(*jmpbuf, 1);
#endif
    }
    else
    {
        // trace
        tb_trace_e("exception: no handler for signal: %d", sig);

        // ignore signal
        signal(SIGILL, SIG_DFL);
        signal(SIGFPE, SIG_DFL);
        signal(SIGBUS, SIG_DFL);
        signal(SIGSEGV, SIG_DFL);
        signal(SIGABRT, SIG_DFL);

#ifdef TB_CONFIG_LIBC_HAVE_KILL
        // kill it
        kill(getpid(), sig);
#endif
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_exception_init_env()
{
    // init the thread local, only once
    if (!tb_thread_local_init(&g_exception_local, tb_exception_stack_exit)) return tb_false;

    // register signal handler
//  signal(SIGINT, tb_exception_signal_func);
    signal(SIGILL, tb_exception_signal_func);
    signal(SIGFPE, tb_exception_signal_func);
    signal(SIGBUS, tb_exception_signal_func);
    signal(SIGSEGV, tb_exception_signal_func);
    signal(SIGABRT, tb_exception_signal_func);
//  signal(SIGTRAP, tb_exception_signal_func);

    // ok
    return tb_true;
}
tb_void_t tb_exception_exit_env()
{
    // unregister signal handler
//  signal(SIGINT, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
//  signal(SIGTRAP, SIG_DFL);

    // exit the thread local
    tb_thread_local_exit(&g_exception_local);
}

