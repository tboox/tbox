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
#ifndef TB_CONFIG_OS_WINDOWS
#   include <signal.h>
#endif

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

// the mcontext type
typedef struct __tb_mcontext_t
{
    /*
     * The first 20 fields must match the definition of sigcontext. 
     * So that we can support sigcontext and ucontext_t at the same time.
     */
    tb_long_t               mc_onstack;    
    tb_long_t               mc_rdi;        
    tb_long_t               mc_rsi;
    tb_long_t               mc_rdx;
    tb_long_t               mc_rcx;
    tb_long_t               mc_r8;
    tb_long_t               mc_r9;
    tb_long_t               mc_rax;
    tb_long_t               mc_rbx;
    tb_long_t               mc_rbp;
    tb_long_t               mc_r10;
    tb_long_t               mc_r11;
    tb_long_t               mc_r12;
    tb_long_t               mc_r13;
    tb_long_t               mc_r14;
    tb_long_t               mc_r15;
    tb_long_t               mc_trapno;
    tb_long_t               mc_addr;
    tb_long_t               mc_flags;
    tb_long_t               mc_err;
    tb_long_t               mc_rip;
    tb_long_t               mc_cs;
    tb_long_t               mc_rflags;
    tb_long_t               mc_rsp;
    tb_long_t               mc_ss;

    tb_long_t               mc_len;         
    tb_long_t               mc_fpformat;
    tb_long_t               mc_ownedfp;

    tb_long_t               mc_fpstate[64];
    tb_long_t               padding[8];

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
	tb_int_t		        padding[8];

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

    // save arguments
    ucontext->uc_mcontext.mc_rdi = arg1;
    ucontext->uc_mcontext.mc_rsi = arg2;

    // make stack address
    tb_long_t* sp = (tb_long_t*)ucontext->uc_stack.ss_sp + ucontext->uc_stack.ss_size / sizeof(tb_long_t);

    // 16-align for macosx
    sp = (tb_long_t*)((tb_size_t)sp & ~0xf);

    // push return address(unused, only reverse the stack space)
    *--sp = 0;

    /* save function and stack address
     *
     * rdi:     arg1
     * rsi:     arg2
     * sp:      return address(0)   => mc_rsp <----- 16-align for macosx
     */
    ucontext->uc_mcontext.mc_rip = (tb_long_t)func;
    ucontext->uc_mcontext.mc_rsp = (tb_long_t)sp;
}

#if 0 //def TB_COMPILER_IS_MSVC
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
        mov [rdi + 8], rdi      // mcontext.mc_rdi = rdi 
        mov [rdi + 16], rsi     // mcontext.mc_rsi = rsi
        mov [rdi + 24], rdx     // mcontext.mc_rdx = rdx 
        mov [rdi + 32], rcx     // mcontext.mc_rcx = rcx 
        mov [rdi + 40], r8      // mcontext.mc_r8  = r8 
        mov [rdi + 48], r9      // mcontext.mc_r9  = r9 
        mov [rdi + 56], 1       /* mcontext.mc_rax = 1
                                 * 
                                 * if (getcontext(ctx) == 0) 
                                 *      setcontext(ctx);
                                 *
                                 * getcontext() will return 1 after calling setcontext()
                                 */
        mov [rdi + 64], rbx     // mcontext.mc_rbx = rbx 
        mov [rdi + 72], rbp     // mcontext.mc_rbp = rbp
        mov [rdi + 80], r10     // mcontext.mc_r10 = r10 
        mov [rdi + 88], r11     // mcontext.mc_r11 = r11 
        mov [rdi + 96], r12     // mcontext.mc_r12 = r12 
        mov [rdi + 104], r13    // mcontext.mc_r13 = r13 
        mov [rdi + 112], r14    // mcontext.mc_r14 = r14 
        mov [rdi + 120], r15    // mcontext.mc_r15 = r15 

        /* rsp + 8: ...         => mcontext.mc_rsp
         * rsp    : return addr => mcontext.mc_rip
         */
        mov rcx, [rsp]          // mcontext.mc_rip = eip (the return address of tb_context_get())
        mov [rdi + 160], rcx

        lea rcx, [rsp + 8]      // mcontext.mc_rsp = rsp + 8 (after ret)
        mov [rdi + 184], rcx
        
        mov rcx, [rdi + 32]     // restore rcx

        mov rax, 0              // return 0
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
        mov rsi, [rdi + 16]     // rsi = mcontext.mc_rsi 
        mov rdx, [rdi + 24]     // rdx = mcontext.mc_rdx 
        mov rcx, [rdi + 32]     // rcx = mcontext.mc_rcx 
        mov r8, [rdi + 40]      // r8 = mcontext.mc_r8
        mov r9, [rdi + 48]      // r9 = mcontext.mc_r9 
        mov rax, [rdi + 56]     // rax = mcontext.mc_rax
        mov rbx, [rdi + 64]     // rbx = mcontext.mc_rbx 
        mov rbp, [rdi + 72]     // rbp = mcontext.mc_rbp 
        mov r10, [rdi + 80]     // r10 = mcontext.mc_r10 
        mov r11, [rdi + 88]     // r11 = mcontext.mc_r11 
        mov r12, [rdi + 96]     // r12 = mcontext.mc_r12 
        mov r13, [rdi + 104]    // r13 = mcontext.mc_r13 
        mov r14, [rdi + 112]    // r14 = mcontext.mc_r14 
        mov r15, [rdi + 120]    // r15 = mcontext.mc_r15 

        mov rsp, [rdi + 184]    // rsp = mcontext.mc_rsp 

        push [rdi + 160]        // push mcontext.mc_rip to the return address

        mov rdi, [rdi + 8]      // rdi = mcontext.mc_rdi 

        ret                     // return and goto mcontext.mc_rip
    }
}
#endif

#endif
