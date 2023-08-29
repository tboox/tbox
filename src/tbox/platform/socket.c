/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        socket.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "platform_socket"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "socket.h"
#include "impl/socket.h"
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
#   include "../coroutine/coroutine.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c_enter__
tb_long_t tb_socket_wait_impl(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout);
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/socket.c"
#elif defined(TB_CONFIG_POSIX_HAVE_SOCKET)
#   include "posix/socket.c"
#else
tb_bool_t tb_socket_init_env()
{
    // ok
    return tb_true;
}
tb_void_t tb_socket_exit_env()
{
}
tb_socket_ref_t tb_socket_init(tb_size_t type, tb_size_t family)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_bool_t tb_socket_pair(tb_size_t type, tb_socket_ref_t pair[2])
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_socket_ctrl(tb_socket_ref_t sock, tb_size_t ctrl, ...)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_long_t tb_socket_connect(tb_socket_ref_t sock, tb_ipaddr_ref_t addr)
{
    tb_trace_noimpl();
    return -1;
}
tb_bool_t tb_socket_bind(tb_socket_ref_t sock, tb_ipaddr_ref_t addr)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_socket_listen(tb_socket_ref_t sock, tb_size_t backlog)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_socket_ref_t tb_socket_accept(tb_socket_ref_t sock, tb_ipaddr_ref_t addr)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_bool_t tb_socket_local(tb_socket_ref_t sock, tb_ipaddr_ref_t addr)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_socket_kill(tb_socket_ref_t sock, tb_size_t mode)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_socket_exit(tb_socket_ref_t sock)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_long_t tb_socket_recv(tb_socket_ref_t sock, tb_byte_t* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_send(tb_socket_ref_t sock, tb_byte_t const* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_recvv(tb_socket_ref_t sock, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_sendv(tb_socket_ref_t sock, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_hong_t tb_socket_sendf(tb_socket_ref_t sock, tb_file_ref_t file, tb_hize_t offset, tb_hize_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_urecv(tb_socket_ref_t sock, tb_ipaddr_ref_t addr, tb_byte_t* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_usend(tb_socket_ref_t sock, tb_ipaddr_ref_t addr, tb_byte_t const* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_urecvv(tb_socket_ref_t sock, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_socket_usendv(tb_socket_ref_t sock, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
#endif

#if defined(TB_CONFIG_OS_WINDOWS)
#   include "posix/socket_select.c"
#elif defined(TB_CONFIG_POSIX_HAVE_POLL) && \
        !defined(TB_CONFIG_OS_MACOSX) /* poll(fifo) exists bug on macosx, @see demo/platform/named_pipe.c */
#   include "posix/socket_poll.c"
#elif defined(TB_CONFIG_POSIX_HAVE_SELECT)
#   include "posix/socket_select.c"
#else
tb_long_t tb_socket_wait_impl(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    tb_trace_noimpl();
    return -1;
}
#endif
tb_long_t tb_socket_wait(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
#ifndef TB_CONFIG_OS_WINDOWS
    // poll it directly if timeout is zero
    if (!timeout)
        return tb_socket_wait_impl(sock, events, 0);
#endif

#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
    // attempt to wait it in coroutine
    if (tb_coroutine_self())
    {
        tb_poller_object_t object;
        object.type = TB_POLLER_OBJECT_SOCK;
        object.ref.sock = sock;
        return tb_coroutine_waitio(&object, events, timeout);
    }
#endif
    return tb_socket_wait_impl(sock, events, timeout);
}

tb_bool_t tb_socket_brecv(tb_socket_ref_t sock, tb_byte_t* data, tb_size_t size)
{
    // recv data
    tb_size_t recv = 0;
    tb_long_t wait = 0;
    while (recv < size)
    {
        // recv it
        tb_long_t real = tb_socket_recv(sock, data + recv, size - recv);

        // has data?
        if (real > 0)
        {
            recv += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = tb_socket_wait(sock, TB_SOCKET_EVENT_RECV, -1);
            tb_check_break(wait > 0);
        }
        // failed or end?
        else break;
    }
    return recv == size;
}
tb_bool_t tb_socket_bsend(tb_socket_ref_t sock, tb_byte_t const* data, tb_size_t size)
{
    // send data
    tb_size_t send = 0;
    tb_long_t wait = 0;
    while (send < size)
    {
        // send it
        tb_long_t real = tb_socket_send(sock, data + send, size - send);

        // has data?
        if (real > 0)
        {
            send += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = tb_socket_wait(sock, TB_SOCKET_EVENT_SEND, -1);
            tb_check_break(wait > 0);
        }
        // failed or end?
        else break;
    }
    return send == size;
}
