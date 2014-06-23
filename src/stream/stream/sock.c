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
 * \sock        sock.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "stream_sock"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../asio/asio.h"
#include "../../string/string.h"
#include "../../network/network.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the sock cache maxn
#ifdef __tb_small__
#   define TB_STREAM_SOCK_CACHE_MAXN  (8192)
#else
#   define TB_STREAM_SOCK_CACHE_MAXN  (8192 << 1)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the sock stream type
typedef struct __tb_stream_impl_t
{
    // the base
    tb_stream_t             base;

    // the sock handle
    tb_handle_t             sock;

#ifdef TB_SSL_ENABLE
    // the ssl handle
    tb_handle_t             hssl;
#endif

    // the sock type
    tb_uint32_t             type    : 22;

    // the try number
    tb_uint32_t             tryn    : 8;

    // the sock bref
    tb_uint32_t             bref    : 1;

    // keep alive after being closed?
    tb_uint32_t             balived : 1;

    // the wait event
    tb_long_t               wait;

    // the read and writ
    tb_size_t               read;
    tb_size_t               writ;
    
    // the host address
    tb_ipv4_t               addr;

}tb_stream_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_stream_impl_t* tb_stream_impl_cast(tb_stream_t* stream)
{
    tb_assert_and_check_return_val(stream && stream->type == TB_STREAM_TYPE_SOCK, tb_null);
    return (tb_stream_impl_t*)stream;
}
static tb_bool_t tb_stream_impl_open(tb_stream_t* stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->type, tb_false);

    // clear
    impl->wait = 0;
    impl->tryn = 0;
    impl->read = 0;
    impl->writ = 0;

    // opened?
    tb_check_return_val(!impl->sock, tb_true);

#ifndef TB_SSL_ENABLE
    // ssl? not supported
    if (tb_url_ssl_get(&impl->base.url))
    {
        // trace
        tb_trace_w("ssl is not supported now! please enable it from config if you need it.");

        // save state
        impl->base.state = TB_STATE_SOCK_SSL_NOT_SUPPORTED;
        return tb_false;
    }
#endif

    // port
    tb_size_t port = tb_url_port_get(&impl->base.url);
    tb_assert_and_check_return_val(port, tb_false);

    // ipv4
    if (!impl->addr.u32)
    {
        // try to get the ipv4 address from url
        tb_ipv4_t const* ipv4 = tb_url_ipv4_get(&impl->base.url);
        if (ipv4 && ipv4->u32) impl->addr = *ipv4;
        else
        {
            // look addr
            if (!tb_dns_looker_done(tb_url_host_get(&impl->base.url), &impl->addr)) 
            {
                impl->base.state = TB_STATE_SOCK_DNS_FAILED;
                return tb_false;
            }

            // save addr
            tb_url_ipv4_set(&impl->base.url, &impl->addr);
        }

        // tcp or udp? for url: sock://ip:port/?udp=
        tb_char_t const* args = tb_url_args_get(&impl->base.url);
        if (args && !tb_strnicmp(args, "udp=", 4)) impl->type = TB_SOCKET_TYPE_UDP;
        else if (args && !tb_strnicmp(args, "tcp=", 4)) impl->type = TB_SOCKET_TYPE_TCP;
    }

    // make sock
    impl->sock = tb_socket_open(impl->type);
    impl->bref = 0;
    
    // open sock failed?
    if (!impl->sock)
    {
        // trace
        tb_trace_e("open sock failed!");

        // save state
        impl->base.state = TB_STATE_SOCK_OPEN_FAILED;
        return tb_false;
    }

    // resize cache 
    tb_size_t recv_cache = tb_socket_recv_buffer_size(impl->sock);
    tb_size_t send_cache = tb_socket_send_buffer_size(impl->sock);
    if (recv_cache || send_cache) tb_queue_buffer_resize(&impl->base.cache, tb_max(recv_cache, send_cache));

    // done
    tb_bool_t ok = tb_false;
    switch (impl->type)
    {
    case TB_SOCKET_TYPE_TCP:
        {
            // trace
            tb_trace_d("connect: %s[%u.%u.%u.%u]:%u: ..", tb_url_host_get(&impl->base.url), tb_ipv4_u8x4(impl->addr), port);

            // connect it
            tb_long_t real = -1;
            while (     !(real = tb_socket_connect(impl->sock, &impl->addr, port))
                    &&  TB_STATE_KILLING != tb_atomic_get(&impl->base.istate))
            {
                real = tb_aioo_wait(impl->sock, TB_AIOE_CODE_CONN, tb_stream_timeout(stream));
                tb_check_break(real > 0);
            }

            // ok?
            if (real > 0)
            {
                ok = tb_true;
                impl->base.state = TB_STATE_OK;
            }
            else impl->base.state = !real? TB_STATE_SOCK_CONNECT_TIMEOUT : TB_STATE_SOCK_CONNECT_FAILED;

            // trace
            tb_trace_d("connect: %s", ok? "ok" : "no");
            
            // ok?
            if (ok)
            {
                // ssl? init it
                if (tb_url_ssl_get(&impl->base.url))
                {
#ifdef TB_SSL_ENABLE
                    // done
                    ok = tb_false;
                    do
                    {
                        // init ssl
                        if (!impl->hssl) impl->hssl = tb_ssl_init(tb_false);
                        tb_assert_and_check_break(impl->hssl);

                        // init bio
                        tb_ssl_set_bio_sock(impl->hssl, impl->sock);

                        // init timeout
                        tb_ssl_set_timeout(impl->hssl, tb_stream_timeout(stream));

                        // open ssl
                        if (!tb_ssl_open(impl->hssl)) break;

                        // ok
                        ok = tb_true;

                    } while (0);

                    // trace
                    tb_trace_d("ssl: %s", ok? "ok" : "no");
            
                    // failed? save state
                    if (!ok) impl->base.state = impl->hssl? tb_ssl_state(impl->hssl) : TB_STATE_SOCK_SSL_FAILED;
#endif
                }
            }
        }
        break;
    case TB_SOCKET_TYPE_UDP:
        {
            // ssl? not supported
            if (tb_url_ssl_get(&impl->base.url))
            {
                // trace
                tb_trace_w("udp ssl is not supported!");

                // save state
                impl->base.state = TB_STATE_SOCK_SSL_NOT_SUPPORTED;
            }
            else
            {
                // ok
                ok = tb_true;
                impl->base.state = TB_STATE_OK;
            }
        }
        break;
    default:
        tb_trace_e("unknown socket type: %lu", impl->type);
        break;
    }

    // ok?
    return ok;
}
static tb_bool_t tb_stream_impl_clos(tb_stream_t* stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

#ifdef TB_SSL_ENABLE
    // close ssl
    if (tb_url_ssl_get(&impl->base.url) && impl->hssl)
        tb_ssl_clos(impl->hssl);
#endif

    // keep alive? not close it
    tb_check_return_val(!impl->balived, tb_true);

    // close sock
    if (!impl->bref) 
    {
        if (impl->sock && !tb_socket_clos(impl->sock)) return tb_false;
        impl->sock = tb_null;
    }

    // clear 
    impl->wait = 0;
    impl->tryn = 0;
    impl->read = 0;
    impl->writ = 0;
    tb_ipv4_clr(&impl->addr);

    // ok
    return tb_true;
}
static tb_void_t tb_stream_impl_exit(tb_stream_t* stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl_cast(stream);
    tb_assert_and_check_return(impl);

#ifdef TB_SSL_ENABLE
    // exit ssl
    if (impl->hssl) tb_ssl_exit(impl->hssl);
    impl->hssl = tb_null;
#endif

    // close sock
    if (!impl->bref) 
    {
        if (impl->sock && !tb_socket_clos(impl->sock))
        {
            // trace
            tb_trace_d("[sock]: exit failed");
        }
        impl->sock = tb_null;
    }

    // clear 
    impl->wait = 0;
    impl->tryn = 0;
    impl->read = 0;
    impl->writ = 0;
    tb_ipv4_clr(&impl->addr);
}
static tb_void_t tb_stream_impl_kill(tb_stream_t* stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl_cast(stream);
    tb_assert_and_check_return(impl);

    // kill it
    if (impl->sock) tb_socket_kill(impl->sock, TB_SOCKET_KILL_RW);
}
static tb_long_t tb_stream_impl_read(tb_stream_t* stream, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->sock, -1);

    // check
    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    // clear writ
    impl->writ = 0;

    // read
    tb_long_t real = -1;
    switch (impl->type)
    {
    case TB_SOCKET_TYPE_TCP:
        {
#ifdef TB_SSL_ENABLE
            // ssl?
            if (tb_url_ssl_get(&impl->base.url))
            {
                // check
                tb_assert_and_check_return_val(impl->hssl, -1);
    
                // read data
                real = tb_ssl_read(impl->hssl, data, size);

                // trace
                tb_trace_d("read: %ld <? %lu", real, size);

                // failed or closed?
                tb_check_return_val(real >= 0, -1);
            }
            else
#endif
            {
                // read data
                real = tb_socket_recv(impl->sock, data, size);

                // trace
                tb_trace_d("read: %ld <? %lu", real, size);

                // failed or closed?
                tb_check_return_val(real >= 0, -1);

                // peer closed?
                if (!real && impl->wait > 0 && (impl->wait & TB_AIOE_CODE_RECV)) return -1;

                // clear wait
                if (real > 0) impl->wait = 0;
            }
        }
        break;
    case TB_SOCKET_TYPE_UDP:
        {
            // port
            tb_size_t port = tb_url_port_get(&impl->base.url);
            tb_assert_and_check_return_val(port, -1);

            // ipv4
            tb_assert_and_check_return_val(impl->addr.u32, -1);

            // read data
            real = tb_socket_urecv(impl->sock, &impl->addr, port, data, size);

            // trace
            tb_trace_d("read: %ld <? %lu", real, size);

            // failed or closed?
            tb_check_return_val(real >= 0, -1);

            // peer closed?
            if (!real && impl->wait > 0 && (impl->wait & TB_AIOE_CODE_RECV)) return -1;

            // clear wait
            if (real > 0) impl->wait = 0;
        }
        break;
    default:
        break;
    }

    // update read
    if (real > 0) impl->read += real;

    // ok?
    return real;
}
static tb_long_t tb_stream_impl_writ(tb_stream_t* stream, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->sock, -1);

    // check
    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    // clear read
    impl->read = 0;

    // writ 
    tb_long_t real = -1;
    switch (impl->type)
    {
    case TB_SOCKET_TYPE_TCP:
        {
#ifdef TB_SSL_ENABLE
            // ssl?
            if (tb_url_ssl_get(&impl->base.url))
            {
                // check
                tb_assert_and_check_return_val(impl->hssl, -1);

                // writ data
                real = tb_ssl_writ(impl->hssl, data, size);

                // trace
                tb_trace_d("writ: %ld <? %lu", real, size);

                // failed or closed?
                tb_check_return_val(real >= 0, -1);
            }
            else
#endif
            {
                // writ data
                real = tb_socket_send(impl->sock, data, size);

                // trace
                tb_trace_d("writ: %ld <? %lu", real, size);

                // failed or closed?
                tb_check_return_val(real >= 0, -1);

                // peer closed?
                if (!real && impl->wait > 0 && (impl->wait & TB_AIOE_CODE_SEND)) return -1;

                // clear wait
                if (real > 0) impl->wait = 0;
            }
        }
        break;
    case TB_SOCKET_TYPE_UDP:
        {
            // port
            tb_size_t port = tb_url_port_get(&impl->base.url);
            tb_assert_and_check_return_val(port, -1);

            // ipv4
            tb_assert_and_check_return_val(impl->addr.u32, -1);

            // writ data
            real = tb_socket_usend(impl->sock, &impl->addr, port, data, size);

            // trace
            tb_trace_d("writ: %ld <? %lu", real, size);

            // failed or closed?
            tb_check_return_val(real >= 0, -1);

            // no data?
            if (!real)
            {
                // abort? writ x, writ 0, or writ 0, writ 0
                tb_check_return_val(!impl->writ && !impl->tryn, -1);

                // tryn++
                impl->tryn++;
            }
            else impl->tryn = 0;
        }
        break;
    default:
        break;
    }

    // update writ
    if (real > 0) impl->writ += real;

    // ok?
    return real;
}
static tb_long_t tb_stream_impl_wait(tb_stream_t* stream, tb_size_t wait, tb_long_t timeout)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->sock, -1);

#ifdef TB_SSL_ENABLE
    // ssl?
    if (tb_url_ssl_get(&impl->base.url))
    {
        // check
        tb_assert_and_check_return_val(impl->hssl, -1);

        // wait 
        impl->wait = tb_ssl_wait(impl->hssl, wait, timeout);

        // timeout or failed? save state
        if (impl->wait <= 0) impl->base.state = tb_ssl_state(impl->hssl);
    }
    else
#endif
    {
        // wait 
        impl->wait = tb_aioo_wait(impl->sock, wait, timeout);
    }

    // trace
    tb_trace_d("wait: %ld", impl->wait);

    // ok?
    return impl->wait;
}
static tb_bool_t tb_stream_impl_ctrl(tb_stream_t* stream, tb_size_t ctrl, tb_va_list_t args)
{
    tb_stream_impl_t* impl = tb_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // the poffset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);

            // get offset
            *poffset = impl->base.offset;
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_SET_TYPE:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_closed(stream), tb_false);

            // the type
            tb_size_t type = (tb_size_t)tb_va_arg(args, tb_size_t);
            tb_assert_and_check_return_val(type == TB_SOCKET_TYPE_TCP || type == TB_SOCKET_TYPE_UDP, tb_false);
            
            // changed? exit the old sock
            if (impl->type != type)
            {
                // exit it
                if (!impl->bref && impl->sock) tb_socket_clos(impl->sock);
                impl->sock = tb_null;
                impl->bref = 0;
            }

            // set type
            impl->type = type;

            // ok
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_GET_TYPE:
        {
            tb_size_t* ptype = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(ptype, tb_false);
            *ptype = impl->type;
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_SET_HANDLE:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_closed(stream), tb_false);
            
            // the sock
            tb_handle_t sock = (tb_handle_t)tb_va_arg(args, tb_handle_t);

            // changed? exit the old sock
            if (impl->sock != sock)
            {
                // exit it
                if (!impl->bref && impl->sock) tb_socket_clos(impl->sock);
            }

            // set sock
            impl->sock = sock;
            impl->bref = sock? 1 : 0;

            // ok
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_GET_HANDLE:
        {
            tb_handle_t* phandle = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
            tb_assert_and_check_return_val(phandle, tb_false);
            *phandle = impl->sock;
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_KEEP_ALIVE:
        {
            // keep alive?
            tb_bool_t balived = (tb_bool_t)tb_va_arg(args, tb_bool_t);
            impl->balived = balived? 1 : 0;
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

tb_stream_t* tb_stream_init_sock()
{
    // done
    tb_bool_t           ok = tb_false;
    tb_stream_impl_t*   stream = tb_null;
    do
    {
        // make stream
        stream = (tb_stream_impl_t*)tb_malloc0(sizeof(tb_stream_impl_t));
        tb_assert_and_check_break(stream);

        // init stream
        if (!tb_stream_init((tb_stream_t*)stream, TB_STREAM_TYPE_SOCK, TB_STREAM_SOCK_CACHE_MAXN)) break;
        stream->base.open      = tb_stream_impl_open;
        stream->base.clos      = tb_stream_impl_clos;
        stream->base.exit      = tb_stream_impl_exit;
        stream->base.read      = tb_stream_impl_read;
        stream->base.writ      = tb_stream_impl_writ;
        stream->base.wait      = tb_stream_impl_wait;
        stream->base.ctrl      = tb_stream_impl_ctrl;
        stream->base.kill      = tb_stream_impl_kill;
        stream->sock           = tb_null;
        stream->type           = TB_SOCKET_TYPE_TCP;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) tb_stream_exit((tb_stream_t*)stream);
        stream = tb_null;
    }

    // ok?
    return (tb_stream_t*)stream;
}
tb_stream_t* tb_stream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
    // check
    tb_assert_and_check_return_val(host && port, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_stream_t*    stream = tb_null;
    do
    {
        // init stream
        stream = tb_stream_init_sock();
        tb_assert_and_check_break(stream);

        // ctrl
        if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_HOST, host)) break;
        if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_PORT, port)) break;
        if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_SSL, bssl)) break;
        if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SOCK_SET_TYPE, type)) break;
   
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) tb_stream_exit(stream);
        stream = tb_null;
    }

    // ok?
    return stream;
}
