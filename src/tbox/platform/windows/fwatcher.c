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
#include "../file.h"
#include "../event.h"
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
    HANDLE              event;

}tb_fwatcher_item_t;

// the fwatcher type
typedef struct __tb_fwatcher_t
{
    tb_event_ref_t       event;
    tb_hash_map_ref_t    watchitems;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_fwatcher_item_free(tb_element_ref_t element, tb_pointer_t buff)
{
    tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)buff;
    tb_assert_and_check_return(watchitem);

    if (watchitem->event != INVALID_HANDLE_VALUE)
    {
        CloseHandle(watchitem->event);
        watchitem->event = INVALID_HANDLE_VALUE;
    }
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

        // init event
        fwatcher->event = tb_event_init();
        tb_assert_and_check_break(fwatcher->event);

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

        // exit event
        if (fwatcher->event) tb_event_exit(fwatcher->event);
        fwatcher->event = tb_null;

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

    // create event
    HANDLE event = CreateEvent(tb_null, TRUE, FALSE, tb_null);
    tb_assert_and_check_return_val(event, tb_false);

    // save watch item
    tb_fwatcher_item_t watchitem;
    watchitem.event = event;
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
    tb_assert_and_check_return(fwatcher && fwatcher->event);

    tb_event_post(fwatcher->event);
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* events, tb_size_t events_maxn, tb_long_t timeout)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->event && events && events_maxn, -1);

    tb_long_t events_count = 0;
    tb_bool_t stop = tb_false;
    tb_hong_t time = tb_mclock();
    while (!events_count && !stop && (timeout < 0 || tb_mclock() < time + timeout))
    {
        // wait some time
        tb_long_t wait = tb_event_wait(fwatcher->event, 500);
        tb_assert_and_check_return_val(wait >= 0, -1);

        // has spark event? we need break the current loop
        if (wait > 0) break;

        // TODO
    }
    return events_count;
}
