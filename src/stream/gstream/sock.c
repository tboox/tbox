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
 * \sock		sock.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_MODULE_NAME 			"sock"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../asio/asio.h"
#include "../../string/string.h"
#include "../../network/network.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the sock cache maxn
#ifdef __tb_small__
# 	define TB_GSTREAM_SOCK_CACHE_MAXN 	(8192)
#else
# 	define TB_GSTREAM_SOCK_CACHE_MAXN 	(8192 << 1)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the sock stream type
typedef struct __tb_gstream_sock_t
{
	// the base
	tb_gstream_t 		base;

	// the sock handle
	tb_handle_t 		sock;

	// the sock type
	tb_uint32_t 		type 	: 22;

	// the try number
	tb_uint32_t 		tryn 	: 8;

	// the sock bref
	tb_uint32_t 		bref 	: 1;

	// keep alive after being closed?
	tb_uint32_t 		balived : 1;

	// the wait event
	tb_long_t 			wait;

	// the read & writ
	tb_size_t 			read;
	tb_size_t 			writ;
	
	// the host address
	tb_ipv4_t 			addr;

}tb_gstream_sock_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_gstream_sock_t* tb_gstream_sock_cast(tb_handle_t stream)
{
	tb_gstream_t* gstream = (tb_gstream_t*)stream;
	tb_assert_and_check_return_val(gstream && gstream->base.type == TB_STREAM_TYPE_SOCK, tb_null);
	return (tb_gstream_sock_t*)gstream;
}
static tb_bool_t tb_gstream_sock_open(tb_handle_t gstream)
{
	// check
	tb_gstream_sock_t* sstream = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sstream && sstream->type, tb_false);

	// clear
	sstream->wait = 0;
	sstream->tryn = 0;
	sstream->read = 0;
	sstream->writ = 0;

	// opened?
	tb_check_return_val(!sstream->sock, tb_true);

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
				sstream->base.state = TB_STREAM_SOCK_STATE_DNS_FAILED;
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
	tb_assert_and_check_return_val(sstream->sock, tb_false);
	
	// resize cache 
	tb_size_t recv_cache = tb_socket_recv_buffer_size(sstream->sock);
	tb_size_t send_cache = tb_socket_send_buffer_size(sstream->sock);
	if (recv_cache || send_cache) tb_stream_ctrl(gstream, TB_STREAM_CTRL_SET_CACHE, tb_max(recv_cache, send_cache));

	// done
	tb_bool_t ok = tb_false;
	switch (sstream->type)
	{
	case TB_SOCKET_TYPE_TCP:
		{
			// trace
			tb_trace_d("connect: %s[%u.%u.%u.%u]:%u: ..", tb_url_host_get(&sstream->base.base.url), tb_ipv4_u8x4(sstream->addr), port);

			// connect it
			tb_long_t r = -1;
			while (!(r = tb_socket_connect(sstream->sock, &sstream->addr, port)) && !tb_atomic_get(&sstream->base.base.bstoped))
			{
				r = tb_aioo_wait(sstream->sock, TB_AIOE_CODE_CONN, tb_stream_timeout(gstream));
				tb_check_break(r > 0);
			}

			// ok?
			if (r > 0)
			{
				ok = tb_true;
				sstream->base.state = TB_STREAM_STATE_OK;
			}
			else sstream->base.state = !r? TB_STREAM_SOCK_STATE_CONNECT_TIMEOUT : TB_STREAM_SOCK_STATE_CONNECT_FAILED;

			// ok
			tb_trace_d("connect: %s", ok? "ok" : "no");
		}
		break;
	case TB_SOCKET_TYPE_UDP:
		ok = tb_true;
		sstream->base.state = TB_STREAM_STATE_OK;
		break;
	default:
		break;
	}

	// ok?
	return ok;
}
static tb_bool_t tb_gstream_sock_clos(tb_handle_t gstream)
{
	// check
	tb_gstream_sock_t* sstream = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sstream, tb_false);

	// keep alive? not close it
	tb_check_return_val(!sstream->balived, tb_true);

	// close it
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
static tb_void_t tb_gstream_sock_exit(tb_handle_t gstream)
{
	// check
	tb_gstream_sock_t* sstream = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return(sstream);

	// close it
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
static tb_void_t tb_gstream_sock_kill(tb_handle_t gstream)
{
	// check
	tb_gstream_sock_t* sstream = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return(sstream);

	// kill it
	if (sstream->sock) tb_socket_kill(sstream->sock, TB_SOCKET_KILL_RW);
}
static tb_long_t tb_gstream_sock_read(tb_handle_t gstream, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_gstream_sock_t* sstream = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sstream && sstream->sock, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// clear writ
	sstream->writ = 0;

	// read
	tb_long_t r = -1;
	switch (sstream->type)
	{
	case TB_SOCKET_TYPE_TCP:
		{
			// read data
			r = /*(sstream->ssl)? gstream->sfunc.read(sstream->ssl, data, size) : */tb_socket_recv(sstream->sock, data, size);
			tb_trace_d("read: %ld <? %lu", r, size);
			tb_check_return_val(r >= 0, -1);

			// abort?
			if (!r && sstream->wait > 0 && (sstream->wait & TB_AIOE_CODE_RECV)) return -1;

			// clear wait
			if (r > 0) sstream->wait = 0;
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
			r = tb_socket_urecv(sstream->sock, &sstream->addr, port, data, size);
			tb_trace_d("read: %ld <? %lu", r, size);
			tb_check_return_val(r >= 0, -1);

			// abort?
			if (!r && sstream->wait > 0 && (sstream->wait & TB_AIOE_CODE_RECV)) return -1;

			// clear wait
			if (r > 0) sstream->wait = 0;
		}
		break;
	default:
		break;
	}

	// update read
	sstream->read += r;

	// ok?
	return r;
}
static tb_long_t tb_gstream_sock_writ(tb_handle_t gstream, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_gstream_sock_t* sstream = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sstream && sstream->sock, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// clear read
	sstream->read = 0;

	// writ 
	tb_long_t r = -1;
	switch (sstream->type)
	{
	case TB_SOCKET_TYPE_TCP:
		{
			// writ data
			r = /*(sstream->ssl)? gstream->sfunc.writ(sstream->ssl, data, size) : */tb_socket_send(sstream->sock, data, size);
			tb_trace_d("writ: %ld <? %lu", r, size);
			tb_check_return_val(r >= 0, -1);

			// abort?
			if (!r && sstream->wait > 0 && (sstream->wait & TB_AIOE_CODE_SEND)) return -1;

			// clear wait
			if (r > 0) sstream->wait = 0;
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
			r = tb_socket_usend(sstream->sock, &sstream->addr, port, data, size);
			tb_trace_d("writ: %ld <? %lu", r, size);
			tb_check_return_val(r >= 0, -1);

			// no data?
			if (!r)
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
	sstream->writ += r;

	// ok?
	return r;
}
static tb_long_t tb_gstream_sock_wait(tb_handle_t gstream, tb_size_t wait, tb_long_t timeout)
{
	// check
	tb_gstream_sock_t* sstream = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sstream && sstream->sock, -1);

	// wait 
	sstream->wait = tb_aioo_wait(sstream->sock, wait, timeout);
	tb_trace_d("wait: %ld", sstream->wait);

	// ok?
	return sstream->wait;
}
static tb_bool_t tb_gstream_sock_ctrl(tb_handle_t gstream, tb_size_t ctrl, tb_va_list_t args)
{
	tb_gstream_sock_t* sstream = tb_gstream_sock_cast(gstream);
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
			tb_assert_and_check_return_val(!tb_stream_is_opened(gstream), tb_false);

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
			tb_assert_and_check_return_val(!tb_stream_is_opened(gstream), tb_false);
			
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
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_t* tb_gstream_init_sock()
{
	// make stream
	tb_gstream_sock_t* gstream = (tb_gstream_sock_t*)tb_malloc0(sizeof(tb_gstream_sock_t));
	tb_assert_and_check_return_val(gstream, tb_null);

	// init stream
	if (!tb_gstream_init((tb_gstream_t*)gstream, TB_STREAM_TYPE_SOCK, TB_GSTREAM_SOCK_CACHE_MAXN)) goto fail;
	gstream->base.open 		= tb_gstream_sock_open;
	gstream->base.clos 		= tb_gstream_sock_clos;
	gstream->base.exit 		= tb_gstream_sock_exit;
	gstream->base.read 		= tb_gstream_sock_read;
	gstream->base.writ 		= tb_gstream_sock_writ;
	gstream->base.wait 		= tb_gstream_sock_wait;
	gstream->base.base.ctrl = tb_gstream_sock_ctrl;
	gstream->base.base.kill	= tb_gstream_sock_kill;
	gstream->sock 			= tb_null;
	gstream->type 			= TB_SOCKET_TYPE_TCP;

	// ok
	return (tb_gstream_t*)gstream;

fail:
	if (gstream) tb_gstream_exit((tb_gstream_t*)gstream);
	return tb_null;
}

tb_gstream_t* tb_gstream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
	// check
	tb_assert_and_check_return_val(host && port, tb_null);

	// init stream
	tb_gstream_t* gstream = tb_gstream_init_sock();
	tb_assert_and_check_return_val(gstream, tb_null);

	// ctrl
	if (!tb_stream_ctrl(gstream, TB_STREAM_CTRL_SET_HOST, host)) goto fail;
	if (!tb_stream_ctrl(gstream, TB_STREAM_CTRL_SET_PORT, port)) goto fail;
	if (!tb_stream_ctrl(gstream, TB_STREAM_CTRL_SET_SSL, bssl)) goto fail;
	if (!tb_stream_ctrl(gstream, TB_STREAM_CTRL_SOCK_SET_TYPE, type)) goto fail;
	
	// ok
	return gstream;

fail:
	if (gstream) tb_gstream_exit(gstream);
	return tb_null;
}
