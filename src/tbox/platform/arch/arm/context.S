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
 * macros
 */
#ifdef TB_CONFIG_OS_IOS
#   define TB_CONTEXT_SJLJ_BYTES    4
#else
#   define TB_CONTEXT_SJLJ_BYTES    0
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/* make context (refer to boost.context)
 *
 *
 *             --------------------------------------------------------------------------------------
 * stackdata: |                                                       |         context             ||
 *             --------------------------------------------------------------------------------------
 *                                                                                              (16-align)
 *
 * for macho (ios):
 *
 * save and restore the sjlj(setjmp/longjmp) exception handler on tls
 *
 *             -------------------------------------------------------
 * context:   |  sjlj  | retval |   r4    |   r5    |   r6   |   r7   |
 *             -------------------------------------------------------
 *            0        4    |   8         12        16       20                             
 *                          |
 *                           ------------------------------------------------
 *                                                                          \|/
 *                                                    __end     func      retval(from)
 *             --------------------------------------------------------------------------------------
 *            |   r8   |   r9   |   r10   |   r11   |   lr   |   pc   | context |  priv  |  padding  |
 *             --------------------------------------------------------------------------------------
 *            24       28       32        36        40       44       48        52       |     
 *                                                                    |                  |
 *                                                                    |              16-align
 *                                                                    |
 *                                                         sp when jump to function
 *
 * for elf (linux, ..):
 *
 *             -----------------------------------------------
 * context:   |  retval |   r4    |   r5    |   r6   |   r7   |
 *             -----------------------------------------------
 *            0        4    |   8         12        16                                  
 *                          |
 *                           ------------------------------------------------
 *                                                                          \|/
 *                                                    __end     func      retval(from)
 *             --------------------------------------------------------------------------------------
 *            |   r8   |   r9   |   r10   |   r11   |   lr   |   pc   | context |  priv  |  padding  |
 *             --------------------------------------------------------------------------------------
 *            20       24       28        32        36       40       44        48       |     
 *                                                                    |                  |
 *                                                                    |              16-align
 *                                                                    |
 *                                                         sp when jump to function
 *
 *
 * @param stackdata     the stack data (r0)
 * @param stacksize     the stack size (r1)
 * @param func          the entry function (r2)
 *
 * @return              the context pointer (r0)
 */
function tb_context_make, export=1

    // save the stack top to r0
    add r0, r0, r1

    // 16-align of the stack top address 
    bic r0, r0, #15

    // reserve space for context-data on context-stack
    sub r0, r0, #52 + TB_CONTEXT_SJLJ_BYTES

    // context.pc = func
    str r2, [r0, #40 + TB_CONTEXT_SJLJ_BYTES]

    /* init retval = a writeable space (context)
     *
     * it will write retval(context, priv) when jump to a new context function entry first
     */
    add r1, r0, #44 + TB_CONTEXT_SJLJ_BYTES
    str r1, [r0, #0 + TB_CONTEXT_SJLJ_BYTES]

    // context.lr = address of label __end
    adr r1, __end
    str r1, [r0, #36 + TB_CONTEXT_SJLJ_BYTES]

    // return pointer to context-data
    bx lr 

__end:
    // exit(0)
    mov r0, #0
#ifdef TB_ARCH_ELF
    bl _exit@PLT
#else
    bl __exit
#endif

endfunc

/* jump context (refer to boost.context)
 *
 * @param retval        the from-context (r0)
 * @param context       the to-context (r1)
 * @param priv          the passed user private data (r2)
 *
 * @return              the from-context (retval)
 */
function tb_context_jump, export=1

    // save lr as pc
    push {lr}

    // save retval, r4 - r11, lr
    push {r0, r4 - r11, lr}

#ifdef TB_CONFIG_OS_IOS
    // load tls to save or restore sjlj handler
    mrc p15, 0, r5, c13, c0, #3
    bic r5, r5, #3

    // load and save sjlj handler: tls[__PTK_LIBC_DYLD_Unwind_SjLj_Key]
    ldr r4, [r5, #8]
    push {r4}
#endif

    // save the old context(sp) to r0
    mov r0, sp

    // switch to the new context(sp) and stack
    mov sp, r1

#ifdef TB_CONFIG_OS_IOS
    // restore sjlj handler
    pop {r4}
    str r4, [r5, #8]
#endif

    // restore retval, r4 - r11, lr
    pop {r3, r4 - r11, lr}

    // return from-context: retval(context: r0, priv: r2) from jump 
    str r0, [r3, #0]
    str r2, [r3, #4]

    // pass old-context(context: r0, priv: r1 = r2) arguments to the context function
    mov r1, r2

    /* jump to the return or entry address(pc)
     *
     *               func      retval(from)
     *             ---------------------------------------
     * context:   |   pc   | context |  priv  |  padding  |
     *             ---------------------------------------
     *            0        4         8             
     *            |
     *            sp
     */
    pop {pc}

endfunc

