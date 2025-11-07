/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      A2va
 * @file        buffer.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the buffer stream type
typedef struct __tb_stream_buffer_t
{
    // the buffer
    tb_buffer_ref_t         buffer;

    // the head
    tb_size_t               head;

    // the buffer is referenced?
    tb_bool_t               bref;

}tb_stream_buffer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_stream_buffer_t* tb_stream_buffer_cast(tb_stream_ref_t stream)
{
    tb_assert_and_check_return_val(stream && tb_stream_type(stream) == TB_STREAM_TYPE_BUFF, tb_null);
    return (tb_stream_buffer_t*)stream;
}
static tb_bool_t tb_stream_buffer_open(tb_stream_ref_t stream)
{
    tb_stream_buffer_t* stream_buffer = tb_stream_buffer_cast(stream);
    tb_assert_and_check_return_val(stream_buffer, tb_false);

    stream_buffer->head = 0;
    return tb_true;
}
static tb_bool_t tb_stream_buffer_clos(tb_stream_ref_t stream)
{
    tb_stream_buffer_t* stream_buffer = tb_stream_buffer_cast(stream);
    tb_assert_and_check_return_val(stream_buffer, tb_false);

    stream_buffer->head = 0;
    return tb_true;
}
static tb_void_t tb_stream_buffer_exit(tb_stream_ref_t stream)
{
    tb_stream_buffer_t* stream_buffer = tb_stream_buffer_cast(stream);
    tb_assert_and_check_return(stream_buffer);

    stream_buffer->head = 0;

    if (stream_buffer->buffer && !stream_buffer->bref) tb_buffer_exit(stream_buffer->buffer);
    stream_buffer->buffer = tb_null;
}
static tb_long_t tb_stream_buffer_read(tb_stream_ref_t stream, tb_byte_t* data, tb_size_t size)
{
    tb_stream_buffer_t* stream_buffer = tb_stream_buffer_cast(stream);
    tb_assert_and_check_return_val(stream_buffer && stream_buffer->buffer, -1);

    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    tb_size_t left = tb_buffer_size(stream_buffer->buffer) - stream_buffer->head;
    if (size > left) size = left;

    if (size) tb_memcpy(data, tb_buffer_data(stream_buffer->buffer) + stream_buffer->head, size);
    stream_buffer->head += size;
    return (tb_long_t)(size);
}
static tb_long_t tb_stream_buffer_writ(tb_stream_ref_t stream, tb_byte_t const* data, tb_size_t size)
{
    tb_stream_buffer_t* stream_buffer = tb_stream_buffer_cast(stream);
    tb_assert_and_check_return_val(stream_buffer && stream_buffer->buffer, -1);

    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    /* ensure the buffer is large enough to hold the data at the write position
     * tb_buffer_memncpyp will resize to head + size, which may truncate existing data
     * so we need to ensure the buffer size is at least max(current_size, head + size)
     *
     * @see https://github.com/tboox/tbox/issues/208
     */
    if (size)
    {
        tb_size_t need_size = stream_buffer->head + size;
        tb_size_t curr_size = tb_buffer_size(stream_buffer->buffer);
        if (need_size > curr_size)
        {
            // expand the buffer
            if (!tb_buffer_resize(stream_buffer->buffer, need_size)) return -1;
        }
        
        // copy data at the write position
        tb_memcpy(tb_buffer_data(stream_buffer->buffer) + stream_buffer->head, data, size);
    }
    stream_buffer->head += size;
    return size;
}
static tb_bool_t tb_stream_buffer_seek(tb_stream_ref_t stream, tb_hize_t offset)
{
    tb_stream_buffer_t* stream_buffer = tb_stream_buffer_cast(stream);
    tb_assert_and_check_return_val(stream_buffer && offset <= tb_buffer_size(stream_buffer->buffer), tb_false);

    stream_buffer->head = (tb_size_t)offset;
    return tb_true;
}
static tb_long_t tb_stream_buffer_wait(tb_stream_ref_t stream, tb_size_t wait, tb_long_t timeout)
{
    tb_stream_buffer_t* stream_buffer = tb_stream_buffer_cast(stream);
    tb_assert_and_check_return_val(stream_buffer, -1);

    tb_long_t events = 0;
    if (stream_buffer->head < tb_buffer_size(stream_buffer->buffer))
    {
        if (wait & TB_STREAM_WAIT_READ) events |= TB_STREAM_WAIT_READ;
        if (wait & TB_STREAM_WAIT_WRIT) events |= TB_STREAM_WAIT_WRIT;
    }
    else events = -1;
    return events;
}
static tb_bool_t tb_stream_buffer_ctrl(tb_stream_ref_t stream, tb_size_t ctrl, tb_va_list_t args)
{
    tb_stream_buffer_t* stream_buffer = tb_stream_buffer_cast(stream);
    tb_assert_and_check_return_val(stream_buffer, tb_false);

    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            // the psize
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize, tb_false);

            // get size
            *psize = tb_buffer_size(stream_buffer->buffer);
            return tb_true;
        }
        break;
    case TB_STREAM_CTRL_BUFF_SET_BUFFER:
        {
            // exit buffer first if exists
            if (stream_buffer->buffer && !stream_buffer->bref) tb_buffer_exit(stream_buffer->buffer);

            stream_buffer->buffer = (tb_buffer_ref_t)tb_va_arg(args, tb_buffer_ref_t);
            stream_buffer->head  = 0;
            stream_buffer->bref = tb_true;
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
tb_stream_ref_t tb_stream_init_buffer()
{
    return tb_stream_init(  TB_STREAM_TYPE_BUFF
                        ,   sizeof(tb_stream_buffer_t)
                        ,   0
                        ,   tb_stream_buffer_open
                        ,   tb_stream_buffer_clos
                        ,   tb_stream_buffer_exit
                        ,   tb_stream_buffer_ctrl
                        ,   tb_stream_buffer_wait
                        ,   tb_stream_buffer_read
                        ,   tb_stream_buffer_writ
                        ,   tb_stream_buffer_seek
                        ,   tb_null
                        ,   tb_null);
}
tb_stream_ref_t tb_stream_init_from_buffer(tb_buffer_ref_t buffer)
{
    // check
    tb_assert_and_check_return_val(buffer, tb_null);

    tb_bool_t           ok = tb_false;
    tb_stream_ref_t     stream = tb_null;
    do
    {
        stream = tb_stream_init_buffer();
        tb_assert_and_check_break(stream);

        if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_BUFF_SET_BUFFER, buffer)) break;
        ok = tb_true;

    } while (0);

    if (!ok)
    {
        if (stream) tb_stream_exit(stream);
        stream = tb_null;
    }
    return stream;
}
