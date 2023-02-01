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
 * @file        filelock.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "filelock.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS) && !defined(TB_COMPILER_LIKE_UNIX)
#   include "windows/filelock.c"
#elif defined(TB_CONFIG_BSD_HAVE_FLOCK) && !defined(TB_CONFIG_OS_HAIKU)
#   include "bsd/filelock.c"
#elif defined(TB_CONFIG_POSIX_HAVE_FCNTL)
#   include "posix/filelock.c"
#else
tb_filelock_ref_t tb_filelock_init(tb_file_ref_t file)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_filelock_ref_t tb_filelock_init_from_path(tb_char_t const* path, tb_size_t mode)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_void_t tb_filelock_exit(tb_filelock_ref_t lock)
{
    tb_trace_noimpl();
}
tb_bool_t tb_filelock_enter(tb_filelock_ref_t lock, tb_size_t mode)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_filelock_enter_try(tb_filelock_ref_t lock, tb_size_t mode)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_filelock_leave(tb_filelock_ref_t lock)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif
