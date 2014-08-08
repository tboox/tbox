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
 * @file        stream.c
 * @ingroup     stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "stream"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stream.h"
#include "impl/stream/prefix.h"
#include "../asio/asio.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_stream_ref_t tb_stream_init(     tb_size_t type
                                ,   tb_size_t type_size
                                ,   tb_size_t cache
                                ,   tb_bool_t (*open)(tb_stream_ref_t stream)
                                ,   tb_bool_t (*clos)(tb_stream_ref_t stream)
                                ,   tb_void_t (*exit)(tb_stream_ref_t stream)
                                ,   tb_bool_t (*ctrl)(tb_stream_ref_t stream, tb_size_t ctrl, tb_va_list_t args)
                                ,   tb_long_t (*wait)(tb_stream_ref_t stream, tb_size_t wait, tb_long_t timeout)
                                ,   tb_long_t (*read)(tb_stream_ref_t stream, tb_byte_t* data, tb_size_t size)
                                ,   tb_long_t (*writ)(tb_stream_ref_t stream, tb_byte_t const* data, tb_size_t size)
                                ,   tb_bool_t (*seek)(tb_stream_ref_t stream, tb_hize_t offset)
                                ,   tb_bool_t (*sync)(tb_stream_ref_t stream, tb_bool_t bclosing)
                                ,   tb_void_t (*kill)(tb_stream_ref_t stream))
{
    // check
    tb_assert_and_check_return_val(type_size, tb_null);
    tb_assert_and_check_return_val(open && clos && ctrl && wait, tb_null);
    tb_assert_and_check_return_val(read || writ, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_stream_impl_t*   impl = tb_null;
    tb_stream_ref_t     stream = tb_null;
    do
    {
        // make impl
        impl = (tb_stream_impl_t*)tb_malloc0(sizeof(tb_stream_impl_t) + type_size);
        tb_assert_and_check_break(impl);

        // init stream
        stream = (tb_stream_ref_t)&impl[1];

        // init type
        impl->type = (tb_uint8_t)type;

        // init timeout, 10s
        impl->timeout = TB_STREAM_DEFAULT_TIMEOUT;

        // init internal state
        impl->istate = TB_STATE_CLOSED;

        // init url
        if (!tb_url_init(&impl->url)) break;

        // init cache
        if (!tb_queue_buffer_init(&impl->cache, cache)) break;

        // init func
        impl->open = open;
        impl->clos = clos;
        impl->exit = exit;
        impl->ctrl = ctrl;
        impl->wait = wait;
        impl->read = read;
        impl->writ = writ;
        impl->seek = seek;
        impl->sync = sync;
        impl->kill = kill;

        // ok
        ok = tb_true;

    } while (0);

    // failed? 
    if (!ok)
    {
        // exit it
        if (stream) tb_stream_exit(stream);
        stream = tb_null;
    }

    // ok?
    return stream;
}
tb_stream_ref_t tb_stream_init_from_url(tb_char_t const* url)
{
    // check
    tb_assert_and_check_return_val(url, tb_null);

    // the init
    static tb_stream_ref_t (*s_init[])() = 
    {
        tb_null
    ,   tb_stream_init_file
    ,   tb_stream_init_sock
    ,   tb_stream_init_http
    ,   tb_stream_init_data
    };

    // probe protocol
    tb_size_t protocol = tb_url_protocol_probe(url);
    tb_assert_static((tb_size_t)TB_URL_PROTOCOL_FILE == (tb_size_t)TB_STREAM_TYPE_FILE);
    tb_assert_static((tb_size_t)TB_URL_PROTOCOL_HTTP == (tb_size_t)TB_STREAM_TYPE_HTTP);
    tb_assert_static((tb_size_t)TB_URL_PROTOCOL_SOCK == (tb_size_t)TB_STREAM_TYPE_SOCK);
    tb_assert_static((tb_size_t)TB_URL_PROTOCOL_DATA == (tb_size_t)TB_STREAM_TYPE_DATA);

    // protocol => type
    tb_size_t type = protocol;
    if (!type || type > TB_STREAM_TYPE_DATA)
    {
        tb_trace_e("unknown stream for url: %s", url);
        return tb_null;
    }
    tb_assert_and_check_return_val(type && type < tb_arrayn(s_init) && s_init[type], tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_stream_ref_t     stream = tb_null;
    do
    {
        // init stream
        stream = s_init[type]();
        tb_assert_and_check_break(stream);

        // init url
        if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_URL, url)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit stream
        if (stream) tb_stream_exit(stream);
        stream = tb_null;
    }

    // ok?
    return stream;
}
tb_void_t tb_stream_exit(tb_stream_ref_t stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return(impl);

    // close it
    tb_stream_clos(stream);

    // exit it
    if (impl->exit) impl->exit(stream);

    // exit cache
    tb_queue_buffer_exit(&impl->cache);

    // exit url
    tb_url_exit(&impl->url);

    // free it
    tb_free(impl);
}
tb_long_t tb_stream_wait(tb_stream_ref_t stream, tb_size_t wait, tb_long_t timeout)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl && impl->wait, -1);

    // stoped?
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&impl->istate), -1);

    // wait it
    tb_long_t ok = impl->wait(stream, wait, timeout);
    
    // wait failed? save state
    if (ok < 0 && !impl->state) impl->state = TB_STATE_WAIT_FAILED;

    // ok?
    tb_check_return_val(!ok, ok);

    // cached?
    if (tb_queue_buffer_maxn(&impl->cache))
    {
        // have read cache?
        if ((wait & TB_STREAM_WAIT_READ) && !impl->bwrited && !tb_queue_buffer_null(&impl->cache)) 
            ok |= TB_STREAM_WAIT_READ;
        // have writ cache?
        else if ((wait & TB_STREAM_WAIT_WRIT) && impl->bwrited && !tb_queue_buffer_full(&impl->cache))
            ok |= TB_STREAM_WAIT_WRIT;
    }

    // ok?
    return ok;
}
tb_size_t tb_stream_state(tb_stream_ref_t stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl, TB_STATE_UNKNOWN_ERROR);

    // the stream state
    return impl->state;
}
tb_void_t tb_stream_state_set(tb_stream_ref_t stream, tb_size_t state)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return(impl);

    // set the stream state
    impl->state = state;
}
tb_size_t tb_stream_type(tb_stream_ref_t stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl, TB_STREAM_TYPE_NONE);

    // the type
    return impl->type;
}
tb_hong_t tb_stream_size(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, 0);

    // get the size
    tb_hong_t size = -1;
    return tb_stream_ctrl((tb_stream_ref_t)stream, TB_STREAM_CTRL_GET_SIZE, &size)? size : -1;
}
tb_hize_t tb_stream_left(tb_stream_ref_t stream)
{
    // check
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
tb_bool_t tb_stream_beof(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_true);

    // size
    tb_hong_t size = tb_stream_size(stream);
    tb_hize_t offt = tb_stream_offset(stream);

    // eof?
    return (size > 0 && offt >= size)? tb_true : tb_false;
}
tb_hize_t tb_stream_offset(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, 0);

    // get the offset
    tb_hize_t offset = 0;
    return tb_stream_ctrl(stream, TB_STREAM_CTRL_GET_OFFSET, &offset)? offset : 0;
}
tb_url_t* tb_stream_url(tb_stream_ref_t stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl, tb_null);

    // get the url
    return &impl->url;
}
tb_long_t tb_stream_timeout(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, -1);

    // get the timeout
    tb_long_t timeout = -1;
    return tb_stream_ctrl(stream, TB_STREAM_CTRL_GET_TIMEOUT, &timeout)? timeout : -1;
}
tb_bool_t tb_stream_is_opened(tb_stream_ref_t stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // the state
    tb_size_t state = tb_atomic_get(&impl->istate);

    // is opened?
    return (TB_STATE_OPENED == state || TB_STATE_KILLING == state)? tb_true : tb_false;
}
tb_bool_t tb_stream_is_closed(tb_stream_ref_t stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // the state
    tb_size_t state = tb_atomic_get(&impl->istate);

    // is closed?
    return (TB_STATE_CLOSED == state || TB_STATE_KILLED == state)? tb_true : tb_false;
}
tb_bool_t tb_stream_is_killed(tb_stream_ref_t stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // the state
    tb_size_t state = tb_atomic_get(&impl->istate);

    // is killed?
    return (TB_STATE_KILLED == state || TB_STATE_KILLING == state)? tb_true : tb_false;
}
tb_bool_t tb_stream_ctrl(tb_stream_ref_t stream, tb_size_t ctrl, ...)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl && impl->ctrl, tb_false);

    // init args
    tb_va_list_t args;
    tb_va_start(args, ctrl);

    // ctrl it
    tb_bool_t ok = tb_stream_ctrl_with_args(stream, ctrl, args);

    // exit args
    tb_va_end(args);

    // ok?
    return ok;
}
tb_bool_t tb_stream_ctrl_with_args(tb_stream_ref_t stream, tb_size_t ctrl, tb_va_list_t args)
{   
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl && impl->ctrl, tb_false);

    // save args
    tb_va_list_t args_saved;
    tb_va_copy(args_saved, args);

    // ctrl
    tb_bool_t ok = tb_false;
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // the poffset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);

            // get offset
            *poffset = impl->offset;
            return tb_true;
        }
    case TB_STREAM_CTRL_SET_URL:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_closed(stream), tb_false);

            // set url
            tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            if (url && tb_url_set(&impl->url, url)) ok = tb_true;
        }
        break;
    case TB_STREAM_CTRL_GET_URL:
        {
            // get url
            tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
            if (purl)
            {
                tb_char_t const* url = tb_url_get(&impl->url);
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
            tb_assert_and_check_return_val(tb_stream_is_closed(stream), tb_false);

            // set host
            tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            if (host)
            {
                tb_url_host_set(&impl->url, host);
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
                tb_char_t const* host = tb_url_host_get(&impl->url);
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
            tb_assert_and_check_return_val(tb_stream_is_closed(stream), tb_false);

            // set port
            tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
            if (port)
            {
                tb_url_port_set(&impl->url, (tb_uint16_t)port);
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
                *pport = tb_url_port_get(&impl->url);
                ok = tb_true;
            }
        }
        break;
    case TB_STREAM_CTRL_SET_PATH:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_closed(stream), tb_false);

            // set path
            tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            if (path)
            {
                tb_url_path_set(&impl->url, path);
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
                tb_char_t const* path = tb_url_path_get(&impl->url);
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
            tb_assert_and_check_return_val(tb_stream_is_closed(stream), tb_false);

            // set ssl
            tb_bool_t bssl = (tb_bool_t)tb_va_arg(args, tb_bool_t);
            tb_url_ssl_set(&impl->url, bssl);
            ok = tb_true;
        }
        break;
    case TB_STREAM_CTRL_GET_SSL:
        {
            // get ssl
            tb_bool_t* pssl = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
            if (pssl)
            {
                *pssl = tb_url_ssl_get(&impl->url);
                ok = tb_true;
            }
        }
        break;
    case TB_STREAM_CTRL_SET_TIMEOUT:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_closed(stream), tb_false);

            // set timeout
            tb_long_t timeout = (tb_long_t)tb_va_arg(args, tb_long_t);
            impl->timeout = timeout? timeout : TB_STREAM_DEFAULT_TIMEOUT;
            ok = tb_true;
        }
        break;
    case TB_STREAM_CTRL_GET_TIMEOUT:
        {
            // get timeout
            tb_long_t* ptimeout = (tb_long_t*)tb_va_arg(args, tb_long_t*);
            if (ptimeout)
            {
                *ptimeout = impl->timeout;
                ok = tb_true;
            }
        }
        break;
    default:
        break;
    }

    // restore args
    tb_va_copy(args, args_saved);

    // ctrl stream
    ok = (impl->ctrl(stream, ctrl, args) || ok)? tb_true : tb_false;

    // ok?
    return ok;
}
tb_void_t tb_stream_kill(tb_stream_ref_t stream)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return(impl);

    // trace
    tb_trace_d("kill: %s: state: %s: ..", tb_url_get(&impl->url), tb_state_cstr(tb_atomic_get(&impl->istate)));

    // opened? kill it
    if (TB_STATE_OPENED == tb_atomic_fetch_and_pset(&impl->istate, TB_STATE_OPENED, TB_STATE_KILLING))
    {
        // kill it
        if (impl->kill) impl->kill(stream);

        // trace
        tb_trace_d("kill: %s: ok", tb_url_get(&impl->url));
    }
    // opening? kill it
    else if (TB_STATE_OPENING == tb_atomic_fetch_and_pset(&impl->istate, TB_STATE_OPENING, TB_STATE_KILLING))
    {
        // kill it
        if (impl->kill) impl->kill(stream);

        // trace
        tb_trace_d("kill: %s: ok", tb_url_get(&impl->url));
    }
    else 
    {
        // closed? killed
        tb_atomic_pset(&impl->istate, TB_STATE_CLOSED, TB_STATE_KILLED);
    }
}
tb_bool_t tb_stream_open(tb_stream_ref_t stream)
{
    // check stream
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl && impl->open, tb_false);

    // already been opened?
    tb_check_return_val(!tb_stream_is_opened(stream), tb_true);

    // closed?
    tb_assert_and_check_return_val(TB_STATE_CLOSED == tb_atomic_get(&impl->istate), tb_false);

    // init offset
    impl->offset = 0;

    // init state
    impl->state = TB_STATE_OK;

    // open it
    tb_bool_t ok = impl->open(stream);

    // opened
    if (ok) tb_atomic_set(&impl->istate, TB_STATE_OPENED);

    // ok?
    return ok;
}
tb_bool_t tb_stream_clos(tb_stream_ref_t stream)
{
    // check 
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // already been closed?
    tb_check_return_val(tb_stream_is_opened(stream), tb_true);

    // flush writed data first
    if (impl->bwrited && !tb_stream_sync(stream, tb_true)) return tb_false;

    // has close?
    if (impl->clos && !impl->clos(stream)) return tb_false;

    // clear state
    impl->offset = 0;
    impl->bwrited = 0;
    impl->state = TB_STATE_OK;
    tb_atomic_set(&impl->istate, TB_STATE_CLOSED);

    // clear cache
    tb_queue_buffer_clear(&impl->cache);

    // ok
    return tb_true;
}
tb_bool_t tb_stream_need(tb_stream_ref_t stream, tb_byte_t** data, tb_size_t size)
{
    // check 
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(data && size, tb_false);

    // check stream
    tb_assert_and_check_return_val(impl && tb_stream_is_opened(stream) && impl->read && impl->wait, tb_false);

    // stoped?
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&impl->istate), tb_false);

    // no cache? enable it first
    if (!tb_queue_buffer_maxn(&impl->cache)) tb_queue_buffer_resize(&impl->cache, size);

    // check
    tb_assert_and_check_return_val(tb_queue_buffer_maxn(&impl->cache) && size <= tb_queue_buffer_maxn(&impl->cache), tb_false);

    // have writed cache? sync first
    if (impl->bwrited && !tb_queue_buffer_null(&impl->cache) && !tb_stream_sync(stream, tb_false)) return tb_false;

    // switch to the read cache mode
    if (impl->bwrited && tb_queue_buffer_null(&impl->cache)) impl->bwrited = 0;

    // check the cache mode, must be read cache
    tb_assert_and_check_return_val(!impl->bwrited, tb_false);

    // enough?
    if (size <= tb_queue_buffer_size(&impl->cache)) 
    {
        // save data
        *data = tb_queue_buffer_head(&impl->cache);

        // ok
        return tb_true;
    }

    // enter cache for push
    tb_size_t   push = 0;
    tb_size_t   need = size - tb_queue_buffer_size(&impl->cache);
    tb_byte_t*  tail = tb_queue_buffer_push_init(&impl->cache, &push);
    tb_assert_and_check_return_val(tail && push, tb_false);
    if (push > need) push = need;

    // fill cache
    tb_size_t read = 0;
    while (read < push && (TB_STATE_OPENED == tb_atomic_get(&impl->istate)))
    {
        // read data
        tb_long_t real = impl->read(stream, tail + read, push - read);
        
        // ok?
        if (real > 0)
        {
            // save read
            read += real;
        }
        // no data?
        else if (!real)
        {
            // wait
            real = impl->wait(stream, TB_STREAM_WAIT_READ, tb_stream_timeout(stream));

            // ok?
            tb_check_break(real > 0);
        }
        else break;
    }
    
    // leave cache for push
    tb_queue_buffer_push_exit(&impl->cache, read);

    // not enough?
    if (size > tb_queue_buffer_size(&impl->cache))
    {
        // killed? save state
        if (!impl->state && (TB_STATE_KILLING == tb_atomic_get(&impl->istate)))
            impl->state = TB_STATE_KILLED;

        // failed
        return tb_false;
    }

    // save data
    *data = tb_queue_buffer_head(&impl->cache);

    // ok
    return tb_true;
}
tb_long_t tb_stream_read(tb_stream_ref_t stream, tb_byte_t* data, tb_size_t size)
{
    // check 
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(data, -1);

    // no size?
    tb_check_return_val(size, 0);

    // check stream
    tb_assert_and_check_return_val(impl && tb_stream_is_opened(stream) && impl->read, -1);

    // done
    tb_long_t read = 0;
    do
    {
        if (tb_queue_buffer_maxn(&impl->cache))
        {
            // switch to the read cache mode
            if (impl->bwrited && tb_queue_buffer_null(&impl->cache)) impl->bwrited = 0;

            // check the cache mode, must be read cache
            tb_assert_and_check_return_val(!impl->bwrited, -1);

            // read data from cache first
            read = tb_queue_buffer_read(&impl->cache, data, size);
            tb_check_return_val(read >= 0, -1);

            // ok?
            tb_check_break(!read);

            // cache is null now.
            tb_assert_and_check_return_val(tb_queue_buffer_null(&impl->cache), -1);

            // enter cache for push
            tb_size_t   push = 0;
            tb_byte_t*  tail = tb_queue_buffer_push_init(&impl->cache, &push);
            tb_assert_and_check_return_val(tail && push, -1);

            // push data to cache from stream
            tb_assert(impl->read);
            tb_long_t   real = impl->read(stream, tail, push);
            tb_check_return_val(real >= 0, -1);

            // read the left data from cache
            if (real > 0) 
            {
                // leave cache for push
                tb_queue_buffer_push_exit(&impl->cache, real);

                // read cache
                real = tb_queue_buffer_read(&impl->cache, data + read, tb_min(real, size - read));
                tb_check_return_val(real >= 0, -1);

                // save read 
                read += real;
            }
        }
        else 
        {
            // read it directly
            read = impl->read(stream, data, size);
            tb_check_return_val(read >= 0, -1);
        }
    }
    while (0);

    // update offset
    impl->offset += read;

//  tb_trace_d("read: %d", read);
    return read;
}
tb_long_t tb_stream_writ(tb_stream_ref_t stream, tb_byte_t const* data, tb_size_t size)
{
    // check 
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(data, -1);

    // no size?
    tb_check_return_val(size, 0);

    // check stream
    tb_assert_and_check_return_val(impl && tb_stream_is_opened(stream) && impl->writ, -1);

    // done
    tb_long_t writ = 0;
    do
    {
        if (tb_queue_buffer_maxn(&impl->cache))
        {
            // switch to the writ cache mode
            if (!impl->bwrited && tb_queue_buffer_null(&impl->cache)) impl->bwrited = 1;

            // check the cache mode, must be writ cache
            tb_assert_and_check_return_val(impl->bwrited, -1);

            // writ data to cache first
            writ = tb_queue_buffer_writ(&impl->cache, data, size);
            tb_check_return_val(writ >= 0, -1);
            
            // ok?
            tb_check_break(!writ);

            // cache is full now.
            tb_assert_and_check_return_val(tb_queue_buffer_full(&impl->cache), -1);

            // enter cache for pull
            tb_size_t   pull = 0;
            tb_byte_t*  head = tb_queue_buffer_pull_init(&impl->cache, &pull);
            tb_assert_and_check_return_val(head && pull, -1);

            // pull data to stream from cache
            tb_long_t   real = impl->writ(stream, head, pull);
            tb_check_return_val(real >= 0, -1);

            // writ the left data to cache
            if (real > 0)
            {
                // leave cache for pull
                tb_queue_buffer_pull_exit(&impl->cache, real);

                // writ cache
                real = tb_queue_buffer_writ(&impl->cache, data + writ, tb_min(real, size - writ));
                tb_check_return_val(real >= 0, -1);

                // save writ 
                writ += real;
            }
        }
        else 
        {
            // writ it directly
            writ = impl->writ(stream, data, size);
            tb_check_return_val(writ >= 0, -1);
        }

    } while (0);

    // update offset
    impl->offset += writ;

//  tb_trace_d("writ: %d", writ);
    return writ;
}
tb_bool_t tb_stream_bread(tb_stream_ref_t stream, tb_byte_t* data, tb_size_t size)
{
    // check 
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl && data, tb_false);
    tb_check_return_val(size, tb_true);

    // have writed cache? sync first
    if (impl->bwrited && !tb_queue_buffer_null(&impl->cache) && !tb_stream_sync(stream, tb_false))
        return tb_false;

    // check the left
    tb_hize_t left = tb_stream_left(stream);
    tb_check_return_val(size <= left, tb_false);

    // read data from cache
    tb_long_t read = 0;
    while (read < size && (TB_STATE_OPENED == tb_atomic_get(&impl->istate)))
    {
        // read data
        tb_long_t real = tb_stream_read(stream, data + read, size - read);    
        if (real > 0) read += real;
        else if (!real)
        {
            // wait
            real = tb_stream_wait(stream, TB_STREAM_WAIT_READ, tb_stream_timeout(stream));
            tb_check_break(real > 0);

            // has read?
            tb_assert_and_check_break(real & TB_STREAM_WAIT_READ);
        }
        else break;
    }

    // killed? save state
    if (read != size && !impl->state && (TB_STATE_KILLING == tb_atomic_get(&impl->istate)))
        impl->state = TB_STATE_KILLED;

    // ok?
    return (read == size? tb_true : tb_false);
}
tb_bool_t tb_stream_bwrit(tb_stream_ref_t stream, tb_byte_t const* data, tb_size_t size)
{
    // check 
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl && data, tb_false);
    tb_check_return_val(size, tb_true);

    // writ data to cache
    tb_long_t writ = 0;
    while (writ < size && (TB_STATE_OPENED == tb_atomic_get(&impl->istate)))
    {
        // writ data
        tb_long_t real = tb_stream_writ(stream, data + writ, size - writ);    
        if (real > 0) writ += real;
        else if (!real)
        {
            // wait
            real = tb_stream_wait(stream, TB_STREAM_WAIT_WRIT, tb_stream_timeout(stream));
            tb_check_break(real > 0);

            // has writ?
            tb_assert_and_check_break(real & TB_STREAM_WAIT_WRIT);
        }
        else break;
    }

    // killed? save state
    if (writ != size && !impl->state && (TB_STATE_KILLING == tb_atomic_get(&impl->istate)))
        impl->state = TB_STATE_KILLED;

    // ok?
    return (writ == size? tb_true : tb_false);
}
tb_bool_t tb_stream_sync(tb_stream_ref_t stream, tb_bool_t bclosing)
{
    // check 
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl && impl->writ && impl->wait && tb_stream_is_opened(stream), tb_false);

    // stoped?
    tb_assert_and_check_return_val((TB_STATE_OPENED == tb_atomic_get(&impl->istate)), tb_false);

    // cached? sync cache first
    if (tb_queue_buffer_maxn(&impl->cache))
    {
        // have data?
        if (!tb_queue_buffer_null(&impl->cache))
        {
            // check: must be writed cache
            tb_assert_and_check_return_val(impl->bwrited, tb_false);

            // enter cache for pull
            tb_size_t   size = 0;
            tb_byte_t*  head = tb_queue_buffer_pull_init(&impl->cache, &size);
            tb_assert_and_check_return_val(head && size, tb_false);

            // writ cache data to stream
            tb_size_t   writ = 0;
            while (writ < size && (TB_STATE_OPENED == tb_atomic_get(&impl->istate)))
            {
                // writ
                tb_long_t real = impl->writ(stream, head + writ, size - writ);

                // ok?
                if (real > 0)
                {
                    // save writ
                    writ += real;
                }
                // no data?
                else if (!real)
                {
                    // wait
                    real = impl->wait(stream, TB_STREAM_WAIT_WRIT, tb_stream_timeout(stream));

                    // ok?
                    tb_check_break(real > 0);
                }
                // error or end?
                else break;
            }

            // leave cache for pull
            tb_queue_buffer_pull_exit(&impl->cache, writ);

            // cache be not cleared?
            if (!tb_queue_buffer_null(&impl->cache))
            {
                // killed? save state
                if (!impl->state && (TB_STATE_KILLING == tb_atomic_get(&impl->istate)))
                    impl->state = TB_STATE_KILLED;

                // failed
                return tb_false;
            }
        }
        else impl->bwrited = 1;
    }

    // sync
    return impl->sync? impl->sync(stream, bclosing) : tb_true;
}
tb_bool_t tb_stream_seek(tb_stream_ref_t stream, tb_hize_t offset)
{
    // check 
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl && tb_stream_is_opened(stream), tb_false);

    // stoped?
    tb_assert_and_check_return_val((TB_STATE_OPENED == tb_atomic_get(&impl->istate)), tb_false);

    // limit offset
    tb_hong_t size = tb_stream_size(stream);
    if (size >= 0 && offset > size) offset = size;

    // the offset be not changed?
    tb_hize_t curt = tb_stream_offset(stream);
    tb_check_return_val(offset != curt, tb_true);

    // for writing
    if (impl->bwrited)
    {
        // sync writed data first
        if (!tb_stream_sync(stream, tb_false)) return tb_false;

        // check cache, must not cache or empty cache
        tb_assert_and_check_return_val(!tb_queue_buffer_maxn(&impl->cache) || tb_queue_buffer_null(&impl->cache), tb_false);

        // seek it
        tb_bool_t ok = impl->seek? impl->seek(stream, offset) : tb_false;

        // save offset
        if (ok) impl->offset = offset;
    }
    // for reading
    else
    {
        // cached? try to seek it at the cache
        tb_bool_t ok = tb_false;
        if (tb_queue_buffer_maxn(&impl->cache))
        {
            tb_size_t   size = 0;
            tb_byte_t*  data = tb_queue_buffer_pull_init(&impl->cache, &size);
            if (data && size && offset > curt && offset < curt + size)
            {
                // seek it at the cache
                tb_queue_buffer_pull_exit(&impl->cache, (tb_size_t)(offset - curt));

                // save offset
                impl->offset = offset;
                
                // ok
                ok = tb_true;
            }
        }

        // seek it
        if (!ok)
        {
            // seek it
            ok = impl->seek? impl->seek(stream, offset) : tb_false;

            // ok?
            if (ok)
            {
                // save offset
                impl->offset = offset;
    
                // clear cache
                tb_queue_buffer_clear(&impl->cache);
            }
        }

        // try to read and seek
        if (!ok && offset > curt)
        {
            // read some data for updating offset
            tb_byte_t data[TB_STREAM_BLOCK_MAXN];
            while (tb_stream_offset(stream) != offset)
            {
                tb_size_t need = (tb_size_t)tb_min(offset - curt, TB_STREAM_BLOCK_MAXN);
                if (!tb_stream_bread(stream, data, need)) return tb_false;
            }
        }
    }

    // ok?
    return tb_stream_offset(stream) == offset? tb_true : tb_false;
}
tb_bool_t tb_stream_skip(tb_stream_ref_t stream, tb_hize_t size)
{
    return tb_stream_seek(stream, tb_stream_offset(stream) + size);
}
tb_long_t tb_stream_bread_line(tb_stream_ref_t stream, tb_char_t* data, tb_size_t size)
{
    // check
    tb_stream_impl_t* impl = tb_stream_impl(stream);
    tb_assert_and_check_return_val(impl, -1);

    // done
    tb_char_t   ch = 0;
    tb_char_t*  p = data;
    while ((TB_STATE_OPENED == tb_atomic_get(&impl->istate)))
    {
        // read char
        ch = tb_stream_bread_s8(stream);

        // is line?
        if (ch == '\n') 
        {
            // finish line
            if (p > data && p[-1] == '\r')
                p--;
            *p = '\0';
    
            // ok
            return p - data;
        }
        // append char to line
        else 
        {
            if ((p - data) < size - 1)
            *p++ = ch;

            // no data?
            if (!ch) break;
        }
    }

    // killed?
    if ((TB_STATE_KILLING == tb_atomic_get(&impl->istate))) return -1;

    // end?
    return tb_stream_beof(stream)? -1 : 0;
}
tb_long_t tb_stream_bwrit_line(tb_stream_ref_t stream, tb_char_t* data, tb_size_t size)
{
    // writ data
    tb_long_t writ = 0;
    if (size) 
    {
        if (!tb_stream_bwrit(stream, (tb_byte_t*)data, size)) return -1;
    }
    else
    {
        tb_char_t* p = data;
        while (*p)
        {
            if (!tb_stream_bwrit(stream, (tb_byte_t*)p, 1)) return -1;
            p++;
        }
    
        writ = p - data;
    }

    // writ "\r\n" or "\n"
#ifdef TB_CONFIG_OS_WINDOWS
    tb_char_t le[] = "\r\n";
    tb_size_t ln = 2;
#else
    tb_char_t le[] = "\n";
    tb_size_t ln = 1;
#endif
    if (!tb_stream_bwrit(stream, (tb_byte_t*)le, ln)) return -1;
    writ += ln;

    // ok
    return writ;
}
tb_long_t tb_stream_printf(tb_stream_ref_t stream, tb_char_t const* fmt, ...)
{
    // init data
    tb_char_t data[TB_STREAM_BLOCK_MAXN] = {0};
    tb_size_t size = 0;

    // format data
    tb_vsnprintf_format(data, TB_STREAM_BLOCK_MAXN, fmt, &size);
    tb_check_return_val(size, 0);

    // writ data
    return tb_stream_bwrit(stream, (tb_byte_t*)data, size)? size : -1;
}
tb_uint8_t tb_stream_bread_u8(tb_stream_ref_t stream)
{
    tb_byte_t b[1];
    if (!tb_stream_bread(stream, b, 1)) return 0;
    return b[0];
}
tb_sint8_t tb_stream_bread_s8(tb_stream_ref_t stream)
{
    tb_byte_t b[1];
    if (!tb_stream_bread(stream, b, 1)) return 0;
    return b[0];
}
tb_uint16_t tb_stream_bread_u16_le(tb_stream_ref_t stream)
{   
    tb_byte_t b[2];
    if (!tb_stream_bread(stream, b, 2)) return 0;
    return tb_bits_get_u16_le(b);
}
tb_sint16_t tb_stream_bread_s16_le(tb_stream_ref_t stream)
{   
    tb_byte_t b[2];
    if (!tb_stream_bread(stream, b, 2)) return 0;
    return tb_bits_get_s16_le(b);
}
tb_uint32_t tb_stream_bread_u24_le(tb_stream_ref_t stream)
{   
    tb_byte_t b[3];
    if (!tb_stream_bread(stream, b, 3)) return 0;
    return tb_bits_get_u24_le(b);
}
tb_sint32_t tb_stream_bread_s24_le(tb_stream_ref_t stream)
{
    tb_byte_t b[3];
    if (!tb_stream_bread(stream, b, 3)) return 0;
    return tb_bits_get_s24_le(b);
}
tb_uint32_t tb_stream_bread_u32_le(tb_stream_ref_t stream)
{
    tb_byte_t b[4];
    if (!tb_stream_bread(stream, b, 4)) return 0;
    return tb_bits_get_u32_le(b);
}
tb_sint32_t tb_stream_bread_s32_le(tb_stream_ref_t stream)
{   
    tb_byte_t b[4];
    if (!tb_stream_bread(stream, b, 4)) return 0;
    return tb_bits_get_s32_le(b);
}
tb_uint64_t tb_stream_bread_u64_le(tb_stream_ref_t stream)
{
    tb_byte_t b[8];
    if (!tb_stream_bread(stream, b, 8)) return 0;
    return tb_bits_get_u64_le(b);
}
tb_sint64_t tb_stream_bread_s64_le(tb_stream_ref_t stream)
{   
    tb_byte_t b[8];
    if (!tb_stream_bread(stream, b, 8)) return 0;
    return tb_bits_get_s64_le(b);
}
tb_uint16_t tb_stream_bread_u16_be(tb_stream_ref_t stream)
{   
    tb_byte_t b[2];
    if (!tb_stream_bread(stream, b, 2)) return 0;
    return tb_bits_get_u16_be(b);
}
tb_sint16_t tb_stream_bread_s16_be(tb_stream_ref_t stream)
{   
    tb_byte_t b[2];
    if (!tb_stream_bread(stream, b, 2)) return 0;
    return tb_bits_get_s16_be(b);
}
tb_uint32_t tb_stream_bread_u24_be(tb_stream_ref_t stream)
{   
    tb_byte_t b[3];
    if (!tb_stream_bread(stream, b, 3)) return 0;
    return tb_bits_get_u24_be(b);
}
tb_sint32_t tb_stream_bread_s24_be(tb_stream_ref_t stream)
{
    tb_byte_t b[3];
    if (!tb_stream_bread(stream, b, 3)) return 0;
    return tb_bits_get_s24_be(b);
}
tb_uint32_t tb_stream_bread_u32_be(tb_stream_ref_t stream)
{
    tb_byte_t b[4];
    if (!tb_stream_bread(stream, b, 4)) return 0;
    return tb_bits_get_u32_be(b);
}
tb_sint32_t tb_stream_bread_s32_be(tb_stream_ref_t stream)
{   
    tb_byte_t b[4];
    if (!tb_stream_bread(stream, b, 4)) return 0;
    return tb_bits_get_s32_be(b);
}
tb_uint64_t tb_stream_bread_u64_be(tb_stream_ref_t stream)
{
    tb_byte_t b[8];
    if (!tb_stream_bread(stream, b, 8)) return 0;
    return tb_bits_get_u64_be(b);
}
tb_sint64_t tb_stream_bread_s64_be(tb_stream_ref_t stream)
{   
    tb_byte_t b[8];
    if (!tb_stream_bread(stream, b, 8)) return 0;
    return tb_bits_get_s64_be(b);
}
tb_bool_t tb_stream_bwrit_u8(tb_stream_ref_t stream, tb_uint8_t val)
{
    tb_byte_t b[1];
    tb_bits_set_u8(b, val);
    return tb_stream_bwrit(stream, b, 1);
}
tb_bool_t tb_stream_bwrit_s8(tb_stream_ref_t stream, tb_sint8_t val)
{
    tb_byte_t b[1];
    tb_bits_set_s8(b, val);
    return tb_stream_bwrit(stream, b, 1);
}
tb_bool_t tb_stream_bwrit_u16_le(tb_stream_ref_t stream, tb_uint16_t val)
{
    tb_byte_t b[2];
    tb_bits_set_u16_le(b, val);
    return tb_stream_bwrit(stream, b, 2);
}
tb_bool_t tb_stream_bwrit_s16_le(tb_stream_ref_t stream, tb_sint16_t val)
{
    tb_byte_t b[2];
    tb_bits_set_s16_le(b, val);
    return tb_stream_bwrit(stream, b, 2);
}
tb_bool_t tb_stream_bwrit_u24_le(tb_stream_ref_t stream, tb_uint32_t val)
{   
    tb_byte_t b[3];
    tb_bits_set_u24_le(b, val);
    return tb_stream_bwrit(stream, b, 3);
}
tb_bool_t tb_stream_bwrit_s24_le(tb_stream_ref_t stream, tb_sint32_t val)
{
    tb_byte_t b[3];
    tb_bits_set_s24_le(b, val);
    return tb_stream_bwrit(stream, b, 3);
}
tb_bool_t tb_stream_bwrit_u32_le(tb_stream_ref_t stream, tb_uint32_t val)
{   
    tb_byte_t b[4];
    tb_bits_set_u32_le(b, val);
    return tb_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_stream_bwrit_s32_le(tb_stream_ref_t stream, tb_sint32_t val)
{
    tb_byte_t b[4];
    tb_bits_set_s32_le(b, val);
    return tb_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_stream_bwrit_u64_le(tb_stream_ref_t stream, tb_uint64_t val)
{   
    tb_byte_t b[8];
    tb_bits_set_u64_le(b, val);
    return tb_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_stream_bwrit_s64_le(tb_stream_ref_t stream, tb_sint64_t val)
{
    tb_byte_t b[8];
    tb_bits_set_s64_le(b, val);
    return tb_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_stream_bwrit_u16_be(tb_stream_ref_t stream, tb_uint16_t val)
{
    tb_byte_t b[2];
    tb_bits_set_u16_be(b, val);
    return tb_stream_bwrit(stream, b, 2);
}
tb_bool_t tb_stream_bwrit_s16_be(tb_stream_ref_t stream, tb_sint16_t val)
{
    tb_byte_t b[2];
    tb_bits_set_s16_be(b, val);
    return tb_stream_bwrit(stream, b, 2);
}
tb_bool_t tb_stream_bwrit_u24_be(tb_stream_ref_t stream, tb_uint32_t val)
{   
    tb_byte_t b[3];
    tb_bits_set_u24_be(b, val);
    return tb_stream_bwrit(stream, b, 3);
}
tb_bool_t tb_stream_bwrit_s24_be(tb_stream_ref_t stream, tb_sint32_t val)
{
    tb_byte_t b[3];
    tb_bits_set_s24_be(b, val);
    return tb_stream_bwrit(stream, b, 3);
}
tb_bool_t tb_stream_bwrit_u32_be(tb_stream_ref_t stream, tb_uint32_t val)
{   
    tb_byte_t b[4];
    tb_bits_set_u32_be(b, val);
    return tb_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_stream_bwrit_s32_be(tb_stream_ref_t stream, tb_sint32_t val)
{
    tb_byte_t b[4];
    tb_bits_set_s32_be(b, val);
    return tb_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_stream_bwrit_u64_be(tb_stream_ref_t stream, tb_uint64_t val)
{   
    tb_byte_t b[8];
    tb_bits_set_u64_be(b, val);
    return tb_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_stream_bwrit_s64_be(tb_stream_ref_t stream, tb_sint64_t val)
{
    tb_byte_t b[8];
    tb_bits_set_s64_be(b, val);
    return tb_stream_bwrit(stream, b, 8);
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_stream_bread_float_le(tb_stream_ref_t stream)
{
    tb_byte_t b[4];
    if (!tb_stream_bread(stream, b, 4)) return 0;
    return tb_bits_get_float_le(b);
}
tb_float_t tb_stream_bread_float_be(tb_stream_ref_t stream)
{
    tb_byte_t b[4];
    if (!tb_stream_bread(stream, b, 4)) return 0;
    return tb_bits_get_float_be(b);
}
tb_double_t tb_stream_bread_double_ble(tb_stream_ref_t stream)
{   
    tb_byte_t b[8];
    if (!tb_stream_bread(stream, b, 8)) return 0;
    return tb_bits_get_double_ble(b);
}
tb_double_t tb_stream_bread_double_bbe(tb_stream_ref_t stream)
{   
    tb_byte_t b[8];
    if (!tb_stream_bread(stream, b, 8)) return 0;
    return tb_bits_get_double_bbe(b);
}
tb_double_t tb_stream_bread_double_lle(tb_stream_ref_t stream)
{
    tb_byte_t b[8];
    if (!tb_stream_bread(stream, b, 8)) return 0;
    return tb_bits_get_double_lle(b);
}
tb_double_t tb_stream_bread_double_lbe(tb_stream_ref_t stream)
{
    tb_byte_t b[8];
    if (!tb_stream_bread(stream, b, 8)) return 0;
    return tb_bits_get_double_lbe(b);
}
tb_bool_t tb_stream_bwrit_float_le(tb_stream_ref_t stream, tb_float_t val)
{
    tb_byte_t b[4];
    tb_bits_set_float_le(b, val);
    return tb_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_stream_bwrit_float_be(tb_stream_ref_t stream, tb_float_t val)
{
    tb_byte_t b[4];
    tb_bits_set_float_be(b, val);
    return tb_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_stream_bwrit_double_ble(tb_stream_ref_t stream, tb_double_t val)
{
    tb_byte_t b[8];
    tb_bits_set_double_ble(b, val);
    return tb_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_stream_bwrit_double_bbe(tb_stream_ref_t stream, tb_double_t val)
{
    tb_byte_t b[8];
    tb_bits_set_double_bbe(b, val);
    return tb_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_stream_bwrit_double_lle(tb_stream_ref_t stream, tb_double_t val)
{
    tb_byte_t b[8];
    tb_bits_set_double_lle(b, val);
    return tb_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_stream_bwrit_double_lbe(tb_stream_ref_t stream, tb_double_t val)
{
    tb_byte_t b[8];
    tb_bits_set_double_lbe(b, val);
    return tb_stream_bwrit(stream, b, 8);
}

#endif
