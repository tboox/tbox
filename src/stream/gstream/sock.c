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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \sock		sock.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../aio/aio.h"
#include "../../string/string.h"
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

	// the sock type
	tb_size_t 			type;

}tb_sstream_t;


/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_sstream_t* tb_sstream_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_SOCK, TB_NULL);
	return (tb_sstream_t*)gst;
}
static tb_long_t tb_sstream_aopen(tb_gstream_t* gst)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, -1);

	// open
	if (!sst->sock) sst->sock = tb_socket_open(sst->type);
	tb_assert_and_check_return_val(sst->sock, -1);

	// host
	tb_char_t const* host = tb_url_host_get(&gst->url);
	tb_assert_and_check_return_val(host, -1);

	// port
	tb_size_t port = tb_url_port_get(&gst->url);
	tb_assert_and_check_return_val(port, -1);

	// connect
	return tb_socket_connect(sst->sock, host, port);
}
static tb_long_t tb_sstream_aclose(tb_gstream_t* gst)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, -1);

	if (sst->sock)
	{
		// close socket
		if (!tb_socket_close(sst->sock)) return 0;
		sst->sock = TB_NULL;
	}

	// ok
	return 1;
}
static tb_long_t tb_sstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst && sst->sock && data, -1);
	tb_check_return_val(size, 0);

	// read data
	return tb_socket_recv(sst->sock, data, size);
}
static tb_long_t tb_sstream_afread(tb_gstream_t* gst)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst && sst->sock, -1);

	// read the left data
	tb_byte_t data[TB_GSTREAM_BLOCK_MAXN];
	tb_long_t r = tb_socket_recv(sst->sock, data, TB_GSTREAM_BLOCK_MAXN);
	tb_assert_and_check_return_val(r >= 0, -1);

	// ok?
	return r > 0? 0 : 1;
}
static tb_long_t tb_sstream_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst && sst->sock && data, -1);
	tb_check_return_val(size, 0);

	// writ data
	return tb_socket_send(sst->sock, data, size);
}
static tb_handle_t tb_sstream_bare(tb_gstream_t* gst)
{	
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst && sst->sock, TB_NULL);
	return tb_socket_bare(sst->sock);
}
static tb_long_t tb_sstream_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst && sst->sock, -1);

	tb_aioo_t o;
	tb_aioo_seto(&o, sst->sock, TB_AIOO_OTYPE_SOCK, etype, TB_NULL);

	return tb_aioo_wait(&o, timeout);
}
static tb_bool_t tb_sstream_ctrl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, TB_FALSE);

	switch (cmd)
	{
	case TB_SSTREAM_CMD_SET_TYPE:
		{
			sst->type = (tb_size_t)arg1;
			return TB_TRUE;
		}
	default:
		break;
	}
	return TB_FALSE;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_t* tb_gstream_init_sock()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_sstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init stream
	tb_sstream_t* sst = (tb_sstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_SOCK;
	gst->aopen 	= tb_sstream_aopen;
	gst->aclose = tb_sstream_aclose;
	gst->aread 	= tb_sstream_aread;
	gst->awrit 	= tb_sstream_awrit;
	gst->afread = tb_sstream_afread;
	gst->ctrl1 	= tb_sstream_ctrl1;
	gst->bare 	= tb_sstream_bare;
	gst->wait 	= tb_sstream_wait;
	sst->sock 	= TB_NULL;
	sst->type 	= TB_SOCKET_TYPE_TCP;

	// ok
	return gst;

fail:
	if (gst) tb_free(gst);
	return TB_NULL;
}

tb_gstream_t* tb_gstream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
	tb_assert_and_check_return_val(host && port, TB_NULL);

	// init sock stream
	tb_gstream_t* gst = tb_gstream_init_sock();
	tb_assert_and_check_return_val(gst, TB_NULL);

	// ioctl
	if (!tb_gstream_ctrl1(gst, TB_GSTREAM_CMD_SET_HOST, host)) goto fail;
	if (!tb_gstream_ctrl1(gst, TB_GSTREAM_CMD_SET_PORT, port)) goto fail;
	if (!tb_gstream_ctrl1(gst, TB_GSTREAM_CMD_SET_SSL, bssl)) goto fail;
	if (!tb_gstream_ctrl1(gst, TB_SSTREAM_CMD_SET_TYPE, type)) goto fail;
	
	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
}
