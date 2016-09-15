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
function tb_context_get_asm, export=1

	str x1, [x0, #8]        // mcontext.mc_x1 = x1
	str x2, [x0, #16]       // mcontext.mc_x2 = x2
	str x3, [x0, #24]       // mcontext.mc_x3 = x3
	str x4, [x0, #32]       // mcontext.mc_x4 = x4
	str x5, [x0, #40]       // mcontext.mc_x5 = x5
	str x6, [x0, #48]       // mcontext.mc_x6 = x6
	str x7, [x0, #56]       // mcontext.mc_x7 = x7
	str x8, [x0, #72]       // mcontext.mc_x8 = x8
	str x9, [x0, #80]       // mcontext.mc_x9 = x9
	str x10, [x0, #88]      // mcontext.mc_x10 = x10
	str x11, [x0, #96]      // mcontext.mc_x11 = x11
	str x12, [x0, #104]     // mcontext.mc_x12 = x12
	str x13, [x0, #112]     // mcontext.mc_x13 = x13
	str x14, [x0, #120]     // mcontext.mc_x14 = x14
	str x15, [x0, #128]     // mcontext.mc_x15 = x15
	str x16, [x0, #136]     // mcontext.mc_x16 = x16
	str x17, [x0, #144]     // mcontext.mc_x17 = x17
	str x18, [x0, #152]     // mcontext.mc_x18 = x18
	str x19, [x0, #160]     // mcontext.mc_x19 = x19
	str x20, [x0, #168]     // mcontext.mc_x20 = x20
	str x21, [x0, #176]     // mcontext.mc_x21 = x21
	str x22, [x0, #184]     // mcontext.mc_x22 = x22
	str x23, [x0, #192]     // mcontext.mc_x23 = x23
	str x24, [x0, #200]     // mcontext.mc_x24 = x24
	str x25, [x0, #208]     // mcontext.mc_x25 = x25
	str x26, [x0, #216]     // mcontext.mc_x26 = x26
	str x27, [x0, #224]     // mcontext.mc_x27 = x27
	str x28, [x0, #232]     // mcontext.mc_x28 = x28
	str x29, [x0, #240]     // mcontext.mc_fp = x29
	str x30, [x0, #248]     // mcontext.mc_lr = x30

    mov sp, x1
	str x1, [x0, #256]      // mcontext.mc_sp = sp

	mov	x1, #1              /* mcontext.mc_x0 = 1
                             * 
                             * if (getcontext(ctx) == 0) 
                             *      setcontext(ctx);
                             *
                             * getcontext() will return 1 after calling setcontext()
                             */
	str x1, [x0]

	mov	x0, #0              // return 0
	ret

endfunc

/* set mcontext
 *
 * @param mcontext      x0
 */
function tb_context_set_asm, export=1

	ldr x2, [x0, #16]       // x2 = mcontext.mc_x2
	ldr x3, [x0, #24]       // x3 = mcontext.mc_x3
	ldr x4, [x0, #32]       // x4 = mcontext.mc_x4
	ldr x5, [x0, #40]       // x5 = mcontext.mc_x5
	ldr x6, [x0, #48]       // x6 = mcontext.mc_x6
	ldr x7, [x0, #56]       // x7 = mcontext.mc_x7
	ldr x8, [x0, #72]       // x8 = mcontext.mc_x8
	ldr x9, [x0, #80]       // x9 = mcontext.mc_x9
	ldr x10, [x0, #88]      // x10 = mcontext.mc_x10
	ldr x11, [x0, #96]      // x11 = mcontext.mc_x11
	ldr x12, [x0, #104]     // x12 = mcontext.mc_x12
	ldr x13, [x0, #112]     // x13 = mcontext.mc_x13
	ldr x14, [x0, #120]     // x14 = mcontext.mc_x14
	ldr x15, [x0, #128]     // x15 = mcontext.mc_x15
	ldr x16, [x0, #136]     // x16 = mcontext.mc_x16
	ldr x17, [x0, #144]     // x17 = mcontext.mc_x17
	ldr x18, [x0, #152]     // x18 = mcontext.mc_x18
	ldr x19, [x0, #160]     // x19 = mcontext.mc_x19
	ldr x20, [x0, #168]     // x20 = mcontext.mc_x20
	ldr x21, [x0, #176]     // x21 = mcontext.mc_x21
	ldr x22, [x0, #184]     // x22 = mcontext.mc_x22
	ldr x23, [x0, #192]     // x23 = mcontext.mc_x23
	ldr x24, [x0, #200]     // x24 = mcontext.mc_x24
	ldr x25, [x0, #208]     // x25 = mcontext.mc_x25
	ldr x26, [x0, #216]     // x26 = mcontext.mc_x26
	ldr x27, [x0, #224]     // x27 = mcontext.mc_x27
	ldr x28, [x0, #232]     // x28 = mcontext.mc_x28
	ldr x29, [x0, #240]     // x29 = mcontext.mc_fp
	ldr x30, [x0, #248]     // x30 = mcontext.mc_lr

    ldr x1, [x0, #256]      // sp = mcontext.mc_sp
    mov sp, x1

	ldr x0, [x0]            // x0 = mcontext.mc_x0
	ldr x1, [x0, #8]        // x1 = mcontext.mc_x1

	ret                     // return

endfunc

