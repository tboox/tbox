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
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
#   include "../../coroutine/coroutine.h"
#   include "../../coroutine/impl/impl.h"
#endif

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

// the watch item type
typedef struct __tb_fwatcher_item_t
{
    tb_int_t            wd;
    tb_char_t const*    watchdir;
    tb_bool_t           recursion;

}tb_fwatcher_item_t;

// the fwatcher type
typedef struct __tb_fwatcher_t
{
    tb_int_t             fd;
    tb_byte_t            buffer[TB_FWATCHER_BUFFER_SIZE];
    tb_poller_ref_t      poller;
    tb_hash_map_ref_t    watchitems;
    tb_queue_ref_t       waited_events;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_fwatcher_event(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    // we need only an empty callback
}

static tb_bool_t tb_fwatcher_item_eq(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    tb_hash_map_item_ref_t hashitem = (tb_hash_map_item_ref_t)item;
    tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)hashitem->data;
    if (watchitem && watchitem->watchdir && value && !tb_strcmp(watchitem->watchdir, value))
        return tb_true;
    return tb_false;
}

static tb_void_t tb_fwatcher_item_free(tb_element_ref_t element, tb_pointer_t buff)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)element->priv;
    tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)buff;
    tb_assert_and_check_return(fwatcher && watchitem);

    if (fwatcher->fd >= 0 && watchitem->wd >= 0)
    {
        inotify_rm_watch(fwatcher->fd, watchitem->wd);
        watchitem->wd = -1;
    }

    if (watchitem->watchdir) tb_free(watchitem->watchdir);
    watchitem->watchdir = tb_null;
}

static tb_bool_t tb_fwatcher_add_watch(tb_fwatcher_t* fwatcher, tb_char_t const* watchdir, tb_bool_t recursion)
{
    // check
    tb_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->watchitems && watchdir, tb_false);

    // this directory has been watched?
    tb_size_t itor = tb_find_all_if(fwatcher->watchitems, tb_fwatcher_item_eq, watchdir);
    if (itor != tb_iterator_tail(fwatcher->watchitems))
        return tb_true;

    // add watch
    tb_int_t wd = inotify_add_watch(fwatcher->fd, watchdir, IN_MODIFY | IN_CREATE | IN_DELETE);
    tb_assert_and_check_return_val(wd >= 0, tb_false);

    // save wd -> watchitem
    tb_fwatcher_item_t watchitem;
    watchitem.wd = wd;
    watchitem.watchdir = tb_strdup(watchdir);
    watchitem.recursion = recursion;
    tb_hash_map_insert(fwatcher->watchitems, tb_i2p(wd), &watchitem);
    return tb_true;
}

static tb_long_t tb_fwatcher_add_watch_dirs(tb_char_t const* path, tb_file_info_t const* info, tb_cpointer_t priv)
{
    // check
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)priv;
    tb_assert_and_check_return_val(path && info && fwatcher, TB_DIRECTORY_WALK_CODE_END);

    // add watch directory
    if (info->type == TB_FILE_TYPE_DIRECTORY)
        tb_fwatcher_add_watch(fwatcher, path, tb_true);
    return TB_DIRECTORY_WALK_CODE_CONTINUE;
}

static tb_bool_t tb_fwatcher_rm_watch(tb_fwatcher_t* fwatcher, tb_char_t const* watchdir)
{
    // check
    tb_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->watchitems && watchdir, tb_false);

    // remove watchitem
    tb_remove_first_if(fwatcher->watchitems, tb_fwatcher_item_eq, watchdir);
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

        // init watchitems
        fwatcher->watchitems = tb_hash_map_init(0, tb_element_uint32(), tb_element_mem(sizeof(tb_fwatcher_item_t), tb_fwatcher_item_free, fwatcher));
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
        // exit poller
        if (fwatcher->poller)
            tb_poller_exit(fwatcher->poller);

        // exit watchitems
        if (fwatcher->watchitems)
        {
            tb_hash_map_exit(fwatcher->watchitems);
            fwatcher->watchitems = tb_null;
        }

        // exit fd
        if (fwatcher->fd >= 0)
        {
            close(fwatcher->fd);
            fwatcher->fd = -1;
        }

        // exit waited events
        if (fwatcher->waited_events)
        {
            tb_queue_exit(fwatcher->waited_events);
            fwatcher->waited_events = tb_null;
        }

        // exit watcher
        tb_free(fwatcher);
        fwatcher = tb_null;
    }
}

tb_bool_t tb_fwatcher_add(tb_fwatcher_ref_t self, tb_char_t const* watchdir, tb_bool_t recursion)
{
    // check
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->watchitems && watchdir, tb_false);

    // directory not found
    tb_file_info_t info;
    if (!tb_file_info(watchdir, &info) || info.type != TB_FILE_TYPE_DIRECTORY)
        return tb_false;

    // is directory? we need scan it and add all subdirs
    if (info.type == TB_FILE_TYPE_DIRECTORY && recursion)
        tb_directory_walk(watchdir, 0, tb_true, tb_fwatcher_add_watch_dirs, fwatcher);
    return tb_fwatcher_add_watch(fwatcher, watchdir, recursion);
}

tb_bool_t tb_fwatcher_remove(tb_fwatcher_ref_t self, tb_char_t const* watchdir)
{
    // check
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && watchdir, tb_false);

    // is directory? we need scan it and remove all subdirs
    tb_file_info_t info;
    if (tb_file_info(watchdir, &info) && info.type == TB_FILE_TYPE_DIRECTORY)
    {
        tb_value_t values[1];
        values[0].ptr = (tb_pointer_t)fwatcher;
        tb_directory_walk(watchdir, 0, tb_false, tb_fwatcher_rm_watch_dirs, values);
    }
    return tb_fwatcher_rm_watch(fwatcher, watchdir);
}

tb_void_t tb_fwatcher_spak(tb_fwatcher_ref_t self)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    if (fwatcher && fwatcher->poller)
        tb_poller_spak(fwatcher->poller);
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* event, tb_long_t timeout)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->waited_events && event, -1);

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

    // wait events
    tb_long_t wait = tb_poller_wait(fwatcher->poller, tb_fwatcher_event, timeout);
    tb_assert_and_check_return_val(wait >= 0, -1);
    tb_check_return_val(wait > 0, 0);

    tb_int_t real = read(fwatcher->fd, fwatcher->buffer, sizeof(fwatcher->buffer));
    tb_check_return_val(real >= 0, -1);

    tb_int_t i = 0;
    while (i < real)
    {
        struct inotify_event* ievent = (struct inotify_event*)&fwatcher->buffer[i];
        i += TB_FWATCHER_EVENT_SIZE + ievent->len;

        // get event code
        tb_size_t event_code = 0;
        if (ievent->mask & IN_CREATE)
            event_code = TB_FWATCHER_EVENT_CREATE;
        else if (ievent->mask & IN_DELETE)
            event_code = TB_FWATCHER_EVENT_DELETE;
        else if (ievent->mask & IN_MODIFY)
            event_code = TB_FWATCHER_EVENT_MODIFY;

        // get watchitem
        tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)tb_hash_map_get(fwatcher->watchitems, tb_i2p(ievent->wd));
        tb_check_continue(watchitem);
        tb_assert_and_check_break(watchitem->watchdir);

        // add event
        if (event_code)
        {
            tb_fwatcher_event_t evt;
            if (ievent->len)
                tb_snprintf(evt.filepath, TB_PATH_MAXN, "%s/%s", watchitem->watchdir, ievent->name);
            else tb_strlcpy(evt.filepath, watchitem->watchdir, TB_PATH_MAXN);
            evt.event = event_code;
            tb_queue_put(fwatcher->waited_events, &evt);

            // rescan the watch directory
            tb_file_info_t info;
            if (watchitem->recursion &&
                tb_file_info(evt.filepath, &info) && info.type == TB_FILE_TYPE_DIRECTORY)
            {
                if (event_code == TB_FWATCHER_EVENT_MODIFY ||
                    event_code == TB_FWATCHER_EVENT_CREATE)
                    tb_fwatcher_add(self, evt.filepath, watchitem->recursion);
                else if (event_code == TB_FWATCHER_EVENT_DELETE)
                    tb_fwatcher_remove(self, evt.filepath);
            }
        }
    }

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
