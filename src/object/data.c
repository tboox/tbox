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
 * @ingroup     object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "object_data"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the data type
typedef struct __tb_object_data_t
{
    // the object base
    tb_object_t         base;

    // the data buffer
    tb_buffer_t      buff;

}tb_object_data_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_object_data_t* tb_object_data_cast(tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_DATA, tb_null);

    // cast
    return (tb_object_data_t*)object;
}
static tb_object_t* tb_object_data_copy(tb_object_t* object)
{
    return tb_object_data_init_from_data(tb_object_data_getp(object), tb_object_data_size(object));
}
static tb_void_t tb_object_data_exit(tb_object_t* object)
{
    tb_object_data_t* data = tb_object_data_cast(object);
    if (data) 
    {
        tb_buffer_exit(&data->buff);
        tb_object_pool_del(tb_object_pool(), (tb_object_t*)data);
    }
}
static tb_void_t tb_object_data_cler(tb_object_t* object)
{
    tb_object_data_t* data = tb_object_data_cast(object);
    if (data) tb_buffer_clear(&data->buff);
}
static tb_object_data_t* tb_object_data_init_base()
{
    // make
    tb_object_data_t* data = (tb_object_data_t*)tb_object_pool_get(tb_object_pool(), sizeof(tb_object_data_t), TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_DATA);
    tb_assert_and_check_return_val(data, tb_null);

    // init base
    data->base.copy = tb_object_data_copy;
    data->base.cler = tb_object_data_cler;
    data->base.exit = tb_object_data_exit;

    // ok
    return data;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_t* tb_object_data_init_from_url(tb_char_t const* url)
{
    // check
    tb_assert_and_check_return_val(url, tb_null);

    // init stream
    tb_stream_ref_t stream = tb_stream_init_from_url(url);
    tb_assert_and_check_return_val(stream, tb_null);

    // make stream
    tb_object_t* object = tb_null;
    if (tb_stream_open(stream))
    {
        // size
        tb_hong_t size = tb_stream_size(stream);
        if (size > 0 && size < TB_MAXS32)
        {
            tb_byte_t* data = tb_malloc0_bytes((tb_size_t)size);
            if (data) 
            {
                if (tb_stream_bread(stream, data, (tb_size_t)size))
                    object = tb_object_data_init_from_data(data, (tb_size_t)size);
                tb_free(data);
            }
        }
        else object = tb_object_data_init_from_data(tb_null, 0);

        // check, TODO: read stream if no size
        tb_assert(size >= 0 && size < TB_MAXS32);

        // exit stream
        tb_stream_exit(stream);
    }

    // ok?
    return object;
}
tb_object_t* tb_object_data_init_from_data(tb_pointer_t addr, tb_size_t size)
{
    // make
    tb_object_data_t* data = tb_object_data_init_base();
    tb_assert_and_check_return_val(data, tb_null);

    // init buff
    if (!tb_buffer_init(&data->buff))
    {
        tb_object_data_exit((tb_object_t*)data);
        return tb_null;
    }

    // copy data
    if (addr && size) tb_buffer_memncpy(&data->buff, (tb_byte_t const*)addr, size);

    // ok
    return (tb_object_t*)data;
}
tb_object_t* tb_object_data_init_from_pbuf(tb_buffer_t* pbuf)
{   
    // make
    tb_object_data_t* data = tb_object_data_init_base();
    tb_assert_and_check_return_val(data, tb_null);

    // init buff
    if (!tb_buffer_init(&data->buff))
    {
        tb_object_data_exit((tb_object_t*)data);
        return tb_null;
    }

    // copy data
    if (pbuf) tb_buffer_memcpy(&data->buff, pbuf);

    // ok
    return (tb_object_t*)data;
}
tb_pointer_t tb_object_data_getp(tb_object_t* object)
{
    // check
    tb_object_data_t* data = tb_object_data_cast(object);
    tb_assert_and_check_return_val(data, tb_null);

    // data
    return tb_buffer_data(&data->buff);
}
tb_bool_t tb_object_data_setp(tb_object_t* object, tb_pointer_t addr, tb_size_t size)
{
    // check
    tb_object_data_t* data = tb_object_data_cast(object);
    tb_assert_and_check_return_val(data && addr, tb_false);

    // data
    tb_buffer_memncpy(&data->buff, (tb_byte_t const*)addr, size);

    // ok
    return tb_true;
}
tb_size_t tb_object_data_size(tb_object_t* object)
{
    // check
    tb_object_data_t* data = tb_object_data_cast(object);
    tb_assert_and_check_return_val(data, 0);

    // data
    return tb_buffer_size(&data->buff);
}
tb_buffer_t* tb_object_data_buff(tb_object_t* object)
{
    // check
    tb_object_data_t* data = tb_object_data_cast(object);
    tb_assert_and_check_return_val(data, tb_null);

    // buff
    return &data->buff;
}
tb_bool_t tb_object_data_writ_to_url(tb_object_t* object, tb_char_t const* url)
{
    // check
    tb_object_data_t* data = tb_object_data_cast(object);
    tb_assert_and_check_return_val(data && tb_object_data_getp((tb_object_t*)data) && url, tb_false);

    // make stream
    tb_stream_ref_t stream = tb_stream_init_from_url(url);
    tb_assert_and_check_return_val(stream, tb_false);

    // ctrl
    if (tb_stream_type(stream) == TB_STREAM_TYPE_FILE)
        tb_stream_ctrl(stream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
    
    // open stream
    tb_bool_t ok = tb_false;
    if (tb_stream_open(stream))
    {
        // writ stream
        if (tb_stream_bwrit(stream, (tb_byte_t const*)tb_object_data_getp((tb_object_t*)data), tb_object_data_size((tb_object_t*)data))) ok = tb_true;
    }

    // exit stream
    tb_stream_exit(stream);

    // ok?
    return ok;
}
