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
 * @file        static_allocator.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_allocator.h"
#include "impl/impl.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_handle_t tb_static_allocator_instance_init(tb_cpointer_t* ppriv)
{
    // check
    tb_check_return_val(ppriv, tb_null);

    // the data and size
    tb_value_ref_t  tuple = (tb_value_ref_t)*ppriv;
    tb_byte_t*      data = (tb_byte_t*)tuple[0].ptr;
    tb_size_t       size = tuple[1].ul;
    tb_assert_and_check_return_val(data && size, tb_null);
    
    // ok?
    return (tb_handle_t)tb_static_allocator_init(data, size);
}
static tb_void_t tb_static_allocator_instance_exit(tb_handle_t self, tb_cpointer_t priv)
{
    // check
    tb_allocator_ref_t allocator = (tb_allocator_ref_t)self;
    tb_assert_and_check_return(allocator);

    // dump allocator
#ifdef __tb_debug__
    if (allocator) tb_allocator_dump(allocator);
#endif

    // exit allocator
    if (allocator) tb_allocator_exit(allocator);
    allocator= tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_allocator_ref_t tb_static_allocator(tb_byte_t* data, tb_size_t size)
{
    /* init singleton first
     *
     * because this allocator may be called before tb_init()
     */
    if (!tb_singleton_init()) return tb_null;

    // init tuple
    tb_value_t tuple[2];
    tuple[0].ptr    = (tb_pointer_t)data;
    tuple[1].ul     = size;

    // get it
    return (tb_allocator_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_STATIC_ALLOCATOR, tb_static_allocator_instance_init, tb_static_allocator_instance_exit, tb_null, tuple);
}
tb_allocator_ref_t tb_static_allocator_init(tb_byte_t* data, tb_size_t size)
{
    // init it
    tb_allocator_ref_t allocator = tb_static_large_allocator_init(data, size, 8);
    tb_assert_and_check_return_val(allocator, tb_null);

    // init type
    allocator->type = TB_ALLOCATOR_STATIC;

    // ok
    return allocator;
}

