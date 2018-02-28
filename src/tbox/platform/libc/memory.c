/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
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
    tb_check_return_val(size, tb_null);

    // malloc it
    return malloc(size);
}
tb_pointer_t tb_native_memory_malloc0(tb_size_t size)
{
    // check
    tb_check_return_val(size, tb_null); 

    // malloc0 it
    return calloc(1, size);
}
tb_pointer_t tb_native_memory_nalloc(tb_size_t item, tb_size_t size)
{
    // check
    tb_check_return_val(item && size, tb_null); 

    // nalloc it
    return malloc(item * size);
}
tb_pointer_t tb_native_memory_nalloc0(tb_size_t item, tb_size_t size)
{
    // check
    tb_check_return_val(item && size, tb_null);

    // nalloc0 it
    return calloc(item, size);
}
tb_pointer_t tb_native_memory_ralloc(tb_pointer_t data, tb_size_t size)
{
    // no size? free it
    if (!size) 
    {
        free(data);
        return tb_null;
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

