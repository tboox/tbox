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
 * @ingroup     stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "stream_filter"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "filter.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_stream_filter_open(tb_stream_filter_t* filter)
{
    // check
    tb_assert_and_check_return_val(filter, tb_false);

    // opened?
    tb_check_return_val(!filter->bopened, tb_true);

    // open it
    filter->bopened = filter->open? filter->open(filter) : tb_true;

    // ok?
    return filter->bopened;
}
tb_void_t tb_stream_filter_clos(tb_stream_filter_t* filter)
{
    // check
    tb_assert_and_check_return(filter);

    // opened?
    tb_check_return(filter->bopened);

    // clos it
    if (filter->clos) filter->clos(filter);

    // clear eof
    filter->beof = tb_false;
    
    // clear limit
    filter->limit = -1;
    
    // clear offset
    filter->offset = 0;
    
    // exit idata
    tb_scoped_buffer_clear(&filter->idata);

    // exit odata
    tb_queue_buffer_clear(&filter->odata);

    // closed
    filter->bopened = tb_false;
}
tb_void_t tb_stream_filter_exit(tb_stream_filter_t* filter)
{
    // check
    tb_assert_and_check_return(filter);
    
    // exit it
    if (filter->exit) filter->exit(filter);

    // exit idata
    tb_scoped_buffer_exit(&filter->idata);

    // exit odata
    tb_queue_buffer_exit(&filter->odata);

    // free it
    tb_free(filter);
}
tb_bool_t tb_stream_filter_ctrl(tb_stream_filter_t* filter, tb_size_t ctrl, ...)
{
    // check
    tb_assert_and_check_return_val(filter && filter->ctrl && ctrl, tb_false);

    // init args
    tb_va_list_t args;
    tb_va_start(args, ctrl);

    // ctrl it
    tb_bool_t ok = filter->ctrl(filter, ctrl, args);

    // exit args
    tb_va_end(args);

    // ok?
    return ok;
}
tb_long_t tb_stream_filter_spak(tb_stream_filter_t* filter, tb_byte_t const* data, tb_size_t size, tb_byte_t const** pdata, tb_size_t need, tb_long_t sync)
{
    // check
    tb_assert_and_check_return_val(filter && filter->spak && pdata, -1);

    // init odata
    *pdata = tb_null;

    // save the input offset
    filter->offset += size;

    // eof?
    if (filter->limit >= 0 && filter->offset == filter->limit)
        filter->beof = tb_true;

    // eof? sync it
    if (filter->beof) sync = -1;

    // the idata
    tb_byte_t const*    idata = tb_scoped_buffer_data(&filter->idata);
    tb_size_t           isize = tb_scoped_buffer_size(&filter->idata);
    if (data && size)
    {
        // append data to cache if have the cache data
        if (idata && isize) 
        {
            // append data
            idata = tb_scoped_buffer_memncat(&filter->idata, data, size);
            isize = tb_scoped_buffer_size(&filter->idata);
        }
        // using the data directly if no cache data
        else
        {
            idata = data;
            isize = size;
        }
    }
    // sync data if null
    else
    {
        // check sync
        tb_assert_and_check_return_val(sync, 0);
    }

    // the need
    if (!need) need = tb_max(size, tb_queue_buffer_maxn(&filter->odata));
    tb_assert_and_check_return_val(need, -1);

    // init pull
    tb_size_t   omaxn = 0;
    tb_byte_t*  odata = tb_queue_buffer_pull_init(&filter->odata, &omaxn);
    if (odata)
    {
        // the osize
        tb_long_t osize = omaxn >= need? need : 0;

        // exit pull
        if (odata) tb_queue_buffer_pull_exit(&filter->odata, osize > 0? osize : 0);

        // enough? return it directly 
        if (osize > 0)
        {
            *pdata = odata;
            return osize;
        }
    }

    // grow odata maxn if not enough
    if (need > tb_queue_buffer_maxn(&filter->odata))
        tb_queue_buffer_resize(&filter->odata, need);

    // the odata
    omaxn = 0;
    odata = tb_queue_buffer_push_init(&filter->odata, &omaxn);
    tb_assert_and_check_return_val(odata && omaxn, -1);

    // init stream
    tb_static_stream_t istream = {0};
    tb_static_stream_t ostream = {0};
    if (idata && isize) 
    {
        // @note istream maybe null for sync the end data
        if (!tb_static_stream_init(&istream, (tb_byte_t*)idata, isize)) return -1;
    }
    if (!tb_static_stream_init(&ostream, (tb_byte_t*)odata, omaxn)) return -1;

    // spak data
    tb_long_t osize = filter->spak(filter, &istream, &ostream, sync);

    // eof?
    if (osize < 0) filter->beof = tb_true;

    // no data and eof?
    if (!osize && !tb_static_stream_left(&istream) && filter->beof) osize = -1;

    // eof? sync it
    if (filter->beof) sync = -1;

    // exit odata
    tb_queue_buffer_push_exit(&filter->odata, osize > 0? osize : 0);

    // have the left idata? 
    tb_size_t left = tb_static_stream_left(&istream);
    if (left) 
    {
        // move to the cache head if idata is belong to the cache
        if (idata != data) tb_scoped_buffer_memnmov(&filter->idata, tb_static_stream_offset(&istream), left);
        // append to the cache if idata is not belong to the cache
        else tb_scoped_buffer_memncat(&filter->idata, tb_static_stream_pos(&istream), left);
    }
    // clear the cache
    else tb_scoped_buffer_clear(&filter->idata);

    // init pull
    omaxn = 0;
    odata = tb_queue_buffer_pull_init(&filter->odata, &omaxn);

    // no sync? cache the output data
    if (!sync) osize = omaxn >= need? need : 0;
    // sync and has data? return it directly 
    else if (omaxn) osize = tb_min(omaxn, need);
    // sync, no data or end?
//  else osize = osize;

    // exit pull
    if (odata) tb_queue_buffer_pull_exit(&filter->odata, osize > 0? osize : 0);

    // return it if have the odata
    if (osize > 0) *pdata = odata;

    // trace
    tb_trace_d("spak: %ld, ileft: %lu, oleft: %lu, offset: %llu, limit: %lld", osize, tb_scoped_buffer_size(&filter->idata), tb_queue_buffer_size(&filter->odata), filter->offset, filter->limit);

    // ok?
    return osize;
}
tb_bool_t tb_stream_filter_push(tb_stream_filter_t* filter, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(filter && data && size, tb_false);

    // push data
    tb_bool_t ok = tb_scoped_buffer_memncat(&filter->idata, data, size)? tb_true : tb_false;

    // save the input offset
    if (ok) filter->offset += size;

    // ok?
    return ok;
}
tb_bool_t tb_stream_filter_beof(tb_stream_filter_t* filter)
{
    // check
    tb_assert_and_check_return_val(filter, tb_false);

    // is eof?
    return filter->beof;
}
tb_void_t tb_stream_filter_limit(tb_stream_filter_t* filter, tb_hong_t limit)
{
    // check
    tb_assert_and_check_return(filter);

    // limit the input size
    filter->limit = limit;
}
