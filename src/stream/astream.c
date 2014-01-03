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
 * @file		astream.c
 * @ingroup 	stream
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 				"astream"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "astream.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_astream_t* tb_astream_init_from_url(tb_aicp_t* aicp, tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(aicp && url, tb_null);

	// the init
	static tb_astream_t* (*g_init[])() = 
	{
		tb_null
	,	tb_astream_init_file
	,	tb_astream_init_sock
	,	tb_astream_init_http
	};

	// init
	tb_char_t const* 	p = url;
	tb_astream_t* 		ast = tb_null;
	tb_size_t 			type = TB_ASTREAM_TYPE_NONE;
	if (!tb_strnicmp(p, "http://", 7)) 			type = TB_ASTREAM_TYPE_HTTP;
	else if (!tb_strnicmp(p, "sock://", 7)) 	type = TB_ASTREAM_TYPE_SOCK;
	else if (!tb_strnicmp(p, "https://", 8)) 	type = TB_ASTREAM_TYPE_HTTP;
	else if (!tb_strnicmp(p, "socks://", 8)) 	type = TB_ASTREAM_TYPE_SOCK;
	else if (!tb_strstr(p, "://")) 				type = TB_ASTREAM_TYPE_FILE;
	else 
	{
		tb_trace("[astream]: unknown prefix for url: %s", url);
		return tb_null;
	}
	tb_assert_and_check_goto(type && type < tb_arrayn(g_init) && g_init[type], fail);

	// init stream
	ast = g_init[type](aicp);
	tb_assert_and_check_goto(ast, fail);

	// set url
	if (!tb_astream_ctrl(ast, TB_ASTREAM_CTRL_SET_URL, url)) goto fail;

	// ok
	return ast;

fail:
	
	// exit stream
	if (ast) tb_astream_exit(ast);
	return tb_null;
}
tb_void_t tb_astream_exit(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return(ast);

	// exit url
	tb_url_exit(&ast->url);

	// exit it
	if (ast->exit) ast->exit(ast);

	// free it
	tb_free(ast);
}
tb_void_t tb_astream_kill(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return(ast);

	// kill it
	if (ast->kill) ast->kill(ast);
}
tb_bool_t tb_astream_open(tb_astream_t* ast, tb_astream_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(ast && ast->open && func, tb_false);

	// open it
	return ast->open(ast, func, priv) >= 0? tb_true : tb_false;
}
tb_bool_t tb_astream_read(tb_astream_t* ast, tb_astream_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(ast && ast->read && func, tb_false);

	// read it
	return ast->read(ast, func, priv) >= 0? tb_true : tb_false;
}
tb_bool_t tb_astream_writ(tb_astream_t* ast, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(ast && ast->writ && data && size && func, tb_false);

	// writ it
	return ast->writ(ast, data, size, func, priv) >= 0? tb_true : tb_false;
}
tb_bool_t tb_astream_save(tb_astream_t* ast, tb_astream_t* ost, tb_astream_save_func_t func, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(ast && ast->save && ost && func, tb_false);

	// save it
	return ast->save(ast, ost, func, priv) >= 0? tb_true : tb_false;
}
tb_bool_t tb_astream_seek(tb_astream_t* ast, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(ast && ast->seek && func, tb_false);

	// seek it
	return ast->seek(ast, offset, func, priv) >= 0? tb_true : tb_false;
}
tb_bool_t tb_astream_sync(tb_astream_t* ast, tb_astream_sync_func_t func, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(ast && ast->sync && func, tb_false);

	// sync it
	return ast->sync(ast, func, priv) >= 0? tb_true : tb_false;
}
tb_bool_t tb_astream_try_open(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return_val(ast && ast->open, tb_false);

	// open it
	return ast->open(ast, tb_null, tb_null) > 0? tb_true : tb_false;
}
tb_bool_t tb_astream_try_seek(tb_astream_t* ast, tb_hize_t offset)
{
	// check
	tb_assert_and_check_return_val(ast && ast->seek, tb_false);

	// seek it
	return ast->seek(ast, offset, tb_null, tb_null) > 0? tb_true : tb_false;
}
tb_aicp_t* tb_astream_aicp(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return_val(ast, tb_null);

	// the aicp
	return ast->aicp;
}
tb_size_t tb_astream_type(tb_astream_t const* ast)
{
	// check
	tb_assert_and_check_return_val(ast, TB_ASTREAM_TYPE_NONE);

	// the type
	return ast->type;
}
tb_hize_t tb_astream_size(tb_astream_t const* ast)
{
	// check
	tb_assert_and_check_return_val(ast, 0);

	// get the size
	tb_hize_t size = 0;
	return tb_astream_ctrl((tb_astream_t*)ast, TB_ASTREAM_CTRL_GET_SIZE, &size)? size : 0;
}
tb_hize_t tb_astream_left(tb_astream_t const* ast)
{
	// check
	tb_assert_and_check_return_val(ast, 0);

	// the size
	tb_hize_t size = tb_astream_size(ast);
	
	// the offset
	tb_hize_t offset = tb_astream_offset(ast);

	// the left
	return ((size && size >= offset)? (size - offset) : -1);
}
tb_hize_t tb_astream_offset(tb_astream_t const* ast)
{
	// check
	tb_assert_and_check_return_val(ast, 0);

	// get the offset
	tb_hize_t offset = 0;
	return tb_astream_ctrl((tb_astream_t*)ast, TB_ASTREAM_CTRL_GET_OFFSET, &offset)? offset : 0;
}
tb_size_t tb_astream_timeout(tb_astream_t const* ast)
{
	// check
	tb_assert_and_check_return_val(ast, 0);

	// get the timeout
	tb_size_t timeout = 0;
	return tb_astream_ctrl((tb_astream_t*)ast, TB_ASTREAM_CTRL_GET_TIMEOUT, &timeout)? timeout : 0;
}
tb_char_t const* tb_astream_state_cstr(tb_size_t state)
{
	// done
	switch (state)
	{
	case TB_ASTREAM_STATE_OK: 					return "ok";
	case TB_ASTREAM_STATE_CLOSED: 				return "closed";
	case TB_ASTREAM_STATE_UNKNOWN_ERROR: 		return "unknown error";
	case TB_ASTREAM_SOCK_STATE_DNS_FAILED: 		return "sock: dns: failed";
	case TB_ASTREAM_SOCK_STATE_CONNECT_FAILED: 	return "sock: connect: failed";
	case TB_ASTREAM_SOCK_STATE_CONNECT_TIMEOUT: return "sock: connect: timeout";
	default: 									return "http: unknown";
	}

	return tb_null;
}
tb_bool_t tb_astream_ctrl(tb_astream_t* ast, tb_size_t ctrl, ...)
{
	// check
	tb_assert_and_check_return_val(ast && ast->ctrl, tb_false);

	// init args
	tb_va_list_t args;
    tb_va_start(args, ctrl);

	// ctrl
	tb_bool_t ret = tb_false;
	switch (ctrl)
	{
	case TB_ASTREAM_CTRL_SET_URL:
		{
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (url && tb_url_set(&ast->url, url)) ret = tb_true;
		}
		break;
	case TB_ASTREAM_CTRL_GET_URL:
		{
			tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			if (purl)
			{
				tb_char_t const* url = tb_url_get(&ast->url);
				if (url)
				{
					*purl = url;
					ret = tb_true;
				}
			}
		}
		break;
	case TB_ASTREAM_CTRL_SET_HOST:
		{
			tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (host)
			{
				tb_url_host_set(&ast->url, host);
				ret = tb_true;
			}
		}
		break;
	case TB_ASTREAM_CTRL_GET_HOST:
		{
			tb_char_t const** phost = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			if (phost)
			{
				tb_char_t const* host = tb_url_host_get(&ast->url);
				if (host)
				{
					*phost = host;
					ret = tb_true;
				}
			}
		}
		break;
	case TB_ASTREAM_CTRL_SET_PORT:
		{
			tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
			if (port)
			{
				tb_url_port_set(&ast->url, port);
				ret = tb_true;
			}
		}
		break;
	case TB_ASTREAM_CTRL_GET_PORT:
		{
			tb_size_t* pport = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			if (pport)
			{
				*pport = tb_url_port_get(&ast->url);
				ret = tb_true;
			}
		}
		break;
	case TB_ASTREAM_CTRL_SET_PATH:
		{
			tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (path)
			{
				tb_url_path_set(&ast->url, path);
				ret = tb_true;
			}
		}
		break;
	case TB_ASTREAM_CTRL_GET_PATH:
		{
			tb_char_t const** ppath = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			if (ppath)
			{
				tb_char_t const* path = tb_url_path_get(&ast->url);
				if (path)
				{
					*ppath = path;
					ret = tb_true;
				}
			}
		}
		break;
	default:
		break;
	}

	// reset args
	tb_va_end(args);
    tb_va_start(args, ctrl);

	// ctrl for native stream
	ret = (ast->ctrl(ast, ctrl, args) || ret)? tb_true : tb_false;

	// exit args
	tb_va_end(args);

	// ok?
	return ret;
}
