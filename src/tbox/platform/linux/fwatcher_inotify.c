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
 * @file        fwatcher_inotify.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../fwatcher.h"
#include "../socket.h"
#include "../poller.h"
#include "../file.h"
#include "../directory.h"
#include "../../libc/libc.h"
#include "../impl/pollerdata.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_FWATCHER_EVENT_SIZE          (sizeof(struct inotify_event))
#ifdef TB_CONFIG_SMALL
#   define TB_FWATCHER_BUFFER_SIZE      (4096 * (TB_FWATCHER_EVENT_SIZE + 16))
#else
#   define TB_FWATCHER_BUFFER_SIZE      (8192 * (TB_FWATCHER_EVENT_SIZE + 16))
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the fwatcher type
typedef struct __tb_fwatcher_t
{
    tb_int_t             fd;
    tb_byte_t            buffer[TB_FWATCHER_BUFFER_SIZE];
    tb_poller_ref_t      poller;
    tb_pollerdata_t      pollerdata;
    tb_hash_map_ref_t    filepath_fds;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_fwatcher_event(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    // we need only an empty callback
}

static tb_bool_t tb_fwatcher_free_fd(tb_iterator_ref_t iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)priv;
    tb_hash_map_item_ref_t fd_item = (tb_hash_map_item_ref_t)item;
    tb_assert_and_check_return_val(fwatcher && fd_item, tb_false);

    // remove fd watch
    inotify_rm_watch(fwatcher->fd, (tb_int_t)(tb_long_t)fd_item->data);
    return tb_true;
}

static tb_bool_t tb_fwatcher_add_watch(tb_fwatcher_t* fwatcher, tb_char_t const* filepath)
{
    // check
    tb_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->filepath_fds && filepath, tb_false);

    // this path has been added?
    tb_size_t itor = tb_hash_map_find(fwatcher->filepath_fds, filepath);
    if (itor != tb_iterator_tail(fwatcher->filepath_fds))
        return tb_true;

    // add watch
    tb_int_t wd = inotify_add_watch(fwatcher->fd, filepath, IN_MODIFY | IN_CREATE | IN_DELETE);
    tb_assert_and_check_return_val(wd >= 0, tb_false);

    // save file path
    tb_char_t const* path = tb_null;
    if ((itor = tb_hash_map_insert(fwatcher->filepath_fds, filepath, tb_i2p(wd))) != tb_iterator_tail(fwatcher->filepath_fds))
    {
        tb_hash_map_item_ref_t item = (tb_hash_map_item_ref_t)tb_iterator_item(fwatcher->filepath_fds, itor);
        if (item) path = item->name;
    }
    tb_assert_and_check_return_val(path, tb_false);

    // save watch file path
    tb_poller_object_t object;
    object.ref.sock = tb_fd2sock(wd); // we just wrap socket object as key
    tb_pollerdata_set(&fwatcher->pollerdata, &object, path);
    return tb_true;
}

static tb_long_t tb_fwatcher_add_watch_dirs(tb_char_t const* path, tb_file_info_t const* info, tb_cpointer_t priv)
{
    // check
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)priv;
    tb_assert_and_check_return_val(path && info && fwatcher, TB_DIRECTORY_WALK_CODE_END);

    // add watch directory
    if (info->type == TB_FILE_TYPE_DIRECTORY)
        tb_fwatcher_add_watch(fwatcher, path);
    return TB_DIRECTORY_WALK_CODE_CONTINUE;
}

static tb_bool_t tb_fwatcher_rm_watch(tb_fwatcher_t* fwatcher, tb_char_t const* filepath)
{
    // check
    tb_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->filepath_fds && filepath, tb_false);

    // remove file path and fd
    tb_size_t itor = tb_hash_map_find(fwatcher->filepath_fds, filepath);
    if (itor != tb_iterator_tail(fwatcher->filepath_fds))
    {
        // remove fd watch
        tb_hash_map_item_ref_t item = (tb_hash_map_item_ref_t)tb_iterator_item(fwatcher->filepath_fds, itor);
        if (item)
        {
            if (0 != inotify_rm_watch(fwatcher->fd, (tb_int_t)(tb_long_t)item->data))
                return tb_false;
        }

        // remove it
        tb_iterator_remove(fwatcher->filepath_fds, itor);
    }
    return tb_true;
}

static tb_long_t tb_fwatcher_rm_watch_dirs(tb_char_t const* path, tb_file_info_t const* info, tb_cpointer_t priv)
{
    // check
    tb_value_t* values = (tb_value_t*)priv;
    tb_assert_and_check_return_val(path && info && values, TB_DIRECTORY_WALK_CODE_END);

    // get fwatcher
    tb_fwatcher_t* fwatcher = values[0].ptr;
    tb_assert_and_check_return_val(fwatcher, TB_DIRECTORY_WALK_CODE_END);

    // rm watch directory
    if (info->type == TB_FILE_TYPE_DIRECTORY)
        tb_fwatcher_rm_watch(fwatcher, path);
    return TB_DIRECTORY_WALK_CODE_CONTINUE;
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

        // init inotify
        fwatcher->fd = inotify_init();
        tb_assert_and_check_break(fwatcher->fd >= 0);

        // init poller
        fwatcher->poller = tb_poller_init(tb_null);
        tb_assert_and_check_break(fwatcher->poller);

        tb_poller_insert_sock(fwatcher->poller, tb_fd2sock(fwatcher->fd), TB_POLLER_EVENT_RECV, tb_null);

        // init pollerdata
        tb_pollerdata_init(&fwatcher->pollerdata);

        // init filepath fds
        fwatcher->filepath_fds = tb_hash_map_init(0, tb_element_str(tb_true), tb_element_uint32());
        tb_assert_and_check_break(fwatcher->filepath_fds);

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
        // exit pollerdata
        tb_pollerdata_exit(&fwatcher->pollerdata);

        // exit poller
        if (fwatcher->poller)
            tb_poller_exit(fwatcher->poller);

        // exit filepath fds
        if (fwatcher->filepath_fds)
        {
            tb_walk_all(fwatcher->filepath_fds, tb_fwatcher_free_fd, fwatcher);
            tb_hash_map_exit(fwatcher->filepath_fds);
            fwatcher->filepath_fds = tb_null;
        }

        // exit fd
        if (fwatcher->fd >= 0)
        {
            close(fwatcher->fd);
            fwatcher->fd = -1;
        }

        // exit watcher
        tb_free(fwatcher);
        fwatcher = tb_null;
    }
}

tb_bool_t tb_fwatcher_add(tb_fwatcher_ref_t self, tb_char_t const* filepath)
{
    // check
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->filepath_fds && filepath, tb_false);

    // file not found
    tb_file_info_t info;
    if (!tb_file_info(filepath, &info))
        return tb_false;

    // is directory? we need scan it and add all subdirs
    if (info.type == TB_FILE_TYPE_DIRECTORY)
        tb_directory_walk(filepath, 0, tb_true, tb_fwatcher_add_watch_dirs, fwatcher);
    return tb_fwatcher_add_watch(fwatcher, filepath);
}

tb_bool_t tb_fwatcher_remove(tb_fwatcher_ref_t self, tb_char_t const* filepath)
{
    // check
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && filepath, tb_false);

    // is directory? we need scan it and remove all subdirs
    tb_file_info_t info;
    if (tb_file_info(filepath, &info) && info.type == TB_FILE_TYPE_DIRECTORY)
    {
        tb_value_t values[1];
        values[0].ptr = (tb_pointer_t)fwatcher;
        tb_directory_walk(filepath, 0, tb_false, tb_fwatcher_rm_watch_dirs, values);
    }
    return tb_fwatcher_rm_watch(fwatcher, filepath);
}

tb_void_t tb_fwatcher_spak(tb_fwatcher_ref_t self)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    if (fwatcher && fwatcher->poller)
        tb_poller_spak(fwatcher->poller);
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* events, tb_size_t events_maxn, tb_long_t timeout)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && events && events_maxn, -1);

    // wait events
    tb_long_t wait = tb_poller_wait(fwatcher->poller, tb_fwatcher_event, timeout);
    tb_assert_and_check_return_val(wait >= 0, -1);
    tb_check_return_val(wait > 0, 0);

    tb_int_t real = read(fwatcher->fd, fwatcher->buffer, sizeof(fwatcher->buffer));
    tb_check_return_val(real >= 0, -1);

    tb_int_t i = 0;
    tb_size_t events_count = 0;
    while (i < real && events_count < events_maxn)
    {
        struct inotify_event* event = (struct inotify_event*)&fwatcher->buffer[i];

        // get event code
        tb_size_t event_code = 0;
        if (event->mask & IN_CREATE)
            event_code = TB_FWATCHER_EVENT_CREATE;
        else if (event->mask & IN_DELETE)
            event_code = TB_FWATCHER_EVENT_DELETE;
        else if (event->mask & IN_MODIFY)
            event_code = TB_FWATCHER_EVENT_MODIFY;

        // get file path
        tb_poller_object_t object;
        object.ref.sock = tb_fd2sock(event->wd); // we just wrap socket object as key
        tb_char_t const* filepath = tb_pollerdata_get(&fwatcher->pollerdata, &object);

        // add event
        if (event_code)
        {
            if (filepath && event->len)
                tb_snprintf(events[events_count].filepath, TB_PATH_MAXN, "%s/%s", filepath, event->name);
            else if (filepath)
                tb_strlcpy(events[events_count].filepath, filepath, TB_PATH_MAXN);
            else events[events_count].filepath[0] = '\0';
            events[events_count].event = event_code;
            events_count++;
        }
        i += TB_FWATCHER_EVENT_SIZE + event->len;

        // rescan the watch directory
        tb_file_info_t info;
        if (event_code && filepath && tb_file_info(filepath, &info) && info.type == TB_FILE_TYPE_DIRECTORY)
        {
            if (event_code == TB_FWATCHER_EVENT_MODIFY ||
                event_code == TB_FWATCHER_EVENT_CREATE)
                tb_fwatcher_add(self, filepath);
            else if (event_code == TB_FWATCHER_EVENT_DELETE)
                tb_fwatcher_remove(self, filepath);
        }
    }
    return events_count;
}