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
 * @param mcontext      r0
 *
 * @return              r0
 */
function tb_context_get_asm, export = 1

	str	r1, [r0, #4]        // mcontext.mc_r1 = r1
	str	r2, [r0, #8]        // mcontext.mc_r2 = r2
	str	r3, [r0, #12]       // mcontext.mc_r3 = r3
	str	r4, [r0, #16]       // mcontext.mc_r4 = r4
	str	r5, [r0, #20]       // mcontext.mc_r5 = r5
	str	r6, [r0, #24]       // mcontext.mc_r6 = r6
	str	r7, [r0, #28]       // mcontext.mc_r7 = r7
	str	r8, [r0, #32]       // mcontext.mc_r8 = r8
	str	r9, [r0, #36]       // mcontext.mc_r9 = r9
	str	r10, [r0, #40]      // mcontext.mc_r10 = r10
	str	r11, [r0, #44]      // mcontext.mc_fp = r11
	str	r12, [r0, #48]      // mcontext.mc_ip = r12
	str	r13, [r0, #52]      // mcontext.mc_sp = r13
	str	r14, [r0, #56]      // mcontext.mc_lr = r14

	mov	r1, #1              /* mcontext.mc_r0 = 1
                             * 
                             * if (getcontext(ctx) == 0) 
                             *      setcontext(ctx);
                             *
                             * getcontext() will return 1 after calling setcontext()
                             */
	str	r1, [r0]

	mov	r0, #0              // return 0
	mov	pc, lr

endfunc

/* set mcontext
 *
 * @param mcontext      r0
 */
function tb_context_set_asm, export = 1

	ldr	r1, [r0, #4]        // r1 = mcontext.mc_r1
	ldr	r2, [r0, #8]        // r2 = mcontext.mc_r2
	ldr	r3, [r0, #12]       // r3 = mcontext.mc_r3
	ldr	r4, [r0, #16]       // r4 = mcontext.mc_r4
	ldr	r5, [r0, #20]       // r5 = mcontext.mc_r5
	ldr	r6, [r0, #24]       // r6 = mcontext.mc_r6
	ldr	r7, [r0, #28]       // r7 = mcontext.mc_r7
	ldr	r8, [r0, #32]       // r8 = mcontext.mc_r8
	ldr	r9, [r0, #36]       // r9 = mcontext.mc_r9
	ldr	r10, [r0, #40]      // r10 = mcontext.mc_r10
	ldr	r11, [r0, #44]      // r11 = mcontext.mc_fp
	ldr	r12, [r0, #48]      // r12 = mcontext.mc_ip
	ldr	r13, [r0, #52]      // r13 = mcontext.mc_sp
	ldr	r14, [r0, #56]      // r14 = mcontext.mc_lr

	ldr	r0, [r0]            // r0 = mcontext.mc_r0

	mov	pc, lr              // return

endfunc

