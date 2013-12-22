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
 * @file		socket.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "api.h"
#include "../socket.h"
#include <windows.h>
#include <winsock2.h>

/* ///////////////////////////////////////////////////////////////////////
 * libraries
 */
#pragma comment(lib, "ws2_32.lib")

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_socket_init()
{
	WSADATA	WSAData = {0};
	if (WSAStartup(MAKEWORD(2, 2), &WSAData))
	{
		WSACleanup();
		return tb_false;
	}
	return tb_true;
}
tb_void_t tb_socket_exit()
{
	WSACleanup();
}
tb_handle_t tb_socket_open(tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(type, tb_null);
	
	// init type & protocol
	tb_int_t t = 0;
	tb_int_t p = 0;
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
		return tb_null;
	}

	// socket
//	tb_long_t fd = socket(AF_INET, t, p);
	SOCKET fd = WSASocket(AF_INET, t, p, tb_null, 0, WSA_FLAG_OVERLAPPED); //!< for iocp
	tb_assert_and_check_return_val(fd >= 0, tb_null);

	// non-block
	tb_ulong_t nb = 1;
	if (ioctlsocket(fd, FIONBIO, &nb) == SOCKET_ERROR) goto fail;

	// ok
	return (tb_handle_t)(fd + 1);

fail: 
	if (fd >= 0) closesocket(fd);
	return tb_null;
}
tb_bool_t tb_socket_pair(tb_size_t type, tb_handle_t pair[2])
{
	// check
	tb_assert_and_check_return_val(type && pair, tb_false);
	
	// init pair
	pair[0] = tb_null;
	pair[1] = tb_null;

	// init type & protocol
	tb_int_t t = 0;
	tb_int_t p = 0;
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
		return tb_false;
	}

	// done
	tb_bool_t 	ok = tb_false;
	SOCKET 		listener = INVALID_SOCKET;
	SOCKET 		sock1 = INVALID_SOCKET;
	SOCKET 		sock2 = INVALID_SOCKET;
	do
	{
		// init listener
		listener = WSASocket(AF_INET, t, p, tb_null, 0, WSA_FLAG_OVERLAPPED);
		tb_assert_and_check_break(listener != INVALID_SOCKET);

		// init bind address
		SOCKADDR_IN b = {0};
		b.sin_family = AF_INET;
		b.sin_port = 0;
		b.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK); 

		// reuse addr
#ifdef SO_REUSEADDR
		{
			tb_int_t reuseaddr = 1;
			if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (tb_int_t *)&reuseaddr, sizeof(reuseaddr)) < 0) 
				break; 
		}
#endif

		// bind it
    	if (bind(listener, (struct sockaddr *)&b, sizeof(b)) == SOCKET_ERROR) break;

		// get sock address
		SOCKADDR_IN d = {0};
		tb_int_t 	n = sizeof(SOCKADDR_IN);
		if (getsockname(listener, (struct sockaddr *)&d, &n) == SOCKET_ERROR) break;
		d.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
        d.sin_family = AF_INET;

		// listen it
		if (listen(listener, 1) == SOCKET_ERROR) break;

		// init sock1
		sock1 = WSASocket(AF_INET, t, p, tb_null, 0, WSA_FLAG_OVERLAPPED);
		tb_assert_and_check_break(sock1 != INVALID_SOCKET);

		// connect it
		if (connect(sock1, &d, sizeof(d)) == SOCKET_ERROR) break;

		// accept it
		sock2 = accept(listener, tb_null, tb_null);
		tb_assert_and_check_break(sock2 != INVALID_SOCKET);

		// set non-block
		tb_ulong_t nb = 1;
		if (ioctlsocket(sock1, FIONBIO, &nb) == SOCKET_ERROR) break;
		if (ioctlsocket(sock2, FIONBIO, &nb) == SOCKET_ERROR) break;

		// ok 
		ok = tb_true;

	} while (0);

	// exit listener
	if (listener != INVALID_SOCKET) closesocket(listener);
	listener = INVALID_SOCKET;

	// failed? exit it
	if (!ok)
	{
		// exit sock1
		if (sock1 != INVALID_SOCKET) closesocket(sock1);
		sock1 = INVALID_SOCKET;

		// exit sock2
		if (sock2 != INVALID_SOCKET) closesocket(sock2);
		sock2 = INVALID_SOCKET;
	}
	else
	{
		pair[0] = (tb_handle_t)(sock1 + 1);
		pair[1] = (tb_handle_t)(sock2 + 1);
	}

	// ok?
	return ok;
}
tb_long_t tb_socket_connect(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port)
{
	// check
	tb_assert_and_check_return_val(handle && addr && addr->u32 && port, -1);

	// init
	SOCKADDR_IN d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = addr->u32;

	// connect
	tb_long_t r = connect((SOCKET)((tb_long_t)handle - 1), (struct sockaddr *)&d, sizeof(d));

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

tb_size_t tb_socket_bind(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port)
{
	// check
	tb_assert_and_check_return_val(handle, 0);

	// init
	SOCKADDR_IN d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = (addr && addr->u32)? addr->u32 : INADDR_ANY; 

	// reuse addr
#ifdef SO_REUSEADDR
	//if (addr && addr->u32)
	{
		tb_int_t reuseaddr = 1;
		if (setsockopt((tb_int_t)handle - 1, SOL_SOCKET, SO_REUSEADDR, (tb_int_t *)&reuseaddr, sizeof(reuseaddr)) < 0) 
			tb_trace("reuseaddr: failed");
	}
#endif

	// reuse port
#ifdef SO_REUSEPORT
	if (port)
	{
		tb_int_t reuseport = 1;
		if (setsockopt((tb_int_t)handle - 1, SOL_SOCKET, SO_REUSEPORT, (tb_int_t *)&reuseport, sizeof(reuseport)) < 0) 
			tb_trace("reuseport: %lu failed", port);
	}
#endif

	// bind 
    if (bind((SOCKET)((tb_long_t)handle - 1), (struct sockaddr *)&d, sizeof(d)) < 0) return 0;
	
	// bind one random port? get the bound port
	if (!port)
	{
		tb_int_t n = sizeof(d);
		if (getsockname((SOCKET)((tb_long_t)handle - 1), (struct sockaddr *)&d, &n) == -1) return 0;
		port = ntohs(d.sin_port);
	}

	// ok?
	return port;
}
tb_bool_t tb_socket_listen(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// listen
    return (listen((SOCKET)((tb_long_t)handle - 1), 20) < 0)? tb_false : tb_true;
}
tb_handle_t tb_socket_accept(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_null);

	// accept  
	SOCKADDR_IN d;
	tb_int_t 	n = sizeof(SOCKADDR_IN);
	tb_long_t 	r = accept((SOCKET)((tb_long_t)handle - 1), (struct sockaddr *)&d, &n);

	// no client?
	tb_check_return_val(r > 0, tb_null);

	// non-block
	tb_ulong_t nb = 1;
	if (ioctlsocket(r, FIONBIO, &nb) == SOCKET_ERROR) goto fail;

	// ok
	return r + 1;

fail: 
	if (r >= 0) closesocket(r);
	return tb_null;
}
tb_bool_t tb_socket_kill(tb_handle_t handle, tb_size_t mode)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// init how
	tb_int_t how = SD_BOTH;
	switch (mode)
	{
	case TB_SOCKET_KILL_RO:
		how = SD_RECEIVE;
		break;
	case TB_SOCKET_KILL_WO:
		how = SD_SEND;
		break;
	case TB_SOCKET_KILL_RW:
		how = SD_BOTH;
		break;
	default:
		break;
	}

	// kill it
	return !shutdown((SOCKET)((tb_long_t)handle - 1), how)? tb_true : tb_false;
}
tb_bool_t tb_socket_close(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// close it
	return !closesocket((SOCKET)((tb_long_t)handle - 1))? tb_true : tb_false;
}
tb_long_t tb_socket_recv(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && data, -1);
	tb_check_return_val(size, 0);

	// recv
	tb_long_t real = recv((SOCKET)((tb_long_t)handle - 1), data, (tb_int_t)size, 0);

	// ok?
	if (real >= 0) return real;

	// errno
	tb_long_t e = WSAGetLastError();

	// continue?
	if (e == WSAEWOULDBLOCK || e == WSAEINPROGRESS) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_send(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && data, -1);
	tb_check_return_val(size, 0);

	// recv
	tb_long_t real = send((SOCKET)((tb_long_t)handle - 1), data, (tb_int_t)size, 0);

	// ok?
	if (real >= 0) return real;

	// errno
	tb_long_t e = WSAGetLastError();

	// continue?
	if (e == WSAEWOULDBLOCK || e == WSAEINPROGRESS) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_recvv(tb_handle_t handle, tb_iovec_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && list && size, -1);

	// walk read
	tb_size_t i = 0;
	tb_size_t read = 0;
	for (i = 0; i < size; i++)
	{
		// the data & size
		tb_byte_t* 	data = list[i].data;
		tb_size_t 	need = list[i].size;
		tb_check_break(data && need);

		// read it
		tb_long_t real = tb_socket_recv(handle, data, need);

		// full? next it
		if (real == need)
		{
			read += real;
			continue ;
		}

		// failed?
		tb_check_return_val(real >= 0, -1);

		// ok?
		if (real > 0) read += real;

		// end
		break;
	}

	// ok?
	return read;
}
tb_long_t tb_socket_sendv(tb_handle_t handle, tb_iovec_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && list && size, -1);

	// walk writ
	tb_size_t i = 0;
	tb_size_t writ = 0;
	for (i = 0; i < size; i++)
	{
		// the data & size
		tb_byte_t* 	data = list[i].data;
		tb_size_t 	need = list[i].size;
		tb_check_break(data && need);

		// writ it
		tb_long_t real = tb_socket_send(handle, data, need);

		// full? next it
		if (real == need)
		{
			writ += real;
			continue ;
		}

		// failed?
		tb_check_return_val(real >= 0, -1);

		// ok?
		if (real > 0) writ += real;

		// end
		break;
	}

	// ok?
	return writ;
}
tb_hong_t tb_socket_sendfile(tb_handle_t handle, tb_handle_t file, tb_hize_t offset, tb_hize_t size)
{
	// check
	tb_assert_and_check_return_val(handle && file && size, -1);

	// the transmitfile func
	tb_api_TransmitFile_t transmitfile = tb_api_TransmitFile();
	tb_assert_and_check_return_val(transmitfile, -1);

	// transmit it
	OVERLAPPED 	olap = {0}; olap.Offset = offset;
	tb_hong_t 	real = transmitfile((SOCKET)handle - 1, (HANDLE)file, (DWORD)size, (1 << 16), &olap, tb_null, 0);

	// ok?
	if (real >= 0) return real;

	// errno
	tb_long_t e = WSAGetLastError();

	// continue?
	if (e == WSAEWOULDBLOCK || e == WSAEINPROGRESS || e == WSA_IO_PENDING) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_urecv(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && addr && addr->u32 && port && data, -1);
	tb_check_return_val(size, 0);

	// init addr
	SOCKADDR_IN d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = addr->u32;

	// recv
	tb_int_t 	n = sizeof(d);
	tb_long_t 	r = recvfrom((SOCKET)((tb_long_t)handle - 1), data, (tb_int_t)size, 0, (struct sockaddr*)&d, &n);

	// ok?
	if (r >= 0) return r;

	// continue?
	if (WSAGetLastError() == WSAEWOULDBLOCK) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_usend(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && addr && addr->u32 && port && data, -1);
	tb_check_return_val(size, 0);

	// init addr
	SOCKADDR_IN d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = addr->u32;

	// send
	tb_long_t r = sendto((SOCKET)((tb_long_t)handle - 1), data, (tb_int_t)size, 0, (struct sockaddr*)&d, sizeof(d));

	// ok?
	if (r >= 0) return r;

	// continue?
	if (WSAGetLastError() == WSAEWOULDBLOCK) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_urecvv(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && addr && addr->u32 && port && list && size, -1);

	// init addr
	SOCKADDR_IN d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = addr->u32;

	// walk read
	tb_size_t 	i = 0;
	tb_int_t 	n = sizeof(d);
	tb_size_t 	read = 0;
	for (i = 0; i < size; i++)
	{
		// the data & size
		tb_byte_t* 	data = list[i].data;
		tb_size_t 	need = list[i].size;
		tb_check_break(data && need);

		// read it
		tb_long_t real = recvfrom((SOCKET)((tb_long_t)handle - 1), data, (tb_int_t)need, 0, (struct sockaddr*)&d, &n);

		// full? next it
		if (real == need)
		{
			read += real;
			continue ;
		}

		// failed?
		tb_check_return_val(real >= 0, -1);

		// ok?
		if (real > 0) read += real;

		// end
		break;
	}

	// ok?
	return read;
}
tb_long_t tb_socket_usendv(tb_handle_t handle, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && addr && addr->u32 && port && list && size, -1);

	// init addr
	SOCKADDR_IN d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.S_un.S_addr = addr->u32;

	// walk writ
	tb_size_t i = 0;
	tb_size_t writ = 0;
	for (i = 0; i < size; i++)
	{
		// the data & size
		tb_byte_t* 	data = list[i].data;
		tb_size_t 	need = list[i].size;
		tb_check_break(data && need);

		// writ it
		tb_long_t real = sendto((SOCKET)((tb_long_t)handle - 1), data, (tb_int_t)need, 0, (struct sockaddr*)&d, sizeof(d));

		// full? next it
		if (real == need)
		{
			writ += real;
			continue ;
		}

		// failed?
		tb_check_return_val(real >= 0, -1);

		// ok?
		if (real > 0) writ += real;

		// end
		break;
	}

	// ok?
	return writ;
}
