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
 * @file        exception.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "exception.h"
#include "../thread_local.h"
#include "../../libc/misc/signal.h"

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
#if defined(tb_sigsetjmp) && defined(tb_siglongjmp)
        tb_sigjmpbuf_t* jmpbuf = (tb_sigjmpbuf_t*)tb_stack_top(stack);
        if (jmpbuf) tb_siglongjmp(*jmpbuf, 1);
#else
        tb_jmpbuf_t* jmpbuf = (tb_jmpbuf_t*)tb_stack_top(stack);
        if (jmpbuf) tb_longjmp(*jmpbuf, 1);
#endif
    }
    else 
    {
        // trace
        tb_trace_e("exception: no handler for signal: %d", sig);

        // ignore signal
        tb_signal(TB_SIGILL, TB_SIG_DFL);
        tb_signal(TB_SIGFPE, TB_SIG_DFL);
        tb_signal(TB_SIGBUS, TB_SIG_DFL);
        tb_signal(TB_SIGSEGV, TB_SIG_DFL);
        tb_signal(TB_SIGABRT, TB_SIG_DFL);

#ifdef TB_CONFIG_LIBC_HAVE_KILL
        // kill it
        kill(getpid(), sig);
#endif
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_exception_init()
{
    // init the thread local, only once
    if (!tb_thread_local_init(&g_exception_local, tb_exception_stack_exit)) return tb_false;

    // register signal handler
//  tb_signal(TB_SIGINT, tb_exception_signal_func);
    tb_signal(TB_SIGILL, tb_exception_signal_func);
    tb_signal(TB_SIGFPE, tb_exception_signal_func);
    tb_signal(TB_SIGBUS, tb_exception_signal_func);
    tb_signal(TB_SIGSEGV, tb_exception_signal_func);
    tb_signal(TB_SIGABRT, tb_exception_signal_func);
//  tb_signal(TB_SIGTRAP, tb_exception_signal_func);

    // ok
    return tb_true;
} 
tb_void_t tb_exception_exit()
{
    // unregister signal handler
//  tb_signal(TB_SIGINT, TB_SIG_DFL);
    tb_signal(TB_SIGILL, TB_SIG_DFL);
    tb_signal(TB_SIGFPE, TB_SIG_DFL);
    tb_signal(TB_SIGBUS, TB_SIG_DFL);
    tb_signal(TB_SIGSEGV, TB_SIG_DFL);
    tb_signal(TB_SIGABRT, TB_SIG_DFL);
//  tb_signal(TB_SIGTRAP, TB_SIG_DFL);
}

