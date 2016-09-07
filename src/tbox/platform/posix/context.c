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
 * @file        context.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#if defined(TB_CONFIG_POSIX_HAVE_GETCONTEXT) && \
        defined(TB_CONFIG_POSIX_HAVE_SETCONTEXT) && \
        defined(TB_CONFIG_POSIX_HAVE_MAKECONTEXT)
#   include <ucontext.h>
#endif
#include <signal.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_context_size()
{
    return sizeof(ucontext_t);
}
tb_context_ref_t tb_context_init(tb_byte_t* data, tb_size_t size)
{
    // check size
    tb_size_t context_size = tb_context_size();
    tb_assert_and_check_return_val(data && context_size && context_size <= size, tb_null);

    // get context
    tb_context_ref_t context = (tb_context_ref_t)data;

    // init context 
    tb_memset(data, 0, context_size);

    // init sigmask
    sigset_t zero;
    sigemptyset(&zero);
    sigprocmask(SIG_BLOCK, &zero, &((ucontext_t*)context)->uc_sigmask);

    // save context
    return tb_context_save(context)? context : tb_null;
}
tb_void_t tb_context_exit(tb_context_ref_t context)
{
    // do nothing
}
tb_bool_t tb_context_save(tb_context_ref_t context)
{
    // check
    ucontext_t* ucontext = (ucontext_t*)context;
    tb_assert_and_check_return_val(ucontext, tb_false);

    // get it
    return getcontext(ucontext) == 0;
}
tb_bool_t tb_context_switch(tb_context_ref_t context)
{
    // check
    ucontext_t* ucontext = (ucontext_t*)context;
    tb_assert_and_check_return_val(ucontext, tb_false);

    // set it
    return setcontext(ucontext) == 0;
}
tb_bool_t tb_context_make(tb_context_ref_t context, tb_context_ref_t context_link, tb_pointer_t stack, tb_size_t stacksize, tb_context_func_t func, tb_cpointer_t priv)
{
    // check
    ucontext_t* ucontext = (ucontext_t*)context;
    tb_assert_and_check_return_val(ucontext && func, tb_false);

    // init stack and size
    ucontext->uc_stack.ss_sp    = stack;
    ucontext->uc_stack.ss_size  = stacksize;

    // init link
    ucontext->uc_link = (ucontext_t*)context_link;

    // make it
    tb_uint64_t value = tb_p2u64(priv);
    makecontext(ucontext, (tb_void_t(*)())func, 2, (tb_uint32_t)(value >> 32), (tb_uint32_t)value);

    // ok
    return tb_true;
}
#ifdef TB_CONFIG_POSIX_HAVE_SWAPCONTEXT
tb_bool_t tb_context_swap(tb_context_ref_t context, tb_context_ref_t context_new)
{
    // check
    tb_assert_and_check_return_val(context && context_new, tb_false);

    // swap it
    return swapcontext((ucontext_t*)context, (ucontext_t*)context_new) == 0;
}
#endif
