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

/* make context
 *
 *
 *             -----------------------------------------------------------------------------------------
 * stackdata: |                                                          |         context        |||||||
 *             -----------------------------------------------------------------------------------|-----
 *                                                                                       (16-align for macosx)
 *
 *
 *                                                       func     __end           from
 *             ------------------------------------------------------------------------------------------
 * context:   |   edi   |   esi   |   ebx   |   ebp   |   eip   |   end   | context |  priv  |  padding  |
 *             ------------------------------------------------------------------------------------------
 *            0         4         8         12        16        20        24    arguments 
 *                                                              |         |
 *                                                              | 16-align for macosx
 *                                                              |
 *                                                   esp when jump to function
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

    // reserve space for context-data on context-stack 
    leal -24(%eax), %eax

    // context.eip = func
    movl 12(%esp), %edx
    movl %edx, 16(%eax)

    // context.ebp = the address of label __end
    call 1f
1:  popl %ecx
    addl $__end - 1b, %ecx
    movl %ecx, 20(%eax) 

    // return the context pointer
    ret

__end:
    // exit(0)
    xorl  %eax, %eax
    movl  %eax, (%esp)
    call  __exit
    hlt

endfunc

/* jump context
 *
 * @param context       the to-context (esp + 4)
 * @param priv          the passed user private data (esp + 8)
 *
 * @return              the from-context (context: rax, priv: rdx)
 */
function(tb_context_jump)

    // edx = argument(priv)
    movl 8(%esp), %edx

    // save registers and construct the current context
    pushl %ebp 
    pushl %ebx 
    pushl %esi 
    pushl %edi 

    // save the old context(esp) to eax
    movl %esp, %eax

    /* switch to the new context(esp) and stack
     *
     * argument(context): (esp + 4) + 4 * sizeof(dword)
     */
    movl 20(%esp), %esp

    // restore registers of the new context
    popl %edi 
    popl %esi
    popl %ebx
    popl %ebp

    // restore the return or function address(ecx)
    popl %ecx

    // return from-context(context: eax, priv: edx) from jump 
    // ...

    // pass old-context(context: eax, priv: edx) argument to the context function
    movl %eax, 4(%esp)
    movl %edx, 8(%esp)

    /* jump to the return or function address(eip)
     *
     *
     *                           old-context
     *              ------------------------------------------
     * context: .. |   end   | context |   priv   |  padding  |
     *              ------------------------------------------
     *             0         4    arguments 
     *             |         |
     *            esp  16-align for macosx
     *           (now)
     */
    jmp *%ecx

endfunc



