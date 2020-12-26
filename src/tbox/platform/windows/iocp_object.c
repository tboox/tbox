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
#include "interface/interface.h"
#include "../poller.h"
#include "../socket.h"
#include "../thread_local.h"
#include "../impl/pollerdata.h"
#include "../posix/sockaddr.h"
#include "../../libc/libc.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the iocp object cache maximum count
#ifdef __tb_small__
#   define TB_IOCP_OBJECT_CACHE_MAXN     (64)
#else
#   define TB_IOCP_OBJECT_CACHE_MAXN     (256)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private declaration
 */
__tb_extern_c_enter__

tb_poller_ref_t tb_poller_iocp_self();
tb_bool_t       tb_poller_iocp_bind_object(tb_poller_ref_t poller, tb_iocp_object_ref_t iocp_object, tb_bool_t is_pipe);
HANDLE          tb_pipe_file_handle(tb_pipe_file_ref_t file);

__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the iocp object cache in the local thread (contains killing/killed iocp object)
static tb_thread_local_t g_iocp_object_cache_local = TB_THREAD_LOCAL_INIT;

// the socket data in the local thread
#ifndef TB_CONFIG_MICRO_ENABLE
static tb_thread_local_t g_iocp_object_pollerdata_recv_local = TB_THREAD_LOCAL_INIT;
static tb_thread_local_t g_iocp_object_pollerdata_send_local = TB_THREAD_LOCAL_INIT;
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
#ifndef TB_CONFIG_MICRO_ENABLE
static tb_void_t tb_iocp_object_pollerdata_free(tb_cpointer_t priv)
{
    tb_pollerdata_ref_t pollerdata = (tb_pollerdata_ref_t)priv;
    if (pollerdata)
    {
        tb_pollerdata_exit(pollerdata);
        tb_free(pollerdata);
    }
}
#endif
static tb_bool_t tb_iocp_object_cache_clean(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    // the cache
    tb_list_entry_head_ref_t cache = (tb_list_entry_head_ref_t)value;
    tb_assert(cache);

    // the iocp object
    tb_iocp_object_ref_t iocp_object = (tb_iocp_object_ref_t)item;
    tb_assert(iocp_object);

    // remove it?
    if (tb_list_entry_size(cache) > TB_IOCP_OBJECT_CACHE_MAXN && iocp_object->state != TB_STATE_KILLING)
    {
        // trace
        tb_trace_d("clean %s iocp_object(%p) in cache(%lu)", tb_state_cstr(iocp_object->state), iocp_object->ref.sock, tb_list_entry_size(cache));
        return tb_true;
    }
    return tb_false;
}
static tb_void_t tb_iocp_object_cache_free(tb_cpointer_t priv)
{
    tb_list_entry_head_ref_t cache = (tb_list_entry_head_ref_t)priv;
    if (cache)
    {
        // trace
        tb_trace_d("exit iocp cache(%lu)", tb_list_entry_size(cache));

        // exit all cached iocp objects
        while (tb_list_entry_size(cache))
        {
            // get the next entry from head
            tb_list_entry_ref_t entry = tb_list_entry_head(cache);
            tb_assert(entry);

            // remove it from the cache
            tb_list_entry_remove_head(cache);

            // exit this iocp object
            tb_iocp_object_ref_t iocp_object = (tb_iocp_object_ref_t)tb_list_entry(cache, entry);
            if (iocp_object)
            {
                // trace
                tb_trace_d("exit %s iocp_object(%p) in cache", tb_state_cstr(iocp_object->state), iocp_object->ref.sock);

                // clear iocp object first
                tb_iocp_object_clear(iocp_object);

                // free iocp object
                tb_free(iocp_object);
            }
        }

        // exit cache entry
        tb_list_entry_exit(cache);

        // free cache
        tb_free(cache);
    }
}
static tb_list_entry_head_ref_t tb_iocp_object_cache()
{
    // init local iocp object cache local data
    if (!tb_thread_local_init(&g_iocp_object_cache_local, tb_iocp_object_cache_free)) return tb_null;

    // init local iocp object cache
    tb_list_entry_head_ref_t cache = (tb_list_entry_head_ref_t)tb_thread_local_get(&g_iocp_object_cache_local);
    if (!cache)
    {
        // make cache
        cache = tb_malloc0_type(tb_list_entry_head_t);
        if (cache)
        {
            // init cache entry
            tb_list_entry_init(cache, tb_iocp_object_t, entry, tb_null);

            // save cache to local thread
            tb_thread_local_set(&g_iocp_object_cache_local, cache);
        }
    }
    return cache;
}
static tb_iocp_object_ref_t tb_iocp_object_cache_alloc()
{
    // get cache
    tb_list_entry_head_ref_t cache = tb_iocp_object_cache();
    tb_assert_and_check_return_val(cache, tb_null);

    // find a free iocp object
    tb_iocp_object_ref_t result = tb_null;
    tb_for_all_if (tb_iocp_object_ref_t, iocp_object, tb_list_entry_itor(cache), iocp_object)
    {
        if (iocp_object->state != TB_STATE_KILLING)
        {
            result = iocp_object;
            break;
        }
    }

    // found?
    if (result)
    {
        // trace
        tb_trace_d("alloc an new iocp object from cache(%lu)", tb_list_entry_size(cache));

        // check
        tb_assert(result->state == TB_STATE_OK);

        // remove this iocp object from the cache
        tb_list_entry_remove(cache, &result->entry);

        // init it
        tb_memset(result, 0, sizeof(tb_iocp_object_t));
    }
    return result;
}
static tb_bool_t tb_iocp_object_cancel(tb_iocp_object_ref_t iocp_object)
{
    // check
    tb_assert_and_check_return_val(iocp_object && iocp_object->state == TB_STATE_WAITING, tb_false);
    tb_assert_and_check_return_val(iocp_object->ref.ptr, tb_false);

    // trace
    tb_trace_d("object(%p): cancel io ..", iocp_object->ref.ptr);

    // get the iocp object handle
    HANDLE handle = tb_null;
    if (tb_iocp_object_is_pipe(iocp_object))
        handle = (HANDLE)tb_pipe_file_handle(iocp_object->ref.pipe);
    else handle = (HANDLE)(SOCKET)tb_sock2fd(iocp_object->ref.sock);

    // cancel io
    if (!CancelIo(handle))
    {
        tb_trace_e("object(%p): cancel io failed(%d)!", iocp_object->ref.ptr, GetLastError());
        return tb_false;
    }
    return tb_true;
}
static tb_pollerdata_ref_t tb_iocp_object_pollerdata(tb_size_t waitevent)
{
#ifndef TB_CONFIG_MICRO_ENABLE
    if (tb_poller_iocp_self())
    {
        // init local socket data
        tb_thread_local_ref_t pollerdata_local = (waitevent & TB_SOCKET_EVENT_RECV)? &g_iocp_object_pollerdata_recv_local : &g_iocp_object_pollerdata_send_local;
        if (!tb_thread_local_init(pollerdata_local, tb_iocp_object_pollerdata_free)) return tb_null;

        // init socket data
        tb_pollerdata_ref_t pollerdata = (tb_pollerdata_ref_t)tb_thread_local_get(pollerdata_local);
        if (!pollerdata)
        {
            // make socket data
            pollerdata = tb_malloc0_type(tb_pollerdata_t);
            if (pollerdata)
            {
                // init socket data
                tb_pollerdata_init(pollerdata);

                // save socket data to local thread
                tb_thread_local_set(pollerdata_local, pollerdata);
            }
        }

        // ok?
        return pollerdata;
    }
#endif
    return tb_null;
}
static tb_void_t tb_iocp_object_pollerdata_remove(tb_poller_object_ref_t object, tb_size_t waitevent)
{
    // get the local socket data
    tb_pollerdata_ref_t pollerdata = tb_iocp_object_pollerdata(waitevent);
    tb_check_return(pollerdata);

    // get cache
    tb_list_entry_head_ref_t cache = tb_iocp_object_cache();
    tb_assert_and_check_return(cache);

    // get iocp object
    tb_iocp_object_ref_t iocp_object = (tb_iocp_object_ref_t)tb_pollerdata_get(pollerdata, object);
    if (iocp_object)
    {
        // trace
        tb_trace_d("object(%p): removing, state: %s", object->ref.ptr, tb_state_cstr(iocp_object->state));

        // clean some objects in cache
        tb_remove_if(tb_list_entry_itor(cache), tb_iocp_object_cache_clean, cache);

        // waiting and cancel ok?
        if (iocp_object->state == TB_STATE_WAITING && tb_iocp_object_cancel(iocp_object))
        {
            // move this iocp object to the cache
            iocp_object->state = TB_STATE_KILLING;
            tb_list_entry_insert_tail(cache, &iocp_object->entry);

            // remove this iocp object from the poller data
            tb_pollerdata_reset(pollerdata, object);

            // trace
            tb_trace_d("cancel ok, insert to the iocp object cache(%lu)", tb_list_entry_size(cache));
        }
        // no waiting io or cancel failed? remove and free this iocp object directly
        else
        {
            // trace
            tb_trace_d("object(%p): removed directly, state: %s", object->ref.ptr, tb_state_cstr(iocp_object->state));

            // remove this iocp object from the poller data
            tb_pollerdata_reset(pollerdata, object);

            // clear and free the iocp object data
            tb_iocp_object_clear(iocp_object);

            // insert to iocp object cache
            if (tb_list_entry_size(cache) < TB_IOCP_OBJECT_CACHE_MAXN)
                tb_list_entry_insert_head(cache, &iocp_object->entry);
            else tb_free(iocp_object);
        }
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iocp_object_ref_t tb_iocp_object_get_or_new(tb_poller_object_ref_t object, tb_size_t waitevent)
{
    // check
    tb_assert_and_check_return_val(object, tb_null);
    tb_assert(object->type == TB_POLLER_OBJECT_SOCK || object->type == TB_POLLER_OBJECT_PIPE);

    // get or new iocp object
    tb_iocp_object_ref_t iocp_object = tb_null;
    do
    {
        // get the poller data
        tb_pollerdata_ref_t pollerdata = tb_iocp_object_pollerdata(waitevent);
        tb_check_break(pollerdata);

        // attempt to get iocp object first if exists
        iocp_object = (tb_iocp_object_ref_t)tb_pollerdata_get(pollerdata, object);

        // new an iocp object if not exists
        if (!iocp_object)
        {
            // attempt to alloc iocp object from the cache first
            iocp_object = tb_iocp_object_cache_alloc();

            // alloc iocp object from the heap if no cache
            if (!iocp_object) iocp_object = tb_malloc0_type(tb_iocp_object_t);
            tb_assert_and_check_break(iocp_object);

            // init iocp object
            tb_iocp_object_clear(iocp_object);
            iocp_object->ref.ptr = object->ref.ptr;

            // save iocp object
            tb_pollerdata_set(pollerdata, object, (tb_cpointer_t)iocp_object);
        }

    } while (0);
    return iocp_object;
}
tb_iocp_object_ref_t tb_iocp_object_get(tb_poller_object_ref_t object, tb_size_t waitevent)
{
    tb_pollerdata_ref_t pollerdata = tb_iocp_object_pollerdata(waitevent);
    return pollerdata? (tb_iocp_object_ref_t)tb_pollerdata_get(pollerdata, object) : tb_null;
}
tb_void_t tb_iocp_object_remove(tb_poller_object_ref_t object)
{
    tb_iocp_object_pollerdata_remove(object, TB_POLLER_EVENT_RECV);
    tb_iocp_object_pollerdata_remove(object, TB_POLLER_EVENT_SEND);
}
tb_void_t tb_iocp_object_clear(tb_iocp_object_ref_t iocp_object)
{
    // check
    tb_assert(iocp_object);

    // free the private buffer for iocp
    if (iocp_object->buffer)
    {
        tb_free(iocp_object->buffer);
        iocp_object->buffer = tb_null;
    }

    // trace
    tb_trace_d("iocp_object(%p): clear %s ..", iocp_object->ref.ptr, tb_state_cstr(iocp_object->state));

    // clear iocp object code and state
    iocp_object->code  = TB_IOCP_OBJECT_CODE_NONE;
    iocp_object->state = TB_STATE_OK;
}
tb_socket_ref_t tb_iocp_object_accept(tb_iocp_object_ref_t iocp_object, tb_ipaddr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(iocp_object, tb_null);

    // always be accept, need not clear iocp object each time
    tb_assert(iocp_object->code == TB_IOCP_OBJECT_CODE_NONE || iocp_object->code == TB_IOCP_OBJECT_CODE_ACPT);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_ACPT)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("accept(%p): state: %s, result: %p", iocp_object->ref.sock, tb_state_cstr(iocp_object->state), iocp_object->u.acpt.result);

            // get result
            iocp_object->state = TB_STATE_OK;
            if (addr) tb_ipaddr_copy(addr, &iocp_object->u.acpt.addr);
            return iocp_object->u.acpt.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // trace
            tb_trace_d("accept(%p): state: %s, continue ..", iocp_object->ref.sock, tb_state_cstr(iocp_object->state));
            return tb_null;
        }
    }

    // trace
    tb_trace_d("accept(%p): state: %s ..", iocp_object->ref.sock, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, tb_null);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return tb_null;

    // post a accept event
    tb_bool_t ok = tb_false;
    tb_bool_t init_ok = tb_false;
    tb_bool_t AcceptEx_ok = tb_false;
    do
    {
        // init olap
        tb_memset(&iocp_object->olap, 0, sizeof(OVERLAPPED));

        // make address buffer
        if (!iocp_object->buffer) iocp_object->buffer = tb_malloc0(((sizeof(struct sockaddr_storage)) << 1));
        tb_assert_and_check_break(iocp_object->buffer);

        // get bound address family
        struct sockaddr_storage bound_addr;
        socklen_t len = sizeof(bound_addr);
        tb_size_t family = TB_IPADDR_FAMILY_IPV4;
        if (getsockname((SOCKET)tb_sock2fd(iocp_object->ref.sock), (struct sockaddr *)&bound_addr, &len) != -1 && bound_addr.ss_family == AF_INET6)
            family = TB_IPADDR_FAMILY_IPV6;

        // make accept socket
        iocp_object->u.acpt.result = tb_socket_init(TB_SOCKET_TYPE_TCP, family);
        tb_assert_and_check_break(iocp_object->u.acpt.result);
        init_ok = tb_true;

        // the client fd
        SOCKET clientfd = tb_sock2fd(iocp_object->u.acpt.result);

        /* do AcceptEx
         *
         * @note this socket have been bound to local address in tb_socket_connect()
         */
        DWORD real = 0;
        AcceptEx_ok = tb_mswsock()->AcceptEx(   (SOCKET)tb_sock2fd(iocp_object->ref.sock)
                                            ,   clientfd
                                            ,   (tb_byte_t*)iocp_object->buffer
                                            ,   0
                                            ,   sizeof(struct sockaddr_storage)
                                            ,   sizeof(struct sockaddr_storage)
                                            ,   &real
                                            ,   (LPOVERLAPPED)&iocp_object->olap)? tb_true : tb_false;

        // trace
        tb_trace_d("accept(%p): AcceptEx: %d, lasterror: %d", iocp_object->ref.sock, AcceptEx_ok, tb_ws2_32()->WSAGetLastError());
        tb_check_break(AcceptEx_ok);

        // update the accept context, otherwise shutdown and getsockname will be failed
        SOCKET acceptfd = (SOCKET)tb_sock2fd(iocp_object->ref.sock);
        tb_ws2_32()->setsockopt(clientfd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (tb_char_t*)&acceptfd, sizeof(acceptfd));

        // non-block
        u_long nb = 1;
        tb_ws2_32()->ioctlsocket(clientfd, FIONBIO, &nb);

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
        tb_ws2_32()->setsockopt(clientfd, IPPROTO_TCP, TCP_NODELAY, (tb_char_t*)&enable, sizeof(enable));

        // get accept socket addresses
        INT                         server_size = 0;
        INT                         client_size = 0;
        struct sockaddr_storage*    server_addr = tb_null;
        struct sockaddr_storage*    client_addr = tb_null;
        if (addr && tb_mswsock()->GetAcceptExSockaddrs)
        {
            // check
            tb_assert(iocp_object->buffer);

            // get server and client addresses
            tb_mswsock()->GetAcceptExSockaddrs( (tb_byte_t*)iocp_object->buffer
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
                tb_sockaddr_save(addr, client_addr);

                // trace
                tb_trace_d("accept(%p): client address: %{ipaddr}", iocp_object->ref.sock, addr);
            }
        }

        // trace
        tb_trace_d("accept(%p): result: %p, state: finished directly", iocp_object->ref.sock, iocp_object->u.acpt.result);

        // ok
        ok = tb_true;

    } while (0);

    // AcceptEx failed?
    if (!ok)
    {
        // pending? continue it
        if (init_ok && WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())
        {
            iocp_object->code  = TB_IOCP_OBJECT_CODE_ACPT;
            iocp_object->state = TB_STATE_WAITING;
        }
        // failed?
        else
        {
            // free result socket
            if (iocp_object->u.acpt.result) tb_socket_exit(iocp_object->u.acpt.result);
            iocp_object->u.acpt.result = tb_null;
        }
    }

    // ok?
    return ok? iocp_object->u.acpt.result : tb_null;
}
tb_long_t tb_iocp_object_connect(tb_iocp_object_ref_t iocp_object, tb_ipaddr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(iocp_object && addr, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_CONN)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            /* clear the previous iocp object data first
             *
             * @note conn.addr and conn.result cannot be cleared
             */
            tb_iocp_object_clear(iocp_object);
            if (tb_ipaddr_is_equal(&iocp_object->u.conn.addr, addr))
            {
                // trace
                tb_trace_d("connect(%p): %{ipaddr}, skip: %d, state: %s, result: %ld", iocp_object->ref.sock, addr, iocp_object->skip_cpos, tb_state_cstr(iocp_object->state), iocp_object->u.conn.result);

                // ok
                return iocp_object->u.conn.result;
            }
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING && tb_ipaddr_is_equal(&iocp_object->u.conn.addr, addr))
        {
            // trace
            tb_trace_d("connect(%p, %{ipaddr}): %s, continue ..", iocp_object->ref.sock, addr, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("connect(%p, %{ipaddr}): %s ..", iocp_object->ref.sock, addr, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // post a connection event
    tb_long_t ok = -1;
    tb_bool_t init_ok = tb_false;
    tb_bool_t ConnectEx_ok = tb_false;
    do
    {
        // init olap
        tb_memset(&iocp_object->olap, 0, sizeof(OVERLAPPED));

        // load client address
        tb_size_t caddr_size = 0;
        struct sockaddr_storage caddr_data;
        if (!(caddr_size = tb_sockaddr_load(&caddr_data, addr))) break;

        // load local address
        tb_size_t               laddr_size = 0;
        struct sockaddr_storage laddr_data;
        tb_ipaddr_t             laddr;
        if (!tb_ipaddr_set(&laddr, tb_null, 0, (tb_uint8_t)tb_ipaddr_family(addr))) break;
        if (!(laddr_size = tb_sockaddr_load(&laddr_data, &laddr))) break;

        // bind it first for ConnectEx
        if (SOCKET_ERROR == tb_ws2_32()->bind((SOCKET)tb_sock2fd(iocp_object->ref.sock), (LPSOCKADDR)&laddr_data, (tb_int_t)laddr_size))
        {
            // trace
            tb_trace_e("connect(%p, %{ipaddr}): bind failed, error: %u", iocp_object->ref.sock, addr, GetLastError());
            break;
        }
        init_ok = tb_true;

        /* do ConnectEx
         *
         * @note this socket have been bound to local address in tb_socket_connect()
         */
        DWORD real = 0;
        ConnectEx_ok = tb_mswsock()->ConnectEx( (SOCKET)tb_sock2fd(iocp_object->ref.sock)
                                            ,   (struct sockaddr const*)&caddr_data
                                            ,   (tb_int_t)caddr_size
                                            ,   tb_null
                                            ,   0
                                            ,   &real
                                            ,   (LPOVERLAPPED)&iocp_object->olap)? tb_true : tb_false;

        // trace
        tb_trace_d("connect(%p): ConnectEx: %d, lasterror: %d", iocp_object->ref.sock, ConnectEx_ok, tb_ws2_32()->WSAGetLastError());
        tb_check_break(ConnectEx_ok);

        // trace
        tb_trace_d("connect(%p): %{ipaddr}, skip: %d, state: finished directly", iocp_object->ref.sock, addr, iocp_object->skip_cpos);

        // ok
        ok = 1;

    } while (0);

    // ConnectEx failed?
    if (ok < 0)
    {
        // pending? continue to wait it
        if (init_ok && WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())
        {
            ok = 0;
            iocp_object->code          = TB_IOCP_OBJECT_CODE_CONN;
            iocp_object->state         = TB_STATE_WAITING;
            iocp_object->u.conn.addr   = *addr;
            iocp_object->u.conn.result = -1;
        }
        // already connected?
        else if (tb_ws2_32()->WSAGetLastError() == WSAEISCONN) ok = 1;
    }

    // failed?
    if (ok < 0) tb_iocp_object_clear(iocp_object);
    return ok;
}
tb_long_t tb_iocp_object_recv(tb_iocp_object_ref_t iocp_object, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && data && size, -1);

    // continue to the previous operation
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_RECV)
    {
        // attempt to get the result if be finished
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("recv(%p): state: %s, result: %ld", iocp_object->ref.sock, tb_state_cstr(iocp_object->state), iocp_object->u.recv.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.recv.result;
        }
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(iocp_object->u.recv.data == data, -1);

            // trace
            tb_trace_d("recv(%p): state: %s, continue ..", iocp_object->ref.sock, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("recv(%p, %lu): %s ..", iocp_object->ref.sock, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // attach buffer data
    iocp_object->u.recv.data = data;
    iocp_object->u.recv.size = (tb_iovec_size_t)size;

    // attempt to recv data directly
    DWORD flag = 0;
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSARecv((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)&iocp_object->u.recv, 1, &real, &flag, (LPOVERLAPPED)&iocp_object->olap, tb_null);

    // finished and skip iocp notification? return it directly
    if (!ok && iocp_object->skip_cpos)
    {
        // trace
        tb_trace_d("recv(%p): WSARecv: %u bytes, skip: %d, state: finished directly", iocp_object->ref.sock, real, iocp_object->skip_cpos);
        return (tb_long_t)(real > 0? real : (tb_long_t)-1);
    }

    // trace
    tb_trace_d("recv(%p): WSARecv: %ld, skip: %d, lasterror: %d", iocp_object->ref.sock, ok, iocp_object->skip_cpos, tb_ws2_32()->WSAGetLastError());

    // ok or pending? continue to wait it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())))
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_RECV;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_send(tb_iocp_object_ref_t iocp_object, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && data, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_SEND)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("send(%p): state: %s, result: %ld", iocp_object->ref.sock, tb_state_cstr(iocp_object->state), iocp_object->u.send.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.send.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(iocp_object->u.send.data == data, -1);

            // trace
            tb_trace_d("send(%p): state: %s, continue ..", iocp_object->ref.sock, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("send(%p, %lu): %s ..", iocp_object->ref.sock, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // attempt buffer data
    iocp_object->u.send.data = data;
    iocp_object->u.send.size = (tb_iovec_size_t)size;

    // attempt to send data directly
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSASend((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)&iocp_object->u.send, 1, &real, 0, (LPOVERLAPPED)&iocp_object->olap, tb_null);

    // finished and skip iocp notification? return it directly
    if (!ok && iocp_object->skip_cpos)
    {
        // trace
        tb_trace_d("send(%p): WSASend: %u bytes, skip: %d, state: finished directly", iocp_object->ref.sock, real, iocp_object->skip_cpos);
        return (tb_long_t)(real > 0? real : (tb_long_t)-1);
    }

    // trace
    tb_trace_d("send(%p): WSASend: %ld, skip: %d, lasterror: %d", iocp_object->ref.sock, ok, iocp_object->skip_cpos, tb_ws2_32()->WSAGetLastError());

    // ok or pending? continue to wait it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())))
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_SEND;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_urecv(tb_iocp_object_ref_t iocp_object, tb_ipaddr_ref_t addr, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && data && size, -1);

    // continue to the previous operation
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_URECV)
    {
        // attempt to get the result if be finished
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("urecv(%p): state: %s, result: %ld", iocp_object->ref.sock, tb_state_cstr(iocp_object->state), iocp_object->u.urecv.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            if (addr) tb_ipaddr_copy(addr, &iocp_object->u.urecv.addr);
            return iocp_object->u.urecv.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(iocp_object->u.urecv.data == data, -1);

            // trace
            tb_trace_d("urecv(%p): state: %s, continue ..", iocp_object->ref.sock, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("urecv(%p, %lu): %s ..", iocp_object->ref.sock, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // attach buffer data
    iocp_object->u.urecv.data = data;
    iocp_object->u.urecv.size = (tb_iovec_size_t)size;

    // make buffer for address, size and flags
    if (!iocp_object->buffer) iocp_object->buffer = tb_malloc0(sizeof(struct sockaddr_storage) + sizeof(tb_int_t) + sizeof(DWORD));
    tb_assert_and_check_return_val(iocp_object->buffer, -1);

    // init size
    tb_int_t* psize = (tb_int_t*)((tb_byte_t*)iocp_object->buffer + sizeof(struct sockaddr_storage));
    *psize = sizeof(struct sockaddr_storage);

    // init flag
    DWORD* pflag = (DWORD*)((tb_byte_t*)iocp_object->buffer + sizeof(struct sockaddr_storage) + sizeof(tb_int_t));
    *pflag = 0;

    /* post to recv event
     *
     * It's not safe to skip completion notifications for UDP:
     * https://blogs.technet.com/b/winserverperformance/archive/2008/06/26/designing-applications-for-high-performance-part-iii.aspx
     */
    tb_long_t ok = tb_ws2_32()->WSARecvFrom((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)&iocp_object->u.urecv, 1, tb_null, pflag, (struct sockaddr*)iocp_object->buffer, psize, (LPOVERLAPPED)&iocp_object->olap, tb_null);

    // trace
    tb_trace_d("urecv(%p): WSARecvFrom: %ld, lasterror: %d", iocp_object->ref.sock, ok, tb_ws2_32()->WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())))
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_URECV;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_usend(tb_iocp_object_ref_t iocp_object, tb_ipaddr_ref_t addr, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && addr && data, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_USEND)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("usend(%p, %{ipaddr}): state: %s, result: %ld", iocp_object->ref.sock, addr, tb_state_cstr(iocp_object->state), iocp_object->u.usend.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.usend.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING && tb_ipaddr_is_equal(&iocp_object->u.usend.addr, addr))
        {
            // check
            tb_assert_and_check_return_val(iocp_object->u.usend.data == data, -1);

            // trace
            tb_trace_d("usend(%p, %{ipaddr}): state: %s, continue ..", iocp_object->ref.sock, addr, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("usend(%p, %{ipaddr}, %lu): %s ..", iocp_object->ref.sock, addr, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // attach buffer data and address
    iocp_object->u.usend.addr = *addr;
    iocp_object->u.usend.data = data;
    iocp_object->u.usend.size = (tb_iovec_size_t)size;

    // load address
    tb_size_t n = 0;
	struct sockaddr_storage d;
    if (!(n = tb_sockaddr_load(&d, &iocp_object->u.usend.addr))) return tb_false;

    /* attempt to send data directly
     *
     * It's not safe to skip completion notifications for UDP:
     * https://blogs.technet.com/b/winserverperformance/archive/2008/06/26/designing-applications-for-high-performance-part-iii.aspx
     *
     * So we attempt to send data firstly without overlapped.
     */
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSASendTo((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)&iocp_object->u.usend, 1, &real, 0, (struct sockaddr*)&d, (tb_int_t)n, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("usend(%p, %{ipaddr}): WSASendTo: %u bytes, state: finished directly", iocp_object->ref.sock, addr, real);
        return (tb_long_t)real;
    }

    // post a send event
    ok = tb_ws2_32()->WSASendTo((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)&iocp_object->u.usend, 1, tb_null, 0, (struct sockaddr*)&d, (tb_int_t)n, (LPOVERLAPPED)&iocp_object->olap, tb_null);

    // trace
    tb_trace_d("usend(%p, %{ipaddr}): WSASendTo: %ld, lasterror: %d", iocp_object->ref.sock, addr, ok, tb_ws2_32()->WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())))
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_USEND;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_hong_t tb_iocp_object_sendf(tb_iocp_object_ref_t iocp_object, tb_file_ref_t file, tb_hize_t offset, tb_hize_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && file, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_SENDF)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("sendfile(%p): state: %s, result: %ld", iocp_object->ref.sock, tb_state_cstr(iocp_object->state), iocp_object->u.sendf.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.sendf.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(iocp_object->u.sendf.file == file, -1);
            tb_assert_and_check_return_val(iocp_object->u.sendf.offset == offset, -1);

            // trace
            tb_trace_d("sendfile(%p): state: %s, continue ..", iocp_object->ref.sock, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("sendfile(%p, %llu at %llu): %s ..", iocp_object->ref.sock, size, offset, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // do send file
    iocp_object->olap.Offset     = (DWORD)offset;
    iocp_object->olap.OffsetHigh = (DWORD)(offset >> 32);
    BOOL ok = tb_mswsock()->TransmitFile((SOCKET)tb_sock2fd(iocp_object->ref.sock), (HANDLE)file, (DWORD)size, (1 << 16), (LPOVERLAPPED)&iocp_object->olap, tb_null, 0);

    // trace
    tb_trace_d("sendfile(%p): TransmitFile: %d, lasterror: %d", iocp_object->ref.sock, ok, tb_ws2_32()->WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())))
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_SENDF;
        iocp_object->state = TB_STATE_WAITING;
        iocp_object->u.sendf.file    = file;
        iocp_object->u.sendf.offset  = offset;
        iocp_object->u.sendf.size    = size;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_recvv(tb_iocp_object_ref_t iocp_object, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && list && size, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_RECVV)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("recvv(%p): state: %s, result: %ld", iocp_object->ref.sock, tb_state_cstr(iocp_object->state), iocp_object->u.recvv.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.recvv.result;
        }
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(iocp_object->u.recvv.list == list, -1);

            // trace
            tb_trace_d("recvv(%p): state: %s, continue ..", iocp_object->ref.sock, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("recvv(%p, %lu): %s ..", iocp_object->ref.sock, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // attach buffer data
    iocp_object->u.recvv.list = list;
    iocp_object->u.recvv.size = (tb_iovec_size_t)size;

    // attempt to recv data directly
    DWORD flag = 0;
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSARecv((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)&iocp_object->u.recvv.list, (DWORD)iocp_object->u.recvv.size, &real, &flag, (LPOVERLAPPED)&iocp_object->olap, tb_null);

    // finished and skip iocp notification? return it directly
    if (!ok && iocp_object->skip_cpos)
    {
        // trace
        tb_trace_d("recvv(%p): WSARecv: %u bytes, skip: %d, state: finished directly", iocp_object->ref.sock, real, iocp_object->skip_cpos);
        return (tb_long_t)(real > 0? real : (tb_long_t)-1);
    }

    // trace
    tb_trace_d("recvv(%p): WSARecv: %ld, skip: %d, lasterror: %d", iocp_object->ref.sock, ok, iocp_object->skip_cpos, tb_ws2_32()->WSAGetLastError());

    // ok or pending? continue to wait it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())))
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_RECVV;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_sendv(tb_iocp_object_ref_t iocp_object, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && list && size, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_SENDV)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("sendv(%p): state: %s, result: %ld", iocp_object->ref.sock, tb_state_cstr(iocp_object->state), iocp_object->u.sendv.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.sendv.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(iocp_object->u.sendv.list == list, -1);

            // trace
            tb_trace_d("sendv(%p): state: %s, continue ..", iocp_object->ref.sock, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("sendv(%p, %lu): %s ..", iocp_object->ref.sock, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // attach buffer data
    iocp_object->u.sendv.list = list;
    iocp_object->u.sendv.size = (tb_iovec_size_t)size;

    // attempt to send data directly
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSASend((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)&iocp_object->u.sendv.list, (DWORD)iocp_object->u.sendv.size, &real, 0, (LPOVERLAPPED)&iocp_object->olap, tb_null);

    // finished and skip iocp notification? return it directly
    if (!ok && iocp_object->skip_cpos)
    {
        // trace
        tb_trace_d("sendv(%p): WSASend: %u bytes, skip: %d, state: finished directly", iocp_object->ref.sock, real, iocp_object->skip_cpos);
        return (tb_long_t)(real > 0? real : (tb_long_t)-1);
    }

    // trace
    tb_trace_d("sendv(%p): WSASend: %ld, skip: %d, lasterror: %d", iocp_object->ref.sock, ok, iocp_object->skip_cpos, tb_ws2_32()->WSAGetLastError());

    // ok or pending? continue to wait it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())))
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_SENDV;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_urecvv(tb_iocp_object_ref_t iocp_object, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && list && size, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_URECVV)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("urecvv(%p): state: %s, result: %ld", iocp_object->ref.sock, tb_state_cstr(iocp_object->state), iocp_object->u.urecvv.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            if (addr) tb_ipaddr_copy(addr, &iocp_object->u.urecvv.addr);
            return iocp_object->u.urecvv.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(iocp_object->u.urecvv.list == list, -1);

            // trace
            tb_trace_d("urecvv(%p): state: %s, continue ..", iocp_object->ref.sock, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("urecvv(%p, %lu): %s ..", iocp_object->ref.sock, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // attach buffer data
    iocp_object->u.urecvv.list = list;
    iocp_object->u.urecvv.size = (tb_iovec_size_t)size;

    // make buffer for address, size and flags
    if (!iocp_object->buffer) iocp_object->buffer = tb_malloc0(sizeof(struct sockaddr_storage) + sizeof(tb_int_t) + sizeof(DWORD));
    tb_assert_and_check_return_val(iocp_object->buffer, tb_false);

    // init size
    tb_int_t* psize = (tb_int_t*)((tb_byte_t*)iocp_object->buffer + sizeof(struct sockaddr_storage));
    *psize = sizeof(struct sockaddr_storage);

    // init flag
    DWORD* pflag = (DWORD*)((tb_byte_t*)iocp_object->buffer + sizeof(struct sockaddr_storage) + sizeof(tb_int_t));
    *pflag = 0;

    /* post to recv event
     *
     * It's not safe to skip completion notifications for UDP:
     * https://blogs.technet.com/b/winserverperformance/archive/2008/06/26/designing-applications-for-high-performance-part-iii.aspx
     */
    tb_long_t ok = tb_ws2_32()->WSARecvFrom((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)&iocp_object->u.urecvv.list, (DWORD)iocp_object->u.urecvv.size, tb_null, pflag, (struct sockaddr*)iocp_object->buffer, psize, (LPOVERLAPPED)&iocp_object->olap, tb_null);

    // trace
    tb_trace_d("urecvv(%p): WSARecvFrom: %ld, lasterror: %d", iocp_object->ref.sock, ok, tb_ws2_32()->WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())))
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_URECVV;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_usendv(tb_iocp_object_ref_t iocp_object, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && addr && list, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_USENDV)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("usendv(%p, %{ipaddr}): state: %s, result: %ld", iocp_object->ref.sock, addr, tb_state_cstr(iocp_object->state), iocp_object->u.usendv.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.usendv.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING && tb_ipaddr_is_equal(&iocp_object->u.usendv.addr, addr))
        {
            // check
            tb_assert_and_check_return_val(iocp_object->u.usendv.list == list, -1);

            // trace
            tb_trace_d("usendv(%p, %{ipaddr}): state: %s, continue ..", iocp_object->ref.sock, addr, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("usendv(%p, %{ipaddr}, %lu): %s ..", iocp_object->ref.sock, addr, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_false)) return -1;

    // attach buffer data and address
    iocp_object->u.usendv.addr = *addr;
    iocp_object->u.usendv.list = list;
    iocp_object->u.usendv.size = (tb_iovec_size_t)size;

    // load address
    tb_size_t n = 0;
	struct sockaddr_storage d;
    if (!(n = tb_sockaddr_load(&d, &iocp_object->u.usendv.addr))) return -1;

    /* attempt to send data directly
     *
     * It's not safe to skip completion notifications for UDP:
     * https://blogs.technet.com/b/winserverperformance/archive/2008/06/26/designing-applications-for-high-performance-part-iii.aspx
     *
     * So we attempt to send data firstly without overlapped.
     */
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSASendTo((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)iocp_object->u.usendv.list, (DWORD)iocp_object->u.usendv.size, &real, 0, (struct sockaddr*)&d, (tb_int_t)n, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("usendv(%p, %{ipaddr}): WSASendTo: %u bytes, state: finished directly", iocp_object->ref.sock, addr, real);
        return (tb_long_t)real;
    }

    // post a send event
    ok = tb_ws2_32()->WSASendTo((SOCKET)tb_sock2fd(iocp_object->ref.sock), (WSABUF*)&iocp_object->u.usendv.list, (DWORD)iocp_object->u.usendv.size, tb_null, 0, (struct sockaddr*)&d, (tb_int_t)n, (LPOVERLAPPED)&iocp_object->olap, tb_null);

    // trace
    tb_trace_d("usendv(%p, %{ipaddr}): WSASendTo: %ld, lasterror: %d", iocp_object->ref.sock, addr, ok, tb_ws2_32()->WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == tb_ws2_32()->WSAGetLastError())))
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_USENDV;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_read(tb_iocp_object_ref_t iocp_object, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && data && size, -1);

    // continue to the previous operation
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_READ)
    {
        // attempt to get the result if be finished
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("read(%p): state: %s, result: %ld", iocp_object->ref.pipe, tb_state_cstr(iocp_object->state), iocp_object->u.read.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.read.result;
        }
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // trace
            tb_trace_d("read(%p): state: %s, continue ..", iocp_object->ref.pipe, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("read(%p, %lu): %s ..", iocp_object->ref.pipe, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_true)) return -1;

    // attempt to read data directly
    DWORD real = 0;
    BOOL ok = ReadFile(tb_pipe_file_handle(iocp_object->ref.pipe), data, (DWORD)size, &real, (LPOVERLAPPED)&iocp_object->olap);

    // finished? return it directly
    if (ok)
    {
        // trace
        tb_trace_d("read(%p): ReadFile: %u bytes, skip: %d, state: finished directly", iocp_object->ref.pipe, real, iocp_object->skip_cpos);
        return (tb_long_t)(real > 0? real : (tb_long_t)-1);
    }

    // trace
    tb_trace_d("read(%p): ReadFile: %ld, lasterror: %d", iocp_object->ref.pipe, real, GetLastError());

    // pending? continue to wait it
    if (ERROR_IO_PENDING == WSAGetLastError())
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_READ;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_write(tb_iocp_object_ref_t iocp_object, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(iocp_object && data, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_WRITE)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("write(%p): state: %s, result: %ld", iocp_object->ref.pipe, tb_state_cstr(iocp_object->state), iocp_object->u.write.result);

            // clear the previous iocp object data first, but the result cannot be cleared
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.write.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // trace
            tb_trace_d("write(%p): state: %s, continue ..", iocp_object->ref.pipe, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("write(%p, %lu): %s ..", iocp_object->ref.pipe, size, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_true)) return -1;

    // attempt to write data directly
    DWORD real = 0;
    BOOL ok = WriteFile(tb_pipe_file_handle(iocp_object->ref.pipe), data, (DWORD)size, &real, (LPOVERLAPPED)&iocp_object->olap);

    // finished? return it directly
    if (ok)
    {
        // trace
        tb_trace_d("write(%p): WriteFile: %u bytes, skip: %d, state: finished directly", iocp_object->ref.pipe, real, iocp_object->skip_cpos);
        return (tb_long_t)(real > 0? real : (tb_long_t)-1);
    }

    // trace
    tb_trace_d("write(%p): WriteFile: %ld, lasterror: %d", iocp_object->ref.pipe, real, GetLastError());

    // ok or pending? continue to wait it
    if (ERROR_IO_PENDING == GetLastError())
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_WRITE;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
tb_long_t tb_iocp_object_connect_pipe(tb_iocp_object_ref_t iocp_object)
{
    // check
    tb_assert_and_check_return_val(iocp_object, -1);

    // attempt to get the result if be finished
    if (iocp_object->code == TB_IOCP_OBJECT_CODE_CONNPIPE)
    {
        if (iocp_object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("connect_pipe(%p): skip: %d, state: %s, result: %ld", iocp_object->ref.pipe, iocp_object->skip_cpos, tb_state_cstr(iocp_object->state), iocp_object->u.connpipe.result);

            // ok
            tb_iocp_object_clear(iocp_object);
            return iocp_object->u.connpipe.result;
        }
        // waiting timeout before?
        else if (iocp_object->state == TB_STATE_WAITING)
        {
            // trace
            tb_trace_d("connect_pipe(%p): %s, continue ..", iocp_object->ref.pipe, tb_state_cstr(iocp_object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("connect_pipe(%p): %s ..", iocp_object->ref.pipe, tb_state_cstr(iocp_object->state));

    // check state
    tb_assert_and_check_return_val(iocp_object->state != TB_STATE_WAITING, -1);

    // bind iocp object first
    if (!tb_poller_iocp_bind_object(tb_poller_iocp_self(), iocp_object, tb_true)) return -1;

    // attempt to connect pipe directly
    BOOL ok = ConnectNamedPipe(tb_pipe_file_handle(iocp_object->ref.pipe), (LPOVERLAPPED)&iocp_object->olap);

    // finished? return it directly
    if (ok)
    {
        // trace
        tb_trace_d("connect_pipe(%p): ConnectNamedPipe: ok, skip: %d, state: finished directly", iocp_object->ref.pipe, iocp_object->skip_cpos);
        return 1;
    }

    // trace
    tb_trace_d("connect_pipe(%p): ConnectNamedPipe lasterror: %d", iocp_object->ref.pipe, GetLastError());

    // ok or pending? continue to wait it
    if (ERROR_IO_PENDING == GetLastError())
    {
        iocp_object->code  = TB_IOCP_OBJECT_CODE_CONNPIPE;
        iocp_object->state = TB_STATE_WAITING;
        return 0;
    }

    // failed
    tb_iocp_object_clear(iocp_object);
    return -1;
}
