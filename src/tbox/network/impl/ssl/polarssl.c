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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        polarssl.c
 * @ingroup     network
 *
 */
#define TB_TRACE_MODULE_NAME            "polarssl"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "polarssl/polarssl.h"
#include "../../../asio/asio.h"
#include "../../../libc/libc.h"
#include "../../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the ssl impl type
typedef struct __tb_ssl_impl_t
{
    // the ssl context
    ssl_context         ssl;

    // the ssl entropy context
    entropy_context     entropy;

    // the ssl ctr drbg context
    ctr_drbg_context    ctr_drbg;

    // the ssl x509 crt
    x509_crt            x509_crt;

    // is opened?
    tb_bool_t           bopened;

    // the state
    tb_size_t           state;

    // the last wait
    tb_long_t           lwait;

    // the timeout
    tb_long_t           timeout;

    // the read func
    tb_ssl_func_read_t  read;

    // the writ func
    tb_ssl_func_writ_t  writ;

    // the wait func
    tb_ssl_func_wait_t  wait;

    // the priv data
    tb_cpointer_t       priv;

}tb_ssl_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_void_t tb_ssl_error(tb_char_t const* info, tb_long_t error)
{
#ifdef POLARSSL_ERROR_C
    tb_char_t error_info[256] = {0};
    polarssl_strerror(error, error_info, sizeof(error_info));
    tb_trace_e("%s: error: %ld, %s", info, error, error_info);
#else
    tb_trace_e("%s: error: %ld", info, error);
#endif
}
#if TB_TRACE_MODULE_DEBUG && defined(__tb_debug__)
static tb_void_t tb_ssl_trace_info(tb_pointer_t priv, tb_int_t level, tb_char_t const* info)
{
    // trace
    if (level < 1) tb_printf("%s", info);
}
#endif
static tb_long_t tb_ssl_sock_read(tb_cpointer_t priv, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(priv, -1);

    // recv it
    return tb_socket_recv((tb_socket_ref_t)priv, data, size);
}
static tb_long_t tb_ssl_sock_writ(tb_cpointer_t priv, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(priv, -1);

    // send it
    return tb_socket_send((tb_socket_ref_t)priv, data, size);
}
static tb_long_t tb_ssl_sock_wait(tb_cpointer_t priv, tb_size_t code, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(priv, -1);

    // wait it
    return tb_aioo_wait((tb_socket_ref_t)priv, code, timeout);
}
static tb_int_t tb_ssl_func_read(tb_pointer_t priv, tb_byte_t* data, size_t size)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)priv;
    tb_assert_and_check_return_val(impl && impl->read, -1);

    // recv it
    tb_long_t real = impl->read(impl->priv, data, (tb_size_t)size);

    // trace 
    tb_trace_d("read: %ld", real);

    // ok? clear wait
    if (real > 0) impl->lwait = 0;
    // peer closed?
    else if (!real && impl->lwait > 0 && (impl->lwait & TB_SOCKET_EVENT_RECV)) real = POLARSSL_ERR_NET_CONN_RESET;
    // no data? continue to read it
    else if (!real) real = POLARSSL_ERR_NET_WANT_READ;
    // failed?
    else real = POLARSSL_ERR_NET_RECV_FAILED;

    // ok?
    return (tb_int_t)real;
}
static tb_int_t tb_ssl_func_writ(tb_pointer_t priv, tb_byte_t const* data, size_t size)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)priv;
    tb_assert_and_check_return_val(impl && impl->writ, -1);

    // send it
    tb_long_t real = impl->writ(impl->priv, data, (tb_size_t)size);

    // trace 
    tb_trace_d("writ: %ld", real);

    // ok? clear wait
    if (real > 0) impl->lwait = 0;
    // peer closed?
    else if (!real && impl->lwait > 0 && (impl->lwait & TB_SOCKET_EVENT_SEND)) real = POLARSSL_ERR_NET_CONN_RESET;
    // no data? continue to writ
    else if (!real) real = POLARSSL_ERR_NET_WANT_WRITE;
    // failed?
    else real = POLARSSL_ERR_NET_SEND_FAILED;

    // ok?
    return (tb_int_t)real;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_ssl_ref_t tb_ssl_init(tb_bool_t bserver)
{
    // done
    tb_bool_t       ok = tb_false;
    tb_ssl_impl_t*  impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_ssl_impl_t);
        tb_assert_and_check_break(impl);

        // init timeout, 30s
        impl->timeout = 30000;

        // init ssl x509_crt
        x509_crt_init(&impl->x509_crt);

        // init ssl entropy context
        entropy_init(&impl->entropy);

        // init ssl ctr_drbg context
        tb_long_t r = 0;
        if ((r = ctr_drbg_init(&impl->ctr_drbg, entropy_func, &impl->entropy, tb_null, 0)))
        {
            tb_ssl_error("init ctr_drbg failed", r);
            break;
        }

#ifdef POLARSSL_CERTS_C
        if ((r = x509_crt_parse(&impl->x509_crt, (tb_byte_t const*)test_ca_list, tb_strlen(test_ca_list))))
        {
            tb_ssl_error("parse x509_crt failed", r);
            break;
        }
#endif

        // init ssl context
        if ((r = ssl_init(&impl->ssl)))
        {
            tb_ssl_error("init impl failed", r);
            break;
        }

        // init ssl endpoint
        ssl_set_endpoint(&impl->ssl, bserver? SSL_IS_SERVER : SSL_IS_CLIENT);

        // init ssl authmode: optional
        ssl_set_authmode(&impl->ssl, SSL_VERIFY_OPTIONAL);

        // init ssl ca chain
        ssl_set_ca_chain(&impl->ssl, &impl->x509_crt, tb_null, tb_null);

        // init ssl random generator
        ssl_set_rng(&impl->ssl, ctr_drbg_random, &impl->ctr_drbg);

        // enable ssl debug?
#if TB_TRACE_MODULE_DEBUG && defined(__tb_debug__)
        ssl_set_dbg(&impl->ssl, tb_ssl_trace_info, tb_null);
#endif

        // init state
        impl->state = TB_STATE_OK;

        // ok
        ok = tb_true;

    } while (0);

    // failed? exit it
    if (!ok)
    {
        if (impl) tb_ssl_exit((tb_ssl_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_ssl_ref_t)impl;
}
tb_void_t tb_ssl_exit(tb_ssl_ref_t ssl)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return(impl);

    // close it first
    tb_ssl_clos(ssl);

    // exit ssl x509_crt
    x509_crt_free(&impl->x509_crt);

    // exit ssl
    ssl_free(&impl->ssl);

    // exit ssl entropy
    entropy_free(&impl->entropy);

    // exit it
    tb_free(impl);
}
tb_void_t tb_ssl_set_bio_sock(tb_ssl_ref_t ssl, tb_socket_ref_t sock)
{
    // the impl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return(impl);

    // set bio: sock
    tb_ssl_set_bio_func(ssl, tb_ssl_sock_read, tb_ssl_sock_writ, tb_ssl_sock_wait, sock);
}
tb_void_t tb_ssl_set_bio_func(tb_ssl_ref_t ssl, tb_ssl_func_read_t read, tb_ssl_func_writ_t writ, tb_ssl_func_wait_t wait, tb_cpointer_t priv)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return(impl && read && writ);

    // save func
    impl->read = read;
    impl->writ = writ;
    impl->wait = wait;
    impl->priv = priv;

    // set bio: func
    ssl_set_bio(&impl->ssl, tb_ssl_func_read, impl, tb_ssl_func_writ, impl);
}
tb_void_t tb_ssl_set_timeout(tb_ssl_ref_t ssl, tb_long_t timeout)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return(impl);

    // save timeout
    impl->timeout = timeout;
}
tb_bool_t tb_ssl_open(tb_ssl_ref_t ssl)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl && impl->wait, tb_false);

    // open it
    tb_long_t ok = -1;
    while (!(ok = tb_ssl_open_try(ssl)))
    {
        // wait it
        ok = tb_ssl_wait(ssl, TB_SOCKET_EVENT_RECV | TB_SOCKET_EVENT_SEND, impl->timeout);
        tb_check_break(ok > 0);
    }

    // ok?
    return ok > 0? tb_true : tb_false;
}
tb_long_t tb_ssl_open_try(tb_ssl_ref_t ssl)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl, -1);

    // done
    tb_long_t ok = -1;
    do
    {
        // init state
        impl->state = TB_STATE_OK;

        // have been opened already?
        if (impl->bopened)
        {
            ok = 1;
            break;
        }

        // done handshake
        tb_long_t r = ssl_handshake(&impl->ssl);
        
        // trace
        tb_trace_d("open: handshake: %ld", r);

        // ok?
        if (!r) ok = 1;
        // peer closed
        else if (r == POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY || r == POLARSSL_ERR_NET_CONN_RESET)
        {
            tb_trace_d("open: handshake: closed: %ld", r);
            impl->state = TB_STATE_CLOSED;
        }
        // continue to wait it?
        else if (r == POLARSSL_ERR_NET_WANT_READ || r == POLARSSL_ERR_NET_WANT_WRITE)
        {
            // trace
            tb_trace_d("open: handshake: wait: %s: ..", r == POLARSSL_ERR_NET_WANT_READ? "read" : "writ");

            // continue it
            ok = 0;

            // save state
            impl->state = (r == POLARSSL_ERR_NET_WANT_READ)? TB_STATE_SOCK_SSL_WANT_READ : TB_STATE_SOCK_SSL_WANT_WRIT;
        }
        // failed?
        else
        {
            // trace
            tb_ssl_error("open: handshake: failed", r);

            // save state
            impl->state = TB_STATE_SOCK_SSL_FAILED;
        }

    } while (0);

    // ok?
    if (ok > 0 && !impl->bopened)
    {
        // done impl verify
#if TB_TRACE_MODULE_DEBUG && defined(__tb_debug__) 
        tb_long_t r = 0;
        if ((r = ssl_get_verify_result(&impl->ssl)))
        {
            if ((r & BADCERT_EXPIRED)) tb_trace_d("server certificate has expired");
            if ((r & BADCERT_REVOKED)) tb_trace_d("server certificate has been revoked");
            if ((r & BADCERT_CN_MISMATCH)) tb_trace_d("cn mismatch");
            if ((r & BADCERT_NOT_TRUSTED)) tb_trace_d("self-signed or not signed by a trusted ca");
            tb_ssl_error("verify: failed", r);
        }
#endif

        // opened
        impl->bopened = tb_true;
    }
    // failed?
    else if (ok < 0)
    {
        // save state
        if (impl->state == TB_STATE_OK)
            impl->state = TB_STATE_SOCK_SSL_FAILED;
    }

    // trace
    tb_trace_d("open: handshake: %s", ok > 0? "ok" : (!ok? ".." : "no"));

    // ok?
    return ok;
}
tb_bool_t tb_ssl_clos(tb_ssl_ref_t ssl)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl, tb_false);

    // open it
    tb_long_t ok = -1;
    while (!(ok = tb_ssl_clos_try(ssl)))
    {
        // wait it
        ok = tb_ssl_wait(ssl, TB_SOCKET_EVENT_RECV | TB_SOCKET_EVENT_SEND, impl->timeout);
        tb_check_break(ok > 0);
    }

    // ok?
    return ok > 0? tb_true : tb_false;
}
tb_long_t tb_ssl_clos_try(tb_ssl_ref_t ssl)
{
    // the impl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl, -1);

    // done
    tb_long_t ok = -1;
    do
    {
        // init state
        impl->state = TB_STATE_OK;

        // have been closed already?
        if (!impl->bopened)
        {
            ok = 1;
            break;
        }

        // done close notify
        tb_long_t r = ssl_close_notify(&impl->ssl);

        // trace
        tb_trace_d("clos: close_notify: %ld", r);

        // ok?
        if (!r) ok = 1;
        // continue to wait it?
        else if (r == POLARSSL_ERR_NET_WANT_READ || r == POLARSSL_ERR_NET_WANT_WRITE)
        {
            // trace
            tb_trace_d("clos: close_notify: wait: %s: ..", r == POLARSSL_ERR_NET_WANT_READ? "read" : "writ");

            // continue it
            ok = 0;

            // save state
            impl->state = (r == POLARSSL_ERR_NET_WANT_READ)? TB_STATE_SOCK_SSL_WANT_READ : TB_STATE_SOCK_SSL_WANT_WRIT;
        }
        // failed?
        else
        {
            // trace
            tb_ssl_error("clos: close_notify: failed", r);

            // save state
            impl->state = TB_STATE_SOCK_SSL_FAILED;
        }

        // clear impl
        if (ok > 0) ssl_session_reset(&impl->ssl);

    } while (0);

    // ok?
    if (ok > 0)
    {
        // closed
        impl->bopened = tb_false;
    }
    // failed?
    else if (ok < 0)
    {
        // save state
        if (impl->state == TB_STATE_OK)
            impl->state = TB_STATE_SOCK_SSL_FAILED;
    }

    // trace
    tb_trace_d("clos: close_notify: %s", ok > 0? "ok" : (!ok? ".." : "no"));

    // ok?
    return ok;
}
tb_long_t tb_ssl_read(tb_ssl_ref_t ssl, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl && impl->bopened && data, -1);

    // read it
    tb_long_t real = ssl_read(&impl->ssl, data, size);

    // want read? continue it
    if (real == POLARSSL_ERR_NET_WANT_READ || !real)
    {
        // trace
        tb_trace_d("read: want read");

        // save state
        impl->state = TB_STATE_SOCK_SSL_WANT_READ;
        return 0;
    }
    // want writ? continue it
    else if (real == POLARSSL_ERR_NET_WANT_WRITE)
    {
        // trace
        tb_trace_d("read: want writ");

        // save state
        impl->state = TB_STATE_SOCK_SSL_WANT_WRIT;
        return 0;
    }
    // peer closed?
    else if (real == POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY || real == POLARSSL_ERR_NET_CONN_RESET)
    {
        // trace
        tb_trace_d("read: peer closed");

        // save state
        impl->state = TB_STATE_CLOSED;
        return -1;
    }
    // failed?
    else if (real < 0)
    {
        // trace
        tb_ssl_error("read: failed:", real);

        // save state
        impl->state = TB_STATE_SOCK_SSL_FAILED;
        return -1;
    }

    // trace
    tb_trace_d("read: %ld", real);

    // ok
    return real;
}
tb_long_t tb_ssl_writ(tb_ssl_ref_t ssl, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl && impl->bopened && data, -1);

    // writ it
    tb_long_t real = ssl_write(&impl->ssl, data, size);

    // want read? continue it
    if (real == POLARSSL_ERR_NET_WANT_READ)
    {
        // trace
        tb_trace_d("writ: want read");

        // save state
        impl->state = TB_STATE_SOCK_SSL_WANT_READ;
        return 0;
    }
    // want writ? continue it
    else if (real == POLARSSL_ERR_NET_WANT_WRITE || !real)
    {
        // trace
        tb_trace_d("writ: want writ");

        // save state
        impl->state = TB_STATE_SOCK_SSL_WANT_WRIT;
        return 0;
    }
    // peer closed?
    else if (real == POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY || real == POLARSSL_ERR_NET_CONN_RESET)
    {
        // trace
        tb_trace_d("writ: peer closed");

        // save state
        impl->state = TB_STATE_CLOSED;
        return -1;
    }
    // failed?
    else if (real < 0)
    {
        // trace
        tb_ssl_error("writ: failed", real);

        // save state
        impl->state = TB_STATE_SOCK_SSL_FAILED;
        return -1;
    }

    // trace
    tb_trace_d("writ: %ld", real);

    // ok
    return real;
}
tb_long_t tb_ssl_wait(tb_ssl_ref_t ssl, tb_size_t code, tb_long_t timeout)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl && impl->wait, -1);
    
    // the impl state
    switch (impl->state)
    {
        // wait read
    case TB_STATE_SOCK_SSL_WANT_READ:
        code = TB_SOCKET_EVENT_RECV;
        break;
        // wait writ
    case TB_STATE_SOCK_SSL_WANT_WRIT:
        code = TB_SOCKET_EVENT_SEND;
        break;
        // ok, wait it
    case TB_STATE_OK:
        break;
        // failed or closed?
    default:
        return -1;
    }

    // trace
    tb_trace_d("wait: %lu: ..", code);

    // wait it
    impl->lwait = impl->wait(impl->priv, code, timeout);

    // timeout or failed? save state
    if (impl->lwait < 0) impl->state = TB_STATE_SOCK_SSL_WAIT_FAILED;
    else if (!impl->lwait) impl->state = TB_STATE_SOCK_SSL_TIMEOUT;

    // trace
    tb_trace_d("wait: %ld", impl->lwait);

    // ok?
    return impl->lwait;
}
tb_size_t tb_ssl_state(tb_ssl_ref_t ssl)
{
    // check
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl, TB_STATE_UNKNOWN_ERROR);

    // the state
    return impl->state;
}
