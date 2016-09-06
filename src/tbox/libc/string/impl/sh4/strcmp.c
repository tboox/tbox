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
 * @file        strcmp.c
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
//#     define TB_LIBC_STRING_IMPL_STRCMP
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if 0//def TB_ASSEMBLER_IS_GAS
static tb_long_t tb_strcmp_impl(tb_char_t const* s1, tb_char_t const* s2)
{
    tb_assert_and_check_return_val(s1 && s2, 0);


    return 0;
}
#endif
