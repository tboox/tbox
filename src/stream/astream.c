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
//#define TB_TRACE_IMPL_TAG 				"astream"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "astream.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_astream_save_done(tb_astream_t* ast, tb_astream_t* ost, tb_astream_save_func_t func, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(ast && ost && func, tb_false);

	// ok?
	return tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
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

	// no pending?
	tb_assert_and_check_return(!tb_atomic_get(&ast->pending));

	// stop it
	tb_atomic_set(&ast->stoped, 1);

	// exit it
	if (ast->exit) ast->exit(ast);

	// exit url
	tb_url_exit(&ast->url);

	// free it
	tb_free(ast);
}
tb_void_t tb_astream_kill(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return(ast);

	// opened?
	tb_check_return(tb_atomic_get(&ast->opened));

	// stop it
	tb_atomic_set(&ast->stoped, 1);

	// pending?
	tb_check_return(tb_atomic_get(&ast->pending));

	// kill it
	if (ast->kill) ast->kill(ast);
}
tb_bool_t tb_astream_open_impl(tb_astream_t* ast, tb_astream_open_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->open && func, tb_false);
	
	// check state
	tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened) && tb_atomic_get(&ast->stoped) && !tb_atomic_get(&ast->pending), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// init state
	tb_atomic_set0(&ast->stoped);
	tb_atomic_set(&ast->pending, 1);

	// open it
	tb_bool_t ok = ast->open(ast, func, priv);

	// post failed?
	if (!ok) 
	{
		tb_atomic_set(&ast->stoped, 1);
		tb_atomic_set0(&ast->pending);
	}

	// ok?
	return ok;
}
tb_bool_t tb_astream_read_impl(tb_astream_t* ast, tb_astream_read_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->read && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened) && !tb_atomic_get(&ast->pending), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// init state
	tb_atomic_set(&ast->pending, 1);

	// read it
	tb_bool_t ok = ast->read(ast, func, priv);

	// post failed?
	if (!ok) tb_atomic_set0(&ast->pending);

	// ok?
	return ok;
}
tb_bool_t tb_astream_writ_impl(tb_astream_t* ast, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->writ && data && size && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened) && !tb_atomic_get(&ast->pending), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// init state
	tb_atomic_set(&ast->pending, 1);

	// writ it
	tb_bool_t ok = ast->writ(ast, data, size, func, priv);

	// post failed?
	if (!ok) tb_atomic_set0(&ast->pending);

	// ok?
	return ok;
}
tb_bool_t tb_astream_save_impl(tb_astream_t* ast, tb_astream_t* ost, tb_astream_save_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->save && ost && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened) && !tb_atomic_get(&ast->pending), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// init state
	tb_atomic_set(&ast->pending, 1);

	// try to save it using the native stream
	tb_bool_t ok = ast->save(ast, ost, func, priv);

	// save it using stream if no native optimization
	if (!ok) ok = tb_astream_save_done(ast, ost, func, priv);

	// post failed?
	if (!ok) tb_atomic_set0(&ast->pending);

	// ok?
	return ok;
}
tb_bool_t tb_astream_seek_impl(tb_astream_t* ast, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->seek && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened) && !tb_atomic_get(&ast->pending), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// init state
	tb_atomic_set(&ast->pending, 1);

	// seek it
	tb_bool_t ok = ast->seek(ast, offset, func, priv);

	// post failed?
	if (!ok) tb_atomic_set0(&ast->pending);

	// ok?
	return ok;
}
tb_bool_t tb_astream_sync_impl(tb_astream_t* ast, tb_astream_sync_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->sync && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened) && !tb_atomic_get(&ast->pending), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// init state
	tb_atomic_set(&ast->pending, 1);

	// sync it
	tb_bool_t ok = ast->sync(ast, func, priv);

	// post failed?
	if (!ok) tb_atomic_set0(&ast->pending);

	// ok?
	return ok;
}
tb_bool_t tb_astream_try_open(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return_val(ast && ast->open, tb_false);
	
	// check state
	tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened) && tb_atomic_get(&ast->stoped) && !tb_atomic_get(&ast->pending), tb_false);

	// init state
	tb_atomic_set0(&ast->stoped);

	// open it
	tb_bool_t ok = ast->try_open(ast);

	// post failed?
	if (!ok) tb_atomic_set(&ast->stoped, 1);

	// ok?
	return ok;
}
tb_bool_t tb_astream_try_seek(tb_astream_t* ast, tb_hize_t offset)
{
	// check
	tb_assert_and_check_return_val(ast && ast->seek, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened) && !tb_atomic_get(&ast->pending), tb_false);

	// seek it
	return ast->try_seek(ast, offset);
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
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened), tb_false);

			// set url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (url && tb_url_set(&ast->url, url)) ret = tb_true;
		}
		break;
	case TB_ASTREAM_CTRL_GET_URL:
		{
			// get url
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
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened), tb_false);

			// set host
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
			// get host
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
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened), tb_false);

			// set port
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
			// get port
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
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened), tb_false);

			// set path
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
			// get path
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
	case TB_ASTREAM_CTRL_SET_TIMEOUT:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened), tb_false);

			// set timeout
			tb_long_t timeout = (tb_long_t)tb_va_arg(args, tb_long_t);
			ast->timeout = timeout;
			ret = tb_true;
		}
		break;
	case TB_ASTREAM_CTRL_GET_TIMEOUT:
		{
			// get timeout
			tb_long_t* ptimeout = (tb_long_t*)tb_va_arg(args, tb_long_t*);
			if (ptimeout)
			{
				*ptimeout = ast->timeout;
				ret = tb_true;
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
#ifdef __tb_debug__
tb_char_t const* tb_astream_func(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return_val(ast, tb_null);

	// the func
	return ast->func;
}
tb_char_t const* tb_astream_file(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return_val(ast, tb_null);

	// the file
	return ast->file;
}
tb_size_t tb_astream_line(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return_val(ast, 0);

	// the line
	return ast->line;
}
#endif
