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
function tb_context_get_asm, export=1

	movl 4(%esp), %eax      // %eax = [%esp + 4] = mcontext

	movw %fs, (%eax)       // mcontext.mc_fs = %fs
	movw %es, 4(%eax)      // mcontext.mc_es = %es
	movw %ds, 8(%eax)      // mcontext.mc_ds = %ds
	movw %ss, 12(%eax)      // mcontext.mc_ss = %ss
	movl %edi, 16(%eax)     // mcontext.mc_edi = %edi
	movl %esi, 20(%eax)     // mcontext.mc_esi = %esi
	movl %ebp, 24(%eax)     // mcontext.mc_ebp = %ebp
	movl %ebx, 28(%eax)     // mcontext.mc_ebx = %ebx
	movl %edx, 32(%eax)     // mcontext.mc_edx = %edx
	movl %ecx, 36(%eax)     // mcontext.mc_ecx = %ecx

	movl $1, 40(%eax)	    /* mcontext.mc_eax = 1
                             * 
                             * if (getcontext(ctx) == 0) 
                             *      setcontext(ctx);
                             *
                             * getcontext() will return 1 after calling setcontext()
                             */

    /* esp + 4: ...         => mcontext.mc_esp
     * esp    : return addr => mcontext.mc_eip
     */
	movl (%esp), %ecx	    // mcontext.mc_eip = %eip (the return address of tb_context_get())
	movl %ecx, 44(%eax)

	leal 4(%esp), %ecx	    // mcontext.mc_esp = %esp + 4 (after ret)
	movl %ecx, 48(%eax)

	movl 36(%eax), %ecx	    // restore %ecx 

	movl $0, %eax           // return 0
	ret

endfunc

/* set mcontext
 *
 * @param mcontext      [%esp + 4]
 */
function tb_context_set_asm, export=1

	movl 4(%esp), %eax      // %eax = [%esp + 4] = mcontext

	movw (%eax), %fs       // %fs = mcontext.mc_fs
	movw 4(%eax), %es      // %es = mcontext.mc_es
	movw 8(%eax), %ds      // %ds = mcontext.mc_ds
	movw 12(%eax), %ss      // %ss = mcontext.mc_ss
	movl 16(%eax), %edi     // %edi = mcontext.mc_edi
	movl 20(%eax), %esi     // %esi = mcontext.mc_esi
	movl 24(%eax), %ebp     // %ebp = mcontext.mc_ebp
	movl 28(%eax), %ebx     // %ebx = mcontext.mc_ebx
	movl 32(%eax), %edx     // %edx = mcontext.mc_edx
	movl 36(%eax), %ecx     // %ecx = mcontext.mc_ecx

	movl 48(%eax), %esp     // %esp = mcontext.mc_esp

	pushl 44(%eax)	        // push mcontext.mc_eip to the return address

	movl 40(%eax), %eax     // %eax = mcontext.mc_eax

	ret                     // return and goto mcontext.mc_eip

endfunc

