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
#include "interface/interface.h"
#include "../thread_local.h"
#include "../impl/sockdata.h"
#include "../posix/sockaddr.h"
#include "../../libc/libc.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
#   include "../../coroutine/coroutine.h"
#   include "../../coroutine/impl/impl.h"
#endif

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
 * globals
 */

// the iocp object cache in the local thread (contains killing/killed object)
static tb_thread_local_t g_iocp_object_cache_local = TB_THREAD_LOCAL_INIT;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_bool_t tb_iocp_object_cache_clean(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    // the cache
    tb_list_entry_head_ref_t cache = (tb_list_entry_head_ref_t)value;
    tb_assert(cache);

    // the object
    tb_iocp_object_ref_t object = (tb_iocp_object_ref_t)item;
    tb_assert(object);

    // remove it?
    if (tb_list_entry_size(cache) > TB_IOCP_OBJECT_CACHE_MAXN && object->state != TB_STATE_KILLING)
    {
        // trace
        tb_trace_d("clean %s object(%p) in cache(%lu)", tb_state_cstr(object->state), object->sock, tb_list_entry_size(cache));
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
            tb_iocp_object_ref_t object = (tb_iocp_object_ref_t)tb_list_entry(cache, entry);
            if (object) 
            {
                // trace
                tb_trace_d("exit %s object(%p) in cache", tb_state_cstr(object->state), object->sock);

                // free object
                tb_free(object);
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
    tb_for_all_if (tb_iocp_object_ref_t, object, tb_list_entry_itor(cache), object)
    {
        if (object->state != TB_STATE_KILLING)
        {
            result = object;
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

        // remove this object from the cache
        tb_list_entry_remove(cache, &result->entry);

        // init it
        tb_memset(result, 0, sizeof(tb_iocp_object_t));
    }
    return result;
}
static __tb_inline__ tb_sockdata_ref_t tb_iocp_object_sockdata()
{
    // we only enable iocp in coroutine
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE)
    return (tb_co_scheduler_self() || tb_lo_scheduler_self_())? tb_sockdata() : tb_null;
#else
    return tb_null;
#endif
}
static tb_bool_t tb_iocp_object_cancel(tb_iocp_object_ref_t object)
{
    // check
    tb_assert_and_check_return_val(object && object->state == TB_STATE_WAITING && object->sock, tb_false);

    // get the local socket data
    tb_sockdata_ref_t sockdata = tb_iocp_object_sockdata();
    tb_assert_and_check_return_val(sockdata, tb_false);

    // get the iocp object cache
    tb_list_entry_head_ref_t cache = tb_iocp_object_cache();
    tb_assert_and_check_return_val(cache, tb_false);

    // trace
    tb_trace_d("sock(%p): cancel io ..", object->sock);

    // cancel io
    if (!CancelIo((HANDLE)(tb_size_t)tb_sock2fd(object->sock)))
    {
        // trace
        tb_trace_e("sock(%p): cancel io failed(%d)!", object->sock, GetLastError());
        return tb_false;
    }

    // move this object to the cache
    object->state = TB_STATE_KILLING;
    tb_list_entry_insert_tail(cache, &object->entry);

    // remove this object from the local socket data
    tb_sockdata_remove(sockdata, object->sock);

    // trace
    tb_trace_d("insert to the iocp object cache(%lu)", tb_list_entry_size(cache));

    // ok
    return tb_true;
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
            // attempt to alloc object from the cache first
            object = tb_iocp_object_cache_alloc();

            // alloc object from the heap if no cache
            if (!object) object = tb_malloc0_type(tb_iocp_object_t);
            tb_assert_and_check_break(object);

            // init object
            object->sock = sock;
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

    // get cache
    tb_list_entry_head_ref_t cache = tb_iocp_object_cache();
    tb_assert_and_check_return(cache);

    // get iocp object
    tb_iocp_object_ref_t object = (tb_iocp_object_ref_t)tb_sockdata_get(sockdata, sock);
    if (object)
    {
        // trace
        tb_trace_d("sock(%p): removing, state: %s", sock, tb_state_cstr(object->state));

        // clean some objects in cache
        tb_remove_if(tb_list_entry_itor(cache), tb_iocp_object_cache_clean, cache);

        // no waiting io or cancel failed? remove and free this iocp object directly
        if (object->state != TB_STATE_WAITING || !tb_iocp_object_cancel(object))
        {
            // trace
            tb_trace_d("sock(%p): removed directly, state: %s", sock, tb_state_cstr(object->state));

            // remove this object from the local socket data
            tb_sockdata_remove(sockdata, sock);

            // clear and free the object data
            tb_iocp_object_clear(object);

            // insert to iocp object cache
            if (tb_list_entry_size(cache) < TB_IOCP_OBJECT_CACHE_MAXN)
                tb_list_entry_insert_head(cache, &object->entry);
            else tb_free(object);
        }
    }
}
tb_void_t tb_iocp_object_clear(tb_iocp_object_ref_t object)
{
    // check
    tb_assert(object);

    // free the private buffer for iocp
    if (object->buffer)
    {
        tb_free(object->buffer);
        object->buffer = tb_null;
    }

    // trace
    tb_trace_d("sock(%p): clear %s ..", object->sock, tb_state_cstr(object->state));

    // clear object code and state
    object->code  = TB_IOCP_OBJECT_CODE_NONE;
    object->state = TB_STATE_OK;
}
tb_socket_ref_t tb_iocp_object_accept(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(object, tb_null);

    // always be accept, need not clear object each time
    tb_assert(object->code == TB_IOCP_OBJECT_CODE_NONE || object->code == TB_IOCP_OBJECT_CODE_ACPT);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_ACPT)
    {
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("accept(%p): state: %s, result: %p", object->sock, tb_state_cstr(object->state), object->u.acpt.result);

            // get result
            object->state = TB_STATE_OK;
            if (addr) tb_ipaddr_copy(addr, &object->u.acpt.addr);
            return object->u.acpt.result;
        }
        // waiting timeout before?
        else if (object->state == TB_STATE_WAITING)
        {
            // trace
            tb_trace_d("accept(%p): state: %s, continue ..", object->sock, tb_state_cstr(object->state));
            return tb_null;
        }
    }

    // trace
    tb_trace_d("accept(%p): state: %s ..", object->sock, tb_state_cstr(object->state));

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_socket_ref_t acpt = tb_null;
    do
    {
        // done  
        struct sockaddr_storage d = {0};
        tb_int_t                n = sizeof(d);
        SOCKET                  fd = tb_ws2_32()->accept(tb_sock2fd(object->sock), (struct sockaddr *)&d, &n);

        // no client?
        tb_check_break(fd >= 0 && fd != INVALID_SOCKET);

        // save sock
        acpt = tb_fd2sock(fd);

        // non-block
        ULONG nb = 1;
        if (tb_ws2_32()->ioctlsocket(fd, FIONBIO, &nb) == SOCKET_ERROR) break;

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

        // save address
        if (addr) tb_sockaddr_save(addr, &d);
        
        // trace
        tb_trace_d("accept(%p): %p, state: finished directly", object->sock, acpt);

        // ok
        ok = tb_true;

    } while (0);

    // accept ok?
    if (ok) return acpt;
    else
    {
        // exit it
        if (acpt) tb_socket_exit(acpt);
        acpt = tb_null;
    }

    // post a accept event to wait it
    object->code          = TB_IOCP_OBJECT_CODE_ACPT;
    object->state         = TB_STATE_PENDING;
    object->u.acpt.result = tb_null;
    return tb_null;
}
tb_long_t tb_iocp_object_connect(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(object && addr, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_CONN)
    {
        if (object->state == TB_STATE_FINISHED)
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
        // waiting timeout before?
        else if (object->state == TB_STATE_WAITING && tb_ipaddr_is_equal(&object->u.conn.addr, addr))
        {
            // trace
            tb_trace_d("connect(%p, %{ipaddr}): %s, continue ..", object->sock, addr, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("connect(%p, %{ipaddr}): %s ..", object->sock, addr, tb_state_cstr(object->state));

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

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

    // continue to the previous operation
    if (object->code == TB_IOCP_OBJECT_CODE_RECV)
    {
        // attempt to get the result if be finished
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("recv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.recv.result);

            // clear the previous object data first, but the result cannot be cleared
            tb_iocp_object_clear(object);
            return object->u.recv.result;
        }
        else if (object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(object->u.recv.data == data, -1);

            // trace
            tb_trace_d("recv(%p): state: %s, continue ..", object->sock, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("recv(%p, %lu): %s ..", object->sock, size, tb_state_cstr(object->state));

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

    // attach buffer data
    object->u.recv.data = data;
    object->u.recv.size = (tb_iovec_size_t)size;

    // attempt to recv data directly
    DWORD flag = 0;
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSARecv((SOCKET)tb_sock2fd(object->sock), (WSABUF*)&object->u.recv, 1, &real, &flag, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("recv(%p): WSARecv: %u bytes, state: finished directly", object->sock, real);
        return (tb_long_t)real;
    }

    // post a recv event to wait it
    object->code  = TB_IOCP_OBJECT_CODE_RECV;
    object->state = TB_STATE_PENDING;
    return 0;
}
tb_long_t tb_iocp_object_send(tb_iocp_object_ref_t object, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && data, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_SEND)
    {
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("send(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.send.result);

            // clear the previous object data first, but the result cannot be cleared
            tb_iocp_object_clear(object);
            return object->u.send.result;
        }     
        // waiting timeout before?
        else if (object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(object->u.send.data == data, -1);

            // trace
            tb_trace_d("send(%p): state: %s, continue ..", object->sock, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("send(%p, %lu): %s ..", object->sock, size, tb_state_cstr(object->state));

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

    // attempt buffer data
    object->u.send.data = data;
    object->u.send.size = (tb_iovec_size_t)size;

    // attempt to send data directly
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSASend((SOCKET)tb_sock2fd(object->sock), (WSABUF*)&object->u.send, 1, &real, 0, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("send(%p): WSASend: %u bytes, state: finished directly", object->sock, real);
        return (tb_long_t)real;
    }

    // post a send event to wait it
    object->code  = TB_IOCP_OBJECT_CODE_SEND;
    object->state = TB_STATE_PENDING;
    return 0;
}
tb_long_t tb_iocp_object_urecv(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && data && size, -1);

    // continue to the previous operation
    if (object->code == TB_IOCP_OBJECT_CODE_URECV)
    {
        // attempt to get the result if be finished
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("urecv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.urecv.result);

            // clear the previous object data first, but the result cannot be cleared
            tb_iocp_object_clear(object);
            if (addr) tb_ipaddr_copy(addr, &object->u.urecv.addr);
            return object->u.urecv.result;
        }
        // waiting timeout before?
        else if (object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(object->u.urecv.data == data, -1);

            // trace
            tb_trace_d("urecv(%p): state: %s, continue ..", object->sock, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("urecv(%p, %lu): %s ..", object->sock, size, tb_state_cstr(object->state));

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

    // attach buffer data
    object->u.urecv.data = data;
    object->u.urecv.size = (tb_iovec_size_t)size;

    // make buffer for address, size and flags
    if (!object->buffer) object->buffer = tb_malloc0(sizeof(struct sockaddr_storage) + sizeof(tb_int_t) + sizeof(DWORD));
    tb_assert_and_check_return_val(object->buffer, -1);

    // init size
    tb_int_t* psize = (tb_int_t*)((tb_byte_t*)object->buffer + sizeof(struct sockaddr_storage));
    *psize = sizeof(struct sockaddr_storage);

    // init flag
    DWORD* pflag = (DWORD*)((tb_byte_t*)object->buffer + sizeof(struct sockaddr_storage) + sizeof(tb_int_t));
    *pflag = 0;

    // attempt to recv data directly
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSARecvFrom((SOCKET)tb_sock2fd(object->sock), (WSABUF*)&object->u.urecv, 1, &real, pflag, (struct sockaddr*)object->buffer, psize, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("urecv(%p): WSARecvFrom: %u bytes, state: finished directly", object->sock, real);
        return (tb_long_t)real;
    }

    // post a urecv event to wait it
    object->code  = TB_IOCP_OBJECT_CODE_URECV;
    object->state = TB_STATE_PENDING;
    return 0;
}
tb_long_t tb_iocp_object_usend(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && addr && data, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_USEND)
    {
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("usend(%p, %{ipaddr}): state: %s, result: %ld", object->sock, addr, tb_state_cstr(object->state), object->u.usend.result);

            // clear the previous object data first, but the result cannot be cleared
            tb_iocp_object_clear(object);
            return object->u.usend.result;
        }  
        // waiting timeout before?
        else if (object->state == TB_STATE_WAITING && tb_ipaddr_is_equal(&object->u.usend.addr, addr))
        {
            // check
            tb_assert_and_check_return_val(object->u.usend.data == data, -1);

            // trace
            tb_trace_d("usend(%p, %{ipaddr}): state: %s, continue ..", object->sock, addr, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("usend(%p, %{ipaddr}, %lu): %s ..", object->sock, addr, size, tb_state_cstr(object->state));

    // has waiting io?
    if (object->state == TB_STATE_WAITING)
    {
        // get bound iocp port
        HANDLE port = object->port;

        // cancel the previous io (urecv) first
        if (!tb_iocp_object_cancel(object)) return -1;

        // create a new iocp object
        object = tb_iocp_object_get_or_new(object->sock);
        tb_assert_and_check_return_val(object, -1);

        // restore the previous bound iocp port
        object->port = port;
    }

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

    // attach buffer data and address
    object->u.usend.addr = *addr;
    object->u.usend.data = data;
    object->u.usend.size = (tb_iovec_size_t)size;

    // load address
    tb_size_t               n = 0;
	struct sockaddr_storage d = {0};
    if (!(n = tb_sockaddr_load(&d, &object->u.usend.addr))) return tb_false;

    // attempt to send data directly
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSASendTo((SOCKET)tb_sock2fd(object->sock), (WSABUF*)&object->u.usend, 1, &real, 0, (struct sockaddr*)&d, (tb_int_t)n, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("usend(%p, %{ipaddr}): WSASendTo: %u bytes, state: finished directly", object->sock, addr, real);
        return (tb_long_t)real;
    }

    // post a usend event to wait it
    object->code  = TB_IOCP_OBJECT_CODE_USEND;
    object->state = TB_STATE_PENDING;
    return 0;
}
tb_hong_t tb_iocp_object_sendf(tb_iocp_object_ref_t object, tb_file_ref_t file, tb_hize_t offset, tb_hize_t size)
{
    // check
    tb_assert_and_check_return_val(object && file, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_SENDF)
    {
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("sendfile(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.sendf.result);

            // clear the previous object data first, but the result cannot be cleared
            tb_iocp_object_clear(object);
            return object->u.sendf.result;
        }
        // waiting timeout before?
        else if (object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(object->u.sendf.file == file, -1);
            tb_assert_and_check_return_val(object->u.sendf.offset == offset, -1);

            // trace
            tb_trace_d("sendfile(%p): state: %s, continue ..", object->sock, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("sendfile(%p, %llu at %llu): %s ..", object->sock, size, offset, tb_state_cstr(object->state));

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

    // post a send event to wait it
    object->code           = TB_IOCP_OBJECT_CODE_SENDF;
    object->state          = TB_STATE_PENDING;
    object->u.sendf.file   = file;
    object->u.sendf.size   = size;
    object->u.sendf.offset = offset;
    return 0;
}
tb_long_t tb_iocp_object_recvv(tb_iocp_object_ref_t object, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && list && size, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_RECVV)
    {
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("recvv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.recvv.result);

            // clear the previous object data first, but the result cannot be cleared
            tb_iocp_object_clear(object);
            return object->u.recvv.result;
        }
        else if (object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(object->u.recvv.list == list, -1);

            // trace
            tb_trace_d("recvv(%p): state: %s, continue ..", object->sock, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("recvv(%p, %lu): %s ..", object->sock, size, tb_state_cstr(object->state));

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

    // attach buffer data
    object->u.recvv.list = list;
    object->u.recvv.size = (tb_iovec_size_t)size;

    // attempt to send data directly
    DWORD flag = 0;
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSARecv((SOCKET)tb_sock2fd(object->sock), (WSABUF*)object->u.recvv.list, (DWORD)object->u.recvv.size, &real, &flag, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("recvv(%p): WSARecv: %u bytes, state: finished directly", object->sock, real);
        return (tb_long_t)real;
    }

    // post a recvv event to wait it
    object->code  = TB_IOCP_OBJECT_CODE_RECVV;
    object->state = TB_STATE_PENDING;
    return 0;
}
tb_long_t tb_iocp_object_sendv(tb_iocp_object_ref_t object, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && list && size, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_SENDV)
    {
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("sendv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.sendv.result);

            // clear the previous object data first, but the result cannot be cleared
            tb_iocp_object_clear(object);
            return object->u.sendv.result;
        }
        // waiting timeout before?
        else if (object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(object->u.sendv.list == list, -1);

            // trace
            tb_trace_d("sendv(%p): state: %s, continue ..", object->sock, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("sendv(%p, %lu): %s ..", object->sock, size, tb_state_cstr(object->state));

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

    // attach buffer data
    object->u.sendv.list = list;
    object->u.sendv.size = (tb_iovec_size_t)size;

    // attempt to send data directly
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSASend((SOCKET)tb_sock2fd(object->sock), (WSABUF*)object->u.sendv.list, (DWORD)object->u.sendv.size, &real, 0, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("sendv(%p): WSASend: %u bytes, state: finished directly", object->sock, real);
        return (tb_long_t)real;
    }

    // post a sendv event to wait it
    object->code  = TB_IOCP_OBJECT_CODE_SENDV;
    object->state = TB_STATE_PENDING;
    return 0;
}
tb_long_t tb_iocp_object_urecvv(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && list && size, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_URECVV)
    {
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("urecvv(%p): state: %s, result: %ld", object->sock, tb_state_cstr(object->state), object->u.urecvv.result);

            // clear the previous object data first, but the result cannot be cleared
            tb_iocp_object_clear(object);
            if (addr) tb_ipaddr_copy(addr, &object->u.urecvv.addr);
            return object->u.urecvv.result;
        }
        // waiting timeout before?
        else if (object->state == TB_STATE_WAITING)
        {
            // check
            tb_assert_and_check_return_val(object->u.urecvv.list == list, -1);

            // trace
            tb_trace_d("urecvv(%p): state: %s, continue ..", object->sock, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("urecvv(%p, %lu): %s ..", object->sock, size, tb_state_cstr(object->state));

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

    // attach buffer data
    object->u.urecvv.list = list;
    object->u.urecvv.size = (tb_iovec_size_t)size;

    // make buffer for address, size and flags
    if (!object->buffer) object->buffer = tb_malloc0(sizeof(struct sockaddr_storage) + sizeof(tb_int_t) + sizeof(DWORD));
    tb_assert_and_check_return_val(object->buffer, tb_false);

    // init size
    tb_int_t* psize = (tb_int_t*)((tb_byte_t*)object->buffer + sizeof(struct sockaddr_storage));
    *psize = sizeof(struct sockaddr_storage);

    // init flag
    DWORD* pflag = (DWORD*)((tb_byte_t*)object->buffer + sizeof(struct sockaddr_storage) + sizeof(tb_int_t));
    *pflag = 0;

    // attempt to recv data directly
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSARecvFrom((SOCKET)tb_sock2fd(object->sock), (WSABUF*)object->u.urecvv.list, (DWORD)object->u.urecvv.size, &real, pflag, (struct sockaddr*)object->buffer, psize, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("urecv(%p): WSARecvFrom: %u bytes, state: finished directly", object->sock, real);
        return (tb_long_t)real;
    }

    // post a urecvv event to wait it
    object->code  = TB_IOCP_OBJECT_CODE_URECVV;
    object->state = TB_STATE_PENDING;
    return 0;
}
tb_long_t tb_iocp_object_usendv(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(object && addr && list, -1);

    // attempt to get the result if be finished
    if (object->code == TB_IOCP_OBJECT_CODE_USENDV)
    {
        if (object->state == TB_STATE_FINISHED)
        {
            // trace
            tb_trace_d("usendv(%p, %{ipaddr}): state: %s, result: %ld", object->sock, addr, tb_state_cstr(object->state), object->u.usendv.result);

            // clear the previous object data first, but the result cannot be cleared
            tb_iocp_object_clear(object);
            return object->u.usendv.result;
        }
        // waiting timeout before?
        else if (object->state == TB_STATE_WAITING && tb_ipaddr_is_equal(&object->u.usendv.addr, addr))
        {
            // check
            tb_assert_and_check_return_val(object->u.usendv.list == list, -1);

            // trace
            tb_trace_d("usendv(%p, %{ipaddr}): state: %s, continue ..", object->sock, addr, tb_state_cstr(object->state));
            return 0;
        }
    }

    // trace
    tb_trace_d("usendv(%p, %{ipaddr}, %lu): %s ..", object->sock, addr, size, tb_state_cstr(object->state));

    // has waiting io?
    if (object->state == TB_STATE_WAITING)
    {
        // get bound iocp port
        HANDLE port = object->port;

        // cancel the previous io (urecv) first
        if (!tb_iocp_object_cancel(object)) return -1;

        // create a new iocp object
        object = tb_iocp_object_get_or_new(object->sock);
        tb_assert_and_check_return_val(object, -1);

        // restore the previous bound iocp port
        object->port = port;
    }

    // check state
    tb_assert_and_check_return_val(object->state != TB_STATE_WAITING, -1);

    // attach buffer data and address
    object->u.usendv.addr = *addr;
    object->u.usendv.list = list;
    object->u.usendv.size = (tb_iovec_size_t)size;

    // load address
    tb_size_t               n = 0;
	struct sockaddr_storage d = {0};
    if (!(n = tb_sockaddr_load(&d, &object->u.usendv.addr))) return -1;

    // attempt to send data directly
    DWORD real = 0;
    tb_long_t ok = tb_ws2_32()->WSASendTo((SOCKET)tb_sock2fd(object->sock), (WSABUF*)object->u.usendv.list, (DWORD)object->u.usendv.size, &real, 0, (struct sockaddr*)&d, (tb_int_t)n, tb_null, tb_null);
    if (!ok && real)
    {
        // trace
        tb_trace_d("usend(%p, %{ipaddr}): WSASendTo: %u bytes, state: finished directly", object->sock, addr, real);
        return (tb_long_t)real;
    }

    // post a usendv event to wait it
    object->code  = TB_IOCP_OBJECT_CODE_USENDV;
    object->state = TB_STATE_PENDING;
    return 0;
}
