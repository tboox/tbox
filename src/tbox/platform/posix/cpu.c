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
 * @file        cpu.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../platform.h"
#include <unistd.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_cpu_count()
{
    // we will pre-initialize it in tb_platform_init()
    static tb_size_t ncpu = -1;
    if (ncpu == -1)
    {
        tb_size_t count = sysconf(_SC_NPROCESSORS_ONLN);
        if (!count) count = sysconf(_SC_NPROCESSORS_CONF);
        if (!count) count = 1;
        ncpu = count;
    }
    return ncpu;
}


