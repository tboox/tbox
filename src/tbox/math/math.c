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
 * @file        math.c
 * @ingroup     math
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "math.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_long_t tb_math_printf_format_fixed(tb_cpointer_t object, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(cstr && maxn, -1);

    // the fixed
    tb_fixed_t fixed = (tb_fixed_t)tb_p2s32(object);

    // format
#ifdef TB_CONFIG_TYPE_FLOAT
    tb_long_t size = tb_snprintf(cstr, maxn - 1, "%f", tb_fixed_to_float(fixed));
    if (size >= 0) cstr[size] = '\0';
#else
    tb_long_t size = tb_snprintf(cstr, maxn - 1, "%ld", tb_fixed_to_long(fixed));
    if (size >= 0) cstr[size] = '\0';
#endif

    // ok?
    return size;
}
    
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_math_init()
{
    // register printf("%{fixed}", fixed);
    tb_printf_object_register("fixed", tb_math_printf_format_fixed);

    // ok
    return tb_true;
}
tb_void_t tb_math_exit()
{
}

