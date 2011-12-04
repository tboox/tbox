/*!The Tiny Box Library
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

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../string/string.h"
#include "../../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_SSTREAM_HOST_MAX 		(1024)

/* /////////////////////////////////////////////////////////
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

	// the port
	tb_size_t 			port;

	// the host
	tb_char_t 			host[TB_SSTREAM_HOST_MAX];

}tb_sstream_t;


/* /////////////////////////////////////////////////////////
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
	tb_assert_and_check_return_val(sst && !sst->sock && sst->port, -1);

	// open socket
	sst->sock = tb_socket_client_open(sst->host, sst->port, sst->type, TB_FALSE);
	tb_assert_and_check_return_val(sst->sock, -1);

	// ok
	return 1;
}
static tb_long_t tb_sstream_aclose(tb_gstream_t* gst)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, -1);

	if (sst->sock)
	{
		// close socket
		tb_socket_close(sst->sock);
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
static tb_long_t tb_sstream_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst && sst->sock && data, -1);
	tb_check_return_val(size, 0);

	// writ data
	return tb_socket_send(sst->sock, data, size);
}
static tb_bool_t tb_sstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1)
{
	tb_sstream_t* sst = tb_sstream_cast(gst);
	tb_assert_and_check_return_val(sst, TB_FALSE);

	switch (cmd)
	{
	case TB_GSTREAM_CMD_SET_URL:
		{
			tb_assert_and_check_return_val(arg1, TB_FALSE);

			// skip prefix
			tb_char_t const* 	p = (tb_char_t const*)arg1;
			tb_size_t 			n = tb_strlen(p);
			tb_char_t const* 	e = p + n;
			tb_assert_and_check_return_val(n > 7 && !tb_strncmp(p, "sock://", 7), TB_FALSE);
			p += 7;

			// get host
			tb_char_t* pb = sst->host;
			tb_char_t* pe = sst->host + TB_SSTREAM_HOST_MAX - 1;
			while (p < e && pb < pe && *p && *p != '/' && *p != ':') *pb++ = *p++;
			*pb = '\0';
			//tb_trace("[sst]: host: %s", sst->host);

			// get port
			if (*p && *p == ':')
			{
				tb_char_t port[12];
				pb = port;
				pe = port + 12 - 1;
				for (p++; p < e && pb < pe && *p && *p != '/'; ) *pb++ = *p++;
				*pb = '\0';
				sst->port = tb_stou32(port);
			}
			tb_assert_and_check_return_val(sst->port, TB_FALSE);
			//tb_trace("[sst]: port: %u", sst->port);

			return TB_TRUE;
		}
	case TB_SSTREAM_CMD_SET_HOST:
		{
			tb_assert_and_check_return_val(arg1, TB_FALSE);
			tb_strncpy(sst->host, (tb_char_t const*)arg1, TB_SSTREAM_HOST_MAX);
			sst->host[TB_SSTREAM_HOST_MAX - 1] = '\0';
			return TB_TRUE;
		}
	case TB_SSTREAM_CMD_SET_PORT:
		{
			tb_assert_and_check_return_val(arg1, TB_FALSE);
			sst->port = (tb_size_t)arg1;
			return TB_TRUE;
		}
	case TB_SSTREAM_CMD_SET_TYPE:
		{
			sst->type = (tb_size_t)arg1;
			return TB_TRUE;
		}
	case TB_SSTREAM_CMD_SET_SSL:
		{
			tb_trace_noimpl();
			return TB_FALSE;
		}
	default:
		break;
	}
	return TB_FALSE;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_t* tb_gstream_init_sock()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_sstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init stream
	tb_sstream_t* sst = (tb_sstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_SOCK;
	gst->aopen 	= tb_sstream_aopen;
	gst->aclose = tb_sstream_aclose;
	gst->aread 	= tb_sstream_aread;
	gst->awrit 	= tb_sstream_awrit;
	gst->ioctl1 = tb_sstream_ioctl1;
	sst->sock 	= TB_NULL;
	sst->type 	= TB_SOCKET_TYPE_TCP;

	return gst;
}

tb_gstream_t* tb_gstream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl)
{
	tb_assert_and_check_return_val(host && port, TB_NULL);

	// init sock stream
	tb_gstream_t* gst = tb_gstream_init_sock();
	tb_assert_and_check_return_val(gst, TB_NULL);

	// ioctl
	if (!tb_gstream_ioctl1(gst, TB_SSTREAM_CMD_SET_HOST, host)) goto fail;
	if (!tb_gstream_ioctl1(gst, TB_SSTREAM_CMD_SET_PORT, port)) goto fail;
	if (!tb_gstream_ioctl1(gst, TB_SSTREAM_CMD_SET_TYPE, type)) goto fail;
	if (!tb_gstream_ioctl1(gst, TB_SSTREAM_CMD_SET_SSL, bssl)) goto fail;
	
	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
}
