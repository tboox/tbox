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
 * @file        queue_buffer.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "memory.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_queue_buffer_init(tb_queue_buffer_t* buffer, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(buffer, tb_false);

    // init 
    buffer->data = tb_null;
    buffer->head = tb_null;
    buffer->size = 0;
    buffer->maxn = maxn;

    // ok
    return tb_true;
}
tb_void_t tb_queue_buffer_exit(tb_queue_buffer_t* buffer)
{
    if (buffer)
    {
        if (buffer->data) tb_free(buffer->data);
        tb_memset(buffer, 0, sizeof(tb_queue_buffer_t));
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_byte_t* tb_queue_buffer_data(tb_queue_buffer_t const* buffer)
{
    tb_assert_and_check_return_val(buffer, tb_null);
    return buffer->data;
}
tb_byte_t* tb_queue_buffer_head(tb_queue_buffer_t const* buffer)
{
    tb_assert_and_check_return_val(buffer, tb_null);
    return buffer->head;
}
tb_byte_t* tb_queue_buffer_tail(tb_queue_buffer_t const* buffer)
{
    tb_assert_and_check_return_val(buffer, tb_null);
    return buffer->head? buffer->head + buffer->size : tb_null;
}
tb_size_t tb_queue_buffer_size(tb_queue_buffer_t const* buffer)
{
    tb_assert_and_check_return_val(buffer, 0);
    return buffer->size;
}
tb_size_t tb_queue_buffer_maxn(tb_queue_buffer_t const* buffer)
{
    tb_assert_and_check_return_val(buffer, 0);
    return buffer->maxn;
}
tb_size_t tb_queue_buffer_left(tb_queue_buffer_t const* buffer)
{
    tb_assert_and_check_return_val(buffer && buffer->size <= buffer->maxn, 0);
    return buffer->maxn - buffer->size;
}
tb_bool_t tb_queue_buffer_full(tb_queue_buffer_t const* buffer)
{
    tb_assert_and_check_return_val(buffer, tb_false);
    return buffer->size == buffer->maxn? tb_true : tb_false;
}
tb_bool_t tb_queue_buffer_null(tb_queue_buffer_t const* buffer)
{
    tb_assert_and_check_return_val(buffer, tb_false);
    return buffer->size? tb_false : tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_queue_buffer_clear(tb_queue_buffer_t* buffer)
{
    tb_assert_and_check_return(buffer);
    buffer->size = 0;
    buffer->head = buffer->data;
}
tb_byte_t* tb_queue_buffer_resize(tb_queue_buffer_t* buffer, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(buffer && maxn && maxn >= buffer->size, tb_null);

    // has data?
    if (buffer->data)
    {
        // move data to head
        if (buffer->head != buffer->data)
        {
            if (buffer->size) tb_memmov(buffer->data, buffer->head, buffer->size);
            buffer->head = buffer->data;
        }

        // realloc
        if (maxn > buffer->maxn)
        {
            // init head
            buffer->head = tb_null;

            // make data
            buffer->data = tb_ralloc(buffer->data, maxn);
            tb_assert_and_check_return_val(buffer->data, tb_null);

            // save head
            buffer->head = buffer->data;
        }
    }

    // update maxn
    buffer->maxn = maxn;

    // ok
    return buffer->data;
}
tb_long_t tb_queue_buffer_skip(tb_queue_buffer_t* buffer, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(buffer, -1);

    // no data?
    tb_check_return_val(buffer->data && buffer->size && size, 0);
    tb_assert_and_check_return_val(buffer->head, -1);

    // read data
    tb_long_t read = buffer->size > size? size : buffer->size;
    buffer->head += read;
    buffer->size -= read;

    // null? reset head
    if (!buffer->size) buffer->head = buffer->data;

    // ok
    return read;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * read & writ
 */
tb_long_t tb_queue_buffer_read(tb_queue_buffer_t* buffer, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(buffer && data, -1);

    // no data?
    tb_check_return_val(buffer->data && buffer->size && size, 0);
    tb_assert_and_check_return_val(buffer->head, -1);

    // read data
    tb_long_t read = buffer->size > size? size : buffer->size;
    tb_memcpy(data, buffer->head, read);
    buffer->head += read;
    buffer->size -= read;

    // null? reset head
    if (!buffer->size) buffer->head = buffer->data;

    // ok
    return read;
}
tb_long_t tb_queue_buffer_writ(tb_queue_buffer_t* buffer, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(buffer && data && buffer->maxn, -1);

    // no data?
    if (!buffer->data)
    {
        // make data
        buffer->data = tb_malloc(buffer->maxn);
        tb_assert_and_check_return_val(buffer->data, -1);

        // init it
        buffer->head = buffer->data;
        buffer->size = 0;
    }
    tb_assert_and_check_return_val(buffer->data && buffer->head, -1);

    // no left?
    tb_size_t left = buffer->maxn - buffer->size;
    tb_check_return_val(left, 0);

    // move data to head
    if (buffer->head != buffer->data)
    {
        if (buffer->size) tb_memmov(buffer->data, buffer->head, buffer->size);
        buffer->head = buffer->data;
    }

    // writ data
    tb_size_t writ = left > size? size : left;
    tb_memcpy(buffer->data + buffer->size, data, writ);
    buffer->size += writ;

    // ok
    return writ;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * pull & push
 */

tb_byte_t* tb_queue_buffer_pull_init(tb_queue_buffer_t* buffer, tb_size_t* size)
{
    // check
    tb_assert_and_check_return_val(buffer, tb_null);

    // no data?
    tb_check_return_val(buffer->data && buffer->size, tb_null);
    tb_assert_and_check_return_val(buffer->head, tb_null);

    // save size
    if (size) *size = buffer->size;

    // ok
    return buffer->head;
}
tb_void_t tb_queue_buffer_pull_exit(tb_queue_buffer_t* buffer, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buffer && buffer->head && size <= buffer->size);

    // update
    buffer->size -= size;
    buffer->head += size;

    // null? reset head
    if (!buffer->size) buffer->head = buffer->data;
}
tb_byte_t* tb_queue_buffer_push_init(tb_queue_buffer_t* buffer, tb_size_t* size)
{
    // check
    tb_assert_and_check_return_val(buffer && buffer->maxn, tb_null);

    // no data?
    if (!buffer->data)
    {
        // make data
        buffer->data = tb_malloc(buffer->maxn);
        tb_assert_and_check_return_val(buffer->data, tb_null);

        // init 
        buffer->head = buffer->data;
        buffer->size = 0;
    }
    tb_assert_and_check_return_val(buffer->data && buffer->head, tb_null);

    // no left?
    tb_size_t left = buffer->maxn - buffer->size;
    tb_check_return_val(left, tb_null);

    // move data to head
    if (buffer->head != buffer->data)
    {
        if (buffer->size) tb_memmov(buffer->data, buffer->head, buffer->size);
        buffer->head = buffer->data;
    }

    // save size
    if (size) *size = left;

    // ok
    return buffer->head + buffer->size;
}
tb_void_t tb_queue_buffer_push_exit(tb_queue_buffer_t* buffer, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buffer && buffer->head && buffer->size + size <= buffer->maxn);
    buffer->size += size;
}

