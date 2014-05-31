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
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "http"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "http.h"
#include "../asio/asio.h"
#include "../zip/zip.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../stream/stream.h"
#include "../platform/platform.h"
#include "../algorithm/algorithm.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the http type
typedef struct __tb_http_t
{
    // the option
    tb_http_option_t    option;

    // the status 
    tb_http_status_t    status;

    // the stream
    tb_basic_stream_t*  stream;

    // the sstream for sock
    tb_basic_stream_t*  sstream;

    // the cstream for chunked
    tb_basic_stream_t*  cstream;

    // the zstream for gzip/deflate
    tb_basic_stream_t*  zstream;

    // is opened?
    tb_bool_t           bopened;

    // the pool for string
    tb_handle_t         pool;

    // the request data
    tb_scoped_string_t  request;

    // the cookies
    tb_scoped_string_t  cookies;

}tb_http_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_char_t const* g_http_methods[] = 
{
    "GET"
,   "POST"
,   "HEAD"
,   "PUT"
,   "OPTIONS"
,   "DELETE"
,   "TRACE"
,   "CONNECT"
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_http_option_init(tb_http_t* http)
{
    // init option using the default value
    http->option.method     = TB_HTTP_METHOD_GET;
    http->option.redirect   = TB_HTTP_DEFAULT_REDIRECT;
    http->option.timeout    = TB_HTTP_DEFAULT_TIMEOUT;
    http->option.version    = 1; // HTTP/1.1
    http->option.bunzip     = 0;
    http->option.cookies    = tb_null;

    // init url
    if (!tb_url_init(&http->option.url)) return tb_false;

    // init post url
    if (!tb_url_init(&http->option.post_url)) return tb_false;

    // init head
    http->option.head = tb_hash_init(8, tb_item_func_str(tb_false, http->pool), tb_item_func_str(tb_false, http->pool));
    tb_assert_and_check_return_val(http->option.head, tb_false);

    // ok
    return tb_true;
}
static tb_void_t tb_http_option_exit(tb_http_t* http)
{
    // exit head
    if (http->option.head) tb_hash_exit(http->option.head);
    http->option.head = tb_null;

    // exit url
    tb_url_exit(&http->option.url);

    // exit post url
    tb_url_exit(&http->option.post_url);

    // clear cookies
    http->option.cookies = tb_null;
}
#ifdef __tb_debug__
static tb_void_t tb_http_option_dump(tb_http_t* http)
{
    // check
    tb_assert_and_check_return(http);

    // dump option
    tb_trace_i("======================================================================");
    tb_trace_i("option: ");
    tb_trace_i("option: url: %s",               tb_url_get(&http->option.url));
    tb_trace_i("option: version: HTTP/1.%1u",   http->option.version);
    tb_trace_i("option: method: %s",            http->option.method < tb_arrayn(g_http_methods)? g_http_methods[http->option.method] : "none");
    tb_trace_i("option: redirect: %d",          http->option.redirect);
    tb_trace_i("option: range: %llu-%llu",      http->option.range.bof, http->option.range.eof);
    tb_trace_i("option: bunzip: %s",            http->option.bunzip? "true" : "false");

    // dump head 
    tb_for_all (tb_hash_item_t*, item, http->option.head)
    {
        if (item) tb_trace_i("option: head: %s: %s", item->name, item->data);
    }

    // dump end
    tb_trace_i("");
}
#endif
static tb_bool_t tb_http_status_init(tb_http_t* http)
{
    // init status using the default value
    http->status.version = 1;

    // init content type 
    if (!tb_scoped_string_init(&http->status.content_type)) return tb_false;

    // init location
    if (!tb_scoped_string_init(&http->status.location)) return tb_false;
    return tb_true;
}
static tb_void_t tb_http_status_exit(tb_http_t* http)
{
    // exit the content type
    tb_scoped_string_exit(&http->status.content_type);

    // exit location
    tb_scoped_string_exit(&http->status.location);
}
static tb_void_t tb_http_status_cler(tb_http_t* http, tb_bool_t host_changed)
{
    // clear status
    http->status.code = 0;
    http->status.bgzip = 0;
    http->status.bdeflate = 0;
    http->status.bchunked = 0;
    http->status.content_size = -1;
    http->status.document_size = -1;
    http->status.state = TB_STATE_OK;

    // clear content type
    tb_scoped_string_clear(&http->status.content_type);

    // clear location
    tb_scoped_string_clear(&http->status.location);

    // host is changed? clear the alived state
    if (host_changed)
    {
        http->status.version = 1;
        http->status.balived = 0;
        http->status.bseeked = 0;
    }
}
#ifdef __tb_debug__
static tb_void_t tb_http_status_dump(tb_http_t* http)
{
    // check
    tb_assert_and_check_return(http);

    // dump status
    tb_trace_i("======================================================================");
    tb_trace_i("status: ");
    tb_trace_i("status: code: %d", http->status.code);
    tb_trace_i("status: version: HTTP/1.%1u", http->status.version);
    tb_trace_i("status: content:type: %s", tb_scoped_string_cstr(&http->status.content_type));
    tb_trace_i("status: content:size: %lld", http->status.content_size);
    tb_trace_i("status: document:size: %lld", http->status.document_size);
    tb_trace_i("status: location: %s", tb_scoped_string_cstr(&http->status.location));
    tb_trace_i("status: bgzip: %s", http->status.bgzip? "true" : "false");
    tb_trace_i("status: bdeflate: %s", http->status.bdeflate? "true" : "false");
    tb_trace_i("status: balived: %s", http->status.balived? "true" : "false");
    tb_trace_i("status: bseeked: %s", http->status.bseeked? "true" : "false");
    tb_trace_i("status: bchunked: %s", http->status.bchunked? "true" : "false");

    // dump end
    tb_trace_i("");
}
#endif
static tb_bool_t tb_http_connect(tb_http_t* http)
{
    // check
    tb_assert_and_check_return_val(http && http->stream, tb_false);
    
    // done
    tb_bool_t ok = tb_false;
    do
    {
        // the host is changed?
        tb_bool_t           host_changed = tb_true;
        tb_char_t const*    host_old = tb_null;
        tb_char_t const*    host_new = tb_url_host_get(&http->option.url);
        tb_stream_ctrl(http->stream, TB_STREAM_CTRL_GET_HOST, &host_old);
        if (host_old && host_new && !tb_stricmp(host_old, host_new)) host_changed = tb_false;

        // trace
        tb_trace_d("connect: host: %s", host_changed? "changed" : "keep");

        // ctrl stream
        if (!tb_stream_ctrl(http->stream, TB_STREAM_CTRL_SET_URL, tb_url_get(&http->option.url))) break;
        if (!tb_stream_ctrl(http->stream, TB_STREAM_CTRL_SET_TIMEOUT, http->option.timeout)) break;

        // dump option
#if defined(__tb_debug__) && TB_TRACE_MODULE_DEBUG
        tb_http_option_dump(http);
#endif
        
        // trace
        tb_trace_d("connect: ..");

        // clear status
        tb_http_status_cler(http, host_changed);

        // open stream
        if (!tb_basic_stream_open(http->stream)) break;

        // trace
        tb_trace_d("connect: ok");

        // ok
        ok = tb_true;

    } while (0);


    // failed? save state
    if (!ok) http->status.state = tb_basic_stream_state(http->stream);

    // ok?
    return ok;
}
static tb_bool_t tb_http_request_post(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // check
    tb_http_t* http = (tb_http_t*)priv;
    tb_assert_and_check_return_val(http && http->stream, tb_false);

    // trace
    tb_trace_d("post: percent: %llu%%, size: %lu, state: %s", size > 0? (offset * 100 / size) : 0, save, tb_state_cstr(state));

    // done func
    if (http->option.post_func && !http->option.post_func(http, state, offset, size, save, rate, http->option.post_priv)) 
        return tb_false;

    // ok?
    return tb_true;
}
static tb_bool_t tb_http_request(tb_http_t* http)
{
    // check
    tb_assert_and_check_return_val(http && http->stream, tb_false);

    // done
    tb_bool_t       ok = tb_false;
    tb_basic_stream_t*  pstream = tb_null;
    tb_hong_t       post_size = 0;
    do
    {
        // clear line data
        tb_scoped_string_clear(&http->request);

        // init the head value
        tb_char_t           data[64];
        tb_static_string_t  value;
        if (!tb_static_string_init(&value, data, 64)) break;

        // init method
        tb_assert_and_check_break(http->option.method < tb_arrayn(g_http_methods));
        tb_char_t const* method = g_http_methods[http->option.method];
        tb_assert_and_check_break(method);

        // init path
        tb_char_t const* path = tb_url_path_get(&http->option.url);
        tb_assert_and_check_break(path);

        // init args
        tb_char_t const* args = tb_url_args_get(&http->option.url);

        // init host
        tb_char_t const* host = tb_url_host_get(&http->option.url);
        tb_assert_and_check_break(host);
        tb_hash_set(http->option.head, "Host", host);

        // init accept
        if (!tb_hash_get(http->option.head, "Accept")) 
            tb_hash_set(http->option.head, "Accept", "*/*");

        // init connection
        if (!tb_hash_get(http->option.head, "Connection")) 
            tb_hash_set(http->option.head, "Connection", "close");
        else if (http->status.balived) tb_hash_set(http->option.head, "Connection", "keep-alive");

        // init cookies
        if (http->option.cookies && !tb_hash_get(http->option.head, "Cookie"))
        {
            // the host
            tb_char_t const* host = tb_null;
            tb_http_option(http, TB_HTTP_OPTION_GET_HOST, &host);

            // the path
            tb_char_t const* path = tb_null;
            tb_http_option(http, TB_HTTP_OPTION_GET_PATH, &path);

            // is ssl?
            tb_bool_t bssl = tb_false;
            tb_http_option(http, TB_HTTP_OPTION_GET_SSL, &bssl);
                
            // set cookie
            if (tb_cookies_get(http->option.cookies, host, path, bssl, &http->cookies))
                tb_hash_set(http->option.head, "Cookie", tb_scoped_string_cstr(&http->cookies));
        }

        // init range
        if (http->option.range.bof && http->option.range.eof >= http->option.range.bof)
            tb_static_string_cstrfcpy(&value, "bytes=%llu-%llu", http->option.range.bof, http->option.range.eof);
        else if (http->option.range.bof && !http->option.range.eof)
            tb_static_string_cstrfcpy(&value, "bytes=%llu-", http->option.range.bof);
        else if (!http->option.range.bof && http->option.range.eof)
            tb_static_string_cstrfcpy(&value, "bytes=0-%llu", http->option.range.eof);
        else if (http->option.range.bof > http->option.range.eof)
        {
            http->status.state = TB_STATE_HTTP_RANGE_INVALID;
            break;
        }

        if (tb_static_string_size(&value)) 
            tb_hash_set(http->option.head, "Range", tb_static_string_cstr(&value));

        // init post
        if (http->option.method == TB_HTTP_METHOD_POST)
        {
            // done
            tb_bool_t post_ok = tb_false;
            do
            {
                // init pstream
                tb_char_t const* url = tb_url_get(&http->option.post_url);
                if (http->option.post_data && http->option.post_size)
                    pstream = tb_basic_stream_init_from_data(http->option.post_data, http->option.post_size);
                else if (url) pstream = tb_basic_stream_init_from_url(url);
                tb_assert_and_check_break(pstream);

                // open pstream
                if (!tb_basic_stream_open(pstream)) break;

                // the post size
                post_size = tb_stream_size(pstream);
                tb_assert_and_check_break(post_size >= 0);

                // append post size
                tb_static_string_cstrfcpy(&value, "%lld", post_size);
                tb_hash_set(http->option.head, "Content-Length", tb_static_string_cstr(&value));

                // ok
                post_ok = tb_true;

            } while (0);

            // init post failed?
            if (!post_ok) 
            {
                http->status.state = TB_STATE_HTTP_POST_FAILED;
                break;
            }
        }

        // check head
        tb_assert_and_check_break(tb_hash_size(http->option.head));

        // append method
        tb_scoped_string_cstrcat(&http->request, method);

        // append ' '
        tb_scoped_string_chrcat(&http->request, ' ');

        // append path
        tb_scoped_string_cstrcat(&http->request, path);

        // append args if exists
        if (args) 
        {
            tb_scoped_string_chrcat(&http->request, '?');
            tb_scoped_string_cstrcat(&http->request, args);
        }

        // append ' '
        tb_scoped_string_chrcat(&http->request, ' ');

        // append version, HTTP/1.1
        tb_scoped_string_cstrfcat(&http->request, "HTTP/1.%1u\r\n", http->status.balived? http->status.version : http->option.version);

        // append key: value
        tb_for_all (tb_hash_item_t*, item, http->option.head)
        {
            if (item && item->name && item->data) 
                tb_scoped_string_cstrfcat(&http->request, "%s: %s\r\n", (tb_char_t const*)item->name, (tb_char_t const*)item->data);
        }

        // append end
        tb_scoped_string_cstrcat(&http->request, "\r\n");

        // exit the head value
        tb_static_string_exit(&value);

        // the head data and size
        tb_char_t const*    head_data = tb_scoped_string_cstr(&http->request);
        tb_size_t           head_size = tb_scoped_string_size(&http->request);
        tb_assert_and_check_break(head_data && head_size);
        
        // trace
        tb_trace_d("request[%lu]:\n%s", head_size, head_data);

        // writ request
        if (!tb_basic_stream_bwrit(http->stream, (tb_byte_t const*)head_data, head_size)) break;

        // writ post
        if (http->option.method == TB_HTTP_METHOD_POST)
        {
            // post stream
            if (tb_basic_transfer_done(pstream, http->stream, http->option.post_lrate, tb_http_request_post, http) != post_size)
            {
                http->status.state = TB_STATE_HTTP_POST_FAILED;
                break;
            }
        }

        // sync request
        if (!tb_basic_stream_sync(http->stream, tb_false)) break;
    
        // ok
        ok = tb_true;
    }
    while (0);

    // failed?
    if (!ok && !http->status.state) http->status.state = TB_STATE_HTTP_REQUEST_FAILED;

    // exit pstream
    if (pstream) tb_basic_stream_exit(pstream);
    pstream = tb_null;

    // ok?
    return ok;
}
/*
 * HTTP/1.1 206 Partial Content
 * Date: Fri, 23 Apr 2010 05:25:45 GMT
 * Server: Apache/2.2.9 (Ubuntu) PHP/5.2.6-2ubuntu4.5 with Suhosin-Patch
 * Last-Modified: Mon, 08 Mar 2010 09:58:09 GMT
 * ETag: "6cc014-8f47f-481471a322e40"
 * Accept-Ranges: bytes
 * Content-Length: 586879
 * Content-Range: bytes 0-586878/586879
 * Connection: close
 * Content-Type: application/x-shockwave-flash
 */
static tb_bool_t tb_http_response_done(tb_http_t* http, tb_char_t const* line, tb_size_t indx)
{
    // check
    tb_assert_and_check_return_val(http && http->sstream && line, tb_false);

    // the first line? 
    tb_char_t const* p = line;
    if (!indx)
    {
        // seek to the http version
        while (*p && *p != '.') p++; 
        tb_assert_and_check_return_val(*p, tb_false);
        p++;

        // parse version
        tb_assert_and_check_return_val((*p - '0') < 2, tb_false);
        http->status.version = *p - '0';
    
        // seek to the http code
        p++; while (tb_isspace(*p)) p++;

        // parse code
        tb_assert_and_check_return_val(*p && tb_isdigit(*p), tb_false);
        http->status.code = tb_stou32(p);

        // save state
        if (http->status.code == 200 || http->status.code == 206)
            http->status.state = TB_STATE_OK;
        else if (http->status.code == 204)
            http->status.state = TB_STATE_HTTP_RESPONSE_204;
        else if (http->status.code >= 300 && http->status.code <= 307)
            http->status.state = TB_STATE_HTTP_RESPONSE_300 + (http->status.code - 300);
        else if (http->status.code >= 400 && http->status.code <= 416)
            http->status.state = TB_STATE_HTTP_RESPONSE_400 + (http->status.code - 400);
        else if (http->status.code >= 500 && http->status.code <= 507)
            http->status.state = TB_STATE_HTTP_RESPONSE_500 + (http->status.code - 500);
        else http->status.state = TB_STATE_HTTP_RESPONSE_UNK;

        // check state code: 4xx & 5xx
        if (http->status.code >= 400 && http->status.code < 600) return tb_false;
    }
    // key: value?
    else
    {
        // seek to value
        while (*p && *p != ':') p++;
        tb_assert_and_check_return_val(*p, tb_false);
        p++; while (*p && tb_isspace(*p)) p++;

        // no value
        tb_check_return_val(*p, tb_true);

        // parse content size
        if (!tb_strnicmp(line, "Content-Length", 14))
        {
            http->status.content_size = tb_stou64(p);
            if (http->status.document_size < 0) 
                http->status.document_size = http->status.content_size;
        }
        // parse content range: "bytes $from-$to/$document_size"
        else if (!tb_strnicmp(line, "Content-Range", 13))
        {
            tb_hize_t from = 0;
            tb_hize_t to = 0;
            tb_hize_t document_size = 0;
            if (!tb_strncmp(p, "bytes ", 6)) 
            {
                p += 6;
                from = tb_stou64(p);
                while (*p && *p != '-') p++;
                if (*p && *p++ == '-') to = tb_stou64(p);
                while (*p && *p != '/') p++;
                if (*p && *p++ == '/') document_size = tb_stou64(p);
            }
            // no stream, be able to seek
            http->status.bseeked = 1;
            http->status.document_size = document_size;
            if (http->status.content_size < 0) 
            {
                if (from && to > from) http->status.content_size = to - from;
                else if (!from && to) http->status.content_size = to;
                else if (from && !to && document_size > from) http->status.content_size = document_size - from;
                else http->status.content_size = document_size;
            }
        }
        // parse accept-ranges: "bytes "
        else if (!tb_strnicmp(line, "Accept-Ranges", 13))
        {
            // no stream, be able to seek
            http->status.bseeked = 1;
        }
        // parse content type
        else if (!tb_strnicmp(line, "Content-Type", 12)) 
        {
            tb_scoped_string_cstrcpy(&http->status.content_type, p);
            tb_assert_and_check_return_val(tb_scoped_string_size(&http->status.content_type), tb_false);
        }
        // parse transfer encoding
        else if (!tb_strnicmp(line, "Transfer-Encoding", 17))
        {
            if (!tb_stricmp(p, "chunked")) http->status.bchunked = 1;
        }
        // parse content encoding
        else if (!tb_strnicmp(line, "Content-Encoding", 16))
        {
            if (!tb_stricmp(p, "gzip")) http->status.bgzip = 1;
            else if (!tb_stricmp(p, "deflate")) http->status.bdeflate = 1;
        }
        // parse location
        else if (!tb_strnicmp(line, "Location", 8)) 
        {
            // redirect? check code: 301 - 307
            tb_assert_and_check_return_val(http->status.code > 300 && http->status.code < 308, tb_false);

            // save location
            tb_scoped_string_cstrcpy(&http->status.location, p);
        }
        // parse connection
        else if (!tb_strnicmp(line, "Connection", 10))
        {
            // keep alive?
            http->status.balived = !tb_stricmp(p, "close")? 0 : 1;

            // ctrl stream for sock
            if (!tb_stream_ctrl(http->sstream, TB_STREAM_CTRL_SOCK_KEEP_ALIVE, http->status.balived? tb_true : tb_false)) return tb_false;
        }
        // parse cookies
        else if (http->option.cookies && !tb_strnicmp(line, "Set-Cookie", 10))
        {
            // the host
            tb_char_t const* host = tb_null;
            tb_http_option(http, TB_HTTP_OPTION_GET_HOST, &host);

            // the path
            tb_char_t const* path = tb_null;
            tb_http_option(http, TB_HTTP_OPTION_GET_PATH, &path);

            // is ssl?
            tb_bool_t bssl = tb_false;
            tb_http_option(http, TB_HTTP_OPTION_GET_SSL, &bssl);
                
            // set cookies
            tb_cookies_set(http->option.cookies, host, path, bssl, p);
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_http_response(tb_http_t* http)
{
    // check
    tb_assert_and_check_return_val(http && http->stream, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // read line
        tb_char_t line[8192];
        tb_long_t real = 0;
        tb_size_t indx = 0;
        while ((real = tb_basic_stream_bread_line(http->stream, line, sizeof(line) - 1)) >= 0)
        {
            // trace
            tb_trace_d("response: %s", line);
 
            // do callback
            if (http->option.head_func && !http->option.head_func((tb_handle_t)http, line, http->option.head_priv)) break;
            
            // end?
            if (!real)
            {
                // switch to cstream if chunked
                if (http->status.bchunked)
                {
                    // init cstream
                    if (http->cstream)
                    {
                        if (!tb_stream_ctrl(http->cstream, TB_STREAM_CTRL_FLTR_SET_STREAM, http->stream)) break;
                    }
                    else http->cstream = tb_basic_stream_init_filter_from_chunked(http->stream, tb_true);
                    tb_assert_and_check_break(http->cstream);

                    // open cstream, need not async
                    if (!tb_basic_stream_open(http->cstream)) break;

                    // using cstream
                    http->stream = http->cstream;

                    // disable seek
                    http->status.bseeked = 0;
                }

                // switch to zstream if gzip or deflate
                if (http->option.bunzip && (http->status.bgzip || http->status.bdeflate))
                {
#ifdef TB_CONFIG_THIRD_HAVE_ZLIB
                    // init zstream
                    if (http->zstream)
                    {
                        if (!tb_stream_ctrl(http->zstream, TB_STREAM_CTRL_FLTR_SET_STREAM, http->stream)) break;
                    }
                    else http->zstream = tb_basic_stream_init_filter_from_zip(http->stream, http->status.bgzip? TB_ZIP_ALGO_GZIP : TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE);
                    tb_assert_and_check_break(http->zstream);

                    // the filter
                    tb_stream_filter_t* filter = tb_null;
                    if (!tb_stream_ctrl(http->zstream, TB_STREAM_CTRL_FLTR_GET_FILTER, &filter)) break;
                    tb_assert_and_check_break(filter);

                    // ctrl filter
                    if (!tb_stream_filter_ctrl(filter, TB_STREAM_FILTER_CTRL_ZIP_SET_ALGO, http->status.bgzip? TB_ZIP_ALGO_GZIP : TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE)) break;

                    // limit the filter input size
                    if (http->status.content_size > 0) tb_stream_filter_limit(filter, http->status.content_size);

                    // open zstream, need not async
                    if (!tb_basic_stream_open(http->zstream)) break;

                    // using zstream
                    http->stream = http->zstream;

                    // disable seek
                    http->status.bseeked = 0;
#else
                    // trace
                    tb_trace_w("gzip is not supported now! please enable it from config if you need it.");

                    // not supported
                    http->status.state = TB_STATE_HTTP_GZIP_NOT_SUPPORTED;
                    break;
#endif
                }

                // trace
                tb_trace_d("response: ok");

                // dump status
#if defined(__tb_debug__) && TB_TRACE_MODULE_DEBUG
                tb_http_status_dump(http);
#endif

                // ok
                ok = tb_true;
                break;
            }

            // done it
            if (!tb_http_response_done(http, line, indx++)) break;
        }

    } while (0);

    // ok?
    return ok;
}
static tb_bool_t tb_http_redirect(tb_http_t* http)
{
    // check
    tb_assert_and_check_return_val(http && http->stream, tb_false);

    // done
    tb_size_t i = 0;
    for (i = 0; i < http->option.redirect && tb_scoped_string_size(&http->status.location); i++)
    {
        // read the redirect content
        if (http->status.content_size > 0)
        {
            tb_byte_t data[TB_BASIC_STREAM_BLOCK_MAXN];
            tb_hize_t read = 0;
            tb_hize_t size = http->status.content_size;
            while (read < size) 
            {
                // the need
                tb_size_t need = tb_min(size - read, TB_BASIC_STREAM_BLOCK_MAXN);

                // read it
                if (!tb_basic_stream_bread(http->stream, data, need)) break;

                // save size
                read += need;
            }

            // check
            tb_assert_and_check_break(read == size);
        }

        // close stream
        if (http->stream && !tb_basic_stream_clos(http->stream)) break;

        // switch to sstream
        http->stream = http->sstream;

        // done location url
        tb_char_t const* location = tb_scoped_string_cstr(&http->status.location);
        tb_assert_and_check_break(location);

        // trace
        tb_trace_d("redirect: %s", location);

        // only path?
        if (tb_url_protocol_probe(location) == TB_URL_PROTOCOL_FILE) tb_url_path_set(&http->option.url, location);
        // full url?
        else
        {
            // set url
            if (!tb_url_set(&http->option.url, location)) break;
        }

        // connect it
        if (!tb_http_connect(http)) break;

        // request it
        if (!tb_http_request(http)) break;

        // response it
        if (!tb_http_response(http)) break;
    }

    // ok?
    return (i < http->option.redirect && tb_scoped_string_size(&http->status.location))? tb_false : tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t tb_http_init()
{
    // done
    tb_bool_t   ok = tb_false;
    tb_http_t*  http = tb_null;
    do
    {
        // make http
        http = tb_malloc0(sizeof(tb_http_t));
        tb_assert_and_check_break(http);

        // init stream
        http->stream = http->sstream = tb_basic_stream_init_sock();
        tb_assert_and_check_break(http->stream);

        // init pool
        http->pool = tb_block_pool_init(TB_BLOCK_POOL_GROW_MICRO, 0);
        tb_assert_and_check_break(http->pool);

        // init request data
        if (!tb_scoped_string_init(&http->request)) break;

        // init cookies data
        if (!tb_scoped_string_init(&http->cookies)) break;

        // init option
        if (!tb_http_option_init(http)) break;

        // init status
        if (!tb_http_status_init(http)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (http) tb_http_exit(http);
        http = tb_null;
    }

    // ok?
    return http;
}
tb_void_t tb_http_kill(tb_handle_t handle)
{
    // check
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return(http);

    // kill stream
    if (http->stream) tb_stream_kill(http->stream);
}
tb_void_t tb_http_exit(tb_handle_t handle)
{
    // check
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return(http);

    // close it
    tb_http_clos(handle);

    // exit zstream
    if (http->zstream) tb_basic_stream_exit(http->zstream);
    http->zstream = tb_null;

    // exit cstream
    if (http->cstream) tb_basic_stream_exit(http->cstream);
    http->cstream = tb_null;

    // exit sstream
    if (http->sstream) tb_basic_stream_exit(http->sstream);
    http->sstream = tb_null;

    // exit stream
    http->stream = tb_null;
    
    // exit status
    tb_http_status_exit(http);

    // exit option
    tb_http_option_exit(http);

    // exit cookies data
    tb_scoped_string_exit(&http->cookies);

    // exit request data
    tb_scoped_string_exit(&http->request);

    // exit pool
    if (http->pool) tb_block_pool_exit(http->pool);
    http->pool = tb_null;

    // free it
    tb_free(http);
}
tb_long_t tb_http_wait(tb_handle_t handle, tb_size_t aioe, tb_long_t timeout)
{
    // check
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return_val(http && http->stream, -1);

    // opened?
    tb_assert_and_check_return_val(http->bopened, -1);

    // wait it
    tb_long_t wait = tb_basic_stream_wait(http->stream, aioe, timeout);

    // failed? save state
    if (wait < 0 && !http->status.state) http->status.state = tb_basic_stream_state(http->stream);

    // ok?
    return wait;
}
tb_bool_t tb_http_open(tb_handle_t handle)
{
    // check
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return_val(http, tb_false);
    
    // opened?
    tb_assert_and_check_return_val(!http->bopened, tb_false);

    // connect it
    if (!tb_http_connect(http)) return tb_false;

    // request it
    if (!tb_http_request(http)) return tb_false;

    // response it
    if (!tb_http_response(http)) return tb_false;

    // redirect it
    if (!tb_http_redirect(http)) return tb_false;

    // opened
    http->bopened = tb_true;

    // ok
    return tb_true;
}
tb_bool_t tb_http_clos(tb_handle_t handle)
{
    // check
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return_val(http, tb_false);
    
    // opened?
    tb_check_return_val(http->bopened, tb_true);

    // close stream
    if (http->stream && !tb_basic_stream_clos(http->stream)) return tb_false;

    // switch to sstream
    http->stream = http->sstream;

    // clear opened
    http->bopened = tb_false;

    // ok
    return tb_true;
}
tb_bool_t tb_http_seek(tb_handle_t handle, tb_hize_t offset)
{
    // check
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return_val(http, tb_false);

    // opened?
    tb_assert_and_check_return_val(http->bopened, tb_false);

    // seeked?
    tb_check_return_val(http->status.bseeked, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // close stream
        if (http->stream && !tb_basic_stream_clos(http->stream)) break;

        // switch to sstream
        http->stream = http->sstream;

        // trace
        tb_trace_d("seek: %llu", offset);

        // set range
        http->option.range.bof = offset;
        http->option.range.eof = http->status.document_size > 0? http->status.document_size - 1 : 0;

        // connect it
        if (!tb_http_connect(http)) break;

        // request it
        if (!tb_http_request(http)) break;

        // response it
        if (!tb_http_response(http)) break;

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
tb_long_t tb_http_read(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return_val(http && http->stream, -1);

    // opened?
    tb_assert_and_check_return_val(http->bopened, -1);

    // read
    return tb_basic_stream_read(http->stream, data, size);
}
tb_bool_t tb_http_bread(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{   
    // check
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return_val(http && http->stream, tb_false);

    // opened?
    tb_assert_and_check_return_val(http->bopened, tb_false);

    // read
    tb_long_t read = 0;
    while (read < size)
    {
        // read data
        tb_long_t real = tb_basic_stream_read(http->stream, data + read, size - read);

        // update size
        if (real > 0) read += real;
        // no data?
        else if (!real)
        {
            // wait
            tb_long_t e = tb_http_wait(handle, TB_AIOE_CODE_RECV, http->option.timeout);
            tb_assert_and_check_break(e >= 0);

            // timeout?
            tb_check_break(e);

            // has read?
            tb_assert_and_check_break(e & TB_AIOE_CODE_RECV);
        }
        else break;
    }

    // ok?
    return read == size? tb_true : tb_false;
}
tb_bool_t tb_http_option(tb_handle_t handle, tb_size_t option, ...)
{
    // check
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return_val(http && option, tb_false);

    // init args
    tb_va_list_t args;
    tb_va_start(args, option);

    // done
    switch (option)
    {
    case TB_HTTP_OPTION_SET_URL:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // url
            tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(url, tb_false);
            
            // set url
            if (tb_url_set(&http->option.url, url)) return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_URL:
        {
            // purl
            tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            tb_assert_and_check_return_val(purl, tb_false);

            // get url
            tb_char_t const* url = tb_url_get(&http->option.url);
            tb_assert_and_check_return_val(url, tb_false);

            // ok
            *purl = url;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_HOST:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // host
            tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(host, tb_false);

            // set host
            tb_url_host_set(&http->option.url, host);
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_HOST:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // phost
            tb_char_t const** phost = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            tb_assert_and_check_return_val(phost, tb_false); 

            // get host
            tb_char_t const* host = tb_url_host_get(&http->option.url);
            tb_assert_and_check_return_val(host, tb_false);

            // ok
            *phost = host;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_PORT:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // port
            tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
            tb_assert_and_check_return_val(port, tb_false);

            // set port
            tb_url_port_set(&http->option.url, port);
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_PORT:
        {
            // pport
            tb_size_t* pport = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pport, tb_false);

            // get port
            *pport = tb_url_port_get(&http->option.url);
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_PATH:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // path
            tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(path, tb_false);
 
            // set path
            tb_url_path_set(&http->option.url, path);
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_PATH:
        {
            // ppath
            tb_char_t const** ppath = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            tb_assert_and_check_return_val(ppath, tb_false);

            // get path
            tb_char_t const* path = tb_url_path_get(&http->option.url);
            tb_assert_and_check_return_val(path, tb_false);

            // ok
            *ppath = path;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_METHOD:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // method
            tb_size_t method = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set method
            http->option.method = method;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_METHOD:
        {
            // pmethod
            tb_size_t* pmethod = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pmethod, tb_false);

            // get method
            *pmethod = http->option.method;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_HEAD:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // check
            tb_assert_and_check_return_val(http->option.head, tb_false);

            // key
            tb_char_t const* key = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(key, tb_false);

            // val
            tb_char_t const* val = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(val, tb_false);
 
            // set head
            tb_hash_set(http->option.head, key, val);
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_HEAD:
        {
            // check
            tb_assert_and_check_return_val(http->option.head, tb_false);

            // key
            tb_char_t const* key = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(key, tb_false);

            // pval
            tb_char_t const** pval = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            tb_assert_and_check_return_val(pval, tb_false);

            // get val
            tb_char_t const* val = tb_hash_get(http->option.head, key);
            tb_assert_and_check_return_val(val, tb_false);

            // ok
            *pval = val;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_HEAD_FUNC:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // head_func
            tb_http_head_func_t head_func = (tb_http_head_func_t)tb_va_arg(args, tb_http_head_func_t);

            // set head_func
            http->option.head_func = head_func;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_HEAD_FUNC:
        {
            // phead_func
            tb_http_head_func_t* phead_func = (tb_http_head_func_t*)tb_va_arg(args, tb_http_head_func_t*);
            tb_assert_and_check_return_val(phead_func, tb_false);

            // get head_func
            *phead_func = http->option.head_func;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_HEAD_PRIV:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // head_priv
            tb_pointer_t head_priv = (tb_pointer_t)tb_va_arg(args, tb_pointer_t);

            // set head_priv
            http->option.head_priv = head_priv;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_HEAD_PRIV:
        {
            // phead_priv
            tb_pointer_t* phead_priv = (tb_pointer_t*)tb_va_arg(args, tb_pointer_t*);
            tb_assert_and_check_return_val(phead_priv, tb_false);

            // get head_priv
            *phead_priv = http->option.head_priv;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_RANGE:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // set range
            http->option.range.bof = (tb_hize_t)tb_va_arg(args, tb_hize_t);
            http->option.range.eof = (tb_hize_t)tb_va_arg(args, tb_hize_t);
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_RANGE:
        {
            // pbof
            tb_hize_t* pbof = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(pbof, tb_false);

            // peof
            tb_hize_t* peof = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(peof, tb_false);

            // ok
            *pbof = http->option.range.bof;
            *peof = http->option.range.eof;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_SSL:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // bssl
            tb_bool_t bssl = (tb_bool_t)tb_va_arg(args, tb_bool_t);

            // set ssl
            tb_url_ssl_set(&http->option.url, bssl);
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_SSL:
        {
            // pssl
            tb_bool_t* pssl = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
            tb_assert_and_check_return_val(pssl, tb_false);

            // get ssl
            *pssl = tb_url_ssl_get(&http->option.url);
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_TIMEOUT:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // the timeout
            tb_long_t timeout = (tb_long_t)tb_va_arg(args, tb_long_t);

            // set timeout
            http->option.timeout = timeout? timeout : TB_HTTP_DEFAULT_TIMEOUT;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_TIMEOUT:
        {
            // ptimeout
            tb_long_t* ptimeout = (tb_long_t*)tb_va_arg(args, tb_long_t*);
            tb_assert_and_check_return_val(ptimeout, tb_false);

            // get timeout
            *ptimeout = http->option.timeout;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_COOKIES:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // set cookies
            http->option.cookies = (tb_handle_t)tb_va_arg(args, tb_handle_t);
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_COOKIES:
        {
            // ptimeout
            tb_handle_t* pcookies = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
            tb_assert_and_check_return_val(pcookies, tb_false);

            // get cookies
            *pcookies = http->option.cookies;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_POST_URL:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // url
            tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(url, tb_false);

            // clear post data and size
            http->option.post_data = tb_null;
            http->option.post_size = 0;
            
            // set url
            if (tb_url_set(&http->option.post_url, url)) return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_POST_URL:
        {
            // purl
            tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            tb_assert_and_check_return_val(purl, tb_false);

            // get url
            tb_char_t const* url = tb_url_get(&http->option.post_url);
            tb_assert_and_check_return_val(url, tb_false);

            // ok
            *purl = url;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_POST_DATA:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // post data
            tb_byte_t const*    data = (tb_byte_t const*)tb_va_arg(args, tb_byte_t const*);

            // post size
            tb_size_t           size = (tb_size_t)tb_va_arg(args, tb_size_t);

            // clear post url
            tb_url_clear(&http->option.post_url);
            
            // set post data
            http->option.post_data = data;
            http->option.post_size = size;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_POST_DATA:
        {
            // pdata and psize
            tb_byte_t const**   pdata = (tb_byte_t const**)tb_va_arg(args, tb_byte_t const**);
            tb_size_t*          psize = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pdata && psize, tb_false);

            // get post data and size
            *pdata = http->option.post_data;
            *psize = http->option.post_size;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_POST_FUNC:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // func
            tb_http_post_func_t func = (tb_http_post_func_t)tb_va_arg(args, tb_http_post_func_t);

            // set post func
            http->option.post_func = func;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_POST_FUNC:
        {
            // pfunc
            tb_http_post_func_t* pfunc = (tb_http_post_func_t*)tb_va_arg(args, tb_http_post_func_t*);
            tb_assert_and_check_return_val(pfunc, tb_false);

            // get post func
            *pfunc = http->option.post_func;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_POST_PRIV:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // post priv
            tb_cpointer_t priv = (tb_pointer_t)tb_va_arg(args, tb_pointer_t);

            // set post priv
            http->option.post_priv = priv;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_POST_PRIV:
        {
            // ppost priv
            tb_cpointer_t* ppriv = (tb_cpointer_t*)tb_va_arg(args, tb_cpointer_t*);
            tb_assert_and_check_return_val(ppriv, tb_false);

            // get post priv
            *ppriv = http->option.post_priv;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_POST_LRATE:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // post lrate
            tb_size_t lrate = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set post lrate
            http->option.post_lrate = lrate;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_POST_LRATE:
        {
            // ppost lrate
            tb_size_t* plrate = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(plrate, tb_false);

            // get post lrate
            *plrate = http->option.post_lrate;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_AUTO_UNZIP:
        {   
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // bunzip
            tb_bool_t bunzip = (tb_bool_t)tb_va_arg(args, tb_bool_t);

            // set bunzip
            http->option.bunzip = bunzip? 1 : 0;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_AUTO_UNZIP:
        {
            // pbunzip
            tb_bool_t* pbunzip = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
            tb_assert_and_check_return_val(pbunzip, tb_false);

            // get bunzip
            *pbunzip = http->option.bunzip? tb_true : tb_false;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_REDIRECT:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // redirect
            tb_size_t redirect = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set redirect
            http->option.redirect = redirect;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_REDIRECT:
        {
            // predirect
            tb_size_t* predirect = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(predirect, tb_false);

            // get redirect
            *predirect = http->option.redirect;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_SET_VERSION:
        {
            // is opened?
            tb_assert_and_check_return_val(!http->bopened, tb_false);

            // version
            tb_size_t version = (tb_size_t)tb_va_arg(args, tb_size_t);

            // set version
            http->option.version = version;
            return tb_true;
        }
        break;
    case TB_HTTP_OPTION_GET_VERSION:
        {
            // pversion
            tb_size_t* pversion = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pversion, tb_false);

            // get version
            *pversion = http->option.version;
            return tb_true;
        }
        break;
    default:
        break;
    }
    return tb_false;
}
tb_http_status_t const* tb_http_status(tb_handle_t handle)
{
    tb_http_t* http = (tb_http_t*)handle;
    tb_assert_and_check_return_val(http, tb_null);
    return &http->status;
}
tb_time_t tb_http_date_from_cstr(tb_char_t const* cstr, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(cstr && size, 0);

    // done
    tb_tm_t             tm = {0};
    tb_time_t           date = 0;
    tb_char_t const*    p = cstr;
    tb_char_t const*    e = cstr + size;
    do
    {
        // skip space
        while (p < e && tb_isspace(*p)) p++;

        // ignore
#if 0
        // parse week
        if ((p + 6 < e && !tb_strnicmp(p, "Monday", 6)) || (p + 3 < e && !tb_strnicmp(p, "Mon", 3)))
            tm.week = 1;
        else if ((p + 7 < e && !tb_strnicmp(p, "Tuesday", 7)) || (p + 3 < e && !tb_strnicmp(p, "Tue", 3)))
            tm.week = 2;
        else if ((p + 9 < e && !tb_strnicmp(p, "Wednesday", 9)) || (p + 3 < e && !tb_strnicmp(p, "Wed", 3)))
            tm.week = 3;    
        else if ((p + 8 < e && !tb_strnicmp(p, "Thursday", 8)) || (p + 3 < e && !tb_strnicmp(p, "Thu", 3)))
            tm.week = 4;
        else if ((p + 6 < e && !tb_strnicmp(p, "Friday", 6)) || (p + 3 < e && !tb_strnicmp(p, "Fri", 3)))
            tm.week = 5;
        else if ((p + 8 < e && !tb_strnicmp(p, "Saturday", 8)) || (p + 3 < e && !tb_strnicmp(p, "Sat", 3)))
            tm.week = 6;
        else if ((p + 6 < e && !tb_strnicmp(p, "Sunday", 6)) || (p + 3 < e && !tb_strnicmp(p, "Sun", 3)))
            tm.week = 7;
#endif

        // skip week
        while (p < e && *p != ',' && !tb_isspace(*p)) p++; if (p < e && (*p == ',' || tb_isspace(*p))) p++;

        // skip space
        while (p < e && tb_isspace(*p)) p++;

        // is day?
        tb_bool_t year_suffix = tb_true;
        if (p < e && tb_isdigit(*p))
        {
            /* prefix year
             * 
             * .e.g 
             * year_suffix == false: Sun, 06-Nov-1994 08:49:37
             * year_suffix == true: Sun Nov 6 08:49:37 1994
             */
            year_suffix = tb_false;

            // parse day
            tm.mday = tb_s10tou32(p);

            // skip day
            while (p < e && *p != '-' && !tb_isspace(*p)) p++; if (p < e && (*p == '-' || tb_isspace(*p))) p++;
        }

        // parse month
        if (p + 3 < e && !tb_strnicmp(p, "Jan", 3))
            tm.month = 1;
        else if (p + 3 < e && !tb_strnicmp(p, "Feb", 3))
            tm.month = 2;
        else if (p + 3 < e && !tb_strnicmp(p, "Mar", 3))
            tm.month = 3;
        else if (p + 3 < e && !tb_strnicmp(p, "Apr", 3))
            tm.month = 4;
        else if (p + 3 < e && !tb_strnicmp(p, "May", 3))
            tm.month = 5;
        else if (p + 3 < e && !tb_strnicmp(p, "Jun", 3))
            tm.month = 6;
        else if (p + 3 < e && !tb_strnicmp(p, "Jul", 3))
            tm.month = 7;
        else if (p + 3 < e && !tb_strnicmp(p, "Aug", 3))
            tm.month = 8;
        else if (p + 3 < e && !tb_strnicmp(p, "Sep", 3))
            tm.month = 9;
        else if (p + 3 < e && !tb_strnicmp(p, "Oct", 3))
            tm.month = 10;
        else if (p + 3 < e && !tb_strnicmp(p, "Nov", 3))
            tm.month = 11;
        else if (p + 3 < e && !tb_strnicmp(p, "Dec", 3))
            tm.month = 12;

        // skip month
        while (p < e && *p != '-' && !tb_isspace(*p)) p++; if (p < e && (*p == '-' || tb_isspace(*p))) p++;

        // year suffix?
        if (year_suffix)
        {   
            // parse day
            tm.mday = tb_s10tou32(p);
        }
        else
        {
            // parse year
            tm.year = tb_s10tou32(p);
            if (tm.year < 100) tm.year += 2000;
        }

        // skip year or day
        while (p < e && !tb_isspace(*p)) p++; 
        while (p < e && tb_isspace(*p)) p++; 

        // parse hour
        tm.hour = tb_s10tou32(p);

        // skip hour
        while (p < e && *p != ':') p++; if (p < e && *p == ':') p++;

        // parse minute
        tm.minute = tb_s10tou32(p);

        // skip minute
        while (p < e && *p != ':') p++; if (p < e && *p == ':') p++;

        // parse second
        tm.second = tb_s10tou32(p);

        // year suffix?
        if (year_suffix)
        {
            // skip time
            while (p < e && !tb_isspace(*p)) p++; 
            while (p < e && tb_isspace(*p)) p++; 

            // parse year
            tm.year = tb_s10tou32(p);
            if (tm.year < 100) tm.year += 1900;
        }

        // make date
        date = tb_gmmktime(&tm);

    } while (0);

    // ok?
    return date;
}
