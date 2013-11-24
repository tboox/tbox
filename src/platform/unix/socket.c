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
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../socket.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <sys/uio.h>
#if defined(TB_CONFIG_OS_LINUX) || defined(TB_CONFIG_OS_ANDROID)
# 	include <sys/sendfile.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_socket_init()
{
	// ignore sigpipe
	signal(SIGPIPE, SIG_IGN);

	// ok
	return tb_true;
}
tb_void_t tb_socket_exit()
{
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
	tb_int_t fd = socket(AF_INET, t, p);
	tb_assert_and_check_return_val(fd >= 0, tb_null);

	// non-block
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

	// ok
	return (fd + 1);
}
tb_bool_t tb_socket_pair(tb_size_t type, tb_handle_t pair[2])
{
	// check
	tb_assert_and_check_return_val(type && pair, tb_false);

	// init type
	tb_int_t t = 0;
	switch (type)
	{
	case TB_SOCKET_TYPE_TCP:
		t = SOCK_STREAM;
		break;
	case TB_SOCKET_TYPE_UDP:
		t = SOCK_DGRAM;
		break;
	default:
		return tb_false;
	}

	// make pair
	tb_int_t fd[2] = {0};
	if (socketpair(AF_LOCAL, t, 0, fd) == -1) return tb_false;

	// non-block
	fcntl(fd[0], F_SETFL, fcntl(fd[0], F_GETFL) | O_NONBLOCK);
	fcntl(fd[1], F_SETFL, fcntl(fd[1], F_GETFL) | O_NONBLOCK);

	// save pair
	pair[0] = fd[0] + 1;
	pair[1] = fd[1] + 1;

	// ok
	return tb_true;
}
tb_long_t tb_socket_connect(tb_handle_t handle, tb_char_t const* ip, tb_size_t port)
{
	// check
	tb_assert_and_check_return_val(handle && ip && port, -1);

	// init
	struct sockaddr_in d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
#if 1
	if (!inet_aton(ip, &(d.sin_addr))) return -1;
#else
	if (!inet_aton(ip, &(d.sin_addr))) 
	{
		struct hostent* h = gethostbyname(ip);
		if (h) memcpy(&d.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
		else return -1;
	}
#endif

	// connect
	tb_long_t r = connect((tb_int_t)handle - 1, (struct sockaddr *)&d, sizeof(d));

	// ok?
	if (!r || errno == EISCONN) return 1;

	// continue?
	if (errno == EINTR || errno == EINPROGRESS || errno == EAGAIN) return 0;

	// error
	return -1;
}

tb_bool_t tb_socket_bind(tb_handle_t handle, tb_char_t const* ip, tb_size_t port)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// init
	struct sockaddr_in d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	if (ip)
	{
		if (!inet_aton(ip, &(d.sin_addr))) return tb_false;
	}
	else d.sin_addr.s_addr = INADDR_ANY;

	// reuse addr
#ifdef SO_REUSEADDR
	//if (ip)
	{
		tb_int_t reuseaddr = 1;
		if (setsockopt((tb_int_t)handle - 1, SOL_SOCKET, SO_REUSEADDR, (tb_int_t *)&reuseaddr, sizeof(reuseaddr)) < 0) 
			tb_trace("reuseaddr: %lu failed", port);
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
    return (bind((tb_int_t)handle - 1, (struct sockaddr *)&d, sizeof(d)) < 0)? tb_false : tb_true;
}
tb_bool_t tb_socket_listen(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// listen
    return (listen((tb_int_t)handle - 1, 20) < 0)? tb_false : tb_true;
}
tb_handle_t tb_socket_accept(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_null);

	// accept  
	struct sockaddr_in d;
	tb_int_t 	n = sizeof(struct sockaddr_in);
	tb_long_t 	r = accept((tb_int_t)handle - 1, (struct sockaddr *)&d, &n);

	// no client?
	tb_check_return_val(r > 0, tb_null);

	// non-block
	fcntl(r, F_SETFL, fcntl(r, F_GETFL) | O_NONBLOCK);

	// ok
	return r + 1;
}
tb_bool_t tb_socket_kill(tb_handle_t handle, tb_size_t mode)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// init how
	tb_int_t how = SHUT_RDWR;
	switch (mode)
	{
	case TB_SOCKET_KILL_RO:
		how = SHUT_RD;
		break;
	case TB_SOCKET_KILL_WO:
		how = SHUT_WR;
		break;
	case TB_SOCKET_KILL_RW:
		how = SHUT_RDWR;
		break;
	default:
		break;
	}

	// kill it
	return !shutdown((tb_int_t)handle - 1, how)? tb_true : tb_false;
}
tb_bool_t tb_socket_close(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// close it
	return !close((tb_int_t)handle - 1)? tb_true : tb_false;
}
tb_long_t tb_socket_recv(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && data, -1);
	tb_check_return_val(size, 0);

	// recv
	tb_long_t real = recv((tb_int_t)handle - 1, data, (tb_int_t)size, 0);

	// ok?
	if (real >= 0) return real;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_send(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && data, -1);
	tb_check_return_val(size, 0);

	// recv
	tb_long_t real = send((tb_int_t)handle - 1, data, (tb_int_t)size, 0);

	// ok?
	if (real >= 0) return real;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_recvv(tb_handle_t socket, tb_iovec_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(socket && list && size, -1);

	// check iovec
	tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
	tb_assert_static(sizeof(size_t) == sizeof(tb_size_t));

	// read it
	tb_long_t real = readv((tb_int_t)socket - 1, list, size);

	// ok?
	if (real >= 0) return real;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_sendv(tb_handle_t socket, tb_iovec_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(socket && list && size, -1);

	// writ it
	tb_long_t real = writev((tb_int_t)socket - 1, list, size);

	// ok?
	if (real >= 0) return real;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;
}
tb_hong_t tb_socket_sendfile(tb_handle_t socket, tb_handle_t file, tb_hize_t offset, tb_hize_t size)
{
	// check
	tb_assert_and_check_return_val(socket && file && size, -1);

#if defined(TB_CONFIG_OS_LINUX) || defined(TB_CONFIG_OS_ANDROID)

	// send it
	off_t 		seek = offset;
	tb_hong_t 	real = sendfile((tb_int_t)socket - 1, (tb_int_t)file - 1, &seek, (size_t)size);

	// ok?
	if (real >= 0) return real;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;

#elif defined(TB_CONFIG_OS_MAC) || defined(TB_CONFIG_OS_IOS)

	// send it
	off_t real = (off_t)size;
	if (!sendfile((tb_int_t)file - 1, (tb_int_t)socket - 1, (off_t)offset, &real, tb_null, 0)) return (tb_hong_t)real;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return (tb_hong_t)real;

	// error
	return -1;
#else

	// no impl
	tb_trace_noimpl();
	
	// error
	return -1;
#endif
}
tb_long_t tb_socket_urecv(tb_handle_t handle, tb_char_t const* ip, tb_size_t port, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && ip && port && data, -1);
	tb_check_return_val(size, 0);

	// init
	struct sockaddr_in d;
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	if (!inet_aton(ip, &(d.sin_addr))) return -1;

	// recv
	tb_int_t 	n = sizeof(d);
	tb_long_t 	r = recvfrom((tb_int_t)handle - 1, data, (tb_int_t)size, 0, (struct sockaddr*)&d, &n);

	// ok?
	if (r >= 0) return r;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_usend(tb_handle_t handle, tb_char_t const* ip, tb_size_t port, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(handle && ip && port && data, -1);
	tb_check_return_val(size, 0);

	// init
	struct sockaddr_in d;
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	if (!inet_aton(ip, &(d.sin_addr))) return -1;

	// send
	tb_long_t 	r = sendto((tb_int_t)handle - 1, data, (tb_int_t)size, 0, (struct sockaddr*)&d, sizeof(d));

	// ok?
	if (r >= 0) return r;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;
}
	
