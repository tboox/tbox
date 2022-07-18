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
 * @file        fwatcher_iocp.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../fwatcher.h"
#include "../file.h"
#include "../time.h"
#include "../directory.h"
#include "../../libc/libc.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#include "interface/interface.h"
#include "ntstatus.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the watch item type
typedef struct __tb_fwatcher_item_t
{
    HANDLE              handle;
    OVERLAPPED          overlapped;
    tb_bool_t           stop;
    /* ReadDirectoryChangesW fails with ERROR_INVALID_PARAMETER when
     * the buffer length is greater than 64 KB and the application is monitoring a directory over the network.
     *
     * http://msdn.microsoft.com/en-us/library/windows/desktop/aa365465(v=vs.85).aspx)
     */
    BYTE                buffer[10 * 1024];

}tb_fwatcher_item_t;

// the fwatcher type
typedef struct __tb_fwatcher_t
{
    HANDLE              port;
    tb_hash_map_ref_t   watchitems;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_fwatcher_item_free(tb_element_ref_t element, tb_pointer_t buff)
{
    tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)buff;
    tb_assert_and_check_return(watchitem);

    watchitem->stop = tb_true;
    if (watchitem->handle && watchitem->handle != INVALID_HANDLE_VALUE)
    {
        CancelIoEx(watchitem->handle, &watchitem->overlapped);
        CloseHandle(watchitem->handle);
        watchitem->handle = INVALID_HANDLE_VALUE;
    }
}

static tb_bool_t tb_fwatcher_item_init(tb_fwatcher_t* fwatcher, tb_char_t const* filepath, tb_fwatcher_item_t* watchitem)
{
    tb_assert_and_check_return_val(fwatcher && watchitem && filepath && !watchitem->handle && fwatcher->port, tb_false);

    // get the absolute path
    tb_wchar_t filepath_w[TB_PATH_MAXN];
    if (!tb_path_absolute_w(filepath, filepath_w, TB_PATH_MAXN))
        return tb_false;

    // create file
    watchitem->handle = CreateFileW(filepath_w, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, tb_null,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, tb_null);
    tb_assert_and_check_return_val(watchitem->handle, tb_false);

    // bind to iocp port
    if (!CreateIoCompletionPort(watchitem->handle, fwatcher->port, 0, 1))
        return tb_false;

    // refresh directory watching
    return ReadDirectoryChangesW(watchitem->handle,
        watchitem->buffer, sizeof(watchitem->buffer), TRUE,
        FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
        tb_null, &watchitem->overlapped, tb_null);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_fwatcher_ref_t tb_fwatcher_init()
{
    tb_bool_t ok = tb_false;
    tb_fwatcher_t* fwatcher = tb_null;
    do
    {
        // init fwatcher
        fwatcher = tb_malloc0_type(tb_fwatcher_t);
        tb_assert_and_check_break(fwatcher);

        // init port
        fwatcher->port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, tb_null, 0, 0);
        tb_assert_and_check_break(fwatcher->port);

        // init watch items
        fwatcher->watchitems = tb_hash_map_init(0, tb_element_str(tb_true), tb_element_mem(sizeof(tb_fwatcher_item_t), tb_fwatcher_item_free, tb_null));
        tb_assert_and_check_break(fwatcher->watchitems);

        ok = tb_true;
    } while (0);

    if (!ok && fwatcher)
    {
        tb_fwatcher_exit((tb_fwatcher_ref_t)fwatcher);
        fwatcher = tb_null;
    }
    return (tb_fwatcher_ref_t)fwatcher;
}

tb_void_t tb_fwatcher_exit(tb_fwatcher_ref_t self)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    if (fwatcher)
    {
        // exit watch items
        if (fwatcher->watchitems)
        {
            tb_hash_map_exit(fwatcher->watchitems);
            fwatcher->watchitems = tb_null;
        }

        // exit port
        if (fwatcher->port) CloseHandle(fwatcher->port);
        fwatcher->port = tb_null;

        // wait watcher
        tb_free(fwatcher);
        fwatcher = tb_null;
    }
}

tb_bool_t tb_fwatcher_add(tb_fwatcher_ref_t self, tb_char_t const* filepath)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->watchitems && filepath, tb_false);

    // file not found
    tb_file_info_t info;
    if (!tb_file_info(filepath, &info))
        return tb_false;

    // this path has been added?
    tb_size_t itor = tb_hash_map_find(fwatcher->watchitems, filepath);
    if (itor != tb_iterator_tail(fwatcher->watchitems))
        return tb_true;

    // save watch item
    tb_fwatcher_item_t watchitem;
    tb_memset(&watchitem, 0, sizeof(tb_fwatcher_item_t));
    return tb_hash_map_insert(fwatcher->watchitems, filepath, &watchitem) != tb_iterator_tail(fwatcher->watchitems);
}

tb_bool_t tb_fwatcher_remove(tb_fwatcher_ref_t self, tb_char_t const* filepath)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->watchitems && filepath, tb_false);

    // remove the watchitem
    tb_hash_map_remove(fwatcher->watchitems, filepath);
    return tb_true;
}

tb_void_t tb_fwatcher_spak(tb_fwatcher_ref_t self)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return(fwatcher && fwatcher->port);

}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* events, tb_size_t events_maxn, tb_long_t timeout)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->port && fwatcher->watchitems && events && events_maxn, -1);

    // init watch items
    tb_for_all(tb_hash_map_item_ref_t, item, fwatcher->watchitems)
    {
        // get watch item and path
        tb_char_t const* path = (tb_char_t const*)item->name;
        tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)item->data;
        tb_assert_and_check_return_val(watchitem && path, -1);

        // init watch item first
        if (!watchitem->handle && !tb_fwatcher_item_init(fwatcher, path, watchitem))
        {
            tb_trace_d("watch %s failed", path);
            return -1;
        }
    }

    // wait watch items
    if (tb_hash_map_size(fwatcher->watchitems))
    {
        tb_size_t wait = 0;
        while (1)
        {
            // compute the timeout
            if (wait) timeout = 0;

            // clear error first
            SetLastError(ERROR_SUCCESS);

            // wait event
            DWORD           real = 0;
            tb_pointer_t    pkey = tb_null;
            OVERLAPPED*     ov = tb_null;
            BOOL            wait_ok = GetQueuedCompletionStatus(fwatcher->port,
                (LPDWORD)&real, (PULONG_PTR)&pkey, (LPOVERLAPPED*)&ov, (DWORD)(timeout < 0? INFINITE : timeout));

            // the last error
            tb_size_t error = (tb_size_t)GetLastError();

            // timeout?
            if (!wait_ok && (error == WAIT_TIMEOUT || error == ERROR_OPERATION_ABORTED))
                break;

            // is spank?
            // TODO pkey

            // handle event
            tb_trace_i("real: %d, ov: %p", real, ov);
        }
    }
    else
    {
        // TODO use event
        tb_hong_t time = tb_mclock();
        while (timeout < 0 || tb_mclock() < time + timeout)
        {
            tb_msleep(100);
        }
    }

    return 0;
}
