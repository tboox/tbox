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
 * If not, see <a href="impl://www.gnu.org/licenses/"> impl://www.gnu.org/licenses/</a>
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
#include "impl/http/date.h"
#include "impl/http/option.h"
#include "impl/http/status.h"
#include "impl/http/method.h"
#include "../zip/zip.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../stream/stream.h"
#include "../platform/platform.h"
#include "../algorithm/algorithm.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the http impl type
typedef struct __tb_http_impl_t
{
    // the option
    tb_http_option_t    option;

    // the status 
    tb_http_status_t    status;

    // the stream
    tb_stream_ref_t     stream;

    // the sstream for sock
    tb_stream_ref_t     sstream;

    // the cstream for chunked
    tb_stream_ref_t     cstream;

    // the zstream for gzip/deflate
    tb_stream_ref_t     zstream;

    // the head
    tb_hash_map_ref_t   head;

    // is opened?
    tb_bool_t           bopened;

    // the request data
    tb_string_t         request;

    // the cookies
    tb_string_t         cookies;

}tb_http_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_http_connect(tb_http_impl_t* impl)
{
    // check
    tb_assert_and_check_return_val(impl && impl->stream, tb_false);
    
    // done
    tb_bool_t ok = tb_false;
    do
    {
        // the host is changed?
        tb_bool_t           host_changed = tb_true;
        tb_char_t const*    host_old = tb_null;
        tb_char_t const*    host_new = tb_url_host(&impl->option.url);
        tb_stream_ctrl(impl->stream, TB_STREAM_CTRL_GET_HOST, &host_old);
        if (host_old && host_new && !tb_stricmp(host_old, host_new)) host_changed = tb_false;

        // trace
        tb_trace_d("connect: host: %s", host_changed? "changed" : "keep");

        // ctrl stream
        if (!tb_stream_ctrl(impl->stream, TB_STREAM_CTRL_SET_URL, tb_url_cstr(&impl->option.url))) break;
        if (!tb_stream_ctrl(impl->stream, TB_STREAM_CTRL_SET_TIMEOUT, impl->option.timeout)) break;

        // dump option
#if defined(__tb_debug__) && TB_TRACE_MODULE_DEBUG
        tb_http_option_dump(&impl->option);
#endif
        
        // trace
        tb_trace_d("connect: ..");

        // clear status
        tb_http_status_cler(&impl->status, host_changed);

        // open stream
        if (!tb_stream_open(impl->stream)) break;

        // trace
        tb_trace_d("connect: ok");

        // ok
        ok = tb_true;

    } while (0);


    // failed? save state
    if (!ok) impl->status.state = tb_stream_state(impl->stream);

    // ok?
    return ok;
}
static tb_bool_t tb_http_request_post(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)priv;
    tb_assert_and_check_return_val(impl && impl->stream, tb_false);

    // trace
    tb_trace_d("post: percent: %llu%%, size: %lu, state: %s", size > 0? (offset * 100 / size) : 0, save, tb_state_cstr(state));

    // done func
    if (impl->option.post_func && !impl->option.post_func(state, offset, size, save, rate, impl->option.post_priv)) 
        return tb_false;

    // ok?
    return tb_true;
}
static tb_bool_t tb_http_request(tb_http_impl_t* impl)
{
    // check
    tb_assert_and_check_return_val(impl && impl->stream, tb_false);

    // done
    tb_bool_t           ok = tb_false;
    tb_stream_ref_t     pstream = tb_null;
    tb_hong_t           post_size = 0;
    do
    {
        // clear line data
        tb_string_clear(&impl->request);

        // init the head value
        tb_char_t           data[8192];
        tb_static_string_t  value;
        if (!tb_static_string_init(&value, data, sizeof(data))) break;

        // init method
        tb_char_t const* method = tb_http_method_cstr(impl->option.method);
        tb_assert_and_check_break(method);

        // init path
        tb_char_t const* path = tb_url_path(&impl->option.url);
        tb_assert_and_check_break(path);

        // init args
        tb_char_t const* args = tb_url_args(&impl->option.url);

        // init host
        tb_char_t const* host = tb_url_host(&impl->option.url);
        tb_assert_and_check_break(host);
        tb_hash_map_insert(impl->head, "Host", host);

        // init accept
        tb_hash_map_insert(impl->head, "Accept", "*/*");

        // init connection
        tb_hash_map_insert(impl->head, "Connection", impl->status.balived? "keep-alive" : "close");

        // init cookies
        tb_bool_t cookie = tb_false;
        if (impl->option.cookies)
        {
            // set cookie
            if (tb_cookies_get(impl->option.cookies, host, path, tb_url_ssl(&impl->option.url), &impl->cookies))
            {
                tb_hash_map_insert(impl->head, "Cookie", tb_string_cstr(&impl->cookies));
                cookie = tb_true;
            }
        }

        // no cookie? remove it
        if (!cookie) tb_hash_map_remove(impl->head, "Cookie");

        // init range
        if (impl->option.range.bof && impl->option.range.eof >= impl->option.range.bof)
            tb_static_string_cstrfcpy(&value, "bytes=%llu-%llu", impl->option.range.bof, impl->option.range.eof);
        else if (impl->option.range.bof && !impl->option.range.eof)
            tb_static_string_cstrfcpy(&value, "bytes=%llu-", impl->option.range.bof);
        else if (!impl->option.range.bof && impl->option.range.eof)
            tb_static_string_cstrfcpy(&value, "bytes=0-%llu", impl->option.range.eof);
        else if (impl->option.range.bof > impl->option.range.eof)
        {
            impl->status.state = TB_STATE_HTTP_RANGE_INVALID;
            break;
        }

        // update range
        if (tb_static_string_size(&value)) 
            tb_hash_map_insert(impl->head, "Range", tb_static_string_cstr(&value));
        // remove range
        else tb_hash_map_remove(impl->head, "Range");

        // init post
        if (impl->option.method == TB_HTTP_METHOD_POST)
        {
            // done
            tb_bool_t post_ok = tb_false;
            do
            {
                // init pstream
                tb_char_t const* url = tb_url_cstr(&impl->option.post_url);
                if (impl->option.post_data && impl->option.post_size)
                    pstream = tb_stream_init_from_data(impl->option.post_data, impl->option.post_size);
                else if (url) pstream = tb_stream_init_from_url(url);
                tb_assert_and_check_break(pstream);

                // open pstream
                if (!tb_stream_open(pstream)) break;

                // the post size
                post_size = tb_stream_size(pstream);
                tb_assert_and_check_break(post_size >= 0);

                // append post size
                tb_static_string_cstrfcpy(&value, "%lld", post_size);
                tb_hash_map_insert(impl->head, "Content-Length", tb_static_string_cstr(&value));

                // ok
                post_ok = tb_true;

            } while (0);

            // init post failed?
            if (!post_ok) 
            {
                impl->status.state = TB_STATE_HTTP_POST_FAILED;
                break;
            }
        }
        // remove post
        else tb_hash_map_remove(impl->head, "Content-Length");

        // replace the custom head 
        tb_char_t const* head_data = (tb_char_t const*)tb_buffer_data(&impl->option.head_data);
        tb_char_t const* head_tail = head_data + tb_buffer_size(&impl->option.head_data);
        while (head_data < head_tail)
        {
            // the name and data
            tb_char_t const* name = head_data;
            tb_char_t const* data = head_data + tb_strlen(name) + 1;
            tb_check_break(data < head_tail);

            // replace it
            tb_hash_map_insert(impl->head, name, data);

            // next
            head_data = data + tb_strlen(data) + 1;
        }

        // exit the head value
        tb_static_string_exit(&value);

        // check head
        tb_assert_and_check_break(tb_hash_map_size(impl->head));

        // append method
        tb_string_cstrcat(&impl->request, method);

        // append ' '
        tb_string_chrcat(&impl->request, ' ');

        // encode path
        tb_url_encode2(path, tb_strlen(path), data, sizeof(data) - 1);
        path = data;

        // append path
        tb_string_cstrcat(&impl->request, path);

        // append args if exists
        if (args) 
        {
            // append '?'
            tb_string_chrcat(&impl->request, '?');

            // encode args
            tb_url_encode2(args, tb_strlen(args), data, sizeof(data) - 1);
            args = data;

            // append args
            tb_string_cstrcat(&impl->request, args);
        }

        // append ' '
        tb_string_chrcat(&impl->request, ' ');

        // append version, HTTP/1.1
        tb_string_cstrfcat(&impl->request, "HTTP/1.%1u\r\n", impl->status.balived? impl->status.version : impl->option.version);

        // append key: value
        tb_for_all (tb_hash_map_item_ref_t, item, impl->head)
        {
            if (item && item->name && item->data) 
                tb_string_cstrfcat(&impl->request, "%s: %s\r\n", (tb_char_t const*)item->name, (tb_char_t const*)item->data);
        }

        // append end
        tb_string_cstrcat(&impl->request, "\r\n");

        // the request data and size
        tb_char_t const*    request_data = tb_string_cstr(&impl->request);
        tb_size_t           request_size = tb_string_size(&impl->request);
        tb_assert_and_check_break(request_data && request_size);
        
        // trace
        tb_trace_d("request[%lu]:\n%s", request_size, request_data);

        // writ request
        if (!tb_stream_bwrit(impl->stream, (tb_byte_t const*)request_data, request_size)) break;

        // writ post
        if (impl->option.method == TB_HTTP_METHOD_POST)
        {
            // post stream
            if (tb_transfer_done(pstream, impl->stream, impl->option.post_lrate, tb_http_request_post, impl) != post_size)
            {
                impl->status.state = TB_STATE_HTTP_POST_FAILED;
                break;
            }
        }

        // sync request
        if (!tb_stream_sync(impl->stream, tb_false)) break;
    
        // ok
        ok = tb_true;
    }
    while (0);

    // failed?
    if (!ok && !impl->status.state) impl->status.state = TB_STATE_HTTP_REQUEST_FAILED;

    // exit pstream
    if (pstream) tb_stream_exit(pstream);
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
static tb_bool_t tb_http_response_done(tb_http_impl_t* impl, tb_char_t const* line, tb_size_t indx)
{
    // check
    tb_assert_and_check_return_val(impl && impl->sstream && line, tb_false);

    // the first line? 
    tb_char_t const* p = line;
    if (!indx)
    {
        // check http response
        if (tb_strnicmp(p, "HTTP/1.", 7))
        {
            // failed
            tb_assert_abort(0);
            return tb_false;
        }

        // seek to the http version
        p += 7;
        tb_assert_and_check_return_val(*p, tb_false);

        // parse version
        tb_assert_and_check_return_val((*p - '0') < 2, tb_false);
        impl->status.version = *p - '0';
    
        // seek to the http code
        p++; while (tb_isspace(*p)) p++;

        // parse code
        tb_assert_and_check_return_val(*p && tb_isdigit(*p), tb_false);
        impl->status.code = tb_stou32(p);

        // save state
        if (impl->status.code == 200 || impl->status.code == 206)
            impl->status.state = TB_STATE_OK;
        else if (impl->status.code == 204)
            impl->status.state = TB_STATE_HTTP_RESPONSE_204;
        else if (impl->status.code >= 300 && impl->status.code <= 307)
            impl->status.state = TB_STATE_HTTP_RESPONSE_300 + (impl->status.code - 300);
        else if (impl->status.code >= 400 && impl->status.code <= 416)
            impl->status.state = TB_STATE_HTTP_RESPONSE_400 + (impl->status.code - 400);
        else if (impl->status.code >= 500 && impl->status.code <= 507)
            impl->status.state = TB_STATE_HTTP_RESPONSE_500 + (impl->status.code - 500);
        else impl->status.state = TB_STATE_HTTP_RESPONSE_UNK;

        // check state code: 4xx & 5xx
        if (impl->status.code >= 400 && impl->status.code < 600) return tb_false;
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
            impl->status.content_size = tb_stou64(p);
            if (impl->status.document_size < 0) 
                impl->status.document_size = impl->status.content_size;
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
            impl->status.bseeked = 1;
            impl->status.document_size = document_size;
            if (impl->status.content_size < 0) 
            {
                if (from && to > from) impl->status.content_size = to - from;
                else if (!from && to) impl->status.content_size = to;
                else if (from && !to && document_size > from) impl->status.content_size = document_size - from;
                else impl->status.content_size = document_size;
            }
        }
        // parse accept-ranges: "bytes "
        else if (!tb_strnicmp(line, "Accept-Ranges", 13))
        {
            // no stream, be able to seek
            impl->status.bseeked = 1;
        }
        // parse content type
        else if (!tb_strnicmp(line, "Content-Type", 12)) 
        {
            tb_string_cstrcpy(&impl->status.content_type, p);
            tb_assert_and_check_return_val(tb_string_size(&impl->status.content_type), tb_false);
        }
        // parse transfer encoding
        else if (!tb_strnicmp(line, "Transfer-Encoding", 17))
        {
            if (!tb_stricmp(p, "chunked")) impl->status.bchunked = 1;
        }
        // parse content encoding
        else if (!tb_strnicmp(line, "Content-Encoding", 16))
        {
            if (!tb_stricmp(p, "gzip")) impl->status.bgzip = 1;
            else if (!tb_stricmp(p, "deflate")) impl->status.bdeflate = 1;
        }
        // parse location
        else if (!tb_strnicmp(line, "Location", 8)) 
        {
            // redirect? check code: 301 - 307
            tb_assert_and_check_return_val(impl->status.code > 300 && impl->status.code < 308, tb_false);

            // save location
            tb_string_cstrcpy(&impl->status.location, p);
        }
        // parse connection
        else if (!tb_strnicmp(line, "Connection", 10))
        {
            // keep alive?
            impl->status.balived = !tb_stricmp(p, "close")? 0 : 1;

            // ctrl stream for sock
            if (!tb_stream_ctrl(impl->sstream, TB_STREAM_CTRL_SOCK_KEEP_ALIVE, impl->status.balived? tb_true : tb_false)) return tb_false;
        }
        // parse cookies
        else if (impl->option.cookies && !tb_strnicmp(line, "Set-Cookie", 10))
        {
            // the host
            tb_char_t const* host = tb_null;
            tb_http_ctrl((tb_http_ref_t)impl, TB_HTTP_OPTION_GET_HOST, &host);

            // the path
            tb_char_t const* path = tb_null;
            tb_http_ctrl((tb_http_ref_t)impl, TB_HTTP_OPTION_GET_PATH, &path);

            // is ssl?
            tb_bool_t bssl = tb_false;
            tb_http_ctrl((tb_http_ref_t)impl, TB_HTTP_OPTION_GET_SSL, &bssl);
                
            // set cookies
            tb_cookies_set(impl->option.cookies, host, path, bssl, p);
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_http_response(tb_http_impl_t* impl)
{
    // check
    tb_assert_and_check_return_val(impl && impl->stream, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // read line
        tb_char_t line[8192];
        tb_long_t real = 0;
        tb_size_t indx = 0;
        while ((real = tb_stream_bread_line(impl->stream, line, sizeof(line) - 1)) >= 0)
        {
            // trace
            tb_trace_d("response: %s", line);
 
            // do callback
            if (impl->option.head_func && !impl->option.head_func(line, impl->option.head_priv)) break;
            
            // end?
            if (!real)
            {
                // switch to cstream if chunked
                if (impl->status.bchunked)
                {
                    // init cstream
                    if (impl->cstream)
                    {
                        if (!tb_stream_ctrl(impl->cstream, TB_STREAM_CTRL_FLTR_SET_STREAM, impl->stream)) break;
                    }
                    else impl->cstream = tb_stream_init_filter_from_chunked(impl->stream, tb_true);
                    tb_assert_and_check_break(impl->cstream);

                    // open cstream, need not async
                    if (!tb_stream_open(impl->cstream)) break;

                    // using cstream
                    impl->stream = impl->cstream;

                    // disable seek
                    impl->status.bseeked = 0;
                }

                // switch to zstream if gzip or deflate
                if (impl->option.bunzip && (impl->status.bgzip || impl->status.bdeflate))
                {
#if defined(TB_CONFIG_PACKAGE_HAVE_ZLIB) && defined(TB_CONFIG_MODULE_HAVE_ZIP)
                    // init zstream
                    if (impl->zstream)
                    {
                        if (!tb_stream_ctrl(impl->zstream, TB_STREAM_CTRL_FLTR_SET_STREAM, impl->stream)) break;
                    }
                    else impl->zstream = tb_stream_init_filter_from_zip(impl->stream, impl->status.bgzip? TB_ZIP_ALGO_GZIP : TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE);
                    tb_assert_and_check_break(impl->zstream);

                    // the filter
                    tb_stream_filter_ref_t filter = tb_null;
                    if (!tb_stream_ctrl(impl->zstream, TB_STREAM_CTRL_FLTR_GET_FILTER, &filter)) break;
                    tb_assert_and_check_break(filter);

                    // ctrl filter
                    if (!tb_stream_filter_ctrl(filter, TB_STREAM_FILTER_CTRL_ZIP_SET_ALGO, impl->status.bgzip? TB_ZIP_ALGO_GZIP : TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE)) break;

                    // limit the filter input size
                    if (impl->status.content_size > 0) tb_stream_filter_limit(filter, impl->status.content_size);

                    // open zstream, need not async
                    if (!tb_stream_open(impl->zstream)) break;

                    // using zstream
                    impl->stream = impl->zstream;

                    // disable seek
                    impl->status.bseeked = 0;
#else
                    // trace
                    tb_trace_w("gzip is not supported now! please enable it from config if you need it.");

                    // not supported
                    impl->status.state = TB_STATE_HTTP_GZIP_NOT_SUPPORTED;
                    break;
#endif
                }

                // trace
                tb_trace_d("response: ok");

                // dump status
#if defined(__tb_debug__) && TB_TRACE_MODULE_DEBUG
                tb_http_status_dump(&impl->status);
#endif

                // ok
                ok = tb_true;
                break;
            }

            // done it
            if (!tb_http_response_done(impl, line, indx++)) break;
        }

    } while (0);

    // ok?
    return ok;
}
static tb_bool_t tb_http_redirect(tb_http_impl_t* impl)
{
    // check
    tb_assert_and_check_return_val(impl && impl->stream, tb_false);

    // done
    tb_size_t i = 0;
    for (i = 0; i < impl->option.redirect && tb_string_size(&impl->status.location); i++)
    {
        // read the redirect content
        if (impl->status.content_size > 0)
        {
            tb_byte_t data[TB_STREAM_BLOCK_MAXN];
            tb_hize_t read = 0;
            tb_hize_t size = impl->status.content_size;
            while (read < size) 
            {
                // the need
                tb_size_t need = (tb_size_t)tb_min(size - read, (tb_hize_t)TB_STREAM_BLOCK_MAXN);

                // read it
                if (!tb_stream_bread(impl->stream, data, need)) break;

                // save size
                read += need;
            }

            // check
            tb_assert_and_check_break(read == size);
        }

        // close stream
        if (impl->stream && !tb_stream_clos(impl->stream)) break;

        // switch to sstream
        impl->stream = impl->sstream;

        // done location url
        tb_char_t const* location = tb_string_cstr(&impl->status.location);
        tb_assert_and_check_break(location);

        // trace
        tb_trace_d("redirect: %s", location);

        // only path?
        if (tb_url_protocol_probe(location) == TB_URL_PROTOCOL_FILE) tb_url_path_set(&impl->option.url, location);
        // full url?
        else
        {
            // set url
            if (!tb_url_cstr_set(&impl->option.url, location)) break;
        }

        // connect it
        if (!tb_http_connect(impl)) break;

        // request it
        if (!tb_http_request(impl)) break;

        // response it
        if (!tb_http_response(impl)) break;
    }

    // ok?
    return (i < impl->option.redirect && tb_string_size(&impl->status.location))? tb_false : tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_http_ref_t tb_http_init()
{
    // done
    tb_bool_t           ok = tb_false;
    tb_http_impl_t*     impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_http_impl_t);
        tb_assert_and_check_break(impl);

        // init stream
        impl->stream = impl->sstream = tb_stream_init_sock();
        tb_assert_and_check_break(impl->stream);

        // init head
        impl->head = tb_hash_map_init(8, tb_item_func_str(tb_false), tb_item_func_str(tb_false));
        tb_assert_and_check_break(impl->head);

        // init request data
        if (!tb_string_init(&impl->request)) break;

        // init cookies data
        if (!tb_string_init(&impl->cookies)) break;

        // init option
        if (!tb_http_option_init(&impl->option)) break;

        // init status
        if (!tb_http_status_init(&impl->status)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (impl) tb_http_exit((tb_http_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_http_ref_t)impl;
}
tb_void_t tb_http_kill(tb_http_ref_t http)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return(impl);

    // kill stream
    if (impl->stream) tb_stream_kill(impl->stream);
}
tb_void_t tb_http_exit(tb_http_ref_t http)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return(impl);

    // close it
    tb_http_clos(http);

    // exit zstream
    if (impl->zstream) tb_stream_exit(impl->zstream);
    impl->zstream = tb_null;

    // exit cstream
    if (impl->cstream) tb_stream_exit(impl->cstream);
    impl->cstream = tb_null;

    // exit sstream
    if (impl->sstream) tb_stream_exit(impl->sstream);
    impl->sstream = tb_null;

    // exit stream
    impl->stream = tb_null;
    
    // exit status
    tb_http_status_exit(&impl->status);

    // exit option
    tb_http_option_exit(&impl->option);

    // exit cookies data
    tb_string_exit(&impl->cookies);

    // exit request data
    tb_string_exit(&impl->request);

    // exit head
    if (impl->head) tb_hash_map_exit(impl->head);
    impl->head = tb_null;

    // free it
    tb_free(impl);
}
tb_long_t tb_http_wait(tb_http_ref_t http, tb_size_t aioe, tb_long_t timeout)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return_val(impl && impl->stream, -1);

    // opened?
    tb_assert_and_check_return_val(impl->bopened, -1);

    // wait it
    tb_long_t wait = tb_stream_wait(impl->stream, aioe, timeout);

    // failed? save state
    if (wait < 0 && !impl->status.state) impl->status.state = tb_stream_state(impl->stream);

    // ok?
    return wait;
}
tb_bool_t tb_http_open(tb_http_ref_t http)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return_val(impl, tb_false);
    
    // opened?
    tb_assert_and_check_return_val(!impl->bopened, tb_false);

    // connect it
    if (!tb_http_connect(impl)) return tb_false;

    // request it
    if (!tb_http_request(impl)) return tb_false;

    // response it
    if (!tb_http_response(impl)) return tb_false;

    // redirect it
    if (!tb_http_redirect(impl)) return tb_false;

    // opened
    impl->bopened = tb_true;

    // ok
    return tb_true;
}
tb_bool_t tb_http_clos(tb_http_ref_t http)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return_val(impl, tb_false);
    
    // opened?
    tb_check_return_val(impl->bopened, tb_true);

    // close stream
    if (impl->stream && !tb_stream_clos(impl->stream)) return tb_false;

    // switch to sstream
    impl->stream = impl->sstream;

    // clear opened
    impl->bopened = tb_false;

    // ok
    return tb_true;
}
tb_bool_t tb_http_seek(tb_http_ref_t http, tb_hize_t offset)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return_val(impl, tb_false);

    // opened?
    tb_assert_and_check_return_val(impl->bopened, tb_false);

    // seeked?
    tb_check_return_val(impl->status.bseeked, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // close stream
        if (impl->stream && !tb_stream_clos(impl->stream)) break;

        // switch to sstream
        impl->stream = impl->sstream;

        // trace
        tb_trace_d("seek: %llu", offset);

        // set range
        impl->option.range.bof = offset;
        impl->option.range.eof = impl->status.document_size > 0? impl->status.document_size - 1 : 0;

        // connect it
        if (!tb_http_connect(impl)) break;

        // request it
        if (!tb_http_request(impl)) break;

        // response it
        if (!tb_http_response(impl)) break;

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
tb_long_t tb_http_read(tb_http_ref_t http, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return_val(impl && impl->stream, -1);

    // opened?
    tb_assert_and_check_return_val(impl->bopened, -1);

    // read
    return tb_stream_read(impl->stream, data, size);
}
tb_bool_t tb_http_bread(tb_http_ref_t http, tb_byte_t* data, tb_size_t size)
{   
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return_val(impl && impl->stream, tb_false);

    // opened?
    tb_assert_and_check_return_val(impl->bopened, tb_false);

    // read
    tb_size_t read = 0;
    while (read < size)
    {
        // read data
        tb_long_t real = tb_stream_read(impl->stream, data + read, size - read);

        // update size
        if (real > 0) read += real;
        // no data?
        else if (!real)
        {
            // wait
            tb_long_t e = tb_http_wait(http, TB_AIOE_CODE_RECV, impl->option.timeout);
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
tb_bool_t tb_http_ctrl(tb_http_ref_t http, tb_size_t option, ...)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return_val(impl && option, tb_false);

    // check
    if (TB_HTTP_OPTION_CODE_IS_SET(option) && impl->bopened)
    {
        // abort
        tb_assert_abort(0);
        return tb_false;
    }

    // init args
    tb_va_list_t args;
    tb_va_start(args, option);

    // done
    tb_bool_t ok = tb_http_option_ctrl(&impl->option, option, args);

    // exit args
    tb_va_end(args);
 
    // ok?
    return ok;
}
tb_http_status_t const* tb_http_status(tb_http_ref_t http)
{
    // check
    tb_http_impl_t* impl = (tb_http_impl_t*)http;
    tb_assert_and_check_return_val(impl, tb_null);

    // the status
    return &impl->status;
}

