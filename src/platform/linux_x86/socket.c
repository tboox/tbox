/*!The Tiny Platform Library
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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

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
	tb_uint16_t 	port;
	tb_char_t 	host[TB_SOCKET_HOST_MAX];

}tb_socket_t;
/* /////////////////////////////////////////////////////////
 * implemention
 */

tb_bool_t tb_socket_init()
{
	return TB_TRUE;
}
void tb_socket_uninit()
{
}
tb_handle_t tb_socket_client_open(tb_char_t const* host, tb_uint16_t port, tb_int_t type, tb_bool_t is_block)
{
	if (type == TB_SOCKET_TYPE_UNKNOWN) return TB_NULL;

	//TB_DBG("socket open: %s %d", host, port);

	// create socket
	tb_int_t t, p;
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
	if (is_block == TB_TRUE) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
	else fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

	if (type == TB_SOCKET_TYPE_TCP)
	{
		// client
		struct sockaddr_in dest;
		dest.sin_family = AF_INET;
		dest.sin_port = htons(port);
		if (!inet_aton(host, &(dest.sin_addr))) 
		{
			struct hostent* h = gethostbyname(host);
			if (h) memcpy(&dest.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
			else return TB_NULL;
		}

		// connect host
		tb_int_t ret = 0;
		while (1)
		{
			ret = connect(fd, (struct sockaddr *)&dest, sizeof(dest));

			// if non-block
			if (ret < 0 && is_block == TB_FALSE)
			{
				if (errno == EINTR) continue;
				if (errno != EINPROGRESS && errno != EAGAIN)
				{
					if (fd >= 0) close(fd);
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
				tb_int_t optlen = sizeof(ret);
				getsockopt(fd, SOL_SOCKET, SO_ERROR, &ret, &optlen);
				if (ret != 0)
				{
					if (fd >= 0) close(fd);
					return TB_NULL;
				}
			}
			else if (ret >= 0) break;
			else
			{
				if (fd >= 0) close(fd);
				return TB_NULL;
			}
		}
	}

	// create socket
	tb_socket_t* s = malloc(sizeof(tb_socket_t));
	TB_ASSERT(s);
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
void tb_socket_close(tb_handle_t hsocket)
{
	TB_ASSERT_RETURN(hsocket);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	//TB_DBG("socket close");

	if (s && s->fd >= 0) 
	{
		close(s->fd);
		free(s);
	}
}

tb_int_t tb_socket_recv(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(hsocket, -1);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	//TB_DBG("socket_recv: %d", size);

	// non-block
	if (s->is_block == TB_FALSE)
	{
		while (1)
		{
			tb_int_t fd_max = s->fd;
			struct timeval tv;
			fd_set rfds;

			FD_ZERO(&rfds);
			FD_SET(s->fd, &rfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100 * 1000;
			tb_int_t ret = select(fd_max + 1, &rfds, NULL, NULL, &tv);

			if (ret > 0 && FD_ISSET(s->fd, &rfds)) 
			{
				tb_int_t len = recv(s->fd, data, size, 0);
				if (len < 0) 
				{
					if (errno != EINTR && errno != EAGAIN) return -1;
				} 
				else return len;
			}
			else if (ret < 0) return -1;
			else return 0; // no data
		}
	}
	// block
	else return recv(s->fd, data, size, 0);
}

tb_int_t tb_socket_send(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(hsocket, -1);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	//TB_DBG("socket_send: %d", size);

	// non-block
	if (s->is_block == TB_FALSE)
	{
		tb_int_t send_n = size;
		while (send_n > 0) 
		{
			tb_int_t fd_max = s->fd;
			struct timeval tv;
			fd_set wfds;

			FD_ZERO(&wfds);
			FD_SET(s->fd, &wfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100 * 1000;
			tb_int_t ret = select(fd_max + 1, NULL, &wfds, NULL, &tv);

			if (ret > 0 && FD_ISSET(s->fd, &wfds)) 
			{
				tb_int_t len = send(s->fd, data, send_n, 0);
				if (len < 0) 
				{
					if (errno != EINTR && errno != EAGAIN) return -1;
					continue;
				}
				send_n -= len;
				data += len;
			} 
			else if (ret < 0) return -1;
			else return 0;
		}
		return (tb_int_t)(size - send_n);
	}
	else return send(s->fd, data, size, 0);
}
tb_int_t tb_socket_recvfrom(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(hsocket, -1);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	// get host & port
	port = port? port : s->port;
	host = host? host : s->host;
	TB_ASSERT(port && host && host[0]);
	if (!port || !host || !host[0]) return -1;

	// recv data
	struct sockaddr_in dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	if (!inet_aton(host, &(dest.sin_addr))) 
	{
		struct hostent* h = gethostbyname(host);
		if (h) memcpy(&dest.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
		else return -1;
	}

	// non-block
	tb_int_t n = sizeof(dest);
	if (s->is_block == TB_FALSE)
	{
		while (1)
		{
			tb_int_t fd_max = s->fd;
			struct timeval tv;
			fd_set rfds;

			FD_ZERO(&rfds);
			FD_SET(s->fd, &rfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100 * 1000;
			tb_int_t ret = select(fd_max + 1, &rfds, NULL, NULL, &tv);

			if (ret > 0 && FD_ISSET(s->fd, &rfds)) 
			{
				tb_int_t len = recvfrom(s->fd, data, size, 0, (struct sockaddr*)&dest, &n);
				if (len < 0) 
				{
					if (errno != EINTR && errno != EAGAIN) return -1;
				} 
				else return len;
			}
			else if (ret < 0) return -1;
			else return 0; // no data
		}
	}
	// block
	else return recvfrom(s->fd, data, size, 0, (struct sockaddr*)&dest, &n);
}
tb_int_t tb_socket_sendto(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(hsocket, -1);
	tb_socket_t* s = (tb_socket_t*)hsocket;

	// get host & port
	port = port? port : s->port;
	host = host? host : s->host;
	TB_ASSERT(port && host && host[0]);
	if (!port || !host || !host[0]) return -1;

	// recv data
	struct sockaddr_in dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	if (!inet_aton(host, &(dest.sin_addr))) 
	{
		struct hostent* h = gethostbyname(host);
		if (h) memcpy(&dest.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
		else return -1;
	}

	// non-block
	if (s->is_block == TB_FALSE)
	{
		while (1)
		{
			tb_int_t fd_max = s->fd;
			struct timeval tv;
			fd_set rfds;

			FD_ZERO(&rfds);
			FD_SET(s->fd, &rfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100 * 1000;
			tb_int_t ret = select(fd_max + 1, &rfds, NULL, NULL, &tv);

			if (ret > 0 && FD_ISSET(s->fd, &rfds)) 
			{
				tb_int_t len = sendto(s->fd, data, size, 0, (struct sockaddr*)&dest, sizeof(dest));
				if (len < 0) 
				{
					if (errno != EINTR && errno != EAGAIN) return -1;
				} 
				else return len;
			}
			else if (ret < 0) return -1;
			else return 0; // no data
		}
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
		TB_DBG("tb_socket_server_open(type == UDP) is not implemention now");
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
	if (is_block == TB_TRUE) fcntl(fd_s, F_SETFL, fcntl(fd_s, F_GETFL) & ~O_NONBLOCK);
	else fcntl(fd_s, F_SETFL, fcntl(fd_s, F_GETFL) | O_NONBLOCK);

	// bind 
    if (bind(fd_s, (struct sockaddr *)&saddr, sizeof(struct sockaddr)) == -1)
	{
        TB_DBG("bind error!");
		close(fd_s);
		return TB_NULL;
    }

	// listen
    if (listen(fd_s, 20) == -1) 
	{
        TB_DBG("listen error!");
		close(fd_s);
		return TB_NULL;
    }

	// return server socket
	tb_socket_t* s = malloc(sizeof(tb_socket_t));
	TB_ASSERT(s);
	if (!s) return TB_NULL;
	memset(s, 0, sizeof(tb_socket_t));
	s->fd = fd_s;
	s->type = type;
	s->is_block = is_block;

	return ((tb_handle_t)s);
}
tb_handle_t tb_socket_server_accept(tb_handle_t hserver)
{
	TB_ASSERT_RETURN_VAL(hserver, TB_NULL);
	tb_socket_t* ps = (tb_socket_t*)hserver;
	if (ps->fd < 0) return TB_NULL;

	// accept client 
	tb_int_t caddr_n = sizeof(struct sockaddr_in);
	struct sockaddr_in caddr;
	tb_int_t fd_c = accept(ps->fd, (struct sockaddr *)&caddr, &caddr_n);
	if (fd_c == -1) return TB_NULL;

	//TB_DBG("connection from %s", inet_ntoa(caddr.sin_addr));

	// return client socket
	tb_socket_t* s = malloc(sizeof(tb_socket_t));
	TB_ASSERT(s);
	if (!s) return TB_NULL;
	memset(s, 0, sizeof(tb_socket_t));
	s->fd = fd_c;
	s->type = ps->type;
	s->is_block = ps->is_block;

	return ((tb_handle_t)s);
}

