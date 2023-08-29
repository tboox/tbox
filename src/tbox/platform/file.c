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
 * @file        file.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "platform_file"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "file.h"
#include "path.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS) && !defined(TB_COMPILER_LIKE_UNIX)
#   include "windows/file.c"
#elif defined(TB_CONFIG_POSIX_HAVE_OPEN)
#   include "posix/file.c"
#else
tb_file_ref_t tb_file_init(tb_char_t const* path, tb_size_t mode)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_bool_t tb_file_exit(tb_file_ref_t file)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_long_t tb_file_read(tb_file_ref_t file, tb_byte_t* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_file_writ(tb_file_ref_t file, tb_byte_t const* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_file_pread(tb_file_ref_t file, tb_byte_t* data, tb_size_t size, tb_hize_t offset)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_file_pwrit(tb_file_ref_t file, tb_byte_t const* data, tb_size_t size, tb_hize_t offset)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_file_readv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_file_writv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_hong_t tb_file_writf(tb_file_ref_t file, tb_file_ref_t ifile, tb_hize_t offset, tb_hize_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_file_preadv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_file_pwritv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
    tb_trace_noimpl();
    return -1;
}
tb_bool_t tb_file_sync(tb_file_ref_t file)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_hong_t tb_file_seek(tb_file_ref_t file, tb_hong_t offset, tb_size_t mode)
{
    tb_trace_noimpl();
    return -1;
}
tb_hong_t tb_file_offset(tb_file_ref_t file)
{
    tb_trace_noimpl();
    return -1;
}
tb_hize_t tb_file_size(tb_file_ref_t file)
{
    tb_trace_noimpl();
    return 0;
}
tb_bool_t tb_file_info(tb_char_t const* path, tb_file_info_t* info)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_file_copy(tb_char_t const* path, tb_char_t const* dest, tb_size_t flags)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_file_create(tb_char_t const* path)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_file_remove(tb_char_t const* path)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_file_rename(tb_char_t const* path, tb_char_t const* dest)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_file_link(tb_char_t const* path, tb_char_t const* dest)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_file_access(tb_char_t const* path, tb_size_t mode)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_file_touch(tb_char_t const* path, tb_time_t atime, tb_time_t mtime)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif
tb_long_t tb_file_fscase(tb_char_t const* path)
{
    // check
    tb_assert_and_check_return_val(path, -1);

    // flip path case
    tb_char_t path_flipcase[TB_PATH_MAXN];
    tb_char_t const* p = path;
    tb_size_t i = 0;
    tb_char_t ch;
    while (*p && i < (TB_PATH_MAXN - 1))
    {
        ch = *p++;
        ch = tb_islower(ch)? tb_toupper(ch) : tb_tolower(ch);
        path_flipcase[i++] = ch;
    }
    path_flipcase[i] = '\0';

    tb_file_info_t info, info_flipcase;
    if (tb_file_info(path, &info))
    {
        if (tb_file_info(path_flipcase, &info_flipcase)
            && info.size == info_flipcase.size
            && info.mtime == info_flipcase.mtime
            && info.type == info_flipcase.type
            && info.flags == info_flipcase.flags)
        {
            return 0;
        }
        return 1;
    }
    else return -1;
}

