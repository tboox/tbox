/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		socket.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "socket.h"
#include "dns.h"

/* /////////////////////////////////////////////////////////
 * globals
 */
static tb_int_t g_socket_init = 0;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t tb_socket_client_open(tb_char_t const* host, tb_uint16_t port, tb_int_t type, tb_bool_t is_block)
{
	// check type
	TB_ASSERT(type == TB_SOCKET_TYPE_TCP || type == TB_SOCKET_TYPE_UDP);
	if (type != TB_SOCKET_TYPE_TCP && type != TB_SOCKET_TYPE_UDP) 
		return TB_INVALID_HANDLE;

	// init first 
	if (!g_socket_init)
	{
		tplat_bool_t ret = tplat_socket_init();
		TB_ASSERT(ret == TB_TRUE);
		g_socket_init = 1;
	}

	// loopup name by dns
	tb_char_t ip[16];
	if (host && host[0])
	{
		tb_char_t const* p = tb_dns_lookup(host, ip);
		if (p) 
		{
			TB_DBG("lookup host: %s => %s", host, p);
			host = p;
		}
	}

	return ((tb_handle_t)tb_socket_client_open(host, port, type, is_block));
}
void tb_socket_close(tb_handle_t hsocket)
{
	TB_ASSERT(hsocket != TB_INVALID_HANDLE);
	if (hsocket == TB_INVALID_HANDLE) return ;

	tb_socket_close(hsocket);
}

tb_int_t tb_socket_recv(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT(hsocket != TB_INVALID_HANDLE);
	if (hsocket == TB_INVALID_HANDLE) return -1;

	return ((tb_int_t)tb_socket_recv(hsocket, data, size));
}

tb_int_t tb_socket_send(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT(hsocket != TB_INVALID_HANDLE);
	if (hsocket == TB_INVALID_HANDLE) return -1;

	return ((tb_int_t)tb_socket_send(hsocket, data, size));
}
tb_int_t tb_socket_recvfrom(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT(hsocket != TB_INVALID_HANDLE);
	if (hsocket == TB_INVALID_HANDLE) return -1;

	// loopup name by dns
	tb_char_t ip[16];
	if (host && host[0])
	{
		tb_char_t const* p = tb_dns_lookup(host, ip);
		if (p) 
		{
			TB_DBG("lookup host: %s => %s", host, p);
			host = p;
		}
	}

	return ((tb_int_t)tb_socket_recvfrom(hsocket, host, port, data, size));
}
tb_int_t tb_socket_sendto(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT(hsocket != TB_INVALID_HANDLE);
	if (hsocket == TB_INVALID_HANDLE) return -1;

	// loopup name by dns
	tb_char_t ip[16];
	if (host && host[0])
	{
		tb_char_t const* p = tb_dns_lookup(host, ip);
		if (p) 
		{
			TB_DBG("lookup host: %s => %s", host, p);
			host = p;
		}
	}

	return ((tb_int_t)tb_socket_sendto(hsocket, host, port, data, size));
}
tb_handle_t tb_socket_server_open(tb_uint16_t port, tb_int_t type, tb_bool_t is_block)
{
	if (!port || type == TB_SOCKET_TYPE_UNKNOWN) return TB_INVALID_HANDLE;

	// init first 
	if (!g_socket_init)
	{
		tplat_bool_t ret = tplat_socket_init();
		TB_ASSERT(ret == TB_TRUE);
		g_socket_init = 1;
	}

	return ((tb_handle_t)tb_socket_server_open(port, type, is_block));
}
tb_handle_t tb_socket_server_accept(tb_handle_t hserver)
{
	return ((tb_handle_t)tb_socket_server_accept(hserver));
}

