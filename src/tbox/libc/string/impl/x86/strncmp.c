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
 * @author      ruki
 * @file        strncmp.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#if 0//def TB_ASSEMBLER_IS_GAS
#   define TB_LIBC_STRING_IMPL_STRNCMP
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if 0//def TB_ASSEMBLER_IS_GAS
static tb_long_t tb_strncmp_impl(tb_char_t const* s1, tb_char_t const* s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, 0);
    if (s1 == s2 || !n) return 0;

    tb_size_t r;
    tb_size_t d0, d1, d2;
    __tb_asm__ __tb_volatile__
    (
        "1:\n"
        "   decl %3\n"
        "   js 2f\n"
        "   lodsb\n"
        "   scasb\n"
        "   jne 3f\n"
        "   testb %%al,%%al\n"
        "   jne 1b\n"
        "2:\n"
        "   xorl %%eax,%%eax\n"
        "   jmp 4f\n"
        "3:\n"
        "   sbbl %%eax,%%eax\n"
        "   orb $1,%%al\n"
        "4:"

        : "=a" (r), "=&S" (d0), "=&D" (d1), "=&c" (d2)
        : "1" (s1), "2" (s2), "3" (n)
        : "memory"
    );

    return r;
}
#endif
