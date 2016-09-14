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
#include "context.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
// uses the windows api
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/context.c"
#else

// uses the getcontext and setcontext with asm (faster)
#include "arch/context.h"
#if defined(tb_context_set_impl) && \
        defined(tb_context_get_impl) && \
        defined(tb_context_make_impl)

#   undef setcontext
#   undef getcontext
#   undef makecontext
#   undef ucontext_t

#   undef TB_CONFIG_POSIX_HAVE_GETCONTEXT
#   undef TB_CONFIG_POSIX_HAVE_SETCONTEXT
#   undef TB_CONFIG_POSIX_HAVE_MAKECONTEXT
#   undef TB_CONFIG_POSIX_HAVE_SWAPCONTEXT

#   define setcontext   tb_context_set_impl
#   define getcontext   tb_context_get_impl
#   define makecontext  tb_context_make_impl
#   define ucontext_t   tb_ucontext_t

#   include "posix/context.c"

// uses the posix getcontext and setcontext api
#elif defined(TB_CONFIG_POSIX_HAVE_GETCONTEXT) && \
        defined(TB_CONFIG_POSIX_HAVE_SETCONTEXT) && \
        defined(TB_CONFIG_POSIX_HAVE_MAKECONTEXT)
#   include "posix/context.c"

// stub
#else
tb_size_t tb_context_size()
{
    tb_trace_noimpl();
    return 0;
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

    // save context
    if (!tb_context_save(context)) return tb_null;

    // ok
    return context;
}
tb_void_t tb_context_exit(tb_context_ref_t context)
{
}
tb_bool_t tb_context_save(tb_context_ref_t context)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_context_switch(tb_context_ref_t context)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_context_make(tb_context_ref_t context, tb_pointer_t stack, tb_size_t stacksize, tb_context_func_t func, tb_cpointer_t priv)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_context_swap(tb_context_ref_t context, tb_context_ref_t context_new)
{
    // swap it
    return tb_context_save(context)? tb_context_switch(context_new) : tb_false;
}
#endif
#endif

