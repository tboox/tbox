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
 * @file        poller_iocp.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "iocp_object.h"
#include "../thread_local.h"
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
    // the GetAcceptExSockaddrs func
    tb_mswsock_GetAcceptExSockaddrs_t           GetAcceptExSockaddrs;

    // the GetQueuedCompletionStatusEx func
    tb_kernel32_GetQueuedCompletionStatusEx_t   GetQueuedCompletionStatusEx;

    // WSAGetLastError
    tb_ws2_32_WSAGetLastError_t                 WSAGetLastError;

}tb_iocp_func_t;

// the poller iocp type
typedef struct __tb_poller_iocp_t
{
    // the poller base
    tb_poller_t             base;

    // the maxn
    tb_size_t               maxn;

    // the iocp func
    tb_iocp_func_t          func;

    // the i/o completion port
    HANDLE                  port;

    // the events
    tb_OVERLAPPED_ENTRY_t*  events;

    // the events count
    tb_size_t               events_count;

    // the last wait count
    tb_size_t               lastwait_count;

}tb_poller_iocp_t, *tb_poller_iocp_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private declaration
 */
__tb_extern_c_enter__

tb_poller_iocp_ref_t    tb_poller_iocp_self();
tb_bool_t               tb_poller_iocp_bind_object(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t iocp_object, tb_bool_t is_pipe);
HANDLE                  tb_pipe_file_handle(tb_pipe_file_ref_t file);

__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_thread_local_t g_poller_self = TB_THREAD_LOCAL_INIT;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
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
    ,   TB_POLLER_EVENT_RECV
    ,   TB_POLLER_EVENT_SEND
    ,   TB_POLLER_EVENT_CONN
    };
    tb_assert_and_check_return_val(code < tb_arrayn(s_events), TB_POLLER_EVENT_NONE);
    return s_events[code];
}
static tb_long_t tb_poller_iocp_event_spak_conn(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t iocp_object, tb_size_t real, tb_size_t error)
{
    // have been finished?
    tb_check_return_val(iocp_object->state != TB_STATE_FINISHED, 1);

    // done
    switch (error)
    {
        // ok?
    case ERROR_SUCCESS:
        iocp_object->u.conn.result = 1;
        break;
        // pending?
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        iocp_object->u.conn.result = 0;
        break;
       // timeout?
    case WSAEINTR:
    case ERROR_SEM_TIMEOUT:
    case ERROR_OPERATION_ABORTED:
        iocp_object->u.conn.result = 0;
        break;
        // failed?
    case WSAENOTCONN:
    case WSAECONNREFUSED:
    case ERROR_CONNECTION_REFUSED:
        iocp_object->u.conn.result = -1;
        break;
        // unknown error
    default:
        // trace
        tb_trace_e("connect(%p): %{ipaddr}, unknown error: %u", iocp_object->ref.sock, &iocp_object->u.conn.addr, error);
        iocp_object->u.conn.result = -1;
        break;
    }

    // trace
    tb_trace_d("connect(%p) %{ipaddr}, return: %ld", iocp_object->ref.sock, &iocp_object->u.conn.addr, iocp_object->u.conn.result);

    // ok
    return 1;
}
static tb_long_t tb_poller_iocp_event_spak_acpt(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t iocp_object, tb_size_t real, tb_size_t error)
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
            SOCKET fd = tb_sock2fd(iocp_object->u.acpt.result);
            SOCKET acpt = (SOCKET)tb_sock2fd(iocp_object->ref.sock);
            tb_ws2_32()->setsockopt(fd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (tb_char_t*)&acpt, sizeof(acpt));

            // non-block
            u_long nb = 1;
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
                tb_assert(iocp_object->buffer);

                // get server and client addresses
                poller->func.GetAcceptExSockaddrs(  (tb_byte_t*)iocp_object->buffer
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
                    tb_sockaddr_save(&iocp_object->u.acpt.addr, client_addr);

                    // trace
                    tb_trace_d("accept(%p): client address: %{ipaddr}", iocp_object->ref.sock, &iocp_object->u.acpt.addr);
                }
            }
        }
        break;
        // canceled? timeout?
    case WSAEINTR:
    case ERROR_OPERATION_ABORTED:
        if (iocp_object->u.acpt.result) tb_socket_exit(iocp_object->u.acpt.result);
        iocp_object->u.acpt.result = tb_null;
        break;
        // unknown error
    default:
        // trace
        tb_trace_e("accept(%p): unknown error: %u", iocp_object->ref.sock, error);
        if (iocp_object->u.acpt.result) tb_socket_exit(iocp_object->u.acpt.result);
        iocp_object->u.acpt.result = tb_null;
        break;
    }

    // ok
    return 1;
}
static tb_long_t tb_poller_iocp_event_spak_iorw(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t iocp_object, tb_size_t real, tb_size_t error)
{
    // have been finished?
    tb_check_return_val(iocp_object->state != TB_STATE_FINISHED, 1);

    // ok?
    if (real)
    {
        // trace
        tb_trace_d("iorw(%p): code: %u, real: %lu", iocp_object->ref.sock, iocp_object->code, real);

        // save address for urecv or urecvv
        if (iocp_object->code == TB_IOCP_OBJECT_CODE_URECV || iocp_object->code == TB_IOCP_OBJECT_CODE_URECVV)
        {
            // the address
            struct sockaddr_storage* addr = (struct sockaddr_storage*)iocp_object->buffer;
            tb_assert_and_check_return_val(addr, -1);

            // save address
            tb_sockaddr_save(iocp_object->code == TB_IOCP_OBJECT_CODE_URECV? &iocp_object->u.urecv.addr : &iocp_object->u.urecvv.addr, addr);
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
        iocp_object->u.recv.result = real;
        return 1;
    }

    // error?
    switch (error)
    {
        // connection closed?
    case ERROR_SUCCESS:
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        iocp_object->u.recv.result = -1;
        break;
       // canceled? timeout
    case WSAEINTR:
    case ERROR_OPERATION_ABORTED:
        iocp_object->u.recv.result = 0;
        break;
        // connection reset?
    case WSAECONNRESET:
    case ERROR_HANDLE_EOF:
    case ERROR_NETNAME_DELETED:
    case ERROR_BAD_COMMAND:
        iocp_object->u.recv.result = -1;
        break;
        // unknown error
    default:
        // trace
        tb_trace_e("iorw(%p): code: %u, unknown error: %lu", iocp_object->ref.sock, iocp_object->code, error);
        iocp_object->u.recv.result = -1;
        break;
    }

    // ok
    return 1;
}
static tb_long_t tb_poller_iocp_event_spak_pipe(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t iocp_object, tb_size_t real, tb_size_t error)
{
    // have been finished?
    tb_check_return_val(iocp_object->state != TB_STATE_FINISHED, 1);

    // ok?
    tb_long_t result = -1;
    if (real)
    {
        // trace
        tb_trace_d("pipe(%p): code: %u, real: %lu", iocp_object->ref.pipe, iocp_object->code, real);

        // save the result
        result = real;
    }
    else
    {
        // trace
        tb_trace_e("pipe(%p): code: %u, unknown error: %lu", iocp_object->ref.pipe, iocp_object->code, error);
    }

    // save the result
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_READ)
        iocp_object->u.read.result = result;
    else iocp_object->u.write.result = result;

    // ok
    return 1;
}
static tb_long_t tb_poller_iocp_event_spak_connpipe(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t iocp_object, tb_size_t real, tb_size_t error)
{
    // have been finished?
    tb_check_return_val(iocp_object->state != TB_STATE_FINISHED, 1);

    // done
    switch (error)
    {
        // ok?
    case ERROR_SUCCESS:
        iocp_object->u.connpipe.result = 1;
        break;
        // pending?
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        iocp_object->u.connpipe.result = 0;
        break;
       // timeout?
    case WSAEINTR:
    case ERROR_SEM_TIMEOUT:
    case ERROR_OPERATION_ABORTED:
        iocp_object->u.connpipe.result = 0;
        break;
        // failed?
    case WSAENOTCONN:
    case WSAECONNREFUSED:
    case ERROR_CONNECTION_REFUSED:
        iocp_object->u.connpipe.result = -1;
        break;
        // unknown error
    default:
        // trace
        tb_trace_e("connect_pipe(%p): unknown error: %u", iocp_object->ref.pipe, error);
        iocp_object->u.connpipe.result = -1;
        break;
    }

    // trace
    tb_trace_d("connect_pipe(%p) return: %ld", iocp_object->ref.pipe, iocp_object->u.connpipe.result);

    // ok
    return 1;
}
static tb_long_t tb_poller_iocp_event_spak(tb_poller_iocp_ref_t poller, tb_poller_event_func_t func, tb_iocp_object_ref_t iocp_object, tb_size_t real, tb_size_t error)
{
    // trace
    tb_trace_d("spak[%p]: code %u, state: %s ..", iocp_object->ref.sock, iocp_object->code, tb_state_cstr(iocp_object->state));

    // check
    tb_assert_and_check_return_val(iocp_object->state == TB_STATE_WAITING || iocp_object->state == TB_STATE_FINISHED, -1);

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
    ,   tb_poller_iocp_event_spak_pipe
    ,   tb_poller_iocp_event_spak_pipe
    ,   tb_poller_iocp_event_spak_connpipe
    };
    tb_assert_and_check_return_val(iocp_object->code < tb_arrayn(s_spak), -1);

    // spank event
    tb_long_t ok = (s_spak[iocp_object->code])? s_spak[iocp_object->code](poller, iocp_object, real, error) : -1;

    // finish to wait events
    iocp_object->state = TB_STATE_FINISHED;

    // do event handler
    tb_poller_object_t object;
    object.type = tb_iocp_object_is_pipe(iocp_object)? TB_POLLER_OBJECT_PIPE : TB_POLLER_OBJECT_SOCK;
    object.ref.ptr = iocp_object->ref.ptr;
    func((tb_poller_ref_t)poller, &object, tb_poller_iocp_event_from_code(iocp_object->code), iocp_object->priv);

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
        tb_iocp_object_ref_t iocp_object = (tb_iocp_object_ref_t)e->lpOverlapped;

        // spank notification?
        if (!iocp_object && tb_p2u32(e->lpCompletionKey) == 0x1)
            continue ;

        // iocp port is killed?
        tb_check_return_val(iocp_object, -1);

        // this iocp object is killing? ignore it directly
        if (iocp_object->state == TB_STATE_KILLING)
        {
            // trace
            tb_trace_d("wait_ex[%p]: ignore killing iocp_object", iocp_object->ref.sock);
            tb_iocp_object_clear(iocp_object);
            continue ;
        }

        // check socket
        tb_assert_and_check_return_val(iocp_object->ref.sock, -1);

        // get real transferred bytes
        tb_size_t real = (tb_size_t)e->dwNumberOfBytesTransferred;

        // get last error
        error = tb_ntstatus_to_winerror((tb_size_t)e->Internal);

        // trace
        tb_trace_d("wait_ex[%p]: real: %u bytes, lasterror: %lu", iocp_object->ref.sock, real, error);

        // spark and update the events
        if (tb_poller_iocp_event_spak(poller, func, iocp_object, real, error) > 0)
            wait++;
    }
    return wait;
}
static tb_long_t tb_poller_iocp_event_wait(tb_poller_iocp_ref_t poller, tb_poller_event_func_t func, tb_long_t timeout)
{
    tb_size_t wait = 0;
    while (1)
    {
        // compute the timeout
        if (wait) timeout = 0;

        // clear error first
        SetLastError(ERROR_SUCCESS);

        // wait event
        DWORD                   real = 0;
        tb_pointer_t            pkey = tb_null;
        tb_iocp_object_ref_t    iocp_object = tb_null;
        BOOL                    wait_ok = GetQueuedCompletionStatus(poller->port, (LPDWORD)&real, (PULONG_PTR)&pkey, (LPOVERLAPPED*)&iocp_object, (DWORD)(timeout < 0? INFINITE : timeout));

        // the last error
        tb_size_t error = (tb_size_t)GetLastError();

        // timeout?
        if (!wait_ok && (error == WAIT_TIMEOUT || error == ERROR_OPERATION_ABORTED))
            break;

        // spank notification?
        if (!iocp_object && tb_p2u32(pkey) == 0x1)
            break ;

        // iocp port is killed?
        tb_check_return_val(iocp_object, -1);

        // this iocp object is killing? ignore it directly
        if (iocp_object->state == TB_STATE_KILLING)
        {
            // trace
            tb_trace_d("wait[%p]: ignore killing iocp_object", iocp_object->ref.sock);
            tb_iocp_object_clear(iocp_object);
            continue ;
        }

        // check
        tb_assert_and_check_return_val(iocp_object->ref.sock, -1);

        // trace
        tb_trace_d("wait[%p]: %s, real: %u bytes, lasterror: %lu", iocp_object->ref.sock, wait_ok? "ok" : "failed", real, error);

        // spark and update the events
        if (tb_poller_iocp_event_spak(poller, func, iocp_object, real, error) > 0)
            wait++;
    }
    return wait;
}
tb_bool_t tb_poller_iocp_bind_object(tb_poller_iocp_ref_t poller, tb_iocp_object_ref_t iocp_object, tb_bool_t is_pipe)
{
    // check
    tb_assert(iocp_object);
    tb_assert_and_check_return_val(poller, tb_false);

    // bind this iocp object to port
    if (!iocp_object->port)
    {
        // get the another iocp object with this socket
        HANDLE handle = tb_null;
        tb_poller_object_t object;
        if (is_pipe)
        {
            object.type = TB_POLLER_OBJECT_PIPE;
            handle = (HANDLE)tb_pipe_file_handle(iocp_object->ref.pipe);
        }
        else
        {
            object.type = TB_POLLER_OBJECT_SOCK;
            handle = (HANDLE)(SOCKET)tb_sock2fd(iocp_object->ref.sock);
        }
        object.ref.ptr = iocp_object->ref.ptr;
        tb_iocp_object_ref_t iocp_object_another = tb_iocp_object_get(&object, TB_POLLER_EVENT_RECV);
        if (!iocp_object_another || iocp_object_another == iocp_object)
            iocp_object_another = tb_iocp_object_get(&object, TB_POLLER_EVENT_SEND);

        if (iocp_object_another && iocp_object_another->port)
        {
            // sync port and skip mode from the another iocp object with same handle
            iocp_object->port      = iocp_object_another->port;
            iocp_object->skip_cpos = iocp_object_another->skip_cpos;
        }
        else
        {
            // trace
            tb_trace_d("CreateIoCompletionPort handle(%p) to port(%d) ..", handle, poller->port);

            // do bind
            HANDLE port = CreateIoCompletionPort(handle, poller->port, (ULONG_PTR)tb_null, 0);
            if (port != poller->port)
            {
                // trace
                tb_trace_e("CreateIoCompletionPort handle(%p) to port(%d), error: %d", handle, poller->port, GetLastError());
                return tb_false;
            }
            iocp_object->port = port;

            // skip the completion notification on success
            if (tb_kernel32_has_SetFileCompletionNotificationModes())
            {
                if (tb_kernel32()->SetFileCompletionNotificationModes(handle, FILE_SKIP_COMPLETION_PORT_ON_SUCCESS))
                    iocp_object->skip_cpos = 1;
            }
        }
    }
    return tb_true;
}
static tb_void_t tb_poller_iocp_exit(tb_poller_t* self)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return(poller);

    // detach poller
    if (tb_poller_iocp_self() == poller)
        tb_thread_local_set(&g_poller_self, tb_null);

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
static tb_void_t tb_poller_iocp_kill(tb_poller_t* self)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return(poller);

    // trace
    tb_trace_d("iocp: kill ..");

    // post kill notification to iocp port
    PostQueuedCompletionStatus(poller->port, 0, (ULONG_PTR)tb_null, tb_null);
}
static tb_void_t tb_poller_iocp_spak(tb_poller_t* self)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return(poller);

    // trace
    tb_trace_d("iocp: spak ..");

    // post spark notification to iocp port
    PostQueuedCompletionStatus(poller->port, 0, (ULONG_PTR)tb_u2p(1), tb_null);
}
static tb_bool_t tb_poller_iocp_insert(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return_val(poller && object, tb_false);

    if (!(events & TB_POLLER_EVENT_NOEXTRA))
    {
        // get iocp object for this socket, @note only init event once in every thread
        if (events & TB_POLLER_EVENT_RECV)
        {
            tb_iocp_object_ref_t iocp_object = tb_iocp_object_get_or_new(object, TB_POLLER_EVENT_RECV);
            if (iocp_object) iocp_object->priv = priv;
        }
        if (events & TB_POLLER_EVENT_SEND)
        {
            tb_iocp_object_ref_t iocp_object = tb_iocp_object_get_or_new(object, TB_POLLER_EVENT_SEND);
            if (iocp_object) iocp_object->priv = priv;
        }
    }
    return tb_true;
}
static tb_bool_t tb_poller_iocp_remove(tb_poller_t* self, tb_poller_object_ref_t object)
{
    return tb_true;
}
static tb_bool_t tb_poller_iocp_modify(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    return tb_poller_iocp_insert(self, object, events, priv);
}
static tb_long_t tb_poller_iocp_wait(tb_poller_t* self, tb_poller_event_func_t func, tb_long_t timeout)
{
    // check
    tb_poller_iocp_ref_t poller = (tb_poller_iocp_ref_t)self;
    tb_assert_and_check_return_val(poller && func, -1);
    tb_assert(self == (tb_poller_t*)tb_poller_iocp_self());

    // trace
    tb_trace_d("waiting with timeout(%ld) ..", timeout);

    /* we can use GetQueuedCompletionStatusEx() to increase performance, perhaps,
     * but we may end up lowering perf if you max out only one I/O thread.
     */
    tb_long_t wait = -1;
    if (poller->lastwait_count > 1 && poller->func.GetQueuedCompletionStatusEx)
        wait = tb_poller_iocp_event_wait_ex(poller, func, timeout);
    else wait = tb_poller_iocp_event_wait(poller, func, timeout);

    // save the last wait count
    poller->lastwait_count = wait;

    // wait ok
    return wait;
}
static tb_void_t tb_poller_iocp_attach(tb_poller_t* self)
{
    // init self poller local
    if (!tb_thread_local_init(&g_poller_self, tb_null)) return ;

    // update and overide the current scheduler
    tb_thread_local_set(&g_poller_self, self);
}
tb_poller_iocp_ref_t tb_poller_iocp_self()
{
    return (tb_poller_iocp_ref_t)tb_thread_local_get(&g_poller_self);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_poller_t* tb_poller_iocp_init()
{
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

        // init base
        poller->base.type   = TB_POLLER_TYPE_IOCP;
        poller->base.exit   = tb_poller_iocp_exit;
        poller->base.kill   = tb_poller_iocp_kill;
        poller->base.spak   = tb_poller_iocp_spak;
        poller->base.wait   = tb_poller_iocp_wait;
        poller->base.insert = tb_poller_iocp_insert;
        poller->base.remove = tb_poller_iocp_remove;
        poller->base.modify = tb_poller_iocp_modify;
        poller->base.attach = tb_poller_iocp_attach;
        poller->base.supported_events = TB_POLLER_EVENT_EALL | TB_POLLER_EVENT_ONESHOT;

        // init maxn
        poller->maxn = 1 << 16;

        // init func
        poller->func.GetAcceptExSockaddrs        = tb_mswsock()->GetAcceptExSockaddrs;
        poller->func.GetQueuedCompletionStatusEx = tb_kernel32()->GetQueuedCompletionStatusEx;
        poller->func.WSAGetLastError             = tb_ws2_32()->WSAGetLastError;
        tb_assert_and_check_break(poller->func.WSAGetLastError);

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
        if (poller) tb_poller_iocp_exit((tb_poller_t*)poller);
        poller = tb_null;
    }

    // ok?
    return (tb_poller_t*)poller;
}

