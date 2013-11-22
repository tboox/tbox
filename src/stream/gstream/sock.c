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
#define TB_SSTREAM_HOST_MAX 		(1024)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the sock stream type
typedef struct __tb_sstream_t
{
	// the base
	tb_gstream_t 		base;

	// the sock handle
	tb_handle_t 		sock;

	// the dns handle
	tb_handle_t 		hdns;

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

	// the ipv4 string
	tb_char_t 			ipv4[16];

}tb_sstream_t;


/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_sstream_t* tb_sstream_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_SOCK, tb_null);
	return (tb_sstream_t*)gst;
}
static tb_long_t tb_sstream_aopen(tb_gstream_t* gst)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, -1);

	// clear
	sst->wait = 0;
	sst->tryn = 0;
	sst->read = 0;
	sst->writ = 0;

	// no reference?
	tb_check_return_val(!(sst->sock && sst->bref), 1);

	// port
	tb_size_t port = tb_url_port_get(&gst->url);
	tb_assert_and_check_return_val(port, -1);

	// ipv4
	tb_char_t const* host = tb_null;
	if (!sst->ipv4[0])
	{
		tb_ipv4_t const* ipv4 = tb_url_ipv4_get(&gst->url);
		tb_assert_and_check_return_val(ipv4, -1);
		if (!ipv4->u32)
		{
			// lookup ipv4
			tb_ipv4_t addr;
			if (sst->hdns)
			{
				// spank
				tb_long_t r = tb_dns_look_spak(sst->hdns, &addr);
				tb_assert_and_check_goto(r >= 0, fail);

				// continue?
				tb_check_return_val(r, 0);
			}
			else
			{
				// host
				host = tb_url_host_get(&gst->url);
				tb_assert_and_check_return_val(host, -1);

				// init dns list
				tb_dns_list_init();
			
				// try get ipv4
				if (!tb_dns_look_try4(host, &addr))
				{
					// init dns
					sst->hdns = tb_dns_look_init(host);
					tb_assert_and_check_return_val(sst->hdns, -1);
					
					// spank
					tb_long_t r = tb_dns_look_spak(sst->hdns, &addr);
					tb_assert_and_check_goto(r >= 0, fail);

					// continue?
					tb_check_return_val(r, 0);
				}
			}

			// exit dns if ok
			if (sst->hdns) 
			{
				tb_dns_look_exit(sst->hdns);
				sst->hdns = tb_null;
			}

			// ipv4 => host
			host = tb_ipv4_get(&addr, sst->ipv4, 16);
			tb_assert_and_check_goto(host, fail);

			// set ipv4
			tb_url_ipv4_set(&gst->url, &addr);
		}
		else 
		{
			// ipv4 => host
			host = tb_ipv4_get(ipv4, sst->ipv4, 16);
			tb_assert_and_check_goto(host, fail);
		}

		// tcp or udp? for url: sock://ip:port/?udp=
		tb_char_t const* args = tb_url_args_get(&gst->url);
		if (args && !tb_strnicmp(args, "udp=", 4)) sst->type = TB_SOCKET_TYPE_UDP;
		else if (args && !tb_strnicmp(args, "tcp=", 4)) sst->type = TB_SOCKET_TYPE_TCP;
	}
	else host = sst->ipv4;

	// open
	if (!sst->sock) sst->sock = tb_socket_open(sst->type);
	tb_assert_and_check_return_val(sst->sock, -1);

	// tcp
	tb_long_t r = -1;
	switch (sst->type)
	{
	case TB_SOCKET_TYPE_TCP:
		{
			// connect
			tb_trace_impl("connect: try: %s[%s]:%u", tb_url_host_get(&gst->url), host, port);
			r = tb_socket_connect(sst->sock, host, port);
			tb_check_return_val(r > 0, r);

			// ok
			tb_trace_impl("connect: ok");

			// ssl? init it
			if (tb_url_ssl_get(&gst->url))
			{
				// init
				if (gst->sfunc.init) sst->ssl = gst->sfunc.init(gst);
				tb_assert_and_check_goto(sst->ssl, fail);

				// check sfunc
				tb_assert_and_check_goto(gst->sfunc.read, fail);
				tb_assert_and_check_goto(gst->sfunc.writ, fail);
				tb_assert_and_check_goto(gst->sfunc.exit, fail);
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
	gst->state = TB_GSTREAM_STATE_OK;

	// ok?
	return r;

fail:

	// failed?
	if (sst)
	{
		// dns failed?
		if (sst->hdns)
		{
			// save state
			gst->state = TB_SSTREAM_STATE_DNS_FAILED;

			// exit dns
			tb_dns_look_exit(sst->hdns);
			sst->hdns = tb_null;
		}
		// ssl or connect failed?
		else if (sst->type == TB_SOCKET_TYPE_TCP)
			gst->state = tb_url_ssl_get(&gst->url)? TB_SSTREAM_STATE_SSL_FAILED : TB_SSTREAM_STATE_CONNECT_FAILED;
	}

	return -1;
}
static tb_long_t tb_sstream_aclose(tb_gstream_t* gst)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, -1);

	// has socket?
	if (sst->sock)
	{
		// exit ssl
		if (sst->ssl) gst->sfunc.exit(sst->ssl);
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
	sst->ipv4[0] = '\0';

	// ok
	return 1;
}
static tb_long_t tb_sstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
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
			r = (sst->ssl)? gst->sfunc.read(sst->ssl, data, size) : tb_socket_recv(sst->sock, data, size);
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
			tb_size_t port = tb_url_port_get(&gst->url);
			tb_assert_and_check_return_val(port, -1);

			// ipv4
			tb_assert_and_check_return_val(sst->ipv4[0], -1);

			// read data
			r = tb_socket_urecv(sst->sock, sst->ipv4, port, data, size);
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
static tb_long_t tb_sstream_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
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
			r = (sst->ssl)? gst->sfunc.writ(sst->ssl, data, size) : tb_socket_send(sst->sock, data, size);
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
			tb_size_t port = tb_url_port_get(&gst->url);
			tb_assert_and_check_return_val(port, -1);

			// ipv4
			tb_assert_and_check_return_val(sst->ipv4[0], -1);

			// writ data
			r = tb_socket_usend(sst->sock, sst->ipv4, port, data, size);
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
static tb_long_t tb_sstream_wait(tb_gstream_t* gst, tb_size_t wait, tb_long_t timeout)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, -1);

	if (!sst->hdns)
	{
		// check socket
		tb_assert_and_check_return_val(sst->sock, -1);

		// wait the gst
		sst->wait = tb_aioo_wait(sst->sock, wait, timeout);
		tb_trace_impl("wait: %ld", sst->wait);
	}
	else
	{
		// wait the dns
		sst->wait = tb_dns_look_wait(sst->hdns, timeout);
		tb_trace_impl("wait: %ld", sst->wait);

		// clear tryn
		if (sst->wait) sst->tryn = 0;
	}

	return sst->wait;
}
static tb_bool_t tb_sstream_ctrl(tb_gstream_t* gst, tb_size_t ctrl, tb_va_list_t args)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, tb_false);

	switch (ctrl)
	{
	case TB_SSTREAM_CTRL_SET_TYPE:
		{
			tb_assert_and_check_return_val(!gst->bopened, tb_false);
			tb_size_t type = (tb_size_t)tb_va_arg(args, tb_size_t);
			tb_assert_and_check_return_val(type == TB_SOCKET_TYPE_TCP || type == TB_SOCKET_TYPE_UDP, tb_false);
			sst->type = type;
			return tb_true;
		}
	case TB_SSTREAM_CTRL_SET_HANDLE:
		{
			tb_assert_and_check_return_val(!gst->bopened, tb_false);
			tb_handle_t handle = (tb_handle_t)tb_va_arg(args, tb_handle_t);
			sst->sock = handle;
			sst->bref = handle? 1 : 0;
			return tb_true;
		}
	case TB_SSTREAM_CTRL_GET_HANDLE:
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
	tb_gstream_t* gst = (tb_gstream_t*)tb_malloc0(sizeof(tb_sstream_t));
	tb_assert_and_check_return_val(gst, tb_null);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init stream
	tb_sstream_t* sst = (tb_sstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_SOCK;
	gst->aopen 	= tb_sstream_aopen;
	gst->aclose = tb_sstream_aclose;
	gst->aread 	= tb_sstream_aread;
	gst->awrit 	= tb_sstream_awrit;
	gst->ctrl 	= tb_sstream_ctrl;
	gst->wait 	= tb_sstream_wait;
	sst->sock 	= tb_null;
	sst->type 	= TB_SOCKET_TYPE_TCP;

	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return tb_null;
}

tb_gstream_t* tb_gstream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
	// check
	tb_assert_and_check_return_val(host && port, tb_null);

	// init sock stream
	tb_gstream_t* gst = tb_gstream_init_sock();
	tb_assert_and_check_return_val(gst, tb_null);

	// ioctl
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CTRL_SET_HOST, host)) goto fail;
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CTRL_SET_PORT, port)) goto fail;
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CTRL_SET_SSL, bssl)) goto fail;
	if (!tb_gstream_ctrl(gst, TB_SSTREAM_CTRL_SET_TYPE, type)) goto fail;
	
	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return tb_null;
}
