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
 * @file        mbedtls.c
 * @ingroup     network
 *
 */
#define TB_TRACE_MODULE_NAME            "mbedtls"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "mbedtls/mbedtls.h"
#include "../../../libc/libc.h"
#include "../../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the ssl type
typedef struct __tb_ssl_t
{
    // the ssl ctr drbg context
    mbedtls_ssl_context         ssl;

    // the ssl entropy context
    mbedtls_entropy_context     entropy;

    // the ssl ctr drbg context
    mbedtls_ctr_drbg_context    ctr_drbg;

    // the ssl x509 crt
    mbedtls_x509_crt            x509_crt;

    // the ssl config
    mbedtls_ssl_config          conf;

    // is opened?
    tb_bool_t                   bopened;

    // the state
    tb_size_t                   state;

    // the last wait
    tb_long_t                   lwait;

    // the timeout
    tb_long_t                   timeout;

    // the read func
    tb_ssl_func_read_t          read;

    // the writ func
    tb_ssl_func_writ_t          writ;

    // the wait func
    tb_ssl_func_wait_t          wait;

    // the priv data
    tb_cpointer_t               priv;

}tb_ssl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_ssl_ref_t tb_ssl_init(tb_bool_t bserver)
{
    // done
    tb_bool_t   ok = tb_false;
    tb_ssl_t*   ssl = tb_null;
    do
    {
        // make ssl
        ssl = tb_malloc0_type(tb_ssl_t);
        tb_assert_and_check_break(ssl);

        // init timeout, 30s
        ssl->timeout = 30000;

        // init ssl x509_crt
        mbedtls_x509_crt_init(&ssl->x509_crt);

        // init ssl entropy context
        mbedtls_entropy_init(&ssl->entropy);

        // init ssl ctr_drbg context
        mbedtls_ctr_drbg_init(&ssl->ctr_drbg);

        // init ssl context
        mbedtls_ssl_init(&ssl->ssl);

        // init ssl config
        mbedtls_ssl_config_init(&ssl->conf);

        // TODO

        // init state
        ssl->state = TB_STATE_OK;

        // ok
        ok = tb_true;

    } while (0);

    // failed? exit it
    if (!ok)
    {
        if (ssl) tb_ssl_exit((tb_ssl_ref_t)ssl);
        ssl = tb_null;
    }

    // ok?
    return (tb_ssl_ref_t)ssl;
}
tb_void_t tb_ssl_exit(tb_ssl_ref_t self)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return(ssl);

    // close it first
    tb_ssl_clos(self);

    // TODO

    // exit it
    tb_free(ssl);
}
tb_void_t tb_ssl_set_bio_sock(tb_ssl_ref_t self, tb_socket_ref_t sock)
{
    // the ssl
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return(ssl);

    // TODO
    // set bio: sock
//    tb_ssl_set_bio_func(self, tb_ssl_sock_read, tb_ssl_sock_writ, tb_ssl_sock_wait, sock);
}
tb_void_t tb_ssl_set_bio_func(tb_ssl_ref_t self, tb_ssl_func_read_t read, tb_ssl_func_writ_t writ, tb_ssl_func_wait_t wait, tb_cpointer_t priv)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return(ssl && read && writ);

    // save func
    ssl->read = read;
    ssl->writ = writ;
    ssl->wait = wait;
    ssl->priv = priv;

    // TODO
}
tb_void_t tb_ssl_set_timeout(tb_ssl_ref_t self, tb_long_t timeout)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return(ssl);

    // save timeout
    ssl->timeout = timeout;
}
tb_bool_t tb_ssl_open(tb_ssl_ref_t self)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return_val(ssl && ssl->wait, tb_false);

    // open it
    tb_long_t ok = -1;
    while (!(ok = tb_ssl_open_try(self)))
    {
        // wait it
        ok = tb_ssl_wait(self, TB_SOCKET_EVENT_RECV | TB_SOCKET_EVENT_SEND, ssl->timeout);
        tb_check_break(ok > 0);
    }

    // ok?
    return ok > 0? tb_true : tb_false;
}
tb_long_t tb_ssl_open_try(tb_ssl_ref_t self)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return_val(ssl, -1);

    // done
    tb_long_t ok = -1;
    do
    {
        // init state
        ssl->state = TB_STATE_OK;

        // have been opened already?
        if (ssl->bopened)
        {
            ok = 1;
            break;
        }

        // done handshake
        // TODO

    } while (0);

    // ok?
    if (ok > 0 && !ssl->bopened)
    {
        // TODO

        // opened
        ssl->bopened = tb_true;
    }
    // failed?
    else if (ok < 0)
    {
        // save state
        if (ssl->state == TB_STATE_OK)
            ssl->state = TB_STATE_SOCK_SSL_FAILED;
    }

    // trace
    tb_trace_d("open: handshake: %s", ok > 0? "ok" : (!ok? ".." : "no"));

    // ok?
    return ok;
}
tb_bool_t tb_ssl_clos(tb_ssl_ref_t self)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return_val(ssl, tb_false);

    // close it
    tb_long_t ok = -1;
    while (!(ok = tb_ssl_clos_try(self)))
    {
        // wait it
        ok = tb_ssl_wait(self, TB_SOCKET_EVENT_RECV | TB_SOCKET_EVENT_SEND, ssl->timeout);
        tb_check_break(ok > 0);
    }

    // ok?
    return ok > 0? tb_true : tb_false;
}
tb_long_t tb_ssl_clos_try(tb_ssl_ref_t self)
{
    // the ssl
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return_val(ssl, -1);

    // done
    tb_long_t ok = -1;
    do
    {
        // init state
        ssl->state = TB_STATE_OK;

        // have been closed already?
        if (!ssl->bopened)
        {
            ok = 1;
            break;
        }

        // TODO

    } while (0);

    // ok?
    if (ok > 0)
    {
        // closed
        ssl->bopened = tb_false;
    }
    // failed?
    else if (ok < 0)
    {
        // save state
        if (ssl->state == TB_STATE_OK)
            ssl->state = TB_STATE_SOCK_SSL_FAILED;
    }

    // trace
    tb_trace_d("clos: close_notify: %s", ok > 0? "ok" : (!ok? ".." : "no"));

    // ok?
    return ok;
}
tb_long_t tb_ssl_read(tb_ssl_ref_t self, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return_val(ssl && ssl->bopened && data, -1);


    // ok
    return -1;
}
tb_long_t tb_ssl_writ(tb_ssl_ref_t self, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return_val(ssl && ssl->bopened && data, -1);

    // ok
    return -1;
}
tb_long_t tb_ssl_wait(tb_ssl_ref_t self, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return_val(ssl && ssl->wait, -1);
    
    // the ssl state
    switch (ssl->state)
    {
        // wait read
    case TB_STATE_SOCK_SSL_WANT_READ:
        events = TB_SOCKET_EVENT_RECV;
        break;
        // wait writ
    case TB_STATE_SOCK_SSL_WANT_WRIT:
        events = TB_SOCKET_EVENT_SEND;
        break;
        // ok, wait it
    case TB_STATE_OK:
        break;
        // failed or closed?
    default:
        return -1;
    }

    // trace
    tb_trace_d("wait: %lu: ..", events);

    // wait it
    ssl->lwait = ssl->wait(ssl->priv, events, timeout);

    // timeout or failed? save state
    if (ssl->lwait < 0) ssl->state = TB_STATE_SOCK_SSL_WAIT_FAILED;
    else if (!ssl->lwait) ssl->state = TB_STATE_SOCK_SSL_TIMEOUT;

    // trace
    tb_trace_d("wait: %ld", ssl->lwait);

    // ok?
    return ssl->lwait;
}
tb_size_t tb_ssl_state(tb_ssl_ref_t self)
{
    // check
    tb_ssl_t* ssl = (tb_ssl_t*)self;
    tb_assert_and_check_return_val(ssl, TB_STATE_UNKNOWN_ERROR);

    // the state
    return ssl->state;
}
