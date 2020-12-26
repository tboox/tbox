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
 * @file        virtual_memory.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../virtual_memory.h"
#include "../../memory/impl/prefix.h"
#include <sys/mman.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#   define MAP_ANONYMOUS MAP_ANON
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_pointer_t tb_virtual_memory_malloc(tb_size_t size)
{
    // check
    tb_check_return_val(size, tb_null);
    tb_assert_and_check_return_val(size >= TB_VIRTUAL_MEMORY_DATA_MINN, tb_null);

    /* allocate an anonymous mmap buffer
     *
     * @note we use tb_pool_data_head_t to support tb_pool_data_size() when checking memory in debug mode
     */
    tb_pool_data_head_t* block = mmap(tb_null, sizeof(tb_pool_data_head_t) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block)
    {
        block->size = size;
        return (tb_pointer_t)&block[1];
    }
    return tb_null;
}
tb_pointer_t tb_virtual_memory_ralloc(tb_pointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(size >= TB_VIRTUAL_MEMORY_DATA_MINN, tb_null);

    // no data? malloc it
    if (!data) return tb_virtual_memory_malloc(size);
    // realloc it
    else
    {
        // shrink size? return it directly
        tb_pool_data_head_t* block = &((tb_pool_data_head_t*)data)[-1];
        if (size <= block->size)
            return data;

        // allocate a new anonymous map buffer
        tb_pointer_t data_new = tb_virtual_memory_malloc(size);
        if (data_new) tb_memcpy(data_new, data, block->size);
        tb_virtual_memory_free(data);
        return data_new;
    }
}
tb_bool_t tb_virtual_memory_free(tb_pointer_t data)
{
    tb_pool_data_head_t* block = (tb_pool_data_head_t*)data;
    if (block)
    {
        block--;
        return munmap((tb_pointer_t)block, sizeof(tb_pool_data_head_t) + block->size) == 0;
    }
    return tb_true;
}

