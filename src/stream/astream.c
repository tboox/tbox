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
static tb_bool_t tb_astream_save_read(tb_astream_t* ast, tb_size_t state, tb_byte_t const* data, tb_size_t size, tb_pointer_t priv);
static tb_bool_t tb_astream_save_writ(tb_astream_t* ast, tb_size_t state, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_astream_storage_t* storage = (tb_astream_storage_t*)priv;
	tb_assert_and_check_return_val(ast && size && storage && storage->istream && storage->func, tb_false);
	tb_assert_and_check_return_val(((tb_astream_t*)storage->istream)->read, tb_false);

	// trace
	tb_trace_impl("storage: writ: real: %lu, size: %lu, state: %s", real, size, tb_astream_state_cstr(state));

	// ok?
	if (state == TB_ASTREAM_STATE_OK)
	{
		// check
		tb_assert_and_check_return_val(real, tb_false);

		// done func
		if (storage->func(storage->istream, storage->ostream, state, real, storage->priv))
		{
			// not finished? continue to writ
			tb_check_return_val(real == size, tb_true);

			// finished? post read
			((tb_astream_t*)storage->istream)->read(storage->istream, tb_astream_save_read, storage);
		}
	}
	// closed or failed?
	else 
	{
		// done func
		storage->func(storage->istream, storage->ostream, state, 0, storage->priv);
	}

	// break
	return tb_false;
}
static tb_bool_t tb_astream_save_read(tb_astream_t* ast, tb_size_t state, tb_byte_t const* data, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_astream_storage_t* storage = (tb_astream_storage_t*)priv;
	tb_assert_and_check_return_val(ast && data && storage && storage->ostream && storage->func, tb_false);
	tb_assert_and_check_return_val(((tb_astream_t*)storage->ostream)->writ, tb_false);

	// trace
	tb_trace_impl("storage: read: size: %lu, state: %s", size, tb_astream_state_cstr(state));

	// ok?
	if (state == TB_ASTREAM_STATE_OK)
	{
		// check
		tb_assert_and_check_return_val(size, tb_false);

		// post writ
		((tb_astream_t*)storage->ostream)->writ(storage->ostream, data, size, tb_astream_save_writ, storage);
	}
	// closed or failed?
	else 
	{
		// done func
		storage->func(ast, storage->ostream, state, 0, storage->priv);
	}

	// break
	return tb_false;
}
static tb_bool_t tb_astream_save_done(tb_astream_t* ast, tb_astream_t* ost, tb_astream_save_func_t func, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(ast && ast->read && ost && ost->writ && func, tb_false);

	// init storage 
	ast->storage.func = func;
	ast->storage.priv = priv;
	ast->storage.istream = ast;
	ast->storage.ostream = ost;

	// post read
	return ast->read(ast, tb_astream_save_read, &ast->storage);
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
	tb_check_return(!tb_atomic_fetch_and_set(&ast->stoped, 1));

	// kill it
	if (ast->kill) ast->kill(ast);
}
tb_bool_t tb_astream_open_impl(tb_astream_t* ast, tb_astream_open_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->open && func, tb_false);
	
	// check state
	tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened) && tb_atomic_get(&ast->stoped), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// init state
	tb_atomic_set0(&ast->stoped);

	// open it
	tb_bool_t ok = ast->open(ast, func, priv);

	// post failed?
	if (!ok) tb_atomic_set(&ast->stoped, 1);

	// ok?
	return ok;
}
tb_bool_t tb_astream_read_impl(tb_astream_t* ast, tb_astream_read_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->read && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// read it
	return ast->read(ast, func, priv);
}
tb_bool_t tb_astream_writ_impl(tb_astream_t* ast, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->writ && data && size && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// writ it
	return ast->writ(ast, data, size, func, priv);
}
tb_bool_t tb_astream_save_impl(tb_astream_t* ast, tb_astream_t* ost, tb_astream_save_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->save && ost && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// try to save it using the native stream
	tb_bool_t ok = ast->save(ast, ost, func, priv);

	// save it using stream if no native optimization
	if (!ok) ok = tb_astream_save_done(ast, ost, func, priv);

	// ok?
	return ok;
}
tb_bool_t tb_astream_seek_impl(tb_astream_t* ast, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->seek && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// seek it
	return ast->seek(ast, offset, func, priv);
}
tb_bool_t tb_astream_sync_impl(tb_astream_t* ast, tb_astream_sync_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(ast && ast->sync && func, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened), tb_false);

	// save debug info
#ifdef __tb_debug__
	ast->func = func_;
	ast->file = file_;
	ast->line = line_;
#endif

	// sync it
	return ast->sync(ast, func, priv);
}
tb_bool_t tb_astream_try_open(tb_astream_t* ast)
{
	// check
	tb_assert_and_check_return_val(ast, tb_false);
	tb_check_return_val(ast->try_open, tb_false);
	
	// check state
	tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened) && tb_atomic_get(&ast->stoped), tb_false);

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
	tb_assert_and_check_return_val(ast, tb_false);
	tb_check_return_val(ast->try_seek, tb_false);
	
	// check state
	tb_check_return_val(!tb_atomic_get(&ast->stoped), tb_false);
	tb_assert_and_check_return_val(tb_atomic_get(&ast->opened), tb_false);

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
tb_hong_t tb_astream_left(tb_astream_t const* ast)
{
	// check
	tb_assert_and_check_return_val(ast, -1);
	
	// the offset
	tb_hong_t offset = tb_astream_offset(ast);
	tb_check_return_val(offset >= 0, -1);

	// the size
	tb_hize_t size = tb_astream_size(ast);

	// the left
	return ((size && size >= offset)? (size - offset) : -1);
}
tb_hong_t tb_astream_offset(tb_astream_t const* ast)
{
	// check
	tb_assert_and_check_return_val(ast, -1);

	// get the offset
	tb_hize_t offset = 0;
	return tb_astream_ctrl((tb_astream_t*)ast, TB_ASTREAM_CTRL_GET_OFFSET, &offset)? offset : -1;
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
	case TB_ASTREAM_STATE_NOT_SUPPORTED: 		return "not supported";
	case TB_ASTREAM_STATE_UNKNOWN_ERROR: 		return "unknown error";
	case TB_ASTREAM_SOCK_STATE_DNS_FAILED: 		return "sock: dns: failed";
	case TB_ASTREAM_SOCK_STATE_CONNECT_FAILED: 	return "sock: connect: failed";
	case TB_ASTREAM_SOCK_STATE_CONNECT_TIMEOUT: return "sock: connect: timeout";
	case TB_ASTREAM_SOCK_STATE_RECV_TIMEOUT: 	return "sock: recv: timeout";
	case TB_ASTREAM_SOCK_STATE_SEND_TIMEOUT: 	return "sock: send: timeout";
	default: 									return "unknown";
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
