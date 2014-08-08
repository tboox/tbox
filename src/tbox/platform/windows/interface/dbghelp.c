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
 * @file        dbghelp.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dbghelp.h"
#include "../../../utils/singleton.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_dbghelp_instance_init(tb_handle_t instance)
{
    // check
    tb_dbghelp_t* dbghelp = (tb_dbghelp_t*)instance;
    tb_check_return_val(dbghelp, tb_false);

    // the dbghelp module
    HANDLE module = tb_dynamic_init("dbghelp.dll");
    tb_check_return_val(module, tb_false);

    // init interfaces
    TB_INTERFACE_LOAD(dbghelp, SymInitialize);
    tb_check_return_val(dbghelp->SymInitialize, tb_false);
 
    // init symbols
    if (!dbghelp->SymInitialize(GetCurrentProcess(), tb_null, TRUE)) return tb_false;

    // init interfaces
    TB_INTERFACE_LOAD(dbghelp, SymFromAddr);
    TB_INTERFACE_LOAD(dbghelp, SymSetOptions);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_dbghelp_t* tb_dbghelp()
{
    // init
    static tb_atomic_t      s_binited = 0;
    static tb_dbghelp_t     s_dbghelp = {0};

    // init the static instance
    tb_singleton_static_init(&s_binited, &s_dbghelp, tb_dbghelp_instance_init);

    // ok
    return &s_dbghelp;
}
