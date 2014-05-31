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
 * @file        async_stream.c
 * @ingroup     stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "async_stream"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stream.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_async_stream_cache_sync_func(tb_async_stream_t* stream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(stream && stream->sync && stream->wcache_and.sync.func, tb_false);

    // move cache
    if (real) tb_scoped_buffer_memmov(&stream->wcache_data, real);

    // not finished? continue it
    if (state == TB_STATE_OK && real < size) return tb_true;

    // not finished? continue it
    tb_bool_t ok = tb_false;
    if (state == TB_STATE_OK && real < size) ok = tb_true;
    // ok? sync it
    else if (state == TB_STATE_OK && real == size)
    {
        // check
        tb_assert_and_check_return_val(!tb_scoped_buffer_size(&stream->wcache_data), tb_false);

        // post sync
        ok = stream->sync(stream, stream->wcache_and.sync.bclosing, stream->wcache_and.sync.func, priv);

        // failed? done func
        if (!ok) ok = stream->wcache_and.sync.func(stream, TB_STATE_UNKNOWN_ERROR, stream->wcache_and.sync.bclosing, priv);
    }
    // failed?
    else
    {
        // failed? done func
        ok = stream->wcache_and.sync.func(stream, state != TB_STATE_OK? state : TB_STATE_UNKNOWN_ERROR, stream->wcache_and.sync.bclosing, priv);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_cache_writ_func(tb_async_stream_t* stream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(stream && stream->wcache_and.writ.func, tb_false);

    // done
    tb_bool_t bwrit = tb_true;
    do
    {
        // check
        tb_check_break(state == TB_STATE_OK);

        // finished?
        if (real == size)
        {
            // trace
            tb_trace_d("cache: writ: %lu: ok", stream->wcache_and.writ.size);

            // clear cache
            tb_scoped_buffer_clear(&stream->wcache_data);
    
            // done func
            stream->wcache_and.writ.func(stream, TB_STATE_OK, stream->wcache_and.writ.data, stream->wcache_and.writ.size, stream->wcache_and.writ.size, priv);

            // break
            bwrit = tb_false;
        }

    } while (0);

    // failed? 
    if (state != TB_STATE_OK)
    {
        // trace
        tb_trace_d("cache: writ: %lu: failed: %s", stream->wcache_and.writ.size, tb_state_cstr(state));

        // done func
        stream->wcache_and.writ.func(stream, state, stream->wcache_and.writ.data, 0, stream->wcache_and.writ.size, priv);

        // break
        bwrit = tb_false;
    }

    // continue writing?
    return bwrit;
}
static tb_bool_t tb_async_stream_cache_writ_done(tb_async_stream_t* stream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(stream && stream->writ && data && size && func, tb_false);
    
    // using cache?
    tb_bool_t ok = tb_false;
    if (stream->wcache_maxn)
    {
        // writ data to cache 
        if (data && size) tb_scoped_buffer_memncat(&stream->wcache_data, data, size);

        // the writ data and size
        tb_byte_t const*    writ_data = tb_scoped_buffer_data(&stream->wcache_data);
        tb_size_t           writ_size = tb_scoped_buffer_size(&stream->wcache_data);
    
        // no full? writ ok
        if (writ_size < stream->wcache_maxn)
        {
            // trace
            tb_trace_d("cache: writ: %lu: ok", size);

            // done func
            func(stream, TB_STATE_OK, data, size, size, priv);
            ok = tb_true;
        }
        else
        {
            // trace
            tb_trace_d("cache: writ: %lu: ..", size);

            // writ it
            stream->wcache_and.writ.func = func;
            stream->wcache_and.writ.data = data;
            stream->wcache_and.writ.size = size;
            ok = stream->writ(stream, delay, writ_data, writ_size, tb_async_stream_cache_writ_func, priv);
        }
    }
    // writ it
    else ok = stream->writ(stream, delay, data, size, func, priv);

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_cache_read_done(tb_async_stream_t* stream, tb_size_t delay, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(stream && stream->read && func, tb_false);

    // have writed cache? need sync it first
    tb_assert_and_check_return_val(!stream->wcache_maxn || !tb_scoped_buffer_size(&stream->wcache_data), tb_false);

    // using cache?
    tb_byte_t* data = tb_null;
    if (stream->rcache_maxn)
    {
        // grow data
        if (stream->rcache_maxn > tb_scoped_buffer_maxn(&stream->rcache_data)) 
            tb_scoped_buffer_resize(&stream->rcache_data, stream->rcache_maxn);

        // the cache data
        data = tb_scoped_buffer_data(&stream->rcache_data);
        tb_assert_and_check_return_val(data, tb_false);

        // the maxn
        tb_size_t maxn = tb_scoped_buffer_maxn(&stream->rcache_data);

        // adjust the size
        if (!size || size > maxn) size = maxn;
    }

    // read it
    return stream->read(stream, delay, data, size, func, priv);
}
static tb_bool_t tb_async_stream_open_read_func(tb_async_stream_t* stream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_stream_open_read_t* open_read = (tb_async_stream_open_read_t*)priv;
    tb_assert_and_check_return_val(stream && stream->read && open_read && open_read->func, tb_false);

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // ok? 
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
        
        // killed?
        if (tb_stream_is_killed(stream))
        {
            state = TB_STATE_KILLED;
            break;
        }
    
        // read it
        if (!tb_async_stream_cache_read_done(stream, 0, open_read->size, open_read->func, open_read->priv)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);
 
    // failed?
    if (state != TB_STATE_OK) 
    {
        // done func
        ok = open_read->func(stream, state, tb_null, 0, open_read->size, open_read->priv);
    }
 
    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_open_writ_func(tb_async_stream_t* stream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_stream_open_writ_t* owrit = (tb_async_stream_open_writ_t*)priv;
    tb_assert_and_check_return_val(stream && stream->writ && owrit && owrit->func, tb_false);

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // ok? 
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
            
        // killed?
        if (tb_stream_is_killed(stream))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // check
        tb_assert_and_check_break(owrit->data && owrit->size);

        // writ it
        if (!tb_async_stream_cache_writ_done(stream, 0, owrit->data, owrit->size, owrit->func, owrit->priv)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed? 
    if (state != TB_STATE_OK)
    {   
        // done func
        ok = owrit->func(stream, state, owrit->data, 0, owrit->size, owrit->priv);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_open_seek_func(tb_async_stream_t* stream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_stream_open_seek_t* open_seek = (tb_async_stream_open_seek_t*)priv;
    tb_assert_and_check_return_val(stream && stream->seek && open_seek && open_seek->func, tb_false);

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // ok? 
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
        
        // killed?
        if (tb_stream_is_killed(stream))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // offset be not modified?
        if (tb_stream_offset(stream) == open_seek->offset)
        {
            // done func
            ok = open_seek->func(stream, TB_STATE_OK, open_seek->offset, open_seek->priv);
        }
        else
        {
            // seek it
            if (!stream->seek(stream, open_seek->offset, open_seek->func, open_seek->priv)) break;
        }

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed? 
    if (state != TB_STATE_OK) 
    {   
        // done func
        ok = open_seek->func(stream, state, 0, open_seek->priv);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_sync_read_func(tb_async_stream_t* stream, tb_size_t state, tb_bool_t bclosing, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sync_read_t* sync_read = (tb_async_stream_sync_read_t*)priv;
    tb_assert_and_check_return_val(stream && stream->read && sync_read && sync_read->func, tb_false);

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // ok? 
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
        
        // killed?
        if (tb_stream_is_killed(stream))
        {
            state = TB_STATE_KILLED;
            break;
        }
    
        // read it
        if (!tb_async_stream_cache_read_done(stream, 0, sync_read->size, sync_read->func, sync_read->priv)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);
 
    // failed?
    if (state != TB_STATE_OK) 
    {
        // done func
        ok = sync_read->func(stream, state, tb_null, 0, sync_read->size, sync_read->priv);
    }
 
    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_sync_seek_func(tb_async_stream_t* stream, tb_size_t state, tb_bool_t bclosing, tb_cpointer_t priv)
{
    // check
    tb_async_stream_sync_seek_t* sync_seek = (tb_async_stream_sync_seek_t*)priv;
    tb_assert_and_check_return_val(stream && stream->seek && sync_seek && sync_seek->func, tb_false);

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // ok? 
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
        
        // killed?
        if (tb_stream_is_killed(stream))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // offset be not modified?
        if (tb_stream_offset(stream) == sync_seek->offset)
        {
            // done func
            ok = sync_seek->func(stream, TB_STATE_OK, sync_seek->offset, sync_seek->priv);
        }
        else
        {
            // seek it
            if (!stream->seek(stream, sync_seek->offset, sync_seek->func, sync_seek->priv)) break;
        }

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed? 
    if (state != TB_STATE_OK) 
    {   
        // done func
        ok = sync_seek->func(stream, state, 0, sync_seek->priv);
    }

    // ok?
    return ok;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_async_stream_init(tb_async_stream_t* stream, tb_aicp_t* aicp, tb_size_t type, tb_size_t rcache, tb_size_t wcache)
{
    // check
    tb_assert_and_check_return_val(stream, tb_false);

    // done
    tb_bool_t ok = tb_false;
    tb_bool_t ok_url = tb_false;
    tb_bool_t ok_rcache = tb_false;
    do
    {
        // init mode
        stream->base.mode       = TB_STREAM_MODE_AICO;
        
        // init type
        stream->base.type       = type;

        // init timeout, 10s
        stream->base.timeout    = TB_STREAM_DEFAULT_TIMEOUT;

        // init internal state
        stream->base.istate     = TB_STATE_CLOSED;

        // init aicp
        stream->aicp            = aicp? aicp : tb_aicp();
        tb_assert_and_check_break(stream->aicp);

        // init url
        if (!tb_url_init(&stream->base.url)) break;
        ok_url = tb_true;

        // init rcache
        if (!tb_scoped_buffer_init(&stream->rcache_data)) break;
        stream->rcache_maxn = rcache;
        ok_rcache = tb_true;

        // init wcache
        if (!tb_scoped_buffer_init(&stream->wcache_data)) break;
        stream->wcache_maxn = wcache;

        // ok
        ok = tb_true;

    } while (0);

    // failed? 
    if (!ok)
    {
        // exit rcache
        if (ok_rcache) tb_scoped_buffer_exit(&stream->rcache_data);

        // exit url
        if (ok_url) tb_url_exit(&stream->base.url);
    }

    // ok?
    return ok;
}
tb_async_stream_t* tb_async_stream_init_from_url(tb_aicp_t* aicp, tb_char_t const* url)
{
    // check
    tb_assert_and_check_return_val(url, tb_null);

    // the init
    static tb_async_stream_t* (*s_init[])() = 
    {
        tb_null
    ,   tb_async_stream_init_file
    ,   tb_async_stream_init_sock
    ,   tb_async_stream_init_http
    ,   tb_async_stream_init_data
    };

    // probe protocol
    tb_size_t protocol = tb_url_protocol_probe(url);
    tb_assert_static(TB_URL_PROTOCOL_FILE == TB_STREAM_TYPE_FILE);
    tb_assert_static(TB_URL_PROTOCOL_HTTP == TB_STREAM_TYPE_HTTP);
    tb_assert_static(TB_URL_PROTOCOL_SOCK == TB_STREAM_TYPE_SOCK);
    tb_assert_static(TB_URL_PROTOCOL_DATA == TB_STREAM_TYPE_DATA);

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
    tb_async_stream_t*  stream = tb_null;
    do
    {
        // init stream
        stream = s_init[type](aicp);
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
        if (stream) tb_async_stream_exit(stream);
        stream = tb_null;
    }

    // ok?
    return stream;
}
tb_bool_t tb_async_stream_exit(tb_async_stream_t* stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_false);

    // trace
    tb_trace_d("exit: %s: ..", tb_url_get(&stream->base.url));

    // kill it first
    tb_stream_kill(stream);

    // try closing it
    tb_size_t tryn = 30;
    tb_bool_t ok = tb_false;
    while (!(ok = tb_async_stream_clos_try(stream)) && tryn--)
    {
        // wait some time
        tb_msleep(200);
    }

    // close failed?
    if (!ok)
    {
        // trace
        tb_trace_e("exit: %s: failed!", tb_url_get(&stream->base.url));
        return tb_false;
    }

    // exit it
    if (stream->exit && !stream->exit(stream)) return tb_false;

    // exit url
    tb_url_exit(&stream->base.url);

    // exit rcache
    tb_scoped_buffer_exit(&stream->rcache_data);

    // exit wcache
    tb_scoped_buffer_exit(&stream->wcache_data);

    // free it
    tb_free(stream);

    // trace
    tb_trace_d("exit: ok");

    // ok
    return tb_true;
}
tb_bool_t tb_async_stream_open_try(tb_async_stream_t* stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_false);

    // not supported?
    if (!stream->open_try) return tb_stream_is_opened(stream);
     
    // trace
    tb_trace_d("open: try: %s: ..", tb_url_get(&stream->base.url));

    // set opening
    tb_size_t state = tb_atomic_fetch_and_pset(&stream->base.istate, TB_STATE_CLOSED, TB_STATE_OPENING);

    // opened?
    tb_check_return_val(state != TB_STATE_OPENED, tb_true);

    // must be closed
    tb_assert_and_check_return_val(state == TB_STATE_CLOSED, tb_false);

    // try opening it
    tb_bool_t ok = stream->open_try(stream);

    // trace
    tb_trace_d("open: try: %s: %s", tb_url_get(&stream->base.url), ok? "ok" : "no");

    // ok?
    return ok;
}
tb_bool_t tb_async_stream_open_(tb_async_stream_t* stream, tb_async_stream_open_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->open && func, tb_false);
    
    // trace
    tb_trace_d("open: %s: ..", tb_url_get(&stream->base.url));

    // try opening ok? done func directly
    if (tb_async_stream_open_try(stream))
    {
        // done func
        func(stream, TB_STATE_OK, priv);
        return tb_true;
    }

    // set opening
    tb_size_t state = tb_atomic_fetch_and_pset(&stream->base.istate, TB_STATE_CLOSED, TB_STATE_OPENING);

    // must be closed
    tb_assert_and_check_return_val(state == TB_STATE_CLOSED, tb_false);

    // open it
    return stream->open(stream, func, priv);
}
tb_bool_t tb_async_stream_clos_try(tb_async_stream_t* stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_false);

    // not supported?
    if (!stream->clos_try) return tb_stream_is_closed(stream);
     
    // trace
    tb_trace_d("clos: try: %s: ..", tb_url_get(&stream->base.url));

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // closed?
        if (TB_STATE_CLOSED == tb_atomic_get(&stream->base.istate))
        {
            ok = tb_true;
            break;
        }

        // try closing it
        ok = stream->clos_try(stream);

    } while (0);

    // trace
    tb_trace_d("clos: try: %s: %s", tb_url_get(&stream->base.url), ok? "ok" : "no");
         
    // ok?
    return ok;
}
tb_bool_t tb_async_stream_clos_(tb_async_stream_t* stream, tb_async_stream_clos_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->clos && func, tb_false);

    // trace
    tb_trace_d("clos: %s: ..", tb_url_get(&stream->base.url));

    // try closing ok? done func directly
    if (tb_async_stream_clos_try(stream))
    {
        // done func
        func(stream, TB_STATE_OK, priv);
        return tb_true;
    }

    // save debug info
#ifdef __tb_debug__
    stream->func = func_;
    stream->file = file_;
    stream->line = line_;
#endif

    // clos it
    return stream->clos(stream, func, priv);
}
tb_bool_t tb_async_stream_read_(tb_async_stream_t* stream, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // read it
    return tb_async_stream_read_after_(stream, 0, size, func, priv __tb_debug_args__);
}
tb_bool_t tb_async_stream_writ_(tb_async_stream_t* stream, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // writ it
    return tb_async_stream_writ_after_(stream, 0, data, size, func, priv __tb_debug_args__);
}
tb_bool_t tb_async_stream_seek_(tb_async_stream_t* stream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->seek && func, tb_false);
    
    // check state
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&stream->base.istate), tb_false);

    // save debug info
#ifdef __tb_debug__
    stream->func = func_;
    stream->file = file_;
    stream->line = line_;
#endif

    // have writed cache? sync it first
    if (stream->wcache_maxn && tb_scoped_buffer_size(&stream->wcache_data))
    {
        // init sync and seek
        stream->sync_and.seek.func = func;
        stream->sync_and.seek.priv = priv;
        stream->sync_and.seek.offset = offset;
        return tb_async_stream_sync_(stream, tb_false, tb_async_stream_sync_seek_func, &stream->sync_and.seek __tb_debug_args__);
    }

    // offset be not modified?
    if (tb_stream_offset(stream) == offset)
    {
        func(stream, TB_STATE_OK, offset, priv);
        return tb_true;
    }

    // seek it
    return stream->seek(stream, offset, func, priv);
}
tb_bool_t tb_async_stream_sync_(tb_async_stream_t* stream, tb_bool_t bclosing, tb_async_stream_sync_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->sync && func, tb_false);
    
    // check state
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&stream->base.istate), tb_false);

    // save debug info
#ifdef __tb_debug__
    stream->func = func_;
    stream->file = file_;
    stream->line = line_;
#endif
    
    // using cache?
    tb_bool_t ok = tb_false;
    if (stream->wcache_maxn)
    {
        // sync the cache data 
        tb_byte_t*  data = tb_scoped_buffer_data(&stream->wcache_data);
        tb_size_t   size = tb_scoped_buffer_size(&stream->wcache_data);
        if (data && size)
        {
            // writ the cache data
            stream->wcache_and.sync.func        = func;
            stream->wcache_and.sync.bclosing    = bclosing;
            ok = stream->writ(stream, 0, data, size, tb_async_stream_cache_sync_func, priv);
        }
        // sync it
        else ok = stream->sync(stream, bclosing, func, priv);
    }
    // sync it
    else ok = stream->sync(stream, bclosing, func, priv);

    // ok?
    return ok;
}
tb_bool_t tb_async_stream_task_(tb_async_stream_t* stream, tb_size_t delay, tb_async_stream_task_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->task && func, tb_false);
    
    // check state
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&stream->base.istate), tb_false);

    // save debug info
#ifdef __tb_debug__
    stream->func = func_;
    stream->file = file_;
    stream->line = line_;
#endif
 
    // task it
    return stream->task(stream, delay, func, priv);
}
tb_bool_t tb_async_stream_open_read_(tb_async_stream_t* stream, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->open && stream->read && func, tb_false);

    // no opened? open it first
    if (TB_STATE_CLOSED == tb_atomic_get(&stream->base.istate))
    {
        // init open and read
        stream->open_and.read.func = func;
        stream->open_and.read.priv = priv;
        stream->open_and.read.size = size;
        return tb_async_stream_open_(stream, tb_async_stream_open_read_func, &stream->open_and.read __tb_debug_args__);
    }

    // read it
    return tb_async_stream_read_(stream, size, func, priv __tb_debug_args__);
}
tb_bool_t tb_async_stream_open_writ_(tb_async_stream_t* stream, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->open && stream->writ && data && size && func, tb_false);

    // no opened? open it first
    if (TB_STATE_CLOSED == tb_atomic_get(&stream->base.istate))
    {
        // init open and writ
        stream->open_and.writ.func = func;
        stream->open_and.writ.priv = priv;
        stream->open_and.writ.data = data;
        stream->open_and.writ.size = size;
        return tb_async_stream_open_(stream, tb_async_stream_open_writ_func, &stream->open_and.writ __tb_debug_args__);
    }

    // writ it
    return tb_async_stream_writ_(stream, data, size, func, priv __tb_debug_args__);
}
tb_bool_t tb_async_stream_open_seek_(tb_async_stream_t* stream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->open && stream->seek && func, tb_false);

    // no opened? open it first
    if (TB_STATE_CLOSED == tb_atomic_get(&stream->base.istate))
    {
        // init open and seek
        stream->open_and.seek.func = func;
        stream->open_and.seek.priv = priv;
        stream->open_and.seek.offset = offset;
        return tb_async_stream_open_(stream, tb_async_stream_open_seek_func, &stream->open_and.seek __tb_debug_args__);
    }

    // seek it
    return tb_async_stream_seek_(stream, offset, func, priv __tb_debug_args__);
}
tb_bool_t tb_async_stream_read_after_(tb_async_stream_t* stream, tb_size_t delay, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->read && func, tb_false);
    
    // check state
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&stream->base.istate), tb_false);

    // save debug info
#ifdef __tb_debug__
    stream->func = func_;
    stream->file = file_;
    stream->line = line_;
#endif

    // have writed cache? sync it first
    if (stream->wcache_maxn && tb_scoped_buffer_size(&stream->wcache_data))
    {
        // init sync and read
        stream->sync_and.read.func = func;
        stream->sync_and.read.priv = priv;
        stream->sync_and.read.size = size;
        return tb_async_stream_sync_(stream, tb_false, tb_async_stream_sync_read_func, &stream->sync_and.read __tb_debug_args__);
    }

    // read it
    return tb_async_stream_cache_read_done(stream, delay, size, func, priv);
}
tb_bool_t tb_async_stream_writ_after_(tb_async_stream_t* stream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(stream && stream->writ && data && size && func, tb_false);
    
    // check state
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&stream->base.istate), tb_false);

    // save debug info
#ifdef __tb_debug__
    stream->func = func_;
    stream->file = file_;
    stream->line = line_;
#endif

    // writ it 
    return tb_async_stream_cache_writ_done(stream, delay, data, size, func, priv);
}
tb_aicp_t* tb_async_stream_aicp(tb_async_stream_t* stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_null);

    // the aicp
    return stream->aicp;
}
#ifdef __tb_debug__
tb_char_t const* tb_async_stream_func(tb_async_stream_t* stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_null);

    // the func
    return stream->func;
}
tb_char_t const* tb_async_stream_file(tb_async_stream_t* stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_null);

    // the file
    return stream->file;
}
tb_size_t tb_async_stream_line(tb_async_stream_t* stream)
{
    // check
    tb_assert_and_check_return_val(stream, 0);

    // the line
    return stream->line;
}
#endif
