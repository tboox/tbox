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
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "virtual_memory.h"
#include "native_memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/virtual_memory.c"
#elif defined(TB_CONFIG_POSIX_HAVE_MMAP)
#   include "posix/virtual_memory.c"
#else
tb_pointer_t tb_virtual_memory_malloc(tb_size_t size)
{
    return tb_native_memory_malloc(size);
}
tb_pointer_t tb_virtual_memory_ralloc(tb_pointer_t data, tb_size_t size)
{
    return tb_native_memory_ralloc(data, size);
}
tb_bool_t tb_virtual_memory_free(tb_pointer_t data)
{
    return tb_native_memory_free(data);
}
#endif

