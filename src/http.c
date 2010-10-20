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
#include "regex.h"

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
static tb_size_t tb_http_url_split(tb_char_t const* url, tb_char_t* host, tb_char_t* path)
{
	TB_ASSERT(url && host);
	if (!url || !host) return 0;

	// {the invalid port
	tb_size_t port = 0;

	// create regex
	tplat_handle_t hregex1 = TB_INVALID_HANDLE;
	tplat_handle_t hregex2 = TB_INVALID_HANDLE;
	tplat_handle_t hregex3 = TB_INVALID_HANDLE;
	tplat_handle_t hregex4 = TB_INVALID_HANDLE;

	hregex1 = tb_regex_create("http://(.+):(\\d+)(/.+)", TB_REGEX_NULL);
	if (hregex1 == TB_INVALID_HANDLE) goto fail;
	
	hregex2 = tb_regex_create("http://(.+?)(/.+)", TB_REGEX_NULL);
	if (hregex2 == TB_INVALID_HANDLE) goto fail;

	hregex3 = tb_regex_create("(.+):(\\d+)(/.+)", TB_REGEX_NULL);
	if (hregex3 == TB_INVALID_HANDLE) goto fail;

	hregex4 = tb_regex_create("(.+?)(/.+)", TB_REGEX_NULL);
	if (hregex4 == TB_INVALID_HANDLE) goto fail;

	// {parse url
	tplat_handle_t hregex = TB_INVALID_HANDLE;
	if (TPLAT_TRUE == tb_regex_exec(hregex1, url, TB_REGEX_NULL))
		hregex = hregex1;
	else if (TPLAT_TRUE == tb_regex_exec(hregex2, url, TB_REGEX_NULL))
		hregex = hregex2;
	else if (TPLAT_TRUE == tb_regex_exec(hregex3, url, TB_REGEX_NULL))
		hregex = hregex3;
	else if (TPLAT_TRUE == tb_regex_exec(hregex4, url, TB_REGEX_NULL))
		hregex = hregex4;
	else goto fail;
	
	// {
	tplat_int_t match_n = tb_regex_count(hregex);
	if (match_n <= 0) goto fail;

	// parse host
	if (TB_NULL == tb_regex_get(hregex, 1, host, TB_NULL)) goto fail;

	// check host
	if (host[0] == '/') goto fail;

	if (hregex == hregex1 || hregex == hregex3)
	{
		// parse path
		if (TB_NULL == tb_regex_get(hregex, 3, path, TB_NULL)) goto fail;

		// {parse port
		tb_char_t port_s[256];
		if (TB_NULL == tb_regex_get(hregex, 2, port_s, TB_NULL)) goto fail;
		port = atoi(port_s);
		// }

	}
	else
	{
		// parse path
		if (TB_NULL == tb_regex_get(hregex, 2, path, TB_NULL)) goto fail;

		// the default port
		port = 80;
	}

fail:
	if (hregex1) tb_regex_destroy(hregex1);
	if (hregex2) tb_regex_destroy(hregex2);
	if (hregex3) tb_regex_destroy(hregex3);
	if (hregex4) tb_regex_destroy(hregex4);
	return port;
	// }}}
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
		while (!isspace(*p) && *p != '\0') p++;
		while (isspace(*p)) p++;

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
		while (isspace(*p)) p++;

		// parse location
		if (!strcmp(tag, "Location")) 
		{
			//TB_HTTP_DBG("redirect to: %s", p);

			// redirect it
			if (http->code == 302 || http->code == 303)
			{
				// next url
				strncpy(http->url, p, TB_HTTP_PATH_MAX - 1);
				http->url[TB_HTTP_PATH_MAX - 1] = '\0';
				http->redirect = 1;
				
				return TB_TRUE;
			}
			else return TB_FALSE;
		}
		// parse content size
		else if (!strcmp (tag, "Content-Length"))
		{
			http->size = atol(p);
		}
		// parse content range: "bytes $from-$to/$document_size"
		else if (!strcmp (tag, "Content-Range"))
		{
			tb_char_t const* slash = TB_NULL;
			tb_int_t offset = 0, filesize = 0;
			if (!strncmp (p, "bytes ", 6)) 
			{
				p += 6;
				offset = atol(p);
				if ((slash = strchr(p, '/')) && strlen(slash) > 0)
					filesize = atol(slash + 1);
				TB_HTTP_DBG("range: %d - %d", offset, filesize);
			}
			// no stream, be able to seek
			http->stream = 0;
		}
	}

	return TB_TRUE;
	// }
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

	// split url
	tb_char_t host[1024];
	tb_char_t path[2048];
	tb_size_t port = tb_http_url_split(url, host, path);
	if (!port) return TB_FALSE;
	TB_HTTP_DBG("open: %s:%d %s:%s%s%s", host, port, method == TB_HTTP_METHOD_GET? "GET" : "POST", path, args? "?" : "", args? args : "");

	// open socket
	if (!http->redirect)
	{
		if (http->socket != TPLAT_INVALID_HANDLE) tplat_socket_close(http->socket);
		http->socket = tplat_socket_client_open(host, port, TPLAT_SOCKET_TYPE_TCP, TB_FALSE);
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
				"\r\n", path, args? "?" : "", args? args : "", host);
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
					"%s", path, host, strlen(args), args);
		}
		else
		{
			request_n = snprintf(request, TB_HTTP_HEAD_MAX - 1,
			"POST %s HTTP/1.1\r\n"
			"Accept: */*\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n", path, host);
		}
	}

	if (request_n < 0 || request_n >= TB_HTTP_HEAD_MAX) goto fail;
	request[request_n] = 0;
	//TB_HTTP_DBG("request: %s", request);
	
	// send http request
	if (request_n != tb_http_send_data(http, request, request_n, TB_TRUE)) goto fail;
	
	// save url
	strncpy(http->url, url, TB_HTTP_PATH_MAX - 1);
	http->url[TB_HTTP_PATH_MAX - 1] = '\0';

	// reset redirect
	http->redirect = 0;
	if (!http->redirect) http->redirect_n = 0;
	
	// handle http reply
	tb_char_t ch = 0;
	tb_size_t line_idx = 0;
	tb_char_t* line = http->line;
	tb_char_t* p = line;
	while (1)
	{
		// recv char
		ch = tb_http_recv_char(http);

		// is fail?
		if (ch < 0) goto fail;

		// is line?
		if (ch == '\n') 
		{
			// finish line
			if (p > line && p[-1] == '\r')
				p--;
			*p = '\0';
	
			// is end?
			if (line[0] == '\0') break;
	
			// new line
			p = line;

			// process line
			if (TB_FALSE == tb_http_process_line(http, line_idx)) goto fail;
			line_idx++;
		}
		// append char to line
		else 
		{
			if ((p - line) < TB_HTTP_LINE_MAX - 1)
			*p++ = ch;
		}
	}

	// redirect?
	if (http->redirect) 
	{
		if (http->redirect_n < TB_HTTP_REDIRECT_MAX)
		{
			TB_HTTP_DBG("redirect: %s", http->url);
			http->redirect_n++;
			return tb_http_open(http, http->url, TB_NULL, TB_HTTP_METHOD_GET);
		}
		else return TB_FALSE;
	}

	return TB_TRUE;

fail:
	if (http) tb_http_close(http);
	return TB_FALSE;
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
tb_char_t const* tb_http_url(tb_http_t* http)
{	
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return TB_NULL;

	return http->url;
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
		tb_int_t try_n = 100;
		while (send_n < size)
		{
			tb_int_t ret = tplat_socket_send(http->socket, data + send_n, size - send_n);
			if (ret < 0) break;
			else if (!ret)
			{
				if (try_n > 0) try_n--;
				else break;
			}
			else send_n += ret;
			tplat_msleep(1);
		}

	}
	else send_n = tplat_socket_send(http->socket, data, size);
	return send_n;
}
tb_int_t tb_http_recv_data(tb_http_t* http, tb_byte_t* data, tb_size_t size, tb_bool_t block)
{
	TB_ASSERT(http && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || http->socket == TPLAT_INVALID_HANDLE) return 0;

	tb_int_t recv_n = 0;
	if (block == TB_TRUE)
	{
		tb_int_t try_n = 100;
		while (recv_n < size)
		{
			tb_int_t ret = tplat_socket_recv(http->socket, data + recv_n, size - recv_n);
			if (ret < 0) break;
			else if (!ret)
			{
				if (try_n > 0) try_n--;
				else break;
			}
			else recv_n += ret;
			tplat_msleep(1);
		}

	}
	else recv_n = tplat_socket_recv(http->socket, data, size);
	return recv_n;
}
tb_char_t const* tb_http_recv_string(tb_http_t* http, tb_string_t* string)
{
	TB_ASSERT(http && string && http->socket != TPLAT_INVALID_HANDLE);
	if (!http || !string || http->socket == TPLAT_INVALID_HANDLE) return TB_NULL;

	tb_int_t recv_n = 0;
	tb_int_t try_n = 100;
	while (recv_n < http->size)
	{
		tb_char_t 	s[4096];
		tb_int_t 	n = tplat_socket_recv(http->socket, s, 4095);
		if (n < 0) break;
		else if (!n)
		{
			if (try_n > 0) try_n--;
			else break;
		}
		else 
		{
			s[n] = '\0';
			tb_string_append_c_string_with_size(string, s, n);
			recv_n += n;
		}
		tplat_msleep(1);
	}
	return (recv_n == http->size? tb_string_c_string(string) : TB_NULL);
}
