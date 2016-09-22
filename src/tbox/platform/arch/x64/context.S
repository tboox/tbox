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

/* make context (refer to boost.context)
 *
 *             -------------------------------------------------------------------------------
 * stackdata: |                                                |         context        |||||||
 *             -------------------------------------------------------------------------|-----
 *                                                                             (16-align for macosx)
 *
 *
 *             -------------------------------------------------------------------------------
 * context:   |   r12   |   r13   |   r14   |   r15   |   rbx   |   rbp   |   rip   |   end   | args | padding ...
 *             -------------------------------------------------------------------------------
 *            0         8         16        24        32        40        48        56        |
 *                                                                                  |  16-align for macosx
 *                                                                                  |
 *                                                                       esp when jump to function
 *
 * @param stackdata     the stack data (rdi)
 * @param stacksize     the stack size (rsi)
 * @param func          the entry function (rdx)
 *
 * @return              the context pointer (rax)
 */
function(tb_context_make)

    // save the stack top to rax
    addq %rsi, %rdi
    movq %rdi, %rax

    // 16-align for the stack top address
    movabs $-16, %r8
    andq %r8, %rax

    // reserve space for context-data on context-stack
    leaq -64(%rax), %rax

    // context.rip = func
    movq %rdx, 48(%rax)

    // context.end = the address of label __end
    leaq __end(%rip), %rcx
    movq %rcx, 56(%rax)

    // return the context pointer
    ret 

__end:
    // exit(0)
    xorq  %rdi, %rdi
    call  __exit
    hlt

endfunc

/* jump context (refer to boost.context)
 *
 * @param context       the to-context (rdi)
 * @param priv          the passed user private data (rsi)
 *
 * @return              the from-context (context: rax, priv: rdx)
 */
function(tb_context_jump)

    // save registers and construct the current context
    pushq %rbp
    pushq %rbx
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12

    // save the old context(rsp) to rax
    movq %rsp, %rax

    // switch to the new context(rsp) and stack
    movq %rdi, %rsp

    // restore registers of the new context
    popq %r12
    popq %r13
    popq %r14
    popq %r15
    popq %rbx
    popq %rbp

    // restore the return or function address(rip)
    popq %r8

    // return from-context(context: rax, priv: rdx) from jump 
    movq %rsi, %rdx

    // pass old-context(context: rdi, priv: rsi) argument to the context function
    movq %rax, %rdi

    /* jump to the return or function address(rip)
     *
     *             -------------------------------
     * context: .. |   end   | args | padding ... |
     *             -------------------------------
     *             0         8        
     *             |         |  
     *            rsp   16-align for macosx
     */
    jmp *%r8

endfunc

