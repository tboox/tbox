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
 * @file        memcpy.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#if 1//def TB_ASSEMBLER_IS_GAS
#   define TB_LIBC_STRING_IMPL_MEMCPY
#endif


/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if 1
static tb_pointer_t tb_memcpy_impl(tb_pointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, tb_null);

    return memcpy(s1, s2, n);
}
#elif defined(TB_ASSEMBLER_IS_GAS)
static tb_pointer_t tb_memcpy_impl(tb_pointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, tb_null);

    
    return tb_null;
}
#endif
