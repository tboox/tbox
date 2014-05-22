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
 * @file        filter.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "async_stream_filter"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the filter stream type
typedef struct __tb_async_stream_filter_t
{
    // the base
    tb_async_stream_t                   base;

    // the filter 
    tb_stream_filter_t*                 filter;

    // the filter is referenced? need not exit it
    tb_uint32_t                         bref    : 1;

    // is reading now?
    tb_uint32_t                         bread   : 1;

    // the astream
    tb_async_stream_t*                  astream;

    // the read size
    tb_size_t                           size;

    // the offset
    tb_atomic64_t                       offset;

    // is closing for sync
    tb_bool_t                           bclosing;

    // the func
    union
    {
        tb_async_stream_open_func_t     open;
        tb_async_stream_read_func_t     read;
        tb_async_stream_writ_func_t     writ;
        tb_async_stream_sync_func_t     sync;
        tb_async_stream_task_func_t     task;

    }                                   func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_filter_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_async_stream_filter_t* tb_async_stream_filter_cast(tb_handle_t stream)
{
    tb_async_stream_t* astream = (tb_async_stream_t*)stream;
    tb_assert_and_check_return_val(astream && astream->base.type == TB_STREAM_TYPE_FLTR, tb_null);
    return (tb_async_stream_filter_t*)astream;
}
static tb_bool_t tb_async_stream_filter_open_func(tb_async_stream_t* astream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(astream, tb_false);

    // the stream
    tb_async_stream_filter_t* fstream = (tb_async_stream_filter_t*)priv;
    tb_assert_and_check_return_val(fstream && fstream->func.open, tb_false);

    // opened
    tb_atomic_set(&fstream->base.base.bopened, 1);

    // done func
    return fstream->func.open((tb_async_stream_t*)fstream, state, fstream->priv);
}
static tb_bool_t tb_async_stream_filter_open(tb_handle_t astream, tb_async_stream_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_filter_t* fstream = tb_async_stream_filter_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->astream, tb_false);

    // clear the mode
    fstream->bread = 0;

    // clear the offset
    tb_atomic64_set0(&fstream->offset);

    // have been opened?
    if (tb_stream_is_opened(fstream->astream)) 
    {
        // opened
        tb_atomic_set(&fstream->base.base.bopened, 1);

        // done func
        return func? func(astream, TB_STATE_OK, fstream->priv) : tb_true;
    }

    // check
    tb_assert_and_check_return_val(func, tb_false);

    // save func and priv
    fstream->priv       = priv;
    fstream->func.open  = func;

    // post open
    return tb_async_stream_open(fstream->astream, tb_async_stream_filter_open_func, astream);
}
static tb_bool_t tb_async_stream_filter_clos(tb_handle_t astream, tb_async_stream_clos_func_t func, tb_cpointer_t priv)
{   
    // check
    tb_async_stream_filter_t* fstream = tb_async_stream_filter_cast(astream);
    tb_assert_and_check_return_val(fstream, tb_false);

    // trace
    tb_trace_d("clos: ..");

    // noimpl
    tb_trace_noimpl();
    return tb_false;

#if 0
    // close stream
    if (fstream->astream) tb_async_stream_clos(fstream->astream, bcalling);

    // clear the filter
    if (fstream->filter) tb_stream_filter_cler(fstream->filter);

    // clear the mode
    fstream->bread = 0;

    // clear the offset
    tb_atomic64_set0(&fstream->offset);
#endif
}
static tb_bool_t tb_async_stream_filter_sync_read_func(tb_async_stream_t* astream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(astream, tb_false);

    // the stream
    tb_async_stream_filter_t* fstream = (tb_async_stream_filter_t*)priv;
    tb_assert_and_check_return_val(fstream && fstream->func.read, tb_false);

    // spak the filter
    tb_byte_t const*    data = tb_null;
    tb_long_t           spak = tb_stream_filter_spak(fstream->filter, tb_null, 0, &data, fstream->size, -1);
    
    // has output data?
    tb_bool_t ok = tb_false;
    if (spak > 0 && data)
    {   
        // save offset
        tb_atomic64_fetch_and_add(&fstream->offset, spak);

        // done data
        ok = fstream->func.read((tb_async_stream_t*)fstream, TB_STATE_OK, data, spak, fstream->size, fstream->priv);
    }
    // closed?
    else
    {
        // done closed
        fstream->func.read((tb_async_stream_t*)fstream, TB_STATE_CLOSED, tb_null, 0, fstream->size, fstream->priv);

        // break it
        // ok = tb_false;
    }

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_filter_read_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(astream, tb_false);

    // the stream
    tb_async_stream_filter_t* fstream = (tb_async_stream_filter_t*)priv;
    tb_assert_and_check_return_val(fstream && fstream->func.read, tb_false);

    // done filter
    tb_bool_t ok = tb_false;
    if (fstream->filter)
    {
        // done filter
        switch (state)
        {
        case TB_STATE_OK:
            {
                // spak the filter
                tb_long_t spak = tb_stream_filter_spak(fstream->filter, data, real, &data, size, 0);
                
                // has output data?
                if (spak > 0 && data)
                {
                    // save offset
                    tb_atomic64_fetch_and_add(&fstream->offset, spak);

                    // done data
                    ok = fstream->func.read((tb_async_stream_t*)fstream, TB_STATE_OK, data, spak, size, fstream->priv);
                }
                // no data? continue it
                else if (!spak) ok = tb_true;
                // closed?
                else
                {
                    // done closed
                    fstream->func.read((tb_async_stream_t*)fstream, TB_STATE_CLOSED, tb_null, 0, size, fstream->priv);

                    // break it
                    // ok = tb_false;
                }

                // eof and continue?
                if (tb_stream_filter_beof(fstream->filter) && ok)
                {
                    // done sync for reading
                    ok = tb_async_stream_task(fstream->astream, 0, tb_async_stream_filter_sync_read_func, fstream);
                    
                    // error? done error
                    if (!ok) fstream->func.read((tb_async_stream_t*)fstream, TB_STATE_UNKNOWN_ERROR, tb_null, 0, size, fstream->priv);

                    // need not read data, break it
                    ok = tb_false;
                }
            }
            break;
        case TB_STATE_CLOSED:
            {
                // done sync for reading
                ok = tb_async_stream_task(fstream->astream, 0, tb_async_stream_filter_sync_read_func, fstream);
                
                // error? done error
                if (!ok) fstream->func.read((tb_async_stream_t*)fstream, TB_STATE_UNKNOWN_ERROR, tb_null, 0, size, fstream->priv);
            }
            break;
        default:
            {
                // done closed or failed
                fstream->func.read((tb_async_stream_t*)fstream, state, tb_null, 0, size, fstream->priv);

                // break it
                // ok = tb_false;
            }
            break;
        }
    }
    // done func
    else ok = fstream->func.read((tb_async_stream_t*)fstream, state, data, real, size, fstream->priv);
 
    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_filter_read(tb_handle_t astream, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_filter_t* fstream = tb_async_stream_filter_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->astream && func, tb_false);

    // clear the offset if be writ mode now
    if (!fstream->bread) tb_atomic64_set0(&fstream->offset);

    // set read mode
    fstream->bread = 1;

    // save func and priv
    fstream->priv       = priv;
    fstream->func.read  = func;
    fstream->size       = size;

    // filter eof? flush the left data
    if (fstream->filter && tb_stream_filter_beof(fstream->filter))
        return tb_async_stream_task(fstream->astream, 0, tb_async_stream_filter_sync_read_func, fstream);

    // post read
    return tb_async_stream_read_after(fstream->astream, delay, size, tb_async_stream_filter_read_func, astream);
}
static tb_bool_t tb_async_stream_filter_writ_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(astream, tb_false);

    // the stream
    tb_async_stream_filter_t* fstream = (tb_async_stream_filter_t*)priv;
    tb_assert_and_check_return_val(fstream && fstream->func.writ, tb_false);

    // save offset
    if (real) tb_atomic64_fetch_and_add(&fstream->offset, real);

    // done func
    return fstream->func.writ((tb_async_stream_t*)fstream, state, data, real, size, fstream->priv);
}
static tb_bool_t tb_async_stream_filter_writ(tb_handle_t astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_filter_t* fstream = tb_async_stream_filter_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->astream && data && size && func, tb_false);

    // clear the offset if be read mode now
    if (fstream->bread) tb_atomic64_set0(&fstream->offset);

    // set writ mode
    fstream->bread = 0;

    // save func and priv
    fstream->priv       = priv;
    fstream->func.writ  = func;

    // done filter
    tb_bool_t ok = tb_true;
    if (fstream->filter)
    {
        // spak the filter
        tb_long_t real = tb_stream_filter_spak(fstream->filter, data, size, &data, size, 0);
        
        // has data? 
        if (real > 0 && data)
        {
            // post writ
            ok = tb_async_stream_writ_after(fstream->astream, delay, data, real, tb_async_stream_filter_writ_func, astream);
        }
        // no data or end? continue to writ or sync
        else
        {
            // done func, no data and finished
            ok = func((tb_async_stream_t*)fstream, TB_STATE_OK, tb_null, 0, 0, fstream->priv);
        }
    }
    // post writ
    else ok = tb_async_stream_writ_after(fstream->astream, delay, data, size, tb_async_stream_filter_writ_func, astream);

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_filter_sync_func(tb_async_stream_t* astream, tb_size_t state, tb_bool_t bclosing, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(astream, tb_false);

    // the stream
    tb_async_stream_filter_t* fstream = (tb_async_stream_filter_t*)priv;
    tb_assert_and_check_return_val(fstream && fstream->func.sync, tb_false);

    // done func
    return fstream->func.sync((tb_async_stream_t*)fstream, state, bclosing, fstream->priv);
}
static tb_bool_t tb_async_stream_filter_writ_sync_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(astream && data && size, tb_false);

    // the stream
    tb_async_stream_filter_t* fstream = (tb_async_stream_filter_t*)priv;
    tb_assert_and_check_return_val(fstream && fstream->func.sync, tb_false);

    // save offset
    if (real) tb_atomic64_fetch_and_add(&fstream->offset, real);

    // not finished? continue it
    tb_bool_t ok = tb_false;
    if (state == TB_STATE_OK && real < size) ok = tb_true;
    // ok? sync it
    else if (state == TB_STATE_OK && real == size)
    {
        // post sync
        ok = tb_async_stream_sync(fstream->astream, fstream->bclosing, tb_async_stream_filter_sync_func, fstream);

        // failed? done func
        if (!ok) ok = fstream->func.sync((tb_async_stream_t*)fstream, TB_STATE_UNKNOWN_ERROR, fstream->bclosing, fstream->priv);
    }
    // failed?
    else
    {
        // failed? done func
        ok = fstream->func.sync((tb_async_stream_t*)fstream, state != TB_STATE_OK? state : TB_STATE_UNKNOWN_ERROR, fstream->bclosing, fstream->priv);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_filter_sync(tb_handle_t astream, tb_bool_t bclosing, tb_async_stream_sync_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_filter_t* fstream = tb_async_stream_filter_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->astream && func, tb_false);

    // clear the offset if be read mode now
    if (fstream->bread) tb_atomic64_set0(&fstream->offset);
 
    // set writ mode
    fstream->bread = 0;

    // save func and priv
    fstream->priv       = priv;
    fstream->func.sync  = func;
    fstream->bclosing   = bclosing;

    // done filter
    tb_bool_t ok = tb_true;
    if (fstream->filter)
    {
        // spak the filter
        tb_byte_t const*    data = tb_null;
        tb_long_t           real = tb_stream_filter_spak(fstream->filter, tb_null, 0, &data, 0, bclosing? -1 : 1);
        
        // has data? post writ and sync
        if (real > 0 && data)
            ok = tb_async_stream_writ(fstream->astream, data, real, tb_async_stream_filter_writ_sync_func, astream);
    }
    // post sync
    else ok = tb_async_stream_sync(fstream->astream, bclosing, tb_async_stream_filter_sync_func, astream);

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_filter_task_func(tb_async_stream_t* astream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(astream, tb_false);

    // the stream
    tb_async_stream_filter_t* fstream = (tb_async_stream_filter_t*)priv;
    tb_assert_and_check_return_val(fstream && fstream->func.task, tb_false);

    // done func
    return fstream->func.task((tb_async_stream_t*)fstream, state, fstream->priv);
}
static tb_bool_t tb_async_stream_filter_task(tb_handle_t astream, tb_size_t delay, tb_async_stream_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_filter_t* fstream = tb_async_stream_filter_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->astream && func, tb_false);

    // save func and priv
    fstream->priv       = priv;
    fstream->func.task  = func;

    // post task
    return tb_async_stream_task(fstream->astream, delay, tb_async_stream_filter_task_func, astream);
}
static tb_void_t tb_async_stream_filter_kill(tb_handle_t astream)
{   
    // check
    tb_async_stream_filter_t* fstream = tb_async_stream_filter_cast(astream);
    tb_assert_and_check_return(fstream);

    // kill stream
    if (fstream->astream) tb_stream_kill(fstream->astream);
}
static tb_bool_t tb_async_stream_filter_exit(tb_handle_t astream)
{   
    // check
    tb_async_stream_filter_t* fstream = tb_async_stream_filter_cast(astream);
    tb_assert_and_check_return_val(fstream, tb_false);

    // exit it
    if (!fstream->bref && fstream->filter) tb_stream_filter_exit(fstream->filter);
    fstream->filter = tb_null;
    fstream->bref = 0;

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_filter_ctrl(tb_handle_t astream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_async_stream_filter_t* fstream = tb_async_stream_filter_cast(astream);
    tb_assert_and_check_return_val(fstream, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_opened(astream), tb_false);

            // get offset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);
            *poffset = (tb_hize_t)tb_atomic64_get(&fstream->offset);
            return tb_true;
        }
    case TB_STREAM_CTRL_FLTR_SET_STREAM:
        {
            // check
            tb_assert_and_check_return_val(!tb_stream_is_opened(astream), tb_false);

            // set astream
            tb_handle_t astream = (tb_async_stream_t*)tb_va_arg(args, tb_async_stream_t*);
            fstream->astream = astream;
            return tb_true;
        }
    case TB_STREAM_CTRL_FLTR_GET_STREAM:
        {
            // get astream
            tb_async_stream_t** pastream = (tb_async_stream_t**)tb_va_arg(args, tb_async_stream_t**);
            tb_assert_and_check_return_val(pastream, tb_false);
            *pastream = fstream->astream;
            return tb_true;
        }
    case TB_STREAM_CTRL_FLTR_SET_FILTER:
        {
            // check
            tb_assert_and_check_return_val(!tb_stream_is_opened(astream), tb_false);

            //  exit filter first if exists
            if (!fstream->bref && fstream->filter) tb_stream_filter_exit(fstream->filter);

            // set filter
            tb_stream_filter_t* filter = (tb_stream_filter_t*)tb_va_arg(args, tb_stream_filter_t*);
            fstream->filter = filter;
            fstream->bref = filter? 1 : 0;
            return tb_true;
        }
    case TB_STREAM_CTRL_FLTR_GET_FILTER:
        {
            // get filter
            tb_stream_filter_t** phandle = (tb_stream_filter_t**)tb_va_arg(args, tb_stream_filter_t**);
            tb_assert_and_check_return_val(phandle, tb_false);
            *phandle = fstream->filter;
            return tb_true;
        }
    default:
        break;
    }
    return tb_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_async_stream_t* tb_async_stream_init_filter(tb_aicp_t* aicp)
{
    // check
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t                   ok = tb_false;
    tb_async_stream_filter_t*   fstream = tb_null;
    do
    {
        // make stream
        fstream = (tb_async_stream_filter_t*)tb_malloc0(sizeof(tb_async_stream_filter_t));
        tb_assert_and_check_break(fstream);

        // init stream
        if (!tb_async_stream_init((tb_async_stream_t*)fstream, aicp, TB_STREAM_TYPE_FLTR, 0, 0)) break;
        fstream->base.open      = tb_async_stream_filter_open;
        fstream->base.read      = tb_async_stream_filter_read;
        fstream->base.writ      = tb_async_stream_filter_writ;
        fstream->base.sync      = tb_async_stream_filter_sync;
        fstream->base.task      = tb_async_stream_filter_task;
        fstream->base.clos      = tb_async_stream_filter_clos;
        fstream->base.exit      = tb_async_stream_filter_exit;
        fstream->base.base.kill = tb_async_stream_filter_kill;
        fstream->base.base.ctrl = tb_async_stream_filter_ctrl;

        // ok 
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (fstream) tb_async_stream_exit((tb_async_stream_t*)fstream);
        fstream = tb_null;
    }

    // ok
    return (tb_async_stream_t*)fstream;
}
tb_async_stream_t* tb_async_stream_init_filter_from_null(tb_async_stream_t* astream)
{
    // check
    tb_assert_and_check_return_val(astream, tb_null);

    // the aicp
    tb_aicp_t* aicp = tb_async_stream_aicp(astream);
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  fstream = tb_null;
    do
    {
        // init stream
        fstream = tb_async_stream_init_filter(aicp);
        tb_assert_and_check_break(fstream);

        // set astream
        if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) break;

        // ok 
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (fstream) tb_async_stream_exit((tb_async_stream_t*)fstream);
        fstream = tb_null;
    }

    // ok?
    return fstream;
}
#ifdef TB_CONFIG_MODULE_HAVE_ZIP
tb_async_stream_t* tb_async_stream_init_filter_from_zip(tb_async_stream_t* astream, tb_size_t algo, tb_size_t action)
{
    // check
    tb_assert_and_check_return_val(astream, tb_null);

    // the aicp
    tb_aicp_t* aicp = tb_async_stream_aicp(astream);
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  fstream = tb_null;
    do
    {
        // init stream
        fstream = tb_async_stream_init_filter(aicp);
        tb_assert_and_check_break(fstream);

        // set astream
        if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) break;

        // set filter
        ((tb_async_stream_filter_t*)fstream)->bref = 0;
        ((tb_async_stream_filter_t*)fstream)->filter = tb_stream_filter_init_from_zip(algo, action);
        tb_assert_and_check_break(((tb_async_stream_filter_t*)fstream)->filter);
        
        // ok 
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (fstream) tb_async_stream_exit(fstream);
        fstream = tb_null;
    }

    // ok?
    return fstream;
}
#endif
tb_async_stream_t* tb_async_stream_init_filter_from_cache(tb_async_stream_t* astream, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(astream, tb_null);

    // the aicp
    tb_aicp_t* aicp = tb_async_stream_aicp(astream);
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  fstream = tb_null;
    do
    {
        // init stream
        fstream = tb_async_stream_init_filter(aicp);
        tb_assert_and_check_break(fstream);

        // set astream
        if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) break;

        // set filter
        ((tb_async_stream_filter_t*)fstream)->bref = 0;
        ((tb_async_stream_filter_t*)fstream)->filter = tb_stream_filter_init_from_cache(size);
        tb_assert_and_check_break(((tb_async_stream_filter_t*)fstream)->filter);
        
        // ok 
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (fstream) tb_async_stream_exit(fstream);
        fstream = tb_null;
    }

    // ok?
    return fstream;
}
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
tb_async_stream_t* tb_async_stream_init_filter_from_charset(tb_async_stream_t* astream, tb_size_t fr, tb_size_t to)
{
    // check
    tb_assert_and_check_return_val(astream, tb_null);

    // the aicp
    tb_aicp_t* aicp = tb_async_stream_aicp(astream);
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  fstream = tb_null;
    do
    {
        // init stream
        fstream = tb_async_stream_init_filter(aicp);
        tb_assert_and_check_break(fstream);

        // set astream
        if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) break;

        // set filter
        ((tb_async_stream_filter_t*)fstream)->bref = 0;
        ((tb_async_stream_filter_t*)fstream)->filter = tb_stream_filter_init_from_charset(fr, to);
        tb_assert_and_check_break(((tb_async_stream_filter_t*)fstream)->filter);
        
        // ok 
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (fstream) tb_async_stream_exit(fstream);
        fstream = tb_null;
    }

    // ok?
    return fstream;
}
#endif
tb_async_stream_t* tb_async_stream_init_filter_from_chunked(tb_async_stream_t* astream, tb_bool_t dechunked)
{
    // check
    tb_assert_and_check_return_val(astream, tb_null);

    // the aicp
    tb_aicp_t* aicp = tb_async_stream_aicp(astream);
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  fstream = tb_null;
    do
    {
        // init stream
        fstream = tb_async_stream_init_filter(aicp);
        tb_assert_and_check_break(fstream);

        // set astream
        if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FLTR_SET_STREAM, astream)) break;

        // set filter
        ((tb_async_stream_filter_t*)fstream)->bref = 0;
        ((tb_async_stream_filter_t*)fstream)->filter = tb_stream_filter_init_from_chunked(dechunked);
        tb_assert_and_check_break(((tb_async_stream_filter_t*)fstream)->filter);
        
        // ok 
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (fstream) tb_async_stream_exit(fstream);
        fstream = tb_null;
    }

    // ok?
    return fstream;
}
