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
 * @file        socket_poll.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <sys/poll.h>
#include <sys/socket.h>
#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
#   include "../../coroutine/coroutine.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_socket_wait(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(sock, -1);

#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
    // attempt to wait it in coroutine
    if (tb_coroutine_self())
    {
        // wait it
        return tb_coroutine_waitio(sock, events, timeout);
    }
#endif

    // init
    struct pollfd pfd = {0};
    pfd.fd = tb_sock2fd(sock);
    if (events & TB_SOCKET_EVENT_RECV) pfd.events |= POLLIN;
    if (events & TB_SOCKET_EVENT_SEND) pfd.events |= POLLOUT;

    // poll
    tb_long_t r = poll(&pfd, 1, timeout);
    tb_assert_and_check_return_val(r >= 0, -1);

    // timeout?
    tb_check_return_val(r, 0);

    // error?
    tb_int_t o = 0;
    socklen_t n = sizeof(socklen_t);
    getsockopt(pfd.fd, SOL_SOCKET, SO_ERROR, &o, &n);
    if (o) return -1;

    // ok
    tb_long_t e = TB_SOCKET_EVENT_NONE;
    if (pfd.revents & POLLIN) e |= TB_SOCKET_EVENT_RECV;
    if (pfd.revents & POLLOUT) e |= TB_SOCKET_EVENT_SEND;
    if ((pfd.revents & POLLHUP) && !(e & (TB_SOCKET_EVENT_RECV | TB_SOCKET_EVENT_SEND))) 
        e |= TB_SOCKET_EVENT_RECV | TB_SOCKET_EVENT_SEND;
    return e;
}

