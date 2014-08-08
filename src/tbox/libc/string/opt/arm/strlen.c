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
 * @file        strlen.c
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
#   define TB_LIBC_STRING_OPT_STRLEN
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_ASSEMBLER_IS_GAS

static tb_size_t tb_strlen_impl(tb_char_t const* s)
{
    tb_assert_and_check_return_val(s, 0);

    __tb_register__ tb_size_t n;
    __tb_asm__ __tb_volatile__
    (
        "   bic     r2, %1, #0x3\n"             //!< align address by 4-bytes
        "   ldr     r3, [r2], #4\n"             //!< get the first dword after alignment
        "   ands    %1, %1, #0x3\n"             //!< left bytes
        "   rsb     %0, %1, #0x0\n"             //!< n = -left
        "   beq     1f\n"                       //!< goto aligned handler
#ifdef TB_WORDS_BIGENDIAN
        "   orr     r3, r3, #0xff000000\n"
        "   subs    %1, %1, #1\n"
        "   orrgt   r3, r3, #0x00ff0000\n"
        "   subs    %1, %1, #1\n"
        "   orrgt   r3, r3, #0x0000ff00\n"
#else
        "   orr     r3, r3, #0x000000ff\n"      //!< fill 0xff
        "   subs    %1, %1, #1\n"               //!< left--
        "   orrgt   r3, r3, #0x0000ff00\n"      //!< continue to fill 0xff if left > 0
        "   subs    %1, %1, #1\n"               //!< left--
        "   orrgt   r3, r3, #0x00ff0000\n"      //!< continue to fill 0xff if left > 0
#endif
        "1:\n"                                  //!< align handler
        "   tst     r3, #0x000000ff\n"      
        "   tstne   r3, #0x0000ff00\n"
        "   tstne   r3, #0x00ff0000\n"
        "   tstne   r3, #0xff000000\n"
        "   addne   %0, %0, #4\n"
        "   ldrne   r3, [r2], #4\n"             //!< n += 4, get the next dword if the dword is not 0
        "   bne     1b\n"
#ifdef TB_WORDS_BIGENDIAN
        "   tst     r3, #0xff000000\n"
        "   addne   %0, %0, #1\n"
        "   tstne   r3, #0x00ff0000\n"
        "   addne   %0, %0, #1\n"
        "   tstne   r3, #0x0000ff00\n"
        "   addne   %0, %0, #1\n"
#else
        "   tst     r3, #0x000000ff\n"          //!< handle the last dword
        "   addne   %0, %0, #1\n"
        "   tstne   r3, #0x0000ff00\n"
        "   addne   %0, %0, #1\n"
        "   tstne   r3, #0x00ff0000\n"
        "   addne   %0, %0, #1\n"
#endif

        : "=r"(n)
        : "r"(s), "0"(0)
        : "r2", "r3"
    );

    return n;
}


#endif
