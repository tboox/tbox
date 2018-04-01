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
 * trace
 */
#define TB_TRACE_MODULE_NAME            "iocp_object"
#define TB_TRACE_MODULE_DEBUG           (1)

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
    // free the private buffer for iocp
    if (object->buffer)
    {
        tb_free(object->buffer);
        object->buffer = tb_null;
    }

    // clear object code and state
    object->code  = TB_IOCP_OBJECT_CODE_NONE;
    object->state = TB_STATE_OK;
}
static __tb_inline__ tb_sockdata_ref_t tb_iocp_object_sockdata()
{
    // we only enable iocp in coroutine
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) && !defined(TB_CONFIG_MICRO_ENABLE)
    return (tb_co_scheduler_self() || tb_lo_scheduler_self_())? tb_sockdata() : tb_null;
#else
    return tb_null;
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

            // init object
            tb_iocp_object_clear(object);

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
        // trace
        tb_trace_d("sock(%p): removed, state: %s", sock, tb_state_cstr(object->state));

        // check state
        if (object->state == TB_STATE_WAITING)
        {
            // trace
            tb_trace_d("sock(%p): cancel io ..", sock);

            // cancel io
            if (!CancelIo((HANDLE)tb_sock2fd(sock)))
            {
                // trace
                tb_trace_e("sock(%p): cancel io failed(%d)!", sock, GetLastError());
            }
            object->state = TB_STATE_OK;
        }
        tb_assert(object->state == TB_STATE_OK || object->state == TB_STATE_FINISHED);

        // remove this object from the local socket data
        tb_sockdata_remove(sockdata, sock);

        // clear and free the object data
        tb_iocp_object_clear(object);

        // free object
        tb_free(object);
    }
}
tb_socket_ref_t tb_iocp_object_accept(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(object, tb_null);

    // always be accept, need not clear object each time
    tb_assert(object->code == TB_IOCP_OBJECT_CODE_NONE || object->code == TB_IOCP_OBJECT_CODE_ACPT);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_ACPT && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("accept(%p): state: %s, result: %p", object->sock, tb_state_cstr(object->state), object->u.acpt.result);

        // get result
        object->state = TB_STATE_OK;
        if (addr) tb_ipaddr_copy(addr, &object->u.acpt.addr);
        return object->u.acpt.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, tb_null);

    // trace
    tb_trace_d("accept(%p): pending ..", object->sock);

    // post a accept event to wait it
    object->code          = TB_IOCP_OBJECT_CODE_ACPT;
    object->state         = TB_STATE_PENDING;
    object->u.acpt.result = tb_null;
    return 0;
}
tb_long_t tb_iocp_object_connect(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(object && addr, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_CONN && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("connect(%p): %{ipaddr}, state: %s, result: %ld", object->sock, addr, tb_state_cstr(object->state), object->u.conn.result);

        /* clear the previous object data first
         *
         * @note conn.addr and conn.result cannot be cleared
         */
        tb_iocp_object_clear(object);
        if (tb_ipaddr_is_equal(&object->u.conn.addr, addr))
            return object->u.conn.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("connect(%p, %{ipaddr}): pending ..", object->sock, addr);

    // post a connection event to wait it
    object->code          = TB_IOCP_OBJECT_CODE_CONN;
    object->state         = TB_STATE_PENDING;
    object->u.conn.addr   = *addr;
    object->u.conn.result = 0;
    return 0;
}
tb_long_t tb_iocp_object_recv(tb_iocp_object_ref_t object, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && data && size, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_RECV && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("recv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.recv.result);

        // clear the previous object data first, but the result cannot be cleared
        tb_iocp_object_clear(object);
        return object->u.recv.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("recv(%p, %lu): pending ..", object->sock, size);

    // post a recv event to wait it
    object->code        = TB_IOCP_OBJECT_CODE_RECV;
    object->state       = TB_STATE_PENDING;
    object->u.recv.data = data;
    object->u.recv.size = (tb_iovec_size_t)size;
    return 0;
}
tb_long_t tb_iocp_object_send(tb_iocp_object_ref_t object, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && data, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_SEND && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("send(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.send.result);

        // clear the previous object data first, but the result cannot be cleared
        tb_iocp_object_clear(object);
        return object->u.send.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("send(%p, %lu): pending ..", object->sock, size);

    // post a send event to wait it
    object->code        = TB_IOCP_OBJECT_CODE_SEND;
    object->state       = TB_STATE_PENDING;
    object->u.send.data = data;
    object->u.send.size = (tb_iovec_size_t)size;
    return 0;
}
tb_long_t tb_iocp_object_urecv(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && data && size, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_URECV && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("urecv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.urecv.result);

        // clear the previous object data first, but the result cannot be cleared
        tb_iocp_object_clear(object);
        if (addr) tb_ipaddr_copy(addr, &object->u.urecv.addr);
        return object->u.urecv.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("urecv(%p, %lu): pending ..", object->sock, size);

    // post a urecv event to wait it
    object->code         = TB_IOCP_OBJECT_CODE_URECV;
    object->state        = TB_STATE_PENDING;
    object->u.urecv.data = data;
    object->u.urecv.size = (tb_iovec_size_t)size;
    return 0;
}
tb_long_t tb_iocp_object_usend(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && addr && data, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_USEND && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("usend(%p, %{ipaddr}): state: %s, result: %ld", object->sock, addr, tb_state_cstr(object->state), object->u.usend.result);

        // clear the previous object data first, but the result cannot be cleared
        tb_iocp_object_clear(object);
        return object->u.usend.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("usend(%p, %{ipaddr}, %lu): pending ..", object->sock, addr, size);

    // post a usend event to wait it
    object->code         = TB_IOCP_OBJECT_CODE_USEND;
    object->state        = TB_STATE_PENDING;
    object->u.usend.addr = *addr;
    object->u.usend.data = data;
    object->u.usend.size = (tb_iovec_size_t)size;
    return 0;
}
tb_hong_t tb_iocp_object_sendf(tb_iocp_object_ref_t object, tb_file_ref_t file, tb_hize_t offset, tb_hize_t size)
{
    // check
    tb_assert_and_check_return_val(object && file, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_SENDF && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("sendfile(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.sendf.result);

        // clear the previous object data first, but the result cannot be cleared
        tb_iocp_object_clear(object);
        return object->u.sendf.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("sendfile(%p, %llu at %llu): pending ..", object->sock, size, offset);

    // post a send event to wait it
    object->code           = TB_IOCP_OBJECT_CODE_SENDF;
    object->state          = TB_STATE_PENDING;
    object->u.sendf.file   = file;
    object->u.sendf.size   = size;
    object->u.sendf.offset = offset;
    return 0;
}
#ifndef TB_CONFIG_MICRO_ENABLE
tb_long_t tb_iocp_object_recvv(tb_iocp_object_ref_t object, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && list && size, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_RECVV && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("recvv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.recvv.result);

        // clear the previous object data first, but the result cannot be cleared
        tb_iocp_object_clear(object);
        return object->u.recvv.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("recvv(%p, %lu): pending ..", object->sock, size);

    // post a recvv event to wait it
    object->code         = TB_IOCP_OBJECT_CODE_RECVV;
    object->state        = TB_STATE_PENDING;
    object->u.recvv.list = list;
    object->u.recvv.size = (tb_iovec_size_t)size;
    return 0;
}
tb_long_t tb_iocp_object_sendv(tb_iocp_object_ref_t object, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && list && size, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_SENDV && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("sendv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.sendv.result);

        // clear the previous object data first, but the result cannot be cleared
        tb_iocp_object_clear(object);
        return object->u.sendv.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("sendv(%p, %lu): pending ..", object->sock, size);

    // post a sendv event to wait it
    object->code         = TB_IOCP_OBJECT_CODE_SENDV;
    object->state        = TB_STATE_PENDING;
    object->u.sendv.list = list;
    object->u.sendv.size = (tb_iovec_size_t)size;
    return 0;
}
tb_long_t tb_iocp_object_urecvv(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && list && size, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_URECVV && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("urecvv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.urecvv.result);

        // clear the previous object data first, but the result cannot be cleared
        tb_iocp_object_clear(object);
        if (addr) tb_ipaddr_copy(addr, &object->u.urecvv.addr);
        return object->u.urecvv.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("urecvv(%p, %lu): pending ..", object->sock, size);

    // post a urecvv event to wait it
    object->code          = TB_IOCP_OBJECT_CODE_URECVV;
    object->state         = TB_STATE_PENDING;
    object->u.urecvv.list = list;
    object->u.urecvv.size = (tb_iovec_size_t)size;
    return 0;
}
tb_long_t tb_iocp_object_usendv(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && addr && list, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_USENDV && object->state == TB_STATE_FINISHED)
    {
        // trace
        tb_trace_d("usendv(%p, %{ipaddr}): state: %s, result: %ld", object->sock, addr, tb_state_cstr(object->state), object->u.usendv.result);

        // clear the previous object data first, but the result cannot be cleared
        tb_iocp_object_clear(object);
        return object->u.usendv.result;
    }

    // check state
    tb_assert_and_check_return_val(object->state == TB_STATE_OK, -1);

    // trace
    tb_trace_d("usendv(%p, %{ipaddr}, %lu): pending ..", object->sock, addr, size);

    // post a usendv event to wait it
    object->code          = TB_IOCP_OBJECT_CODE_USENDV;
    object->state         = TB_STATE_PENDING;
    object->u.usendv.addr = *addr;
    object->u.usendv.list = list;
    object->u.usendv.size = (tb_iovec_size_t)size;
    return 0;
}
#endif
