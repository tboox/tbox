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
    // the user private data
    tb_cpointer_t           priv;

    // the iocp func
    tb_iocp_func_t          func;

    // the i/o completion port
    HANDLE                  port;

}tb_poller_iocp_t, *tb_poller_iocp_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_bool_t tb_poller_iocp_event_post_conn(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_SEND, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("post connection event for socket(%p): %{ipaddr}: ..", sock, &object->u.conn.addr);

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
        if (!PostQueuedCompletionStatus(poller->port, 0, (ULONG_PTR)object, (LPOVERLAPPED)&object->olap)) break;

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
            if (PostQueuedCompletionStatus(poller->port, 0, (ULONG_PTR)object, (LPOVERLAPPED)&object->olap)) ok = tb_true;
        }
    }

    // ok?
    return ok;
}
static tb_bool_t tb_poller_iocp_event_post_send(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events & TB_POLLER_EVENT_SEND, tb_false);
    tb_assert_and_check_return_val(object && object->state == TB_STATE_PENDING, tb_false);

    // trace
    tb_trace_d("insert send event for socket(%p): %lu bytes ..", sock, &object->u.send.size);

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
    return PostQueuedCompletionStatus(poller->port, 0, (ULONG_PTR)object, (LPOVERLAPPED)&object->olap);
}
static tb_bool_t tb_poller_iocp_event_post(tb_poller_iocp_ref_t poller, tb_socket_ref_t sock, tb_iocp_object_ref_t object, tb_size_t events)
{
    // check
    tb_assert_and_check_return_val(events, tb_false);

    // trace
    tb_trace_d("post events(%lx) for socket(%p), code: %u ..", events, sock, object->code);

    // init post
    static tb_bool_t (*s_post[])(tb_poller_iocp_ref_t , tb_socket_ref_t, tb_iocp_object_ref_t, tb_size_t ) = 
    {
        tb_null
    ,   tb_null // acpt
    ,   tb_poller_iocp_event_post_conn
    ,   tb_null // recv
    ,   tb_poller_iocp_event_post_send
    ,   tb_null // urecv
    ,   tb_null // usend
    ,   tb_null // recvv
    ,   tb_null // sendv
    ,   tb_null // urecvv
    ,   tb_null // usendv
    ,   tb_null // sendf
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
    tb_trace_d("tb_poller_iocp_event_spak_acpt");
    return -1;
}
static tb_long_t tb_poller_iocp_event_spak_iorw(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t object, tb_size_t real, tb_size_t error)
{
    // ok?
    if (real)
    {
        // trace
        tb_trace_d("iorw(%p): code: %u, real: %lu", object->sock, object->code, real);

        // check struct member offset for hacking the same result offset for the other iocp object
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_send_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_recvv_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_sendv_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_urecv_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_usend_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_urecvv_t, result));
        tb_assert_static(tb_offsetof(tb_iocp_object_recv_t, result) == tb_offsetof(tb_iocp_object_usendv_t, result));

        // save the result size
        object->u.recv.result = real;
        return 1;
    }

    // error? 
    switch (error)
    {       
        // ok?
    case ERROR_SUCCESS:
        object->u.recv.result = 1;
        break;
        // pending?
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        object->u.recv.result = 0;
        break;
       // canceled? timeout 
    case WSAEINTR:
    case ERROR_OPERATION_ABORTED:
        object->u.recv.result = 0;
        break;
        // closed?
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
        object->u.conn.result = -1;
        break;
    }

    // ok
    return 1;
}
static tb_long_t tb_poller_iocp_event_spak(tb_poller_iocp_ref_t poller, tb_poller_event_func_t func, tb_iocp_object_ref_t object, tb_size_t real, tb_size_t error)
{
    // have been finished? spark it directly
    if (object->state == TB_STATE_FINISHED) 
    {
        func((tb_poller_ref_t)poller, object->sock, tb_poller_iocp_event_from_code(object->code), object->priv);
        return 1;
    }

    // check
    tb_assert_and_check_return_val(object->state == TB_STATE_WAITING, -1);

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

    // trace
    tb_trace_d("spak[%p]: code %u ..", object->sock, object->code);

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
    tb_trace_d("wait_ex: ..");
    return -1;
}
static tb_long_t tb_poller_iocp_event_wait(tb_poller_iocp_ref_t poller, tb_poller_event_func_t func, tb_long_t timeout)
{
    // clear error first
    SetLastError(ERROR_SUCCESS);

    // wait event
    DWORD                   real = 0;
    tb_iocp_object_ref_t    object = tb_null;
    LPOVERLAPPED            olap = tb_null;
    BOOL                    wait = GetQueuedCompletionStatus(poller->port, (LPDWORD)&real, (PULONG_PTR)&object, (LPOVERLAPPED*)&olap, (DWORD)(timeout < 0? INFINITE : timeout));
    tb_assert_and_check_return_val(object && olap, -1);

    // the last error
    tb_size_t error = (tb_size_t)GetLastError();

    // trace
    tb_trace_d("wait[%p]: %d, real: %u, lasterror: %lu", object->sock, wait, real, error);

    // timeout?
    if (!wait && error == WAIT_TIMEOUT) return 0;

    // spark the events
    return tb_poller_iocp_event_spak(poller, func, object, real, error);
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

    // free it
    tb_free(poller);
}
tb_void_t tb_poller_clear(tb_poller_ref_t self)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return(poller);

    // TODO
    tb_trace_d("tb_poller_clear");
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

    // TODO
    tb_trace_d("tb_poller_kill");
}
tb_void_t tb_poller_spak(tb_poller_ref_t self)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return(poller);

    // TODO
    tb_trace_d("tb_poller_spak");
}
tb_bool_t tb_poller_support(tb_poller_ref_t self, tb_size_t events)
{
    // TODO
    // all supported events 
    static tb_size_t events_supported = TB_POLLER_EVENT_EALL;

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
    HANDLE port = CreateIoCompletionPort((HANDLE)tb_sock2fd(sock), poller->port, (ULONG_PTR)object, 0);
    if (port != poller->port)
    {
        // trace
        tb_trace_e("CreateIoCompletionPort failed: %d, socket: %p", GetLastError(), sock);
        return tb_false;
    }

    // post events
    return tb_poller_iocp_event_post(poller, sock, object, events);
}
tb_bool_t tb_poller_remove(tb_poller_ref_t self, tb_socket_ref_t sock)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return_val(poller && sock, tb_false);

    // ok
    return tb_true;
}
tb_bool_t tb_poller_modify(tb_poller_ref_t self, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return_val(poller && sock, tb_false);

    // get iocp object for this socket, @note only init event once in every thread
    tb_iocp_object_ref_t object = tb_iocp_object_get_or_new(sock);
    tb_assert_and_check_return_val(object, tb_false);

    // save the user private data
    object->priv = priv;

    // post events
    return tb_poller_iocp_event_post(poller, sock, object, events);
}
tb_long_t tb_poller_wait(tb_poller_ref_t self, tb_poller_event_func_t func, tb_long_t timeout)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return_val(poller && func, -1);

    // does use GetQueuedCompletionStatusEx to wait events?
    if (poller->func.GetQueuedCompletionStatusEx)
        return tb_poller_iocp_event_wait_ex(poller, func, timeout);
    else return tb_poller_iocp_event_wait(poller, func, timeout);
}

