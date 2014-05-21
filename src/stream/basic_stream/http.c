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
 * \http		http.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../asio/asio.h"
#include "../../string/string.h"
#include "../../network/network.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the http stream type
typedef struct __tb_basic_stream_http_t
{
	// the base
	tb_basic_stream_t 		base;

	// the http 
	tb_handle_t 			http;

}tb_basic_stream_http_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_basic_stream_http_t* tb_basic_stream_http_cast(tb_handle_t stream)
{
	tb_basic_stream_t* bstream = (tb_basic_stream_t*)stream;
	tb_assert_and_check_return_val(bstream && bstream->base.type == TB_STREAM_TYPE_HTTP, tb_null);
	return (tb_basic_stream_http_t*)bstream;
}
static tb_bool_t tb_basic_stream_http_open(tb_handle_t bstream)
{
	// check
	tb_basic_stream_http_t* hstream = tb_basic_stream_http_cast(bstream);
	tb_assert_and_check_return_val(hstream && hstream->http, tb_false);

	// the http status
	tb_http_status_t const*	status = tb_http_status(hstream->http);
	tb_assert_and_check_return_val(status, tb_false);

	// open it
	tb_bool_t ok = tb_http_open(hstream->http);

	// save state
	hstream->base.state = ok? TB_STATE_OK : status->state;

	// ok?
	return ok;
}
static tb_bool_t tb_basic_stream_http_clos(tb_handle_t bstream)
{
	// check
	tb_basic_stream_http_t* hstream = tb_basic_stream_http_cast(bstream);
	tb_assert_and_check_return_val(hstream && hstream->http, tb_false);

	// close it
	return tb_http_clos(hstream->http);
}
static tb_void_t tb_basic_stream_http_exit(tb_handle_t bstream)
{
	tb_basic_stream_http_t* hstream = tb_basic_stream_http_cast(bstream);
	if (hstream && hstream->http) tb_http_exit(hstream->http);
}
static tb_void_t tb_basic_stream_http_kill(tb_handle_t bstream)
{
	tb_basic_stream_http_t* hstream = tb_basic_stream_http_cast(bstream);
	if (hstream && hstream->http) tb_http_kill(hstream->http);
}
static tb_long_t tb_basic_stream_http_read(tb_handle_t bstream, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_basic_stream_http_t* hstream = tb_basic_stream_http_cast(bstream);
	tb_assert_and_check_return_val(hstream && hstream->http, -1);

	// the http status
	tb_http_status_t const*	status = tb_http_status(hstream->http);
	tb_assert_and_check_return_val(status, -1);

	// check
	tb_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// read data
	tb_long_t ok = tb_http_read(hstream->http, data, size);

	// save state
	hstream->base.state = ok >= 0? TB_STATE_OK : status->state;

	// ok?
	return ok;
}
static tb_bool_t tb_basic_stream_http_seek(tb_handle_t bstream, tb_hize_t offset)
{
	// check
	tb_basic_stream_http_t* hstream = tb_basic_stream_http_cast(bstream);
	tb_assert_and_check_return_val(hstream && hstream->http, tb_false);

	// seek
	return tb_http_seek(hstream->http, offset);
}
static tb_long_t tb_basic_stream_http_wait(tb_handle_t bstream, tb_size_t wait, tb_long_t timeout)
{
	// check
	tb_basic_stream_http_t* hstream = tb_basic_stream_http_cast(bstream);
	tb_assert_and_check_return_val(hstream && hstream->http, -1);

	// the http status
	tb_http_status_t const*	status = tb_http_status(hstream->http);
	tb_assert_and_check_return_val(status, -1);

	// wait
	tb_long_t ok = tb_http_wait(hstream->http, wait, timeout);

	// save state
	hstream->base.state = ok >= 0? TB_STATE_OK : status->state;

	// ok?
	return ok;
}
static tb_bool_t tb_basic_stream_http_ctrl(tb_handle_t bstream, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_basic_stream_http_t* hstream = tb_basic_stream_http_cast(bstream);
	tb_assert_and_check_return_val(hstream && hstream->http, tb_false);

	// done
	switch (ctrl)
	{
	case TB_STREAM_CTRL_GET_SIZE:
		{
			// psize
			tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
			tb_assert_and_check_return_val(psize, tb_false);

			// status
			tb_http_status_t const* status = tb_http_status(hstream->http);
			tb_assert_and_check_return_val(status, 0);

			// get size
			*psize = (!status->bgzip && !status->bdeflate && !status->bchunked)? status->document_size : -1;
			return tb_true;
		}
	case TB_STREAM_CTRL_GET_OFFSET:
		{
			// the poffset
			tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(poffset, tb_false);

			// get offset
			*poffset = hstream->base.offset;
			return tb_true;
		}
	case TB_STREAM_CTRL_SET_URL:
		{
			// url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(url, tb_false);
		
			// set url
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_URL, url);
		}
		break;
	case TB_STREAM_CTRL_GET_URL:
		{
			// purl
			tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(purl, tb_false);
	
			// get url
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_URL, purl);
		}
		break;
	case TB_STREAM_CTRL_SET_HOST:
		{
			// host
			tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(host, tb_false);
	
			// set host
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_HOST, host);
		}
		break;
	case TB_STREAM_CTRL_GET_HOST:
		{
			// phost
			tb_char_t const** phost = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(phost, tb_false); 

			// get host
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_HOST, phost);
		}
		break;
	case TB_STREAM_CTRL_SET_PORT:
		{
			// port
			tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
			tb_assert_and_check_return_val(port, tb_false);
	
			// set port
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_PORT, port);
		}
		break;
	case TB_STREAM_CTRL_GET_PORT:
		{
			// pport
			tb_size_t* pport = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pport, tb_false);
	
			// get port
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_PORT, pport);
		}
		break;
	case TB_STREAM_CTRL_SET_PATH:
		{
			// path
			tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(path, tb_false);
	
			// set path
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_PATH, path);
		}
		break;
	case TB_STREAM_CTRL_GET_PATH:
		{
			// ppath
			tb_char_t const** ppath = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(ppath, tb_false);
	
			// get path
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_PATH, ppath);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_METHOD:
		{
			// method
			tb_size_t method = (tb_size_t)tb_va_arg(args, tb_size_t);
	
			// set method
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_METHOD, method);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_METHOD:
		{
			// pmethod
			tb_size_t* pmethod = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pmethod, tb_false);
	
			// get method
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_METHOD, pmethod);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_HEAD:
		{
			// key
			tb_char_t const* key = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(key, tb_false);

 			// val
			tb_char_t const* val = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(val, tb_false);
	
			// set head
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_HEAD, key, val);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_HEAD:
		{
			// key
			tb_char_t const* key = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(key, tb_false);

			// pval
			tb_char_t const** pval = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(pval, tb_false);
	
			// get head
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_HEAD, key, pval);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_HEAD_FUNC:
		{
			// head_func
			tb_http_head_func_t head_func = (tb_http_head_func_t)tb_va_arg(args, tb_http_head_func_t);

			// set head_func
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_HEAD_FUNC, head_func);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_HEAD_FUNC:
		{
			// phead_func
			tb_http_head_func_t* phead_func = (tb_http_head_func_t*)tb_va_arg(args, tb_http_head_func_t*);
			tb_assert_and_check_return_val(phead_func, tb_false);

			// get head_func
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_HEAD_FUNC, phead_func);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_HEAD_PRIV:
		{
			// head_priv
			tb_pointer_t head_priv = (tb_pointer_t)tb_va_arg(args, tb_pointer_t);

			// set head_priv
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_HEAD_PRIV, head_priv);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_HEAD_PRIV:
		{
			// phead_priv
			tb_pointer_t* phead_priv = (tb_pointer_t*)tb_va_arg(args, tb_pointer_t*);
			tb_assert_and_check_return_val(phead_priv, tb_false);

			// get head_priv
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_HEAD_PRIV, phead_priv);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_RANGE:
		{
			tb_hize_t bof = (tb_hize_t)tb_va_arg(args, tb_hize_t);
			tb_hize_t eof = (tb_hize_t)tb_va_arg(args, tb_hize_t);
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_RANGE, bof, eof);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_RANGE:
		{
			// pbof
			tb_hize_t* pbof = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(pbof, tb_false);

			// peof
			tb_hize_t* peof = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(peof, tb_false);

			// ok
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_RANGE, pbof, peof);
		}
		break;
	case TB_STREAM_CTRL_SET_SSL:
		{
			// bssl
			tb_bool_t bssl = (tb_bool_t)tb_va_arg(args, tb_bool_t);
	
			// set ssl
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_SSL, bssl);
		}
		break;
	case TB_STREAM_CTRL_GET_SSL:
		{
			// pssl
			tb_bool_t* pssl = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
			tb_assert_and_check_return_val(pssl, tb_false);
	
			// get ssl
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_SSL, pssl);
		}
		break;
	case TB_STREAM_CTRL_SET_TIMEOUT:
		{
			// timeout
			tb_size_t timeout = (tb_size_t)tb_va_arg(args, tb_size_t);
			tb_assert_and_check_return_val(timeout, tb_false);
	
			// set timeout
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_TIMEOUT, timeout);
		}
		break;
	case TB_STREAM_CTRL_GET_TIMEOUT:
		{
			// ptimeout
			tb_size_t* ptimeout = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(ptimeout, tb_false);
	
			// get timeout
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_TIMEOUT, ptimeout);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_POST_URL:
		{
			// url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(url, tb_false);
			
			// set url
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_URL, url);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_POST_URL:
		{
			// purl
			tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(purl, tb_false);

			// get url
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_URL, purl);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_POST_DATA:
		{
			// post data
			tb_byte_t const* 	data = (tb_byte_t const*)tb_va_arg(args, tb_byte_t const*);

			// post size
			tb_size_t 			size = (tb_size_t)tb_va_arg(args, tb_size_t);

			// set data and size
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_DATA, data, size);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_POST_DATA:
		{
			// pdata and psize
			tb_byte_t const** 	pdata = (tb_byte_t const**)tb_va_arg(args, tb_byte_t const**);
			tb_size_t* 			psize = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pdata && psize, tb_false);

			// get post data and size
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_DATA, pdata, psize);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_POST_FUNC:
		{
			// func
			tb_http_post_func_t func = (tb_http_post_func_t)tb_va_arg(args, tb_http_post_func_t);

			// set post func
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_FUNC, func);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_POST_FUNC:
		{
			// pfunc
			tb_http_post_func_t* pfunc = (tb_http_post_func_t*)tb_va_arg(args, tb_http_post_func_t*);
			tb_assert_and_check_return_val(pfunc, tb_false);

			// get post func
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_FUNC, pfunc);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_POST_PRIV:
		{
			// post priv
			tb_cpointer_t priv = (tb_pointer_t)tb_va_arg(args, tb_pointer_t);

			// set post priv
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_PRIV, priv);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_POST_PRIV:
		{
			// ppost priv
			tb_pointer_t* ppriv = (tb_pointer_t*)tb_va_arg(args, tb_pointer_t*);
			tb_assert_and_check_return_val(ppriv, tb_false);

			// get post priv
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_PRIV, ppriv);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_POST_LRATE:
		{
			// post lrate
			tb_size_t lrate = (tb_size_t)tb_va_arg(args, tb_size_t);

			// set post lrate
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_LRATE, lrate);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_POST_LRATE:
		{
			// ppost lrate
			tb_size_t* plrate = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(plrate, tb_false);

			// get post lrate
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_LRATE, plrate);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_AUTO_UNZIP:
		{
			// bunzip
			tb_bool_t bunzip = (tb_bool_t)tb_va_arg(args, tb_bool_t);

			// set bunzip
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_AUTO_UNZIP, bunzip);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_AUTO_UNZIP:
		{
			// pbunzip
			tb_bool_t* pbunzip = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
			tb_assert_and_check_return_val(pbunzip, tb_false);

			// get bunzip
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_AUTO_UNZIP, pbunzip);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_REDIRECT:
		{
			// redirect
			tb_size_t redirect = (tb_size_t)tb_va_arg(args, tb_size_t);

			// set redirect
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_REDIRECT, redirect);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_REDIRECT:
		{
			// predirect
			tb_size_t* predirect = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(predirect, tb_false);

			// get redirect
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_REDIRECT, predirect);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_VERSION:
		{
			// version
			tb_size_t version = (tb_size_t)tb_va_arg(args, tb_size_t);

			// set version
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_VERSION, version);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_VERSION:
		{
			// pversion
			tb_size_t* pversion = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pversion, tb_false);

			// get version
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_VERSION, pversion);
		}
		break;
	case TB_STREAM_CTRL_HTTP_SET_COOKIES:
		{
			// cookies
			tb_handle_t cookies = (tb_handle_t)tb_va_arg(args, tb_handle_t);

			// set cookies
			return tb_http_option(hstream->http, TB_HTTP_OPTION_SET_COOKIES, cookies);
		}
		break;
	case TB_STREAM_CTRL_HTTP_GET_COOKIES:
		{
			// pcookies
			tb_handle_t* pcookies = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
			tb_assert_and_check_return_val(pcookies, tb_false);

			// get version
			return tb_http_option(hstream->http, TB_HTTP_OPTION_GET_COOKIES, pcookies);
		}
		break;
	default:
		break;
	}
	return tb_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_basic_stream_t* tb_basic_stream_init_http()
{
	// make stream
	tb_basic_stream_http_t* bstream = (tb_basic_stream_http_t*)tb_malloc0(sizeof(tb_basic_stream_http_t));
	tb_assert_and_check_return_val(bstream, tb_null);

	// init stream
	if (!tb_basic_stream_init((tb_basic_stream_t*)bstream, TB_STREAM_TYPE_HTTP, 0)) goto fail;
	bstream->base.open 		= tb_basic_stream_http_open;
	bstream->base.clos 		= tb_basic_stream_http_clos;
	bstream->base.read 		= tb_basic_stream_http_read;
	bstream->base.seek 		= tb_basic_stream_http_seek;
	bstream->base.wait 		= tb_basic_stream_http_wait;
	bstream->base.exit 		= tb_basic_stream_http_exit;
	bstream->base.base.ctrl = tb_basic_stream_http_ctrl;
	bstream->base.base.kill = tb_basic_stream_http_kill;
	bstream->http 			= tb_http_init();
	tb_assert_and_check_goto(bstream->http, fail);

	// ok
	return (tb_basic_stream_t*)bstream;

fail:
	if (bstream) tb_basic_stream_exit((tb_basic_stream_t*)bstream);
	return tb_null;
}
tb_basic_stream_t* tb_basic_stream_init_from_http(tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl)
{
	// check
	tb_assert_and_check_return_val(host && port && path, tb_null);

	// init http stream
	tb_basic_stream_t* bstream = tb_basic_stream_init_http();
	tb_assert_and_check_return_val(bstream, tb_null);

	// ctrl
	if (!tb_stream_ctrl(bstream, TB_STREAM_CTRL_SET_HOST, host)) goto fail;
	if (!tb_stream_ctrl(bstream, TB_STREAM_CTRL_SET_PORT, port)) goto fail;
	if (!tb_stream_ctrl(bstream, TB_STREAM_CTRL_SET_PATH, path)) goto fail;
	if (!tb_stream_ctrl(bstream, TB_STREAM_CTRL_SET_SSL, bssl)) goto fail;
	
	// ok
	return bstream;

fail:
	if (bstream) tb_basic_stream_exit(bstream);
	return tb_null;
}
