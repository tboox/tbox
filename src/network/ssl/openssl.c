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
 * @author		ruki
 * @file		openssl.c
 * @ingroup 	network
 *
 */
#define TB_TRACE_MODULE_NAME 			"ssl"
#define TB_TRACE_MODULE_DEBUG 			(1)

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the ssl type
typedef struct __tb_ssl_t
{
	// the ssl session
	SSL* 				ssl;

	// the ssl context
	SSL_CTX* 			ctx;

	// the ssl bio
	BIO* 				bio;

	// is opened?
	tb_bool_t 			bopened;

	// the state
	tb_size_t 			state;

	// the last wait
	tb_long_t 			lwait;

	// the timeout
	tb_long_t 			timeout;

	// the read func
	tb_ssl_func_read_t 	read;

	// the writ func
	tb_ssl_func_writ_t 	writ;

	// the wait func
	tb_ssl_func_wait_t 	wait;

	// the priv data
	tb_pointer_t 		priv;

}tb_ssl_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_int_t tb_ssl_verify(tb_int_t ok, X509_STORE_CTX* ctx)
{
	return 1;
}
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
static tb_long_t tb_ssl_sock_read(tb_pointer_t priv, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(priv, -1);

	// recv it
	return tb_socket_recv((tb_handle_t)priv, data, size);
}
static tb_long_t tb_ssl_sock_writ(tb_pointer_t priv, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(priv, -1);

	// send it
	return tb_socket_send((tb_handle_t)priv, data, size);
}
static tb_long_t tb_ssl_sock_wait(tb_pointer_t priv, tb_size_t code, tb_long_t timeout)
{
	// check
	tb_assert_and_check_return_val(priv, -1);

	// wait it
	return tb_aioo_wait((tb_handle_t)priv, code, timeout);
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_ssl_init(tb_bool_t bserver)
{
	// done
	tb_ssl_t* ssl = tb_null;
	tb_bool_t ok = tb_false;
	do
	{
		// init ssl library first
		static tb_atomic_t s_init = 0;
		if (!tb_atomic_fetch_and_set(&s_init, 1)) SSL_library_init();

		// make ssl
		ssl = tb_malloc0(sizeof(tb_ssl_t));
		tb_assert_and_check_break(ssl);

		// init timeout, 30s
		ssl->timeout = 30000;

		// init ctx
		ssl->ctx = SSL_CTX_new(SSLv3_method());
		tb_assert_and_check_break(ssl->ctx);
		
		// make ssl
		ssl->ssl = SSL_new(ssl->ctx);
		tb_assert_and_check_break(ssl->ssl);

		// init endpoint 
		if (bserver) SSL_set_accept_state(ssl->ssl);
		else SSL_set_connect_state(ssl->ssl);

		// init verify
		SSL_set_verify(ssl, 0, tb_ssl_verify);

		// init state
		ssl->state = TB_STATE_OK;

		// ok
		ok = tb_true;

	} while (0);

	// failed? exit it
	if (!ok)
	{
		if (ssl) tb_ssl_exit((tb_handle_t)ssl);
		ssl = tb_null;
	}

	// ok?
	return (tb_handle_t)ssl;
}
tb_void_t tb_ssl_exit(tb_handle_t handle)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// close it first
	tb_ssl_clos(handle);

	// exit ssl
	if (ssl->ssl) SSL_free(ssl->ssl);
	ssl->ssl = tb_null;

	// exit ctx
	if (ssl->ctx) SSL_CTX_free(ssl->ctx);
	ssl->ctx = tb_null;

	// exit it
	tb_free(ssl);
}
tb_void_t tb_ssl_set_bio_sock(tb_handle_t handle, tb_handle_t sock)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// set bio: sock
	tb_ssl_set_bio_func(ssl, tb_ssl_sock_read, tb_ssl_sock_writ, tb_ssl_sock_wait, sock);
}
tb_void_t tb_ssl_set_bio_func(tb_handle_t handle, tb_ssl_func_read_t read, tb_ssl_func_writ_t writ, tb_ssl_func_wait_t wait, tb_pointer_t priv)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return(ssl && read && writ);

	// save func
	ssl->read = read;
	ssl->writ = writ;
	ssl->wait = wait;
	ssl->priv = priv;

}
tb_void_t tb_ssl_set_timeout(tb_handle_t handle, tb_long_t timeout)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// save timeout
	ssl->timeout = timeout;
}
tb_bool_t tb_ssl_open(tb_handle_t handle)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && ssl->wait, tb_false);

	// open it
	tb_long_t ok = -1;
	while (!(ok = tb_ssl_open_try(handle)))
	{
		// wait it
		ok = tb_ssl_wait(handle, TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND, ssl->timeout);
		tb_check_break(ok > 0);
	}

	// ok?
	return ok > 0? tb_true : tb_false;
}
tb_long_t tb_ssl_open_try(tb_handle_t handle)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && ssl->ssl, -1);

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

		// do handshake
		tb_long_t r = SSL_do_handshake(ssl->ssl);
	
		// trace
		tb_trace_d("handshake: %ld", r);

		// ok?
		if (r == 1) ok = 1;
		else
		{
			// trace
			tb_trace_d("handshake: %s", tb_ssl_error(SSL_get_error(ssl->ssl, r)));
	
			// save state
			ssl->state = TB_STATE_SOCK_SSL_FAILED;
			break;
		}

	} while (0);

	// ok?
	if (ok > 0)
	{
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
	tb_trace_d("handshake: %s", ok > 0? "ok" : (!ok? ".." : "no"));

	// ok?
	return ok;
}
tb_void_t tb_ssl_clos(tb_handle_t handle)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// check
	tb_check_return(ssl->bopened);

	// close ssl
	if (ssl->ssl) SSL_shutdown(ssl->ssl);

	// close it
	ssl->bopened = tb_false;
	ssl->state = TB_STATE_OK;
}
tb_long_t tb_ssl_read(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && ssl->ssl && ssl->bopened && data, -1);

	// read it
	tb_long_t real = SSL_read(ssl->ssl, data, size);

	// trace
	tb_trace_d("read: %ld", real);

	// ok
	return real;
}
tb_long_t tb_ssl_writ(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && ssl->ssl && ssl->bopened && data, -1);

	// writ it
	tb_long_t real = SSL_write(ssl->ssl, data, size);

	// trace
	tb_trace_d("writ: %ld", real);

	// ok
	return real;
}
tb_long_t tb_ssl_wait(tb_handle_t handle, tb_size_t code, tb_long_t timeout)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && ssl->wait, -1);
	
	// the ssl state
	switch (ssl->state)
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
	ssl->lwait = ssl->wait(ssl->priv, code, timeout);

	// timeout or failed? save state
	if (ssl->lwait < 0) ssl->state = TB_STATE_SOCK_SSL_WAIT_FAILED;
	else if (!ssl->lwait) ssl->state = TB_STATE_SOCK_SSL_TIMEOUT;

	// trace
	tb_trace_d("wait: %ld", ssl->lwait);

	// ok?
	return ssl->lwait;
}
tb_size_t tb_ssl_state(tb_handle_t handle)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl, TB_STATE_UNKNOWN_ERROR);

	// the state
	return ssl->state;
}

