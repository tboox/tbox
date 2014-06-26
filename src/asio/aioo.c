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
#include "impl/impl.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_long_t tb_aioo_rtor_wait(tb_socket_ref_t sock, tb_size_t code, tb_long_t timeout);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_socket_ref_t tb_aioo_sock(tb_aioo_ref_t aioo)
{
    // check
    tb_aioo_impl_t const* impl = (tb_aioo_impl_t const*)aioo;
    tb_assert_and_check_return_val(impl, tb_null);

    // the sock
    return impl->sock;
}
tb_long_t tb_aioo_wait(tb_socket_ref_t sock, tb_size_t code, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(sock && code, 0);

    // wait aioo
    return tb_aioo_rtor_wait(sock, code, timeout);
}

