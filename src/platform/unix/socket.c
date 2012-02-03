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
 * \file		socket.c
 *
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
#include <netdb.h>
#include <errno.h>

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_bool_t tb_socket_init()
{
	return TB_TRUE;
}
tb_void_t tb_socket_exit()
{
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
	tb_assert_and_check_return_val(fd >= 0, TB_NULL);

	// non-block
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

	// ok
	return (fd + 1);
}

tb_long_t tb_socket_connect(tb_handle_t handle, tb_char_t const* ip, tb_size_t port)
{
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
	tb_long_t r = connect((tb_long_t)handle - 1, (struct sockaddr *)&d, sizeof(d));

	// ok?
	if (!r) return 1;

	// continue?
	if (errno == EINTR || errno == EINPROGRESS || errno == EAGAIN) return 0;

	// error
	return -1;
}

tb_bool_t tb_socket_bind(tb_handle_t handle, tb_size_t port)
{
	tb_assert_and_check_return_val(handle && port, TB_FALSE);

	// init
	struct sockaddr_in d = {0};
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	d.sin_addr.s_addr = INADDR_ANY;

	// bind 
    if (bind((tb_long_t)handle - 1, (struct sockaddr *)&d, sizeof(d)) < 0) return TB_FALSE;

	// listen
    return (listen((tb_long_t)handle - 1, 20) < 0)? TB_FALSE : TB_TRUE;
}
tb_handle_t tb_socket_accept(tb_handle_t handle)
{
	tb_assert_and_check_return_val(handle, TB_NULL);

	// accept  
	struct sockaddr_in d;
	tb_int_t 	n = sizeof(struct sockaddr_in);
	tb_long_t 	r = accept((tb_long_t)handle - 1, (struct sockaddr *)&d, &n);

	// no client?
	tb_check_return_val(r > 0, TB_NULL);

	// non-block
	fcntl(r, F_SETFL, fcntl(r, F_GETFL) | O_NONBLOCK);

	// ok
	return r + 1;
}

tb_bool_t tb_socket_close(tb_handle_t handle)
{
	tb_assert_and_check_return_val(handle, TB_FALSE);
	return !close((tb_long_t)handle - 1)? TB_TRUE : TB_FALSE;
}
tb_handle_t tb_socket_bare(tb_handle_t handle)
{
	return handle? (tb_handle_t)((tb_long_t)handle - 1) : TB_NULL;
}
tb_long_t tb_socket_recv(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(handle && data, -1);
	tb_check_return_val(size, 0);

	// recv
	tb_long_t r = recv((tb_long_t)handle - 1, data, size, 0);

	// ok?
	if (r >= 0) return r;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

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

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;
}

// recv & send for udp
tb_long_t tb_socket_urecv(tb_handle_t handle, tb_char_t const* ip, tb_size_t port, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(handle && ip && port && data, -1);
	tb_check_return_val(size, 0);

	// init
	struct sockaddr_in d;
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	if (!inet_aton(ip, &(d.sin_addr))) return -1;

	// recv
	tb_int_t 	n = sizeof(d);
	tb_long_t 	r = recvfrom((tb_long_t)handle - 1, data, size, 0, (struct sockaddr*)&d, &n);

	// ok?
	if (r >= 0) return r;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;
}
tb_long_t tb_socket_usend(tb_handle_t handle, tb_char_t const* ip, tb_size_t port, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(handle && ip && port && data, -1);
	tb_check_return_val(size, 0);

	// init
	struct sockaddr_in d;
	d.sin_family = AF_INET;
	d.sin_port = htons(port);
	if (!inet_aton(ip, &(d.sin_addr))) return -1;

	// send
	tb_long_t 	r = sendto((tb_long_t)handle - 1, data, size, 0, (struct sockaddr*)&d, sizeof(d));

	// ok?
	if (r >= 0) return r;

	// continue?
	if (errno == EINTR || errno == EAGAIN) return 0;

	// error
	return -1;
}
	
