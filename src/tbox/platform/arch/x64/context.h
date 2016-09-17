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
 * @file        context.h
 *
 */
#ifndef TB_PLATFORM_ARCH_x64_CONTEXT_H
#define TB_PLATFORM_ARCH_x64_CONTEXT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// set context
#define	tb_context_set_impl(ctx)                    tb_context_set_asm(&(ctx)->uc_mcontext)

// get context
#define	tb_context_get_impl(ctx)	                tb_context_get_asm(&(ctx)->uc_mcontext)

// make context
#define	tb_context_make_impl 	                    tb_context_make_asm

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mcontext type, @note be incompatible with sigcontext
typedef struct __tb_mcontext_t
{
    tb_uint64_t             mc_rdi;        
    tb_uint64_t             mc_rsi;
    tb_uint64_t             mc_rdx;
    tb_uint64_t             mc_rcx;
    tb_uint64_t             mc_r8;
    tb_uint64_t             mc_r9;
    tb_uint64_t             mc_rax;
    tb_uint64_t             mc_rbx;
    tb_uint64_t             mc_rbp;
    tb_uint64_t             mc_r10;
    tb_uint64_t             mc_r11;
    tb_uint64_t             mc_r12;
    tb_uint64_t             mc_r13;
    tb_uint64_t             mc_r14;
    tb_uint64_t             mc_r15;
    tb_uint64_t             mc_rip;
    tb_uint64_t             mc_rsp;

}tb_mcontext_t, *tb_mcontext_ref_t;

// the ucontext stack type
typedef struct __tb_ucontext_stack_t
{
    // the stack pointer
    tb_pointer_t            ss_sp;

    // the stack size
    tb_size_t               ss_size;

}tb_ucontext_stack_t;

// the ucontext type
typedef struct __tb_ucontext_t
{
    // the mcontext
    tb_mcontext_t           uc_mcontext;

    // the ucontext link (unused)
    struct __tb_ucontext_t* uc_link;

    // the ucontext stack
    tb_ucontext_stack_t     uc_stack;

}tb_ucontext_t, *tb_ucontext_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declarations
 */

/* get mcontext
 *
 * @param mcontext      the mcontext
 *
 * @return              the error code, ok: 0
 */
tb_int_t                tb_context_get_asm(tb_mcontext_ref_t mcontext);

/* set mcontext
 *
 * @param mcontext      the mcontext
 *
 * @return              when successful, does not return. 
 */
tb_int_t                tb_context_set_asm(tb_mcontext_ref_t mcontext);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/* make ucontext
 *
 * @param ucontext      the ucontext
 * @param func          the function pointer
 * @param argc          the arguments count
 * @param arg1          the first argument
 *
 * @return              the error code, ok: 0
 */
static tb_void_t tb_context_make_asm(tb_ucontext_ref_t ucontext, tb_void_t (*func)(tb_void_t), tb_int_t argc, tb_size_t arg1)
{
    // check
    tb_assert_and_check_return(ucontext && argc == 1);

    // save arguments
#ifdef TB_CONFIG_OS_WINDOWS
    ucontext->uc_mcontext.mc_rcx = arg1;
#else
    ucontext->uc_mcontext.mc_rdi = arg1;
#endif

    // make stack address
    tb_uint64_t* sp = (tb_uint64_t*)ucontext->uc_stack.ss_sp + ucontext->uc_stack.ss_size / sizeof(tb_uint64_t);

    // 16-align for macosx
    sp = (tb_uint64_t*)((tb_size_t)sp & ~0xf);

    // push return address(unused, only reverse the stack space)
    *--sp = 0;

    /* save function and stack address
     *
     * rdi:     arg1
     * sp:      return address(0)   => mc_rsp <----- 16-align for macosx
     */
    ucontext->uc_mcontext.mc_rip = (tb_uint64_t)func;
    ucontext->uc_mcontext.mc_rsp = (tb_uint64_t)sp;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif