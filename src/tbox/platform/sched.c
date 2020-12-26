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
 * @file        sched.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "platform_sched"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sched.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/sched_affinity.c"
#elif defined(TB_CONFIG_OS_MACOSX)
#   include "mach/sched_affinity.c"
#elif defined(TB_CONFIG_POSIX_HAVE_SCHED_SETAFFINITY)
#   include "posix/sched_affinity.c"
#else
tb_bool_t tb_sched_setaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_sched_getaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif

