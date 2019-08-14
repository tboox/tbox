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
 * types
 */

// the pipe file type
typedef struct __tb_pipe_file_t
{
    // the pipe handle
    HANDLE              pipe;

    // the pipe name
    tb_char_t const*    name;

    // the data buffer
    tb_byte_t*          data;

    // the real writed/readed size
    DWORD               real;

    // the overlap
    OVERLAPPED          overlap; 

}tb_pipe_file_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c_enter__
HANDLE tb_pipe_file_handle(tb_pipe_file_t* file);
__tb_extern_c_leave__

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
static tb_pipe_file_t* tb_pipe_file_init_impl(tb_char_t const* name, HANDLE pipe)
{
    // check
    tb_assert_and_check_return_val(pipe, tb_null);

    // init pipe file
    tb_pipe_file_t* file = tb_malloc0_type(tb_pipe_file_t);
    tb_assert_and_check_return_val(file, tb_null);

    file->data = tb_null;
    file->real = 0;
    file->pipe = pipe;
    file->name = name? tb_strdup(name) : tb_null;
    return file;
}
HANDLE tb_pipe_file_handle(tb_pipe_file_t* file)
{
    tb_assert_and_check_return_val(file, tb_null);
    return file->pipe;
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

        // set pipe handles are not inherited 
        SECURITY_ATTRIBUTES sattr; 
        sattr.nLength              = sizeof(SECURITY_ATTRIBUTES);
        sattr.bInheritHandle       = FALSE;
        sattr.lpSecurityDescriptor = tb_null;

        if (mode == TB_FILE_MODE_WO)
        {
            // create named pipe
            pipefd = CreateNamedPipeW(pipename, PIPE_ACCESS_INBOUND | PIPE_ACCESS_OUTBOUND, PIPE_WAIT, 1, (DWORD)buffer_size, (DWORD)buffer_size, 0, &sattr);
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
    return pipefd? (tb_pipe_file_ref_t)tb_pipe_file_init_impl(name, pipefd) : tb_null;
}
tb_bool_t tb_pipe_file_init_pair(tb_pipe_file_ref_t pair[2], tb_size_t buffer_size)
{
    // check
    tb_assert_and_check_return_val(pair, tb_false);

    HANDLE    pipefd[2] = {0};
    tb_bool_t ok = tb_false;
    do
    {
        // set pipe handles are not inherited 
        SECURITY_ATTRIBUTES sattr; 
        sattr.nLength              = sizeof(SECURITY_ATTRIBUTES);
        sattr.bInheritHandle       = FALSE;
        sattr.lpSecurityDescriptor = tb_null;

        // create pipe fd pair
        if (!CreatePipe(&pipefd[0], &pipefd[1], &sattr, (DWORD)buffer_size)) break;
        tb_assert_and_check_break(pipefd[0] != INVALID_HANDLE_VALUE);
        tb_assert_and_check_break(pipefd[1] != INVALID_HANDLE_VALUE);

        // save to file pair
        pair[0] = (tb_pipe_file_ref_t)tb_pipe_file_init_impl(tb_null, pipefd[0]);
        pair[1] = (tb_pipe_file_ref_t)tb_pipe_file_init_impl(tb_null, pipefd[1]);
        tb_assert_and_check_break(pair[0] && pair[1]);
        
        // ok
        ok = tb_true;

    } while (0);

    if (!ok)
    {
        if (pair[0]) tb_pipe_file_exit(pair[0]);
        if (pair[1]) tb_pipe_file_exit(pair[1]);
        pair[0] = tb_null;
        pair[1] = tb_null;
    }
    return ok;
}
tb_bool_t tb_pipe_file_exit(tb_pipe_file_ref_t self)
{
    // check
    tb_pipe_file_t* file = (tb_pipe_file_t*)self;
    tb_assert_and_check_return_val(file, tb_false);

    // disconnect the named pipe
    if (file->name && file->pipe) DisconnectNamedPipe(file->pipe);

    // close pipe
    if (file->pipe) 
    {
        if (!CloseHandle(file->pipe) && GetLastError() != ERROR_INVALID_HANDLE)
            return tb_false;
        file->pipe = tb_null;
    }

    // exit name
    if (file->name) tb_free(file->name);
    file->name = tb_null;

    // exit the pipe file
    tb_free(file);
    return tb_true;
}
tb_long_t tb_pipe_file_read(tb_pipe_file_ref_t self, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_pipe_file_t* file = (tb_pipe_file_t*)self;
    tb_assert_and_check_return_val(file && file->pipe && data, -1);
    tb_check_return_val(size, 0);

    // read
    if (file->name)
    {
        // has the completed result?
        tb_check_return_val(!file->real, (tb_long_t)file->real);

        // read data
        file->data = data;
        file->real = 0;
        BOOL ok = ReadFile(file->pipe, data, (DWORD)size, &file->real, &file->overlap);
        if (ok && file->real == size)
        {
            DWORD real_size = file->real;
            file->data = tb_null;
            file->real = 0;
            return (tb_long_t)real_size;
        }
        else
        {
            // pending?
            if (!ok && (GetLastError() == ERROR_IO_PENDING)) 
                return 0;
            else 
            {
                file->data = tb_null;
                file->real = 0;
                return -1;
            }
        }
    }
    else 
    {
        DWORD real_size = 0;
        BOOL  ok = PeekNamedPipe(file->pipe, data, 1, &real_size, tb_null, tb_null);
        if (ok && real_size > 0)
            return ReadFile(file->pipe, data, (DWORD)size, &real_size, tb_null)? (tb_long_t)real_size : -1;
        else return -1;
    }
}
tb_long_t tb_pipe_file_writ(tb_pipe_file_ref_t self, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_pipe_file_t* file = (tb_pipe_file_t*)self;
    tb_assert_and_check_return_val(file && file->pipe && data, -1);
    tb_check_return_val(size, 0);

    // write
    if (file->name)
    {
        // has the completed result?
        tb_check_return_val(!file->real, (tb_long_t)file->real);

        // write data
        file->data = (tb_byte_t*)data;
        file->real = 0;
        BOOL ok = WriteFile(file->pipe, file->data, (DWORD)size, &file->real, &file->overlap);
        if (ok && file->real == size)
        {
            DWORD real_size = file->real;
            file->data = tb_null;
            file->real = 0;
            return (tb_long_t)real_size;
        }
        else
        {
            // pending?
            if (!ok && (GetLastError() == ERROR_IO_PENDING)) 
                return 0;
            else 
            {
                file->data = tb_null;
                file->real = 0;
                return -1;
            }
        }
    }
    else 
    {
        DWORD real_size = 0;
        return WriteFile(file->pipe, data, (DWORD)size, &real_size, tb_null)? (tb_long_t)real_size : -1;
    }
}
tb_long_t tb_pipe_file_wait(tb_pipe_file_ref_t self, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_pipe_file_t* file = (tb_pipe_file_t*)self;
    tb_assert_and_check_return_val(file && file->pipe, -1);

    // wait it
    tb_long_t   ok = -1;
    DWORD       result = WaitForSingleObject(file->pipe, timeout < 0? INFINITE : (DWORD)timeout);
    switch (result)
    {
    case WAIT_OBJECT_0: // ok
        {
            // wait for named pipe?
            if (file->name)
            {
                // pending?
                if (file->data && GetOverlappedResult(file->pipe, &file->overlap, &file->real, FALSE))
                    ok = 1;
            } 
            else ok = 1;
        }
        break;
    case WAIT_TIMEOUT: // timeout 
        ok = 0;
        break;
    case WAIT_FAILED: // failed
    default:
        break;
    }
    return ok;
}
