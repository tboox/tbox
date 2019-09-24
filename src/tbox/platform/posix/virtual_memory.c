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
 * Copyright (C) 2009 - 2019, TBOOX Open Source Group.
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
#include <sys/mman.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the virtual memory header type
typedef struct __tb_virtual_memory_header_t
{
    // the block size
    tb_size_t       size;

}tb_virtual_memory_header_t, *tb_virtual_memory_header_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_pointer_t tb_virtual_memory_malloc(tb_size_t size)
{
    // check
    tb_check_return_val(size, tb_null);

    // allocate an anonymous mmap buffer
    tb_virtual_memory_header_ref_t block = mmap(tb_null, sizeof(tb_virtual_memory_header_t) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block)
    {
        block->size = size;
        return (tb_pointer_t)&block[1];
    }
    return tb_null;
}
tb_pointer_t tb_virtual_memory_ralloc(tb_pointer_t data, tb_size_t size)
{
    // no size? free it
    if (!size) 
    {
        tb_virtual_memory_free(data);
        return tb_null;
    }
    // no data? malloc it
    else if (!data) return tb_virtual_memory_malloc(size);
    // realloc it
    else 
    {
        // shrink size? return it directly 
        tb_virtual_memory_header_ref_t block = &((tb_virtual_memory_header_ref_t)data)[-1];
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
    tb_virtual_memory_header_ref_t block = (tb_virtual_memory_header_ref_t)data;
    if (block) 
    {
        block--;
        return munmap((tb_pointer_t)block, block->size) == 0;
    }
    return tb_true;
}

