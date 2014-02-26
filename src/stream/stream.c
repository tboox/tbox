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
 * @file		stream.c
 * @defgroup 	stream
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stream.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_stream_mode(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, TB_STREAM_MODE_NONE);

	// the mode
	return stream->mode;
}
tb_size_t tb_stream_type(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, TB_STREAM_TYPE_NONE);

	// the type
	return stream->type;
}
tb_bool_t tb_stream_bopened(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, tb_false);

	// is opened?
	return tb_atomic_get(&stream->bopened)? tb_true : tb_false;
}
tb_long_t tb_stream_timeout(tb_handle_t handle)
{
	return -1;
}
tb_char_t const* tb_stream_state_cstr(tb_size_t state)
{
	// done
	switch (state)
	{
	case TB_STREAM_STATE_OK: 					return "ok";
	case TB_STREAM_STATE_CLOSED: 				return "closed";
	case TB_STREAM_STATE_KILLED: 				return "killed";
	case TB_STREAM_STATE_PAUSED: 				return "paused";
	case TB_STREAM_STATE_WAIT_FAILED: 			return "wait failed";
	case TB_STREAM_STATE_NOT_SUPPORTED: 		return "not supported";
	case TB_STREAM_STATE_UNKNOWN_ERROR: 		return "unknown error";
	case TB_STREAM_SOCK_STATE_DNS_FAILED: 		return "sock: dns: failed";
	case TB_STREAM_SOCK_STATE_CONNECT_FAILED: 	return "sock: connect: failed";
	case TB_STREAM_SOCK_STATE_CONNECT_TIMEOUT: 	return "sock: connect: timeout";
	case TB_STREAM_SOCK_STATE_SSL_FAILED: 		return "sock: ssl: failed";
	case TB_STREAM_SOCK_STATE_RECV_TIMEOUT: 	return "sock: recv: timeout";
	case TB_STREAM_SOCK_STATE_SEND_TIMEOUT: 	return "sock: send: timeout";
	case TB_STREAM_HTTP_STATE_RESPONSE_204: 	return "http: response: 204";
	case TB_STREAM_HTTP_STATE_RESPONSE_300: 	return "http: response: 300";
	case TB_STREAM_HTTP_STATE_RESPONSE_301: 	return "http: response: 301";
	case TB_STREAM_HTTP_STATE_RESPONSE_302: 	return "http: response: 302";
	case TB_STREAM_HTTP_STATE_RESPONSE_303: 	return "http: response: 303";
	case TB_STREAM_HTTP_STATE_RESPONSE_304: 	return "http: http: response: 304";
	case TB_STREAM_HTTP_STATE_RESPONSE_400: 	return "http: response: 400";
	case TB_STREAM_HTTP_STATE_RESPONSE_401: 	return "http: response: 401";
	case TB_STREAM_HTTP_STATE_RESPONSE_402: 	return "http: response: 402";
	case TB_STREAM_HTTP_STATE_RESPONSE_403: 	return "http: response: 403";
	case TB_STREAM_HTTP_STATE_RESPONSE_404: 	return "http: response: 404";
	case TB_STREAM_HTTP_STATE_RESPONSE_405: 	return "http: response: 405";
	case TB_STREAM_HTTP_STATE_RESPONSE_406: 	return "http: response: 406";
	case TB_STREAM_HTTP_STATE_RESPONSE_407: 	return "http: response: 407";
	case TB_STREAM_HTTP_STATE_RESPONSE_408: 	return "http: response: 408";
	case TB_STREAM_HTTP_STATE_RESPONSE_409: 	return "http: response: 409";
	case TB_STREAM_HTTP_STATE_RESPONSE_410: 	return "http: response: 410";
	case TB_STREAM_HTTP_STATE_RESPONSE_411: 	return "http: response: 411";
	case TB_STREAM_HTTP_STATE_RESPONSE_412: 	return "http: response: 412";
	case TB_STREAM_HTTP_STATE_RESPONSE_413: 	return "http: response: 413";
	case TB_STREAM_HTTP_STATE_RESPONSE_414: 	return "http: response: 414";
	case TB_STREAM_HTTP_STATE_RESPONSE_415: 	return "http: response: 415";
	case TB_STREAM_HTTP_STATE_RESPONSE_416: 	return "http: response: 416";
	case TB_STREAM_HTTP_STATE_RESPONSE_500: 	return "http: response: 500";
	case TB_STREAM_HTTP_STATE_RESPONSE_501: 	return "http: response: 501";
	case TB_STREAM_HTTP_STATE_RESPONSE_502: 	return "http: response: 502";
	case TB_STREAM_HTTP_STATE_RESPONSE_503: 	return "http: response: 503";
	case TB_STREAM_HTTP_STATE_RESPONSE_504: 	return "http: response: 504";
	case TB_STREAM_HTTP_STATE_RESPONSE_505: 	return "http: response: 505";
	case TB_STREAM_HTTP_STATE_RESPONSE_506: 	return "http: response: 506";
	case TB_STREAM_HTTP_STATE_RESPONSE_507: 	return "http: response: 507";
	case TB_STREAM_HTTP_STATE_RESPONSE_UNK: 	return "http: response: unknown code";
	case TB_STREAM_HTTP_STATE_RESPONSE_NUL: 	return "http: response: no";
	case TB_STREAM_HTTP_STATE_REQUEST_FAILED: 	return "http: request: failed";
	default: 									return "unknown";
	}

	return tb_null;
}
tb_bool_t tb_stream_ctrl(tb_handle_t handle, tb_size_t ctrl, ...)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, tb_false);


	return tb_false;
}
