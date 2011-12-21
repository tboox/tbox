/*!The Treasure Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		socket.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../socket.h"
#include <windows.h>
#include <winsock2.h>

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_SOCKET_HOST_MAX 			(256)

/* /////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_socket_t
{
	tb_int_t 	fd;
	tb_int_t 	type;
	tb_bool_t 	is_block;
	tb_uint16_t port;
	tb_char_t 	host[TB_SOCKET_HOST_MAX];

}tb_socket_t;


/* /////////////////////////////////////////////////////////
 * decls
 */

INT WSAAPI inet_pton(INT Family, PCTSTR pszAddrString, PVOID pAddrBuf);

/* /////////////////////////////////////////////////////////
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
tb_handle_t tb_socket_client_open(tb_char_t const* host, tb_uint16_t port, tb_int_t type, tb_bool_t is_block)
{
	if (type == TB_SOCKET_TYPE_UNKNOWN) return TB_NULL;

	//tb_trace("socket open: %s %d", host, port);

	// create socket
	tb_int_t t = 0;
	tb_int_t p = 0;
	if (type == TB_SOCKET_TYPE_TCP)
	{
		t = SOCK_STREAM;
		p = IPPROTO_TCP;
		if (!host || !port) return TB_NULL;
	}
	else if (type == TB_SOCKET_TYPE_UDP)
	{
		t = SOCK_DGRAM;
		p = IPPROTO_UDP;
	}
	else return TB_NULL;

	tb_int_t fd = socket(AF_INET, t, p);
	if (fd < 0) return TB_NULL;

	// set block or non-block
	unsigned long nonblock = is_block? 1 : 0;
	if (ioctlsocket(fd, FIONBIO, &nonblock) == SOCKET_ERROR)
	{
		closesocket(fd);
		return TB_FALSE;
	}

	if (type == TB_SOCKET_TYPE_TCP)
	{
		// client
		struct sockaddr_in dest;
		dest.sin_family = AF_INET;
		dest.sin_port = htons(port);
		if (1)//-1 == inet_pton(AF_INET, host, &(dest.sin_addr))) 
		{
			struct hostent* h = gethostbyname(host);
			if (h) memcpy(&dest.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
			else 
			{
				if (fd >= 0) closesocket(fd);
				return TB_NULL;
			}
		}

		// connect host
		tb_int_t ret = 0;
		while (1)
		{
			ret = connect(fd, (struct sockaddr *)&dest, sizeof(dest));

			// if non-block
			if (ret == SOCKET_ERROR && is_block == TB_FALSE)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					if (fd >= 0) closesocket(fd);
					return TB_NULL;
				}

				// wait until we are connected or until abort
				while (1) 
				{
					struct timeval tv;
					fd_set wfds;
					tb_int_t fd_max = fd;

					FD_ZERO(&wfds);
					FD_SET(fd, &wfds);
					tv.tv_sec = 0;
					tv.tv_usec = 100 * 1000;
					ret = select(fd_max + 1, NULL, &wfds, NULL, &tv);
					if (ret > 0 && FD_ISSET(fd, &wfds)) break;
				}

				// test error
				int optlen = sizeof(ret);
				getsockopt(fd, SOL_SOCKET, SO_ERROR, &ret, &optlen);
				if (ret != 0)
				{
					if (fd >= 0) closesocket(fd);
					return TB_NULL;
				}
			}
			else if (ret >= 0) break;
			else
			{
				if (fd >= 0) closesocket(fd);
				return TB_NULL;
			}
		}
	}

	// create socket
	tb_socket_t* s = malloc(sizeof(tb_socket_t));
	tb_assert(s);
	if (!s) return TB_NULL;
	memset(s, 0, sizeof(tb_socket_t));
	s->fd = fd;
	s->type = type;
	s->port = port;
	s->is_block = is_block;

	// save host
	if (host)
	{
		strncpy(s->host, host, TB_SOCKET_HOST_MAX - 1);
		s->host[TB_SOCKET_HOST_MAX - 1] = '\0';
	}

	return ((tb_handle_t)s);
}
tb_void_t tb_socket_close(tb_handle_t hsocket)
{
	tb_assert_and_check_return(hsocket);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	//tb_trace("socket closesocket");

	if (s && s->fd >= 0) 
	{
		closesocket(s->fd);
		free(s);
	}
}

tb_int_t tb_socket_recv(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(hsocket, -1);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	//tb_trace("socket_recv: %d", size);

	// non-block
	if (!s->is_block)
	{
		tb_int_t fd_max = s->fd;
		struct timeval tv;
		fd_set rfds;

		FD_ZERO(&rfds);
		FD_SET(s->fd, &rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 100 * 1000;
		tb_int_t ret = select(fd_max + 1, &rfds, NULL, NULL, &tv);

		if (!ret) return 0; 
		else if (ret != SOCKET_ERROR && FD_ISSET(s->fd, &rfds)) 
			return recv(s->fd, data, size, 0);
		else return -1;
	}
	// block
	else return recv(s->fd, data, size, 0);
}

tb_int_t tb_socket_send(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(hsocket, -1);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	//tb_trace("socket_send: %d", size);

	// non-block
	if (!s->is_block)
	{
		tb_int_t fd_max = s->fd;
		struct timeval tv;
		fd_set wfds;

		FD_ZERO(&wfds);
		FD_SET(s->fd, &wfds);

		tv.tv_sec = 0;
		tv.tv_usec = 100 * 1000;
		tb_int_t ret = select(fd_max + 1, NULL, &wfds, NULL, &tv);

		if (!ret) return 0;
		else if (ret != SOCKET_ERROR && FD_ISSET(s->fd, &wfds)) 
			return send(s->fd, data, size, 0);
		else return -1;
	}
	else return send(s->fd, data, size, 0);
}
tb_int_t tb_socket_recvfrom(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(hsocket, -1);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	// get host & port
	port = port? port : s->port;
	host = host? host : s->host;
	tb_assert(port && host && host[0]);
	if (!port || !host || !host[0]) return -1;

	// recv data
	struct sockaddr_in dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	if (1)//-1 == inet_pton(AF_INET, host, &(dest.sin_addr))) 
	{
		struct hostent* h = gethostbyname(host);
		if (h) memcpy(&dest.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
		else return -1;
	}

	// non-block
	tb_int_t n = sizeof(dest);
	if (!s->is_block)
	{
		tb_int_t fd_max = s->fd;
		struct timeval tv;
		fd_set rfds;

		FD_ZERO(&rfds);
		FD_SET(s->fd, &rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 100 * 1000;
		tb_int_t ret = select(fd_max + 1, &rfds, NULL, NULL, &tv);

		if (!ret) return 0; 
		else if (ret != SOCKET_ERROR && FD_ISSET(s->fd, &rfds)) 
			return recvfrom(s->fd, data, size, 0, (struct sockaddr*)&dest, &n);
		else return -1;
	}
	// block
	else return recvfrom(s->fd, data, size, 0, (struct sockaddr*)&dest, &n);
}
tb_int_t tb_socket_sendto(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(hsocket, -1);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	// get host & port
	port = port? port : s->port;
	host = host? host : s->host;
	tb_assert(port && host && host[0]);
	if (!port || !host || !host[0]) return -1;

	// recv data
	struct sockaddr_in dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	if (1)//-1 == inet_pton(AF_INET, host, &(dest.sin_addr))) 
	{
		struct hostent* h = gethostbyname(host);
		if (h) memcpy(&dest.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
		else return -1;
	}

	// non-block
	if (!s->is_block)
	{
		tb_int_t fd_max = s->fd;
		struct timeval tv;
		fd_set wfds;

		FD_ZERO(&wfds);
		FD_SET(s->fd, &wfds);

		tv.tv_sec = 0;
		tv.tv_usec = 100 * 1000;
		tb_int_t ret = select(fd_max + 1, NULL, &wfds, NULL, &tv);

		if (!ret) return 0;
		else if (ret != SOCKET_ERROR && FD_ISSET(s->fd, &wfds)) 
			return sendto(s->fd, data, size, 0, (struct sockaddr*)&dest, sizeof(dest));
		else return -1;
	}
	// block
	else return sendto(s->fd, data, size, 0, (struct sockaddr*)&dest, sizeof(dest));
}
tb_handle_t tb_socket_server_open(tb_uint16_t port, tb_int_t type, tb_bool_t is_block)
{
	if (!port || type == TB_SOCKET_TYPE_UNKNOWN) return TB_NULL;

	// create socket
	tb_int_t t;
	if (type == TB_SOCKET_TYPE_TCP) t = SOCK_STREAM;
	else if (type == TB_SOCKET_TYPE_UDP) t = SOCK_DGRAM;
	else return TB_NULL;

	// udp is not implemention now
	if (type == TB_SOCKET_TYPE_UDP)
	{
		// not implemention now
		tb_trace("tb_socket_server_open(type == UDP) is not implemention now");
		return TB_NULL;
	}

	// server socket
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	tb_int_t fd_s = socket(AF_INET, t, 0);
	if (fd_s < 0) return TB_NULL;

	// set block or non-block
	unsigned long nonblock = is_block? 1 : 0;
	if (ioctlsocket(fd_s, FIONBIO, &nonblock) == SOCKET_ERROR)
	{
		closesocket(fd_s);
		return TB_FALSE;
	}

	// bind 
	if (bind(fd_s, (struct sockaddr *)&saddr, sizeof(struct sockaddr)) == -1)
	{
		tb_trace("bind error!");
		closesocket(fd_s);
		return TB_NULL;
	}

	// listen
	if (listen(fd_s, 20) == -1) 
	{
		tb_trace("listen error!");
		closesocket(fd_s);
		return TB_NULL;
	}

	// return server socket
	tb_socket_t* s = malloc(sizeof(tb_socket_t));
	tb_assert(s);
	if (!s) return TB_NULL;
	memset(s, 0, sizeof(tb_socket_t));
	s->fd = fd_s;
	s->type = type;
	s->is_block = is_block;

	return ((tb_handle_t)s);
}
tb_handle_t tb_socket_server_accept(tb_handle_t hserver)
{
	tb_assert_and_check_return_val(hserver, TB_NULL);
	tb_socket_t* ps = (tb_socket_t*)hserver;
	if (ps->fd < 0) return TB_NULL;

	// accept client 
	tb_int_t caddr_n = sizeof(struct sockaddr_in);
	struct sockaddr_in caddr;
	tb_int_t fd_c = accept(ps->fd, (struct sockaddr *)&caddr, &caddr_n);
	if (fd_c == -1) return TB_NULL;

	//tb_trace("connection from %s", inet_ntoa(caddr.sin_addr));

	// return client socket
	tb_socket_t* s = malloc(sizeof(tb_socket_t));
	tb_assert(s);
	if (!s) return TB_NULL;
	memset(s, 0, sizeof(tb_socket_t));
	s->fd = fd_c;
	s->type = ps->type;
	s->is_block = ps->is_block;

	return ((tb_handle_t)s);
}

