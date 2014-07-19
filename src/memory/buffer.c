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
 * @file        buffer.c
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
 * macros
 */
// the maximum grow size of value buffer 
#ifdef __tb_small__
#   define TB_BUFFER_GROW_SIZE       (64)
#else
#   define TB_BUFFER_GROW_SIZE       (256)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_buffer_init(tb_buffer_t* buffer)
{
    // check
    tb_assert_and_check_return_val(buffer, tb_false);

    // init
    buffer->data = buffer->buff;
    buffer->size = 0;
    buffer->maxn = sizeof(buffer->buff);

    // ok
    return tb_true;
}
tb_void_t tb_buffer_exit(tb_buffer_t* buffer)
{
    // check
    tb_assert_and_check_return(buffer);

    // clear it
    tb_buffer_clear(buffer);
}
tb_byte_t* tb_buffer_data(tb_buffer_t* buffer)
{
    // check
    tb_assert_and_check_return_val(buffer, tb_null);

    // the buffer data
    return buffer->data;
}
tb_size_t tb_buffer_size(tb_buffer_t const* buffer)
{ 
    // check
    tb_assert_and_check_return_val(buffer, 0);

    // the buffer size
    return buffer->size;
}
tb_size_t tb_buffer_maxn(tb_buffer_t const* buffer)
{
    // check
    tb_assert_and_check_return_val(buffer, 0);

    // the buffer maxn
    return buffer->maxn;
}
tb_void_t tb_buffer_clear(tb_buffer_t* buffer)
{
    // check
    tb_assert_and_check_return(buffer);

    // clear data
    if (buffer->data && buffer->data != buffer->buff) tb_free(buffer->data);
    buffer->data = buffer->buff;

    // clear size
    buffer->size = 0;
    buffer->maxn = sizeof(buffer->buff);
}
tb_byte_t* tb_buffer_resize(tb_buffer_t* buffer, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(buffer && size, tb_null);

    // done
    tb_bool_t   ok = tb_false;
    tb_byte_t*  buff_data = buffer->data;
    tb_size_t   buff_size = buffer->size;
    tb_size_t   buff_maxn = buffer->maxn;
    do
    {
        // check
        tb_assert_and_check_break(buff_data);

        // using static buffer?
        if (buff_data == buffer->buff)
        {
            // grow?
            if (size > buff_maxn)
            {
                // grow maxn
                buff_maxn = tb_align8(size + TB_BUFFER_GROW_SIZE);
                tb_assert_and_check_break(size <= buff_maxn);

                // grow data
                buff_data = tb_malloc_bytes(buff_maxn);
                tb_assert_and_check_break(buff_data);

                // copy data
                tb_memcpy(buff_data, buffer->buff, buff_size);
            }

            // update the size
            buff_size = size;
        }
        else
        {
            // grow?
            if (size > buff_maxn)
            {
                // grow maxn
                buff_maxn = tb_align8(size + TB_BUFFER_GROW_SIZE);
                tb_assert_and_check_break(size <= buff_maxn);

                // grow data
                buff_data = (tb_byte_t*)tb_ralloc(buff_data, buff_maxn);
                tb_assert_and_check_break(buff_data);
            }
            // decrease to the static buffer
            else if (size <= sizeof(buffer->buff))
            {
                // update the maxn
                buff_maxn = sizeof(buffer->buff);

                // copy data
                tb_memcpy(buffer->buff, buff_data, size);

                // free data
                tb_free(buff_data);

                // using the static buffer
                buff_data = buffer->buff;
            }

            // update the size
            buff_size = size;
        }

        // update the buffer
        buffer->data = buff_data;
        buffer->size = buff_size;
        buffer->maxn = buff_maxn;

        // ok 
        ok = tb_true;

    } while (0);

    // ok
    return ok? (tb_byte_t*)buffer->data : tb_null;
}
tb_byte_t* tb_buffer_memset(tb_buffer_t* buffer, tb_byte_t b)
{
    return tb_buffer_memnsetp(buffer, 0, b, tb_buffer_size(buffer));
}
tb_byte_t* tb_buffer_memsetp(tb_buffer_t* buffer, tb_size_t p, tb_byte_t b)
{
    return tb_buffer_memnsetp(buffer, p, b, tb_buffer_size(buffer));
}
tb_byte_t* tb_buffer_memnset(tb_buffer_t* buffer, tb_byte_t b, tb_size_t n)
{
    return tb_buffer_memnsetp(buffer, 0, b, n);
}
tb_byte_t* tb_buffer_memnsetp(tb_buffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(buffer, tb_null);
    
    // check
    tb_check_return_val(n, tb_buffer_data(buffer));

    // resize
    tb_byte_t* d = tb_buffer_resize(buffer, p + n);
    tb_assert_and_check_return_val(d, tb_null);

    // memset
    tb_memset(d + p, b, n);

    // ok?
    return d;
}
tb_byte_t* tb_buffer_memcpy(tb_buffer_t* buffer, tb_buffer_t* b)
{
    return tb_buffer_memncpyp(buffer, 0, tb_buffer_data(b), tb_buffer_size(b));
}
tb_byte_t* tb_buffer_memcpyp(tb_buffer_t* buffer, tb_size_t p, tb_buffer_t* b)
{
    return tb_buffer_memncpyp(buffer, p, tb_buffer_data(b), tb_buffer_size(b));
}
tb_byte_t* tb_buffer_memncpy(tb_buffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{
    return tb_buffer_memncpyp(buffer, 0, b, n);
}
tb_byte_t* tb_buffer_memncpyp(tb_buffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(buffer && b, tb_null);
    
    // check
    tb_check_return_val(n, tb_buffer_data(buffer));

    // resize
    tb_byte_t* d = tb_buffer_resize(buffer, p + n);
    tb_assert_and_check_return_val(d, tb_null);

    // safer than memcpy, buffer maybe overlap
    tb_memmov(d + p, b, n);

    // ok
    return d;
}
tb_byte_t* tb_buffer_memmov(tb_buffer_t* buffer, tb_size_t b)
{
    // check
    tb_assert_and_check_return_val(b <= tb_buffer_size(buffer), tb_null);
    return tb_buffer_memnmovp(buffer, 0, b, tb_buffer_size(buffer) - b);
}
tb_byte_t* tb_buffer_memmovp(tb_buffer_t* buffer, tb_size_t p, tb_size_t b)
{
    // check
    tb_assert_and_check_return_val(b <= tb_buffer_size(buffer), tb_null);
    return tb_buffer_memnmovp(buffer, p, b, tb_buffer_size(buffer) - b);
}
tb_byte_t* tb_buffer_memnmov(tb_buffer_t* buffer, tb_size_t b, tb_size_t n)
{
    return tb_buffer_memnmovp(buffer, 0, b, n);
}
tb_byte_t* tb_buffer_memnmovp(tb_buffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(buffer && (b + n) <= tb_buffer_size(buffer), tb_null);

    // clear?
    if (b == tb_buffer_size(buffer)) 
    {
        tb_buffer_clear(buffer);
        return tb_buffer_data(buffer);
    }

    // check
    tb_check_return_val(p != b && n, tb_buffer_data(buffer));

    // resize
    tb_byte_t* d = tb_buffer_resize(buffer, p + n);
    tb_assert_and_check_return_val(d, tb_null);

    // memmov
    tb_memmov(d + p, d + b, n);
    return d;
}
tb_byte_t* tb_buffer_memcat(tb_buffer_t* buffer, tb_buffer_t* b)
{
    return tb_buffer_memncat(buffer, tb_buffer_data(b), tb_buffer_size(b));
}
tb_byte_t* tb_buffer_memncat(tb_buffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{   
    // check
    tb_assert_and_check_return_val(buffer && b, tb_null);
    
    // check
    tb_check_return_val(n, tb_buffer_data(buffer));

    // is null?
    tb_size_t p = tb_buffer_size(buffer);
    if (!p) return tb_buffer_memncpy(buffer, b, n);

    // resize
    tb_byte_t* d = tb_buffer_resize(buffer, p + n);
    tb_assert_and_check_return_val(d, tb_null);

    // memcat
    tb_memcpy(d + p, b, n);

    return d;
}

