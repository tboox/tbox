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
 * \http		http.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../network/network.h"
#include "../../string/string.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the http stream type
typedef struct __tb_hstream_t
{
	// the base
	tb_gstream_t 		base;

	// the http handle
	tb_handle_t 		http;

}tb_hstream_t;

/* /////////////////////////////////////////////////////////
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
static tb_long_t tb_hstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http && data, -1);
	tb_check_return_val(size, 0);

	// recv data
	return tb_http_aread(hst->http, data, size);
}
static tb_uint64_t tb_hstream_size(tb_gstream_t const* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, 0);

	return tb_http_status_document_size(hst->http);
}
static tb_bool_t tb_hstream_seek(tb_gstream_t* gst, tb_int64_t offset)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, TB_FALSE);

	// is seekable?
	if (!tb_http_status_isseeked(hst->http)) return TB_FALSE;

	// close it
	tb_http_bclose(hst->http);

	// set range
	tb_http_range_t range;
	range.bof = offset;
	range.eof = 0;
	tb_http_option_set_range(hst->http, &range);

	// reopen it
	if (!tb_http_bopen(hst->http)) return TB_FALSE;

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_hstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, TB_FALSE);

	switch (cmd)
	{
	case TB_GSTREAM_CMD_SET_URL:
		{
			tb_assert_and_check_return_val(arg1, TB_FALSE);
			return tb_http_option_set_url(hst->http, (tb_char_t const*)arg1);
		}
	case TB_GSTREAM_CMD_SET_TIMEOUT:
		{
			return tb_http_option_set_timeout(hst->http, (tb_size_t)arg1);
		}
	case TB_HSTREAM_CMD_SET_HOST:
		{
			tb_assert_and_check_return_val(arg1, TB_FALSE);
			return tb_http_option_set_host(hst->http, (tb_char_t const*)arg1);
		}
	case TB_HSTREAM_CMD_SET_PORT:
		{
			tb_assert_and_check_return_val(arg1, TB_FALSE);
			return tb_http_option_set_port(hst->http, (tb_size_t)arg1);
		}
	case TB_HSTREAM_CMD_SET_PATH:
		{
			tb_assert_and_check_return_val(arg1, TB_FALSE);
			return tb_http_option_set_path(hst->http, (tb_char_t const*)arg1);
		}
	case TB_HSTREAM_CMD_SET_METHOD:
		{
			return tb_http_option_set_method(hst->http, (tb_http_method_t)arg1);
		}
	case TB_HSTREAM_CMD_SET_KALIVE:
		{
			return tb_http_option_set_kalive(hst->http, (tb_http_method_t)arg1);
		}
	case TB_HSTREAM_CMD_SET_REDIRECT:
		{
			return tb_http_option_set_redirect(hst->http, (tb_uint8_t)arg1);
		}
	case TB_HSTREAM_CMD_SET_HEAD:
		{
			tb_assert_and_check_return_val(arg1, TB_FALSE);
			return tb_http_option_set_head(hst->http, (tb_char_t const*)arg1);
		}
	case TB_HSTREAM_CMD_SET_SSL:
		{
			return tb_http_option_set_ssl(hst->http, (tb_bool_t)arg1);
		}
	case TB_HSTREAM_CMD_GET_CODE:
		{
			tb_size_t* pcode = (tb_size_t*)arg1;
			tb_assert_and_check_return_val(pcode, TB_FALSE);
			*pcode = tb_http_status_code(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_GET_REDIRECT:
		{
			tb_size_t* predirect = (tb_size_t*)arg1;
			tb_assert_and_check_return_val(predirect, TB_FALSE);
			*predirect = tb_http_status_redirect(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_GET_COOKIES:
		{
			tb_cookies_t** pcookies = (tb_cookies_t**)arg1;
			tb_assert_and_check_return_val(pcookies, TB_FALSE);
			*pcookies = tb_http_option_get_cookies(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_ISCHUNKED:
		{
			tb_bool_t* pischunked = (tb_bool_t*)arg1;
			tb_assert_and_check_return_val(pischunked, TB_FALSE);
			*pischunked = tb_http_status_ischunked(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_ISREDIRECT:
		{
			tb_bool_t* pisredirect = (tb_bool_t*)arg1;
			tb_assert_and_check_return_val(pisredirect, TB_FALSE);
			*pisredirect = tb_http_status_isredirect(hst->http);
			return TB_TRUE;
		}	
	case TB_HSTREAM_CMD_SET_COOKIES:
		{
			return tb_http_option_set_cookies(hst->http, (tb_cookies_t*)arg1);
		}
	case TB_HSTREAM_CMD_SET_RANGE:
		{
			return tb_http_option_set_range(hst->http, (tb_http_range_t const*)arg1);
		}
	default:
		break;
	}
	return TB_FALSE;
}
static tb_bool_t tb_hstream_ioctl2(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1, tb_pointer_t arg2)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	tb_assert_and_check_return_val(hst && hst->http, TB_FALSE);

	switch (cmd)
	{
	case TB_HSTREAM_CMD_SET_HFUNC:
		{
			tb_assert_and_check_return_val(arg1, TB_FALSE);
			return tb_http_option_set_hfunc(hst->http, (tb_bool_t (*)(tb_char_t const* , tb_pointer_t ))arg1, arg2);
		}
	case TB_HSTREAM_CMD_SET_POST:
		{
			tb_assert_and_check_return_val(arg1 && arg2, TB_FALSE);
			return tb_http_option_set_post(hst->http, (tb_byte_t const*)arg1, (tb_size_t)arg2);
		}
	default:
		break;
	}
	return TB_FALSE;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_gstream_t* tb_gstream_init_http()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_hstream_t));
	tb_assert_and_check_return_val(gst, TB_NULL);

	// init stream
	tb_hstream_t* hst = (tb_hstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_HTTP;
	gst->aopen 	= tb_hstream_aopen;
	gst->aclose = tb_hstream_aclose;
	gst->aread 	= tb_hstream_aread;
	gst->seek 	= tb_hstream_seek;
	gst->size 	= tb_hstream_size;
	gst->ioctl1 = tb_hstream_ioctl1;
	gst->ioctl2 = tb_hstream_ioctl2;
	gst->free 	= tb_hstream_free;
	hst->http 	= tb_http_init(TB_NULL);
	tb_assert_and_check_goto(hst->http, fail);

	return gst;

fail:
	if (gst) tb_free(gst);
	return TB_NULL;
}

tb_gstream_t* tb_gstream_init_from_http(tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl)
{
	tb_assert_and_check_return_val(host && port && path, TB_NULL);

	// init http stream
	tb_gstream_t* gst = tb_gstream_init_http();
	tb_assert_and_check_return_val(gst, TB_NULL);

	// ioctl
	if (!tb_gstream_ioctl1(gst, TB_HSTREAM_CMD_SET_HOST, host)) goto fail;
	if (!tb_gstream_ioctl1(gst, TB_HSTREAM_CMD_SET_PORT, port)) goto fail;
	if (!tb_gstream_ioctl1(gst, TB_HSTREAM_CMD_SET_PATH, path)) goto fail;
	if (!tb_gstream_ioctl1(gst, TB_HSTREAM_CMD_SET_SSL, bssl)) goto fail;
	
	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return TB_NULL;
}
