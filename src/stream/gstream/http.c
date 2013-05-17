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
	tb_assert_and_check_return_val(gst && gst->type == TB_GSTREAM_TYPE_HTTP, tb_null);
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
	tb_assert_and_check_return_val(hst && hst->http, tb_false);

	switch (cmd)
	{
	case TB_GSTREAM_CMD_SET_URL:
		{
			// url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(url, tb_false);
			
			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// set url
			if (tb_url_set(&option->url, url)) return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_GET_URL:
		{
			// purl
			tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(purl, tb_false);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// get url
			tb_char_t const* url = tb_url_get(&option->url);
			tb_assert_and_check_return_val(url, tb_false);

			// ok
			*purl = url;
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_SET_HOST:
		{
			// host
			tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(host, tb_false);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// set host
			tb_url_host_set(&option->url, host);
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_GET_HOST:
		{
			// phost
			tb_char_t const** phost = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(phost, tb_false); 

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// get host
			tb_char_t const* host = tb_url_host_get(&option->url);
			tb_assert_and_check_return_val(host, tb_false);

			// ok
			*phost = host;
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_SET_PORT:
		{
			// port
			tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
			tb_assert_and_check_return_val(port, tb_false);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// set port
			tb_url_port_set(&option->url, port);
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_GET_PORT:
		{
			// pport
			tb_size_t* pport = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pport, tb_false);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// get port
			*pport = tb_url_port_get(&option->url);
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_SET_PATH:
		{
			// path
			tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(path, tb_false);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// set path
			tb_url_path_set(&option->url, path);
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_GET_PATH:
		{
			// ppath
			tb_char_t const** ppath = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(ppath, tb_false);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// get path
			tb_char_t const* path = tb_url_path_get(&option->url);
			tb_assert_and_check_return_val(path, tb_false);

			// ok
			*ppath = path;
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_SET_SSL:
		{
			// bssl
			tb_bool_t bssl = (tb_bool_t)tb_va_arg(args, tb_bool_t);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// set ssl
			tb_url_ssl_set(&option->url, bssl);
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_GET_SSL:
		{
			// pssl
			tb_bool_t* pssl = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
			tb_assert_and_check_return_val(pssl, tb_false);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// get ssl
			*pssl = tb_url_ssl_get(&option->url);
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_SET_SFUNC:
		{
			// sfunc
			tb_http_sfunc_t const* sfunc = (tb_http_sfunc_t const*)tb_va_arg(args, tb_http_sfunc_t*);
			tb_assert_static(sizeof(tb_http_sfunc_t) == sizeof(tb_gstream_sfunc_t));

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// set sfunc
			if (sfunc) option->sfunc = *sfunc;
			else tb_memset(&option->sfunc, 0, sizeof(tb_http_sfunc_t));

			// ok
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_GET_SFUNC:
		{
			// sfunc
			tb_http_sfunc_t* sfunc = (tb_http_sfunc_t*)tb_va_arg(args, tb_http_sfunc_t*);
			tb_assert_static(sizeof(tb_http_sfunc_t) == sizeof(tb_gstream_sfunc_t));
			tb_assert_and_check_return_val(sfunc, tb_false);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// get sfunc
			*sfunc = option->sfunc;
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_SET_TIMEOUT:
		{
			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// set timeout
			option->timeout = (tb_size_t)tb_va_arg(args, tb_size_t);
			return tb_true;
		}
		break;
	case TB_GSTREAM_CMD_GET_TIMEOUT:
		{
			// ptimeout
			tb_size_t* ptimeout = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(ptimeout, tb_false);

			// option
			tb_http_option_t* option = tb_http_option(hst->http);
			tb_assert_and_check_return_val(option, tb_false);

			// get timeout
			*ptimeout = option->timeout;
			return tb_true;
		}
		break;
	case TB_HSTREAM_CMD_GET_OPTION:
		{
			tb_http_option_t** poption = (tb_http_option_t**)tb_va_arg(args, tb_http_option_t**);
			tb_assert_and_check_return_val(poption, tb_false);
			*poption = tb_http_option(hst->http);
			return tb_true;
		}
	case TB_HSTREAM_CMD_GET_STATUS:
		{
			tb_http_status_t const** pstatus = (tb_http_status_t const**)tb_va_arg(args, tb_http_status_t const**);
			tb_assert_and_check_return_val(pstatus, tb_false);
			*pstatus = tb_http_status(hst->http);
			return tb_true;
		}
	default:
		break;
	}
	return tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_t* tb_gstream_init_http()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_malloc0(sizeof(tb_hstream_t));
	tb_assert_and_check_return_val(gst, tb_null);

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
	return tb_null;
}

tb_gstream_t* tb_gstream_init_from_http(tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl)
{
	tb_assert_and_check_return_val(host && port && path, tb_null);

	// init http stream
	tb_gstream_t* gst = tb_gstream_init_http();
	tb_assert_and_check_return_val(gst, tb_null);

	// ioctl
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_HOST, host)) goto fail;
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_PORT, port)) goto fail;
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_PATH, path)) goto fail;
	if (!tb_gstream_ctrl(gst, TB_GSTREAM_CMD_SET_SSL, bssl)) goto fail;
	
	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return tb_null;
}
