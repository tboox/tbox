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
 * @file        pipe.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "pipe"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pipe.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/pipe.c"
#elif defined(TB_CONFIG_POSIX_HAVE_PIPE) || \
        defined(TB_CONFIG_POSIX_HAVE_PIPE2) || \
            defined(TB_CONFIG_POSIX_HAVE_MKFIFO)
#   include "posix/pipe.c"
#else
tb_pipe_file_ref_t tb_pipe_file_init(tb_char_t const* name, tb_size_t mode, tb_size_t buffer_size)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_bool_t tb_pipe_file_init_pair(tb_pipe_file_ref_t pair[2], tb_size_t mode[2], tb_size_t buffer_size)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_pipe_file_exit(tb_pipe_file_ref_t file)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_long_t tb_pipe_file_read(tb_pipe_file_ref_t file, tb_byte_t* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_pipe_file_write(tb_pipe_file_ref_t file, tb_byte_t const* data, tb_size_t size)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_pipe_file_wait(tb_pipe_file_ref_t file, tb_size_t events, tb_long_t timeout)
{
    tb_trace_noimpl();
    return -1;
}
tb_long_t tb_pipe_file_connect(tb_pipe_file_ref_t file)
{
    tb_trace_noimpl();
    return -1;
}
#endif

tb_bool_t tb_pipe_file_bread(tb_pipe_file_ref_t file, tb_byte_t* data, tb_size_t size)
{
    // read data
    tb_size_t read = 0;
    tb_long_t wait = 0;
    while (read < size)
    {
        // read it
        tb_long_t real = tb_pipe_file_read(file, data + read, tb_min(size - read, 8192));

        // has data?
        if (real > 0)
        {
            read += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = tb_pipe_file_wait(file, TB_PIPE_EVENT_READ, -1);
            tb_check_break(wait > 0);
        }
        // failed or end?
        else break;
    }
    return read == size;
}
tb_bool_t tb_pipe_file_bwrit(tb_pipe_file_ref_t file, tb_byte_t const* data, tb_size_t size)
{
    // writ data
    tb_size_t writ = 0;
    tb_long_t wait = 0;
    while (writ < size)
    {
        // write it
        tb_long_t real = tb_pipe_file_write(file, data + writ, tb_min(size - writ, 8192));

        // has data?
        if (real > 0)
        {
            writ += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = tb_pipe_file_wait(file, TB_PIPE_EVENT_WRIT, -1);
            tb_check_break(wait > 0);
        }
        // failed or end?
        else break;
    }
    return writ == size;
}
