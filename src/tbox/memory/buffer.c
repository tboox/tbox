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
tb_bool_t tb_buffer_init(tb_buffer_ref_t buffer)
{
    tb_assert_and_check_return_val(buffer, tb_false);

    buffer->data = buffer->buff;
    buffer->size = 0;
    buffer->maxn = sizeof(buffer->buff);
    return tb_true;
}
tb_void_t tb_buffer_exit(tb_buffer_ref_t buffer)
{
    tb_assert_and_check_return(buffer);

    tb_buffer_clear(buffer);

    if (buffer->data && buffer->data != buffer->buff) tb_free(buffer->data);
    buffer->data = buffer->buff;

    buffer->size = 0;
    buffer->maxn = sizeof(buffer->buff);
}
tb_byte_t* tb_buffer_data(tb_buffer_ref_t buffer)
{
    tb_assert_and_check_return_val(buffer, tb_null);
    return buffer->data;
}
tb_size_t tb_buffer_size(tb_buffer_ref_t buffer)
{
    tb_assert_and_check_return_val(buffer, 0);
    return buffer->size;
}
tb_size_t tb_buffer_maxn(tb_buffer_ref_t buffer)
{
    tb_assert_and_check_return_val(buffer, 0);
    return buffer->maxn;
}
tb_void_t tb_buffer_clear(tb_buffer_ref_t buffer)
{
    tb_assert_and_check_return(buffer);
    buffer->size = 0;
}
tb_byte_t* tb_buffer_resize(tb_buffer_ref_t buffer, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(buffer && size, tb_null);

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
            if (size > buff_maxn)
            {
                buff_maxn = tb_align_pow2(size + TB_BUFFER_GROW_SIZE);
                tb_assert_and_check_break(size <= buff_maxn);

                buff_data = tb_malloc_bytes(buff_maxn);
                tb_assert_and_check_break(buff_data);

                tb_memcpy(buff_data, buffer->buff, buff_size);
            }

            buff_size = size;
        }
        else
        {
            if (size > buff_maxn)
            {
                buff_maxn = tb_align_pow2(size + TB_BUFFER_GROW_SIZE);
                tb_assert_and_check_break(size <= buff_maxn);

                buff_data = (tb_byte_t*)tb_ralloc(buff_data, buff_maxn);
                tb_assert_and_check_break(buff_data);
            }
#if 0
            // decrease to the static buffer
            else if (size <= sizeof(buffer->buff))
            {
                buff_maxn = sizeof(buffer->buff);
                tb_memcpy(buffer->buff, buff_data, size);
                tb_free(buff_data);
                buff_data = buffer->buff;
            }
#endif
            buff_size = size;
        }

        buffer->data = buff_data;
        buffer->size = buff_size;
        buffer->maxn = buff_maxn;
        ok = tb_true;

    } while (0);

    tb_assertf(ok, "resize buffer failed: %lu => %lu", buff_size, size);

    return ok? (tb_byte_t*)buffer->data : tb_null;
}
tb_byte_t* tb_buffer_memset(tb_buffer_ref_t buffer, tb_byte_t b)
{
    return tb_buffer_memnsetp(buffer, 0, b, tb_buffer_size(buffer));
}
tb_byte_t* tb_buffer_memsetp(tb_buffer_ref_t buffer, tb_size_t p, tb_byte_t b)
{
    tb_size_t size = tb_buffer_size(buffer);
    tb_assert_and_check_return_val(p < size, tb_null);
    return tb_buffer_memnsetp(buffer, p, b, size - p);
}
tb_byte_t* tb_buffer_memnset(tb_buffer_ref_t buffer, tb_byte_t b, tb_size_t n)
{
    return tb_buffer_memnsetp(buffer, 0, b, n);
}
tb_byte_t* tb_buffer_memnsetp(tb_buffer_ref_t buffer, tb_size_t p, tb_byte_t b, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(buffer, tb_null);
    tb_check_return_val(n, tb_buffer_data(buffer));

    // resize
    tb_byte_t* d = tb_buffer_resize(buffer, p + n);
    tb_assert_and_check_return_val(d, tb_null);

    // memset
    tb_memset(d + p, b, n);
    return d;
}
tb_byte_t* tb_buffer_memcpy(tb_buffer_ref_t buffer, tb_buffer_ref_t b)
{
    return tb_buffer_memncpyp(buffer, 0, tb_buffer_data(b), tb_buffer_size(b));
}
tb_byte_t* tb_buffer_memcpyp(tb_buffer_ref_t buffer, tb_size_t p, tb_buffer_ref_t b)
{
    return tb_buffer_memncpyp(buffer, p, tb_buffer_data(b), tb_buffer_size(b));
}
tb_byte_t* tb_buffer_memncpy(tb_buffer_ref_t buffer, tb_byte_t const* b, tb_size_t n)
{
    return tb_buffer_memncpyp(buffer, 0, b, n);
}
tb_byte_t* tb_buffer_memncpyp(tb_buffer_ref_t buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(buffer && b, tb_null);
    tb_check_return_val(n, tb_buffer_data(buffer));

    // resize
    tb_byte_t* d = tb_buffer_resize(buffer, p + n);
    tb_assert_and_check_return_val(d, tb_null);

    // copy it
    tb_memcpy(d + p, b, n);
    return d;
}
tb_byte_t* tb_buffer_memmov(tb_buffer_ref_t buffer, tb_size_t b)
{
    // check
    tb_assert_and_check_return_val(b <= tb_buffer_size(buffer), tb_null);
    return tb_buffer_memnmovp(buffer, 0, b, tb_buffer_size(buffer) - b);
}
tb_byte_t* tb_buffer_memmovp(tb_buffer_ref_t buffer, tb_size_t p, tb_size_t b)
{
    // check
    tb_assert_and_check_return_val(b <= tb_buffer_size(buffer), tb_null);
    return tb_buffer_memnmovp(buffer, p, b, tb_buffer_size(buffer) - b);
}
tb_byte_t* tb_buffer_memnmov(tb_buffer_ref_t buffer, tb_size_t b, tb_size_t n)
{
    return tb_buffer_memnmovp(buffer, 0, b, n);
}
tb_byte_t* tb_buffer_memnmovp(tb_buffer_ref_t buffer, tb_size_t p, tb_size_t b, tb_size_t n)
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

    tb_memmov(d + p, d + b, n);
    return d;
}
tb_byte_t* tb_buffer_memcat(tb_buffer_ref_t buffer, tb_buffer_ref_t b)
{
    return tb_buffer_memncat(buffer, tb_buffer_data(b), tb_buffer_size(b));
}
tb_byte_t* tb_buffer_memncat(tb_buffer_ref_t buffer, tb_byte_t const* b, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(buffer && b, tb_null);
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

