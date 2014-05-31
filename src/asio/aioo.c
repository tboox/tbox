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
 * @file        aioo.c
 * @ingroup     asio
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aioo.h"
#include "aioe.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_long_t tb_aioo_reactor_wait(tb_handle_t handle, tb_size_t code, tb_long_t timeout);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_cpointer_t tb_aioo_priv(tb_handle_t handle)
{
    // check
    tb_aioo_t const* aioo = (tb_aioo_t const*)handle;
    tb_assert_and_check_return_val(aioo, tb_null);

    // the handle
    return aioo->priv;
}
tb_handle_t tb_aioo_handle(tb_handle_t handle)
{
    // check
    tb_aioo_t const* aioo = (tb_aioo_t const*)handle;
    tb_assert_and_check_return_val(aioo, tb_null);

    // the handle
    return aioo->handle;
}
tb_long_t tb_aioo_wait(tb_handle_t handle, tb_size_t code, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(handle && code, 0);

    // wait aioo
    return tb_aioo_reactor_wait(handle, code, timeout);
}

