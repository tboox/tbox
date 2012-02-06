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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * \author		ruki
 * \file		strnlen.c
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
//# 	define TB_LIBC_STRING_OPT_STRNLEN
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
#if 0//def TB_CONFIG_ASSEMBLER_GAS
tb_size_t tb_strnlen(tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(s, 0);
	if (!n) return 0;

	__tb_register__ tb_size_t r = 0;
	__tb_asm__ __tb_volatile__
	(
		" 	movl 	%1, %0\n"
		" 	decl 	%0\n"
		"1:\n"	
		" 	incl 	%0\n"
		" 	test 	%2, %2\n"
		" 	je 		2f\n"
		" 	decl 	%2\n"
		" 	cmpb 	$0, (%0)\n"
		" 	jne 	1b\n"
		"2:\n"
		" 	subl 	%1, %0"

		: "=a" (r)
		: "d" (s), "c"(n)
		: "memory"
	);
	return r;
}
#endif
