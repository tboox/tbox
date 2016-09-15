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
 * @file        context.S
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/* get mcontext
 *
 * @param mcontext      %rdi
 *
 * @return              %rax
 */
function tb_context_get_asm, export = 1

    movq %rdi, (%rdi)       // mcontext.mc_rdi = %rdi 
    movq %rsi, 8(%rdi)      // mcontext.mc_rsi = %rsi
    movq %rdx, 16(%rdi)     // mcontext.mc_rdx = %rdx 
    movq %rcx, 24(%rdi)     // mcontext.mc_rcx = %rcx 
    movq %r8, 32(%rdi)      // mcontext.mc_r8  = %r8 
    movq %r9, 40(%rdi)      // mcontext.mc_r9  = %r9 
    movq $1, 48(%rdi)       /* mcontext.mc_rax = 1
                             * 
                             * if (getcontext(ctx) == 0) 
                             *      setcontext(ctx);
                             *
                             * getcontext() will return 1 after calling setcontext()
                             */
    movq %rbx, 56(%rdi)     // mcontext.mc_rbx = %rbx 
    movq %rbp, 64(%rdi)     // mcontext.mc_rbp = %rbp
    movq %r10, 72(%rdi)     // mcontext.mc_r10 = %r10 
    movq %r11, 80(%rdi)     // mcontext.mc_r11 = %r11 
    movq %r12, 88(%rdi)     // mcontext.mc_r12 = %r12 
    movq %r13, 96(%rdi)     // mcontext.mc_r13 = %r13 
    movq %r14, 104(%rdi)    // mcontext.mc_r14 = %r14 
    movq %r15, 112(%rdi)    // mcontext.mc_r15 = %r15 

    /* rsp + 8: ...         => mcontext.mc_rsp
     * rsp    : return addr => mcontext.mc_rip
     */
    movq (%rsp), %rcx       // mcontext.mc_rip = %eip (the return address of tb_context_get())
    movq %rcx, 120(%rdi)

    leaq 8(%rsp), %rcx      // mcontext.mc_rsp = %rsp + 8 (after ret)
    movq %rcx, 128(%rdi)
    
    movq 24(%rdi), %rcx     // restore %rcx

    movq $0, %rax           // return 0
    ret

endfunc

/* set mcontext
 *
 * @param mcontext      %rdi
 */
function tb_context_set_asm, export = 1

    movq 8(%rdi), %rsi      // %rsi = mcontext.mc_rsi 
    movq 16(%rdi), %rdx     // %rdx = mcontext.mc_rdx 
    movq 24(%rdi), %rcx     // %rcx = mcontext.mc_rcx 
    movq 32(%rdi), %r8      // %r8 = mcontext.mc_r8
    movq 40(%rdi), %r9      // %r9 = mcontext.mc_r9 
    movq 48(%rdi), %rax     // %rax = mcontext.mc_rax
    movq 56(%rdi), %rbx     // %rbx = mcontext.mc_rbx 
    movq 64(%rdi), %rbp     // %rbp = mcontext.mc_rbp 
    movq 72(%rdi), %r10     // %r10 = mcontext.mc_r10 
    movq 80(%rdi), %r11     // %r11 = mcontext.mc_r11 
    movq 88(%rdi), %r12     // %r12 = mcontext.mc_r12 
    movq 96(%rdi), %r13     // %r13 = mcontext.mc_r13 
    movq 104(%rdi), %r14    // %r14 = mcontext.mc_r14 
    movq 112(%rdi), %r15    // %r15 = mcontext.mc_r15 

    movq 128(%rdi), %rsp    // %rsp = mcontext.mc_rsp 

    pushq 120(%rdi)         // push mcontext.mc_rip to the return address

    movq (%rdi), %rdi       // %rdi = mcontext.mc_rdi 

    ret                     // return and goto mcontext.mc_rip

endfunc

