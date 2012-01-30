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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		strcmp.c
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
//# 	define TB_LIBC_STRING_OPT_STRCMP
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
#if 0//def TB_CONFIG_ASSEMBLER_GAS
tb_long_t tb_strcmp(tb_char_t const* s1, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(s1 && s2, 0);

	// FIXME: return is -1, 0, 1
	tb_size_t d0, d1;
	tb_size_t r;
	__tb_asm__ __tb_volatile__
	(
		"1:\n"
		" 	lodsb\n"
		" 	scasb\n"
		" 	jne 2f\n"
		" 	testb %%al, %%al\n"
		" 	jne 1b\n"
		" 	xorl %%eax, %%eax\n"
		" 	jmp 3f\n"
		"2:\n"
		" 	sbbl %%eax, %%eax\n"
		" 	orb $1, %%al\n"
		"3:"

		: "=a" (r), "=&S" (d0), "=&D" (d1)
		: "1" (s1), "2" (s2)
		: "memory"
	);

	return r;
}
#endif
