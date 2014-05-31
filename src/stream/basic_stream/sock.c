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
#define TB_TRACE_MODULE_NAME            "basic_stream_sock"
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
#   define TB_BASIC_STREAM_SOCK_CACHE_MAXN  (8192)
#else
#   define TB_BASIC_STREAM_SOCK_CACHE_MAXN  (8192 << 1)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the sock stream type
typedef struct __tb_basic_stream_sock_t
{
    // the base
    tb_basic_stream_t       base;

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

}tb_basic_stream_sock_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_basic_stream_sock_t* tb_basic_stream_sock_cast(tb_handle_t stream)
{
    tb_basic_stream_t* bstream = (tb_basic_stream_t*)stream;
    tb_assert_and_check_return_val(bstream && bstream->base.type == TB_STREAM_TYPE_SOCK, tb_null);
    return (tb_basic_stream_sock_t*)bstream;
}
static tb_bool_t tb_basic_stream_sock_open(tb_handle_t bstream)
{
    // check
    tb_basic_stream_sock_t* sstream = tb_basic_stream_sock_cast(bstream);
    tb_assert_and_check_return_val(sstream && sstream->type, tb_false);

    // clear
    sstream->wait = 0;
    sstream->tryn = 0;
    sstream->read = 0;
    sstream->writ = 0;

    // opened?
    tb_check_return_val(!sstream->sock, tb_true);

#ifndef TB_SSL_ENABLE
    // ssl? not supported
    if (tb_url_ssl_get(&sstream->base.base.url))
    {
        // trace
        tb_trace_w("ssl is not supported now! please enable it from config if you need it.");

        // save state
        sstream->base.state = TB_STATE_SOCK_SSL_NOT_SUPPORTED;
        return tb_false;
    }
#endif

    // port
    tb_size_t port = tb_url_port_get(&sstream->base.base.url);
    tb_assert_and_check_return_val(port, tb_false);

    // ipv4
    if (!sstream->addr.u32)
    {
        // try to get the ipv4 address from url
        tb_ipv4_t const* ipv4 = tb_url_ipv4_get(&sstream->base.base.url);
        if (ipv4 && ipv4->u32) sstream->addr = *ipv4;
        else
        {
            // look addr
            if (!tb_dns_looker_done(tb_url_host_get(&sstream->base.base.url), &sstream->addr)) 
            {
                sstream->base.state = TB_STATE_SOCK_DNS_FAILED;
                return tb_false;
            }

            // save addr
            tb_url_ipv4_set(&sstream->base.base.url, &sstream->addr);
        }

        // tcp or udp? for url: sock://ip:port/?udp=
        tb_char_t const* args = tb_url_args_get(&sstream->base.base.url);
        if (args && !tb_strnicmp(args, "udp=", 4)) sstream->type = TB_SOCKET_TYPE_UDP;
        else if (args && !tb_strnicmp(args, "tcp=", 4)) sstream->type = TB_SOCKET_TYPE_TCP;
    }

    // make sock
    sstream->sock = tb_socket_open(sstream->type);
    sstream->bref = 0;
    
    // open sock failed?
    if (!sstream->sock)
    {
        // trace
        tb_trace_e("open sock failed!");

        // save state
        sstream->base.state = TB_STATE_SOCK_OPEN_FAILED;
        return tb_false;
    }

    // resize cache 
    tb_size_t recv_cache = tb_socket_recv_buffer_size(sstream->sock);
    tb_size_t send_cache = tb_socket_send_buffer_size(sstream->sock);
    if (recv_cache || send_cache) tb_queue_buffer_resize(&sstream->base.cache, tb_max(recv_cache, send_cache));

    // done
    tb_bool_t ok = tb_false;
    switch (sstream->type)
    {
    case TB_SOCKET_TYPE_TCP:
        {
            // trace
            tb_trace_d("connect: %s[%u.%u.%u.%u]:%u: ..", tb_url_host_get(&sstream->base.base.url), tb_ipv4_u8x4(sstream->addr), port);

            // connect it
            tb_long_t real = -1;
            while (     !(real = tb_socket_connect(sstream->sock, &sstream->addr, port))
                    &&  TB_STATE_KILLING != tb_atomic_get(&sstream->base.base.istate))
            {
                real = tb_aioo_wait(sstream->sock, TB_AIOE_CODE_CONN, tb_stream_timeout(bstream));
                tb_check_break(real > 0);
            }

            // ok?
            if (real > 0)
            {
                ok = tb_true;
                sstream->base.state = TB_STATE_OK;
            }
            else sstream->base.state = !real? TB_STATE_SOCK_CONNECT_TIMEOUT : TB_STATE_SOCK_CONNECT_FAILED;

            // trace
            tb_trace_d("connect: %s", ok? "ok" : "no");
            
            // ok?
            if (ok)
            {
                // ssl? init it
                if (tb_url_ssl_get(&sstream->base.base.url))
                {
#ifdef TB_SSL_ENABLE
                    // done
                    ok = tb_false;
                    do
                    {
                        // init ssl
                        if (!sstream->hssl) sstream->hssl = tb_ssl_init(tb_false);
                        tb_assert_and_check_break(sstream->hssl);

                        // init bio
                        tb_ssl_set_bio_sock(sstream->hssl, sstream->sock);

                        // init timeout
                        tb_ssl_set_timeout(sstream->hssl, tb_stream_timeout(bstream));

                        // open ssl
                        if (!tb_ssl_open(sstream->hssl)) break;

                        // ok
                        ok = tb_true;

                    } while (0);

                    // trace
                    tb_trace_d("ssl: %s", ok? "ok" : "no");
            
                    // failed? save state
                    if (!ok) sstream->base.state = sstream->hssl? tb_ssl_state(sstream->hssl) : TB_STATE_SOCK_SSL_FAILED;
#endif
                }
            }
        }
        break;
    case TB_SOCKET_TYPE_UDP:
        {
            // ssl? not supported
            if (tb_url_ssl_get(&sstream->base.base.url))
            {
                // trace
                tb_trace_w("udp ssl is not supported!");

                // save state
                sstream->base.state = TB_STATE_SOCK_SSL_NOT_SUPPORTED;
            }
            else
            {
                // ok
                ok = tb_true;
                sstream->base.state = TB_STATE_OK;
            }
        }
        break;
    default:
        tb_trace_e("unknown socket type: %lu", sstream->type);
        break;
    }

    // ok?
    return ok;
}
static tb_bool_t tb_basic_stream_sock_clos(tb_handle_t bstream)
{
    // check
    tb_basic_stream_sock_t* sstream = tb_basic_stream_sock_cast(bstream);
    tb_assert_and_check_return_val(sstream, tb_false);

#ifdef TB_SSL_ENABLE
    // close ssl
    if (tb_url_ssl_get(&sstream->base.base.url) && sstream->hssl)
        tb_ssl_clos(sstream->hssl);
#endif

    // keep alive? not close it
    tb_check_return_val(!sstream->balived, tb_true);

    // close sock
    if (!sstream->bref) 
    {
        if (sstream->sock && !tb_socket_clos(sstream->sock)) return tb_false;
        sstream->sock = tb_null;
    }

    // clear 
    sstream->wait = 0;
    sstream->tryn = 0;
    sstream->read = 0;
    sstream->writ = 0;
    tb_ipv4_clr(&sstream->addr);

    // ok
    return tb_true;
}
static tb_void_t tb_basic_stream_sock_exit(tb_handle_t bstream)
{
    // check
    tb_basic_stream_sock_t* sstream = tb_basic_stream_sock_cast(bstream);
    tb_assert_and_check_return(sstream);

#ifdef TB_SSL_ENABLE
    // exit ssl
    if (sstream->hssl) tb_ssl_exit(sstream->hssl);
    sstream->hssl = tb_null;
#endif

    // close sock
    if (!sstream->bref) 
    {
        if (sstream->sock && !tb_socket_clos(sstream->sock))
        {
            // trace
            tb_trace_d("[sock]: exit failed");
        }
        sstream->sock = tb_null;
    }

    // clear 
    sstream->wait = 0;
    sstream->tryn = 0;
    sstream->read = 0;
    sstream->writ = 0;
    tb_ipv4_clr(&sstream->addr);
}
static tb_void_t tb_basic_stream_sock_kill(tb_handle_t bstream)
{
    // check
    tb_basic_stream_sock_t* sstream = tb_basic_stream_sock_cast(bstream);
    tb_assert_and_check_return(sstream);

    // kill it
    if (sstream->sock) tb_socket_kill(sstream->sock, TB_SOCKET_KILL_RW);
}
static tb_long_t tb_basic_stream_sock_read(tb_handle_t bstream, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_basic_stream_sock_t* sstream = tb_basic_stream_sock_cast(bstream);
    tb_assert_and_check_return_val(sstream && sstream->sock, -1);

    // check
    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    // clear writ
    sstream->writ = 0;

    // read
    tb_long_t real = -1;
    switch (sstream->type)
    {
    case TB_SOCKET_TYPE_TCP:
        {
#ifdef TB_SSL_ENABLE
            // ssl?
            if (tb_url_ssl_get(&sstream->base.base.url))
            {
                // check
                tb_assert_and_check_return_val(sstream->hssl, -1);
    
                // read data
                real = tb_ssl_read(sstream->hssl, data, size);

                // trace
                tb_trace_d("read: %ld <? %lu", real, size);

                // failed or closed?
                tb_check_return_val(real >= 0, -1);
            }
            else
#endif
            {
                // read data
                real = tb_socket_recv(sstream->sock, data, size);

                // trace
                tb_trace_d("read: %ld <? %lu", real, size);

                // failed or closed?
                tb_check_return_val(real >= 0, -1);

                // peer closed?
                if (!real && sstream->wait > 0 && (sstream->wait & TB_AIOE_CODE_RECV)) return -1;

                // clear wait
                if (real > 0) sstream->wait = 0;
            }
        }
        break;
    case TB_SOCKET_TYPE_UDP:
        {
            // port
            tb_size_t port = tb_url_port_get(&sstream->base.base.url);
            tb_assert_and_check_return_val(port, -1);

            // ipv4
            tb_assert_and_check_return_val(sstream->addr.u32, -1);

            // read data
            real = tb_socket_urecv(sstream->sock, &sstream->addr, port, data, size);

            // trace
            tb_trace_d("read: %ld <? %lu", real, size);

            // failed or closed?
            tb_check_return_val(real >= 0, -1);

            // peer closed?
            if (!real && sstream->wait > 0 && (sstream->wait & TB_AIOE_CODE_RECV)) return -1;

            // clear wait
            if (real > 0) sstream->wait = 0;
        }
        break;
    default:
        break;
    }

    // update read
    if (real > 0) sstream->read += real;

    // ok?
    return real;
}
static tb_long_t tb_basic_stream_sock_writ(tb_handle_t bstream, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_basic_stream_sock_t* sstream = tb_basic_stream_sock_cast(bstream);
    tb_assert_and_check_return_val(sstream && sstream->sock, -1);

    // check
    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    // clear read
    sstream->read = 0;

    // writ 
    tb_long_t real = -1;
    switch (sstream->type)
    {
    case TB_SOCKET_TYPE_TCP:
        {
#ifdef TB_SSL_ENABLE
            // ssl?
            if (tb_url_ssl_get(&sstream->base.base.url))
            {
                // check
                tb_assert_and_check_return_val(sstream->hssl, -1);

                // writ data
                real = tb_ssl_writ(sstream->hssl, data, size);

                // trace
                tb_trace_d("writ: %ld <? %lu", real, size);

                // failed or closed?
                tb_check_return_val(real >= 0, -1);
            }
            else
#endif
            {
                // writ data
                real = tb_socket_send(sstream->sock, data, size);

                // trace
                tb_trace_d("writ: %ld <? %lu", real, size);

                // failed or closed?
                tb_check_return_val(real >= 0, -1);

                // peer closed?
                if (!real && sstream->wait > 0 && (sstream->wait & TB_AIOE_CODE_SEND)) return -1;

                // clear wait
                if (real > 0) sstream->wait = 0;
            }
        }
        break;
    case TB_SOCKET_TYPE_UDP:
        {
            // port
            tb_size_t port = tb_url_port_get(&sstream->base.base.url);
            tb_assert_and_check_return_val(port, -1);

            // ipv4
            tb_assert_and_check_return_val(sstream->addr.u32, -1);

            // writ data
            real = tb_socket_usend(sstream->sock, &sstream->addr, port, data, size);

            // trace
            tb_trace_d("writ: %ld <? %lu", real, size);

            // failed or closed?
            tb_check_return_val(real >= 0, -1);

            // no data?
            if (!real)
            {
                // abort? writ x, writ 0, or writ 0, writ 0
                tb_check_return_val(!sstream->writ && !sstream->tryn, -1);

                // tryn++
                sstream->tryn++;
            }
            else sstream->tryn = 0;
        }
        break;
    default:
        break;
    }

    // update writ
    if (real > 0) sstream->writ += real;

    // ok?
    return real;
}
static tb_long_t tb_basic_stream_sock_wait(tb_handle_t bstream, tb_size_t wait, tb_long_t timeout)
{
    // check
    tb_basic_stream_sock_t* sstream = tb_basic_stream_sock_cast(bstream);
    tb_assert_and_check_return_val(sstream && sstream->sock, -1);

#ifdef TB_SSL_ENABLE
    // ssl?
    if (tb_url_ssl_get(&sstream->base.base.url))
    {
        // check
        tb_assert_and_check_return_val(sstream->hssl, -1);

        // wait 
        sstream->wait = tb_ssl_wait(sstream->hssl, wait, timeout);

        // timeout or failed? save state
        if (sstream->wait <= 0) sstream->base.state = tb_ssl_state(sstream->hssl);
    }
    else
#endif
    {
        // wait 
        sstream->wait = tb_aioo_wait(sstream->sock, wait, timeout);
    }

    // trace
    tb_trace_d("wait: %ld", sstream->wait);

    // ok?
    return sstream->wait;
}
static tb_bool_t tb_basic_stream_sock_ctrl(tb_handle_t bstream, tb_size_t ctrl, tb_va_list_t args)
{
    tb_basic_stream_sock_t* sstream = tb_basic_stream_sock_cast(bstream);
    tb_assert_and_check_return_val(sstream, tb_false);

    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // the poffset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);

            // get offset
            *poffset = sstream->base.offset;
            return tb_true;
        }
    case TB_STREAM_CTRL_SOCK_SET_TYPE:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_closed(bstream), tb_false);

            // the type
            tb_size_t type = (tb_size_t)tb_va_arg(args, tb_size_t);
            tb_assert_and_check_return_val(type == TB_SOCKET_TYPE_TCP || type == TB_SOCKET_TYPE_UDP, tb_false);
            
            // changed? exit the old sock
            if (sstream->type != type)
            {
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
            tb_assert_and_check_return_val(tb_stream_is_closed(bstream), tb_false);
            
            // the sock
            tb_handle_t sock = (tb_handle_t)tb_va_arg(args, tb_handle_t);

            // changed? exit the old sock
            if (sstream->sock != sock)
            {
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
            tb_handle_t* phandle = (tb_handle_t)tb_va_arg(args, tb_handle_t*);
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

tb_basic_stream_t* tb_basic_stream_init_sock()
{
    // make stream
    tb_basic_stream_sock_t* bstream = (tb_basic_stream_sock_t*)tb_malloc0(sizeof(tb_basic_stream_sock_t));
    tb_assert_and_check_return_val(bstream, tb_null);

    // init stream
    if (!tb_basic_stream_init((tb_basic_stream_t*)bstream, TB_STREAM_TYPE_SOCK, TB_BASIC_STREAM_SOCK_CACHE_MAXN)) goto fail;
    bstream->base.open      = tb_basic_stream_sock_open;
    bstream->base.clos      = tb_basic_stream_sock_clos;
    bstream->base.exit      = tb_basic_stream_sock_exit;
    bstream->base.read      = tb_basic_stream_sock_read;
    bstream->base.writ      = tb_basic_stream_sock_writ;
    bstream->base.wait      = tb_basic_stream_sock_wait;
    bstream->base.base.ctrl = tb_basic_stream_sock_ctrl;
    bstream->base.base.kill = tb_basic_stream_sock_kill;
    bstream->sock           = tb_null;
    bstream->type           = TB_SOCKET_TYPE_TCP;

    // ok
    return (tb_basic_stream_t*)bstream;

fail:
    if (bstream) tb_basic_stream_exit((tb_basic_stream_t*)bstream);
    return tb_null;
}

tb_basic_stream_t* tb_basic_stream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
    // check
    tb_assert_and_check_return_val(host && port, tb_null);

    // init stream
    tb_basic_stream_t* bstream = tb_basic_stream_init_sock();
    tb_assert_and_check_return_val(bstream, tb_null);

    // ctrl
    if (!tb_stream_ctrl(bstream, TB_STREAM_CTRL_SET_HOST, host)) goto fail;
    if (!tb_stream_ctrl(bstream, TB_STREAM_CTRL_SET_PORT, port)) goto fail;
    if (!tb_stream_ctrl(bstream, TB_STREAM_CTRL_SET_SSL, bssl)) goto fail;
    if (!tb_stream_ctrl(bstream, TB_STREAM_CTRL_SOCK_SET_TYPE, type)) goto fail;
    
    // ok
    return bstream;

fail:
    if (bstream) tb_basic_stream_exit(bstream);
    return tb_null;
}
