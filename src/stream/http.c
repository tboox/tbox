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
 * \http		http.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../regex.h"

/* /////////////////////////////////////////////////////////
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

#define TB_HTTP_PATH_MAX 						(8192)
#define TB_HTTP_LINE_MAX 						(8192)
/* /////////////////////////////////////////////////////////
 * types
 */

// the http context type
typedef struct __tb_http_context_t
{
	// the http code
	tb_int_t 			code;

	// the file size
	tb_int_t 			filesize;

	// is redirect
	tb_int_t 			is_redirect;

	// is stream
	tb_int_t 			is_stream;
	
	// the url
	tb_char_t 			url[TB_HTTP_PATH_MAX];

	// the process line
	tb_char_t 			line[TB_HTTP_LINE_MAX];

}tb_http_context_t;

/* /////////////////////////////////////////////////////////
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
static tb_bool_t tb_http_send(tplat_handle_t hsocket, tb_byte_t const* data, tb_size_t size)
{
	tb_int_t send_n = 0;
	tb_int_t try_n = 100;
	while (send_n < size)
	{
		tb_int_t ret = tplat_socket_send(hsocket, data + send_n, size - send_n);
		if (ret < 0) break;
		else if (!ret)
		{
			if (try_n > 0) try_n--;
			else break;
		}
		else send_n += ret;
		tplat_usleep(200);
	}

	//TB_HTTP_DBG("send(%d): %d", size, send_n);
	return ((send_n == size)? TB_TRUE : TB_FALSE);
}
static tb_bool_t tb_http_recv(tplat_handle_t hsocket, tb_byte_t* data, tb_size_t size)
{
	tb_int_t recv_n = 0;
	tb_int_t try_n = 100;
	while (recv_n < size)
	{
		tb_int_t ret = tplat_socket_recv(hsocket, data + recv_n, size - recv_n);
		if (ret < 0) break;
		else if (!ret)
		{
			if (try_n > 0) try_n--;
			else break;
		}
		else recv_n += ret;
		tplat_usleep(200);
	}

	//TB_HTTP_DBG("recv(%d): %d", size, recv_n);
	return ((recv_n == size)? TB_TRUE : TB_FALSE);
}
static tb_char_t tb_http_recv_char(tplat_handle_t hsocket)
{
	tb_char_t ch[1];
	if (TB_FALSE == tb_http_recv(hsocket, ch, 1)) return 0;
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
static tb_bool_t tb_http_process_line(tb_http_context_t* ctx, tb_size_t line_idx)
{
	tb_char_t* line = ctx->line;
	TB_HTTP_DBG("%s", line);

	// { process http code
	tb_char_t* p = line;
	tb_char_t* tag = TB_NULL;
	if (!line_idx)
	{
		while (!isspace(*p) && *p != '\0') p++;
		while (isspace(*p)) p++;

		// {
		ctx->code = strtol(p, TB_NULL, 10);
		TB_HTTP_DBG("code: %d", ctx->code);

		// check error code: 4xx & 5xx
		if (ctx->code >= 400 && ctx->code < 600) return TB_FALSE;
		// }
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
			if (ctx->code == 302 || ctx->code == 303)
			{
				// next url
				strncpy(ctx->url, p, TB_HTTP_PATH_MAX - 1);
				ctx->url[TB_HTTP_PATH_MAX - 1] = '\0';
				ctx->is_redirect = 1;
				
				return TB_TRUE;
			}
			else return TB_FALSE;
		}
		// parse content size
		else if (!strcmp (tag, "Content-Length"))
		{
			tb_int_t filesize = atol(p);
			if (filesize > 0) ctx->filesize = filesize;
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
			// be able to seek
			ctx->is_stream = 0;
		}
	}

	return TB_TRUE;
	// }
}
static tb_bool_t tb_http_stream_seek(tb_stream_t* st, tb_int_t offset, tb_stream_seek_t flag)
{
	tb_http_stream_t* hst = st;
	if (hst && !(st->flag & TB_STREAM_FLAG_IS_ZLIB))
	{
		// get context
		tb_http_context_t* ctx = st->pdata;
		TB_ASSERT(ctx);
		if (!ctx) return 0;



		return TB_TRUE;
	}
	else return TB_FALSE;
}
static tplat_bool_t tb_http_try_connect(tb_http_stream_t* hst, tb_char_t const* host, tb_size_t port, tb_char_t const* path)
{
	// get context
	tb_http_context_t* ctx = hst->base.pdata;
	TB_ASSERT(ctx);
	if (!ctx) return TB_FALSE;

	// connect to host if not redirect
	if (!ctx->is_redirect) hst->hsocket = tplat_socket_client_open(host, port, TPLAT_SOCKET_TYPE_TCP, TB_FALSE);

	// check socket
	TB_ASSERT(hst->hsocket != TB_INVALID_HANDLE);
	if (hst->hsocket == TPLAT_INVALID_HANDLE) return TB_FALSE;
	
	// { format http request
	tb_char_t request[4096];

#if 0
	tb_int_t request_n = snprintf(request, 4096,
			"GET %s HTTP/1.1\r\n"
			"Accept: */*\r\n"
			"Range: bytes=%d-\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n", path, 0, host);
#else
	tb_int_t request_n = snprintf(request, 4096,
			"GET %s HTTP/1.1\r\n"
			"Accept: */*\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n", path, host);
#endif

	if (request_n < 0 || request_n >= 4096) goto fail;
	request[request_n] = 0;
	//TB_HTTP_DBG("request: %s", request);

	// send http request
	if (TB_FALSE == tb_http_send(hst->hsocket, request, request_n)) goto fail;

	// {handle reply
	tb_char_t ch = 0;
	tb_size_t line_idx = 0;
	tb_char_t* line = ctx->line;
	tb_char_t* p = line;
	while (1)
	{
		// recv char
		ch = tb_http_recv_char(hst->hsocket);

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
			if (TB_FALSE == tb_http_process_line(ctx, line_idx)) goto fail;
			line_idx++;
		}
		// append char to line
		else 
		{
			if ((p - line) < TB_HTTP_LINE_MAX - 1)
			*p++ = ch;
		}
	}

	return TB_TRUE;

fail:
	if (hst->hsocket != TB_INVALID_HANDLE) tplat_socket_close(hst->hsocket);
	hst->hsocket = TB_INVALID_HANDLE;
	return TB_FALSE;
	// }}
}
static tplat_bool_t tb_http_connect(tb_http_stream_t* hst, tb_char_t const* url)
{
	// get context
	tb_http_context_t* ctx = hst->base.pdata;
	TB_ASSERT(ctx && url);
	if (!ctx || !url) return TB_FALSE;

	// the first url
	strncpy(ctx->url, url, TB_HTTP_PATH_MAX - 1);
	ctx->url[TB_HTTP_PATH_MAX - 1] = '\0';

	tb_int_t try_n = 10;
	do
	{
		// split url
		tb_char_t host[1024];
		tb_char_t path[2048];
		tb_size_t port = tb_http_url_split(ctx->url, host, path);
		if (!port) return TB_FALSE;
		TB_HTTP_DBG("%s to http: %s:%d at %s.", ctx->is_redirect? "redirect" : "connect", host, port, path);

		// reset status
		ctx->filesize = 0;
		ctx->is_redirect = 0;
		ctx->is_stream = 1;
	
		// try connecting
		if (TB_TRUE == tb_http_try_connect(hst, host, port, path) && !ctx->is_redirect) 
		{
			// save filesize
			hst->size = ctx->filesize;

			// save url
			tb_string_assign_c_string_by_ref(&hst->base.url, ctx->url);

#if 0
			// is able to seek?
			if (ctx->is_stream) hst->base.seek = TB_NULL;
			else hst->base.seek = tb_http_stream_seek;
#endif

			TB_HTTP_DBG("filesize: %d", ctx->filesize);
			TB_HTTP_DBG("is_stream: %d", ctx->is_stream);

			return TB_TRUE;
		}
	
		// update counter
		if (try_n-- <= 0) break;

	} while(ctx->is_redirect);
	
	return TB_FALSE;
}

static tb_int_t tb_http_stream_read(tb_stream_t* st, tb_byte_t* data, tb_size_t size)
{
	tb_http_stream_t* hst = st;
	TB_ASSERT(data && size);
	if (hst && hst->hsocket != TPLAT_INVALID_HANDLE) 
		return tplat_socket_recv(hst->hsocket, data, size);
	else return -1;
}
static void tb_http_stream_close(tb_stream_t* st)
{
	tb_http_stream_t* hst = st;
	if (hst)
	{
		// free context
		if (st->pdata) tb_free(st->pdata);
		st->pdata = TB_NULL;

		// close socket
		if (hst->hsocket != TB_INVALID_HANDLE) 
			tplat_socket_close(hst->hsocket);
		hst->hsocket = TB_INVALID_HANDLE;
	}
}
static tb_size_t tb_http_stream_size(tb_stream_t* st)
{
	tb_http_stream_t* hst = st;
	if (hst && !(st->flag & TB_STREAM_FLAG_IS_ZLIB)) return hst->size;
	else return 0;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_stream_t* tb_stream_open_from_http(tb_http_stream_t* st, tb_char_t const* url, tb_stream_flag_t flag)
{
	TB_ASSERT(st && url);
	if (!st || !url) return TB_NULL;

	// init stream
	memset(st, 0, sizeof(tb_http_stream_t));
	st->base.flag = flag;
	st->base.head = st->base.data;
	st->base.size = 0;
	st->base.offset = 0;
	st->base.pdata = tb_malloc(sizeof(tb_http_context_t));
	if (!st->base.pdata) return TB_NULL;

	// init http stream
	st->base.read = tb_http_stream_read;
	st->base.close = tb_http_stream_close;
	st->base.ssize = tb_http_stream_size;

	// init url
	tb_string_init(&st->base.url);

	// init http context
	tb_http_context_t* ctx = st->base.pdata;
	memset(ctx, 0, sizeof(tb_http_context_t));
	ctx->is_stream = 1;

	// connect to host
	if (TB_FALSE == tb_http_connect(st, url)
		|| st->hsocket == TPLAT_INVALID_HANDLE) return TB_NULL;
	TB_HTTP_DBG("connect ok!");

#ifdef TB_CONFIG_ZLIB
	// is hzlib?
	if (flag & TB_STREAM_FLAG_IS_ZLIB)
	{
		st->base.hzlib = tb_zlib_create();
		if (st->base.hzlib == TB_INVALID_HANDLE) goto fail;
	}
#endif
	return ((tb_stream_t*)st);

fail:
	if (st->hsocket != TPLAT_INVALID_HANDLE) tplat_socket_close(st->hsocket);
	st->hsocket = TB_INVALID_HANDLE;
	return TB_NULL;
	// }
}
