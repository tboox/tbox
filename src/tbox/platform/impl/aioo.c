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
 * @file        aioo.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "platform_aioo"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../asio/aioe.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_long_t tb_aioo_rtor_wait(tb_socket_ref_t sock, tb_size_t code, tb_long_t timeout);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "../posix/aioo_select.c"
    tb_long_t tb_aioo_rtor_wait(tb_socket_ref_t sock, tb_size_t code, tb_long_t timeout)
    {
        return tb_aioo_rtor_select_wait(sock, code, timeout);
    }
#elif defined(TB_CONFIG_POSIX_HAVE_POLL)
#   include "../posix/aioo_poll.c"
    tb_long_t tb_aioo_rtor_wait(tb_socket_ref_t sock, tb_size_t code, tb_long_t timeout)
    {
        return tb_aioo_rtor_poll_wait(sock, code, timeout);
    }
#else
#   error have not available event mode
#endif

