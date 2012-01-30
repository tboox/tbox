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

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "url.h"
#include "http.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

// init & exit
tb_bool_t tb_url_init(tb_url_t* url)
{
	tb_assert_and_check_return_val(url, TB_FALSE);
	url->poto = TB_URL_PROTO_NULL;
	url->port = 0;
	url->bssl = 0;
	if (!tb_sstring_init(&url->host, url->data, TB_URL_HOST_MAX)) return TB_FALSE;
	if (!tb_sstring_init(&url->path, url->data + TB_URL_HOST_MAX, TB_URL_PATH_MAX)) return TB_FALSE;
	if (!tb_pstring_init(&url->args)) return TB_FALSE;
	if (!tb_pstring_init(&url->urls)) return TB_FALSE;
	return TB_TRUE;
}
tb_void_t tb_url_exit(tb_url_t* url)
{
	if (url)
	{
		tb_sstring_exit(&url->host);
		tb_sstring_exit(&url->path);
		tb_pstring_exit(&url->args);
		tb_pstring_exit(&url->urls);
	}
}
tb_void_t tb_url_clear(tb_url_t* url)
{
	tb_assert_and_check_return(url);

	url->poto = TB_URL_PROTO_NULL;
	url->port = 0;
	url->bssl = 0;
	tb_sstring_clear(&url->host);
	tb_sstring_clear(&url->path);
	tb_pstring_clear(&url->args);
	tb_pstring_clear(&url->urls);
}
tb_char_t const* tb_url_get(tb_url_t* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);

	// init 
	tb_pstring_clear(&url->urls);

	// make
	switch (url->poto)
	{
	case TB_URL_PROTO_FILE:
		{
			// check
			tb_check_return(tb_sstring_size(&url->path));

			// add protocol
			if (url->bssl) tb_pstring_cstrncpy(&url->urls, "files://", 8);
			else tb_pstring_cstrncpy(&url->urls, "file://", 7);

			// add path
			tb_pstring_cstrncat(&url->urls, tb_sstring_cstr(&url->path), tb_sstring_size(&url->path));
		}
		break;
	case TB_URL_PROTO_SOCK:
	case TB_URL_PROTO_HTTP:
		{
			// check
			tb_check_return(url->port && tb_sstring_size(&url->host));

			// add protocol
			if (url->poto == TB_URL_PROTO_HTTP) tb_pstring_cstrcpy(&url->urls, "http");
			else tb_pstring_cstrcpy(&url->urls, "sock");

			// add ssl
			if (url->bssl) tb_pstring_chrcat(&url->urls, 's');

			// add ://
			tb_pstring_cstrncat(&url->urls, "://", 3);

			// add host
			tb_pstring_cstrncat(&url->urls, tb_sstring_cstr(&url->host), tb_sstring_size(&url->host));

			// add port
			if ( 	(url->poto != TB_URL_PROTO_HTTP)
				|| 	(url->bssl && url->port != TB_HTTPS_PORT_DEFAULT) 
				|| 	(!url->bssl && url->port != TB_HTTP_PORT_DEFAULT)) 
				tb_pstring_cstrfcat(&url->urls, ":%u", url->port);

			// add path
			if (tb_sstring_size(&url->path)) 
				tb_pstring_cstrncat(&url->urls, tb_sstring_cstr(&url->path), tb_sstring_size(&url->path));

			// add args
			if (tb_pstring_size(&url->args)) 
			{
				tb_pstring_chrcat(&url->urls, '?');
				tb_pstring_strcat(&url->urls, &url->args);
			}
		}
		break;
	default:
		break;
	}

	// ok?
	return tb_pstring_size(&url->urls)? tb_pstring_cstr(&url->urls) : TB_NULL;
}
tb_bool_t tb_url_set(tb_url_t* url, tb_char_t const* u)
{
	tb_assert_and_check_return_val(url && u, TB_FALSE);
	
	// init
	tb_url_clear(url);

	// parse proto
	tb_char_t const* p = u;
	if (!tb_strnicmp(p, "http://", 7)) 
	{
		url->poto = TB_URL_PROTO_HTTP;
		url->bssl = 0;
		p += 7;
	}
	else if ((*p == '/') || (!tb_strnicmp(p, "file://", 7))) 
	{
		url->poto = TB_URL_PROTO_FILE;
		url->bssl = 0;
		if (*p != '/') p += 7;
	}
	else if (!tb_strnicmp(p, "sock://", 7))
	{
		url->poto = TB_URL_PROTO_SOCK;
		url->bssl = 0;
		p += 7;
	}
	else if (!tb_strnicmp(p, "https://", 8))
	{
		url->poto = TB_URL_PROTO_HTTP;
		url->bssl = 1;
		p += 8;
	}
	else if (!tb_strnicmp(p, "files://", 8))
	{
		url->poto = TB_URL_PROTO_FILE;
		url->bssl = 1;
		p += 8;
	}
	else if (!tb_strnicmp(p, "socks://", 8))
	{
		url->poto = TB_URL_PROTO_SOCK;
		url->bssl = 1;
		p += 8;
	}
	else goto fail;

	// end?
	tb_assert_and_check_goto(*p, fail);

	// parse host and port for http or sock
	if (url->poto == TB_URL_PROTO_HTTP || url->poto == TB_URL_PROTO_SOCK)
	{
		// parse host
		while (*p && *p != '/' && *p != ':') tb_sstring_chrcat(&url->host, *p++);
	
		// parse port
		if (*p == ':')
		{
			tb_char_t 	port[12] = {0};
			tb_char_t* 	pb = port;
			tb_char_t* 	pe = port + 12;
			for (p++; pb < pe && *p && *p != '/' && *p != '?' && *p != '&' && *p != '='; ) *pb++ = *p++;
			url->port = tb_s10tou32(port);
			tb_assert_and_check_goto(url->port, fail);
		}
		else if (url->poto == TB_URL_PROTO_HTTP) url->port = url->bssl? TB_HTTPS_PORT_DEFAULT : TB_HTTP_PORT_DEFAULT;
		else goto fail;

		// skip '/'
		if (*p == '/') p++;
	}

	// parse path
	if (*p != '/') tb_sstring_chrcat(&url->path, '/');
	while (*p && *p != '?' && *p != '&' && *p != '=') tb_sstring_chrcat(&url->path, *p++);

	// parse args
	while (*p && (*p == '?' || *p == '=')) p++;
	if (*p) tb_pstring_cstrcpy(&url->args, p);

	// ok
	return TB_TRUE;

fail:
	tb_url_clear(url);
	return TB_FALSE;
}
tb_void_t tb_url_cpy(tb_url_t* url, tb_url_t const* u)
{
	tb_assert_and_check_return(url && u);
	url->poto = u->poto;
	url->port = u->port;
	url->bssl = u->bssl;
	tb_sstring_strcpy(&url->host, &u->host);
	tb_sstring_strcpy(&url->path, &u->path);
	tb_pstring_strcpy(&url->args, &u->args);
	tb_pstring_strcpy(&url->urls, &u->urls);
}
tb_bool_t tb_url_ssl_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_FALSE);
	return url->bssl? TB_TRUE : TB_FALSE;
}
tb_void_t tb_url_ssl_set(tb_url_t* url, tb_bool_t bssl)
{
	tb_assert_and_check_return(url);
	url->bssl = bssl? 1 : 0;
}
tb_size_t tb_url_poto_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_URL_PROTO_NULL);
	return url->poto;
}
tb_void_t tb_url_poto_set(tb_url_t* url, tb_size_t poto)
{
	tb_assert_and_check_return(url);
	url->poto = poto;
}
tb_size_t tb_url_port_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, 0);
	return url->port;
}
tb_void_t tb_url_port_set(tb_url_t* url, tb_size_t port)
{
	tb_assert_and_check_return(url);
	url->port = port;
}
tb_char_t const* tb_url_host_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);
	return tb_sstring_size(&url->host)? tb_sstring_cstr(&url->host) : TB_NULL;
}
tb_void_t tb_url_host_set(tb_url_t* url, tb_char_t const* host)
{
	tb_assert_and_check_return(url);
	if (host) tb_sstring_cstrcpy(&url->host, host);
	else tb_sstring_clear(&url->host);
}
tb_char_t const* tb_url_path_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);
	return tb_sstring_size(&url->path)? tb_sstring_cstr(&url->path) : TB_NULL;
}
tb_void_t tb_url_path_set(tb_url_t* url, tb_char_t const* path)
{
	tb_assert_and_check_return(url);
	tb_sstring_clear(&url->path);
	if (path) 
	{
		if (path[0] != '/') tb_sstring_chrcat(&url->path, '/');
		tb_sstring_cstrcat(&url->path, path);
	}
}
tb_char_t const* tb_url_args_get(tb_url_t const* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);
	return tb_pstring_size(&url->args)? tb_pstring_cstr(&url->args) : TB_NULL;
}
tb_void_t tb_url_args_set(tb_url_t* url, tb_char_t const* args)
{
	tb_assert_and_check_return(url);
	tb_pstring_clear(&url->args);
	if (args) 
	{
		tb_char_t const* p = args;
		while (*p && (*p == '?' || *p == '=')) p++;
		if (*p) tb_pstring_cstrcpy(&url->args, p);
	}
}

