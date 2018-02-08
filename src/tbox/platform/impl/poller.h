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
 * @file        poller.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_IMPL_POLLER_H
#define TB_PLATFORM_IMPL_POLLER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "socket.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the poller type
typedef struct __tb_poller_t
{
    // the socket data (sock => priv)
    tb_cpointer_t*          data;

    // the socket data maximum count
    tb_size_t               maxn;
    
}tb_poller_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init socket data for poller
 *
 * @param poller    the poller
 */
tb_void_t           tb_poller_sock_data_init(tb_poller_t* poller);

/* exit socket data in poller
 *
 * @param poller    the poller
 */
tb_void_t           tb_poller_sock_data_exit(tb_poller_t* poller);

/* clear socket data in poller
 *
 * @param poller    the poller
 */
tb_void_t           tb_poller_sock_data_clear(tb_poller_t* poller);

/* insert socket data to poller
 *
 * @param poller    the poller
 * @param sock      the socket
 * @param priv      the socket private data
 */
tb_void_t           tb_poller_sock_data_insert(tb_poller_t* poller, tb_socket_ref_t sock, tb_cpointer_t priv);

/* remove socket data from poller
 *
 * @param poller    the poller
 * @param sock      the socket
 */
tb_void_t           tb_poller_sock_data_remove(tb_poller_t* poller, tb_socket_ref_t sock);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
static __tb_inline__ tb_cpointer_t tb_poller_sock_data(tb_poller_t* poller, tb_socket_ref_t sock)
{
    // check
    tb_long_t fd = tb_sock2fd(sock);
    tb_assert(poller && poller->data);
    tb_assert(fd > 0 && fd < TB_MAXS32);

    // get the socket private data
    return fd < poller->maxn? poller->data[fd] : tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
