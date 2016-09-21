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
 *
 *             --------------------------------------------------------------------------------
 * stackdata: |                                                |         context        ||||||||
 *             -------------------------------------------------------------------------|------
 *                                                                              (16-align for macosx)
 *
 *
 *                                    func     __end   __trampoline                 from
 *             --------------------------------------------------------------------------------
 * context:   |   edi   |   esi   |   ebx   |   ebp   |   eip   |  padding  | context |  priv  |
 *             --------------------------------------------------------------------------------
 *            0         4         8         12        16        20          24        28
 *                                                              |
 *                                                      16-align for macosx
 *
 * @param stackdata     the stack data (esp + 4)
 * @param stacksize     the stack size (esp + 8)
 * @param func          the entry function (esp + 12)
 *
 * @return              the context pointer (eax)
 */
function(tb_context_make)

    // save the stack top to eax
    movl 4(%esp), %eax
    addl 8(%esp), %eax

    // reserve space for first argument(from) of context-function
    leal -8(%eax), %eax

    // 16-align of the stack top address for macosx
    andl $-16, %eax

    // TODO 40? 20
    // reserve space for context-data on context-stack 
    leal -40(%eax), %eax

    // context.ebx = func
    movl 12(%esp), %edx
    movl %edx, 8(%eax)

    // TODO
    /* return transport_t */
    /* FCTX == EDI, DATA == ESI */
    leal (%eax), %ecx
    movl %ecx, 20(%eax)

    // context.eip = the address of label __trampoline
    call 1f
1:  popl %ecx
    addl $__trampoline - 1b, %ecx
    movl %ecx, 16(%eax)

    // context.ebp = the address of label __end
    call 2f
2:  popl %ecx
    addl $__end - 2b, %ecx
    movl %ecx, 12(%eax) 

    // return the context pointer
    ret 

__trampoline:
    /* move transport_t for entering context-function */
    movl  %edi, (%esp)
    movl  %esi, 4(%esp)
    pushl %ebp

    // jump to the return or function address(ebx)
    jmp *%ebx

__end:
    // exit(0)
    xorl  %eax, %eax
    movl  %eax, (%esp)
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

    /* save registers and construct the current context
     *
     *                                    func     __end   __trampoline                 from
     *             -------------------------------------------------------------------------------
     * context:   |   edi   |   esi   |   ebx   |   ebp   |   eip   |  padding  |  context   |   priv   |
     *             -------------------------------------------------------------------------------
     *            0         4         8         12        16        20                         |
     *            |                                                                |
     *           esp                                                return address of tb_context_jump()
     */
    pushl %ebp 
    pushl %ebx 
    pushl %esi 
    pushl %edi 

    // save the current context(esp) to ecx
    movl %esp, %ecx

    // eax = argument(context)
//    movl 24(%esp), %eax
    movl 20(%esp), %eax

    // edx = argument(priv)
//    movl 28(%esp), %edx
    movl 24(%esp), %edx

    // switch to the new context(esp) and stack
    movl %eax, %esp

    /* address of returned transport_t */
    movl 20(%esp), %eax
    /* return parent fcontext_t */
    movl %ecx, (%eax)
    /* return data */
    movl %edx, 4(%eax)

    // restore registers of the new context
    popl %edi 
    popl %esi
    popl %ebx
    popl %ebp

    // jump to the address of label __trampoline(eip)
    ret $4

endfunc

