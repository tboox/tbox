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
 * @author      ruki
 * @file        http.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "async_stream_http"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream.h"
#include "../../asio/asio.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the http stream type
typedef struct __tb_async_stream_http_t
{
    // the base
    tb_async_stream_t                   base;

    // the http 
    tb_handle_t                         http;

    // the size
    tb_atomic64_t                       size;

    // the offset
    tb_atomic64_t                       offset;

    // the func
    union
    {
        tb_async_stream_open_func_t     open;
        tb_async_stream_read_func_t     read;
        tb_async_stream_seek_func_t     seek;
        tb_async_stream_sync_func_t     sync;
        tb_async_stream_task_func_t     task;
        tb_async_stream_clos_func_t     clos;

    }                                   func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_http_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_async_stream_http_t* tb_async_stream_http_cast(tb_handle_t stream)
{
    tb_async_stream_t* astream = (tb_async_stream_t*)stream;
    tb_assert_and_check_return_val(astream && astream->base.type == TB_STREAM_TYPE_HTTP, tb_null);
    return (tb_async_stream_http_t*)astream;
}
static tb_bool_t tb_async_stream_http_open_func(tb_handle_t http, tb_size_t state, tb_http_status_t const* status, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(http && status, tb_false);

    // the stream
    tb_async_stream_http_t* hstream = (tb_async_stream_http_t*)priv;
    tb_assert_and_check_return_val(hstream && hstream->func.open, tb_false);

    // opened
    tb_atomic_set(&hstream->base.base.istate, TB_STATE_OPENED);

    // save size
    tb_hong_t size = (!status->bgzip && !status->bdeflate && !status->bchunked)? status->document_size : -1;
    if (size >= 0) tb_atomic64_set(&hstream->size, size);

    // done func
    return hstream->func.open((tb_async_stream_t*)hstream, state, hstream->priv);
}
static tb_bool_t tb_async_stream_http_open(tb_handle_t astream, tb_async_stream_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_http_t* hstream = tb_async_stream_http_cast(astream);
    tb_assert_and_check_return_val(hstream && hstream->http && func, tb_false);

    // save func and priv
    hstream->priv       = priv;
    hstream->func.open  = func;

    // init size and offset
    tb_atomic64_set(&hstream->size, -1);
    tb_atomic64_set0(&hstream->offset);
 
    // post open
    return tb_aicp_http_open(hstream->http, tb_async_stream_http_open_func, astream);
}
static tb_void_t tb_async_stream_http_clos_func(tb_handle_t http, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_stream_http_t* hstream = tb_async_stream_http_cast((tb_handle_t)priv);
    tb_assert_and_check_return(hstream && hstream->func.clos);

    // trace
    tb_trace_d("clos: notify: ..");

    // clear size 
    tb_atomic64_set(&hstream->size, -1);

    // clear offset
    tb_atomic64_set0(&hstream->offset);

	// clear base
	tb_async_stream_clear(&hstream->base);

    /* done clos func
     *
     * note: cannot use this stream after closing, the stream may be exited in the closing func
     */
    hstream->func.clos(&hstream->base, TB_STATE_OK, hstream->priv);

    // trace
    tb_trace_d("clos: notify: ok");
}
static tb_bool_t tb_async_stream_http_clos(tb_handle_t astream, tb_async_stream_clos_func_t func, tb_cpointer_t priv)
{   
    // check
    tb_async_stream_http_t* hstream = tb_async_stream_http_cast(astream);
    tb_assert_and_check_return_val(hstream && hstream->http && func, tb_false);

    // trace
    tb_trace_d("clos: ..");

    // init func
    hstream->func.clos  = func;
    hstream->priv       = priv;

    // close it
    return tb_aicp_http_clos(hstream->http, tb_async_stream_http_clos_func, hstream);
}
static tb_bool_t tb_async_stream_http_read_func(tb_handle_t http, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(http, tb_false);

    // the stream
    tb_async_stream_http_t* hstream = (tb_async_stream_http_t*)priv;
    tb_assert_and_check_return_val(hstream && hstream->func.read, tb_false);

    // ok?
    tb_bool_t bend = tb_false;
    if (state == TB_STATE_OK) 
    {
        // save offset
        tb_hize_t offset = tb_atomic64_add_and_fetch(&hstream->offset, real);

        // end? 
        tb_hong_t hsize = tb_atomic64_get(&hstream->size);
        if (hsize >= 0 && offset == hsize) bend = tb_true;
    }

    // done func
    tb_bool_t ok = hstream->func.read((tb_async_stream_t*)hstream, state, data, real, size, hstream->priv);

    // end? closed 
    if (ok && bend) ok = hstream->func.read((tb_async_stream_t*)hstream, TB_STATE_CLOSED, data, 0, size, hstream->priv);

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_http_read(tb_handle_t astream, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_http_t* hstream = tb_async_stream_http_cast(astream);
    tb_assert_and_check_return_val(hstream && hstream->http && func, tb_false);

    // end? closed
    tb_hong_t hsize = tb_atomic64_get(&hstream->size);
    if (hsize >= 0) 
    {
        tb_hize_t offset = tb_atomic64_get(&hstream->offset);
        if (offset == hsize)
        {
            func(astream, TB_STATE_CLOSED, tb_null, 0, size, priv);
            return tb_true;
        }
    }

    // save func and priv
    hstream->priv       = priv;
    hstream->func.read  = func;

    // post read
    return tb_aicp_http_read_after(hstream->http, delay, size, tb_async_stream_http_read_func, astream);
}
static tb_bool_t tb_async_stream_http_seek_func(tb_handle_t http, tb_size_t state, tb_hize_t offset, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(http, tb_false);

    // the stream
    tb_async_stream_http_t* hstream = (tb_async_stream_http_t*)priv;
    tb_assert_and_check_return_val(hstream && hstream->func.seek, tb_false);

    // save offset
    if (state == TB_STATE_OK) tb_atomic64_set(&hstream->offset, offset);

    // done func
    return hstream->func.seek((tb_async_stream_t*)hstream, state, offset, hstream->priv);
}
static tb_bool_t tb_async_stream_http_seek(tb_handle_t astream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_http_t* hstream = tb_async_stream_http_cast(astream);
    tb_assert_and_check_return_val(hstream && hstream->http && func, tb_false);

    // save func and priv
    hstream->priv       = priv;
    hstream->func.seek  = func;

    // post seek
    return tb_aicp_http_seek(hstream->http, offset, tb_async_stream_http_seek_func, astream);
}
static tb_bool_t tb_async_stream_http_task_func(tb_handle_t http, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(http, tb_false);

    // the stream
    tb_async_stream_http_t* hstream = (tb_async_stream_http_t*)priv;
    tb_assert_and_check_return_val(hstream && hstream->func.task, tb_false);

    // done func
    return hstream->func.task((tb_async_stream_t*)hstream, state, hstream->priv);
}
static tb_bool_t tb_async_stream_http_task(tb_handle_t astream, tb_size_t delay, tb_async_stream_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_http_t* hstream = tb_async_stream_http_cast(astream);
    tb_assert_and_check_return_val(hstream && hstream->http && func, tb_false);

    // save func and priv
    hstream->priv       = priv;
    hstream->func.task  = func;

    // post task
    return tb_aicp_http_task(hstream->http, delay, tb_async_stream_http_task_func, astream);
}
static tb_void_t tb_async_stream_http_kill(tb_handle_t astream)
{   
    // check
    tb_async_stream_http_t* hstream = tb_async_stream_http_cast(astream);
    tb_assert_and_check_return(hstream);

    // trace
    tb_trace_d("kill: ..");

    // kill it
    if (hstream->http) tb_aicp_http_kill(hstream->http);
}
static tb_bool_t tb_async_stream_http_exit(tb_handle_t astream)
{   
    // check
    tb_async_stream_http_t* hstream = tb_async_stream_http_cast(astream);
    tb_assert_and_check_return_val(hstream, tb_false);

    // exit it
    if (hstream->http) tb_aicp_http_exit(hstream->http);
    hstream->http = tb_null;

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_http_ctrl(tb_handle_t astream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_async_stream_http_t* hstream = tb_async_stream_http_cast(astream);
    tb_assert_and_check_return_val(hstream && hstream->http, tb_false);

    // done
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_opened(astream) && hstream->http, tb_false);

            // get size
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize, tb_false);
            *psize = (tb_hong_t)tb_atomic64_get(&hstream->size);
            return tb_true;
        }
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_opened(astream) && hstream->http, tb_false);

            // get offset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);
            *poffset = (tb_hize_t)tb_atomic64_get(&hstream->offset);
            return tb_true;
        }
    case TB_STREAM_CTRL_SET_URL:
        {
            // url
            tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(url, tb_false);
        
            // set url
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_URL, url);
        }
        break;
    case TB_STREAM_CTRL_GET_URL:
        {
            // purl
            tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            tb_assert_and_check_return_val(purl, tb_false);
    
            // get url
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_URL, purl);
        }
        break;
    case TB_STREAM_CTRL_SET_HOST:
        {
            // host
            tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(host, tb_false);
    
            // set host
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_HOST, host);
        }
        break;
    case TB_STREAM_CTRL_GET_HOST:
        {
            // phost
            tb_char_t const** phost = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            tb_assert_and_check_return_val(phost, tb_false); 

            // get host
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_HOST, phost);
        }
        break;
    case TB_STREAM_CTRL_SET_PORT:
        {
            // port
            tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
            tb_assert_and_check_return_val(port, tb_false);
    
            // set port
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_PORT, port);
        }
        break;
    case TB_STREAM_CTRL_GET_PORT:
        {
            // pport
            tb_size_t* pport = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pport, tb_false);
    
            // get port
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_PORT, pport);
        }
        break;
    case TB_STREAM_CTRL_SET_PATH:
        {
            // path
            tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(path, tb_false);
    
            // set path
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_PATH, path);
        }
        break;
    case TB_STREAM_CTRL_GET_PATH:
        {
            // ppath
            tb_char_t const** ppath = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            tb_assert_and_check_return_val(ppath, tb_false);
    
            // get path
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_PATH, ppath);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_METHOD:
        {
            // method
            tb_size_t method = (tb_size_t)tb_va_arg(args, tb_size_t);
    
            // set method
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_METHOD, method);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_METHOD:
        {
            // pmethod
            tb_size_t* pmethod = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pmethod, tb_false);
    
            // get method
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_METHOD, pmethod);
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
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_HEAD, key, val);
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
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_HEAD, key, pval);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_HEAD_FUNC:
        {
            // head_func
            tb_http_head_func_t head_func = (tb_http_head_func_t)tb_va_arg(args, tb_http_head_func_t);

            // set head_func
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_HEAD_FUNC, head_func);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_HEAD_FUNC:
        {
            // phead_func
            tb_http_head_func_t* phead_func = (tb_http_head_func_t*)tb_va_arg(args, tb_http_head_func_t*);
            tb_assert_and_check_return_val(phead_func, tb_false);

            // get head_func
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_HEAD_FUNC, phead_func);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_HEAD_PRIV:
        {
            // head_priv
            tb_pointer_t head_priv = (tb_pointer_t)tb_va_arg(args, tb_pointer_t);

            // set head_priv
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_HEAD_PRIV, head_priv);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_HEAD_PRIV:
        {
            // phead_priv
            tb_pointer_t* phead_priv = (tb_pointer_t*)tb_va_arg(args, tb_pointer_t*);
            tb_assert_and_check_return_val(phead_priv, tb_false);

            // get head_priv
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_HEAD_PRIV, phead_priv);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_RANGE:
        {
            tb_hize_t bof = (tb_hize_t)tb_va_arg(args, tb_hize_t);
            tb_hize_t eof = (tb_hize_t)tb_va_arg(args, tb_hize_t);
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_RANGE, bof, eof);
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
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_RANGE, pbof, peof);
        }
        break;
    case TB_STREAM_CTRL_SET_SSL:
        {
            // bssl
            tb_bool_t bssl = (tb_bool_t)tb_va_arg(args, tb_bool_t);
    
            // set ssl
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_SSL, bssl);
        }
        break;
    case TB_STREAM_CTRL_GET_SSL:
        {
            // pssl
            tb_bool_t* pssl = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
            tb_assert_and_check_return_val(pssl, tb_false);

            // get ssl
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_SSL, pssl);
        }
        break;
    case TB_STREAM_CTRL_SET_TIMEOUT:
        {
            // timeout
            tb_size_t timeout = (tb_size_t)tb_va_arg(args, tb_size_t);
            tb_assert_and_check_return_val(timeout, tb_false);
    
            // set timeout
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_TIMEOUT, timeout);
        }
        break;
    case TB_STREAM_CTRL_GET_TIMEOUT:
        {
            // ptimeout
            tb_size_t* ptimeout = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(ptimeout, tb_false);
    
            // get timeout
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_TIMEOUT, ptimeout);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_POST_URL:
        {
            // url
            tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(url, tb_false);
            
            // set url
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_URL, url);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_POST_URL:
        {
            // purl
            tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            tb_assert_and_check_return_val(purl, tb_false);

            // get url
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_URL, purl);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_POST_DATA:
        {
            // post data
            tb_byte_t const*    data = (tb_byte_t const*)tb_va_arg(args, tb_byte_t const*);

            // post size
            tb_size_t           size = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set post data
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_DATA, data, size);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_POST_DATA:
        {
            // pdata and psize
            tb_byte_t const**   pdata = (tb_byte_t const**)tb_va_arg(args, tb_byte_t const**);
            tb_size_t*          psize = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pdata && psize, tb_false);

            // get post data and size
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_DATA, pdata, psize);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_POST_FUNC:
        {
            // func
            tb_http_post_func_t func = (tb_http_post_func_t)tb_va_arg(args, tb_http_post_func_t);

            // set post func
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_FUNC, func);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_POST_FUNC:
        {
            // pfunc
            tb_http_post_func_t* pfunc = (tb_http_post_func_t*)tb_va_arg(args, tb_http_post_func_t*);
            tb_assert_and_check_return_val(pfunc, tb_false);

            // get post func
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_FUNC, pfunc);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_POST_PRIV:
        {
            // post priv
            tb_cpointer_t priv = (tb_pointer_t)tb_va_arg(args, tb_pointer_t);

            // set post priv
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_PRIV, priv);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_POST_PRIV:
        {
            // ppost priv
            tb_pointer_t* ppriv = (tb_pointer_t*)tb_va_arg(args, tb_pointer_t*);
            tb_assert_and_check_return_val(ppriv, tb_false);

            // get post priv
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_PRIV, ppriv);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_POST_LRATE:
        {
            // post lrate
            tb_size_t lrate = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set post lrate
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_POST_LRATE, lrate);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_POST_LRATE:
        {
            // ppost lrate
            tb_size_t* plrate = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(plrate, tb_false);

            // get post lrate
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_POST_LRATE, plrate);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_AUTO_UNZIP:
        {
            // bunzip
            tb_bool_t bunzip = (tb_bool_t)tb_va_arg(args, tb_bool_t);

            // set bunzip
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_AUTO_UNZIP, bunzip);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_AUTO_UNZIP:
        {
            // pbunzip
            tb_bool_t* pbunzip = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
            tb_assert_and_check_return_val(pbunzip, tb_false);

            // get bunzip
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_AUTO_UNZIP, pbunzip);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_REDIRECT:
        {
            // redirect
            tb_size_t redirect = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set redirect
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_REDIRECT, redirect);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_REDIRECT:
        {
            // predirect
            tb_size_t* predirect = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(predirect, tb_false);

            // get redirect
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_REDIRECT, predirect);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_VERSION:
        {
            // version
            tb_size_t version = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set version
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_VERSION, version);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_VERSION:
        {
            // pversion
            tb_size_t* pversion = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pversion, tb_false);

            // get version
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_VERSION, pversion);
        }
        break;
    case TB_STREAM_CTRL_HTTP_SET_COOKIES:
        {
            // cookies
            tb_handle_t cookies = (tb_handle_t)tb_va_arg(args, tb_handle_t);

            // set cookies
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_SET_COOKIES, cookies);
        }
        break;
    case TB_STREAM_CTRL_HTTP_GET_COOKIES:
        {
            // pcookies
            tb_handle_t* pcookies = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
            tb_assert_and_check_return_val(pcookies, tb_false);

            // get version
            return tb_aicp_http_option(hstream->http, TB_HTTP_OPTION_GET_COOKIES, pcookies);
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
tb_async_stream_t* tb_async_stream_init_http(tb_aicp_t* aicp)
{
    // check
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_async_stream_http_t* hstream = tb_null;
    do
    {
        // make stream
        hstream = (tb_async_stream_http_t*)tb_malloc0(sizeof(tb_async_stream_http_t));
        tb_assert_and_check_break(hstream);

        // init stream
        if (!tb_async_stream_init((tb_async_stream_t*)hstream, aicp, TB_STREAM_TYPE_HTTP, 0, 0)) break;
        hstream->base.open      = tb_async_stream_http_open;
        hstream->base.read      = tb_async_stream_http_read;
        hstream->base.seek      = tb_async_stream_http_seek;
        hstream->base.task      = tb_async_stream_http_task;
        hstream->base.clos      = tb_async_stream_http_clos;
        hstream->base.exit      = tb_async_stream_http_exit;
        hstream->base.base.kill = tb_async_stream_http_kill;
        hstream->base.base.ctrl = tb_async_stream_http_ctrl;

        // init http
        hstream->http = tb_aicp_http_init(aicp);
        tb_assert_and_check_break(hstream->http);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (hstream) tb_async_stream_exit((tb_async_stream_t*)hstream);
        hstream = tb_null;
    }

    // ok
    return (tb_async_stream_t*)hstream;
}
tb_async_stream_t* tb_async_stream_init_from_http(tb_aicp_t* aicp, tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl)
{
    // check
    tb_assert_and_check_return_val(aicp && host && port && path, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  hstream = tb_null;
    do
    {
        // init stream
        hstream = tb_async_stream_init_http(aicp);
        tb_assert_and_check_break(hstream);

        // ctrl
        if (!tb_stream_ctrl(hstream, TB_STREAM_CTRL_SET_HOST, host)) break;
        if (!tb_stream_ctrl(hstream, TB_STREAM_CTRL_SET_PORT, port)) break;
        if (!tb_stream_ctrl(hstream, TB_STREAM_CTRL_SET_PATH, path)) break;
        if (!tb_stream_ctrl(hstream, TB_STREAM_CTRL_SET_SSL, bssl)) break;
    
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (hstream) tb_async_stream_exit(hstream);
        hstream = tb_null;
    }

    // ok
    return hstream;
}

