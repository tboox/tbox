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
 * @file        fwatcher.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../fwatcher.h"
#include "../../libc/libc.h"
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_SMALL
#   define TB_FWATCHER_ENTRIES_MAXN     64
#else
#   define TB_FWATCHER_ENTRIES_MAXN     256
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the fwatcher type
typedef struct __tb_fwatcher_t
{
    tb_int_t        kqfd;
    tb_int_t        entries[TB_FWATCHER_ENTRIES_MAXN];
    tb_size_t       entries_size;
    struct kevent   events_to_monitor[TB_FWATCHER_ENTRIES_MAXN];
    struct kevent*  events;
    tb_size_t       events_count;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_fwatcher_ref_t tb_fwatcher_init()
{
    tb_bool_t ok = tb_false;
    tb_fwatcher_t* fwatcher = tb_null;
    do
    {
        fwatcher = tb_malloc0_type(tb_fwatcher_t);
        tb_assert_and_check_break(fwatcher);

        fwatcher->kqfd = kqueue();
        tb_assert_and_check_break(fwatcher->kqfd >= 0);

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
        // exit events
        if (fwatcher->events) tb_free(fwatcher->events);
        fwatcher->events = tb_null;
        fwatcher->events_count = 0;

        // exit entries
        for (tb_size_t i = 0; i < fwatcher->entries_size; i++)
        {
            tb_int_t fd = fwatcher->entries[i];
            if (fd >= 0) close(fd);
        }
        fwatcher->entries_size = 0;

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

tb_bool_t tb_fwatcher_register(tb_fwatcher_ref_t self, tb_char_t const* dir, tb_size_t events)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->kqfd >= 0 && dir && events, tb_false);
    tb_assert_and_check_return_val(fwatcher->entries_size < tb_arrayn(fwatcher->entries), tb_false);

    tb_int_t o_flags = 0;
#  ifdef O_SYMLINK
    o_flags |= O_SYMLINK;
#  endif
#  ifdef O_EVTONLY
    // The descriptor is requested for event notifications only.
    o_flags |= O_EVTONLY;
#  else
    o_flags |= O_RDONLY;
#  endif
    tb_int_t wd = open(dir, o_flags);
    tb_assert_and_check_return_val(wd >= 0, tb_false);

    tb_size_t i = fwatcher->entries_size;
    tb_uint_t vnode_events = NOTE_DELETE | NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_RENAME | NOTE_REVOKE;
    EV_SET(&fwatcher->events_to_monitor[i], wd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, vnode_events, 0, (tb_pointer_t)dir);

    fwatcher->entries[i] = wd;
    fwatcher->entries_size++;
    return tb_true;
}

tb_void_t tb_fwatcher_spak(tb_fwatcher_ref_t self)
{
    tb_trace_noimpl();
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* events, tb_size_t events_maxn, tb_long_t timeout)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->kqfd >= 0 && fwatcher->entries_size && events && events_maxn, -1);

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

    // wait events
    tb_long_t events_count = kevent(fwatcher->kqfd, fwatcher->events_to_monitor, fwatcher->entries_size,
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
    tb_size_t          wait = 0;
    struct kevent*     event = tb_null;
    for (i = 0; i < events_count; i++)
    {
        // get event
        event = fwatcher->events + i;
        if (event->flags & EV_ERROR)
            continue;

        // get event code
        tb_size_t event_code = 0;
        if (event->fflags & NOTE_DELETE)
            event_code = TB_FWATCHER_EVENT_DELETE;
        else if ((event->fflags & NOTE_RENAME) || (event->fflags & NOTE_REVOKE) || (event->fflags & NOTE_WRITE))
            event_code = TB_FWATCHER_EVENT_MODIFY;

        // add event
        if (event_code)
        {
            tb_char_t const* filepath = (tb_char_t const*)event->udata;
            if (filepath) tb_strlcpy(events[wait].filepath, filepath, TB_PATH_MAXN);
            else events[wait].filepath[0] = '\0';
            events[wait].event = event_code;
            wait++;
        }
    }
    return wait;
}
