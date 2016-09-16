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

    stp x1, x2, [x0, #8]        // store x1, x2 to mcontext
    stp x3, x4, [x0, #24]       // store x3, x4 to mcontext
    stp x5, x6, [x0, #40]       // store x5, x6 to mcontext
    stp x7, x8, [x0, #56]       // store x7, x8 to mcontext
    stp x9, x10, [x0, #72]      // store x9, x10 to mcontext
    stp x11, x12, [x0, #88]     // store x11, x12 to mcontext
    stp x13, x14, [x0, #104]    // store x13, x14 to mcontext
    stp x15, x16, [x0, #120]    // store x15, x16 to mcontext
    stp x17, x18, [x0, #136]    // store x17, x18 to mcontext
    stp x19, x20, [x0, #152]    // store x19, x20 to mcontext
    stp x21, x22, [x0, #168]    // store x21, x22 to mcontext
    stp x23, x24, [x0, #184]    // store x23, x24 to mcontext
    stp x25, x26, [x0, #200]    // store x25, x26 to mcontext
    stp x27, x28, [x0, #216]    // store x27, x28 to mcontext
    stp x29, x30, [x0, #232]    // store x29, x30 to mcontext

    mov x1, sp
	str x1, [x0, #248]          // mcontext.mc_sp = sp

	mov	x1, #1                  /* mcontext.mc_x0 = 1
                                 * 
                                 * if (getcontext(ctx) == 0) 
                                 *      setcontext(ctx);
                                 *
                                 * getcontext() will return 1 after calling setcontext()
                                 */
	str x1, [x0]

	mov	x0, #0                  // return 0
	ret

endfunc

/* set mcontext
 *
 * @param mcontext      x0
 */
function tb_context_set_asm, export=1

	ldp x2, x3, [x0, #16]       // load x2, x3 from mcontext
	ldp x4, x5, [x0, #32]       // load x4, x5 from mcontext
	ldp x6, x7, [x0, #48]       // load x6, x7 from mcontext
	ldp x8, x9, [x0, #64]       // load x8, x9 from mcontext
	ldp x10, x11, [x0, #80]     // load x10, x11 from mcontext
	ldp x12, x13, [x0, #96]     // load x12, x13 from mcontext
	ldp x14, x15, [x0, #112]    // load x14, x15 from mcontext
	ldp x16, x17, [x0, #128]    // load x16, x17 from mcontext
	ldp x18, x19, [x0, #144]    // load x18, x19 from mcontext
	ldp x20, x21, [x0, #160]    // load x20, x21 from mcontext
	ldp x22, x23, [x0, #176]    // load x22, x23 from mcontext
	ldp x24, x25, [x0, #192]    // load x24, x25 from mcontext
	ldp x26, x27, [x0, #208]    // load x26, x27 from mcontext
	ldp x28, x29, [x0, #224]    // load x28, x29 from mcontext
	ldp x30, x1, [x0, #240]     // load x30, sp from mcontext
    mov sp, x1

	ldr x1, [x0, #8]            // x1 = mcontext.mc_x1
	ldr x0, [x0]                // x0 = mcontext.mc_x0

	ret                         // return

endfunc

