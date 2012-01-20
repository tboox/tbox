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
 * \file		url.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "url.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* ////////////////////////////////////////////////////////////////////////
 * helper
 */
static tb_void_t tb_url_sync(tb_url_t* url)
{
	switch (url->poto)
	{
	case TB_URL_PROTO_FILE:
		{
			// check
			tb_check_return(tb_pstring_size(&url->path));

			// add protocol
			if (url->bssl) tb_pstring_cstrncpy(&url->data, "files://", 8);
			else tb_pstring_cstrncpy(&url->data, "file://", 7);

			// add path
			tb_pstring_strcat(&url->data, &url->path);
		}
		break;
	case TB_URL_PROTO_SOCK:
	case TB_URL_PROTO_HTTP:
		{
			// check
			tb_check_return(url->port && tb_pstring_size(&url->host));

			// add protocol
			if (url->poto == TB_URL_PROTO_HTTP) tb_pstring_cstrcpy(&url->data, "http");
			else tb_pstring_cstrcpy(&url->data, "sock");

			// add ssl
			if (url->bssl) tb_pstring_chrcat(&url->data, 's');

			// add ://
			tb_pstring_cstrncat(&url->data, "://", 3);

			// add host
			tb_pstring_strcat(&url->data, &url->host);

			// add port
			if (url->poto != TB_URL_PROTO_HTTP || url->port != 80) 
				tb_pstring_cstrfcat(&url->data, ":%u", url->port);

			// add path
			if (tb_pstring_size(&url->path)) 
				tb_pstring_strcat(&url->data, &url->path);
		}
		break;
	default:
		break;
	}
}

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */

// init & exit
tb_bool_t tb_url_init(tb_url_t* url)
{
	tb_assert_and_check_return_val(url, TB_FALSE);
	url->poto = TB_URL_PROTO_NULL;
	url->port = 0;
	url->bssl = 0;
	if (!tb_pstring_init(&url->data)) return TB_FALSE;
	if (!tb_pstring_init(&url->host)) return TB_FALSE;
	if (!tb_pstring_init(&url->path)) return TB_FALSE;
	return TB_TRUE;
}
tb_void_t tb_url_exit(tb_url_t* url)
{
	if (url)
	{
		tb_pstring_exit(&url->data);
		tb_pstring_exit(&url->host);
		tb_pstring_exit(&url->path);
	}
}
tb_char_t const* tb_url_cstr(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);
	return tb_pstring_cstr(&url->data);
}
tb_size_t tb_url_size(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, 0);
	return tb_pstring_size(&url->data);
}
tb_void_t tb_url_clear(tb_url_t* url)
{
	tb_assert_and_check_return(url);

	url->poto = TB_URL_PROTO_NULL;
	url->port = 0;
	url->bssl = 0;
	tb_pstring_clear(&url->data);
	tb_pstring_clear(&url->host);
	tb_pstring_clear(&url->path);
}
tb_char_t const* tb_url_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);
	return tb_pstring_cstr(&url->data);
}
tb_bool_t tb_url_set(tb_url_t* url, tb_char_t const* u)
{
	tb_trace_noimpl();
	return TB_FALSE;
}
tb_bool_t tb_url_ssl_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_FALSE);
	return url->bssl? TB_TRUE : TB_FALSE;
}
tb_void_t tb_url_ssl_set(tb_url_t* url, tb_bool_t bssl)
{
	tb_assert_and_check_return(url);

	tb_size_t b = bssl? 1 : 0;
	if (url->bssl != b)
	{
		url->bssl = b;
		tb_url_sync(url);
	}
}
tb_size_t tb_url_poto_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_URL_PROTO_NULL);
	return url->poto;
}
tb_void_t tb_url_poto_set(tb_url_t* url, tb_size_t poto)
{
	tb_assert_and_check_return(url);
	if (url->poto != poto)
	{
		url->poto = poto;
		tb_url_sync(url);
	}
}
tb_size_t tb_url_port_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, 0);
	return url->port;
}
tb_void_t tb_url_port_set(tb_url_t* url, tb_size_t port)
{
	tb_assert_and_check_return(url);
	if (url->port != port)
	{
		url->port = port;
		tb_url_sync(url);
	}
}
tb_char_t const* tb_url_host_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);
	return tb_pstring_cstr(&url->host);
}
tb_void_t tb_url_host_set(tb_url_t* url, tb_char_t const* host)
{
	tb_assert_and_check_return(url);
	if (host) tb_pstring_cstrcpy(&url->host, host);
	else tb_pstring_clear(&url->host);
	tb_url_sync(url);
}
tb_char_t const* tb_url_path_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);
	return tb_pstring_cstr(&url->path);
}
tb_void_t tb_url_path_set(tb_url_t* url, tb_char_t const* path)
{
	tb_assert_and_check_return(url);
	if (path) 
	{
		if (path[0] != '/') tb_pstring_chrcat(&url->path, '/');
		tb_pstring_cstrcat(&url->path, path);
	}
	else tb_pstring_clear(&url->path);
	tb_url_sync(url);
}
