/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		strnlen.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#if 0//def TB_CONFIG_ASSEMBLER_GAS
# 	define TB_LIBC_STRING_OPT_STRNLEN
#endif

/* /////////////////////////////////////////////////////////
 * implemention
 */
#if 0//def TB_CONFIG_ASSEMBLER_GAS
tb_size_t tb_strnlen(tb_char_t const* s, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s, 0);

	tb_int_t edx;
	tb_int_t eax;
	__tb_asm__ __tb_volatile__
	(
		"	leal	-1(%%ecx), %%eax\n"
		"1:	incl	%%eax\n"
		"	decl	%%edx\n"
		"	jz		3f\n"
		"	cmpb	$0, (%%eax)\n"
		"	jnz		1b\n"
		"3:	subl	%%ecx, %%eax"

		: "=a" (eax), "=&d" (edx)
		: "c" (s), "1" (n + 1)
	);
	return eax;
}
#endif
