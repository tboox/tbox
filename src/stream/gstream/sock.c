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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * \sock		sock.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"sock"

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

	// the dns looker
	tb_handle_t 		looker;

	// the sock type
	tb_size_t 			type : 23;

	// the sock bref
	tb_size_t 			bref : 1;

	// the try number
	tb_size_t 			tryn : 8;

	// the wait event
	tb_long_t 			wait;

	// the read & writ
	tb_size_t 			read;
	tb_size_t 			writ;
	
	// the ssl handle
	tb_handle_t 		ssl;

	// the host address
	tb_ipv4_t 			addr;

}tb_gstream_sock_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_gstream_sock_t* tb_gstream_sock_cast(tb_gstream_t* gstream)
{
	tb_assert_and_check_return_val(gstream && gstream->type == TB_GSTREAM_TYPE_SOCK, tb_null);
	return (tb_gstream_sock_t*)gstream;
}
static tb_long_t tb_gstream_sock_open(tb_gstream_t* gstream)
{
	tb_gstream_sock_t* sst = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sst && sst->type, -1);

	// clear
	sst->wait = 0;
	sst->tryn = 0;
	sst->read = 0;
	sst->writ = 0;

	// no reference?
	tb_check_return_val(!(sst->sock && sst->bref), 1);

	// port
	tb_size_t port = tb_url_port_get(&gstream->url);
	tb_assert_and_check_return_val(port, -1);

	// ipv4
	if (!sst->addr.u32)
	{
		// try to get the ipv4 address from url
		tb_ipv4_t const* ipv4 = tb_url_ipv4_get(&gstream->url);
		if (ipv4 && ipv4->u32) sst->addr = *ipv4;
		else
		{
			// lookup ipv4
			if (sst->looker)
			{
				// spank
				tb_long_t r = tb_dns_looker_spak(sst->looker, &sst->addr);
				tb_assert_and_check_goto(r >= 0, fail);

				// continue?
				tb_check_return_val(r, 0);
			}
			else
			{
				// get the host from url
				tb_char_t const* host = tb_url_host_get(&gstream->url);
				tb_assert_and_check_return_val(host, -1);

				// try get ipv4
				if (!tb_dns_cache_get(host, &sst->addr))
				{
					// init dns
					sst->looker = tb_dns_looker_init(host);
					tb_assert_and_check_return_val(sst->looker, -1);
					
					// spank
					tb_long_t r = tb_dns_looker_spak(sst->looker, &sst->addr);
					tb_assert_and_check_goto(r >= 0, fail);

					// continue?
					tb_check_return_val(r, 0);
				}
			}

			// exit dns if ok
			if (sst->looker) 
			{
				tb_dns_looker_exit(sst->looker);
				sst->looker = tb_null;
			}

			// save addr
			tb_url_ipv4_set(&gstream->url, &sst->addr);
		}

		// tcp or udp? for url: sock://ip:port/?udp=
		tb_char_t const* args = tb_url_args_get(&gstream->url);
		if (args && !tb_strnicmp(args, "udp=", 4)) sst->type = TB_SOCKET_TYPE_UDP;
		else if (args && !tb_strnicmp(args, "tcp=", 4)) sst->type = TB_SOCKET_TYPE_TCP;
	}

	// open
	if (!sst->sock) 
	{
		sst->sock = tb_socket_open(sst->type);
		sst->bref = 0;
	}
	tb_assert_and_check_return_val(sst->sock, -1);

	// init cache 
	tb_size_t cache = tb_socket_recv_buffer_size(sst->sock);
	if (cache) 
	{
		if (!tb_gstream_ctrl(gstream, TB_GSTREAM_CTRL_SET_CACHE, cache)) goto fail;
	}

	// done
	tb_long_t r = -1;
	switch (sst->type)
	{
	case TB_SOCKET_TYPE_TCP:
		{
			// trace
			tb_trace_impl("connect: try: %s[%u.%u.%u.%u]:%u", tb_url_host_get(&gstream->url), sst->addr.u8[0], sst->addr.u8[1], sst->addr.u8[2], sst->addr.u8[3], port);

			// connect it
			r = tb_socket_connect(sst->sock, &sst->addr, port);
			tb_check_return_val(r > 0, r);

			// ok
			tb_trace_impl("connect: ok");

			// ssl? init it
			if (tb_url_ssl_get(&gstream->url))
			{
				// init
				if (gstream->sfunc.init) sst->ssl = gstream->sfunc.init(gstream);
				tb_assert_and_check_goto(sst->ssl, fail);

				// check sfunc
				tb_assert_and_check_goto(gstream->sfunc.read, fail);
				tb_assert_and_check_goto(gstream->sfunc.writ, fail);
				tb_assert_and_check_goto(gstream->sfunc.exit, fail);
			}
		}
		break;
	case TB_SOCKET_TYPE_UDP:
		r = 1;
		break;
	default:
		break;
	}

	// save state
	gstream->state = TB_GSTREAM_STATE_OK;

	// ok?
	return r;

fail:

	// failed?
	if (sst)
	{
		// dns failed?
		if (sst->looker)
		{
			// save state
			gstream->state = TB_GSTREAM_SOCK_STATE_DNS_FAILED;

			// exit dns
			tb_dns_looker_exit(sst->looker);
			sst->looker = tb_null;
		}
		// ssl or connect failed?
		else if (sst->type == TB_SOCKET_TYPE_TCP)
			gstream->state = tb_url_ssl_get(&gstream->url)? TB_GSTREAM_SOCK_STATE_SSL_FAILED : TB_GSTREAM_SOCK_STATE_CONNECT_FAILED;
	}

	return -1;
}
static tb_long_t tb_gstream_sock_close(tb_gstream_t* gstream)
{
	tb_gstream_sock_t* sst = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sst, -1);

	// has socket?
	if (sst->sock)
	{
		// exit ssl
		if (sst->ssl) gstream->sfunc.exit(sst->ssl);
		sst->ssl = tb_null;

		// close it
		if (!sst->bref) if (!tb_socket_close(sst->sock)) return 0;

		// reset
		sst->sock = tb_null;
		sst->bref = 0;
	}

	// clear 
	sst->wait = 0;
	sst->tryn = 0;
	sst->read = 0;
	sst->writ = 0;
	tb_ipv4_clr(&sst->addr);

	// ok
	return 1;
}
static tb_long_t tb_gstream_sock_read(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_gstream_sock_t* sst = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sst && sst->sock, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// clear writ
	sst->writ = 0;

	// read
	tb_long_t r = -1;
	switch (sst->type)
	{
	case TB_SOCKET_TYPE_TCP:
		{
			// read data
			r = (sst->ssl)? gstream->sfunc.read(sst->ssl, data, size) : tb_socket_recv(sst->sock, data, size);
			tb_trace_impl("read: %ld <? %lu", r, size);
			tb_check_return_val(r >= 0, -1);

			// abort?
			if (!r && sst->wait > 0 && (sst->wait & TB_AIOE_CODE_RECV)) return -1;

			// clear wait
			if (r > 0) sst->wait = 0;
		}
		break;
	case TB_SOCKET_TYPE_UDP:
		{
			// port
			tb_size_t port = tb_url_port_get(&gstream->url);
			tb_assert_and_check_return_val(port, -1);

			// ipv4
			tb_assert_and_check_return_val(sst->addr.u32, -1);

			// read data
			r = tb_socket_urecv(sst->sock, &sst->addr, port, data, size);
			tb_trace_impl("read: %ld <? %lu", r, size);
			tb_check_return_val(r >= 0, -1);

			// abort?
			if (!r && sst->wait > 0 && (sst->wait & TB_AIOE_CODE_RECV)) return -1;

			// clear wait
			if (r > 0) sst->wait = 0;
		}
		break;
	default:
		break;
	}

	// update read
	sst->read += r;

	// ok?
	return r;
}
static tb_long_t tb_gstream_sock_writ(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_gstream_sock_t* sst = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sst && sst->sock, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// clear read
	sst->read = 0;

	// writ 
	tb_long_t r = -1;
	switch (sst->type)
	{
	case TB_SOCKET_TYPE_TCP:
		{
			// writ data
			r = (sst->ssl)? gstream->sfunc.writ(sst->ssl, data, size) : tb_socket_send(sst->sock, data, size);
			tb_trace_impl("writ: %ld <? %lu", r, size);
			tb_check_return_val(r >= 0, -1);

			// abort?
			if (!r && sst->wait > 0 && (sst->wait & TB_AIOE_CODE_SEND)) return -1;

			// clear wait
			if (r > 0) sst->wait = 0;
		}
		break;
	case TB_SOCKET_TYPE_UDP:
		{
			// port
			tb_size_t port = tb_url_port_get(&gstream->url);
			tb_assert_and_check_return_val(port, -1);

			// ipv4
			tb_assert_and_check_return_val(sst->addr.u32, -1);

			// writ data
			r = tb_socket_usend(sst->sock, &sst->addr, port, data, size);
			tb_trace_impl("writ: %ld <? %lu", r, size);
			tb_check_return_val(r >= 0, -1);

			// no data?
			if (!r)
			{
				// abort? writ x, writ 0, or writ 0, writ 0
				tb_check_return_val(!sst->writ && !sst->tryn, -1);

				// tryn++
				sst->tryn++;
			}
			else sst->tryn = 0;
		}
		break;
	default:
		break;
	}

	// update writ
	sst->writ += r;

	// ok?
	return r;
}
static tb_long_t tb_gstream_sock_wait(tb_gstream_t* gstream, tb_size_t wait, tb_long_t timeout)
{
	tb_gstream_sock_t* sst = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sst, -1);

	if (!sst->looker)
	{
		// check socket
		tb_assert_and_check_return_val(sst->sock, -1);

		// wait the gstream
		sst->wait = tb_aioo_wait(sst->sock, wait, timeout);
		tb_trace_impl("wait: %ld", sst->wait);
	}
	else
	{
		// wait the dns
		sst->wait = tb_dns_looker_wait(sst->looker, timeout);
		tb_trace_impl("wait: %ld", sst->wait);

		// clear tryn
		if (sst->wait) sst->tryn = 0;
	}

	return sst->wait;
}
static tb_bool_t tb_gstream_sock_ctrl(tb_gstream_t* gstream, tb_size_t ctrl, tb_va_list_t args)
{
	tb_gstream_sock_t* sst = tb_gstream_sock_cast(gstream);
	tb_assert_and_check_return_val(sst, tb_false);

	switch (ctrl)
	{
	case TB_GSTREAM_CTRL_SOCK_SET_TYPE:
		{
			// check
			tb_assert_and_check_return_val(!gstream->bopened, tb_false);

			// the type
			tb_size_t type = (tb_size_t)tb_va_arg(args, tb_size_t);
			tb_assert_and_check_return_val(type == TB_SOCKET_TYPE_TCP || type == TB_SOCKET_TYPE_UDP, tb_false);
			
			// changed? exit the old sock
			if (sst->type != type)
			{
				// exit it
				if (!sst->bref && sst->sock) tb_socket_close(sst->sock);
				sst->sock = tb_null;
				sst->bref = 0;
			}

			// set type
			sst->type = type;

			// ok
			return tb_true;
		}
	case TB_GSTREAM_CTRL_SOCK_SET_HANDLE:
		{
			// check
			tb_assert_and_check_return_val(!gstream->bopened, tb_false);
			
			// the sock
			tb_handle_t sock = (tb_handle_t)tb_va_arg(args, tb_handle_t);

			// changed? exit the old sock
			if (sst->sock != sock)
			{
				// exit it
				if (!sst->bref && sst->sock) tb_socket_close(sst->sock);
			}

			// set sock
			sst->sock = sock;
			sst->bref = sock? 1 : 0;

			// ok
			return tb_true;
		}
	case TB_GSTREAM_CTRL_SOCK_GET_HANDLE:
		{
			tb_handle_t* phandle = (tb_handle_t)tb_va_arg(args, tb_handle_t*);
			tb_assert_and_check_return_val(phandle, tb_false);
			*phandle = sst->sock;
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
	if (!tb_gstream_init((tb_gstream_t*)gstream, TB_GSTREAM_TYPE_SOCK)) goto fail;
	gstream->base.open 	= tb_gstream_sock_open;
	gstream->base.close = tb_gstream_sock_close;
	gstream->base.read 	= tb_gstream_sock_read;
	gstream->base.writ 	= tb_gstream_sock_writ;
	gstream->base.ctrl 	= tb_gstream_sock_ctrl;
	gstream->base.wait 	= tb_gstream_sock_wait;
	gstream->sock 		= tb_null;
	gstream->type 		= TB_SOCKET_TYPE_TCP;

	// init sock cache
	if (!tb_gstream_ctrl((tb_gstream_t*)gstream, TB_GSTREAM_CTRL_SET_CACHE, TB_GSTREAM_SOCK_CACHE_MAXN)) goto fail;

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
	if (!tb_gstream_ctrl(gstream, TB_GSTREAM_CTRL_SET_HOST, host)) goto fail;
	if (!tb_gstream_ctrl(gstream, TB_GSTREAM_CTRL_SET_PORT, port)) goto fail;
	if (!tb_gstream_ctrl(gstream, TB_GSTREAM_CTRL_SET_SSL, bssl)) goto fail;
	if (!tb_gstream_ctrl(gstream, TB_GSTREAM_CTRL_SOCK_SET_TYPE, type)) goto fail;
	
	// ok
	return gstream;

fail:
	if (gstream) tb_gstream_exit(gstream);
	return tb_null;
}
