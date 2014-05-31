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
 * @file        memmov.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_ASSEMBLER_GAS
#   define TB_LIBC_STRING_OPT_MEMMOV
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_ASSEMBLER_GAS
static tb_pointer_t tb_memmov_impl(tb_pointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, tb_null);

    tb_long_t eax, ecx, esi, edi;
    __tb_asm__ __tb_volatile__
    (
        "   movl    %%eax, %%edi\n"
        "   cmpl    %%esi, %%eax\n"
        "   je      2f\n" /* (optional) s2 == s1 -> NOP */
        "   jb      1f\n" /* s2 > s1 -> simple copy */
        "   leal    -1(%%esi,%%ecx), %%esi\n"
        "   leal    -1(%%eax,%%ecx), %%edi\n"
        "   std\n"
        "1: rep;    movsb\n"
        "   cld\n"
        "2:\n"

        : "=&c" (ecx), "=&S" (esi), "=&a" (eax), "=&D" (edi)
        : "0" (n), "1" (s2), "2" (s1)
        : "memory"
    );
    return (tb_pointer_t)eax;
}
#endif
