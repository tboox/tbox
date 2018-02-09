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
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        iocp_event.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "iocp_event.h"
#include "../../libc/libc.h"
#include "../impl/sockdata.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_iocp_event_clear(tb_iocp_event_ref_t event)
{
    // TODO
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iocp_event_ref_t tb_iocp_event_init(tb_socket_ref_t sock)
{
    // check
    tb_assert_and_check_return_val(sock, tb_null);

    // get or new event 
    tb_bool_t ok = tb_false;
    tb_iocp_event_ref_t event = tb_null;
    do
    { 
        // get the local socket data
        tb_sockdata_ref_t sockdata = tb_sockdata();
        tb_assert_and_check_break(sockdata);

        // attempt to get event first if exists
        event = (tb_iocp_event_ref_t)tb_sockdata_get(sockdata, sock);

        // clear the previous data if exists
        if (event) tb_iocp_event_clear(event);
        else
        {
            // make event
            event = tb_malloc0_type(tb_iocp_event_t);
            tb_assert_and_check_break(event);

            // save event
            tb_sockdata_insert(sockdata, sock, (tb_cpointer_t)event);
        }

        // ok
        ok = tb_true;

    } while (0);

    // done
    return event;
}
tb_void_t tb_iocp_event_exit(tb_socket_ref_t sock)
{
    tb_iocp_event_ref_t event = tb_iocp_event_get(sock);
    if (event)
    {
        // remove this event from the local socket data
        tb_sockdata_remove(tb_sockdata(), sock);

        // clear and free the event data
        tb_iocp_event_clear(event);

        // free event
        tb_free(event);
    }
}
tb_iocp_event_ref_t tb_iocp_event_get(tb_socket_ref_t sock)
{
    return (tb_iocp_event_ref_t)tb_sockdata_get(tb_sockdata(), sock);
}
