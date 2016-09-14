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

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_context_size()
{
    return sizeof(CONTEXT);
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
    return tb_context_save(context)? context : tb_null;
}
tb_void_t tb_context_exit(tb_context_ref_t context)
{
    // do nothing
}
tb_bool_t tb_context_save(tb_context_ref_t context)
{
    // check
    LPCONTEXT mcontext = (LPCONTEXT)context;
    tb_assert_and_check_return_val(mcontext, tb_false);

    // save and restore the full machine context 
    mcontext->ContextFlags = CONTEXT_FULL;

    // get it
    return GetThreadContext(GetCurrentThread(), mcontext);
}
tb_bool_t tb_context_switch(tb_context_ref_t context)
{
    // check
    LPCONTEXT mcontext = (LPCONTEXT)context;
    tb_assert_and_check_return_val(mcontext, tb_false);

    // set it
    return SetThreadContext(GetCurrentThread(), mcontext);
}
#ifdef TB_ARCH_x64
tb_bool_t tb_context_make(tb_context_ref_t context, tb_pointer_t stack, tb_size_t stacksize, tb_context_func_t func, tb_cpointer_t priv)
{
    // check
    LPCONTEXT mcontext = (LPCONTEXT)context;
    tb_assert_and_check_return_val(mcontext && stack && stacksize && func, tb_false);

    // make stack address
    tb_long_t* sp = (tb_long_t*)stack + stacksize / sizeof(tb_long_t);
 
    // push return address(unused, only reverse the stack space)
    *--sp = 0;

    // push arguments
    tb_uint64_t value = tb_p2u64(priv);
    mcontext->Rdi = (tb_long_t)(tb_uint32_t)(value >> 32);
    mcontext->Rsi = (tb_long_t)(tb_uint32_t)(value);

    /* save function and stack address
     *
     * rdi:     arg1
     * rsi:     arg2
     * sp:      return address(0)   => rsp 
     */
    mcontext->Rip = (tb_long_t)func;
    mcontext->Rsp = (tb_long_t)sp;
    tb_assert_static(sizeof(tb_long_t) == 8);

    // save and restore the full machine context 
    mcontext->ContextFlags = CONTEXT_FULL;

    // ok
    return tb_true;
}
#else
tb_bool_t tb_context_make(tb_context_ref_t context, tb_pointer_t stack, tb_size_t stacksize, tb_context_func_t func, tb_cpointer_t priv)
{
    // check
    LPCONTEXT mcontext = (LPCONTEXT)context;
    tb_assert_and_check_return_val(mcontext && stack && stacksize && func, tb_false);

    // make stack address
    tb_long_t* sp = (tb_long_t*)stack + stacksize / sizeof(tb_long_t);

    // push arguments
    tb_uint64_t value = tb_p2u64(priv);
    *--sp = (tb_long_t)(tb_uint32_t)(value);
    *--sp = (tb_long_t)(tb_uint32_t)(value >> 32);

    // push return address(unused, only reverse the stack space)
    *--sp = 0;

    /* save function and stack address
     *
     * sp + 8:  arg2
     * sp + 4:  arg1                         
     * sp:      return address(0)   => esp 
     */
    mcontext->Eip = (tb_long_t)func;
    mcontext->Esp = (tb_long_t)sp;
    tb_assert_static(sizeof(tb_long_t) == 4);

    // save and restore the full machine context 
    mcontext->ContextFlags = CONTEXT_FULL;

    // ok
    return tb_true;
}
#endif
tb_bool_t tb_context_swap(tb_context_ref_t context, tb_context_ref_t context_new)
{
    // swap it
    return tb_context_save(context)? tb_context_switch(context_new) : tb_false;
}

