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

// the aicp ssl open type
typedef struct __tb_aicp_ssl_open_t
{
	// the func
	tb_aicp_ssl_open_func_t func;

	// the priv 
	tb_pointer_t 			priv;

}tb_aicp_ssl_open_t;

// the aicp ssl read type
typedef struct __tb_aicp_ssl_read_t
{
	// the func
	tb_aicp_ssl_read_func_t func;

	// the data
	tb_byte_t* 				data;

	// the size
	tb_size_t 				size;

	// the priv 
	tb_pointer_t 			priv;

	// the delay
	tb_size_t 				delay;

}tb_aicp_ssl_read_t;

// the aicp ssl writ type
typedef struct __tb_aicp_ssl_writ_t
{
	// the func
	tb_aicp_ssl_writ_func_t func;

	// the data
	tb_byte_t const* 		data;

	// the size
	tb_size_t 				size;

	// the priv 
	tb_pointer_t 			priv;

	// the delay
	tb_size_t 				delay;

}tb_aicp_ssl_writ_t;

// the aicp ssl task type
typedef struct __tb_aicp_ssl_task_t
{
	// the func
	tb_aicp_ssl_task_func_t func;

	// the priv 
	tb_pointer_t 			priv;

	// the delay
	tb_size_t 				delay;

}tb_aicp_ssl_task_t;

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

	// the func
	union
	{
		tb_aicp_ssl_open_t 	open;
		tb_aicp_ssl_read_t 	read;
		tb_aicp_ssl_writ_t 	writ;
		tb_aicp_ssl_task_t 	task;

	} 						func;

	// the open and func
	union
	{
		tb_aicp_ssl_read_t 	read;
		tb_aicp_ssl_writ_t 	writ;

	} 						open_and;

	// the timeout
	tb_long_t 				timeout;

	// is opened?
	tb_atomic_t 			bopened;

	// the read data
	tb_pbuffer_t 			read_data;

	// the writ data
	tb_pbuffer_t 			writ_data;

	// the read real size
	tb_size_t 				read_real;

	// the writ real size
	tb_size_t 				writ_real;

}tb_aicp_ssl_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_aicp_ssl_try_read(tb_aicp_ssl_t* ssl, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(ssl, -1);

	// trace
	tb_trace_d("read: try: %lu: ..", size);

	// done
	tb_long_t real = -1;
	do
	{
		// check
		tb_assert_and_check_break(ssl->aico);
		tb_assert_and_check_break(data && size);

		// have read data?
		if (ssl->read_real)
		{
			// save real
			tb_size_t read_real = ssl->read_real;

			// clear real
			ssl->read_real = 0;

			// check
			tb_assert_and_check_break(read_real <= size);

			// the data and size
			tb_byte_t* 	read_data = tb_pbuffer_data(&ssl->read_data);
			tb_size_t 	read_size = tb_pbuffer_size(&ssl->read_data);
			tb_assert_and_check_break(read_data && read_size && size <= read_size);

			// copy data
			tb_memcpy(data, read_data, read_real);

			// trace
			tb_trace_d("read: done: %lu: ok", read_real);

			// read ok
			real = read_real;
			break;
		}

		// continue it
		real = 0;

	} while (0);

	// ok?
	return real;
}
static tb_long_t tb_aicp_ssl_try_writ(tb_aicp_ssl_t* ssl, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(ssl, -1);

	// trace
	tb_trace_d("writ: try: %lu: ..", size);

	// done
	tb_long_t real = -1;
	do
	{
		// check
		tb_assert_and_check_break(ssl->aico);
		tb_assert_and_check_break(size);

		// have writ data?
		if (ssl->writ_real)
		{
			// save real
			tb_size_t writ_real = ssl->writ_real;

			// clear real
			ssl->writ_real = 0;

			// check
			tb_assert_and_check_break(writ_real <= size);

			// trace
			tb_trace_d("writ: done: %lu: ok", writ_real);

			// writ ok
			return writ_real;
		}

		// continue it
		real = 0;

	} while (0);

	// ok?
	return real;
}
static tb_bool_t tb_aicp_ssl_open_done_read(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RECV, tb_false);

	// the ssl
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
	tb_assert_and_check_return_val(ssl && ssl->func.open.func, tb_false);

	// trace
	tb_trace_d("open: done: read: real: %lu, state: %s", aice->u.recv.real, tb_state_cstr(aice->state));

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// failed or closed?
		if (aice->state != TB_STATE_OK)
		{
			state = aice->state;
			break;
		}

		// save the real size
		ssl->read_real = aice->u.recv.real;

		// try opening it
		tb_long_t ok = tb_ssl_open_try(ssl->ssl);

		// trace
		tb_trace_d("open: done: try: %ld", ok);
	
		// ok?
		if (ok > 0)
		{
			// opened
			tb_atomic_set(&ssl->bopened, 1);

			// done func
			ssl->func.open.func(ssl, TB_STATE_OK, ssl->func.open.priv);
		}
		// failed?
		else if (ok < 0)
		{
			// save state
			state = tb_ssl_state(ssl->ssl);
			break;
		}
		// continue 
		else ;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.open.func(ssl, state, ssl->func.open.priv);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_ssl_open_done_writ(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SEND, tb_false);

	// the ssl
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
	tb_assert_and_check_return_val(ssl && ssl->func.open.func, tb_false);

	// trace
	tb_trace_d("open: done: writ: real: %lu, state: %s", aice->u.send.real, tb_state_cstr(aice->state));

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// failed or closed?
		if (aice->state != TB_STATE_OK)
		{
			state = aice->state;
			break;
		}

		// save the real size
		ssl->writ_real = aice->u.send.real;

		// try opening it
		tb_long_t ok = tb_ssl_open_try(ssl->ssl);

		// trace
		tb_trace_d("open: done: try: %ld", ok);
	
		// ok?
		if (ok > 0)
		{
			// opened
			tb_atomic_set(&ssl->bopened, 1);

			// done func
			ssl->func.open.func(ssl, TB_STATE_OK, ssl->func.open.priv);
		}
		// failed?
		else if (ok < 0)
		{
			// save state
			state = tb_ssl_state(ssl->ssl);
			break;
		}
		// continue 
		else ;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.open.func(ssl, state, ssl->func.open.priv);
	}

	// ok
	return tb_true;
}
static tb_long_t tb_aicp_ssl_open_post_read(tb_pointer_t priv, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)priv;
	tb_assert_and_check_return_val(ssl && ssl->func.open.func, -1);

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// try read it
		tb_long_t real = tb_aicp_ssl_try_read(ssl, data, size);
		tb_check_break(real >= 0);

		// ok?
		tb_check_return_val(!real, real);

		// resize data
		if (tb_pbuffer_size(&ssl->read_data) < size)
			tb_pbuffer_resize(&ssl->read_data, size);

		// the data and size
		tb_byte_t* 	read_data = tb_pbuffer_data(&ssl->read_data);
		tb_size_t 	read_size = tb_pbuffer_size(&ssl->read_data);
		tb_assert_and_check_break(read_data && read_size && size <= read_size);

		// check
		tb_assert_and_check_break(ssl->aico);

		// trace
		tb_trace_d("open: post: read: %lu: ..", size);

		// post read
		if (!tb_aico_recv(ssl->aico, read_data, size, tb_aicp_ssl_open_done_read, ssl)) break;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.open.func(ssl, state, ssl->func.open.priv);
	}

	// read failed or continue?
	return state != TB_STATE_OK? -1 : 0;
}
static tb_long_t tb_aicp_ssl_open_post_writ(tb_pointer_t priv, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)priv;
	tb_assert_and_check_return_val(ssl && ssl->func.open.func, -1);

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// try writ it
		tb_long_t real = tb_aicp_ssl_try_writ(ssl, data, size);
		tb_check_break(real >= 0);

		// ok?
		tb_check_return_val(!real, real);

		// save data
		tb_pbuffer_memncpy(&ssl->writ_data, data, size);

		// the data and size
		tb_byte_t* 	writ_data = tb_pbuffer_data(&ssl->writ_data);
		tb_size_t 	writ_size = tb_pbuffer_size(&ssl->writ_data);
		tb_assert_and_check_break(writ_data && writ_size && size == writ_size);

		// check
		tb_assert_and_check_break(ssl->aico);

		// trace
		tb_trace_d("open: post: writ: %lu: ..", writ_size);

		// post writ
		if (!tb_aico_send(ssl->aico, writ_data, writ_size, tb_aicp_ssl_open_done_writ, ssl)) break;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.open.func(ssl, state, ssl->func.open.priv);
	}

	// ok?
	return state != TB_STATE_OK? -1 : 0;
}
static tb_bool_t tb_aicp_ssl_read_done_read(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RECV, tb_false);

	// the ssl
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
	tb_assert_and_check_return_val(ssl && ssl->func.read.func, tb_false);

	// trace
	tb_trace_d("read: done: read: real: %lu, state: %s", aice->u.recv.real, tb_state_cstr(aice->state));

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// failed or closed?
		if (aice->state != TB_STATE_OK)
		{
			state = aice->state;
			break;
		}

		// save the real size
		ssl->read_real = aice->u.recv.real;

		// trace
		tb_trace_d("read: done: try: %lu: ..", ssl->func.read.size);
	
		// try reading it
		tb_long_t real = tb_ssl_read(ssl->ssl, ssl->func.read.data, ssl->func.read.size);

		// trace
		tb_trace_d("read: done: try: %lu: %ld", ssl->func.read.size, real);
	
		// ok?
		if (real > 0)
		{
			// done func
			ssl->func.read.func(ssl, TB_STATE_OK, ssl->func.read.data, real, ssl->func.read.size, ssl->func.read.priv);
		}
		// failed?
		else if (real < 0)
		{
			// save state
			state = tb_ssl_state(ssl->ssl);
			break;
		}
		// continue 
		else ;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.read.func(ssl, state, ssl->func.read.data, 0, ssl->func.read.size, ssl->func.read.priv);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_ssl_read_done_writ(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SEND, tb_false);

	// the ssl
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
	tb_assert_and_check_return_val(ssl && ssl->func.read.func, tb_false);

	// trace
	tb_trace_d("read: done: writ: real: %lu, state: %s", aice->u.send.real, tb_state_cstr(aice->state));

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// failed or closed?
		if (aice->state != TB_STATE_OK)
		{
			state = aice->state;
			break;
		}

		// save the real size
		ssl->writ_real = aice->u.send.real;

		// trace
		tb_trace_d("read: done: try: %lu: ..", ssl->func.read.size);
	
		// try reading it
		tb_long_t real = tb_ssl_read(ssl->ssl, ssl->func.read.data, ssl->func.read.size);

		// trace
		tb_trace_d("read: done: try: %lu: %ld", ssl->func.read.size, real);
	
		// ok?
		if (real > 0)
		{
			// done func
			ssl->func.read.func(ssl, TB_STATE_OK, ssl->func.read.data, real, ssl->func.read.size, ssl->func.read.priv);
		}
		// failed?
		else if (real < 0)
		{
			// save state
			state = tb_ssl_state(ssl->ssl);
			break;
		}
		// continue 
		else ;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.read.func(ssl, state, ssl->func.read.data, 0, ssl->func.read.size, ssl->func.read.priv);
	}

	// ok
	return tb_true;
}
static tb_long_t tb_aicp_ssl_read_post_read(tb_pointer_t priv, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)priv;
	tb_assert_and_check_return_val(ssl && ssl->func.read.func, -1);

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// try read it
		tb_long_t real = tb_aicp_ssl_try_read(ssl, data, size);
		tb_check_break(real >= 0);

		// ok?
		tb_check_return_val(!real, real);

		// resize data
		if (tb_pbuffer_size(&ssl->read_data) < size)
			tb_pbuffer_resize(&ssl->read_data, size);

		// the data and size
		tb_byte_t* 	read_data = tb_pbuffer_data(&ssl->read_data);
		tb_size_t 	read_size = tb_pbuffer_size(&ssl->read_data);
		tb_assert_and_check_break(read_data && read_size && size <= read_size);

		// check
		tb_assert_and_check_break(ssl->aico);

		// trace
		tb_trace_d("read: post: read: %lu: ..", size);

		// post read
		if (!tb_aico_recv_after(ssl->aico, ssl->func.read.delay, read_data, size, tb_aicp_ssl_read_done_read, ssl)) break;

		// delay only for first
		ssl->func.read.delay = 0;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.read.func(ssl, state, ssl->func.read.data, 0, ssl->func.read.size, ssl->func.read.priv);
	}

	// read failed or continue?
	return state != TB_STATE_OK? -1 : 0;
}
static tb_long_t tb_aicp_ssl_read_post_writ(tb_pointer_t priv, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)priv;
	tb_assert_and_check_return_val(ssl && ssl->func.read.func, -1);

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// try writ it
		tb_long_t real = tb_aicp_ssl_try_writ(ssl, data, size);
		tb_check_break(real >= 0);

		// ok?
		tb_check_return_val(!real, real);

		// save data
		tb_pbuffer_memncpy(&ssl->writ_data, data, size);

		// the data and size
		tb_byte_t* 	writ_data = tb_pbuffer_data(&ssl->writ_data);
		tb_size_t 	writ_size = tb_pbuffer_size(&ssl->writ_data);
		tb_assert_and_check_break(writ_data && writ_size && size == writ_size);

		// check
		tb_assert_and_check_break(ssl->aico);

		// trace
		tb_trace_d("read: post: writ: %lu: ..", writ_size);

		// post writ
		if (!tb_aico_send_after(ssl->aico, ssl->func.read.delay, writ_data, writ_size, tb_aicp_ssl_read_done_writ, ssl)) break;

		// delay only for first
		ssl->func.read.delay = 0;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.read.func(ssl, state, ssl->func.read.data, 0, ssl->func.read.size, ssl->func.read.priv);
	}

	// ok?
	return state != TB_STATE_OK? -1 : 0;
}
static tb_bool_t tb_aicp_ssl_writ_done_read(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RECV, tb_false);

	// the ssl
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
	tb_assert_and_check_return_val(ssl && ssl->func.writ.func, tb_false);

	// trace
	tb_trace_d("writ: done: read: real: %lu, state: %s", aice->u.recv.real, tb_state_cstr(aice->state));

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// failed or closed?
		if (aice->state != TB_STATE_OK)
		{
			state = aice->state;
			break;
		}

		// save the real size
		ssl->read_real = aice->u.recv.real;

		// trace
		tb_trace_d("writ: done: try: %lu: ..", ssl->func.writ.size);

		// try writing it
		tb_long_t real = tb_ssl_writ(ssl->ssl, ssl->func.writ.data, ssl->func.writ.size);

		// trace
		tb_trace_d("writ: done: try: %lu: %ld", ssl->func.writ.size, real);
	
		// ok?
		if (real > 0)
		{
			// done func
			ssl->func.writ.func(ssl, TB_STATE_OK, ssl->func.writ.data, real, ssl->func.writ.size, ssl->func.writ.priv);
		}
		// failed?
		else if (real < 0)
		{
			// save state
			state = tb_ssl_state(ssl->ssl);
			break;
		}
		// continue 
		else ;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.writ.func(ssl, state, ssl->func.writ.data, 0, ssl->func.writ.size, ssl->func.writ.priv);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_ssl_writ_done_writ(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SEND, tb_false);

	// the ssl
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
	tb_assert_and_check_return_val(ssl && ssl->func.writ.func, tb_false);

	// trace
	tb_trace_d("writ: done: writ: real: %lu, state: %s", aice->u.send.real, tb_state_cstr(aice->state));

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// failed or closed?
		if (aice->state != TB_STATE_OK)
		{
			state = aice->state;
			break;
		}

		// save the real size
		ssl->writ_real = aice->u.send.real;

		// trace
		tb_trace_d("writ: done: try: %lu: ..", ssl->func.writ.size);

		// try writing it
		tb_long_t real = tb_ssl_writ(ssl->ssl, ssl->func.writ.data, ssl->func.writ.size);

		// trace
		tb_trace_d("writ: done: try: %lu: %ld", ssl->func.writ.size, real);
	
		// ok?
		if (real > 0)
		{
			// done func
			ssl->func.writ.func(ssl, TB_STATE_OK, ssl->func.writ.data, real, ssl->func.writ.size, ssl->func.writ.priv);
		}
		// failed?
		else if (real < 0)
		{
			// save state
			state = tb_ssl_state(ssl->ssl);
			break;
		}
		// continue 
		else ;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.writ.func(ssl, state, ssl->func.writ.data, 0, ssl->func.writ.size, ssl->func.writ.priv);
	}

	// ok
	return tb_true;
}
static tb_long_t tb_aicp_ssl_writ_post_read(tb_pointer_t priv, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)priv;
	tb_assert_and_check_return_val(ssl && ssl->func.writ.func, -1);

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// try read it
		tb_long_t real = tb_aicp_ssl_try_read(ssl, data, size);
		tb_check_break(real >= 0);

		// ok?
		tb_check_return_val(!real, real);

		// resize data
		if (tb_pbuffer_size(&ssl->read_data) < size)
			tb_pbuffer_resize(&ssl->read_data, size);

		// the data and size
		tb_byte_t* 	read_data = tb_pbuffer_data(&ssl->read_data);
		tb_size_t 	read_size = tb_pbuffer_size(&ssl->read_data);
		tb_assert_and_check_break(read_data && read_size && size <= read_size);

		// trace
		tb_trace_d("writ: post: read: %lu: ..", size);

		// check
		tb_assert_and_check_break(ssl->aico);

		// post read
		if (!tb_aico_recv_after(ssl->aico, ssl->func.writ.delay, read_data, size, tb_aicp_ssl_writ_done_read, ssl)) break;

		// delay only for first
		ssl->func.writ.delay = 0;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.writ.func(ssl, state, ssl->func.writ.data, 0, ssl->func.writ.size, ssl->func.writ.priv);
	}

	// read failed or continue?
	return state != TB_STATE_OK? -1 : 0;
}
static tb_long_t tb_aicp_ssl_writ_post_writ(tb_pointer_t priv, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)priv;
	tb_assert_and_check_return_val(ssl && ssl->func.writ.func, -1);

	// done
	tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
	do
	{
		// try writ it
		tb_long_t real = tb_aicp_ssl_try_writ(ssl, data, size);
		tb_check_break(real >= 0);

		// ok?
		tb_check_return_val(!real, real);

		// save data
		tb_pbuffer_memncpy(&ssl->writ_data, data, size);

		// the data and size
		tb_byte_t* 	writ_data = tb_pbuffer_data(&ssl->writ_data);
		tb_size_t 	writ_size = tb_pbuffer_size(&ssl->writ_data);
		tb_assert_and_check_break(writ_data && writ_size && size == writ_size);

		// trace
		tb_trace_d("writ: post: writ: %lu: ..", writ_size);

		// check
		tb_assert_and_check_break(ssl->aico);

		// post writ
		if (!tb_aico_send_after(ssl->aico, ssl->func.writ.delay, writ_data, writ_size, tb_aicp_ssl_writ_done_writ, ssl)) break;

		// delay only for first
		ssl->func.writ.delay = 0;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ssl->func.writ.func(ssl, state, ssl->func.writ.data, 0, ssl->func.writ.size, ssl->func.writ.priv);
	}

	// ok?
	return state != TB_STATE_OK? -1 : 0;
}
static tb_bool_t tb_aicp_ssl_open_and_read(tb_handle_t ssl, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_aicp_ssl_read_t* read = (tb_aicp_ssl_read_t*)priv;
	tb_assert_and_check_return_val(ssl && read && read->func, tb_false);

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// check
		tb_check_break(state == TB_STATE_OK);

		// clear state
		state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;

		// post read
		if (!tb_aicp_ssl_read(ssl, read->data, read->size, read->func, read->priv)) break;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ok = read->func(ssl, state, read->data, 0, read->size, read->priv);
	}

	// ok?
	return ok;
}
static tb_bool_t tb_aicp_ssl_open_and_writ(tb_handle_t ssl, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_aicp_ssl_writ_t* writ = (tb_aicp_ssl_writ_t*)priv;
	tb_assert_and_check_return_val(ssl && writ && writ->func, tb_false);

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// check
		tb_check_break(state == TB_STATE_OK);

		// clear state
		state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;

		// post writ
		if (!tb_aicp_ssl_writ(ssl, writ->data, writ->size, writ->func, writ->priv)) break;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		ok = writ->func(ssl, state, writ->data, 0, writ->size, writ->priv);
	}

	// ok?
	return ok;
}
static tb_bool_t tb_aicp_ssl_done_task(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

	// the ssl
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
	tb_assert_and_check_return_val(ssl && ssl->func.task.func, tb_false);

	// trace
	tb_trace_d("task: done: state: %s", tb_state_cstr(aice->state));

	// done func
	ssl->func.task.func(ssl, aice->state, ssl->func.task.delay, ssl->func.task.priv);

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
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

		// init aicp
		ssl->aicp = aicp;

		// init ssl
		ssl->ssl = tb_ssl_init(bserver);
		tb_assert_and_check_break(ssl->ssl);

		// init read data
		if (!tb_pbuffer_init(&ssl->read_data)) break;

		// init writ data
		if (!tb_pbuffer_init(&ssl->writ_data)) break;

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

	// trace
	tb_trace_d("kill: ..");

	// kill aico
	if (ssl->aico) tb_aico_kill(ssl->aico);
}
tb_void_t tb_aicp_ssl_clos(tb_handle_t handle, tb_bool_t bcalling)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// trace
	tb_trace_d("clos: ..");
	
	// close ssl
	if (ssl->ssl) 
	{		
		// init bio sock, need some blocking time for closing
		tb_ssl_set_bio_sock(ssl->ssl, ssl->sock);

		// close it
		tb_ssl_clos(ssl->ssl);
	}

	// exit aico
	if (ssl->aico) tb_aico_exit(ssl->aico, bcalling);
	ssl->aico = tb_null;

	// clear data
	tb_pbuffer_clear(&ssl->read_data);
	tb_pbuffer_clear(&ssl->writ_data);

	// clear real
	ssl->read_real = 0;
	ssl->writ_real = 0;

	// clear opened
	tb_atomic_set0(&ssl->bopened);

	// trace
	tb_trace_d("clos: ok");	
}
tb_void_t tb_aicp_ssl_exit(tb_handle_t handle, tb_bool_t bcalling)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return(ssl);

	// trace
	tb_trace_d("exit: ..");
	
	// close it first
	tb_aicp_ssl_clos(ssl, bcalling);

	// exit ssl
	if (ssl->ssl) tb_ssl_exit(ssl->ssl);
	ssl->ssl = tb_null;

	// exit data
	tb_pbuffer_exit(&ssl->read_data);
	tb_pbuffer_exit(&ssl->writ_data);

	// exit it
	tb_free(ssl);

	// trace
	tb_trace_d("exit: ok");
	
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
		// opened?
		tb_assert_and_check_break(!tb_atomic_get(&ssl->bopened));

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

		// save func
		ssl->func.open.func = func;
		ssl->func.open.priv = priv;

		// clear real
		ssl->read_real = 0;
		ssl->writ_real = 0;

		// init post func
		tb_ssl_set_bio_func(ssl->ssl, tb_aicp_ssl_open_post_read, tb_aicp_ssl_open_post_writ, tb_null, ssl);

		// try opening it
		tb_long_t r = tb_ssl_open_try(ssl->ssl);

		// ok
		if (r > 0)
		{
			// done func
			func(ssl, TB_STATE_OK, priv);
		}
		// failed?
		else if (r < 0)
		{
			// done func
			func(ssl, tb_ssl_state(ssl->ssl), priv);
			break;
		}
		// continue it
		else ;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
tb_bool_t tb_aicp_ssl_read(tb_handle_t handle, tb_byte_t* data, tb_size_t size, tb_aicp_ssl_read_func_t func, tb_pointer_t priv)
{
	return tb_aicp_ssl_read_after(handle, 0, data, size, func, priv);
}
tb_bool_t tb_aicp_ssl_writ(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_ssl_writ_func_t func, tb_pointer_t priv)
{
	return tb_aicp_ssl_writ_after(handle, 0, data, size, func, priv);
}
tb_bool_t tb_aicp_ssl_read_after(tb_handle_t handle, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_aicp_ssl_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && data && size && func, tb_false);

	// trace
	tb_trace_d("read: %lu, after: %lu", size, delay);
		
	// done
	tb_bool_t ok = tb_false;
	do
	{
		// opened?
		tb_assert_and_check_break(tb_atomic_get(&ssl->bopened));

		// check
		tb_assert_and_check_break(ssl->aicp && ssl->ssl && ssl->aico);

		// save func
		ssl->func.read.func 	= func;
		ssl->func.read.priv 	= priv;
		ssl->func.read.data 	= data;
		ssl->func.read.size 	= size;
		ssl->func.read.delay 	= delay;

		// clear real
		ssl->read_real = 0;
		ssl->writ_real = 0;

		// init post func
		tb_ssl_set_bio_func(ssl->ssl, tb_aicp_ssl_read_post_read, tb_aicp_ssl_read_post_writ, tb_null, ssl);

		// try reading it
		tb_long_t real = tb_ssl_read(ssl->ssl, data, size);

		// ok
		if (real > 0)
		{
			// done func
			func(ssl, TB_STATE_OK, data, real, size, priv);
		}
		// failed?
		else if (real < 0)
		{
			// done func
			func(ssl, tb_ssl_state(ssl->ssl), data, 0, size, priv);
			break;
		}
		// continue it
		else ;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
tb_bool_t tb_aicp_ssl_writ_after(tb_handle_t handle, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_aicp_ssl_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && data && size && func, tb_false);

	// trace
	tb_trace_d("writ: %lu, after: %lu", size, delay);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// opened?
		tb_assert_and_check_break(tb_atomic_get(&ssl->bopened));

		// check
		tb_assert_and_check_break(ssl->aicp && ssl->ssl && ssl->aico);

		// save func
		ssl->func.writ.func 	= func;
		ssl->func.writ.priv 	= priv;
		ssl->func.writ.data 	= data;
		ssl->func.writ.size 	= size;
		ssl->func.writ.delay 	= delay;

		// clear real
		ssl->read_real = 0;
		ssl->writ_real = 0;

		// init post func
		tb_ssl_set_bio_func(ssl->ssl, tb_aicp_ssl_writ_post_read, tb_aicp_ssl_writ_post_writ, tb_null, ssl);

		// try writing it
		tb_long_t real = tb_ssl_writ(ssl->ssl, data, size);

		// ok
		if (real > 0)
		{
			// done func
			func(ssl, TB_STATE_OK, data, real, size, priv);
		}
		// failed?
		else if (real < 0)
		{
			// done func
			func(ssl, tb_ssl_state(ssl->ssl), data, 0, size, priv);
			break;
		}
		// continue it
		else ;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
tb_bool_t tb_aicp_ssl_task(tb_handle_t handle, tb_size_t delay, tb_aicp_ssl_task_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && ssl->aico && func, tb_false);

	// save func
	ssl->func.task.func 	= func;
	ssl->func.task.priv 	= priv;
	ssl->func.task.delay 	= delay;

	// run task
	return tb_aico_task_run(ssl->aico, delay, tb_aicp_ssl_done_task, ssl);
}
tb_bool_t tb_aicp_ssl_oread(tb_handle_t handle, tb_byte_t* data, tb_size_t size, tb_aicp_ssl_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && data && size && func, tb_false);

	// not opened? open it first
	if (!tb_atomic_get(&ssl->bopened))
	{
		ssl->open_and.read.func = func;
		ssl->open_and.read.data = data;
		ssl->open_and.read.size = size;
		ssl->open_and.read.priv = priv;
		return tb_aicp_ssl_open(handle, tb_aicp_ssl_open_and_read, &ssl->open_and.read);
	}

	// read it
	return tb_aicp_ssl_read(handle, data, size, func, priv);
}
tb_bool_t tb_aicp_ssl_owrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_ssl_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl && data && size && func, tb_false);

	// not opened? open it first
	if (!tb_atomic_get(&ssl->bopened))
	{
		ssl->open_and.writ.func = func;
		ssl->open_and.writ.data = data;
		ssl->open_and.writ.size = size;
		ssl->open_and.writ.priv = priv;
		return tb_aicp_ssl_open(handle, tb_aicp_ssl_open_and_writ, &ssl->open_and.writ);
	}

	// writ it
	return tb_aicp_ssl_writ(handle, data, size, func, priv);
}
tb_aicp_t* tb_aicp_ssl_aicp(tb_handle_t handle)
{
	// check
	tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
	tb_assert_and_check_return_val(ssl, tb_null);

	// the aicp
	return ssl->aicp;
}

