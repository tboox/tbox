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
 * @file        shell32.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "shell32.h"
#include "../../../utils/singleton.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_shell32_instance_init(tb_handle_t instance, tb_cpointer_t priv)
{
    // check
    tb_shell32_ref_t shell32 = (tb_shell32_ref_t)instance;
    tb_check_return_val(shell32, tb_false);

    // the shell32 module
    HANDLE module = GetModuleHandleA("shell32.dll");
    if (!module) module = tb_dynamic_init("shell32.dll");
    tb_check_return_val(module, tb_false);

    // init interfaces
    TB_INTERFACE_LOAD(shell32, SHGetSpecialFolderLocation);
    TB_INTERFACE_LOAD(shell32, SHGetPathFromIDListW);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_shell32_ref_t tb_shell32()
{
    // init
    static tb_atomic_t      s_binited = 0;
    static tb_shell32_t     s_shell32 = {0};

    // init the static instance
    tb_singleton_static_init(&s_binited, &s_shell32, tb_shell32_instance_init, tb_null);

    // ok
    return &s_shell32;
}
