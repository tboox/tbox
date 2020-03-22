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
 * Copyright (C) 2009-2020, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        process_group.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "process_group"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "process_group.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/process_group.c"
#elif defined(TB_CONFIG_POSIX_HAVE_WAITPID) && \
        defined(TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP)
#   include "posix/process_group.c"
#elif defined(TB_CONFIG_POSIX_HAVE_WAITPID) && \
         (defined(TB_CONFIG_POSIX_HAVE_FORK) || defined(TB_CONFIG_POSIX_HAVE_VFORK)) && \
            (defined(TB_CONFIG_POSIX_HAVE_EXECVP) || defined(TB_CONFIG_POSIX_HAVE_EXECVPE)) 
#   include "posix/process_group.c"
#else
tb_process_group_ref_t tb_process_group_init(tb_char_t const* name)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_void_t tb_process_group_exit(tb_process_group_ref_t group)
{
    tb_trace_noimpl();
}
tb_void_t tb_process_group_kill(tb_process_group_ref_t group)
{
    tb_trace_noimpl();
}

#endif
