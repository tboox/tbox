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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		strlen.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_ASSEMBLER_GAS
//# 	define TB_LIBC_STRING_OPT_STRLEN
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
#if 0//def TB_CONFIG_ASSEMBLER_GAS
static tb_size_t tb_strlen_impl(tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, 0);

#if 0
	__tb_register__ tb_size_t r = 0;
	__tb_asm__ __tb_volatile__
	(
		"repne\n"
		"scasb\n"
		"notl 	%0\n"
		"decl 	%0"

		: "=c" (r)
		: "D" (s), "a" (0), "0" (0xffffffffu)
		: "memory"
	);
	return r;
#else
	__tb_register__ tb_size_t r = 0;
	__tb_asm__ __tb_volatile__
	(
		" 	movl 	%1, %0\n"
		" 	decl 	%0\n"
		"1:\n"	
		" 	incl 	%0\n"
		" 	cmpb 	$0, (%0)\n"
		" 	jne 	1b\n"
		" 	subl 	%1, %0"

		: "=a" (r)
		: "d" (s)
		: "memory"
	);
	return r;
#endif
}
#endif
