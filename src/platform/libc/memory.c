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
 * @file        memory.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../memory.h"
#include <stdlib.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_native_memory_init()
{
    return tb_true;
}
tb_void_t tb_native_memory_exit()
{
}
tb_pointer_t tb_native_memory_malloc(tb_size_t size)
{
    // check
    tb_check_return_val(size, tb_object_null);

    // malloc it
    return malloc(size);
}
tb_pointer_t tb_native_memory_malloc0(tb_size_t size)
{
    // check
    tb_check_return_val(size, tb_object_null); 

    // malloc0 it
    return calloc(1, size);
}
tb_pointer_t tb_native_memory_nalloc(tb_size_t item, tb_size_t size)
{
    // check
    tb_check_return_val(item && size, tb_object_null); 

    // nalloc it
    return malloc(item * size);
}
tb_pointer_t tb_native_memory_nalloc0(tb_size_t item, tb_size_t size)
{
    // check
    tb_check_return_val(item && size, tb_object_null);

    // nalloc0 it
    return calloc(item, size);
}
tb_pointer_t tb_native_memory_ralloc(tb_pointer_t data, tb_size_t size)
{
    // no size? free it
    if (!size) 
    {
        free(data);
        return tb_object_null;
    }
    // no data? malloc it
    else if (!data) return malloc(size);
    // realloc it
    else return realloc(data, size);
}
tb_bool_t tb_native_memory_free(tb_pointer_t data)
{
    // free it
    if (data) free(data);

    // ok
    return tb_true;
}

