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
 * @file        socket.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "platform_socket"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "socket.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/socket.c"
#elif defined(TB_CONFIG_API_HAVE_POSIX)
#   include "posix/socket.c"
#else
tb_bool_t tb_socket_init()
{
    // ok
    return tb_true;
}
tb_void_t tb_socket_exit()
{
}
tb_handle_t tb_socket_open(tb_size_t type)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_bool_t tb_socket_pair(tb_size_t type, tb_handle_t pair[2])
{
    tb_trace_noimpl();
    return tb_false;
}
tb_size_t tb_socket_recv_buffer_size(tb_handle_t handle)
{
    tb_trace_noimpl();
    return 0;
}
tb_size_t tb_socket_send_buffer_size(tb_handle_t handle)
{
    tb_trace_noimpl();
    return 0;
}
tb_void_t tb_socket_block(tb_handle_t handle, tb_bool_t block)
{
    tb_trace_noimpl();
}
tb_long_t tb_socket_connect(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port)
{
    tb_trace_noimpl();
    return -1;
}
tb_size_t tb_socket_bind(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port)
{
    tb_trace_noimpl();
    return 0;
}
tb_bool_t tb_socket_listen(tb_handle_t handle)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_handle_t tb_socket_accept(tb_handle_t handle)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_bool_t tb_socket_kill(tb_handle_t handle, tb_size_t mode)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_socket_clos(tb_handle_t handle)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_long_t tb_socket_recv(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_send(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_recvv(tb_handle_t socket, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_sendv(tb_handle_t socket, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_hong_t tb_socket_sendf(tb_handle_t socket, tb_handle_t file, tb_hize_t offset, tb_hize_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_urecv(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_usend(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t const* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_urecvv(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_usendv(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
#endif
