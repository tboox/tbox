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
 * @file        user32.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "user32.h"
#include "../../../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_user32_instance_init(tb_handle_t instance, tb_cpointer_t priv)
{
    // check
    tb_user32_ref_t user32 = (tb_user32_ref_t)instance;
    tb_assert_and_check_return_val(user32, tb_false);

    // the user32 module
    HANDLE module = GetModuleHandleA("user32.dll");
    if (!module) module = tb_dynamic_init("user32.dll");
    tb_assert_and_check_return_val(module, tb_false);

    // init interfaces
    TB_INTERFACE_LOAD(user32, GetSystemMetrics);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_user32_ref_t tb_user32()
{
    // init
    static tb_atomic_t      s_binited = 0;
    static tb_user32_t      s_user32 = {0};

    // init the static instance
    tb_bool_t ok = tb_singleton_static_init(&s_binited, &s_user32, tb_user32_instance_init, tb_null);
    tb_assert(ok); tb_used(ok);

    // ok
    return &s_user32;
}
