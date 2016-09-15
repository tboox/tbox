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
 * @param mcontext      x0
 *
 * @return              x0
 */
function tb_context_get_asm, export = 1

	str x1, [x0, #4]        // mcontext.mc_x1 = x1
	str x2, [x0, #8]        // mcontext.mc_x2 = x2
	str x3, [x0, #12]       // mcontext.mc_x3 = x3
	str x4, [x0, #16]       // mcontext.mc_x4 = x4
	str x5, [x0, #20]       // mcontext.mc_x5 = x5
	str x6, [x0, #24]       // mcontext.mc_x6 = x6
	str x7, [x0, #28]       // mcontext.mc_x7 = x7
	str x8, [x0, #32]       // mcontext.mc_x8 = x8
	str x9, [x0, #36]       // mcontext.mc_x9 = x9
	str x10, [x0, #40]      // mcontext.mc_x10 = x10
	str x11, [x0, #44]      // mcontext.mc_fp = x11
	str x12, [x0, #48]      // mcontext.mc_ip = x12
	str x13, [x0, #52]      // mcontext.mc_sp = x13
	str x14, [x0, #56]      // mcontext.mc_lr = x14

	mov	x1, #1              /* mcontext.mc_x0 = 1
                             * 
                             * if (getcontext(ctx) == 0) 
                             *      setcontext(ctx);
                             *
                             * getcontext() will return 1 after calling setcontext()
                             */
	str x1, [x0]

	mov	x0, #0              // xeturn 0
	mov	pc, lr

endfunc

/* set mcontext
 *
 * @param mcontext      x0
 */
function tb_context_set_asm, export = 1

	ldr x1, [x0, #4]        // x1 = mcontext.mc_x1
	ldr x2, [x0, #8]        // x2 = mcontext.mc_x2
	ldr x3, [x0, #12]       // x3 = mcontext.mc_x3
	ldr x4, [x0, #16]       // x4 = mcontext.mc_x4
	ldr x5, [x0, #20]       // x5 = mcontext.mc_x5
	ldr x6, [x0, #24]       // x6 = mcontext.mc_x6
	ldr x7, [x0, #28]       // x7 = mcontext.mc_x7
	ldr x8, [x0, #32]       // x8 = mcontext.mc_x8
	ldr x9, [x0, #36]       // x9 = mcontext.mc_x9
	ldr x10, [x0, #40]      // x10 = mcontext.mc_x10
	ldr x11, [x0, #44]      // x11 = mcontext.mc_fp
	ldr x12, [x0, #48]      // x12 = mcontext.mc_ip
	ldr x13, [x0, #52]      // x13 = mcontext.mc_sp
	ldr x14, [x0, #56]      // x14 = mcontext.mc_lr

	ldr x0, [x0]            // x0 = mcontext.mc_x0

	mov	pc, lr              // return

endfunc

