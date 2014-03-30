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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		stream.c
 * @defgroup 	stream
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 				"stream"
#define TB_TRACE_MODULE_DEBUG 				(0)
 
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
tb_hong_t tb_stream_size(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, 0);

	// get the size
	tb_hong_t size = -1;
	return tb_stream_ctrl((tb_stream_t*)stream, TB_STREAM_CTRL_GET_SIZE, &size)? size : -1;
}
tb_hize_t tb_stream_left(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, 0);
	
	// the size
	tb_hong_t size = tb_stream_size(stream);
	tb_check_return_val(size >= 0, -1);

	// the offset
	tb_hize_t offset = tb_stream_offset(stream);
	tb_assert_and_check_return_val(offset <= size, 0);

	// the left
	return size - offset;
}
tb_bool_t tb_stream_beof(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, tb_true);

	// size
	tb_hong_t size = tb_stream_size(stream);
	tb_hize_t offt = tb_stream_offset(stream);

	// eof?
	return (size > 0 && offt >= size)? tb_true : tb_false;
}
tb_hize_t tb_stream_offset(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, 0);

	// get the offset
	tb_hize_t offset = 0;
	return tb_stream_ctrl((tb_stream_t*)stream, TB_STREAM_CTRL_GET_OFFSET, &offset)? offset : 0;
}
tb_bool_t tb_stream_is_opened(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, tb_false);

	// is opened?
	return tb_atomic_get(&stream->bopened)? tb_true : tb_false;
}
tb_long_t tb_stream_timeout(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream, -1);

	// get the timeout
	tb_long_t timeout = -1;
	return tb_stream_ctrl(stream, TB_STREAM_CTRL_GET_TIMEOUT, &timeout)? timeout : -1;
}
tb_bool_t tb_stream_ctrl(tb_handle_t handle, tb_size_t ctrl, ...)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return_val(stream && stream->ctrl, tb_false);

	// init args
	tb_va_list_t args;
    tb_va_start(args, ctrl);

	// ctrl
	tb_bool_t ok = tb_false;
	switch (ctrl)
	{
	case TB_STREAM_CTRL_SET_URL:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(stream), tb_false);

			// set url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (url && tb_url_set(&stream->url, url)) ok = tb_true;
		}
		break;
	case TB_STREAM_CTRL_GET_URL:
		{
			// get url
			tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			if (purl)
			{
				tb_char_t const* url = tb_url_get(&stream->url);
				if (url)
				{
					*purl = url;
					ok = tb_true;
				}
			}
		}
		break;
	case TB_STREAM_CTRL_SET_HOST:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(stream), tb_false);

			// set host
			tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (host)
			{
				tb_url_host_set(&stream->url, host);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_GET_HOST:
		{
			// get host
			tb_char_t const** phost = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			if (phost)
			{
				tb_char_t const* host = tb_url_host_get(&stream->url);
				if (host)
				{
					*phost = host;
					ok = tb_true;
				}
			}
		}
		break;
	case TB_STREAM_CTRL_SET_PORT:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(stream), tb_false);

			// set port
			tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
			if (port)
			{
				tb_url_port_set(&stream->url, port);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_GET_PORT:
		{
			// get port
			tb_size_t* pport = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			if (pport)
			{
				*pport = tb_url_port_get(&stream->url);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_SET_PATH:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(stream), tb_false);

			// set path
			tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (path)
			{
				tb_url_path_set(&stream->url, path);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_GET_PATH:
		{
			// get path
			tb_char_t const** ppath = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			if (ppath)
			{
				tb_char_t const* path = tb_url_path_get(&stream->url);
				if (path)
				{
					*ppath = path;
					ok = tb_true;
				}
			}
		}
		break;
	case TB_STREAM_CTRL_SET_SSL:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(stream), tb_false);

			// set ssl
			tb_bool_t bssl = (tb_bool_t)tb_va_arg(args, tb_bool_t);
			tb_url_ssl_set(&stream->url, bssl);
			ok = tb_true;
		}
		break;
	case TB_STREAM_CTRL_GET_SSL:
		{
			// get ssl
			tb_bool_t* pssl = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
			if (pssl)
			{
				*pssl = tb_url_ssl_get(&stream->url);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_SET_TIMEOUT:
		{
			// check
			tb_assert_and_check_return_val(!tb_stream_is_opened(stream), tb_false);

			// set timeout
			tb_long_t timeout = (tb_long_t)tb_va_arg(args, tb_long_t);
			stream->timeout = timeout;
			ok = tb_true;
		}
		break;
	case TB_STREAM_CTRL_GET_TIMEOUT:
		{
			// get timeout
			tb_long_t* ptimeout = (tb_long_t*)tb_va_arg(args, tb_long_t*);
			if (ptimeout)
			{
				*ptimeout = stream->timeout;
				ok = tb_true;
			}
		}
		break;
	default:
		break;
	}

	// reset args
	tb_va_end(args);
    tb_va_start(args, ctrl);

	// ctrl stream
	ok = (stream->ctrl(stream, ctrl, args) || ok)? tb_true : tb_false;

	// exit args
	tb_va_end(args);

	// ok?
	return ok;
}
tb_void_t tb_stream_kill(tb_handle_t handle)
{
	// check
	tb_stream_t* stream = (tb_stream_t*)handle;
	tb_assert_and_check_return(stream);

	// stop it
	tb_check_return(!tb_atomic_fetch_and_set(&stream->bstoped, 1));

	// trace
	tb_trace_d("kill: ..");

	// kill it
	if (stream->kill) stream->kill(stream);
}
