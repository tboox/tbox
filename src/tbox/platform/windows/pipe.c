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
 * @file        pipe.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../pipe.h"
#include "../file.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

// get pipe file name
static __tb_inline__ tb_wchar_t const* tb_pipe_file_name(tb_char_t const* name, tb_wchar_t* data, tb_size_t maxn)
{
    // get pipe name
    tb_char_t pipename[TB_PATH_MAXN];
    tb_long_t size = tb_snprintf(pipename, sizeof(pipename) - 1, "\\\\.\\pipe\\%s", name);
    tb_assert_and_check_return_val(size > 0, tb_null);
    return tb_atow(data, pipename, maxn) != -1? data : tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_pipe_file_ref_t tb_pipe_file_init(tb_char_t const* name, tb_size_t mode, tb_size_t buffer_size)
{
    // check
    tb_assert_and_check_return_val(name, tb_null);
    tb_assert_and_check_return_val(mode == TB_FILE_MODE_WO || mode == TB_FILE_MODE_RO, tb_null);
    
    HANDLE    pipefd = tb_null;
    tb_bool_t ok = tb_false;
    do
    {
        // get pipe name
        tb_wchar_t buffer[TB_PATH_MAXN];
        tb_wchar_t const* pipename = tb_pipe_file_name(name, buffer, tb_arrayn(buffer));
        tb_assert_and_check_break(pipename);

        // set pipe handles are inherited 
        SECURITY_ATTRIBUTES sattr; 
        sattr.nLength              = sizeof(SECURITY_ATTRIBUTES);
        sattr.bInheritHandle       = TRUE;
        sattr.lpSecurityDescriptor = tb_null;

        if (mode == TB_FILE_MODE_WO)
        {
            // create named pipe
            pipefd = CreateNamedPipeW(pipename, PIPE_ACCESS_INBOUND | PIPE_ACCESS_OUTBOUND, PIPE_NOWAIT, 1, (DWORD)buffer_size, (DWORD)buffer_size, 0, &sattr);
            tb_assert_and_check_break(pipefd && pipefd != INVALID_HANDLE_VALUE);

            // connect pipe
            if (!ConnectNamedPipe(pipefd, tb_null)) break;
        }
        else
        {
            // open named pipe
            pipefd = CreateFileW(pipename, GENERIC_READ | GENERIC_WRITE, 0, tb_null, OPEN_EXISTING, 0, tb_null);
            tb_assert_and_check_break(pipefd && pipefd != INVALID_HANDLE_VALUE);
        }

        // ok
        ok = tb_true;

    } while (0);

    // failed? 
    if (!ok)
    {
        if (pipefd != INVALID_HANDLE_VALUE)
            CloseHandle(pipefd);
        pipefd = tb_null;
    }
    return (tb_pipe_file_ref_t)pipefd;
}
tb_bool_t tb_pipe_file_init_pair(tb_pipe_file_ref_t pair[2], tb_size_t buffer_size)
{
    // check
    tb_assert_and_check_return_val(pair, tb_false);

    HANDLE    pipefd[2] = {0};
    tb_bool_t ok = tb_false;
    do
    {
        // set pipe handles are inherited 
        SECURITY_ATTRIBUTES sattr; 
        sattr.nLength              = sizeof(SECURITY_ATTRIBUTES);
        sattr.bInheritHandle       = TRUE;
        sattr.lpSecurityDescriptor = tb_null;

        // create pipe fd pair
        if (!CreatePipe(&pipefd[0], &pipefd[1], &sattr, (DWORD)buffer_size)) break;
        tb_assert_and_check_break(pipefd[0] != INVALID_HANDLE_VALUE);
        tb_assert_and_check_break(pipefd[1] != INVALID_HANDLE_VALUE);

        // save to file pair
        pair[0] = (tb_pipe_file_ref_t)pipefd[0];
        pair[1] = (tb_pipe_file_ref_t)pipefd[1];
        
        // ok
        ok = tb_true;

    } while (0);
    return ok;
}
tb_bool_t tb_pipe_file_exit(tb_pipe_file_ref_t file)
{
    // check
    tb_assert_and_check_return_val(file, tb_false);

    // close it
    return CloseHandle((HANDLE)file)? tb_true : tb_false;
}
tb_long_t tb_pipe_file_read(tb_pipe_file_ref_t file, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && data, -1);
    tb_check_return_val(size, 0);

    // read
    DWORD real_size = 0;
    return ReadFile((HANDLE)file, data, (DWORD)size, &real_size, tb_null)? (tb_long_t)real_size : -1;
}
tb_long_t tb_pipe_file_writ(tb_pipe_file_ref_t file, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && data, -1);
    tb_check_return_val(size, 0);

    // write
    DWORD real_size = 0;
    return WriteFile((HANDLE)file, data, (DWORD)size, &real_size, tb_null)? (tb_long_t)real_size : -1;
}
tb_long_t tb_pipe_file_wait(tb_pipe_file_ref_t file, tb_size_t events, tb_long_t timeout)
{
    tb_trace_noimpl();
    return -1;
}
