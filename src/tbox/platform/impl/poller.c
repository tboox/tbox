/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2017, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        poller.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "poller.h"
#include "../../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_poller_sock_data_init(tb_poller_t* poller)
{
    // check
    tb_assert(poller);

    // init it
    poller->data = tb_null;
    poller->maxn = 0;
}
tb_void_t tb_poller_sock_data_exit(tb_poller_t* poller)
{
    // check
    tb_assert(poller);

    // exit socket data
    if (poller->data) tb_free(poller->data);
    poller->data = tb_null;
    poller->maxn = 0;
}
tb_void_t tb_poller_sock_data_clear(tb_poller_t* poller)
{
    // check
    tb_assert(poller);

    // clear data
    if (poller->data) tb_memset(poller->data, 0, poller->maxn * sizeof(tb_cpointer_t));
}
tb_void_t tb_poller_sock_data_insert(tb_poller_t* poller, tb_socket_ref_t sock, tb_cpointer_t priv)
{
    // check
    tb_long_t fd = tb_sock2fd(sock);
    tb_assert(poller && fd > 0 && fd < TB_MAXS32);

    // not null?
    if (priv)
    {
        // no data? init it first
        tb_size_t need = fd + 1;
        if (!poller->data)
        {
            // init data
            poller->data = tb_nalloc0_type(need, tb_cpointer_t);
            tb_assert_and_check_return(poller->data);

            // init data size
            poller->maxn = need;
        }
        else if (need > poller->maxn)
        {
            // grow data
            poller->data = (tb_cpointer_t*)tb_ralloc(poller->data, need * sizeof(tb_cpointer_t));
            tb_assert_and_check_return(poller->data);

            // init growed space
            tb_memset(poller->data + poller->maxn, 0, (need - poller->maxn) * sizeof(tb_cpointer_t));

            // grow data size
            poller->maxn = need;
        }

        // save the socket private data
        poller->data[fd] = priv;
    }
}
tb_void_t tb_poller_sock_data_remove(tb_poller_t* poller, tb_socket_ref_t sock)
{
    // check
    tb_long_t fd = tb_sock2fd(sock);
    tb_assert(poller && poller->data);
    tb_assert(fd > 0 && fd < TB_MAXS32);

    // remove the socket private data
    if (fd < poller->maxn) poller->data[fd] = tb_null;
}
