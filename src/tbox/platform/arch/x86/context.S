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
 * @param mcontext      [%esp + 4]
 *
 * @return              %eax
 */
function tb_context_get_asm, export = 1

	movl	4(%esp), %eax   // %eax = [%esp + 4] = mcontext

	movl	%fs, 8(%eax)    // mcontext.mc_fs = %fs
	movl	%es, 12(%eax)   // mcontext.mc_es = %es
	movl	%ds, 16(%eax)   // mcontext.mc_ds = %ds
	movl	%ss, 76(%eax)   // mcontext.mc_ss = %ss
	movl	%edi, 20(%eax)  // mcontext.mc_edi = %edi
	movl	%esi, 24(%eax)  // mcontext.mc_esi = %esi
	movl	%ebp, 28(%eax)  // mcontext.mc_ebp = %ebp
	movl	%ebx, 36(%eax)  // mcontext.mc_ebx = %ebx
	movl	%edx, 40(%eax)  // mcontext.mc_edx = %edx
	movl	%ecx, 44(%eax)  // mcontext.mc_ecx = %ecx

	movl	$1, 48(%eax)	/* mcontext.mc_eax = 1
                             * 
                             * if (getcontext(ctx) == 0) 
                             *      setcontext(ctx);
                             *
                             * getcontext() will return 1 after calling setcontext()
                             */

    /* esp + 4: ...         => mcontext.mc_esp
     * esp    : return addr => mcontext.mc_eip
     */
	movl	(%esp), %ecx	// mcontext.mc_eip = %eip (the return address of tb_context_get())
	movl	%ecx, 60(%eax)

	leal	4(%esp), %ecx	// mcontext.mc_esp = %esp + 4 (after ret)
	movl	%ecx, 72(%eax)

	movl	44(%eax), %ecx	// restore %ecx 

	movl	$0, %eax        // return 0
	ret

endfunc

/* set mcontext
 *
 * @param mcontext      [%esp + 4]
 */
function tb_context_set_asm, export = 1

	movl 4(%esp), %eax      // %eax = [%esp + 4] = mcontext

	movl 8(%eax), %fs       // %fs = mcontext.mc_fs
	movl 12(%eax), %es      // %es = mcontext.mc_es
	movl 16(%eax), %ds      // %ds = mcontext.mc_ds
	movl 76(%eax), %ss      // %ss = mcontext.mc_ss
	movl 20(%eax), %edi     // %edi = mcontext.mc_edi
	movl 24(%eax), %esi     // %esi = mcontext.mc_esi
	movl 28(%eax), %ebp     // %ebp = mcontext.mc_ebp
	movl 36(%eax), %ebx     // %ebx = mcontext.mc_ebx
	movl 40(%eax), %edx     // %edx = mcontext.mc_edx
	movl 44(%eax), %ecx     // %ecx = mcontext.mc_ecx

	movl 72(%eax), %esp     // %esp = mcontext.mc_esp

	pushl 60(%eax)	        // push mcontext.mc_eip to the return address

	movl 48(%eax), %eax     // %eax = mcontext.mc_eax

	ret                     // return and goto mcontext.mc_eip

endfunc

