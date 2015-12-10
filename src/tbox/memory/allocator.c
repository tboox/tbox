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
 * @file        allocator.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "allocator"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "allocator.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the allocator 
__tb_extern_c__ tb_allocator_ref_t  g_allocator = tb_null;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_allocator_ref_t tb_allocator()
{
    return g_allocator;
}
tb_size_t tb_allocator_type(tb_allocator_ref_t allocator)
{
    // check
    tb_assert_and_check_return_val(allocator, TB_ALLOCATOR_DEFAULT);

    // get it
    return allocator->type;
}
tb_pointer_t tb_allocator_malloc_(tb_allocator_ref_t allocator, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator && allocator->malloc, tb_null);

    // malloc it
    return allocator->malloc(allocator, size __tb_debug_args__);
}
tb_pointer_t tb_allocator_malloc0_(tb_allocator_ref_t allocator, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator && allocator->malloc, tb_null);

    // malloc it
    tb_pointer_t data = allocator->malloc(allocator, size __tb_debug_args__);

    // clear it
    if (data) tb_memset_(data, 0, size);

    // ok
    return data;
}
tb_pointer_t tb_allocator_nalloc_(tb_allocator_ref_t allocator, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator && allocator->malloc, tb_null);

    // nalloc it
    return allocator->malloc(allocator, item * size __tb_debug_args__);
}
tb_pointer_t tb_allocator_nalloc0_(tb_allocator_ref_t allocator, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator && allocator->malloc, tb_null);

    // adjust size
    size *= item;

    // nalloc0 it
    tb_pointer_t data = allocator->malloc(allocator, size __tb_debug_args__);

    // clear it
    if (data) tb_memset_(data, 0, size);

    // ok
    return data;
}
tb_pointer_t tb_allocator_ralloc_(tb_allocator_ref_t allocator, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator && allocator->ralloc, tb_null);

    // ralloc it
    return allocator->ralloc(allocator, data, size __tb_debug_args__);
}
tb_bool_t tb_allocator_free_(tb_allocator_ref_t allocator, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator && allocator->free, tb_false);

    // free it
    return allocator->free(allocator, data __tb_debug_args__);
}
tb_pointer_t tb_allocator_large_malloc_(tb_allocator_ref_t allocator, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator, tb_null);

    // no large malloc? 
    if (!allocator->large_malloc)
    {
        // check
        tb_assert_and_check_return_val(allocator->malloc, tb_null);

        // malloc it
        if (real) *real = size;
        return allocator->malloc(allocator, size __tb_debug_args__);
    }

    // malloc it
    return allocator->large_malloc(allocator, size, real __tb_debug_args__);
}
tb_pointer_t tb_allocator_large_malloc0_(tb_allocator_ref_t allocator, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator, tb_null);

    // malloc it
    tb_pointer_t data = tb_allocator_large_malloc_(allocator, size, real __tb_debug_args__);

    // clear it
    if (data) tb_memset_(data, 0, real? *real : size);

    // ok
    return data;
}
tb_pointer_t tb_allocator_large_nalloc_(tb_allocator_ref_t allocator, tb_size_t item, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator, tb_null);

    // malloc it
    return tb_allocator_large_malloc_(allocator, item * size, real __tb_debug_args__);
}
tb_pointer_t tb_allocator_large_nalloc0_(tb_allocator_ref_t allocator, tb_size_t item, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator, tb_null);

    // malloc it
    tb_pointer_t data = tb_allocator_large_malloc_(allocator, item * size, real __tb_debug_args__);

    // clear it
    if (data) tb_memset_(data, 0, real? *real : (item * size));

    // ok
    return data;
}
tb_pointer_t tb_allocator_large_ralloc_(tb_allocator_ref_t allocator, tb_pointer_t data, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator, tb_null);

    // no large ralloc? 
    if (!allocator->large_ralloc)
    {
        // check
        tb_assert_and_check_return_val(allocator->ralloc, tb_null);

        // ralloc it
        if (real) *real = size;
        return allocator->ralloc(allocator, data, size __tb_debug_args__);
    }

    // ralloc it
    return allocator->large_ralloc(allocator, data, size, real __tb_debug_args__);
}
tb_bool_t tb_allocator_large_free_(tb_allocator_ref_t allocator, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator, tb_false);

    // no large free? 
    if (!allocator->large_free)
    {
        // check
        tb_assert_and_check_return_val(allocator->free, tb_false);

        // free it
        return allocator->free(allocator, data __tb_debug_args__);
    }

    // free it
    return allocator->large_free(allocator, data __tb_debug_args__);
}
tb_pointer_t tb_allocator_align_malloc_(tb_allocator_ref_t allocator, tb_size_t size, tb_size_t align __tb_debug_decl__)
{
    // check
    tb_assertf_abort(!(align & 3), "invalid alignment size: %lu", align);
    tb_check_return_val(!(align & 3), tb_null);

    // malloc it
    tb_byte_t* data = (tb_byte_t*)tb_allocator_malloc_(allocator, size + align __tb_debug_args__);
    tb_check_return_val(data, tb_null);

    // the different bytes
    tb_byte_t diff = (tb_byte_t)((~(tb_long_t)data) & (align - 1)) + 1;

    // adjust the address
    data += diff;

    // check
    tb_assert_abort(!((tb_size_t)data & (align - 1)));

    // save the different bytes
    data[-1] = diff;

    // ok?
    return (tb_pointer_t)data;
}
tb_pointer_t tb_allocator_align_malloc0_(tb_allocator_ref_t allocator, tb_size_t size, tb_size_t align __tb_debug_decl__)
{
    // malloc it
    tb_pointer_t data = tb_allocator_align_malloc_(allocator, size, align __tb_debug_args__);
    tb_assert_and_check_return_val(data, tb_null);

    // clear it
    tb_memset(data, 0, size);

    // ok
    return data;
}
tb_pointer_t tb_allocator_align_nalloc_(tb_allocator_ref_t allocator, tb_size_t item, tb_size_t size, tb_size_t align __tb_debug_decl__)
{
    return tb_allocator_align_malloc_(allocator, item * size, align __tb_debug_args__);
}
tb_pointer_t tb_allocator_align_nalloc0_(tb_allocator_ref_t allocator, tb_size_t item, tb_size_t size, tb_size_t align __tb_debug_decl__)
{
    // nalloc it
    tb_pointer_t data = tb_allocator_align_nalloc_(allocator, item, size, align __tb_debug_args__);
    tb_assert_and_check_return_val(data, tb_null);

    // clear it
    tb_memset(data, 0, item * size);

    // ok
    return data;
}
tb_pointer_t tb_allocator_align_ralloc_(tb_allocator_ref_t allocator, tb_pointer_t data, tb_size_t size, tb_size_t align __tb_debug_decl__)
{
    // check align
    tb_assertf_abort(!(align & 3), "invalid alignment size: %lu", align);
    tb_check_return_val(!(align & 3), tb_null);

    // ralloc?
    tb_byte_t diff = 0;
    if (data)
    {
        // check address 
        tb_assertf_abort(!((tb_size_t)data & (align - 1)), "invalid address %p", data);
        tb_check_return_val(!((tb_size_t)data & (align - 1)), tb_null);

        // the different bytes
        diff = ((tb_byte_t*)data)[-1];

        // adjust the address
        data = (tb_byte_t*)data - diff;

        // ralloc it
        data = tb_allocator_ralloc_(allocator, data, size + align __tb_debug_args__);
        tb_check_return_val(data, tb_null);
    }
    // no data?
    else
    {
        // malloc it directly
        data = tb_allocator_malloc_(allocator, size + align __tb_debug_args__);
        tb_check_return_val(data, tb_null);
    }

    // the different bytes
    diff = (tb_byte_t)((~(tb_long_t)data) & (align - 1)) + 1;

    // adjust the address
    data = (tb_byte_t*)data + diff;

    // check
    tb_assert_abort(!((tb_size_t)data & (align - 1)));

    // save the different bytes
    ((tb_byte_t*)data)[-1] = diff;

    // ok?
    return data;
}
tb_bool_t tb_allocator_align_free_(tb_allocator_ref_t allocator, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(data, tb_false);
    tb_assert_abort(!((tb_size_t)data & 3));

    // the different bytes
    tb_byte_t diff = ((tb_byte_t*)data)[-1];

    // adjust the address
    data = (tb_byte_t*)data - diff;

    // free it
    return tb_allocator_free_(allocator, data __tb_debug_args__);
}
#ifdef __tb_debug__
tb_void_t tb_allocator_dump(tb_allocator_ref_t allocator)
{
    // check
    tb_assert_and_check_return(allocator);

    // dump it
    if (allocator->dump) allocator->dump(allocator);
}
#endif
