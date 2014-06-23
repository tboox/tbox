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
 * @file        data.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../asio/asio.h"
#include "../../utils/utils.h"
#include "../../memory/memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the data stream type
typedef struct __tb_basic_stream_data_t
{
    // the base
    tb_basic_stream_t       base;

    // the data
    tb_byte_t*              data;

    // the head
    tb_byte_t*              head;

    // the size
    tb_size_t               size;

    // the data is referenced?
    tb_bool_t               bref;

}tb_basic_stream_data_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_basic_stream_data_t* tb_basic_stream_data_cast(tb_basic_stream_t* stream)
{
    tb_assert_and_check_return_val(stream && stream->type == TB_STREAM_TYPE_DATA, tb_null);
    return (tb_basic_stream_data_t*)stream;
}
static tb_bool_t tb_basic_stream_data_open(tb_basic_stream_t* stream)
{
    // check
    tb_basic_stream_data_t* dstream = tb_basic_stream_data_cast(stream);
    tb_assert_and_check_return_val(dstream && dstream->data && dstream->size, tb_false);

    // init head
    dstream->head = dstream->data;

    // ok
    return tb_true;
}
static tb_bool_t tb_basic_stream_data_clos(tb_basic_stream_t* stream)
{
    // check
    tb_basic_stream_data_t* dstream = tb_basic_stream_data_cast(stream);
    tb_assert_and_check_return_val(dstream, tb_false);
    
    // clear head
    dstream->head = tb_null;

    // ok
    return tb_true;
}
static tb_void_t tb_basic_stream_data_exit(tb_basic_stream_t* stream)
{
    // check
    tb_basic_stream_data_t* dstream = tb_basic_stream_data_cast(stream);
    tb_assert_and_check_return(dstream);
    
    // clear head
    dstream->head = tb_null;

    // exit data
    if (dstream->data && !dstream->bref) tb_free(dstream->data);
    dstream->data = tb_null;
    dstream->size = 0;
}
static tb_long_t tb_basic_stream_data_read(tb_basic_stream_t* stream, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_basic_stream_data_t* dstream = tb_basic_stream_data_cast(stream);
    tb_assert_and_check_return_val(dstream && dstream->data && dstream->head, -1);

    // check
    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    // the left
    tb_size_t left = dstream->data + dstream->size - dstream->head;

    // the need
    if (size > left) size = left;

    // read data
    if (size) tb_memcpy(data, dstream->head, size);

    // save head
    dstream->head += size;

    // ok?
    return (tb_long_t)(size);
}
static tb_long_t tb_basic_stream_data_writ(tb_basic_stream_t* stream, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_basic_stream_data_t* dstream = tb_basic_stream_data_cast(stream);
    tb_assert_and_check_return_val(dstream && dstream->data && dstream->head, -1);

    // check
    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    // the left
    tb_size_t left = dstream->data + dstream->size - dstream->head;

    // the need
    if (size > left) size = left;

    // writ data
    if (size) tb_memcpy(dstream->head, data, size);

    // save head
    dstream->head += size;

    // ok?
    return left? (tb_long_t)(size) : -1; // force end if full
}
static tb_bool_t tb_basic_stream_data_seek(tb_basic_stream_t* stream, tb_hize_t offset)
{
    // check
    tb_basic_stream_data_t* dstream = tb_basic_stream_data_cast(stream);
    tb_assert_and_check_return_val(dstream && offset <= dstream->size, tb_false);

    // seek 
    dstream->head = dstream->data + offset;

    // ok
    return tb_true;
}
static tb_long_t tb_basic_stream_data_wait(tb_basic_stream_t* stream, tb_size_t wait, tb_long_t timeout)
{
    // check
    tb_basic_stream_data_t* dstream = tb_basic_stream_data_cast(stream);
    tb_assert_and_check_return_val(dstream && dstream->head <= dstream->data + dstream->size, -1);

    // wait 
    tb_long_t aioe = 0;
    if (!tb_basic_stream_beof(stream))
    {
        if (wait & TB_BASIC_STREAM_WAIT_READ) aioe |= TB_BASIC_STREAM_WAIT_READ;
        if (wait & TB_BASIC_STREAM_WAIT_WRIT) aioe |= TB_BASIC_STREAM_WAIT_WRIT;
    }

    // ok?
    return aioe;
}
static tb_bool_t tb_basic_stream_data_ctrl(tb_basic_stream_t* stream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_basic_stream_data_t* dstream = tb_basic_stream_data_cast(stream);
    tb_assert_and_check_return_val(dstream, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            // the psize
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize, tb_false);

            // get size
            *psize = dstream->size;
            return tb_true;
        }   
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // the poffset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);

            // get offset
            *poffset = dstream->base.offset;
            return tb_true;
        }
    case TB_STREAM_CTRL_DATA_SET_DATA:
        {
            // exit data first if exists
            if (dstream->data && !dstream->bref) tb_free(dstream->data);

            // save data
            dstream->data = (tb_byte_t*)tb_va_arg(args, tb_byte_t*);
            dstream->size = (tb_size_t)tb_va_arg(args, tb_size_t);
            dstream->head = tb_null;
            dstream->bref = tb_true;

            // check
            tb_assert_and_check_return_val(dstream->data && dstream->size, tb_false);
            return tb_true;
        }
    case TB_STREAM_CTRL_SET_URL:
        {
            // check
            tb_assert_and_check_return_val(tb_basic_stream_is_closed(stream), tb_false);

            // set url
            tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(url, tb_false); 
            
            // the url size
            tb_size_t url_size = tb_strlen(url);
            tb_assert_and_check_return_val(url_size > 7, tb_false);

            // the base64 data and size
            tb_char_t const*    base64_data = url + 7;
            tb_size_t           base64_size = url_size - 7;

            // make data
            tb_size_t   maxn = base64_size;
            tb_byte_t*  data = (tb_byte_t*)tb_malloc(maxn); 
            tb_assert_and_check_return_val(data, tb_false);

            // decode base64 data
            tb_size_t   size = tb_base64_decode(base64_data, base64_size, data, maxn);
            tb_assert_and_check_return_val(size, tb_false);

            // exit data first if exists
            if (dstream->data && !dstream->bref) tb_free(dstream->data);

            // save data
            dstream->data = data;
            dstream->size = size;
            dstream->bref = tb_false;
            dstream->head = tb_null;

            // ok
            return tb_true;
        }
        break;
    default:
        break;
    }
    return tb_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interface implementation
 */
tb_basic_stream_t* tb_basic_stream_init_data()
{
    // done
    tb_bool_t           ok = tb_false;
    tb_basic_stream_t*  stream = tb_null;
    do
    {
        // make stream
        stream = (tb_basic_stream_t*)tb_malloc0(sizeof(tb_basic_stream_data_t));
        tb_assert_and_check_break(stream);

        // init stream
        if (!tb_basic_stream_init(stream, TB_STREAM_TYPE_DATA, 0)) break;

        // init func
        stream->open        = tb_basic_stream_data_open;
        stream->clos        = tb_basic_stream_data_clos;
        stream->exit        = tb_basic_stream_data_exit;
        stream->read        = tb_basic_stream_data_read;
        stream->writ        = tb_basic_stream_data_writ;
        stream->seek        = tb_basic_stream_data_seek;
        stream->wait        = tb_basic_stream_data_wait;
        stream->ctrl        = tb_basic_stream_data_ctrl;

        // ok
        ok = tb_true;

    } while (0);

    // failed? 
    if (!ok)
    {
        // exit it
        if (stream) tb_basic_stream_exit(stream);
        stream = tb_null;
    }

    // ok?
    return stream;
}
tb_basic_stream_t* tb_basic_stream_init_from_data(tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(data && size, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_basic_stream_t*  stream = tb_null;
    do
    {
        // init stream
        stream = tb_basic_stream_init_data();
        tb_assert_and_check_break(stream);

        // set data & size
        if (!tb_basic_stream_ctrl(stream, TB_STREAM_CTRL_DATA_SET_DATA, data, size)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed? 
    if (!ok)
    {
        // exit it
        if (stream) tb_basic_stream_exit(stream);
        stream = tb_null;
    }

    // ok
    return stream;
}
