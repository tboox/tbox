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

	// the http offset
	tb_size_t 			offset;

}tb_hstream_t;

/* /////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_hstream_t* tb_hstream_cast(tb_gstream_t* gst)
{
	TB_ASSERT_RETURN_VAL(gst && gst->type == TB_GSTREAM_TYPE_HTTP, TB_NULL);
	return (tb_hstream_t*)gst;
}
static tb_bool_t tb_hstream_open(tb_gstream_t* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	TB_ASSERT_RETURN_VAL(hst && hst->http != TB_INVALID_HANDLE, TB_FALSE);

	// init offset
	hst->offset = 0;

	// open it
	return tb_http_open(hst->http);
}
static void tb_hstream_close(tb_gstream_t* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	if (hst)
	{
		if (hst->http != TB_INVALID_HANDLE)
			tb_http_close(hst->http);
	}
}
static void tb_hstream_free(tb_gstream_t* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	if (hst)
	{
		if (hst->http != TB_INVALID_HANDLE)
			tb_http_destroy(hst->http);
	}
}
static tb_int_t tb_hstream_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	TB_ASSERT_RETURN_VAL(hst && hst->http != TB_INVALID_HANDLE && data, -1);
	TB_IF_FAIL_RETURN_VAL(size, 0);

	// recv data
	tb_int_t ret = tb_http_read(hst->http, data, size);

	// update offset
	if (ret > 0) hst->offset += ret;
	return ret;
}
static tb_int_t tb_hstream_bread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	TB_ASSERT_RETURN_VAL(hst && hst->http != TB_INVALID_HANDLE && data, -1);
	TB_IF_FAIL_RETURN_VAL(size, 0);

	// recv data
	tb_int_t ret = tb_http_bread(hst->http, data, size);

	// update offset
	if (ret > 0) hst->offset += ret;
	return ret;
}
static tb_size_t tb_hstream_size(tb_gstream_t const* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	TB_ASSERT_RETURN_VAL(hst && hst->http != TB_INVALID_HANDLE, 0);

	return tb_http_status_content_size(hst->http);
}
static tb_size_t tb_hstream_offset(tb_gstream_t* gst)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	TB_ASSERT_RETURN_VAL(hst && hst->http != TB_INVALID_HANDLE, 0);
	return hst->offset;
}
static tb_bool_t tb_hstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, void* arg1)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	TB_ASSERT_RETURN_VAL(hst && hst->http != TB_INVALID_HANDLE, TB_FALSE);

	switch (cmd)
	{
	case TB_GSTREAM_CMD_SET_URL:
		{
			TB_ASSERT_RETURN_VAL(arg1, TB_FALSE);
			return tb_http_option_set_url(hst->http, (tb_char_t const*)arg1);
		}
	case TB_GSTREAM_CMD_GET_URL:
		{
			tb_char_t const** purl = (tb_char_t const**)arg1;
			TB_ASSERT_RETURN_VAL(purl, TB_FALSE);
			*purl = tb_http_option_get_url(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_SET_METHOD:
		{
			return tb_http_option_set_method(hst->http, (tb_http_method_t)arg1);
		}
	case TB_HSTREAM_CMD_SET_REDIRECT:
		{
			return tb_http_option_set_redirect(hst->http, (tb_uint8_t)arg1);
		}
	case TB_HSTREAM_CMD_SET_HEAD:
		{
			TB_ASSERT_RETURN_VAL(arg1, TB_FALSE);
			return tb_http_option_set_head(hst->http, (tb_char_t const*)arg1);
		}
	case TB_HSTREAM_CMD_GET_CODE:
		{
			tb_size_t* pcode = (tb_size_t*)arg1;
			TB_ASSERT_RETURN_VAL(pcode, TB_FALSE);
			*pcode = tb_http_status_code(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_GET_REDIRECT:
		{
			tb_size_t* predirect = (tb_size_t*)arg1;
			TB_ASSERT_RETURN_VAL(predirect, TB_FALSE);
			*predirect = tb_http_status_redirect(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_GET_COOKIES:
		{
			tb_cookies_t** pcookies = (tb_cookies_t**)arg1;
			TB_ASSERT_RETURN_VAL(pcookies, TB_FALSE);
			*pcookies = tb_http_option_get_cookies(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_ISCHUNKED:
		{
			tb_bool_t* pischunked = (tb_bool_t*)arg1;
			TB_ASSERT_RETURN_VAL(pischunked, TB_FALSE);
			*pischunked = tb_http_status_ischunked(hst->http);
			return TB_TRUE;
		}
	case TB_HSTREAM_CMD_ISREDIRECT:
		{
			tb_bool_t* pisredirect = (tb_bool_t*)arg1;
			TB_ASSERT_RETURN_VAL(pisredirect, TB_FALSE);
			*pisredirect = tb_http_status_isredirect(hst->http);
			return TB_TRUE;
		}	
	case TB_HSTREAM_CMD_SET_COOKIES:
		{
			return tb_http_option_set_cookies(hst->http, (tb_cookies_t*)arg1);
		}
	case TB_HSTREAM_CMD_SET_SOPEN_FUNC:
		{
			TB_ASSERT_RETURN_VAL(arg1, TB_FALSE);
			return tb_http_option_set_sopen_func(hst->http, (tb_handle_t (*)(tb_char_t const*))arg1);
		}
	case TB_HSTREAM_CMD_SET_SCLOSE_FUNC:
		{
			TB_ASSERT_RETURN_VAL(arg1, TB_FALSE);
			return tb_http_option_set_sclose_func(hst->http, (void (*)(tb_handle_t ))arg1);
		}
	case TB_HSTREAM_CMD_SET_SREAD_FUNC:
		{
			TB_ASSERT_RETURN_VAL(arg1, TB_FALSE);
			return tb_http_option_set_sread_func(hst->http, (tb_int_t (*)(tb_handle_t, tb_byte_t* , tb_size_t ))arg1);
		}	
	case TB_HSTREAM_CMD_SET_SWRITE_FUNC:
		{
			TB_ASSERT_RETURN_VAL(arg1, TB_FALSE);
			return tb_http_option_set_swrite_func(hst->http, (tb_int_t (*)(tb_handle_t, tb_byte_t const* , tb_size_t ))arg1);
		}
	default:
		break;
	}
	return TB_FALSE;
}
static tb_bool_t tb_hstream_ioctl2(tb_gstream_t* gst, tb_size_t cmd, void* arg1, void* arg2)
{
	tb_hstream_t* hst = tb_hstream_cast(gst);
	TB_ASSERT_RETURN_VAL(hst && hst->http != TB_INVALID_HANDLE, TB_FALSE);

	switch (cmd)
	{
	case TB_HSTREAM_CMD_SET_HEAD_FUNC:
		{
			TB_ASSERT_RETURN_VAL(arg1, TB_FALSE);
			return tb_http_option_set_head_func(hst->http, (tb_bool_t (*)(tb_char_t const* , void* ))arg1, arg2);
		}
	case TB_HSTREAM_CMD_SET_POST:
		{
			TB_ASSERT_RETURN_VAL(arg1 && arg2, TB_FALSE);
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

tb_gstream_t* tb_gstream_create_http()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_hstream_t));
	TB_ASSERT_RETURN_VAL(gst, TB_NULL);

	// init stream
	tb_hstream_t* hst = (tb_hstream_t*)gst;
	gst->type 	= TB_GSTREAM_TYPE_HTTP;
	gst->open 	= tb_hstream_open;
	gst->close 	= tb_hstream_close;
	gst->free 	= tb_hstream_free;
	gst->read 	= tb_hstream_read;
	gst->bread 	= tb_hstream_bread;
	gst->size 	= tb_hstream_size;
	gst->offset = tb_hstream_offset;
	gst->ioctl1 = tb_hstream_ioctl1;
	gst->ioctl2 = tb_hstream_ioctl2;
	hst->http 	= tb_http_create(TB_NULL);
	TB_ASSERT_GOTO(hst->http != TB_INVALID_HANDLE, fail);

	return gst;

fail:
	if (gst) tb_free(gst);
	return TB_NULL;
}

