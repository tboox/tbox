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
 * \file		http.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "http.h"
#include "../math/math.h"
#include "../conv.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifndef TPLAT_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO
#if 1
# 	define TB_HTTP_DBG(fmt, arg...) 			TB_DBG("http: " fmt, ##arg)
#else
# 	define TB_HTTP_DBG(fmt, arg...)
#endif

#else
# 	define TB_HTTP_DBG
#endif
/* ////////////////////////////////////////////////////////////////////////
 * types
 */

/* ////////////////////////////////////////////////////////////////////////
 * details
 */

static tb_bool_t tb_http_split(tb_http_t* http, tb_char_t const* url)
{
	TB_ASSERT(url && http);
	if (!url || !http) return TB_FALSE;

	// get url size
	tb_int_t n = tb_cstring_size(url);
	if (n <= 0) return TB_FALSE;

	// get url pointer
	tb_char_t const* p = url;
	tb_char_t const* e = url + n;

	// is file root?
	if (p[0] == '/') 
	{
		if (http->redirect)
		{
			tb_cstring_ncopy(http->path, p, TB_HTTP_PATH_MAX - 1);
			http->path[TB_HTTP_PATH_MAX - 1] = '\0';
			return TB_TRUE;
		}
		else return TB_FALSE;
	}

	// filter http://
	if (n > 7 
		&& p[0] == 'h'
		&& p[1] == 't'
		&& p[2] == 't'
		&& p[3] == 'p'
		&& p[4] == ':'
		&& p[5] == '/'
		&& p[6] == '/')
	{
		p += 7;
	}

	// get host
	tb_char_t* pb = http->host;
	tb_char_t* pe = http->host + TB_HTTP_HOST_MAX - 1;
	while (p < e && pb < pe && *p && *p != '/' && *p != ':') *pb++ = *p++;
	if (p == e) return TB_FALSE;
	*pb = '\0';
	TB_HTTP_DBG("host: %s", http->host);

	// get port
	if (*p == ':')
	{
		tb_char_t port[12];
		pb = port;
		pe = port + 12 - 1;
		for (p++; p < e && pb < pe && *p && *p != '/'; ) *pb++ = *p++;
		if (p == e) return TB_FALSE;
		*pb = '\0';
		http->port = TB_CONV_S10TOU32(port);
	}
	else http->port = TB_HTTP_PORT_DEFAULT;
	TB_HTTP_DBG("port: %d", http->port);

	// get path
	pb = http->path;
	pe = http->path + TB_HTTP_PATH_MAX - 1;
	while (p < e && pb < pe && *p) *pb++ = *p++;
	*pb = '\0';
	TB_HTTP_DBG("path: %s", http->path);

	return TB_TRUE;

fail:
	return TB_FALSE;
}
static tb_char_t tb_http_recv_char(tb_http_t* http)
{
	tb_char_t ch[1];
	if (1 != tb_http_recv_data(http, ch, 1, TB_TRUE)) return '\0';
	else return ch[0];
}
/*
 * HTTP/1.1 206 Partial Content
 * Date: Fri, 23 Apr 2010 05:25:45 GMT
 * Server: Apache/2.2.9 (Ubuntu) PHP/5.2.6-2ubuntu4.5 with Suhosin-Patch
 * Last-Modified: Mon, 08 Mar 2010 09:58:09 GMT
 * ETag: "6cc014-8f47f-481471a322e40"
 * Accept-Ranges: bytes
 * Content-Length: 586879
 * Content-Range: bytes 0-586878/586879
 * Connection: close
 * Content-Type: application/x-shockwave-flash
 */
static tb_bool_t tb_http_process_line(tb_http_t* http, tb_size_t line_idx)
{
	tb_char_t* line = http->line;
	TB_HTTP_DBG("%s", line);

	// { process http code
	tb_char_t* p = line;
	tb_char_t* tag = TB_NULL;
	if (!line_idx)
	{
		while (!TB_CONV_ISSPACE(*p) && *p != '\0') p++;
		while (TB_CONV_ISSPACE(*p)) p++;

		http->code = strtol(p, TB_NULL, 10);
		TB_HTTP_DBG("code: %d", http->code);

		// check error code: 4xx & 5xx
		if (http->code >= 400 && http->code < 600) return TB_FALSE;
	}
	else
	{
		// parse tag
		while (*p != '\0' && *p != ':') p++;
		if (*p != ':') return TB_TRUE;
		*p = '\0';
		tag = line;
		p++;
		while (TB_CONV_ISSPACE(*p)) p++;

		// parse location
		if (!tb_cstring_compare(tag, "Location")) 
		{
			//TB_HTTP_DBG("redirect to: %s", p);

			// redirect it
			if (http->code == 301 || http->code == 302 || http->code == 303)
			{
				// next url
				http->redirect = 1;
				
				// split url
				if (TB_FALSE == tb_http_split(http, p)) return TB_FALSE;
				TB_HTTP_DBG("redirect: %s:%d %s", http->host, http->port, http->path);

				// the host is changed, open new socket
				if (p[0] != '/') http->changed = 1;

				return TB_TRUE;
			}
			else return TB_FALSE;
		}
		// parse content size
		else if (!tb_cstring_compare (tag, "Content-Length"))
		{
			http->size = TB_CONV_S10TOU32(p);
		}
		// parse content range: "bytes $from-$to/$document_size"
		else if (!tb_cstring_compare (tag, "Content-Range"))
		{
			tb_char_t const* slash = TB_NULL;
			tb_int_t offset = 0, filesize = 0;
			if (!strncmp (p, "bytes ", 6)) 
			{
				p += 6;
				offset = TB_CONV_S10TOU32(p);
				if ((slash = strchr(p, '/')) && tb_cstring_size(slash) > 0)
					filesize = TB_CONV_S10TOU32(slash + 1);
				TB_HTTP_DBG("range: %d - %d", offset, filesize);
			}
			// no stream, be able to seek
			http->stream = 0;
		}
		// parse content type
		else if (!tb_cstring_compare (tag, "Content-Type"))
		{
			//TB_DBG("type: %s", p);
		}
		// parse transfer encoding
		else if (!tb_cstring_compare (tag, "Transfer-Encoding"))
		{
			if (!tb_cstring_compare(p, "chunked")) 
			{
				http->chunked = 1;
				http->stream = 1;
			}
		}
	}

	return TB_TRUE;
	// }
}
static tb_char_t const* tb_http_recv_line(tb_http_t* http)
{
	tb_char_t ch = 0;
	tb_char_t* line = http->line;
	tb_char_t* p = line;
	while (1)
	{
		// recv char
		ch = tb_http_recv_char(http);

		// is fail?
		if (ch < 0) break;

		// is line?
		if (ch == '\n') 
		{
			// finish line
			if (p > line && p[-1] == '\r')
				p--;
			*p = '\0';
	
			return line;
		}
		// append char to line
		else 
		{
			if ((p - line) < TB_HTTP_LINE_MAX - 1)
			*p++ = ch;
		}
	}
	return TB_NULL;
}
static tb_bool_t tb_http_open_host(tb_http_t* http, tb_char_t const* args, tb_http_method_t method)
{
	TB_ASSERT(http);
	if (!http) return TB_FALSE;

	// open socket
	if (http->socket == TPLAT_INVALID_HANDLE || http->changed)
	{
		if (http->socket != TPLAT_INVALID_HANDLE) tplat_socket_close(http->socket);
		http->socket = tplat_socket_client_open(http->host, http->port, TPLAT_SOCKET_TYPE_TCP, TB_FALSE);
	}

	// check socket
	TB_ASSERT(http->socket != TPLAT_INVALID_HANDLE);
	if (http->socket == TPLAT_INVALID_HANDLE) return TB_FALSE;
	
	// format http request
	tb_char_t 	request[TB_HTTP_HEAD_MAX];
	tb_int_t 	request_n = 0;
	
	if (method == TB_HTTP_METHOD_GET)
	{
		request_n = snprintf(request, TB_HTTP_HEAD_MAX - 1,
				"GET %s%s%s HTTP/1.1\r\n"
				"Accept: */*\r\n"
				"Host: %s\r\n"
				"Connection: close\r\n"
				"\r\n", http->path, args? "?" : "", args? args : "", http->host);
	}
	else
	{
		if (args)
		{
			request_n = snprintf(request, TB_HTTP_HEAD_MAX - 1,
					"POST %s HTTP/1.1\r\n"
					"Accept: */*\r\n"
					"Host: %s\r\n"
					"Content-Type: application/x-www-form-urlencoded\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					"%s", http->path, http->host, tb_cstring_size(args), args);
		}
		else
		{
			request_n = snprintf(request, TB_HTTP_HEAD_MAX - 1,
			"POST %s HTTP/1.1\r\n"
			"Accept: */*\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n", http->path, http->host);
		}
	}

	if (request_n < 0 || request_n >= TB_HTTP_HEAD_MAX) goto fail;
	request[request_n] = 0;
	//TB_HTTP_DBG("request: %s", request);
	
	// send http request
	if (request_n != tb_http_send_data(http, request, request_n, TB_TRUE)) goto fail;

	// reset info
	http->size = 0;
	http->code = 0;
	http->stream = 1;
	http->chunked = 0;
	http->redirect = 0;
	if (!http->redirect) http->redirect_n = 0;

	// process reply
	tb_size_t line_idx = 0;
	while (1)
	{
		tb_char_t const* line = tb_http_recv_line(http);
		if (line)
		{
			// is end?
			if (line[0] == '\0') break;

			// process line
			if (TB_FALSE == tb_http_process_line(http, line_idx)) goto fail;
			line_idx++;
		}
		else break;
	}

	// redirect?
	if (http->redirect) 
	{
		if (http->redirect_n < TB_HTTP_REDIRECT_MAX)
		{
			http->redirect_n++;
			return tb_http_open_host(http, TB_NULL, TB_HTTP_METHOD_GET);
		}
		else return TB_FALSE;
	}

	return TB_TRUE;

fail:
	if (http) tb_http_close(http);
	return TB_FALSE;
}
/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_http_t* tb_http_create()
{
	// alloc
	tb_http_t* http = tb_malloc(sizeof(tb_http_t));
	if (!http) return TB_NULL;

	// init
	memset(http, 0, sizeof(tb_http_t));
	http->socket = TPLAT_INVALID_HANDLE;
	http->stream = 1;
	http->port = TB_HTTP_PORT_DEFAULT;

	return http;
}
void tb_http_destroy(tb_http_t* http)
{
	if (http)
	{
		if (http->socket != TPLAT_INVALID_HANDLE) tplat_socket_close(http->socket);
		tb_free(http);
	}
}

tb_bool_t tb_http_open(tb_http_t* http, tb_char_t const* url, tb_char_t const* args, tb_http_method_t method)
{
	TB_ASSERT(http && url);
	if (!http || !url) return TB_FALSE;

	// close old socket
	tb_http_close(http);

	// split url
	if (TB_FALSE == tb_http_split(http, url)) return TB_FALSE;
	TB_HTTP_DBG("open: %s:%d %s:%s%s%s", http->host, http->port, method == TB_HTTP_METHOD_GET? "GET" : "POST", http->path, args? "?" : "", args? args : "");

	// open host
	return tb_http_open_host(http, args, method);
}
void tb_http_close(tb_http_t* http)
{
	if (http)
	{
		// close socket
		if (http->socket != TPLAT_INVALID_HANDLE) tplat_socket_close(http->socket);
		http->socket = TPLAT_INVALID_HANDLE;

		// clear
		memset(http, 0, sizeof(tb_http_t));
		http->socket = TPLAT_INVALID_HANDLE;
		http->stream = 1;
		http->port = TB_HTTP_PORT_DEFAULT;
	}
}
tb_size_t tb_http_size(tb_http_t* http)
{	
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return 0;

	return (http->size > 0? http->size : 0);
}
tb_size_t tb_http_code(tb_http_t* http)
{
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return 0;

	return (http->code > 0? http->code : 0);
}
tb_char_t const* tb_http_host(tb_http_t* http)
{	
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return TB_NULL;

	return http->host;
}
tb_char_t const* tb_http_path(tb_http_t* http)
{	
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return TB_NULL;

	return http->path;
}
tb_size_t tb_http_port(tb_http_t* http)
{	
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return 0;

	return http->port;
}
tb_bool_t tb_http_stream(tb_http_t* http)
{
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return TB_TRUE;

	return (http->stream > 0? TB_TRUE : TB_FALSE);
}
tb_int_t tb_http_send_data(tb_http_t* http, tb_byte_t* data, tb_size_t size, tb_bool_t block)
{
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return 0;

	tb_int_t send_n = 0;
	if (block == TB_TRUE)
	{
		tb_int_t try = 100;
		while (send_n < size)
		{
			tb_int_t ret = tplat_socket_send(http->socket, data + send_n, size - send_n);
			if (ret < 0) break;
			else if (!ret)
			{
				if (try-- <= 0) break;
			}
			else send_n += ret;
		}

	}
	else send_n = tplat_socket_send(http->socket, data, size);
	//TB_DBG("send: %d", send_n);
	return send_n;
}
tb_int_t tb_http_recv_data(tb_http_t* http, tb_byte_t* data, tb_size_t size, tb_bool_t block)
{
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return 0;

	tb_int_t recv_n = 0;
	if (block == TB_TRUE)
	{
		tb_int_t try = 100;
		while (recv_n < size)
		{
			tb_int_t ret = tplat_socket_recv(http->socket, data + recv_n, size - recv_n);
			if (ret < 0) break;
			else if (!ret)
			{
				if (try-- <= 0) break;
			}
			else recv_n += ret;
		}

	}
	else recv_n = tplat_socket_recv(http->socket, data, size);
	//TB_DBG("recv: %d", recv_n);
	return recv_n;
}

tb_char_t const* tb_http_recv_string(tb_http_t* http, tb_string_t* string)
{
	TB_ASSERT(http && string && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || !string || http->socket == TPLAT_INVALID_HANDLE) return TB_NULL;

	if (http->chunked)
	{
		tb_int_t recv_n = 0;
		while (1)
		{
			// recv chunk header
			tb_char_t const* line = tb_http_recv_line(http);
			if (line) 
			{
				// get chunk size
				tb_char_t* stop;
				tb_int_t size = strtol(line, &stop, 16);
				//TB_DBG("%s %d", line, size);

				// is end?
				if (!size) break;

				// recv chunk data
				tb_int_t chunk_n = 0;
				tb_int_t try = 100;
				while (chunk_n < size)
				{
					tb_char_t 	s[4096];
					tb_int_t 	n = tplat_socket_recv(http->socket, s, TB_MATH_MIN(size - chunk_n, 4095));
					if (n < 0) break;
					else if (!n)
					{
						if (try-- <= 0) break;
					}
					else 
					{
						s[n] = '\0';
						tb_string_append_c_string_with_size(string, s, n);
						chunk_n += n;
					}
				}
				recv_n += chunk_n;
				//TB_DBG("recv: %d %s", recv_n, tb_string_c_string(string));
			}
			else break;
		}

		http->size = recv_n;
		return (recv_n > 0? tb_string_c_string(string) : TB_NULL);
	}
	else
	{
		tb_int_t recv_n = 0;
		tb_int_t try = 100;
		while (recv_n < http->size)
		{
			tb_char_t 	s[4096];
			tb_int_t 	n = tplat_socket_recv(http->socket, s, TB_MATH_MIN(http->size - recv_n, 4095));
			if (n < 0) break;
			else if (!n)
			{
				if (try-- <= 0) break;
			}
			else 
			{
				s[n] = '\0';
				tb_string_append_c_string_with_size(string, s, n);
				recv_n += n;
			}
		}
		return (recv_n == http->size? tb_string_c_string(string) : TB_NULL);
	}
}
tb_char_t const* tb_http_url(tb_http_t* http, tb_string_t* url)
{
	TB_ASSERT(http && url && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || !url || http->socket == TPLAT_INVALID_HANDLE) return TB_NULL;

	tb_string_assign_c_string(url, "http://");
	tb_string_append_c_string(url, tb_http_host(http));
	if (tb_http_port(http) != TB_HTTP_PORT_DEFAULT)
		tb_string_append_format(url, ":%d", tb_http_port(http));
	return tb_string_append_c_string(url, tb_http_path(http));
}
