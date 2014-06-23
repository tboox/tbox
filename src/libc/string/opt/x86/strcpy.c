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
 * @file        strcpy.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_ASSEMBLER_IS_GAS
//#     define TB_LIBC_STRING_OPT_STRCPY
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if 0//def TB_ASSEMBLER_IS_GAS
static tb_char_t* tb_strcpy_impl(tb_char_t* s1, tb_char_t const* s2)
{
    tb_assert_and_check_return_val(s1 && s2, tb_object_null);

    tb_size_t edi, esi, eax;
    __tb_asm__ __tb_volatile__
    (
        // align?
        "1:\n"
        "   movl (%%esi), %%eax\n" // lodsl is too slower, why?
        "   add $4, %%esi\n"
        "   movl %%eax, %%edx\n"
        "   testb %%dl, %%dl\n"
        "   je 2f\n"
        "   shr $8, %%edx\n"
        "   testb %%dl, %%dl\n"
        "   je 2f\n"
        "   shr $8, %%edx\n"
        "   testb %%dl, %%dl\n"
        "   je 2f\n"
        "   shr $8, %%edx\n"
        "   testb %%dl, %%dl\n"
        "   je 2f\n"
        "   stosl\n"
        "   jmp 1b\n"
        "2:\n"
        "   stosb\n"
        "   testb %%al, %%al\n"
        "   je 3f\n"
        "   shr $8, %%eax\n"
        "   jmp 2b\n"
        "3:\n"


        : "=&S" (esi), "=&D" (edi)
        : "0" (s2), "1" (s1) 
        : "memory", "eax", "edx"
    );
    return s1;
}
#endif
