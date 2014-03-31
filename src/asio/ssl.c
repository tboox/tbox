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
 * @file		ssl.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 				"aicp_ssl"
#define TB_TRACE_MODULE_DEBUG 				(1)

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ssl.h"
#include "aico.h"
#include "aicp.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aicp ssl type
typedef struct __tb_aicp_ssl_t
{
	// the ssl 
	tb_handle_t 			ssl;

	// the aicp
	tb_aicp_t* 				aicp;

	// the aico
	tb_handle_t 			aico;

	// the sock
	tb_handle_t 			sock;

	// the timeout
	tb_long_t 				timeout;

}tb_aicp_ssl_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_aicp_ssl_init(tb_aicp_t* aicp, tb_bool_t bserver)
{
	// check
	tb_assert_and_check_return_val(aicp, tb_null);

	// done
	tb_bool_t 		ok = tb_false;
	tb_aicp_ssl_t* 	ssl = tb_null;
	do
	{
		// make ssl
		ssl = tb_malloc0(sizeof(tb_aicp_ssl_t));
		tb_assert_and_check_break(ssl);

		// init ssl
		ssl->ssl = tb_ssl_init(bserver);
		tb_assert_and_check_break(ssl->ssl);

		// init aicp
		ssl->aicp = aicp;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit it
		if (ssl) tb_aicp_ssl_exit(ssl, tb_false);
		ssl = tb_null;
	}

	// ok?
	return ssl;
}
tb_void_t tb_aicp_ssl_kill(tb_handle_t handle)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// kill aico
	if (ssl->aico) tb_aico_kill(ssl->aico);
}
tb_void_t tb_aicp_ssl_clos(tb_handle_t handle, tb_bool_t bcalling)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// exit aico
	if (ssl->aico) tb_aico_exit(ssl->aico, bcalling);
	ssl->aico = tb_null;

	// close ssl
	if (ssl->ssl) tb_ssl_clos(ssl->ssl);
}
tb_void_t tb_aicp_ssl_exit(tb_handle_t handle, tb_bool_t bcalling)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// close it first
	tb_aicp_ssl_clos(ssl, bcalling);

	// exit ssl
	if (ssl->ssl) tb_ssl_exit(ssl->ssl);
	ssl->ssl = tb_null;

	// exit it
	tb_free(ssl);
}
tb_void_t tb_aicp_ssl_set_sock(tb_handle_t handle, tb_handle_t sock)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return(ssl && sock);

	// save sock
	ssl->sock = sock;
}
tb_void_t tb_aicp_ssl_set_timeout(tb_handle_t handle, tb_long_t timeout)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// save timeout
	ssl->timeout = timeout;
}
tb_bool_t tb_aicp_ssl_open(tb_handle_t handle, tb_aicp_ssl_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && func, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// check
		tb_assert_and_check_break(ssl->aicp && ssl->ssl && ssl->sock && !ssl->aico);

		// init aico
		ssl->aico = tb_aico_init_sock(ssl->aicp, ssl->sock);
		tb_assert_and_check_break(ssl->aico);

		// init timeout
		if (ssl->timeout)
		{
			tb_aico_timeout_set(ssl->aico, TB_AICO_TIMEOUT_RECV, ssl->timeout);
			tb_aico_timeout_set(ssl->aico, TB_AICO_TIMEOUT_SEND, ssl->timeout);
		}

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
tb_bool_t tb_aicp_ssl_read(tb_handle_t handle, tb_size_t delay, tb_size_t maxn, tb_aicp_ssl_read_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_aicp_ssl_writ(tb_handle_t handle, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_aicp_ssl_writ_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_aicp_ssl_task(tb_handle_t handle, tb_size_t delay, tb_aicp_ssl_task_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_aicp_ssl_oread(tb_handle_t handle, tb_size_t maxn, tb_aicp_ssl_read_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_bool_t tb_aicp_ssl_owrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_ssl_writ_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
tb_aicp_t* tb_aicp_ssl_aicp(tb_handle_t handle)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl, tb_null);

	// the aicp
	return ssl->aicp;
}

