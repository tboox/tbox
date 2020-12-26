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
 * @file        socket_poll.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <poll.h>
#include <sys/socket.h>
#include <errno.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_socket_wait_impl(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(sock, -1);

    // init
    struct pollfd pfd = {0};
    pfd.fd = tb_sock2fd(sock);
    if (events & TB_SOCKET_EVENT_RECV) pfd.events |= POLLIN;
    if (events & TB_SOCKET_EVENT_SEND) pfd.events |= POLLOUT;

    // poll
    tb_long_t r = poll(&pfd, 1, timeout);

    // timeout or interrupted?
    if (!r || (r == -1 && errno == EINTR))
        return 0;

    // poll error?
    tb_assert_and_check_return_val(r >= 0, -1);

    // socket error?
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

