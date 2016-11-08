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
 *             --------------------------------------------------------------------------------------
 * stackdata: |                                                       |         context             ||
 *             --------------------------------------------------------------------------------------
 *                                                                                              (16-align)
 *
 *             ------------------------------------------------
 * context:   |  s0  |  s1  |  s2  |  s3  |  s4  |  s5  |  s6  |
 *             ------------------------------------------------
 *            0      4      8      12     16     20     24                               
 *                          
 *                                ------------------------------------
 *                               |                                   \|/
 *                               |       __end  func             retval(from)     function arguments        
 *             ----------------------------------------------------------------------------------------
 *            |  s7  |  fp  |  retval  |  ra  |  pc  |  gp  | context |  priv  |  (a0-a4)  |  padding  |
 *             ----------------------------------------------------------------------------------------
 *            28     32     36         40     44     48     52        56       60          76          80 (16-align)
 *                                                                             |                                 
 *                                                                             |                               
 *                                                                   sp when jump to function
 *
 *
 * @param stackdata     the stack data (a0)
 * @param stacksize     the stack size (a1)
 * @param func          the entry function (a2)
 *
 * @return              the context pointer (v0)
 */
function tb_context_make, export=1
#ifdef __PIC__
    .set    noreorder
    .cpload $t9
    .set    reorder
#endif

    // save the stack top to v0
    addu $v0, $a0, $a1

    // 16-align of the stack top address 
    move $v1, $v0
    li $v0, -16
    and $v0, $v1, $v0

    /* reserve space for context-data on context-stack
     *
     * 80 = align8(76)
     */
    addiu $v0, $v0, -80

    // context.pc = func
    sw $a2, 44($v0)

    // context.gp = global pointer
    sw $gp, 48($v0)

    /* init retval = a writeable space (context)
     *
     * it will write retval(context, priv) when jump to a new context function entry first
     */
    addiu $t0, $v0, 52
    sw $t0, 36($v0)

    // context.ra = address of label __end
    la $t9, __end
    sw $t9, 40($v0)

    // return pointer to context-data
    jr $ra 

__end:

    // restore global pointer
//    lw $gp, 0($sp)

    // allocate stack frame space and save return address
    addiu $sp, $sp, -32
    sw $ra, 28($sp)

    // exit(0)
    move  $a0, $zero
    lw $t9, %call16(_exit)($gp)
    jalr $t9

endfunc

/* jump context (refer to boost.context)
 *
 * @param retval        the from-context (a0)
 * @param context       the to-context (a1)
 * @param priv          the passed user private data (a2)
 *
 * @return              the from-context (v0: retval)
 */
function tb_context_jump, export=1

    # reserve stack space first
    addiu $sp, $sp, -80

    // save registers and construct the current context
    sw $s0, ($sp)
    sw $s1, 4($sp) 
    sw $s2, 8($sp) 
    sw $s3, 12($sp)
    sw $s4, 16($sp) 
    sw $s5, 20($sp)
    sw $s6, 24($sp) 
    sw $s7, 28($sp)
    sw $fp, 32($sp)
    sw $a0, 36($sp)     // save retval 
    sw $ra, 40($sp)
    sw $ra, 44($sp)     // save ra as pc
    sw $gp, 48($sp)     // save gp

    // save the old context(sp) to a0
    move $a0, $sp

    // switch to the new context(sp) and stack
    move $sp, $a1

    // restore registers of the new context
    lw $s0, ($sp) 
    lw $s1, 4($sp)
    lw $s2, 8($sp)
    lw $s3, 12($sp)
    lw $s4, 16($sp)
    lw $s5, 20($sp) 
    lw $s6, 24($sp)
    lw $s7, 28($sp)
    lw $fp, 32($sp)
    lw $t0, 36($sp)     // load retval
    lw $ra, 40($sp)
    lw $t9, 44($sp)     // load t9 = pc
    lw $gp, 48($sp)     // load gp

    // restore stack space
    addiu $sp, $sp, 80

    // TODO a1 = a2
    // return from-context(context: a0, priv: a1) from jump 
    sw $a0, ($t0)
    sw $a1, 4($t0)
//    sw $a2, 4($t0)

    // pass old-context(context: a0, priv: a1) arguments to the context function
    move $a1, $a2 

    /* jump to the return or entry address(pc)
     *
     *             ----------------------
     * context:   |   args   |  padding  |
     *             ----------------------
     *            0  
     *            |
     *            sp
     */
    jr $t9

endfunc

