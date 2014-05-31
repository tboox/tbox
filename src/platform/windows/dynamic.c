/*!The Treasure Box dynamic
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
 * @file        dynamic.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../dynamic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_handle_t tb_dynamic_init(tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(name, tb_null);

    // atow
    tb_wchar_t temp[TB_PATH_MAXN] = {0};
    if (!tb_atow(temp, name, TB_PATH_MAXN)) return tb_null;

    // load
    return (tb_handle_t)LoadLibraryExW(temp, tb_null, LOAD_WITH_ALTERED_SEARCH_PATH);
}
tb_void_t tb_dynamic_exit(tb_handle_t dynamic)
{
    tb_assert_and_check_return(dynamic);
    FreeLibrary(dynamic);
}
tb_pointer_t tb_dynamic_func(tb_handle_t dynamic, tb_char_t const* name)
{
    tb_assert_and_check_return_val(dynamic && name, tb_null);
    return (tb_pointer_t)GetProcAddress(dynamic, name);
}
tb_pointer_t tb_dynamic_pvar(tb_handle_t dynamic, tb_char_t const* name)
{
    tb_assert_and_check_return_val(dynamic && name, tb_null);
    return (tb_pointer_t)GetProcAddress(dynamic, name);
}
