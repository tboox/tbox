/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        sock.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "async_stream_sock"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the sock cache maxn
#ifdef __tb_small__
#   define TB_BASIC_STREAM_SOCK_CACHE_MAXN  (8192)
#else
#   define TB_BASIC_STREAM_SOCK_CACHE_MAXN  (8192 << 1)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the sock stream type
typedef struct __tb_async_stream_sock_t
{
    // the base
    tb_async_stream_t                   base;

    // the sock handle
    tb_handle_t                         sock;

    // the aico
    tb_handle_t                         aico;

    // the aicp dns
    tb_handle_t                         hdns;

#ifdef TB_SSL_ENABLE
    // the aicp ssl
    tb_handle_t                         hssl;
#endif

    // the ipv4 addr
    tb_ipv4_t                           ipv4;

    // the sock type
    tb_uint32_t                         type    : 30;

    // the sock bref
    tb_uint32_t                         bref    : 1;

    // keep alive after being closed?
    tb_uint32_t                         balived : 1;

    // is reading now?
    tb_uint32_t                         bread   : 1;

    // the offset
    tb_atomic64_t                       offset;

    // the func
    union
    {
        tb_async_stream_open_func_t     open;
        tb_async_stream_read_func_t     read;
        tb_async_stream_writ_func_t     writ;
        tb_async_stream_task_func_t     task;

    }                                   func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_sock_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_async_stream_sock_t* tb_async_stream_sock_cast(tb_handle_t stream)
{
    tb_async_stream_t* astream = (tb_async_stream_t*)stream;
    tb_assert_and_check_return_val(astream && astream->base.type == TB_STREAM_TYPE_SOCK, tb_null);
    return (tb_async_stream_sock_t*)astream;
}
#ifdef TB_SSL_ENABLE
static tb_bool_t tb_async_stream_sock_sopen_func(tb_handle_t ssl, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast((tb_handle_t)priv);
    tb_assert_and_check_return_val(ssl && sstream->func.open, tb_false);

    // trace
    tb_trace_d("ssl: open: %s", tb_state_cstr(state));

    // ok? opened
    if (state == TB_STATE_OK) tb_atomic_set(&sstream->base.base.bopened, 1);

    // done func
    sstream->func.open((tb_async_stream_t*)sstream, state, sstream->priv);

    // ok
    return tb_true;
}
#endif
static tb_bool_t tb_async_stream_sock_conn_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_CONN, tb_false);

    // the stream
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast((tb_handle_t)aice->priv);
    tb_assert_and_check_return_val(sstream && sstream->func.open, tb_false);

    // done
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        {
#ifdef TB_SSL_ENABLE
            // ssl?
            if (tb_url_ssl_get(&sstream->base.base.url))
            {
                // init state
                state = TB_STATE_SOCK_SSL_FAILED;

                // check
                tb_assert_and_check_break(sstream->sock);

                // init ssl
                if (!sstream->hssl) sstream->hssl = tb_aicp_ssl_init(sstream->base.aicp, tb_false);
                tb_assert_and_check_break(sstream->hssl);

                // init ssl sock
                tb_aicp_ssl_set_sock(sstream->hssl, sstream->sock);

                // init ssl timeout
                tb_aicp_ssl_set_timeout(sstream->hssl, sstream->base.base.timeout);

                // open ssl
                if (!tb_aicp_ssl_open(sstream->hssl, tb_async_stream_sock_sopen_func, sstream)) break;
            }
            else
#endif
            {
                // opened
                tb_atomic_set(&sstream->base.base.bopened, 1);

                // done func
                sstream->func.open((tb_async_stream_t*)sstream, TB_STATE_OK, sstream->priv);
            }
            
            // ok
            state = TB_STATE_OK;
        }
        break;
        // timeout
    case TB_STATE_TIMEOUT:
        state = TB_STATE_SOCK_CONNECT_TIMEOUT;
        break;
        // killed
    case TB_STATE_KILLED:
        state = TB_STATE_KILLED;
        break;
        // failed
    default:
        state = TB_STATE_SOCK_CONNECT_FAILED;
        break;
    }

    // failed? done func
    if (state != TB_STATE_OK) sstream->func.open((tb_async_stream_t*)sstream, state, sstream->priv);

    // ok
    return tb_true;
}
static tb_void_t tb_async_stream_sock_dns_func(tb_handle_t haddr, tb_char_t const* host, tb_ipv4_t const* addr, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast((tb_handle_t)priv);
    tb_assert_and_check_return(haddr && sstream && sstream->func.open);

    // done
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    do
    {
        // addr ok?
        if (addr)
        {
            // trace
            tb_trace_d("addr[%s]: %u.%u.%u.%u", host, tb_ipv4_u8x4(*addr));

            // init sock
            if (!sstream->sock) 
            {
                // open sock
                sstream->sock = tb_socket_open(sstream->type);
                sstream->bref = 0;

                // open sock failed?
                if (!sstream->sock)
                {
                    // trace
                    tb_trace_e("open sock failed!");

                    // save state
                    state = TB_STATE_SOCK_OPEN_FAILED;
                    break;
                }
            }

            // resize cache
            tb_size_t rcache = tb_socket_recv_buffer_size(sstream->sock);
            tb_size_t wcache = tb_socket_send_buffer_size(sstream->sock);
            if (rcache) sstream->base.rcache_maxn = rcache;
            if (wcache) sstream->base.wcache_maxn = wcache;

            // init aico
            if (!sstream->aico) sstream->aico = tb_aico_init_sock(sstream->base.aicp, sstream->sock);
            tb_assert_and_check_break(sstream->aico);

            // init timeout
            tb_aico_timeout_set(sstream->aico, TB_AICO_TIMEOUT_CONN, sstream->base.base.timeout);
            tb_aico_timeout_set(sstream->aico, TB_AICO_TIMEOUT_RECV, sstream->base.base.timeout);
            tb_aico_timeout_set(sstream->aico, TB_AICO_TIMEOUT_SEND, sstream->base.base.timeout);

            // port
            tb_size_t port = tb_url_port_get(&sstream->base.base.url);
            tb_assert_and_check_break(port);
            
            // the sock type: tcp or udp? for url: sock://ip:port/?udp=
            tb_char_t const* args = tb_url_args_get(&sstream->base.base.url);
            if (args && !tb_strnicmp(args, "udp=", 4)) sstream->type = TB_SOCKET_TYPE_UDP;
            else if (args && !tb_strnicmp(args, "tcp=", 4)) sstream->type = TB_SOCKET_TYPE_TCP;
            tb_assert_and_check_break(sstream->type == TB_SOCKET_TYPE_TCP || sstream->type == TB_SOCKET_TYPE_UDP);

            // tcp?
            if (sstream->type == TB_SOCKET_TYPE_TCP)
            {
                // done conn
                if (!tb_aico_conn(sstream->aico, addr, port, tb_async_stream_sock_conn_func, sstream)) break;
            }
            // udp?
            else
            {
                // ssl? not supported
                if (tb_url_ssl_get(&sstream->base.base.url))
                {
                    // trace
                    tb_trace_w("udp ssl is not supported!");
                }

                // save ipv4
                sstream->ipv4 = *addr;

                // opened
                tb_atomic_set(&sstream->base.base.bopened, 1);

                // done func
                sstream->func.open((tb_async_stream_t*)sstream, TB_STATE_OK, sstream->priv);
            }

            // ok
            state = TB_STATE_OK;
        }
        // timeout or failed?
        else
        {
            // trace
            tb_trace_d("addr[%s]: failed", host);

            // dns failed
            state = TB_STATE_SOCK_DNS_FAILED;
        }

    } while (0);

    // done func if failed
    if (state != TB_STATE_OK) sstream->func.open((tb_async_stream_t*)sstream, state, sstream->priv);
}
static tb_bool_t tb_async_stream_sock_open(tb_handle_t astream, tb_async_stream_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return_val(sstream && sstream->type && func, tb_false);

    // clear the mode
    sstream->bread = 0;

    // clear the offset
    tb_atomic64_set0(&sstream->offset);

#ifndef TB_SSL_ENABLE
    // ssl? not supported
    if (tb_url_ssl_get(&sstream->base.base.url))
    {
        // trace
        tb_trace_w("ssl is not supported now! please enable it from config if you need it.");

        // done func
        func((tb_async_stream_t*)sstream, TB_STATE_SOCK_SSL_NOT_SUPPORTED, priv);

        // ok
        return tb_true;
    }
#endif

    // keep alive and have been opened? reopen it directly
    if (sstream->balived && sstream->hdns && sstream->aico)
    {
        // opened
        tb_atomic_set(&sstream->base.base.bopened, 1);

        // done func
        func((tb_async_stream_t*)sstream, TB_STATE_OK, priv);

        // ok
        return tb_true;
    }

    // get the host from url
    tb_char_t const* host = tb_url_host_get(&sstream->base.base.url);
    tb_assert_and_check_return_val(host, tb_false);

    // clear ipv4
    tb_ipv4_clr(&sstream->ipv4);

    // save func and priv
    sstream->priv       = priv;
    sstream->func.open  = func;

    // init dns
    if (!sstream->hdns) sstream->hdns = tb_aicp_dns_init(sstream->base.aicp, tb_stream_timeout(astream), tb_async_stream_sock_dns_func, tb_null, astream);
    tb_assert(sstream->hdns);

    // done addr
    if (!sstream->hdns || !tb_aicp_dns_done(sstream->hdns, host))
    {
        // done func
        func((tb_async_stream_t*)sstream, TB_STATE_SOCK_DNS_FAILED, priv);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_sock_clos(tb_handle_t astream, tb_async_stream_clos_func_t func, tb_cpointer_t priv)
{   
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return_val(sstream, tb_false);

    // trace
    tb_trace_d("clos: ..");

    // noimpl
    tb_trace_noimpl();
    return tb_false;

#if 0
    // clear the mode
    sstream->bread = 0;

    // clear the offset
    tb_atomic64_set0(&sstream->offset);

#ifdef TB_SSL_ENABLE
    // close ssl
    if (sstream->hssl) tb_aicp_ssl_clos(sstream->hssl, bcalling);
#endif

    // keep alive? not close it
    tb_check_return(!sstream->balived);

    // exit aico
    if (sstream->aico) tb_aico_exit(sstream->aico);
    sstream->aico = tb_null;

    // exit dns
    if (sstream->hdns) tb_aicp_dns_exit(sstream->hdns);
    sstream->hdns = tb_null;

    // exit it
    if (!sstream->bref && sstream->sock) tb_socket_clos(sstream->sock);
    sstream->sock = tb_null;
    sstream->bref = 0;

    // exit ipv4
    tb_ipv4_clr(&sstream->ipv4);

    // trace
    tb_trace_d("clos: ok");
#endif
}
static tb_bool_t tb_async_stream_sock_read_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RECV, tb_false);

    // the stream
    tb_async_stream_sock_t* sstream = (tb_async_stream_sock_t*)aice->priv;
    tb_assert_and_check_return_val(sstream && sstream->func.read, tb_false);
 
    // trace
    tb_trace_d("recv: real: %lu, size: %lu, state: %s", aice->u.recv.real, aice->u.recv.size, tb_state_cstr(aice->state));

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        tb_atomic64_fetch_and_add(&sstream->offset, aice->u.recv.real);
        state = TB_STATE_OK;
        break;
        // closed
    case TB_STATE_CLOSED:
        state = TB_STATE_CLOSED;
        break;
        // killed
    case TB_STATE_KILLED:
        state = TB_STATE_KILLED;
        break;
        // timeout?
    case TB_STATE_TIMEOUT:
        state = TB_STATE_SOCK_RECV_TIMEOUT;
        break;
    default:
        tb_trace_d("read: unknown state: %s", tb_state_cstr(aice->state));
        break;
    }

    // done func
    if (sstream->func.read((tb_async_stream_t*)sstream, state, aice->u.recv.data, aice->u.recv.real, aice->u.recv.size, sstream->priv))
    {
        // continue?
        if (aice->state == TB_STATE_OK)
        {
            // continue to post read
            if (!tb_aico_recv(aice->aico, aice->u.recv.data, aice->u.recv.size, tb_async_stream_sock_read_func, (tb_async_stream_t*)sstream))
                sstream->func.read((tb_async_stream_t*)sstream, TB_STATE_SOCK_RECV_FAILED, aice->u.recv.data, 0, aice->u.recv.size, sstream->priv);
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_sock_uread_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_URECV, tb_false);

    // the stream
    tb_async_stream_sock_t* sstream = (tb_async_stream_sock_t*)aice->priv;
    tb_assert_and_check_return_val(sstream && sstream->func.read, tb_false);
 
    // trace
    tb_trace_d("urecv: real: %lu, size: %lu, state: %s", aice->u.urecv.real, aice->u.urecv.size, tb_state_cstr(aice->state));

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        tb_atomic64_fetch_and_add(&sstream->offset, aice->u.urecv.real);
        state = TB_STATE_OK;
        break;
        // closed
    case TB_STATE_CLOSED:
        state = TB_STATE_CLOSED;
        break;
        // killed
    case TB_STATE_KILLED:
        state = TB_STATE_KILLED;
        break;
        // timeout?
    case TB_STATE_TIMEOUT:
        state = TB_STATE_SOCK_RECV_TIMEOUT;
        break;
    default:
        tb_trace_d("read: unknown state: %s", tb_state_cstr(aice->state));
        break;
    }

    // done func
    if (sstream->func.read((tb_async_stream_t*)sstream, state, aice->u.urecv.data, aice->u.urecv.real, aice->u.urecv.size, sstream->priv))
    {
        // continue?
        if (aice->state == TB_STATE_OK)
        {
            // continue to post read
            if (!tb_aico_urecv(aice->aico, &aice->u.urecv.addr, aice->u.urecv.port, aice->u.urecv.data, aice->u.urecv.size, tb_async_stream_sock_uread_func, (tb_async_stream_t*)sstream))
                sstream->func.read((tb_async_stream_t*)sstream, TB_STATE_SOCK_RECV_FAILED, aice->u.urecv.data, 0, aice->u.urecv.size, sstream->priv);
        }
    }

    // ok
    return tb_true;
}
#ifdef TB_SSL_ENABLE
static tb_bool_t tb_async_stream_sock_sread_func(tb_handle_t ssl, tb_size_t state, tb_byte_t* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(ssl, tb_false);

    // the stream
    tb_async_stream_sock_t* sstream = (tb_async_stream_sock_t*)priv;
    tb_assert_and_check_return_val(sstream && sstream->func.read, tb_false);
 
    // trace
    tb_trace_d("srecv: real: %lu, size: %lu, state: %s", real, size, tb_state_cstr(state));

    // save offset
    if (state == TB_STATE_OK) tb_atomic64_fetch_and_add(&sstream->offset, real);

    // done func
    if (sstream->func.read((tb_async_stream_t*)sstream, state, data, real, size, sstream->priv))
    {
        // ok? continue it
        if (state == TB_STATE_OK)
        {
            // continue to post read
            if (!tb_aicp_ssl_read(sstream->hssl, data, size, tb_async_stream_sock_sread_func, (tb_async_stream_t*)sstream))
                sstream->func.read((tb_async_stream_t*)sstream, TB_STATE_SOCK_SSL_READ_FAILED, data, 0, size, sstream->priv);
        }
    }

    // ok
    return tb_true;
}
#endif
static tb_bool_t tb_async_stream_sock_read(tb_handle_t astream, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return_val(sstream && sstream->sock && sstream->aico && data && size && func, tb_false);

    // clear the offset if be writ mode now
    if (!sstream->bread) tb_atomic64_set0(&sstream->offset);

    // set read mode
    sstream->bread = 1;

    // save func and priv
    sstream->priv       = priv;
    sstream->func.read  = func;

    // done
    tb_bool_t ok = tb_false;
    switch (sstream->type)
    {
    case TB_SOCKET_TYPE_TCP:
        {
#ifdef TB_SSL_ENABLE
            // ssl?
            if (tb_url_ssl_get(&sstream->base.base.url))
            {
                // check
                tb_assert_and_check_break(sstream->hssl);

                // post ssl read
                ok = tb_aicp_ssl_read_after(sstream->hssl, delay, data, size, tb_async_stream_sock_sread_func, astream);
            }
            // post tcp read
            else
#endif
            {
                ok = tb_aico_recv_after(sstream->aico, delay, data, size, tb_async_stream_sock_read_func, astream);
            }
        }
        break;
    case TB_SOCKET_TYPE_UDP:
        {
            // post udp read
            ok = tb_aico_urecv_after(sstream->aico, delay, &sstream->ipv4, tb_url_port_get(&sstream->base.base.url), data, size, tb_async_stream_sock_uread_func, astream);
        }
        break;
    default:
        tb_trace_e("unknown socket type: %lu", sstream->type);
        break;
    }

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_sock_writ_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_SEND, tb_false);

    // the stream
    tb_async_stream_sock_t* sstream = (tb_async_stream_sock_t*)aice->priv;
    tb_assert_and_check_return_val(sstream && sstream->func.writ, tb_false);

    // trace
    tb_trace_d("send: real: %lu, size: %lu, state: %s", aice->u.send.real, aice->u.send.size, tb_state_cstr(aice->state));

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        tb_assert_and_check_break(aice->u.send.data && aice->u.send.real <= aice->u.send.size);
        tb_atomic64_fetch_and_add(&sstream->offset, aice->u.send.real);
        state = TB_STATE_OK;
        break;
        // closed
    case TB_STATE_CLOSED:
        state = TB_STATE_CLOSED;
        break;
        // killed
    case TB_STATE_KILLED:
        state = TB_STATE_KILLED;
        break;
        // timeout?
    case TB_STATE_TIMEOUT:
        state = TB_STATE_SOCK_SEND_TIMEOUT;
        break;
    default:
        tb_trace_d("writ: unknown state: %s", tb_state_cstr(aice->state));
        break;
    }
 
    // done func
    if (sstream->func.writ((tb_async_stream_t*)sstream, state, aice->u.send.data, aice->u.send.real, aice->u.send.size, sstream->priv))
    {
        // continue?
        if (aice->state == TB_STATE_OK && aice->u.send.real < aice->u.send.size)
        {
            // continue to post writ
            if (!tb_aico_send(aice->aico, aice->u.send.data + aice->u.send.real, aice->u.send.size - aice->u.send.real, tb_async_stream_sock_writ_func, (tb_async_stream_t*)sstream))
                sstream->func.writ((tb_async_stream_t*)sstream, TB_STATE_SOCK_SEND_FAILED, aice->u.usend.data, 0, aice->u.usend.size, sstream->priv);
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_sock_uwrit_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_USEND, tb_false);

    // the stream
    tb_async_stream_sock_t* sstream = (tb_async_stream_sock_t*)aice->priv;
    tb_assert_and_check_return_val(sstream && sstream->func.writ, tb_false);

    // trace
    tb_trace_d("usend: real: %lu, size: %lu, state: %s", aice->u.usend.real, aice->u.usend.size, tb_state_cstr(aice->state));

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        tb_assert_and_check_break(aice->u.usend.data && aice->u.usend.real <= aice->u.usend.size);
        tb_atomic64_fetch_and_add(&sstream->offset, aice->u.usend.real);
        state = TB_STATE_OK;
        break;
        // closed
    case TB_STATE_CLOSED:
        state = TB_STATE_CLOSED;
        break;
        // killed
    case TB_STATE_KILLED:
        state = TB_STATE_KILLED;
        break;
        // timeout?
    case TB_STATE_TIMEOUT:
        state = TB_STATE_SOCK_SEND_TIMEOUT;
        break;
    default:
        tb_trace_d("writ: unknown state: %s", tb_state_cstr(aice->state));
        break;
    }
 
    // done func
    if (sstream->func.writ((tb_async_stream_t*)sstream, state, aice->u.usend.data, aice->u.usend.real, aice->u.usend.size, sstream->priv))
    {
        // continue?
        if (aice->state == TB_STATE_OK && aice->u.usend.real < aice->u.usend.size)
        {
            // continue to post writ
            if (!tb_aico_usend(aice->aico, &aice->u.usend.addr, aice->u.usend.port, aice->u.usend.data + aice->u.usend.real, aice->u.usend.size - aice->u.usend.real, tb_async_stream_sock_uwrit_func, (tb_async_stream_t*)sstream))
                sstream->func.writ((tb_async_stream_t*)sstream, TB_STATE_SOCK_SEND_FAILED, aice->u.usend.data, 0, aice->u.usend.size, sstream->priv);
        }
    }

    // ok
    return tb_true;
}
#ifdef TB_SSL_ENABLE
static tb_bool_t tb_async_stream_sock_swrit_func(tb_handle_t ssl, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(ssl, tb_false);

    // the stream
    tb_async_stream_sock_t* sstream = (tb_async_stream_sock_t*)priv;
    tb_assert_and_check_return_val(sstream && sstream->func.writ, tb_false);
 
    // trace
    tb_trace_d("ssend: real: %lu, size: %lu, state: %s", real, size, tb_state_cstr(state));

    // save offset
    if (state == TB_STATE_OK) tb_atomic64_fetch_and_add(&sstream->offset, real);

    // done func
    if (sstream->func.writ((tb_async_stream_t*)sstream, state, data, real, size, sstream->priv))
    {
        // ok? continue it
        if (state == TB_STATE_OK && real < size)
        {
            // continue to post writ
            if (!tb_aicp_ssl_writ(sstream->hssl, data, size, tb_async_stream_sock_swrit_func, (tb_async_stream_t*)sstream))
                sstream->func.writ((tb_async_stream_t*)sstream, TB_STATE_SOCK_SSL_WRIT_FAILED, data, 0, size, sstream->priv);
        }
    }

    // ok
    return tb_true;
}
#endif
static tb_bool_t tb_async_stream_sock_writ(tb_handle_t astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return_val(sstream && sstream->sock && sstream->aico && data && size && func, tb_false);

    // clear the offset if be read mode now
    if (sstream->bread) tb_atomic64_set0(&sstream->offset);

    // set writ mode
    sstream->bread = 0;

    // save func and priv
    sstream->priv       = priv;
    sstream->func.writ  = func;

    // done
    tb_bool_t ok = tb_false;
    switch (sstream->type)
    {
    case TB_SOCKET_TYPE_TCP:
        {
#ifdef TB_SSL_ENABLE
            // ssl?
            if (tb_url_ssl_get(&sstream->base.base.url))
            {
                // check
                tb_assert_and_check_break(sstream->hssl);

                // post ssl writ
                ok = tb_aicp_ssl_writ_after(sstream->hssl, delay, data, size, tb_async_stream_sock_swrit_func, astream);
            }
            // post tcp writ
            else 
#endif
            {
                ok = tb_aico_send_after(sstream->aico, delay, data, size, tb_async_stream_sock_writ_func, astream);
            }
        }
        break;
    case TB_SOCKET_TYPE_UDP:
        {
            // post udp writ
            ok = tb_aico_usend_after(sstream->aico, delay, &sstream->ipv4, tb_url_port_get(&sstream->base.base.url), data, size, tb_async_stream_sock_uwrit_func, astream);
        }
        break;
    default:
        tb_trace_e("unknown socket type: %lu", sstream->type);
        break;
    }

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_sock_seek(tb_handle_t astream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return_val(sstream && func, tb_false);

    // done func
    func(astream, TB_STATE_NOT_SUPPORTED, 0, priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_sock_sync(tb_handle_t astream, tb_bool_t bclosing, tb_async_stream_sync_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return_val(sstream && sstream->sock && sstream->aico && func, tb_false);

    // done func
    func(astream, TB_STATE_OK, bclosing, priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_sock_task_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the stream
    tb_async_stream_sock_t* sstream = (tb_async_stream_sock_t*)aice->priv;
    tb_assert_and_check_return_val(sstream && sstream->func.task, tb_false);

    // done func
    tb_bool_t ok = sstream->func.task((tb_async_stream_t*)sstream, aice->state, sstream->priv);

    // ok and continue?
    if (ok && aice->state == TB_STATE_OK)
    {
        // post task
        tb_aico_task_run(aice->aico, aice->u.runtask.delay, tb_async_stream_sock_task_func, sstream);
    }

    // ok
    return tb_true;
}
#ifdef TB_SSL_ENABLE
static tb_bool_t tb_async_stream_sock_stask_func(tb_handle_t ssl, tb_size_t state, tb_size_t delay, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(ssl, tb_false);

    // the stream
    tb_async_stream_sock_t* sstream = (tb_async_stream_sock_t*)priv;
    tb_assert_and_check_return_val(sstream && sstream->func.task, tb_false);

    // done func
    tb_bool_t ok = sstream->func.task((tb_async_stream_t*)sstream, state, sstream->priv);

    // ok and continue?
    if (ok && state == TB_STATE_OK)
    {
        // post task
        tb_aicp_ssl_task(ssl, delay, tb_async_stream_sock_stask_func, sstream);
    }

    // ok
    return tb_true;
}
#endif
static tb_bool_t tb_async_stream_sock_task(tb_handle_t astream, tb_size_t delay, tb_async_stream_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return_val(sstream && sstream->sock && func, tb_false);

    // save func and priv
    sstream->priv       = priv;
    sstream->func.task  = func;

    // post task
    if (sstream->aico) return tb_aico_task_run(sstream->aico, delay, tb_async_stream_sock_task_func, astream);
#ifdef TB_SSL_ENABLE
    else if (sstream->hssl) return tb_aicp_ssl_task(sstream->hssl, delay, tb_async_stream_sock_stask_func, astream);
#endif

    // failed
    tb_trace_e("cannot run task!");
    return tb_false;
}
static tb_void_t tb_async_stream_sock_kill(tb_handle_t astream)
{   
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return(sstream);

    // kill aico
    if (sstream->aico) tb_aico_kill(sstream->aico);
    // kill addr
    else if (sstream->hdns) tb_aicp_dns_kill(sstream->hdns);
}
static tb_bool_t tb_async_stream_sock_exit(tb_handle_t astream)
{   
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return_val(sstream, tb_false);

    // aico has been not closed already?
    tb_assert_and_check_return_val(!sstream->aico, tb_false);

    // dns has been not closed already?
    tb_assert_and_check_return_val(!sstream->hdns, tb_false);

#ifdef TB_SSL_ENABLE
    // ssl has been not closed already?
    tb_assert_and_check_return_val(!sstream->hssl, tb_false);
#endif

    // sock has been not closed already?
    tb_assert_and_check_return_val(!sstream->sock, tb_false);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_sock_ctrl(tb_handle_t astream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_async_stream_sock_t* sstream = tb_async_stream_sock_cast(astream);
    tb_assert_and_check_return_val(sstream, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_opened(astream), tb_false);

            // get offset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);
            *poffset = (tb_hize_t)tb_atomic64_get(&sstream->offset);
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_SET_TYPE:
        {
            // check
            tb_assert_and_check_return_val(!tb_stream_is_opened(astream), tb_false);

            // the type
            tb_size_t type = (tb_size_t)tb_va_arg(args, tb_size_t);
            tb_assert_and_check_return_val(type == TB_SOCKET_TYPE_TCP || type == TB_SOCKET_TYPE_UDP, tb_false);

            // changed? exit the old sock
            if (sstream->type != type)
            {
                // exit aico
                if (sstream->aico) tb_aico_exit(sstream->aico, tb_null, tb_null);
                sstream->aico = tb_null;

                // exit it
                if (!sstream->bref && sstream->sock) tb_socket_clos(sstream->sock);
                sstream->sock = tb_null;
                sstream->bref = 0;
            }

            // set type
            sstream->type = type;

            // ok
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_GET_TYPE:
        {
            tb_size_t* ptype = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(ptype, tb_false);
            *ptype = sstream->type;
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_SET_HANDLE:
        {
            // check
            tb_assert_and_check_return_val(!tb_stream_is_opened(astream), tb_false);

            // the sock
            tb_handle_t sock = (tb_handle_t)tb_va_arg(args, tb_handle_t);

            // changed? exit the old sock
            if (sstream->sock != sock)
            {
                // exit aico
                if (sstream->aico) tb_aico_exit(sstream->aico, tb_null, tb_null);
                sstream->aico = tb_null;

                // exit it
                if (!sstream->bref && sstream->sock) tb_socket_clos(sstream->sock);
            }

            // set sock
            sstream->sock = sock;
            sstream->bref = sock? 1 : 0;

            // ok
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_GET_HANDLE:
        {
            // get handle
            tb_handle_t* phandle = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
            tb_assert_and_check_return_val(phandle, tb_false);
            *phandle = sstream->sock;
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_KEEP_ALIVE:
        {
            // keep alive?
            tb_bool_t balived = (tb_bool_t)tb_va_arg(args, tb_bool_t);
            sstream->balived = balived? 1 : 0;
            return tb_true;
        }
    default:
        break;
    }
    return tb_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_async_stream_t* tb_async_stream_init_sock(tb_aicp_t* aicp)
{
    // check
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_async_stream_sock_t* sstream = tb_null;
    do
    {
        // make stream
        sstream = (tb_async_stream_sock_t*)tb_malloc0(sizeof(tb_async_stream_sock_t));
        tb_assert_and_check_break(sstream);

        // init stream
        if (!tb_async_stream_init((tb_async_stream_t*)sstream, aicp, TB_STREAM_TYPE_SOCK, TB_BASIC_STREAM_SOCK_CACHE_MAXN, TB_BASIC_STREAM_SOCK_CACHE_MAXN)) break;
        sstream->base.open      = tb_async_stream_sock_open;
        sstream->base.read      = tb_async_stream_sock_read;
        sstream->base.writ      = tb_async_stream_sock_writ;
        sstream->base.seek      = tb_async_stream_sock_seek;
        sstream->base.sync      = tb_async_stream_sock_sync;
        sstream->base.task      = tb_async_stream_sock_task;
        sstream->base.clos      = tb_async_stream_sock_clos;
        sstream->base.exit      = tb_async_stream_sock_exit;
        sstream->base.base.kill = tb_async_stream_sock_kill;
        sstream->base.base.ctrl = tb_async_stream_sock_ctrl;
        sstream->type           = TB_SOCKET_TYPE_TCP;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (sstream) tb_async_stream_exit((tb_async_stream_t*)sstream);
        sstream = tb_null;
    }

    // ok
    return (tb_async_stream_t*)sstream;
}
tb_async_stream_t* tb_async_stream_init_from_sock(tb_aicp_t* aicp, tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
    // check
    tb_assert_and_check_return_val(aicp && host && port, tb_null);

    // ssl is not supported now.
    tb_assert_and_check_return_val(!bssl, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  sstream = tb_null;
    do
    {
        // init stream
        sstream = tb_async_stream_init_sock(aicp);
        tb_assert_and_check_return_val(sstream, tb_null);

        // ctrl stream
        if (!tb_stream_ctrl(sstream, TB_STREAM_CTRL_SET_HOST, host)) break;
        if (!tb_stream_ctrl(sstream, TB_STREAM_CTRL_SET_PORT, port)) break;
        if (!tb_stream_ctrl(sstream, TB_STREAM_CTRL_SET_SSL, bssl)) break;
        if (!tb_stream_ctrl(sstream, TB_STREAM_CTRL_SOCK_SET_TYPE, type)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (sstream) tb_async_stream_exit(sstream);
        sstream = tb_null;
    }

    // ok
    return sstream;
}
