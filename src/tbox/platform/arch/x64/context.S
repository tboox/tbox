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

    movq %rdi, 8(%rdi)      // mcontext.mc_rdi = %rdi 
    movq %rsi, 16(%rdi)     // mcontext.mc_rsi = %rsi
    movq %rdx, 24(%rdi)     // mcontext.mc_rdx = %rdx 
    movq %rcx, 32(%rdi)     // mcontext.mc_rcx = %rcx 
    movq %r8, 40(%rdi)      // mcontext.mc_r8  = %r8 
    movq %r9, 48(%rdi)      // mcontext.mc_r9  = %r9 
    movq $1, 56(%rdi)       // mcontext.mc_rax = %rax 
    movq %rbx, 64(%rdi)     // mcontext.mc_rbx = %rbx 
    movq %rbp, 72(%rdi)     // mcontext.mc_rbp = %rbp
    movq %r10, 80(%rdi)     // mcontext.mc_r10 = %r10 
    movq %r11, 88(%rdi)     // mcontext.mc_r11 = %r11 
    movq %r12, 96(%rdi)     // mcontext.mc_r12 = %r12 
    movq %r13, 104(%rdi)    // mcontext.mc_r13 = %r13 
    movq %r14, 112(%rdi)    // mcontext.mc_r14 = %r14 
    movq %r15, 120(%rdi)    // mcontext.mc_r15 = %r15 

    movq (%rsp), %rcx       // mcontext.mc_rip = %rip 
    movq %rcx, 160(%rdi)

    leaq 8(%rsp), %rcx      // mcontext.mc_rsp = %rsp 
    movq %rcx, 184(%rdi)
    
    movq 32(%rdi), %rcx     // restore %rcx

    movq $0, %rax           // return 0
    ret

endfunc


/* set mcontext
 *
 * @param mcontext      %rdi
 *
 * @return              %rax
 */
function tb_context_set_asm, export = 1

    movq 16(%rdi), %rsi
    movq 24(%rdi), %rdx
    movq 32(%rdi), %rcx
    movq 40(%rdi), %r8
    movq 48(%rdi), %r9
    movq 56(%rdi), %rax
    movq 64(%rdi), %rbx
    movq 72(%rdi), %rbp
    movq 80(%rdi), %r10
    movq 88(%rdi), %r11
    movq 96(%rdi), %r12
    movq 104(%rdi), %r13
    movq 112(%rdi), %r14
    movq 120(%rdi), %r15
    movq 184(%rdi), %rsp
    pushq 160(%rdi)   /* new %eip */
    movq 8(%rdi), %rdi
    ret

endfunc

