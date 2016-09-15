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
#ifndef TB_PLATFORM_ARCH_x86_CONTEXT_H
#define TB_PLATFORM_ARCH_x86_CONTEXT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#ifndef TB_CONFIG_OS_WINDOWS
#   include <signal.h>
#endif

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

// the mcontext type
typedef struct __tb_mcontext_t
{
    /*
     * The first 20 fields must match the definition of sigcontext. 
     * So that we can support sigcontext and ucontext_t at the same time.
     */
    tb_int_t                mc_onstack;
    tb_int_t                mc_gs;
    tb_int_t                mc_fs;
    tb_int_t                mc_es;
    tb_int_t                mc_ds;
    tb_int_t                mc_edi;
    tb_int_t                mc_esi;
    tb_int_t                mc_ebp;
    tb_int_t                mc_isp;
    tb_int_t                mc_ebx;
    tb_int_t                mc_edx;
    tb_int_t                mc_ecx;
    tb_int_t                mc_eax;
    tb_int_t                mc_trapno;
    tb_int_t                mc_err;
    tb_int_t                mc_eip;
    tb_int_t                mc_cs;
    tb_int_t                mc_eflags;
    tb_int_t                mc_esp; 
    tb_int_t                mc_ss;

    tb_int_t                mc_fpregs[28];
    tb_int_t                padding[17];

}tb_mcontext_t, *tb_mcontext_ref_t;

// the ucontext stack type
#ifdef TB_CONFIG_OS_WINDOWS
typedef struct __tb_ucontext_stack_t
{
    // the stack pointer
    tb_pointer_t            ss_sp;

    // the stack size
    tb_size_t               ss_size;

}tb_ucontext_stack_t;
#endif

// the ucontext type
typedef struct __tb_ucontext_t
{
#ifndef TB_CONFIG_OS_WINDOWS
    /* the sigmask (unused)
     *
     * Keep the order of the first two fields. 
     * Also, keep them the first two fields in the structure.
     * This way we can have a union with struct sigcontext and ucontext_t. 
     * This allows us to support them both at the same time.
     *
     * note: the union is not defined, though.
     */
    sigset_t                uc_sigmask;
#endif

    // the mcontext
    tb_mcontext_t           uc_mcontext;

    // the ucontext link (unused)
    struct __tb_ucontext_t* uc_link;

    // the ucontext stack
#ifdef TB_CONFIG_OS_WINDOWS
    tb_ucontext_stack_t     uc_stack;
#else
    stack_t                 uc_stack;
#endif

    // the padding data (unused)
    tb_int_t                padding[8];

}tb_ucontext_t, *tb_ucontext_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declarations
 */

#ifndef TB_COMPILER_IS_MSVC
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
#endif

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
    tb_assert_static(sizeof(tb_long_t) == 4);

    // make stack address
    tb_long_t* sp = (tb_long_t*)ucontext->uc_stack.ss_sp + ucontext->uc_stack.ss_size / sizeof(tb_long_t);

    // reserve the arguments space
    sp -= argc;

    // 16-align for macosx
    sp = (tb_long_t*)((tb_size_t)sp & ~0xf);

    // push arguments
    sp[0] = arg1;
    sp[1] = arg2;

    // push return address(unused, only reverse the stack space)
    *--sp = 0;

    /* save function and stack address
     *
     *          padding 
     * sp + 8:  arg2
     * sp + 4:  arg1                       
     * sp:      return address(0)   => mc_esp <-------- 16-align for macosx
     */
    ucontext->uc_mcontext.mc_eip = (tb_long_t)func;
    ucontext->uc_mcontext.mc_esp = (tb_long_t)sp;
}

#ifdef TB_COMPILER_IS_MSVC
/* get mcontext
 *
 * @param mcontext      the mcontext
 *
 * @return              the error code, ok: 0
 */
static __declspec(naked) tb_int_t tb_context_get_asm(tb_mcontext_ref_t mcontext)
{
    __tb_asm__ 
    {
        mov eax, [esp + 4]      // eax = [esp + 4] = mcontext

        mov [eax + 8], fs      // mcontext.mc_fs = fs
        mov [eax + 12], es     // mcontext.mc_es = es
        mov [eax + 16], ds     // mcontext.mc_ds = ds
        mov [eax + 76], ss     // mcontext.mc_ss = ss
        mov [eax + 20], edi     // mcontext.mc_edi = edi
        mov [eax + 24], esi     // mcontext.mc_esi = esi
        mov [eax + 28], ebp     // mcontext.mc_ebp = ebp
        mov [eax + 36], ebx     // mcontext.mc_ebx = ebx
        mov [eax + 40], edx     // mcontext.mc_edx = edx
        mov [eax + 44], ecx     // mcontext.mc_ecx = ecx

        mov [eax + 48], 1	    /* mcontext.mc_eax = 1
                                 * 
                                 * if (getcontext(ctx) == 0) 
                                 *      setcontext(ctx);
                                 *
                                 * getcontext() will return 1 after calling setcontext()
                                 */

        /* esp + 4: ...         => mcontext.mc_esp
         * esp    : return addr => mcontext.mc_eip
         */
        mov ecx, [esp]	        // mcontext.mc_eip = eip (the return address of tb_context_get())
        mov [eax + 60], ecx

        lea ecx, [esp + 4]	    // mcontext.mc_esp = esp + 4 (after ret)
        mov [eax + 72], ecx

        mov ecx, [eax + 44]	    // restore ecx 

        mov eax, 0              // return 0
        ret
    }
}

/* set mcontext
 *
 * @param mcontext      the mcontext
 *
 * @return              when successful, does not return. 
 */
static __declspec(naked) tb_int_t tb_context_set_asm(tb_mcontext_ref_t mcontext)
{
    __tb_asm__
    {
        mov eax, [esp + 4]      // eax = [esp + 4] = mcontext

        mov fs, [eax + 8]       // fs = mcontext.mc_fs
        mov es, [eax + 12]      // es = mcontext.mc_es
        mov ds, [eax + 16]      // ds = mcontext.mc_ds
        mov ss, [eax + 76]      // ss = mcontext.mc_ss
        mov edi, [eax + 20]     // edi = mcontext.mc_edi
        mov esi, [eax + 24]     // esi = mcontext.mc_esi
        mov ebp, [eax + 28]     // ebp = mcontext.mc_ebp
        mov ebx, [eax + 36]     // ebx = mcontext.mc_ebx
        mov edx, [eax + 40]     // edx = mcontext.mc_edx
        mov ecx, [eax + 44]     // ecx = mcontext.mc_ecx

        mov esp, [eax + 72]     // esp = mcontext.mc_esp

        push [eax + 60]	        // push mcontext.mc_eip to the return address

        mov eax, [eax + 48]     // eax = mcontext.mc_eax

        ret                     // return and goto mcontext.mc_eip
    }
}
#endif

#endif
