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
 * @file        iocp_object.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "iocp_object.h"
#include "../../libc/libc.h"
#include "../impl/sockdata.h"
#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
#   include "../../coroutine/coroutine.h"
#   include "../../coroutine/impl/impl.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_iocp_object_clear(tb_iocp_object_ref_t object)
{
    // clear object code and state
    object->code  = TB_IOCP_OBJECT_CODE_NONE;
    object->state = TB_IOCP_OBJECT_STATE_NONE;
}
static __tb_inline__ tb_sockdata_ref_t tb_iocp_object_sockdata()
{
    // we only enable iocp in coroutine
#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
#   ifdef TB_CONFIG_MICRO_ENABLE
    return (tb_lo_scheduler_self_())? tb_sockdata() : tb_null;
#   else
    return (tb_co_scheduler_self() || tb_lo_scheduler_self_())? tb_sockdata() : tb_null;
#   endif
#endif
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iocp_object_ref_t tb_iocp_object_get_or_new(tb_socket_ref_t sock)
{
    // check
    tb_assert_and_check_return_val(sock, tb_null);

    // get or new object 
    tb_iocp_object_ref_t object = tb_null;
    do
    { 
        // get the local socket data
        tb_sockdata_ref_t sockdata = tb_iocp_object_sockdata();
        tb_check_break(sockdata);

        // attempt to get object first if exists
        object = (tb_iocp_object_ref_t)tb_sockdata_get(sockdata, sock);

        // new an object if not exists
        if (!object) 
        {
            // make object
            object = tb_malloc0_type(tb_iocp_object_t);
            tb_assert_and_check_break(object);

            // save socket
            object->sock = sock;

            // save object
            tb_sockdata_insert(sockdata, sock, (tb_cpointer_t)object);
        }

    } while (0);

    // done
    return object;
}
tb_iocp_object_ref_t tb_iocp_object_get(tb_socket_ref_t sock)
{
    tb_sockdata_ref_t sockdata = tb_iocp_object_sockdata();
    return sockdata? (tb_iocp_object_ref_t)tb_sockdata_get(sockdata, sock) : tb_null;
}
tb_void_t tb_iocp_object_remove(tb_socket_ref_t sock)
{
    // get the local socket data
    tb_sockdata_ref_t sockdata = tb_iocp_object_sockdata();
    tb_check_return(sockdata);

    // get iocp object
    tb_iocp_object_ref_t object = (tb_iocp_object_ref_t)tb_sockdata_get(sockdata, sock);
    if (object)
    {
        // remove this object from the local socket data
        tb_sockdata_remove(sockdata, sock);

        // clear and free the object data
        tb_iocp_object_clear(object);

        // free object
        tb_free(object);
    }
}
tb_long_t tb_iocp_object_connect(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(object && addr, -1);

    // attempt to get the connection result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_CONN && object->state == TB_IOCP_OBJECT_STATE_FINISHED)
    {
        // @note conn.addr and conn.result cannot be cleared
        tb_iocp_object_clear(object);
        if (tb_ipaddr_is_equal(&object->u.conn.addr, addr))
            return object->u.conn.result;
    }

    // clear the previous object data first
    tb_iocp_object_clear(object);

    // post a connection event to wait it
    object->code          = TB_IOCP_OBJECT_CODE_CONN;
    object->state         = TB_IOCP_OBJECT_STATE_PENDING;
    object->u.conn.addr   = *addr;
    object->u.conn.result = 0;
    return 0;
}
