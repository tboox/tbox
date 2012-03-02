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
 * \author		ruki
 * \file		socket.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../socket.h"
#include <windows.h>
#include <winsock2.h>

/* ///////////////////////////////////////////////////////////////////////
 * libraries
 */
#pragma comment(lib, "ws2_32.lib")

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_bool_t tb_socket_init()
{
	WSADATA	WSAData = {0};
	if (WSAStartup(MAKEWORD(2, 2), &WSAData))
	{
		WSACleanup();
		return TB_FALSE;
	}
	return TB_TRUE;
}
tb_void_t tb_socket_exit()
{
	WSACleanup();
}
tb_handle_t tb_socket_open(tb_size_t type)
{
	tb_assert_and_check_return_val(type, TB_NULL);
	
	// init type & protocol
	tb_size_t t = 0;
	tb_size_t p = 0;
	switch (type)
	{
	case TB_SOCKET_TYPE_TCP:
		{
			t = SOCK_STREAM;
			p = IPPROTO_TCP;
		}
		break;
	case TB_SOCKET_TYPE_UDP:
		{
			t = SOCK_DGRAM;
			p = IPPROTO_UDP;
		}
		break;
	default:
		return TB_NULL;
	}

	// socket
	tb_long_t fd = socket(AF_INET, t, p);
//	SOCKET fd = WSASocket(AF_INET, t, p, TB_NULL, 0, WSA_FLAG_OVERLAPPED);
	tb_assert_and_check_return_val(fd >= 0, TB_NULL);

	// non-block
	tb_ulong_t nb = 1;
	if (ioctlsocket(fd, FIONBIO, &nb) == SOCKET_ERROR) goto fail;

	// ok
	return (tb_handle_t)(fd + 1);

fail: 
	if (fd >= 0) closesocket(fd);
	return TB_NULL;
}

tb_long_t tb_socket_connect(tb_handle_t handle, tb_char_t const* ip, tb_size_t port)
{
	tb_assert_and_check_return_val(handle && ip && port, -1);

	// init
	SOCKADDR_IN d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = inet_addr(ip);

	// connect
	tb_long_t r = connect((tb_long_t)handle - 1, (struct sockaddr *)&d, sizeof(d));

	// ok?
	if (!r) return 1;

	// errno
	tb_long_t e = WSAGetLastError();

	// have been connected?
	if (e == WSAEISCONN) return 1;

	// continue?
	if (e == WSAEWOULDBLOCK || e == WSAEINPROGRESS) return 0;

	// error
	return -1;
}

tb_bool_t tb_socket_bind(tb_handle_t handle, tb_size_t port)
{
	tb_assert_and_check_return_val(handle && port, TB_FALSE);

	// init
	SOCKADDR_IN d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 

	// bind 
    if (bind((tb_long_t)handle - 1, (struct sockaddr *)&d, sizeof(d)) < 0) return TB_FALSE;

	// listen
    return (listen((tb_long_t)handle - 1, 20) < 0)? TB_FALSE : TB_TRUE;
}
tb_handle_t tb_socket_accept(tb_handle_t handle)
{
	tb_assert_and_check_return_val(handle, TB_NULL);

	// accept  
	SOCKADDR_IN d;
	tb_int_t 	n = sizeof(SOCKADDR_IN);
	tb_long_t 	r = accept((tb_long_t)handle - 1, (struct sockaddr *)&d, &n);

	// no client?
	tb_check_return_val(r > 0, TB_NULL);

	// non-block
	tb_ulong_t nb = 1;
	if (ioctlsocket(r, FIONBIO, &nb) == SOCKET_ERROR) goto fail;

	// ok
	return r + 1;

fail: 
	if (r >= 0) closesocket(r);
	return TB_NULL;
}

tb_bool_t tb_socket_close(tb_handle_t handle)
{
	tb_assert_and_check_return_val(handle, TB_FALSE);
	return !closesocket((tb_long_t)handle - 1)? TB_TRUE : TB_FALSE;
}
tb_long_t tb_socket_recv(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(handle && data, -1);
	tb_check_return_val(size, 0);

	// recv
	tb_long_t r = recv((tb_long_t)handle - 1, data, size, 0);

	// ok?
	if (r >= 0) return r;

	// errno
	tb_long_t e = WSAGetLastError();

	// continue?
	if (e == WSAEWOULDBLOCK || e == WSAEINPROGRESS) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_send(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(handle && data, -1);
	tb_check_return_val(size, 0);

	// recv
	tb_long_t r = send((tb_long_t)handle - 1, data, size, 0);

	// ok?
	if (r >= 0) return r;

	// errno
	tb_long_t e = WSAGetLastError();

	// continue?
	if (e == WSAEWOULDBLOCK || e == WSAEINPROGRESS) return 0;

	// error
	return -1;
}

// recv & send for udp
tb_long_t tb_socket_urecv(tb_handle_t handle, tb_char_t const* ip, tb_size_t port, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(handle && ip && port && data, -1);
	tb_check_return_val(size, 0);

	// init
	SOCKADDR_IN d;
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = inet_addr(ip);

	// recv
	tb_int_t 	n = sizeof(d);
	tb_long_t 	r = recvfrom((tb_long_t)handle - 1, data, size, 0, (struct sockaddr*)&d, &n);

	// ok?
	if (r >= 0) return r;

	// continue?
	if (WSAGetLastError() == WSAEWOULDBLOCK) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_usend(tb_handle_t handle, tb_char_t const* ip, tb_size_t port, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(handle && ip && port && data, -1);
	tb_check_return_val(size, 0);

	// init
	SOCKADDR_IN d;
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = inet_addr(ip);

	// send
	tb_long_t r = sendto((tb_long_t)handle - 1, data, size, 0, (struct sockaddr*)&d, sizeof(d));

	// ok?
	if (r >= 0) return r;

	// continue?
	if (WSAGetLastError() == WSAEWOULDBLOCK) return 0;

	// error
	return -1;
}
	
