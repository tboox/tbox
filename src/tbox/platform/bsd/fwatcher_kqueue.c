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
 * @file        fwatcher_kqueue.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../fwatcher.h"
#include "../file.h"
#include "../socket.h"
#include "../directory.h"
#include "../../libc/libc.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
#   include "../../coroutine/coroutine.h"
#   include "../../coroutine/impl/impl.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifndef EV_ENABLE
#   define EV_ENABLE    (0)
#endif

#ifndef NOTE_EOF
#   define NOTE_EOF     (0)
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
    tb_int_t             kqfd;
    struct kevent*       watchevents;
    tb_size_t            watchevents_size;
    tb_size_t            watchevents_maxn;
    struct kevent*       events;
    tb_size_t            events_count;
    tb_hash_map_ref_t    watchitems;
    tb_socket_ref_t      pair[2];
    tb_queue_ref_t       waited_events;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_fwatcher_item_free(tb_element_ref_t element, tb_pointer_t buff)
{
    tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)buff;
    if (watchitem && watchitem->wd >= 0)
    {
        close(watchitem->wd);
        watchitem->wd = -1;
    }
}

static tb_bool_t tb_fwatcher_add_watch(tb_fwatcher_t* fwatcher, tb_char_t const* watchdir, tb_bool_t recursion)
{
    // check
    tb_assert_and_check_return_val(fwatcher && fwatcher->kqfd >= 0 && fwatcher->watchitems && watchdir, tb_false);

    // this path has been added?
    tb_size_t itor = tb_hash_map_find(fwatcher->watchitems, watchdir);
    if (itor != tb_iterator_tail(fwatcher->watchitems))
        return tb_true;

    // open watch fd
    tb_int_t o_flags = 0;
#  ifdef O_EVTONLY
    o_flags |= O_EVTONLY;
#  else
    o_flags |= O_RDONLY;
#  endif
    tb_int_t wd = open(watchdir, o_flags);
    tb_check_return_val(wd >= 0, tb_false);

    // save watch item
    tb_fwatcher_item_t watchitem;
    watchitem.wd = wd;
    watchitem.recursion = recursion;
    watchitem.watchdir = tb_null;
    return tb_hash_map_insert(fwatcher->watchitems, watchdir, &watchitem) != tb_iterator_tail(fwatcher->watchitems);
}

static tb_long_t tb_fwatcher_add_watch_filedirs(tb_char_t const* path, tb_file_info_t const* info, tb_cpointer_t priv)
{
    // check
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)priv;
    tb_assert_and_check_return_val(path && info && fwatcher, TB_DIRECTORY_WALK_CODE_END);

    // add file watch
    tb_fwatcher_add_watch(fwatcher, path, tb_true);
    return TB_DIRECTORY_WALK_CODE_CONTINUE;
}

static tb_bool_t tb_fwatcher_rm_watch(tb_fwatcher_t* fwatcher, tb_char_t const* watchdir)
{
    // check
    tb_assert_and_check_return_val(fwatcher && fwatcher->kqfd >= 0 && fwatcher->watchitems && watchdir, tb_false);

    // remove the watchitem
    tb_hash_map_remove(fwatcher->watchitems, watchdir);
    return tb_true;
}

static tb_long_t tb_fwatcher_rm_watch_filedirs(tb_char_t const* path, tb_file_info_t const* info, tb_cpointer_t priv)
{
    // check
    tb_value_t* values = (tb_value_t*)priv;
    tb_assert_and_check_return_val(path && info && values, TB_DIRECTORY_WALK_CODE_END);

    // get fwatcher
    tb_fwatcher_t* fwatcher = values[0].ptr;
    tb_assert_and_check_return_val(fwatcher, TB_DIRECTORY_WALK_CODE_END);

    // rm file watch
    tb_fwatcher_rm_watch(fwatcher, path);
    return TB_DIRECTORY_WALK_CODE_CONTINUE;
}

static tb_bool_t tb_fwatcher_update_watchevents(tb_iterator_ref_t iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)priv;
    tb_hash_map_item_ref_t hashitem = (tb_hash_map_item_ref_t)item;
    tb_assert_and_check_return_val(fwatcher && fwatcher->watchitems && hashitem, tb_false);

    // get watch item and path
    tb_char_t const* path = (tb_char_t const*)hashitem->name;
    tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)hashitem->data;
    tb_assert_and_check_return_val(watchitem->wd >= 0 && path, tb_false);

    // grow watchevents
    tb_size_t watchsize = tb_hash_map_size(fwatcher->watchitems);
    if (!fwatcher->watchevents)
    {
        fwatcher->watchevents_maxn = watchsize;
        fwatcher->watchevents = tb_nalloc_type(1 + fwatcher->watchevents_maxn, struct kevent);
    }
    else if (fwatcher->watchevents_size >= fwatcher->watchevents_maxn)
    {
        fwatcher->watchevents_maxn = watchsize + 64;
        fwatcher->watchevents = tb_ralloc(fwatcher->watchevents, (1 + fwatcher->watchevents_maxn) * sizeof(struct kevent));
    }
    tb_assert_and_check_return_val(fwatcher->watchevents && fwatcher->watchevents_size < fwatcher->watchevents_maxn, tb_false);

    // register pair1 to watchevents first
    if (!fwatcher->watchevents_size)
    {
        EV_SET(&fwatcher->watchevents[0], tb_sock2fd(fwatcher->pair[1]),
            EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_EOF, 0, tb_null);
    }

    // register watch events
    tb_uint_t vnode_events = NOTE_DELETE | NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_RENAME | NOTE_REVOKE;
    tb_assert_and_check_return_val(vnode_events, tb_false);

    watchitem->watchdir = path;
    EV_SET(&fwatcher->watchevents[1 + fwatcher->watchevents_size], watchitem->wd,
        EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, vnode_events, 0, (tb_pointer_t)watchitem);
    fwatcher->watchevents_size++;
    return tb_true;
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

        // init kqueue
        fwatcher->kqfd = kqueue();
        tb_assert_and_check_break(fwatcher->kqfd >= 0);

        // init socket pair
        if (!tb_socket_pair(TB_SOCKET_TYPE_TCP, fwatcher->pair)) break;

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
        // exit pair sockets
        if (fwatcher->pair[0]) tb_socket_exit(fwatcher->pair[0]);
        if (fwatcher->pair[1]) tb_socket_exit(fwatcher->pair[1]);
        fwatcher->pair[0] = tb_null;
        fwatcher->pair[1] = tb_null;

        // exit events
        if (fwatcher->events) tb_free(fwatcher->events);
        fwatcher->events = tb_null;
        fwatcher->events_count = 0;

        // exit watch events
        if (fwatcher->watchevents) tb_free(fwatcher->watchevents);
        fwatcher->watchevents = tb_null;
        fwatcher->watchevents_size = 0;
        fwatcher->watchevents_maxn = 0;

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

        // exit kqueue fd
        if (fwatcher->kqfd >= 0)
        {
            close(fwatcher->kqfd);
            fwatcher->kqfd = -1;
        }

        // wait watcher
        tb_free(fwatcher);
        fwatcher = tb_null;
    }
}

tb_bool_t tb_fwatcher_add(tb_fwatcher_ref_t self, tb_char_t const* watchdir, tb_bool_t recursion)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && watchdir, tb_false);

    // file not found
    tb_file_info_t info;
    if (!tb_file_info(watchdir, &info) || info.type != TB_FILE_TYPE_DIRECTORY)
        return tb_false;

    // is directory? we need scan it and add all subfiles
    if (info.type == TB_FILE_TYPE_DIRECTORY)
        tb_directory_walk(watchdir, recursion? -1 : 0, tb_true, tb_fwatcher_add_watch_filedirs, fwatcher);
    return tb_fwatcher_add_watch(fwatcher, watchdir, recursion);
}

tb_bool_t tb_fwatcher_remove(tb_fwatcher_ref_t self, tb_char_t const* watchdir)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && watchdir, tb_false);

    // is directory? we need scan it and remove all subfiles
    tb_file_info_t info;
    if (tb_file_info(watchdir, &info) && info.type == TB_FILE_TYPE_DIRECTORY)
    {
        tb_value_t values[1];
        values[0].ptr = (tb_pointer_t)fwatcher;
        tb_directory_walk(watchdir, 0, tb_false, tb_fwatcher_rm_watch_filedirs, values);
    }
    return tb_fwatcher_rm_watch(fwatcher, watchdir);
}

tb_void_t tb_fwatcher_spak(tb_fwatcher_ref_t self)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return(fwatcher);

    if (fwatcher->pair[0]) tb_socket_send(fwatcher->pair[0], (tb_byte_t const*)"p", 1);
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* event, tb_long_t timeout)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->kqfd >= 0 && fwatcher->waited_events && event, -1);

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

    // init time
    struct timespec t = {0};
    if (timeout > 0)
    {
        t.tv_sec = timeout / 1000;
        t.tv_nsec = (timeout % 1000) * 1000000;
    }

    // init events
    tb_size_t grow = 256;
    if (!fwatcher->events)
    {
        fwatcher->events_count = grow;
        fwatcher->events = tb_nalloc_type(fwatcher->events_count, struct kevent);
        tb_assert_and_check_return_val(fwatcher->events, -1);
    }

    // update watch events
    fwatcher->watchevents_size = 0;
    tb_walk_all(fwatcher->watchitems, tb_fwatcher_update_watchevents, fwatcher);
    tb_assert_and_check_return_val(fwatcher->watchevents && fwatcher->watchevents_size, -1);

    // wait events
    tb_long_t events_count = kevent(fwatcher->kqfd, fwatcher->watchevents, fwatcher->watchevents_size,
        fwatcher->events, fwatcher->events_count, timeout >= 0? &t : tb_null);

    // timeout or interrupted?
    if (!events_count || (events_count == -1 && errno == EINTR))
        return 0;

    // error?
    tb_assert_and_check_return_val(events_count >= 0 && events_count <= fwatcher->events_count, -1);

    // grow it if events is full
    if (events_count == fwatcher->events_count)
    {
        // grow size
        fwatcher->events_count += grow;

        // grow data
        fwatcher->events = (struct kevent*)tb_ralloc(fwatcher->events, fwatcher->events_count * sizeof(struct kevent));
        tb_assert_and_check_return_val(fwatcher->events, -1);
    }
    tb_assert(events_count <= fwatcher->events_count);

    // handle events
    tb_size_t          i = 0;
    tb_socket_ref_t    pair = fwatcher->pair[1];
    struct kevent*     kevt = tb_null;
    for (i = 0; i < events_count; i++)
    {
        // get event
        kevt = fwatcher->events + i;
        if (kevt->flags & EV_ERROR)
            continue;

        // spank socket events?
        tb_socket_ref_t sock = tb_fd2sock(kevt->ident);
        if (sock == pair && kevt->filter == EVFILT_READ)
        {
            tb_char_t spak = '\0';
            if (1 != tb_socket_recv(pair, (tb_byte_t*)&spak, 1)) return -1;
            continue ;
        }

        // get watchitem
        tb_fwatcher_item_t const* watchitem = (tb_fwatcher_item_t const*)kevt->udata;
        tb_assert_and_check_break(watchitem);

        // get event code
        tb_size_t event_code = 0;
        if (kevt->fflags & NOTE_DELETE)
            event_code = TB_FWATCHER_EVENT_DELETE;
        else if ((kevt->fflags & NOTE_RENAME) || (kevt->fflags & NOTE_REVOKE) || (kevt->fflags & NOTE_WRITE))
            event_code = TB_FWATCHER_EVENT_MODIFY;

        // add event
        if (event_code)
        {
            tb_fwatcher_event_t evt;
            if (watchitem->watchdir) tb_strlcpy(evt.filepath, watchitem->watchdir, TB_PATH_MAXN);
            else evt.filepath[0] = '\0';
            evt.event = event_code;
            tb_queue_put(fwatcher->waited_events, &evt);
        }

        // rescan the watch directory
        if (watchitem->watchdir)
        {
            tb_file_info_t info;
            if ((event_code == TB_FWATCHER_EVENT_MODIFY || event_code == TB_FWATCHER_EVENT_CREATE) &&
                tb_file_info(watchitem->watchdir, &info) && info.type == TB_FILE_TYPE_DIRECTORY)
                tb_fwatcher_add(self, watchitem->watchdir, watchitem->recursion);
            else if (event_code == TB_FWATCHER_EVENT_DELETE)
                tb_fwatcher_remove(self, watchitem->watchdir);
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
