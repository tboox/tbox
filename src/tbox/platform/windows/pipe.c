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
 * includes
 */
#include "prefix.h"
#include "../pipe.h"
#include "../file.h"
#include "../atomic.h"
#include "iocp_object.h"
#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
#   include "../../coroutine/coroutine.h"
#   include "../../coroutine/impl/impl.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the anonymous pipe name prefix
#define TB_PIPE_ANONYMOUS_PREFIX    "tbox_pipe_"

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

    // is connecting?
    tb_bool_t           connecting;

    // is connected
    tb_bool_t           connected;

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
static tb_long_t tb_pipe_file_connect_direct(tb_pipe_file_ref_t self)
{
    // check
    tb_pipe_file_t* file = (tb_pipe_file_t*)self;
    tb_assert_and_check_return_val(file && file->pipe, -1);

    // has the completed result?
    tb_check_return_val(!file->connected, 1);
    tb_assert_and_check_return_val(!file->connecting, 0);

    // connect pipe
    BOOL ok = ConnectNamedPipe(file->pipe, &file->overlap);
    if (ok) return 1;
    else
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            file->connecting = tb_true;
            return 0;
        }
        else return -1;
    }
}
static tb_long_t tb_pipe_file_wait_direct(tb_pipe_file_ref_t self, tb_size_t events, tb_long_t timeout)
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
            // get pending result?
            if (GetOverlappedResult(file->pipe, &file->overlap, &file->real, FALSE))
            {
                if (file->connecting) file->connected = tb_true;
                ok = events;
            }
            file->connecting = tb_false;
        }
        break;
    case WAIT_TIMEOUT: // timeout
        ok = 0;
        break;
    case WAIT_FAILED: // failed
    default:
        file->connecting = tb_false;
        break;
    }
    return ok;
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

    tb_bool_t       ok = tb_false;
    tb_pipe_file_t* file = tb_null;
    do
    {
        // init pipe file
        file = tb_malloc0_type(tb_pipe_file_t);
        tb_assert_and_check_break(file);

        // get pipe name
        tb_wchar_t buffer[TB_PATH_MAXN];
        tb_wchar_t const* pipename = tb_pipe_file_name(name, buffer, tb_arrayn(buffer));
        tb_assert_and_check_break(pipename);

        // save the pipe name if be named pipe
        if (!tb_strstr(name, TB_PIPE_ANONYMOUS_PREFIX))
        {
            file->name = tb_strdup(name);
            tb_assert_and_check_break(file->name);
        }

        // set pipe handles are not inherited
        SECURITY_ATTRIBUTES sattr;
        sattr.nLength              = sizeof(SECURITY_ATTRIBUTES);
        sattr.bInheritHandle       = FALSE;
        sattr.lpSecurityDescriptor = tb_null;

        // open or create named pipe
        if (mode == TB_PIPE_MODE_WO)
        {
            file->pipe = CreateFileW(pipename, GENERIC_WRITE, 0, tb_null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, tb_null);
            if (file->pipe == INVALID_HANDLE_VALUE)
                file->pipe = CreateNamedPipeW(pipename, PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE, PIPE_REJECT_REMOTE_CLIENTS | PIPE_TYPE_BYTE | PIPE_WAIT, 1, (DWORD)buffer_size, (DWORD)buffer_size, 0, &sattr);
            tb_assert_and_check_break(file->pipe && file->pipe != INVALID_HANDLE_VALUE);
        }
        else
        {
            file->pipe = CreateFileW(pipename, GENERIC_READ, 0, tb_null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, tb_null);
            if (file->pipe == INVALID_HANDLE_VALUE)
                file->pipe = CreateNamedPipeW(pipename, PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE, PIPE_REJECT_REMOTE_CLIENTS | PIPE_TYPE_BYTE | PIPE_WAIT, 1, (DWORD)buffer_size, (DWORD)buffer_size, 0, &sattr);
            tb_assert_and_check_break(file->pipe && file->pipe != INVALID_HANDLE_VALUE);
        }

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit the pipe file
        if (file) tb_pipe_file_exit((tb_pipe_file_ref_t)file);
        file = tb_null;
    }
    return (tb_pipe_file_ref_t)file;
}
tb_bool_t tb_pipe_file_init_pair(tb_pipe_file_ref_t pair[2], tb_size_t mode[2], tb_size_t buffer_size)
{
    // check
    tb_assert_and_check_return_val(pair, tb_false);

    tb_bool_t ok = tb_false;
    do
    {
        // init the pipe pair
        pair[0] = tb_null;
        pair[1] = tb_null;

        // get pid and tid
        DWORD pid = GetCurrentProcessId();
        DWORD tid = GetCurrentThreadId();

        // get the timestamp
        LARGE_INTEGER timestamp;
        BOOL success = QueryPerformanceCounter(&timestamp);
        tb_assert_and_check_break(success);

        // get the pipe name id
        static tb_atomic32_t s_pipe_id = 0;

        /* get the unique pipe name
         *
         * asynchronous (overlapped) read and write operations are not supported by anonymous pipes.
         * @see https://docs.microsoft.com/zh-cn/windows/win32/ipc/anonymous-pipe-operations?redirectedfrom=MSDN
         *
         * so we need to use a named pipe with a unique name to support it. (for overlapped/io and iocp)
         */
        tb_char_t name[128] = {0};
        tb_long_t size = tb_snprintf(name, sizeof(name), TB_PIPE_ANONYMOUS_PREFIX "%08x_%08x_%08x_%08x%08x", pid, tid, tb_atomic32_fetch_and_add(&s_pipe_id, 1), timestamp.HighPart, timestamp.LowPart);
        tb_assert_and_check_break(size > 0);

        // init the anonymous pipe for writing
        pair[1] = tb_pipe_file_init(name, TB_PIPE_MODE_WO, buffer_size);
        tb_assert_and_check_break(pair[1]);

        // connect the writed pipe first
        tb_long_t connected = tb_pipe_file_connect_direct(pair[1]);

        // init the anonymous pipe for reading
        pair[0] = tb_pipe_file_init(name, TB_PIPE_MODE_RO, buffer_size);
        tb_assert_and_check_break(pair[0]);

        // wait the connected result
        do
        {
            tb_long_t wait = tb_pipe_file_wait_direct(pair[1], TB_PIPE_EVENT_CONN, -1);
            tb_assert_and_check_break(wait > 0);

        } while (!(connected = tb_pipe_file_connect_direct(pair[1])));
        tb_assert_and_check_break(connected > 0);

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
tb_long_t tb_pipe_file_connect(tb_pipe_file_ref_t self)
{
#ifndef TB_CONFIG_MICRO_ENABLE
    // attempt to use iocp object to read data if exists
    tb_iocp_object_ref_t iocp_object = tb_iocp_object_get_or_new_from_pipe(self, TB_POLLER_EVENT_CONN);
    if (iocp_object) return tb_iocp_object_connect_pipe(iocp_object);
#endif
    return tb_pipe_file_connect_direct(self);
}
tb_bool_t tb_pipe_file_exit(tb_pipe_file_ref_t self)
{
    // check
    tb_pipe_file_t* file = (tb_pipe_file_t*)self;
    tb_assert_and_check_return_val(file, tb_false);

#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) || !defined(TB_CONFIG_MICRO_ENABLE)
    // init pipe object
    tb_poller_object_t object;
    object.type     = TB_POLLER_OBJECT_PIPE;
    object.ref.pipe = self;
#endif

#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
    // attempt to cancel waiting from coroutine first
    tb_pointer_t scheduler_io = tb_null;
#   ifndef TB_CONFIG_MICRO_ENABLE
    if ((scheduler_io = tb_co_scheduler_io_self()) && tb_co_scheduler_io_cancel((tb_co_scheduler_io_ref_t)scheduler_io, &object)) {}
    else
#   endif
    if ((scheduler_io = tb_lo_scheduler_io_self()) && tb_lo_scheduler_io_cancel((tb_lo_scheduler_io_ref_t)scheduler_io, &object)) {}
#endif

#ifndef TB_CONFIG_MICRO_ENABLE
    // remove iocp object for this pipe file if exists
    tb_iocp_object_remove(&object);
#endif

    // disconnect the named pipe
    if (file->connected && file->pipe) DisconnectNamedPipe(file->pipe);

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

#ifndef TB_CONFIG_MICRO_ENABLE
    // attempt to use iocp object to read data if exists
    tb_iocp_object_ref_t iocp_object = tb_iocp_object_get_or_new_from_pipe(self, TB_POLLER_EVENT_RECV);
    if (iocp_object) return tb_iocp_object_read(iocp_object, data, size);
#endif

    // has the completed result?
    if (file->real)
    {
         tb_long_t result = (tb_long_t)file->real;
         file->real = 0;
         return result;
    }

    // read data
    file->data = data;
    file->real = 0;
    BOOL ok = ReadFile(file->pipe, data, (DWORD)size, &file->real, &file->overlap);
    if (ok)
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
tb_long_t tb_pipe_file_write(tb_pipe_file_ref_t self, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_pipe_file_t* file = (tb_pipe_file_t*)self;
    tb_assert_and_check_return_val(file && file->pipe && data, -1);
    tb_check_return_val(size, 0);

#ifndef TB_CONFIG_MICRO_ENABLE
    // attempt to use iocp object to read data if exists
    tb_iocp_object_ref_t iocp_object = tb_iocp_object_get_or_new_from_pipe(self, TB_POLLER_EVENT_SEND);
    if (iocp_object) return tb_iocp_object_write(iocp_object, data, size);
#endif

    // has the completed result?
    if (file->real)
    {
         tb_long_t result = (tb_long_t)file->real;
         file->real = 0;
         return result;
    }

    // write data
    file->data = (tb_byte_t*)data;
    file->real = 0;
    BOOL ok = WriteFile(file->pipe, file->data, (DWORD)size, &file->real, &file->overlap);
    if (ok)
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
tb_long_t tb_pipe_file_wait(tb_pipe_file_ref_t self, tb_size_t events, tb_long_t timeout)
{
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
    // attempt to wait it in coroutine
    if (tb_coroutine_self())
    {
        tb_poller_object_t object;
        object.type = TB_POLLER_OBJECT_PIPE;
        object.ref.pipe = self;
        return tb_coroutine_waitio(&object, events, timeout);
    }
#endif

    // wait it
    return tb_pipe_file_wait_direct(self, events, timeout);
}
