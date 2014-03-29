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
 * @file		polarssl.c
 * @ingroup 	network
 *
 */
#define TB_TRACE_MODULE_NAME 			"ssl"
#define TB_TRACE_MODULE_DEBUG 			(1)

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <polarssl/ssl.h>
#include <polarssl/certs.h>
#include <polarssl/error.h>
#include <polarssl/entropy.h>
#include <polarssl/ctr_drbg.h>
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the ssl type
typedef struct __tb_ssl_t
{
	// the ssl context
    ssl_context 		ssl;

	// the ssl entropy context
	entropy_context 	entropy;

	// the ssl ctr drbg context
	ctr_drbg_context 	ctr_drbg;

	// the ssl x509 crt
	x509_crt 			x509_crt;

	// is opened?
	tb_bool_t 			bopened;

	// the state
	tb_size_t 			state;

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
static tb_void_t tb_ssl_trace_info(tb_pointer_t priv, tb_int_t level, tb_char_t const* info)
{
	// trace
	if (level < 2) tb_printf("%s", info);
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
static tb_int_t tb_ssl_func_read(tb_pointer_t priv, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_ssl_t* ssl = (tb_ssl_t*)priv;
	tb_assert_and_check_return_val(ssl && ssl->read, -1);
	tb_assert_static(sizeof(tb_size_t) == sizeof(size_t));

	// recv it
	return (tb_int_t)ssl->read(ssl->priv, data, size);
}
static tb_int_t tb_ssl_func_writ(tb_pointer_t priv, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_ssl_t* ssl = (tb_ssl_t*)priv;
	tb_assert_and_check_return_val(ssl && ssl->writ, -1);
	tb_assert_static(sizeof(tb_size_t) == sizeof(size_t));

	// send it
	return (tb_int_t)ssl->writ(ssl->priv, data, size);
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
		// make ssl
		ssl = tb_malloc0(sizeof(tb_ssl_t));
		tb_assert_and_check_break(ssl);

		// init ssl x509_crt
		x509_crt_init(&ssl->x509_crt);

		// init ssl entropy context
		entropy_init(&ssl->entropy);

		// init ssl ctr_drbg context
		tb_long_t r = 0;
		if ((r = ctr_drbg_init(&ssl->ctr_drbg, entropy_func, &ssl->entropy, tb_null, 0)))
		{
			tb_tracef_d("init ctr_drbg failed: %ld", r);
			break;
		}

#ifdef POLARSSL_CERTS_C
		if ((r = x509_crt_parse(&ssl->x509_crt, (tb_byte_t const*)test_ca_list, tb_strlen(test_ca_list))))
		{
			tb_tracef_d("parse x509_crt failed: %ld", r);
			break;
		}
#endif

		// init ssl context
		if ((r = ssl_init(&ssl->ssl)))
		{
			tb_tracef_d("init ssl failed: %ld", r);
			break;
		}

		// init ssl endpoint
		ssl_set_endpoint(&ssl->ssl, bserver? SSL_IS_SERVER : SSL_IS_CLIENT);

		// init ssl authmode: optional
		ssl_set_authmode(&ssl->ssl, SSL_VERIFY_OPTIONAL);

		// init ssl ca chain
		ssl_set_ca_chain(&ssl->ssl, &ssl->x509_crt, tb_null, tb_null);

		// init ssl random generator
		ssl_set_rng(&ssl->ssl, ctr_drbg_random, &ssl->ctr_drbg);

		// enable ssl debug?
#if TB_TRACE_MODULE_DEBUG && defined(__tb_debug__)
		ssl_set_dbg(&ssl->ssl, tb_ssl_trace_info, tb_null);
#endif

		// init state
		ssl->state = TB_STREAM_STATE_OK;

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

	// exit ssl x509_crt
    x509_crt_free(&ssl->x509_crt);

	// exit ssl
	ssl_free(&ssl->ssl);

	// exit ssl entropy
    entropy_free(&ssl->entropy);

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

	// set bio: func
	ssl_set_bio(&ssl->ssl, tb_ssl_func_read, ssl, tb_ssl_func_writ, ssl);
}
tb_bool_t tb_ssl_open(tb_handle_t handle)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl, tb_false);

	// done
	do
	{
		// init state
		ssl->state = TB_STREAM_STATE_OK;

		// check
		tb_assert_and_check_break(!ssl->bopened);

		// done handshake
		tb_long_t r = 0;
		while ((r = ssl_handshake(&ssl->ssl)))
		{
			// failed?
			if (r != POLARSSL_ERR_NET_WANT_READ && r != POLARSSL_ERR_NET_WANT_WRITE)
			{
				// trace
#if TB_TRACE_MODULE_DEBUG && defined(__tb_debug__)
# 	ifdef POLARSSL_ERROR_C
				tb_char_t error[256] = {0};
				polarssl_strerror(r, error, sizeof(error));
				tb_trace_d("handshake: failed: %ld, %s", r, error);
# 	else
				tb_trace_d("handshake: failed: %ld", r);
# 	endif
#endif
				break;
			}
			// wait it?
			else if (ssl->wait)
			{
				// trace
				tb_trace_d("handshake: wait: %s: ..", r == POLARSSL_ERR_NET_WANT_READ? "read" : "writ");

				// wait 
				r = ssl->wait(ssl->priv, r == POLARSSL_ERR_NET_WANT_READ? TB_AIOE_CODE_RECV : TB_AIOE_CODE_SEND, -1);
				tb_check_break(r > 0);
			}
		}

		// failed? break it
		tb_check_break(!r);

		// done ssl verify
#if TB_TRACE_MODULE_DEBUG && defined(__tb_debug__)
		if ((r = ssl_get_verify_result(&ssl->ssl)))
		{
			if ((r & BADCERT_EXPIRED)) tb_trace_d("server certificate has expired");
			if ((r & BADCERT_REVOKED)) tb_trace_d("server certificate has been revoked");
			if ((r & BADCERT_CN_MISMATCH)) tb_trace_d("cn mismatch");
			if ((r & BADCERT_NOT_TRUSTED)) tb_trace_d("self-signed or not signed by a trusted ca");
			tb_trace_d("verify: failed: %ld", r);
		}
#endif

		// ok
		ssl->bopened = tb_true;

	} while (0);

	// failed?
	if (!ssl->bopened)
	{
		// save state
		if (ssl->state == TB_STREAM_STATE_OK)
			ssl->state = TB_STREAM_SOCK_STATE_SSL_FAILED;
	}

	// trace
	tb_trace_d("handshake: %s", ssl->bopened? "ok" : "no");

	// ok?
	return ssl->bopened;
}
tb_void_t tb_ssl_clos(tb_handle_t handle)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// check
	tb_check_return(ssl->bopened);

	// close ssl notify
    ssl_close_notify(&ssl->ssl);

	// clear ssl context
	ssl_session_reset(&ssl->ssl);

	// close it
	ssl->bopened = tb_false;
	ssl->state = TB_STREAM_STATE_OK;
}
tb_long_t tb_ssl_read(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && ssl->bopened && data, -1);

	// read it
	tb_long_t real = ssl_read(&ssl->ssl, data, size);

	// want read? continue it
	if (real == POLARSSL_ERR_NET_WANT_READ)
	{
		// trace
		tb_trace_d("read: want read");

		// save state
		ssl->state = TB_STREAM_SOCK_STATE_SSL_WANT_READ;
		return 0;
	}
	// want writ? continue it
	else if (real == POLARSSL_ERR_NET_WANT_WRITE)
	{
		// trace
		tb_trace_d("read: want writ");

		// save state
		ssl->state = TB_STREAM_SOCK_STATE_SSL_WANT_WRIT;
		return 0;
	}
	// peer closed?
	else if (real == POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY)
	{
		// trace
		tb_trace_d("read: peer closed");

		// save state
		ssl->state = TB_STREAM_STATE_CLOSED;
		return -1;
	}
	// failed?
	else if (real < 0)
	{
		// trace
#if TB_TRACE_MODULE_DEBUG && defined(__tb_debug__)
# 	ifdef POLARSSL_ERROR_C
		tb_char_t error[256] = {0};
		polarssl_strerror(real, error, sizeof(error));
		tb_trace_d("read: failed: %ld, %s", real, error);
# 	else
		tb_trace_d("read: failed: %ld", real);
# 	endif
#endif

		// save state
		ssl->state = TB_STREAM_SOCK_STATE_SSL_FAILED;
		return -1;
	}
	// end?
	else if (!real)
	{
		// trace
		tb_trace_d("read: end");

		// save state
		ssl->state = TB_STREAM_STATE_CLOSED;
		return -1;
	}

	// trace
	tb_trace_d("read: %ld", real);

	// ok
	return real;
}
tb_long_t tb_ssl_writ(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && ssl->bopened && data, -1);

	// writ it
	tb_long_t real = ssl_write(&ssl->ssl, data, size);

	// want read? continue it
	if (real == POLARSSL_ERR_NET_WANT_READ)
	{
		// trace
		tb_trace_d("writ: want read");

		// save state
		ssl->state = TB_STREAM_SOCK_STATE_SSL_WANT_READ;
		return 0;
	}
	// want writ? continue it
	else if (real == POLARSSL_ERR_NET_WANT_WRITE)
	{
		// trace
		tb_trace_d("writ: want writ");

		// save state
		ssl->state = TB_STREAM_SOCK_STATE_SSL_WANT_WRIT;
		return 0;
	}
	// failed?
	else if (real <= 0)
	{
		// trace
#if TB_TRACE_MODULE_DEBUG && defined(__tb_debug__)
# 	ifdef POLARSSL_ERROR_C
		tb_char_t error[256] = {0};
		polarssl_strerror(real, error, sizeof(error));
		tb_trace_d("writ: failed: %ld, %s", real, error);
# 	else
		tb_trace_d("writ: failed: %ld", real);
# 	endif
#endif

		// save state
		ssl->state = TB_STREAM_SOCK_STATE_SSL_FAILED;
		return -1;
	}

	// trace
	tb_trace_d("writ: %ld", real);

	// ok
	return real;
}
tb_size_t tb_ssl_state(tb_handle_t handle)
{
	// the ssl
	tb_ssl_t* ssl = (tb_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl, TB_STREAM_STATE_UNKNOWN_ERROR);

	// the state
	return ssl->state;
}
