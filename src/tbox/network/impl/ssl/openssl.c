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
 * @file        openssl.c
 * @ingroup     network
 *
 */
#define TB_TRACE_MODULE_NAME            "openssl"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>
#include "../../../asio/asio.h"
#include "../../../utils/utils.h"
#include "../../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
// the ssl impl type
typedef struct __tb_ssl_impl_t
{
    // the ssl session
    SSL*                ssl;

    // the ssl context
    SSL_CTX*            ctx;

    // the ssl bio
    BIO*                bio;

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
    tb_cpointer_t        priv;

}tb_ssl_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
static tb_int_t         tb_ssl_bio_method_init(BIO* bio);
static tb_int_t         tb_ssl_bio_method_exit(BIO* bio);
static tb_int_t         tb_ssl_bio_method_read(BIO* bio, tb_char_t* data, tb_int_t size);
static tb_int_t         tb_ssl_bio_method_writ(BIO* bio, tb_char_t const* data, tb_int_t size);
static tb_long_t        tb_ssl_bio_method_ctrl(BIO* bio, tb_int_t cmd, tb_long_t num, tb_pointer_t ptr);
static tb_int_t         tb_ssl_bio_method_puts(BIO* bio, tb_char_t const* data);
static tb_int_t         tb_ssl_bio_method_gets(BIO* bio, tb_char_t* data, tb_int_t size);

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static BIO_METHOD g_ssl_bio_method =
{
    BIO_TYPE_SOURCE_SINK | 100
,   "ssl_bio"
,   tb_ssl_bio_method_writ
,   tb_ssl_bio_method_read
,   tb_ssl_bio_method_puts
,   tb_ssl_bio_method_gets
,   tb_ssl_bio_method_ctrl
,   tb_ssl_bio_method_init
,   tb_ssl_bio_method_exit
,   tb_null
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * library implementation
 */
static tb_handle_t tb_ssl_library_init(tb_cpointer_t* ppriv)
{
    // init it
    SSL_library_init();

    // ok
    return ppriv;
}
static tb_void_t tb_ssl_library_exit(tb_handle_t ssl, tb_cpointer_t priv)
{
}
static tb_handle_t tb_ssl_library_load()
{
    return tb_singleton_instance(TB_SINGLETON_TYPE_LIBRARY_OPENSSL, tb_ssl_library_init, tb_ssl_library_exit, tb_null);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_int_t tb_ssl_verify(tb_int_t ok, X509_STORE_CTX* ctx)
{
    return 1;
}
#ifdef __tb_debug__
static tb_char_t const* tb_ssl_error(tb_long_t error)
{
    // done
    switch (error)
    {
    case SSL_ERROR_NONE:
        return "SSL_ERROR_NONE";
    case SSL_ERROR_SSL:
        return "SSL_ERROR_SSL";
    case SSL_ERROR_WANT_READ:
        return "SSL_ERROR_WANT_READ";
    case SSL_ERROR_WANT_WRITE:
        return "SSL_ERROR_WANT_WRITE";
    case SSL_ERROR_WANT_X509_LOOKUP:
        return "SSL_ERROR_WANT_X509_LOOKUP";
    case SSL_ERROR_SYSCALL:
        return "SSL_ERROR_SYSCALL";
    case SSL_ERROR_ZERO_RETURN:
        return "SSL_ERROR_ZERO_RETURN";
    case SSL_ERROR_WANT_CONNECT:
        return "SSL_ERROR_WANT_CONNECT";
    case SSL_ERROR_WANT_ACCEPT:
        return "SSL_ERROR_WANT_ACCEPT";
    default:
        return "UNKOWN_ERROR_VALUE";
    }
    return "";
}
#endif
static tb_long_t tb_ssl_sock_read(tb_cpointer_t priv, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(priv, -1);

    // recv it
    return tb_socket_recv((tb_handle_t)priv, data, size);
}
static tb_long_t tb_ssl_sock_writ(tb_cpointer_t priv, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(priv, -1);

    // send it
    return tb_socket_send((tb_handle_t)priv, data, size);
}
static tb_long_t tb_ssl_sock_wait(tb_cpointer_t priv, tb_size_t code, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(priv, -1);

    // wait it
    return tb_aioo_wait((tb_handle_t)priv, code, timeout);
}
static tb_int_t tb_ssl_bio_method_init(BIO* bio)
{
    // check
    tb_assert_and_check_return_val(bio, 0);

    // trace
    tb_trace_d("bio: init");

    // init 
    bio->init       = 1;
    bio->num        = 0;
    bio->ptr        = tb_null;
    bio->flags      = 0;
    bio->shutdown   = 1;

    // ok
    return 1;
}
static tb_int_t tb_ssl_bio_method_exit(BIO* bio)
{
    // check
    tb_assert_and_check_return_val(bio, 0);

    // trace
    tb_trace_d("bio: exit");

    // exit 
    bio->init       = 0;
    bio->num        = 0;
    bio->ptr        = tb_null;
    bio->flags      = 0;

    // ok
    return 1;
}
static tb_int_t tb_ssl_bio_method_read(BIO* bio, tb_char_t* data, tb_int_t size)
{
    // check
    tb_assert_and_check_return_val(bio && data && size >= 0, -1);

    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)bio->ptr;
    tb_assert_and_check_return_val(impl && impl->read, -1);

    // writ 
    tb_long_t real = impl->read(impl->priv, (tb_byte_t*)data, size);

    // trace
    tb_trace_d("bio: read: real: %ld, size: %d", real, size);

    // ok? clear wait
    if (real > 0) impl->lwait = 0;
    // peer closed?
    else if (!real && impl->lwait > 0 && (impl->lwait & TB_AIOE_CODE_RECV)) 
    {
        BIO_clear_retry_flags(bio);
        real = -1;
    }
    // no data? continue to read it
    else if (!real) 
    {
        BIO_clear_retry_flags(bio);
        BIO_set_retry_read(bio);
        real = -1;
    }
    // failed?
    else 
    {
        BIO_clear_retry_flags(bio);
        real = -1;
    }

    // ok?
    return (tb_int_t)real;
}
static tb_int_t tb_ssl_bio_method_writ(BIO* bio, tb_char_t const* data, tb_int_t size)
{
    // check
    tb_assert_and_check_return_val(bio && data && size >= 0, -1);

    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)bio->ptr;
    tb_assert_and_check_return_val(impl && impl->writ, -1);

    // writ 
    tb_long_t real = impl->writ(impl->priv, (tb_byte_t const*)data, size);

    // trace
    tb_trace_d("bio: writ: real: %ld, size: %d", real, size);

    // ok? clear wait
    if (real > 0) impl->lwait = 0;
    // peer closed?
    else if (!real && impl->lwait > 0 && (impl->lwait & TB_AIOE_CODE_SEND)) 
    {
        BIO_clear_retry_flags(bio);
        real = -1;
    }
    // no data? continue to writ
    else if (!real) 
    {
        BIO_clear_retry_flags(bio);
        BIO_set_retry_write(bio);
        real = -1;
    }
    // failed?
    else
    {
        BIO_clear_retry_flags(bio);
        real = -1;
    }

    // ok?
    return (tb_int_t)real;
}
static tb_long_t tb_ssl_bio_method_ctrl(BIO* bio, tb_int_t cmd, tb_long_t num, tb_pointer_t ptr)
{
    // check
    tb_assert_and_check_return_val(bio, -1);

    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)bio->ptr;
    tb_assert_and_check_return_val(impl, -1);

    // done
    tb_long_t ok = 0;
    switch (cmd)
    {
    case BIO_CTRL_FLUSH:
        {
            // trace
            tb_trace_d("bio: ctrl: flush");

            // ok
            ok = 1;
        }
        break;
    default:
        {
            // trace
            tb_trace_d("bio: ctrl: unknown: %d", cmd);
        }
        break;
    }
    // ok?
    return ok;
}
static tb_int_t tb_ssl_bio_method_puts(BIO* bio, tb_char_t const* data)
{
    // check
    tb_assert_and_check_return_val(bio && data, -1);

    // trace
    tb_trace_d("bio: puts: %s", data);

    // writ
    return tb_ssl_bio_method_writ(bio, data, tb_strlen(data));
}
static tb_int_t tb_ssl_bio_method_gets(BIO* bio, tb_char_t* data, tb_int_t size)
{
    // check
    tb_assert_and_check_return_val(bio && data, -1);

    // trace
    tb_trace_d("bio: gets: %d", size);

    // read it
    return tb_ssl_bio_method_read(bio, data, size);
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
        // load openssl library
        if (!tb_ssl_library_load()) break;

        // make ssl
        impl = tb_malloc0_type(tb_ssl_impl_t);
        tb_assert_and_check_break(impl);

        // init timeout, 30s
        impl->timeout = 30000;

        // init ctx
        impl->ctx = SSL_CTX_new(SSLv3_method());
        tb_assert_and_check_break(impl->ctx);
        
        // make ssl
        impl->ssl = SSL_new(impl->ctx);
        tb_assert_and_check_break(impl->ssl);

        // init endpoint 
        if (bserver) SSL_set_accept_state(impl->ssl);
        else SSL_set_connect_state(impl->ssl);

        // init verify
        SSL_set_verify(impl->ssl, 0, tb_ssl_verify);

        // init bio
        impl->bio = BIO_new(&g_ssl_bio_method);
        tb_assert_and_check_break(impl->bio);

        // set bio to ssl
        impl->bio->ptr = impl;
        SSL_set_bio(impl->ssl, impl->bio, impl->bio);

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
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return(impl);

    // close it first
    tb_ssl_clos(ssl);

    // exit ssl
    if (impl->ssl) SSL_free(impl->ssl);
    impl->ssl = tb_null;

    // exit ctx
    if (impl->ctx) SSL_CTX_free(impl->ctx);
    impl->ctx = tb_null;

    // exit it
    tb_free(impl);
}
tb_void_t tb_ssl_set_bio_sock(tb_ssl_ref_t ssl, tb_socket_ref_t sock)
{
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return(impl);

    // set bio: sock
    tb_ssl_set_bio_func(ssl, tb_ssl_sock_read, tb_ssl_sock_writ, tb_ssl_sock_wait, sock);
}
tb_void_t tb_ssl_set_bio_func(tb_ssl_ref_t ssl, tb_ssl_func_read_t read, tb_ssl_func_writ_t writ, tb_ssl_func_wait_t wait, tb_cpointer_t priv)
{
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return(impl && read && writ);

    // save func
    impl->read = read;
    impl->writ = writ;
    impl->wait = wait;
    impl->priv = priv;
}
tb_void_t tb_ssl_set_timeout(tb_ssl_ref_t ssl, tb_long_t timeout)
{
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return(impl);

    // save timeout
    impl->timeout = timeout;
}
tb_bool_t tb_ssl_open(tb_ssl_ref_t ssl)
{
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl && impl->wait, tb_false);

    // open it
    tb_long_t ok = -1;
    while (!(ok = tb_ssl_open_try(ssl)))
    {
        // wait it
        ok = tb_ssl_wait(ssl, TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND, impl->timeout);
        tb_check_break(ok > 0);
    }

    // ok?
    return ok > 0? tb_true : tb_false;
}
tb_long_t tb_ssl_open_try(tb_ssl_ref_t ssl)
{
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl && impl->ssl, -1);

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

        // do handshake
        tb_long_t r = SSL_do_handshake(impl->ssl);
    
        // trace
        tb_trace_d("open: handshake: %ld", r);

        // ok?
        if (r == 1) ok = 1;
        // continue ?
        else if (!r) ok = 0;
        else
        {
            // the error
            tb_long_t error = SSL_get_error(impl->ssl, r);

            // wait?
            if (error == SSL_ERROR_WANT_WRITE || error == SSL_ERROR_WANT_READ)
            {
                // trace
                tb_trace_d("open: handshake: wait: %s: ..", error == SSL_ERROR_WANT_READ? "read" : "writ");

                // continue it
                ok = 0;

                // save state
                impl->state = (error == SSL_ERROR_WANT_READ)? TB_STATE_SOCK_SSL_WANT_READ : TB_STATE_SOCK_SSL_WANT_WRIT;
            }
            // failed?
            else
            {
                // trace
                tb_trace_d("open: handshake: failed: %s", tb_ssl_error(error));
    
                // save state
                impl->state = TB_STATE_SOCK_SSL_FAILED;
            }
        }

    } while (0);

    // ok?
    if (ok > 0)
    {
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
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl, tb_false);

    // open it
    tb_long_t ok = -1;
    while (!(ok = tb_ssl_clos_try(ssl)))
    {
        // wait it
        ok = tb_ssl_wait(ssl, TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND, impl->timeout);
        tb_check_break(ok > 0);
    }

    // ok?
    return ok > 0? tb_true : tb_false;
}
tb_long_t tb_ssl_clos_try(tb_ssl_ref_t ssl)
{
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl, -1);

    // done
    tb_long_t ok = -1;
    do
    {
        // init state
        impl->state = TB_STATE_OK;

        // have been closed already?
        if (!impl->bopened || !impl->ssl)
        {
            ok = 1;
            break;
        }

        // do shutdown
        tb_long_t r = SSL_shutdown(impl->ssl);
    
        // trace
        tb_trace_d("clos: shutdown: %ld", r);

        // ok?
        if (r == 1) ok = 1;
        // continue?
        else if (!r) ok = 0;
        else
        {
            // the error
            tb_long_t error = SSL_get_error(impl->ssl, r);

            // wait?
            if (error == SSL_ERROR_WANT_WRITE || error == SSL_ERROR_WANT_READ)
            {
                // trace
                tb_trace_d("clos: shutdown: wait: %s: ..", error == SSL_ERROR_WANT_READ? "read" : "writ");

                // continue it
                ok = 0;

                // save state
                impl->state = (error == SSL_ERROR_WANT_READ)? TB_STATE_SOCK_SSL_WANT_READ : TB_STATE_SOCK_SSL_WANT_WRIT;
            }
            // failed?
            else
            {
                // trace
                tb_trace_d("clos: shutdown: failed: %s", tb_ssl_error(error));
    
                // save state
                impl->state = TB_STATE_SOCK_SSL_FAILED;
            }
        }

    } while (0);

    // ok?
    if (ok > 0)
    {
        // closed
        impl->bopened = tb_false;

        // clear ssl
        if (impl->ssl) SSL_clear(impl->ssl);
    }
    // failed?
    else if (ok < 0)
    {
        // save state
        if (impl->state == TB_STATE_OK)
            impl->state = TB_STATE_SOCK_SSL_FAILED;
    }

    // trace
    tb_trace_d("clos: shutdown: %s", ok > 0? "ok" : (!ok? ".." : "no"));

    // ok?
    return ok;
}
tb_long_t tb_ssl_read(tb_ssl_ref_t ssl, tb_byte_t* data, tb_size_t size)
{
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl && impl->ssl && impl->bopened && data, -1);

    // read it
    tb_long_t real = SSL_read(impl->ssl, data, size);

    // trace
    tb_trace_d("read: %ld", real);

    // done
    if (real < 0)
    {
        // the error
        tb_long_t error = SSL_get_error(impl->ssl, real);

        // want read? continue it
        if (error == SSL_ERROR_WANT_READ)
        {
            // trace
            tb_trace_d("read: want read");

            // save state
            impl->state = TB_STATE_SOCK_SSL_WANT_READ;
            return 0;
        }
        // want writ? continue it
        else if (error == SSL_ERROR_WANT_WRITE)
        {
            // trace
            tb_trace_d("read: want writ");

            // save state
            impl->state = TB_STATE_SOCK_SSL_WANT_WRIT;
            return 0;
        }
        // failed?
        else
        {
            // trace
            tb_trace_d("read: failed: %ld, %s", real, tb_ssl_error(error));

            // save state
            impl->state = TB_STATE_SOCK_SSL_FAILED;
            return -1;
        }
    }
    // closed?
    else if (!real)
    {
        // trace
        tb_trace_d("read: closed");

        // save state
        impl->state = TB_STATE_CLOSED;
        return -1;
    }

    // ok
    return real;
}
tb_long_t tb_ssl_writ(tb_ssl_ref_t ssl, tb_byte_t const* data, tb_size_t size)
{
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl && impl->ssl && impl->bopened && data, -1);

    // writ it
    tb_long_t real = SSL_write(impl->ssl, data, size);

    // trace
    tb_trace_d("writ: %ld", real);

    // done
    if (real < 0)
    {
        // the error
        tb_long_t error = SSL_get_error(impl->ssl, real);

        // want read? continue it
        if (error == SSL_ERROR_WANT_READ)
        {
            // trace
            tb_trace_d("writ: want read");

            // save state
            impl->state = TB_STATE_SOCK_SSL_WANT_READ;
            return 0;
        }
        // want writ? continue it
        else if (error == SSL_ERROR_WANT_WRITE)
        {
            // trace
            tb_trace_d("writ: want writ");

            // save state
            impl->state = TB_STATE_SOCK_SSL_WANT_WRIT;
            return 0;
        }
        // failed?
        else
        {
            // trace
            tb_trace_d("writ: failed: %ld, %s", real, tb_ssl_error(error));

            // save state
            impl->state = TB_STATE_SOCK_SSL_FAILED;
            return -1;
        }
    }
    // closed?
    else if (!real)
    {
        // trace
        tb_trace_d("read: closed");

        // save state
        impl->state = TB_STATE_CLOSED;
        return -1;
    }

    // ok
    return real;
}
tb_long_t tb_ssl_wait(tb_ssl_ref_t ssl, tb_size_t code, tb_long_t timeout)
{
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl && impl->wait, -1);
    
    // the ssl state
    switch (impl->state)
    {
        // wait read
    case TB_STATE_SOCK_SSL_WANT_READ:
        code = TB_AIOE_CODE_RECV;
        break;
        // wait writ
    case TB_STATE_SOCK_SSL_WANT_WRIT:
        code = TB_AIOE_CODE_SEND;
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
    // the ssl
    tb_ssl_impl_t* impl = (tb_ssl_impl_t*)ssl;
    tb_assert_and_check_return_val(impl, TB_STATE_UNKNOWN_ERROR);

    // the state
    return impl->state;
}

