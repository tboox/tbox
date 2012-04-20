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
 * \http		http.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../aio/aio.h"
#include "../../string/string.h"
#include "../../network/network.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the http stream type
typedef struct __tb_hstream_t
{
	// the base
	tb_gstream_t 		base;

	// the http 
	tb_handle_t 		http;

}tb_hstream_t;

/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_hstream_t* tb_hstream_cast(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_HTTP, TB_NULL);
	return (tb_hstream_t*)gst;
}
static tb_long_t tb_hstream_aopen(tb_gstream_t* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, -1);

	// open it
	return tb_http_aopen(hst->http);
}
static tb_long_t tb_hstream_aclose(tb_gstream_t* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, -1);

	// close it
	return tb_http_aclose(hst->http);
}
static tb_void_t tb_hstream_free(tb_gstream_t* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	if (hst && hst->http) tb_http_exit(hst->http);
}
static tb_long_t tb_hstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// recv data
	return tb_http_aread(hst->http, data, size);
}
static tb_hize_t tb_hstream_size(tb_gstream_t const* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, 0);

	// status
	tb_http_status_t const* status = tb_http_status(hst->http);
	tb_assert_and_check_return_val(status, 0);

	// document_size
	return status->document_size;
}
static tb_long_t tb_hstream_aseek(tb_gstream_t* gst, tb_hize_t offset)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, -1);

	// status
	tb_http_status_t const* status = tb_http_status(hst->http);
	tb_assert_and_check_return_val(status, -1);

	// be able to seek?
	tb_check_return_val(status->bseeked, -1);

	// seek
	return tb_http_aseek(hst->http, offset);
}
static tb_long_t tb_hstream_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, -1);

	return tb_http_wait(hst->http, etype, timeout);
}
static tb_bool_t tb_hstream_ctrl(tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, TB_FALSE);

	switch (cmd)
	{
	case TB_GSTREAM_CMD_SET_URL:
		{
			// url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(url, TB_FALSE);
			
			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// set url
			if (tb_url_set(&option->url, url)) return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_GET_URL:
		{
			// purl
			tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(purl, TB_FALSE);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// get url
			tb_char_t const* url = tb_url_get(&option->url);
			tb_assert_and_check_return_val(url, TB_FALSE);

			// ok
			*purl = url;
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_SET_HOST:
		{
			// host
			tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(host, TB_FALSE);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// set host
			tb_url_host_set(&option->url, host);
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_GET_HOST:
		{
			// phost
			tb_char_t const** phost = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(phost, TB_FALSE); 

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// get host
			tb_char_t const* host = tb_url_host_get(&option->url);
			tb_assert_and_check_return_val(host, TB_FALSE);

			// ok
			*phost = host;
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_SET_PORT:
		{
			// port
			tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
			tb_assert_and_check_return_val(port, TB_FALSE);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// set port
			tb_url_port_set(&option->url, port);
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_GET_PORT:
		{
			// pport
			tb_size_t* pport = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pport, TB_FALSE);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// get port
			*pport = tb_url_port_get(&option->url);
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_SET_PATH:
		{
			// path
			tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(path, TB_FALSE);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// set path
			tb_url_path_set(&option->url, path);
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_GET_PATH:
		{
			// ppath
			tb_char_t const** ppath = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(ppath, TB_FALSE);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// get path
			tb_char_t const* path = tb_url_path_get(&option->url);
			tb_assert_and_check_return_val(path, TB_FALSE);

			// ok
			*ppath = path;
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_SET_SSL:
		{
			// bssl
			tb_bool_t bssl = (tb_bool_t)tb_va_arg(args, tb_bool_t);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// set ssl
			tb_url_ssl_set(&option->url, bssl);
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_GET_SSL:
		{
			// pssl
			tb_bool_t* pssl = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
			tb_assert_and_check_return_val(pssl, TB_FALSE);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// get ssl
			*pssl = tb_url_ssl_get(&option->url);
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_SET_TIMEOUT:
		{
			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// set timeout
			option->timeout = (tb_size_t)tb_va_arg(args, tb_size_t);
			return TB_TRUE;
		}
		break;
	case TB_GSTREAM_CMD_GET_TIMEOUT:
		{
			// ptimeout
			tb_size_t* ptimeout = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(ptimeout, TB_FALSE);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, TB_FALSE);

			// get timeout
			*ptimeout = option->timeout;
			return TB_TRUE;
		}
		break;
	case TB_HSTREAM_CMD_GET_OPTION:
		{
			tb_http_option_t** poption = (tb_http_option_t**)tb_va_arg(args, tb_http_option_t**);
			tb_assert_and_check_return_val(poption, TB_FALSE);
			*poption = tb_http_option(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_GET_STATUS:
		{
			tb_http_status_t const** pstatus = (tb_http_status_t const**)tb_va_arg(args, tb_http_status_t const**);
			tb_assert_and_check_return_val(pstatus, TB_FALSE);
			*pstatus = tb_http_status(hst->http);
			return TB_TRUE;
		}
	default:
		break;
	}
	return TB_FALSE;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_t* tb_gstream_init_http()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_malloc0(sizeof(tb_hstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init stream
	tb_hstream_t* hst = (tb_hstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_HTTP;
	gst->aopen 	= tb_hstream_aopen;
	gst->aclose = tb_hstream_aclose;
	gst->aread 	= tb_hstream_aread;
	gst->aseek 	= tb_hstream_aseek;
	gst->size 	= tb_hstream_size;
	gst->wait 	= tb_hstream_wait;
	gst->ctrl 	= tb_hstream_ctrl;
	gst->free 	= tb_hstream_free;
	hst->http 	= tb_http_init();
	tb_assert_and_check_goto(hst->http, fail);

	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
}

tb_gstream_t* tb_gstream_init_from_http(tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl)
{
	tb_assert_and_check_return_val(host && port && path, TB_NULL);

	// init http stream
	tb_gstream_t* gst = tb_gstream_init_http();
	tb_assert_and_check_return_val(gst, TB_NULL);

	// ioctl
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_HOST, host)) goto fail;
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_PORT, port)) goto fail;
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_PATH, path)) goto fail;
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_SSL, bssl)) goto fail;
	
	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
}
