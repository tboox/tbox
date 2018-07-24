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
 * @file        poller_iocp.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "iocp_object.h"
#include "../posix/sockaddr.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#include "interface/interface.h"
#include "ntstatus.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the iocp func type
typedef struct __tb_iocp_func_t
{
    // the AcceptEx func
    tb_mswsock_AcceptEx_t                       AcceptEx;

    // the ConnectEx func
    tb_mswsock_ConnectEx_t                      ConnectEx;

    // the DisconnectEx func
    tb_mswsock_DisconnectEx_t                   DisconnectEx;

    // the TransmitFile func
    tb_mswsock_TransmitFile_t                   TransmitFile;

    // the GetAcceptExSockaddrs func
    tb_mswsock_GetAcceptExSockaddrs_t           GetAcceptExSockaddrs;

    // the GetQueuedCompletionStatusEx func
    tb_kernel32_GetQueuedCompletionStatusEx_t   GetQueuedCompletionStatusEx;
 
    // WSAGetLastError
    tb_ws2_32_WSAGetLastError_t                 WSAGetLastError;

    // WSASend
    tb_ws2_32_WSASend_t                         WSASend;

    // WSARecv
    tb_ws2_32_WSARecv_t                         WSARecv;

    // WSASendTo
    tb_ws2_32_WSASendTo_t                       WSASendTo;

    // WSARecvFrom
    tb_ws2_32_WSARecvFrom_t                     WSARecvFrom;

    // bind
    tb_ws2_32_bind_t                            bind;

}tb_iocp_func_t;

// the poller iocp type
typedef struct __tb_poller_iocp_t
{
    // the maxn
    tb_size_t               maxn;

    // the user private data
    tb_cpointer_t           priv;

    // the iocp func
    tb_iocp_func_t          func;

    // the i/o completion port
    HANDLE                  port;

    // the events
    tb_OVERLAPPED_ENTRY_t*  events;

    // the events count
    tb_size_t               events_count;

}tb_poller_iocp_t, *tb_poller_iocp_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_bool_t tb_poller_iocp_event_post_acpt(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_RECV, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post accept(%p) event: ..", sock);

    // post a connection event 
    tb_bool_t ok = tb_false;
    tb_bool_t init_ok = tb_false;
    tb_bool_t AcceptEx_ok = tb_false;
    do
    {
        // init olap
        tb_memset(&object->olap, 0, sizeof(OVERLAPPED));

        // make address buffer
        if (!object->buffer) object->buffer = tb_malloc0(((sizeof(struct sockaddr_storage)) << 1));
        tb_assert_and_check_break(object->buffer);

        // get bound address family
        struct sockaddr_storage bound_addr;
        socklen_t len = sizeof(bound_addr);
        tb_size_t family = TB_IPADDR_FAMILY_IPV4;
        if (getsockname((SOCKET)tb_sock2fd(sock), (struct sockaddr *)&bound_addr, &len) != -1 && bound_addr.ss_family == AF_INET6)
            family = TB_IPADDR_FAMILY_IPV6;

        // make accept socket
        object->u.acpt.result = tb_socket_init(TB_SOCKET_TYPE_TCP, family);
        tb_assert_and_check_break(object->u.acpt.result);
        init_ok = tb_true;

        /* do AcceptEx
         *
         * @note this socket have been bound to local address in tb_socket_connect()
         */
        DWORD real = 0;
        AcceptEx_ok = poller->func.AcceptEx(    (SOCKET)tb_sock2fd(sock)
                                            ,   (SOCKET)tb_sock2fd(object->u.acpt.result)
                                            ,   (tb_byte_t*)object->buffer
                                            ,   0
                                            ,   sizeof(struct sockaddr_storage)
                                            ,   sizeof(struct sockaddr_storage)
                                            ,   &real
                                            ,   (LPOVERLAPPED)&object->olap)? tb_true : tb_false;

        // trace
        tb_trace_d("accepting[%p]: AcceptEx: %d, lasterror: %d", sock, AcceptEx_ok, poller->func.WSAGetLastError());
        tb_check_break(AcceptEx_ok);

        // accepted? finished
        object->state = TB_STATE_FINISHED;
        if (!PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap)) break;

        // ok
        ok = tb_true;

    } while (0);

    // AcceptEx failed?
    if (!ok)
    {
        // pending? continue it
        if (init_ok && WSA_IO_PENDING == poller->func.WSAGetLastError()) 
        {
            ok = tb_true;
            object->state = TB_STATE_WAITING;
        }
        // failed?
        else
        {
            // free result socket
            if (object->u.acpt.result) tb_socket_exit(object->u.acpt.result);
            object->u.acpt.result = tb_null;

            // finished
            object->state = TB_STATE_FINISHED;
            if (PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap)) ok = tb_true;
        }
    }

    // ok?
    return ok;
}
static tb_bool_t tb_poller_iocp_event_post_conn(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_SEND, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post connect(%p, %{ipaddr}) event: ..", sock, &object->u.conn.addr);

    // post a connection event 
    tb_bool_t ok = tb_false;
    tb_bool_t init_ok = tb_false;
    tb_bool_t ConnectEx_ok = tb_false;
    do
    {
        // init olap
        tb_memset(&object->olap, 0, sizeof(OVERLAPPED));

        // load client address
        tb_size_t               caddr_size = 0;
        struct sockaddr_storage caddr_data = {0};
        if (!(caddr_size = tb_sockaddr_load(&caddr_data, &object->u.conn.addr))) break;

        // load local address
        tb_size_t               laddr_size = 0;
        struct sockaddr_storage laddr_data = {0};
        tb_ipaddr_t             laddr;
        if (!tb_ipaddr_set(&laddr, tb_null, 0, (tb_uint8_t)tb_ipaddr_family(&object->u.conn.addr))) break;
        if (!(laddr_size = tb_sockaddr_load(&laddr_data, &laddr))) break;

        // bind it first for ConnectEx
        if (SOCKET_ERROR == poller->func.bind((SOCKET)tb_sock2fd(sock), (LPSOCKADDR)&laddr_data, (tb_int_t)laddr_size)) 
        {
            // trace
            tb_trace_e("connect[%p]: bind failed, error: %u", sock, GetLastError());
            break;
        }
        init_ok = tb_true;

        /* do ConnectEx
         *
         * @note this socket have been bound to local address in tb_socket_connect()
         */
        DWORD real = 0;
        ConnectEx_ok = poller->func.ConnectEx(  (SOCKET)tb_sock2fd(sock)
                                            ,   (struct sockaddr const*)&caddr_data
                                            ,   (tb_int_t)caddr_size
                                            ,   tb_null
                                            ,   0
                                            ,   &real
                                            ,   (LPOVERLAPPED)&object->olap)? tb_true : tb_false;

        // trace
        tb_trace_d("connecting[%p]: ConnectEx: %d, lasterror: %d", sock, ConnectEx_ok, poller->func.WSAGetLastError());
        tb_check_break(ConnectEx_ok);

        // connected, post result directly
        object->state = TB_STATE_FINISHED;
        object->u.conn.result = 1;
        if (!PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap)) break;

        // ok
        ok = tb_true;

    } while (0);

    // ConnectEx failed?
    if (!ok)
    {
        // pending? continue it
        if (init_ok && WSA_IO_PENDING == poller->func.WSAGetLastError()) 
        {
            ok = tb_true;
            object->state = TB_STATE_WAITING;
        }
        // already connected or failed?
        else
        {
            object->state = TB_STATE_FINISHED;
            object->u.conn.result = poller->func.WSAGetLastError() == WSAEISCONN? 1 : -1;
            if (PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap)) ok = tb_true;
        }
    }

    // ok?
    return ok;
}
static tb_bool_t tb_poller_iocp_event_post_recv(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_RECV, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post recv(%p, %lu) event: ..", sock, object->u.recv.size);

    // do recv
    DWORD flag = 0;
    tb_long_t ok = poller->func.WSARecv((SOCKET)tb_sock2fd(sock), (WSABUF*)&object->u.recv, 1, tb_null, &flag, (LPOVERLAPPED)&object->olap, tb_null);

    // trace
    tb_trace_d("recving[%p]: WSARecv: %ld, lasterror: %d", sock, ok, poller->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == poller->func.WSAGetLastError()))) 
    {
        object->state = TB_STATE_WAITING;
        return tb_true;
    }

    // error? finished
    object->state = TB_STATE_FINISHED;
    object->u.recv.result = -1;
    return PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post_send(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_SEND, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post send(%p, %lu) event: ..", sock, object->u.send.size);

    // do send
    tb_long_t ok = poller->func.WSASend((SOCKET)tb_sock2fd(sock), (WSABUF*)&object->u.send, 1, tb_null, 0, (LPOVERLAPPED)&object->olap, tb_null);

    // trace
    tb_trace_d("sending[%p]: WSASend: %ld, lasterror: %d", sock, ok, poller->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == poller->func.WSAGetLastError()))) 
    {
        object->state = TB_STATE_WAITING;
        return tb_true;
    }

    // error? finished
    object->state = TB_STATE_FINISHED;
    object->u.send.result = -1;
    return PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post_urecv(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_RECV, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post urecv(%p, %lu) event: ..", sock, object->u.urecv.size);

    // make buffer for address, size and flags
    if (!object->buffer) object->buffer = tb_malloc0(sizeof(struct sockaddr_storage) + sizeof(tb_int_t) + sizeof(DWORD));
    tb_assert_and_check_return_val(object->buffer, tb_false);

    // init size
    tb_int_t* psize = (tb_int_t*)((tb_byte_t*)object->buffer + sizeof(struct sockaddr_storage));
    *psize = sizeof(struct sockaddr_storage);

    // init flag
    DWORD* pflag = (DWORD*)((tb_byte_t*)object->buffer + sizeof(struct sockaddr_storage) + sizeof(tb_int_t));
    *pflag = 0;

    // do urecv
    tb_long_t ok = poller->func.WSARecvFrom((SOCKET)tb_sock2fd(sock), (WSABUF*)&object->u.urecv, 1, tb_null, pflag, (struct sockaddr*)object->buffer, psize, (LPOVERLAPPED)&object->olap, tb_null);

    // trace
    tb_trace_d("urecving[%p]: WSARecvFrom: %ld, lasterror: %d", sock, ok, poller->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == poller->func.WSAGetLastError()))) 
    {
        object->state = TB_STATE_WAITING;
        return tb_true;
    }

    // error? finished
    object->state = TB_STATE_FINISHED;
    object->u.urecv.result = -1;
    return PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post_usend(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_SEND, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post usend(%p, %{ipaddr}, %lu) event: ..", sock, &object->u.usend.addr, object->u.usend.size);

    // load address
    tb_size_t               n = 0;
	struct sockaddr_storage d = {0};
    if (!(n = tb_sockaddr_load(&d, &object->u.usend.addr))) return tb_false;

    // do usend
    tb_long_t ok = poller->func.WSASendTo((SOCKET)tb_sock2fd(sock), (WSABUF*)&object->u.usend, 1, tb_null, 0, (struct sockaddr*)&d, (tb_int_t)n, (LPOVERLAPPED)&object->olap, tb_null);

    // trace
    tb_trace_d("usending[%p]: WSASendTo: %ld, lasterror: %d", sock, ok, poller->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == poller->func.WSAGetLastError()))) 
    {
        object->state = TB_STATE_WAITING;
        return tb_true;
    }

    // error? finished
    object->state = TB_STATE_FINISHED;
    object->u.usend.result = -1;
    return PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post_sendf(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_SEND, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);
    tb_assert_and_check_return_val(poller->func.TransmitFile, tb_false);

    // trace
    tb_trace_d("post sendfile(%p, %lu) event: ..", sock, object->u.sendf.size);

    // do send file
    object->olap.Offset = (DWORD)object->u.sendf.offset;
    object->olap.OffsetHigh = (DWORD)(object->u.sendf.offset >> 32);
    BOOL ok = poller->func.TransmitFile((SOCKET)tb_sock2fd(sock), (HANDLE)object->u.sendf.file, (DWORD)object->u.sendf.size, (1 << 16), (LPOVERLAPPED)&object->olap, tb_null, 0);
    tb_trace_d("sending[%p]: TransmitFile: %d, lasterror: %d", sock, ok, poller->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == poller->func.WSAGetLastError()))) 
    {
        object->state = TB_STATE_WAITING;
        return tb_true;
    }

    // error? finished
    object->state = TB_STATE_FINISHED;
    object->u.sendf.result = -1;
    return PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post_recvv(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_RECV, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post recvv(%p, %lu) event: ..", sock, object->u.recvv.size);

    // do recv
    DWORD flag = 0;
    tb_long_t ok = poller->func.WSARecv((SOCKET)tb_sock2fd(sock), (WSABUF*)object->u.recvv.list, (DWORD)object->u.recvv.size, tb_null, &flag, (LPOVERLAPPED)&object->olap, tb_null);

    // trace
    tb_trace_d("recving[%p]: WSARecv: %ld, lasterror: %d", sock, ok, poller->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == poller->func.WSAGetLastError()))) 
    {
        object->state = TB_STATE_WAITING;
        return tb_true;
    }

    // error? finished
    object->state = TB_STATE_FINISHED;
    object->u.recvv.result = -1;
    return PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post_sendv(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_SEND, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post sendv(%p, %lu) event: ..", sock, object->u.sendv.size);

    // do send
    tb_long_t ok = poller->func.WSASend((SOCKET)tb_sock2fd(sock), (WSABUF*)object->u.sendv.list, (DWORD)object->u.sendv.size, tb_null, 0, (LPOVERLAPPED)&object->olap, tb_null);

    // trace
    tb_trace_d("sending[%p]: WSASend: %ld, lasterror: %d", sock, ok, poller->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == poller->func.WSAGetLastError()))) 
    {
        object->state = TB_STATE_WAITING;
        return tb_true;
    }

    // error? finished
    object->state = TB_STATE_FINISHED;
    object->u.sendv.result = -1;
    return PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post_urecvv(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_RECV, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post urecvv(%p, %lu) event: ..", sock, object->u.urecvv.size);

    // make buffer for address, size and flags
    if (!object->buffer) object->buffer = tb_malloc0(sizeof(struct sockaddr_storage) + sizeof(tb_int_t) + sizeof(DWORD));
    tb_assert_and_check_return_val(object->buffer, tb_false);

    // init size
    tb_int_t* psize = (tb_int_t*)((tb_byte_t*)object->buffer + sizeof(struct sockaddr_storage));
    *psize = sizeof(struct sockaddr_storage);

    // init flag
    DWORD* pflag = (DWORD*)((tb_byte_t*)object->buffer + sizeof(struct sockaddr_storage) + sizeof(tb_int_t));
    *pflag = 0;

    // do urecvv
    tb_long_t ok = poller->func.WSARecvFrom((SOCKET)tb_sock2fd(sock), (WSABUF*)object->u.urecvv.list, (DWORD)object->u.urecvv.size, tb_null, pflag, (struct sockaddr*)object->buffer, psize, (LPOVERLAPPED)&object->olap, tb_null);

    // trace
    tb_trace_d("urecving[%p]: WSARecvFrom: %ld, lasterror: %d", sock, ok, poller->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == poller->func.WSAGetLastError()))) 
    {
        object->state = TB_STATE_WAITING;
        return tb_true;
    }

    // error? finished
    object->state = TB_STATE_FINISHED;
    object->u.urecvv.result = -1;
    return PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post_usendv(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_SEND, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post usendv(%p, %{ipaddr}, %lu) event: ..", sock, &object->u.usendv.addr, object->u.usendv.size);

    // load addr
    tb_size_t               n = 0;
	struct sockaddr_storage d = {0};
    if (!(n = tb_sockaddr_load(&d, &object->u.usendv.addr))) return tb_false;

    // do usendv
    tb_long_t ok = poller->func.WSASendTo((SOCKET)tb_sock2fd(sock), (WSABUF*)object->u.usendv.list, (DWORD)object->u.usendv.size, tb_null, 0, (struct sockaddr*)&d, (tb_int_t)n, (LPOVERLAPPED)&object->olap, tb_null);

    // trace
    tb_trace_d("usending[%p]: WSASendTo: %ld, lasterror: %d", sock, ok, poller->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == poller->func.WSAGetLastError()))) 
    {
        object->state = TB_STATE_WAITING;
        return tb_true;
    }

    // error? finished
    object->state = TB_STATE_FINISHED;
    object->u.usendv.result = -1;
    return PostQueuedCompletionStatus(poller->port, 0, tb_null, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events, tb_false);

    // no pending event or waiting? return it directly
    if (object->state == TB_STATE_OK || object->state == TB_STATE_WAITING)
        return tb_true;

    // trace
    tb_trace_d("post events(%lx) for socket(%p), code: %u, state: %s ..", events, sock, object->code, tb_state_cstr(object->state));

    // init post
    static tb_bool_t (*s_post[])(tb_poller_iocp_ref_t , tb_socket_ref_t, tb_iocp_object_ref_t, tb_size_t ) = 
    {
        tb_null
    ,   tb_poller_iocp_event_post_acpt 
    ,   tb_poller_iocp_event_post_conn
    ,   tb_poller_iocp_event_post_recv
    ,   tb_poller_iocp_event_post_send
    ,   tb_poller_iocp_event_post_urecv 
    ,   tb_poller_iocp_event_post_usend 
    ,   tb_poller_iocp_event_post_recvv
    ,   tb_poller_iocp_event_post_sendv
    ,   tb_poller_iocp_event_post_urecvv
    ,   tb_poller_iocp_event_post_usendv
    ,   tb_poller_iocp_event_post_sendf 
    };
    tb_assert_and_check_return_val(object->code < tb_arrayn(s_post), tb_false);

    // trace
    tb_trace_d("post[%p]: code %u ..", object->sock, object->code);

    // post event
    return (s_post[object->code])? s_post[object->code](poller, sock, object, events) : tb_false;
}
static tb_size_t tb_poller_iocp_event_from_code(tb_size_t code)
{
    static tb_size_t s_events[] = 
    {
        TB_POLLER_EVENT_CONN
    ,   TB_POLLER_EVENT_ACPT
    ,   TB_POLLER_EVENT_CONN
    ,   TB_POLLER_EVENT_RECV
    ,   TB_POLLER_EVENT_SEND
    ,   TB_POLLER_EVENT_RECV
    ,   TB_POLLER_EVENT_SEND
    ,   TB_POLLER_EVENT_RECV
    ,   TB_POLLER_EVENT_SEND
    ,   TB_POLLER_EVENT_RECV
    ,   TB_POLLER_EVENT_SEND
    ,   TB_POLLER_EVENT_SEND
    };
    tb_assert_and_check_return_val(code < tb_arrayn(s_events), TB_POLLER_EVENT_NONE);
    return s_events[code];
}
static tb_long_t tb_poller_iocp_event_spak_conn(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t object, tb_size_t real, tb_size_t error)
{
    // have been finished?
    tb_check_return_val(object->state != TB_STATE_FINISHED, 1);

    // done
    switch (error)
    {
        // ok?
    case ERROR_SUCCESS:
        object->u.conn.result = 1;
        break;
        // pending?
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        object->u.conn.result = 0;
        break;
       // timeout?
    case WSAEINTR:
    case ERROR_SEM_TIMEOUT:
    case ERROR_OPERATION_ABORTED:
        object->u.conn.result = 0;
        break;
        // failed?
    case WSAENOTCONN:
    case WSAECONNREFUSED:
    case ERROR_CONNECTION_REFUSED:
        object->u.conn.result = -1;
        break;
        // unknown error
    default:
        // trace
        tb_trace_e("connect(%p): %{ipaddr}, unknown error: %u", object->sock, &object->u.conn.addr, error);
        object->u.conn.result = -1;
        break;
    }

    // trace
    tb_trace_d("connect(%p) %{ipaddr}, return: %ld", object->sock, &object->u.conn.addr, object->u.conn.result);

    // ok
    return 1;
}
static tb_long_t tb_poller_iocp_event_spak_acpt(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t object, tb_size_t real, tb_size_t error)
{
    // done
    switch (error)
    {
        // ok or pending?
    case ERROR_SUCCESS:
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        {
            // update the accept context, otherwise shutdown and getsockname will be failed
            SOCKET fd = tb_sock2fd(object->u.acpt.result);
            SOCKET acpt = (SOCKET)tb_sock2fd(object->sock);
            tb_ws2_32()->setsockopt(fd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (tb_char_t*)&acpt, sizeof(acpt));

            // non-block
            ULONG nb = 1;
            tb_ws2_32()->ioctlsocket(fd, FIONBIO, &nb);

            /* disable the nagle's algorithm to fix 40ms ack delay in some case (.e.g send-send-40ms-recv)
             *
             * 40ms is the tcp ack delay, which indicates that you are likely 
             * encountering a bad interaction between delayed acks and the nagle's algorithm. 
             *
             * TCP_NODELAY simply disables the nagle's algorithm and is a one-time setting on the socket, 
             * whereas the other two must be set at the appropriate times during the life of the connection 
             * and can therefore be trickier to use.
             * 
             * so we set TCP_NODELAY to reduce response delay for the accepted socket in the server by default
             */
            tb_int_t enable = 1;
            tb_ws2_32()->setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (tb_char_t*)&enable, sizeof(enable));

            // get accept socket addresses
            INT                         server_size = 0;
            INT                         client_size = 0;
            struct sockaddr_storage*    server_addr = tb_null;
            struct sockaddr_storage*    client_addr = tb_null;
            if (poller->func.GetAcceptExSockaddrs)
            {
                // check
                tb_assert(object->buffer);

                // get server and client addresses
                poller->func.GetAcceptExSockaddrs(  (tb_byte_t*)object->buffer
                                                ,   0
                                                ,   sizeof(struct sockaddr_storage)
                                                ,   sizeof(struct sockaddr_storage)
                                                ,   (LPSOCKADDR*)&server_addr
                                                ,   &server_size
                                                ,   (LPSOCKADDR*)&client_addr
                                                ,   &client_size);

                // exists client address?
                if (client_addr)
                {
                    // save address
                    tb_sockaddr_save(&object->u.acpt.addr, client_addr);

                    // trace
                    tb_trace_d("accept(%p): client address: %{ipaddr}", object->sock, &object->u.acpt.addr);
                }
            }
        }
        break;
        // canceled? timeout?
    case WSAEINTR:
    case ERROR_OPERATION_ABORTED:
        if (object->u.acpt.result) tb_socket_exit(object->u.acpt.result);
        object->u.acpt.result = tb_null;
        break;
        // unknown error
    default:
        // trace
        tb_trace_e("accept(%p): unknown error: %u", object->sock, error);
        if (object->u.acpt.result) tb_socket_exit(object->u.acpt.result);
        object->u.acpt.result = tb_null;
        break;
    }

    // ok
    return 1;
}
static tb_long_t tb_poller_iocp_event_spak_iorw(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t object, tb_size_t real, tb_size_t error)
{
    // have been finished?
    tb_check_return_val(object->state != TB_STATE_FINISHED, 1);

    // ok?
    if (real)
    {
        // trace
        tb_trace_d("iorw(%p): code: %u, real: %lu", object->sock, object->code, real);

        // save address for urecv or urecvv
        if (object->code == TB_IOCP_OBJECT_CODE_URECV || object->code == TB_IOCP_OBJECT_CODE_URECVV)
        {
            // the address
            struct sockaddr_storage* addr = (struct sockaddr_storage*)object->buffer;
            tb_assert_and_check_return_val(addr, -1);

            // save address
            tb_sockaddr_save(object->code == TB_IOCP_OBJECT_CODE_URECV? &object->u.urecv.addr : &object->u.urecvv.addr, addr);
        }

        // check struct member offset for hacking the same result offset for the other iocp object
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_send_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_recvv_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_sendv_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_urecv_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_usend_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_urecvv_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_usendv_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_sendf_t, result));


        // save the result size, @note: hack the result offset 
        object->u.recv.result = real;
        return 1;
    }

    // error? 
    switch (error)
    {       
        // connection closed?
    case ERROR_SUCCESS:
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        object->u.recv.result = -1;
        break;
       // canceled? timeout 
    case WSAEINTR:
    case ERROR_OPERATION_ABORTED:
        object->u.recv.result = 0;
        break;
        // connection reset?
    case WSAECONNRESET:
    case ERROR_HANDLE_EOF:
    case ERROR_NETNAME_DELETED:
    case ERROR_BAD_COMMAND:
        object->u.recv.result = -1;
        break;
        // unknown error
    default:
        // trace
        tb_trace_e("iorw(%p): code: %u, unknown error: %lu", object->sock, object->code, error);
        object->u.recv.result = -1;
        break;
    }

    // ok
    return 1;
}
static tb_long_t tb_poller_iocp_event_spak(tb_poller_iocp_ref_t poller, tb_poller_event_func_t func, tb_iocp_object_ref_t object, tb_size_t real, tb_size_t error)
{
    // trace
    tb_trace_d("spak[%p]: code %u, state: %s ..", object->sock, object->code, tb_state_cstr(object->state));

    // check
    tb_assert_and_check_return_val(object->state == TB_STATE_WAITING || object->state == TB_STATE_FINISHED, -1);

    // init spak
    static tb_long_t (*s_spak[])(tb_poller_iocp_ref_t , tb_iocp_object_ref_t, tb_size_t , tb_size_t ) = 
    {
        tb_null
    ,   tb_poller_iocp_event_spak_acpt
    ,   tb_poller_iocp_event_spak_conn
    ,   tb_poller_iocp_event_spak_iorw
    ,   tb_poller_iocp_event_spak_iorw
    ,   tb_poller_iocp_event_spak_iorw
    ,   tb_poller_iocp_event_spak_iorw
    ,   tb_poller_iocp_event_spak_iorw
    ,   tb_poller_iocp_event_spak_iorw
    ,   tb_poller_iocp_event_spak_iorw
    ,   tb_poller_iocp_event_spak_iorw
    ,   tb_poller_iocp_event_spak_iorw
    };
    tb_assert_and_check_return_val(object->code < tb_arrayn(s_spak), -1);

    // spark event
    tb_long_t ok = (s_spak[object->code])? s_spak[object->code](poller, object, real, error) : -1;

    // finish to wait events    
    object->state = TB_STATE_FINISHED;

    // do event handler 
    func((tb_poller_ref_t)poller, object->sock, tb_poller_iocp_event_from_code(object->code), object->priv);

    // ok?
    return ok;
}
static tb_long_t tb_poller_iocp_event_wait_ex(tb_poller_iocp_ref_t poller, tb_poller_event_func_t func, tb_long_t timeout)
{
    // clear error first
    SetLastError(ERROR_SUCCESS);

    // init events
    tb_size_t grow = tb_align8((poller->maxn >> 3) + 1);
    if (!poller->events)
    {
        poller->events_count = grow;
        poller->events = tb_nalloc_type(poller->events_count, tb_OVERLAPPED_ENTRY_t);
        tb_assert_and_check_return_val(poller->events, -1);
    }

    // wait events
    DWORD events_count = 0;
    BOOL  wait_ok = poller->func.GetQueuedCompletionStatusEx(poller->port, poller->events, (DWORD)poller->events_count, &events_count, (DWORD)(timeout < 0? INFINITE : timeout), FALSE);

    // the last error
    tb_size_t error = (tb_size_t)GetLastError();

    // timeout?
    if (!wait_ok && error == WAIT_TIMEOUT) return 0;

    // error?
    tb_assert_and_check_return_val(wait_ok, -1);

    // grow it if events is full
    if (events_count == poller->events_count)
    {
        // grow size
        poller->events_count += grow;
        if (poller->events_count > poller->maxn) poller->events_count = poller->maxn;

        // grow data
        poller->events = (tb_OVERLAPPED_ENTRY_t*)tb_ralloc(poller->events, poller->events_count * sizeof(tb_OVERLAPPED_ENTRY_t));
        tb_assert_and_check_return_val(poller->events, -1);
    }
    tb_assert(events_count <= poller->events_count);

    // limit 
    events_count = tb_min(events_count, (DWORD)poller->maxn);

    // handle events
    tb_size_t               i = 0;
    tb_size_t               wait = 0; 
    tb_OVERLAPPED_ENTRY_t*  e = tb_null;
    for (i = 0; i < events_count; i++)
    {
        // the iocp event
        e = poller->events + i;

        // get iocp object
        tb_iocp_object_ref_t object = (tb_iocp_object_ref_t)e->lpOverlapped;

        // iocp port is killed?
        tb_check_return_val(object, -1);

        // spark notification, is 1? 
        tb_check_continue(!e->lpCompletionKey);

        // this object is killing? ignore it directly
        if (object->state == TB_STATE_KILLING)
        {
            // trace
            tb_trace_d("wait_ex[%p]: ignore killing object", object->sock);
            tb_iocp_object_clear(object);
            continue ;
        }

        // check socket
        tb_assert_and_check_return_val(object->sock, -1);

        // get real transferred bytes
        tb_size_t real = (tb_size_t)e->dwNumberOfBytesTransferred;

        // get last error
        error = tb_ntstatus_to_winerror((tb_size_t)e->Internal);

        // trace
        tb_trace_d("wait_ex[%p]: real: %u bytes, lasterror: %lu", object->sock, real, error);

        // spark and update the events
        if (tb_poller_iocp_event_spak(poller, func, object, real, error) > 0)
            wait++;
    }

    // ok
    return wait;
}
static tb_long_t tb_poller_iocp_event_wait(tb_poller_iocp_ref_t poller, tb_poller_event_func_t func, tb_long_t timeout)
{
    tb_size_t wait = 0;
    tb_hong_t startime = tb_mclock();
    while (1)
    {
        // compute the left timeout
        tb_hong_t leftime = -1;
        if (timeout >= 0)
        {
            leftime = timeout - (tb_long_t)(tb_mclock() - startime);
            if (leftime <= 0) break;
        }

        // clear error first
        SetLastError(ERROR_SUCCESS);

        // wait event
        DWORD                   real = 0;
        tb_pointer_t            pkey = tb_null;
        tb_iocp_object_ref_t    object = tb_null;
        BOOL                    wait_ok = GetQueuedCompletionStatus(poller->port, (LPDWORD)&real, (PULONG_PTR)&pkey, (LPOVERLAPPED*)&object, (DWORD)(leftime < 0? INFINITE : (tb_size_t)leftime));

        // the last error
        tb_size_t error = (tb_size_t)GetLastError();

        // timeout or spark?
        if ((!wait_ok && (error == WAIT_TIMEOUT || error == ERROR_OPERATION_ABORTED)) || pkey) break;

        // iocp port is killed?
        tb_check_return_val(object, -1);

        // this object is killing? ignore it directly
        if (object->state == TB_STATE_KILLING)
        {
            // trace
            tb_trace_d("wait[%p]: ignore killing object", object->sock);
            tb_iocp_object_clear(object);
            continue ;
        }

        // check
        tb_assert_and_check_return_val(object->sock, -1);

        // trace
        tb_trace_d("wait[%p]: %s, real: %u bytes, lasterror: %lu", object->sock, wait_ok? "ok" : "failed", real, error);

        // spark and update the events
        if (tb_poller_iocp_event_spak(poller, func, object, real, error) > 0)
            wait++;
    }

    // ok
    return wait;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_poller_ref_t tb_poller_init(tb_cpointer_t priv)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_poller_iocp_ref_t    poller = tb_null;
    do
    {
        // check iovec
        tb_assert_static(sizeof(tb_iovec_t) == sizeof(WSABUF));
        tb_assert_and_check_break(tb_memberof_eq(tb_iovec_t, data, WSABUF, buf));
        tb_assert_and_check_break(tb_memberof_eq(tb_iovec_t, size, WSABUF, len));

        // make poller
        poller = tb_malloc0_type(tb_poller_iocp_t);
        tb_assert_and_check_break(poller);

        // init maxn 
        poller->maxn = 1 << 16;

        // init user private data
        poller->priv = priv;

        // init func
        poller->func.AcceptEx                    = tb_mswsock()->AcceptEx;
        poller->func.ConnectEx                   = tb_mswsock()->ConnectEx;
        poller->func.DisconnectEx                = tb_mswsock()->DisconnectEx;
        poller->func.TransmitFile                = tb_mswsock()->TransmitFile;
        poller->func.GetAcceptExSockaddrs        = tb_mswsock()->GetAcceptExSockaddrs;
        poller->func.GetQueuedCompletionStatusEx = tb_kernel32()->GetQueuedCompletionStatusEx;
        poller->func.WSAGetLastError             = tb_ws2_32()->WSAGetLastError;
        poller->func.WSASend                     = tb_ws2_32()->WSASend;
        poller->func.WSARecv                     = tb_ws2_32()->WSARecv;
        poller->func.WSASendTo                   = tb_ws2_32()->WSASendTo;
        poller->func.WSARecvFrom                 = tb_ws2_32()->WSARecvFrom;
        poller->func.bind                        = tb_ws2_32()->bind;
        tb_assert_and_check_break(poller->func.AcceptEx);
        tb_assert_and_check_break(poller->func.ConnectEx);
        tb_assert_and_check_break(poller->func.WSAGetLastError);
        tb_assert_and_check_break(poller->func.WSASend);
        tb_assert_and_check_break(poller->func.WSARecv);
        tb_assert_and_check_break(poller->func.WSASendTo);
        tb_assert_and_check_break(poller->func.WSARecvFrom);
        tb_assert_and_check_break(poller->func.bind);

        // init port
        poller->port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, tb_null, 0, 0);
        tb_assert_and_check_break(poller->port && poller->port != INVALID_HANDLE_VALUE);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (poller) tb_poller_exit((tb_poller_ref_t)poller);
        poller = tb_null;
    }

    // ok?
    return (tb_poller_ref_t)poller;
}
tb_void_t tb_poller_exit(tb_poller_ref_t self)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return(poller);

    // exit port
    if (poller->port) CloseHandle(poller->port);
    poller->port = tb_null;

    // exit events
    if (poller->events) tb_free(poller->events);
    poller->events          = tb_null;
    poller->events_count    = 0;

    // free it
    tb_free(poller);
}
tb_size_t tb_poller_type(tb_poller_ref_t poller)
{
    return TB_POLLER_TYPE_IOCP;
}
tb_cpointer_t tb_poller_priv(tb_poller_ref_t self)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return_val(poller, tb_null);

    // get the user private data
    return poller->priv;
}
tb_void_t tb_poller_kill(tb_poller_ref_t self)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return(poller);

    // post kill notification to iocp port
    PostQueuedCompletionStatus(poller->port, 0, tb_null, tb_null);
}
tb_void_t tb_poller_spak(tb_poller_ref_t self)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return(poller);

    // post spark notification to iocp port
    PostQueuedCompletionStatus(poller->port, 0, (ULONG_PTR)tb_u2p(1), tb_null);
}
tb_bool_t tb_poller_support(tb_poller_ref_t self, tb_size_t events)
{
    // all supported events 
    static const tb_size_t events_supported = TB_POLLER_EVENT_EALL | TB_POLLER_EVENT_ONESHOT;

    // is supported?
    return (events_supported & events) == events;
}
tb_bool_t tb_poller_insert(tb_poller_ref_t self, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return_val(poller && sock, tb_false);

    // get iocp object for this socket, @note only init event once in every thread
    tb_iocp_object_ref_t object = tb_iocp_object_get_or_new(sock);
    tb_assert_and_check_return_val(object, tb_false);

    // save the user private data
    object->priv = priv;

    // bind this socket and object to port
    if (!object->port) 
    {
        HANDLE port = CreateIoCompletionPort((HANDLE)(SOCKET)tb_sock2fd(sock), poller->port, tb_null, 0);
        if (port != poller->port)
        {
            // trace
            tb_trace_e("CreateIoCompletionPort failed: %d, socket: %p", GetLastError(), sock);
            return tb_false;
        }

        // bind ok
        object->port = port;
    }

    // post events
    return tb_poller_iocp_event_post(poller, sock, object, events);
}
tb_bool_t tb_poller_remove(tb_poller_ref_t self, tb_socket_ref_t sock)
{
    return tb_true;
}
tb_bool_t tb_poller_modify(tb_poller_ref_t self, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    return tb_poller_insert(self, sock, events, priv);
}
tb_long_t tb_poller_wait(tb_poller_ref_t self, tb_poller_event_func_t func, tb_long_t timeout)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return_val(poller && func, -1);

    // trace
    tb_trace_d("waiting with timeout(%ld) ..", timeout);

    // does use GetQueuedCompletionStatusEx to wait events?
    if (poller->func.GetQueuedCompletionStatusEx)
        return tb_poller_iocp_event_wait_ex(poller, func, timeout);
    else return tb_poller_iocp_event_wait(poller, func, timeout);
}

