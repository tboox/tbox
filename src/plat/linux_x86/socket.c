/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TPlat; 
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
#include "../tplat.h"
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
#define TPLAT_SOCKET_HOST_MAX 			(256)

/* /////////////////////////////////////////////////////////
 * types
 */
typedef struct __tplat_socket_t
{
	tplat_int_t 	fd;
	tplat_int_t 	type;
	tplat_bool_t 	is_block;
	tplat_uint16_t 	port;
	tplat_char_t 	host[TPLAT_SOCKET_HOST_MAX];

}tplat_socket_t;
/* /////////////////////////////////////////////////////////
 * implemention
 */

tplat_bool_t tplat_socket_init()
{
	return TPLAT_TRUE;
}
void tplat_socket_uninit()
{
}
tplat_handle_t tplat_socket_client_open(tplat_char_t const* host, tplat_uint16_t port, tplat_int_t type, tplat_bool_t is_block)
{
	if (type == TPLAT_SOCKET_TYPE_UNKNOWN) return TPLAT_INVALID_HANDLE;

	//TPLAT_DBG("socket open: %s %d", host, port);

	// create socket
	tplat_int_t t, p;
	if (type == TPLAT_SOCKET_TYPE_TCP)
	{
		t = SOCK_STREAM;
		p = IPPROTO_TCP;
		if (!host || !port) return TPLAT_INVALID_HANDLE;
	}
	else if (type == TPLAT_SOCKET_TYPE_UDP)
	{
		t = SOCK_DGRAM;
		p = IPPROTO_UDP;
	}
	else return TPLAT_INVALID_HANDLE;

	tplat_int_t fd = socket(AF_INET, t, p);
	if (fd < 0) return TPLAT_INVALID_HANDLE;

	// set block or non-block
	if (is_block == TPLAT_TRUE) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
	else fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

	if (type == TPLAT_SOCKET_TYPE_TCP)
	{
		// client
		struct sockaddr_in dest;
		dest.sin_family = AF_INET;
		dest.sin_port = htons(port);
		if (!inet_aton(host, &(dest.sin_addr))) 
		{
			struct hostent* h = gethostbyname(host);
			if (h) memcpy(&dest.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
			else return TPLAT_INVALID_HANDLE;
		}

		// connect host
		tplat_int_t ret = 0;
		while (1)
		{
			ret = connect(fd, (struct sockaddr *)&dest, sizeof(dest));

			// if non-block
			if (ret < 0 && is_block == TPLAT_FALSE)
			{
				if (errno == EINTR) continue;
				if (errno != EINPROGRESS && errno != EAGAIN)
				{
					if (fd >= 0) close(fd);
					return TPLAT_INVALID_HANDLE;
				}

				// wait until we are connected or until abort
				while (1) 
				{
					struct timeval tv;
					fd_set wfds;
					tplat_int_t fd_max = fd;

					FD_ZERO(&wfds);
					FD_SET(fd, &wfds);
					tv.tv_sec = 0;
					tv.tv_usec = 100 * 1000;
					ret = select(fd_max + 1, NULL, &wfds, NULL, &tv);
					if (ret > 0 && FD_ISSET(fd, &wfds)) break;
				}

				// test error
				tplat_int_t optlen = sizeof(ret);
				getsockopt(fd, SOL_SOCKET, SO_ERROR, &ret, &optlen);
				if (ret != 0)
				{
					if (fd >= 0) close(fd);
					return TPLAT_INVALID_HANDLE;
				}
			}
			else if (ret >= 0) break;
			else
			{
				if (fd >= 0) close(fd);
				return TPLAT_INVALID_HANDLE;
			}
		}
	}

	// create socket
	tplat_socket_t* s = malloc(sizeof(tplat_socket_t));
	TPLAT_ASSERT(s);
	if (!s) return TPLAT_INVALID_HANDLE;
	memset(s, 0, sizeof(tplat_socket_t));
	s->fd = fd;
	s->type = type;
	s->port = port;
	s->is_block = is_block;

	// save host
	if (host)
	{
		strncpy(s->host, host, TPLAT_SOCKET_HOST_MAX - 1);
		s->host[TPLAT_SOCKET_HOST_MAX - 1] = '\0';
	}

	return ((tplat_handle_t)s);
}
void tplat_socket_close(tplat_handle_t hsocket)
{
	TPLAT_ASSERT(hsocket != TPLAT_INVALID_HANDLE);
	if (hsocket == TPLAT_INVALID_HANDLE) return ;

	tplat_socket_t* s = (tplat_socket_t*)hsocket;
	TPLAT_ASSERT(s);

	//TPLAT_DBG("socket close");

	if (s && s->fd >= 0) 
	{
		close(s->fd);
		free(s);
	}
}

tplat_int_t tplat_socket_recv(tplat_handle_t hsocket, tplat_byte_t* data, tplat_size_t size)
{
	TPLAT_ASSERT(hsocket != TPLAT_INVALID_HANDLE);
	if (hsocket == TPLAT_INVALID_HANDLE) return -1;

	tplat_socket_t* s = (tplat_socket_t*)hsocket;
	TPLAT_ASSERT(s);

	//TPLAT_DBG("socket_recv: %d", size);

	// non-block
	if (s->is_block == TPLAT_FALSE)
	{
		while (1)
		{
			tplat_int_t fd_max = s->fd;
			struct timeval tv;
			fd_set rfds;

			FD_ZERO(&rfds);
			FD_SET(s->fd, &rfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100 * 1000;
			tplat_int_t ret = select(fd_max + 1, &rfds, NULL, NULL, &tv);

			if (ret > 0 && FD_ISSET(s->fd, &rfds)) 
			{
				tplat_int_t len = recv(s->fd, data, size, 0);
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

tplat_int_t tplat_socket_send(tplat_handle_t hsocket, tplat_byte_t* data, tplat_size_t size)
{
	TPLAT_ASSERT(hsocket != TPLAT_INVALID_HANDLE);
	if (hsocket == TPLAT_INVALID_HANDLE) return -1;

	tplat_socket_t* s = (tplat_socket_t*)hsocket;
	TPLAT_ASSERT(s);

	//TPLAT_DBG("socket_send: %d", size);

	// non-block
	if (s->is_block == TPLAT_FALSE)
	{
		tplat_int_t send_n = size;
		while (send_n > 0) 
		{
			tplat_int_t fd_max = s->fd;
			struct timeval tv;
			fd_set wfds;

			FD_ZERO(&wfds);
			FD_SET(s->fd, &wfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100 * 1000;
			tplat_int_t ret = select(fd_max + 1, NULL, &wfds, NULL, &tv);

			if (ret > 0 && FD_ISSET(s->fd, &wfds)) 
			{
				tplat_int_t len = send(s->fd, data, send_n, 0);
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
		return (tplat_int_t)(size - send_n);
	}
	else return send(s->fd, data, size, 0);
}
tplat_int_t tplat_socket_recvfrom(tplat_handle_t hsocket, tplat_char_t const* host, tplat_uint16_t port, tplat_byte_t* data, tplat_size_t size)
{
	TPLAT_ASSERT(hsocket != TPLAT_INVALID_HANDLE);
	if (hsocket == TPLAT_INVALID_HANDLE) return -1;

	tplat_socket_t* s = (tplat_socket_t*)hsocket;
	TPLAT_ASSERT(s);

	// get host & port
	port = port? port : s->port;
	host = host? host : s->host;
	TPLAT_ASSERT(port && host && host[0]);
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
	tplat_int_t n = sizeof(dest);
	if (s->is_block == TPLAT_FALSE)
	{
		while (1)
		{
			tplat_int_t fd_max = s->fd;
			struct timeval tv;
			fd_set rfds;

			FD_ZERO(&rfds);
			FD_SET(s->fd, &rfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100 * 1000;
			tplat_int_t ret = select(fd_max + 1, &rfds, NULL, NULL, &tv);

			if (ret > 0 && FD_ISSET(s->fd, &rfds)) 
			{
				tplat_int_t len = recvfrom(s->fd, data, size, 0, (struct sockaddr*)&dest, &n);
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
tplat_int_t tplat_socket_sendto(tplat_handle_t hsocket, tplat_char_t const* host, tplat_uint16_t port, tplat_byte_t* data, tplat_size_t size)
{
	TPLAT_ASSERT(hsocket != TPLAT_INVALID_HANDLE);
	if (hsocket == TPLAT_INVALID_HANDLE) return -1;

	tplat_socket_t* s = (tplat_socket_t*)hsocket;
	TPLAT_ASSERT(s);

	// get host & port
	port = port? port : s->port;
	host = host? host : s->host;
	TPLAT_ASSERT(port && host && host[0]);
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
	if (s->is_block == TPLAT_FALSE)
	{
		while (1)
		{
			tplat_int_t fd_max = s->fd;
			struct timeval tv;
			fd_set rfds;

			FD_ZERO(&rfds);
			FD_SET(s->fd, &rfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100 * 1000;
			tplat_int_t ret = select(fd_max + 1, &rfds, NULL, NULL, &tv);

			if (ret > 0 && FD_ISSET(s->fd, &rfds)) 
			{
				tplat_int_t len = sendto(s->fd, data, size, 0, (struct sockaddr*)&dest, sizeof(dest));
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
tplat_handle_t tplat_socket_server_open(tplat_uint16_t port, tplat_int_t type, tplat_bool_t is_block)
{
	if (!port || type == TPLAT_SOCKET_TYPE_UNKNOWN) return TPLAT_INVALID_HANDLE;

	// create socket
	tplat_int_t t;
	if (type == TPLAT_SOCKET_TYPE_TCP) t = SOCK_STREAM;
	else if (type == TPLAT_SOCKET_TYPE_UDP) t = SOCK_DGRAM;
	else return TPLAT_INVALID_HANDLE;

	// udp is not implemention now
	if (type == TPLAT_SOCKET_TYPE_UDP)
	{
		// not implemention now
		TPLAT_DBG("tplat_socket_server_open(type == UDP) is not implemention now");
		return TPLAT_INVALID_HANDLE;
	}

	// server socket
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	tplat_int_t fd_s = socket(AF_INET, t, 0);
	if (fd_s < 0) return TPLAT_INVALID_HANDLE;

	// set block or non-block
	if (is_block == TPLAT_TRUE) fcntl(fd_s, F_SETFL, fcntl(fd_s, F_GETFL) & ~O_NONBLOCK);
	else fcntl(fd_s, F_SETFL, fcntl(fd_s, F_GETFL) | O_NONBLOCK);

	// bind 
    if (bind(fd_s, (struct sockaddr *)&saddr, sizeof(struct sockaddr)) == -1)
	{
        TPLAT_DBG("bind error!");
		close(fd_s);
		return TPLAT_INVALID_HANDLE;
    }

	// listen
    if (listen(fd_s, 20) == -1) 
	{
        TPLAT_DBG("listen error!");
		close(fd_s);
		return TPLAT_INVALID_HANDLE;
    }

	// return server socket
	tplat_socket_t* s = malloc(sizeof(tplat_socket_t));
	TPLAT_ASSERT(s);
	if (!s) return TPLAT_INVALID_HANDLE;
	memset(s, 0, sizeof(tplat_socket_t));
	s->fd = fd_s;
	s->type = type;
	s->is_block = is_block;

	return ((tplat_handle_t)s);
}
tplat_handle_t tplat_socket_server_accept(tplat_handle_t hserver)
{
	TPLAT_ASSERT(hserver != TPLAT_INVALID_HANDLE);
	if (hserver == TPLAT_INVALID_HANDLE) return TPLAT_INVALID_HANDLE;

	tplat_socket_t* ps = (tplat_socket_t*)hserver;
	if (!ps || ps->fd < 0) return TPLAT_INVALID_HANDLE;

	// accept client 
	tplat_int_t caddr_n = sizeof(struct sockaddr_in);
	struct sockaddr_in caddr;
	tplat_int_t fd_c = accept(ps->fd, (struct sockaddr *)&caddr, &caddr_n);
	if (fd_c == -1) return TPLAT_INVALID_HANDLE;

	//TPLAT_DBG("connection from %s", inet_ntoa(caddr.sin_addr));

	// return client socket
	tplat_socket_t* s = malloc(sizeof(tplat_socket_t));
	TPLAT_ASSERT(s);
	if (!s) return TPLAT_INVALID_HANDLE;
	memset(s, 0, sizeof(tplat_socket_t));
	s->fd = fd_c;
	s->type = ps->type;
	s->is_block = ps->is_block;

	return ((tplat_handle_t)s);
}

#if 0
tplat_bool_t tplat_socket_peername(tplat_handle_t hsocket, tplat_char_t* ip, tplat_int_t* port)
{
	TPLAT_ASSERT(hsocket != TPLAT_INVALID_HANDLE);
	if (hsocket == TPLAT_INVALID_HANDLE || !ip || !port) return TPLAT_FALSE;

	tplat_socket_t* s = (tplat_socket_t*)hsocket;
	TPLAT_ASSERT(s);
	if (s->fd < 0) return TPLAT_FALSE;

	struct sockaddr_in 	addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	tplat_int_t 		addr_n = sizeof(struct sockaddr_in);

	if (getpeername(s->fd, (struct sockaddr *)&addr, &addr_n) == -1)
		return TPLAT_FALSE;

	// parse ip
	memcpy(ip, inet_ntoa(addr.sin_addr), 16);
	ip[16] = 0;

	*port = ntohs(addr.sin_port);

	return TPLAT_TRUE;
}

#endif
