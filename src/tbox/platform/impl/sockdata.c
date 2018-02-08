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
 * @file        sockdata.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sockdata.h"
#include "../../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_sockdata_init(tb_sockdata_ref_t sockdata)
{
    // check
    tb_assert(sockdata);

    // init it
    sockdata->data = tb_null;
    sockdata->maxn = 0;
}
tb_void_t tb_sockdata_exit(tb_sockdata_ref_t sockdata)
{
    // check
    tb_assert(sockdata);

    // exit socket data
    if (sockdata->data) tb_free(sockdata->data);
    sockdata->data = tb_null;
    sockdata->maxn = 0;
}
tb_void_t tb_sockdata_clear(tb_sockdata_ref_t sockdata)
{
    // check
    tb_assert(sockdata);

    // clear data
    if (sockdata->data) tb_memset(sockdata->data, 0, sockdata->maxn * sizeof(tb_cpointer_t));
}
tb_void_t tb_sockdata_insert(tb_sockdata_ref_t sockdata, tb_socket_ref_t sock, tb_cpointer_t priv)
{
    // check
    tb_long_t fd = tb_sock2fd(sock);
    tb_assert(sockdata && fd > 0 && fd < TB_MAXS32);

    // not null?
    if (priv)
    {
        // no data? init it first
        tb_size_t need = fd + 1;
        if (!sockdata->data)
        {
            // init data
            sockdata->data = tb_nalloc0_type(need, tb_cpointer_t);
            tb_assert_and_check_return(sockdata->data);

            // init data size
            sockdata->maxn = need;
        }
        else if (need > sockdata->maxn)
        {
            // grow data
            sockdata->data = (tb_cpointer_t*)tb_ralloc(sockdata->data, need * sizeof(tb_cpointer_t));
            tb_assert_and_check_return(sockdata->data);

            // init growed space
            tb_memset(sockdata->data + sockdata->maxn, 0, (need - sockdata->maxn) * sizeof(tb_cpointer_t));

            // grow data size
            sockdata->maxn = need;
        }

        // save the socket private data
        sockdata->data[fd] = priv;
    }
}
tb_void_t tb_sockdata_remove(tb_sockdata_ref_t sockdata, tb_socket_ref_t sock)
{
    // check
    tb_long_t fd = tb_sock2fd(sock);
    tb_assert(sockdata && sockdata->data);
    tb_assert(fd > 0 && fd < TB_MAXS32);

    // remove the socket private data
    if (fd < sockdata->maxn) sockdata->data[fd] = tb_null;
}
