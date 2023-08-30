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
#include "prefix.h"
#include "../fwatcher.h"
#include "../file.h"
#include "../time.h"
#include "../directory.h"
#include "../../libc/libc.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#include "interface/interface.h"
#include "ntstatus.h"
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
#   include "../../coroutine/coroutine.h"
#   include "../../coroutine/impl/impl.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the watch item type
typedef struct __tb_fwatcher_item_t
{
    OVERLAPPED          overlapped;
    HANDLE              handle;
    tb_bool_t           stop;
    /* ReadDirectoryChangesW fails with ERROR_INVALID_PARAMETER when
     * the buffer length is greater than 64 KB and the application is monitoring a directory over the network.
     *
     * http://msdn.microsoft.com/en-us/library/windows/desktop/aa365465(v=vs.85).aspx)
     */
    BYTE                buffer[10 * 1024];
    tb_char_t const*    watchdir;
    tb_bool_t           recursion;

}tb_fwatcher_item_t;

// the fwatcher type
typedef struct __tb_fwatcher_t
{
    HANDLE              port;
    tb_hash_map_ref_t   watchitems;
    tb_queue_ref_t      waited_events;

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
        if (tb_kernel32()->CancelIoEx)
            tb_kernel32()->CancelIoEx(watchitem->handle, &watchitem->overlapped);
        CloseHandle(watchitem->handle);
        watchitem->handle = INVALID_HANDLE_VALUE;
    }
}

static tb_bool_t tb_fwatcher_item_refresh(tb_fwatcher_item_t* watchitem)
{
    // refresh directory watching
    return ReadDirectoryChangesW(watchitem->handle,
        watchitem->buffer, sizeof(watchitem->buffer), watchitem->recursion? TRUE : FALSE,
        FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
        tb_null, &watchitem->overlapped, tb_null);
}

static tb_bool_t tb_fwatcher_item_init(tb_fwatcher_t* fwatcher, tb_char_t const* watchdir, tb_fwatcher_item_t* watchitem)
{
    tb_assert_and_check_return_val(fwatcher && watchitem && watchdir && !watchitem->handle && fwatcher->port, tb_false);

    // get the absolute path
    tb_wchar_t watchdir_w[TB_PATH_MAXN];
    if (!tb_path_absolute_w(watchdir, watchdir_w, TB_PATH_MAXN))
        return tb_false;

    // create file
    watchitem->handle = CreateFileW(watchdir_w, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, tb_null,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, tb_null);
    tb_assert_and_check_return_val(watchitem->handle, tb_false);

    // bind to iocp port
    if (!CreateIoCompletionPort(watchitem->handle, fwatcher->port, 0, 1))
        return tb_false;

    // save file path
    watchitem->watchdir = watchdir;

    // refresh directory watching
    return tb_fwatcher_item_refresh(watchitem);
}

static tb_void_t tb_fwatcher_item_spak(tb_fwatcher_t* fwatcher, tb_fwatcher_item_t* watchitem)
{
    tb_assert_and_check_return(fwatcher && watchitem && watchitem->handle && watchitem->watchdir);

    tb_size_t offset = 0;
    tb_fwatcher_event_t event;
	PFILE_NOTIFY_INFORMATION notify;
    do
    {
		notify = (PFILE_NOTIFY_INFORMATION)&watchitem->buffer[offset];
		offset += notify->NextEntryOffset;

        // get file path
        tb_char_t filename[TB_PATH_MAXN];
		tb_int_t count = WideCharToMultiByte(CP_UTF8, 0, notify->FileName, notify->FileNameLength / sizeof(WCHAR),
            filename, sizeof(filename) - 1, tb_null, tb_null);
		filename[count] = '\0';
        tb_snprintf(event.filepath, TB_PATH_MAXN, "%s/%s", watchitem->watchdir, filename);

        // get event code
        if (notify->Action == FILE_ACTION_ADDED)
            event.event = TB_FWATCHER_EVENT_CREATE;
        else if (notify->Action == FILE_ACTION_MODIFIED)
            event.event = TB_FWATCHER_EVENT_MODIFY;
        else if (notify->Action == FILE_ACTION_REMOVED)
            event.event = TB_FWATCHER_EVENT_DELETE;
        else if (notify->Action == FILE_ACTION_RENAMED_NEW_NAME)
        {
            // the parent directory is changed
            event.event = TB_FWATCHER_EVENT_MODIFY;
            tb_strlcpy(event.filepath, watchitem->watchdir, sizeof(event.filepath));
        }

        // save event
        tb_queue_put(fwatcher->waited_events, &event);

    } while (notify->NextEntryOffset);

    // continue to refresh directory watching
	if (!watchitem->stop)
        tb_fwatcher_item_refresh(watchitem);
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

        // init waited events
        fwatcher->waited_events = tb_queue_init(0, tb_element_mem(sizeof(tb_fwatcher_event_t), tb_null, tb_null));
        tb_assert_and_check_break(fwatcher->waited_events);

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

        // exit waited events
        if (fwatcher->waited_events)
        {
            tb_queue_exit(fwatcher->waited_events);
            fwatcher->waited_events = tb_null;
        }

        // exit port
        if (fwatcher->port) CloseHandle(fwatcher->port);
        fwatcher->port = tb_null;

        // wait watcher
        tb_free(fwatcher);
        fwatcher = tb_null;
    }
}

tb_bool_t tb_fwatcher_add(tb_fwatcher_ref_t self, tb_char_t const* watchdir, tb_bool_t recursion)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->watchitems && watchdir, tb_false);

    // file not found
    tb_file_info_t info;
    if (!tb_file_info(watchdir, &info) || info.type != TB_FILE_TYPE_DIRECTORY)
        return tb_false;

    // this path has been added?
    tb_size_t itor = tb_hash_map_find(fwatcher->watchitems, watchdir);
    if (itor != tb_iterator_tail(fwatcher->watchitems))
        return tb_true;

    // save watch item
    tb_fwatcher_item_t watchitem;
    tb_memset(&watchitem, 0, sizeof(tb_fwatcher_item_t));
    watchitem.recursion = recursion;
    return tb_hash_map_insert(fwatcher->watchitems, watchdir, &watchitem) != tb_iterator_tail(fwatcher->watchitems);
}

tb_bool_t tb_fwatcher_remove(tb_fwatcher_ref_t self, tb_char_t const* watchdir)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->watchitems && watchdir, tb_false);

    // remove the watchitem
    tb_hash_map_remove(fwatcher->watchitems, watchdir);
    return tb_true;
}

tb_void_t tb_fwatcher_spak(tb_fwatcher_ref_t self)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return(fwatcher && fwatcher->port);

    PostQueuedCompletionStatus(fwatcher->port, 0, (ULONG_PTR)tb_u2p(1), tb_null);
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* event, tb_long_t timeout)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->port && fwatcher->watchitems && fwatcher->waited_events && event, -1);

#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
    // attempt to wait it in coroutine if timeout is non-zero
    if (timeout && tb_coroutine_self())
    {
        tb_poller_object_t object;
        object.type = TB_POLLER_OBJECT_FWATCHER;
        object.ref.fwatcher = self;
        return tb_coroutine_waitfs(&object, event, timeout);
    }
#endif

    // get it if has events
    tb_bool_t has_events = tb_false;
    if (!tb_queue_null(fwatcher->waited_events))
    {
        tb_fwatcher_event_t* e = (tb_fwatcher_event_t*)tb_queue_get(fwatcher->waited_events);
        if (e)
        {
            *event = *e;
            tb_queue_pop(fwatcher->waited_events);
            has_events = tb_true;
        }
    }
    tb_check_return_val(!has_events, 1);

    // init watch items
    tb_for_all(tb_hash_map_item_ref_t, item, fwatcher->watchitems)
    {
        // get watch item and path
        tb_char_t const* watchdir = (tb_char_t const*)item->name;
        tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)item->data;
        tb_assert_and_check_return_val(watchitem && watchdir, -1);

        // init watch item first
        if (!watchitem->handle && !tb_fwatcher_item_init(fwatcher, watchdir, watchitem))
        {
            tb_trace_d("watch %s failed", watchdir);
            return -1;
        }
    }

    // clear error first
    SetLastError(ERROR_SUCCESS);

    // wait event
    DWORD           real = 0;
    tb_pointer_t    pkey = tb_null;
    OVERLAPPED*     overlapped = tb_null;
    BOOL            wait_ok = GetQueuedCompletionStatus(fwatcher->port,
        (LPDWORD)&real, (PULONG_PTR)&pkey, (LPOVERLAPPED*)&overlapped, (DWORD)(timeout < 0? INFINITE : timeout));

    // the last error
    tb_size_t error = (tb_size_t)GetLastError();

    // timeout?
    if (!wait_ok && (error == WAIT_TIMEOUT || error == ERROR_OPERATION_ABORTED))
        return 0;

    // spank notification?
    if (tb_p2u32(pkey) == 0x1)
        return 0;

    // handle event
    if (real && overlapped)
        tb_fwatcher_item_spak(fwatcher, (tb_fwatcher_item_t*)overlapped);

    // get event
    if (!tb_queue_null(fwatcher->waited_events))
    {
        tb_fwatcher_event_t* e = (tb_fwatcher_event_t*)tb_queue_get(fwatcher->waited_events);
        if (e)
        {
            *event = *e;
            tb_queue_pop(fwatcher->waited_events);
            has_events = tb_true;
        }
    }
    return has_events? 1 : 0;
}
