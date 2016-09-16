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
#ifndef TB_PLATFORM_ARCH_ARM64_CONTEXT_H
#define TB_PLATFORM_ARCH_ARM64_CONTEXT_H

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
#define tb_context_set_impl(ctx)                    tb_context_set_asm(&(ctx)->uc_mcontext)

// get context
#define tb_context_get_impl(ctx)                    tb_context_get_asm(&(ctx)->uc_mcontext)

// make context
#define tb_context_make_impl                        tb_context_make_asm

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mcontext type, @note be incompatible with sigcontext
typedef struct __tb_mcontext_t
{
     tb_uint64_t            mc_x0;
     tb_uint64_t            mc_x1;
     tb_uint64_t            mc_x2;
     tb_uint64_t            mc_x3;
     tb_uint64_t            mc_x4;
     tb_uint64_t            mc_x5;
     tb_uint64_t            mc_x6;
     tb_uint64_t            mc_x7;
     tb_uint64_t            mc_x8;
     tb_uint64_t            mc_x9;
     tb_uint64_t            mc_x10;
     tb_uint64_t            mc_x11;
     tb_uint64_t            mc_x12;
     tb_uint64_t            mc_x13;
     tb_uint64_t            mc_x14;
     tb_uint64_t            mc_x15;
     tb_uint64_t            mc_x16;
     tb_uint64_t            mc_x17;
     tb_uint64_t            mc_x18;
     tb_uint64_t            mc_x19;
     tb_uint64_t            mc_x20;
     tb_uint64_t            mc_x21;
     tb_uint64_t            mc_x22;
     tb_uint64_t            mc_x23;
     tb_uint64_t            mc_x24;
     tb_uint64_t            mc_x25;
     tb_uint64_t            mc_x26;
     tb_uint64_t            mc_x27;
     tb_uint64_t            mc_x28;
     tb_uint64_t            mc_fp;
     tb_uint64_t            mc_lr;
     tb_uint64_t            mc_sp;

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
 * @param arg2          the second argument
 *
 * @return              the error code, ok: 0
 */
static tb_void_t tb_context_make_asm(tb_ucontext_ref_t ucontext, tb_void_t (*func)(tb_void_t), tb_int_t argc, tb_int_t arg1, tb_int_t arg2)
{
    // check
    tb_assert_and_check_return(ucontext && argc == 2);

    // make stack address
    tb_uint64_t* sp = (tb_uint64_t*)ucontext->uc_stack.ss_sp + ucontext->uc_stack.ss_size / sizeof(tb_uint64_t);

    // 16-align
    sp = (tb_uint64_t*)((tb_size_t)sp & ~0xf);

    // save arguments
    ucontext->uc_mcontext.mc_x0 = arg1;
    ucontext->uc_mcontext.mc_x1 = arg2;

    /* save function and stack address
     *
     * x0:     arg1
     * x1:     arg2
     * sp:     return address(0)   => mc_sp 
     */
    ucontext->uc_mcontext.mc_lr = (tb_uint64_t)func;
    ucontext->uc_mcontext.mc_sp = (tb_uint64_t)sp;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
