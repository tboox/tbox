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
 * @file        fwatcher_fsevent.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../fwatcher.h"
#include "../file.h"
#include "../socket.h"
#include "../directory.h"
#include "../spinlock.h"
#include "../semaphore.h"
#include "../../libc/libc.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
#   include "../../coroutine/coroutine.h"
#   include "../../coroutine/impl/impl.h"
#endif
#include <CoreServices/CoreServices.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the watch item type
typedef struct __tb_fwatcher_item_t
{
    FSEventStreamContext    context;
    FSEventStreamRef        stream;
    dispatch_queue_t        fsevents_queue;
    tb_char_t const*        watchdir;
    tb_bool_t               recursion;
    tb_fwatcher_ref_t       fwatcher;

}tb_fwatcher_item_t;

// the fwatcher type
typedef struct __tb_fwatcher_t
{
    tb_hash_map_ref_t       watchitems;
    tb_semaphore_ref_t      semaphore;
    tb_queue_ref_t          events_queue;
    tb_spinlock_t           lock;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static tb_void_t tb_fwatcher_item_free(tb_element_ref_t element, tb_pointer_t buff)
{
    tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)buff;
    if (watchitem)
    {
        // exit stream
        if (watchitem->stream)
        {
            FSEventStreamStop(watchitem->stream);
            FSEventStreamInvalidate(watchitem->stream);
            FSEventStreamRelease(watchitem->stream);
            watchitem->stream = tb_null;
        }

        // exit dispatch queue
        if (watchitem->fsevents_queue)
            dispatch_release(watchitem->fsevents_queue);
        watchitem->fsevents_queue = tb_null;

        // reset watchdir
        watchitem->watchdir = tb_null;
    }
}

static tb_void_t tb_fwatcher_item_callback(ConstFSEventStreamRef stream, tb_pointer_t priv,
    size_t events_count, tb_pointer_t event_paths, const FSEventStreamEventFlags event_flags[], FSEventStreamEventId const* event_id)
{
    // check
    tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)priv;
    tb_assert_and_check_return(watchitem && watchitem->stream && watchitem->watchdir);

    // get fwatcher
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)watchitem->fwatcher;
    tb_assert_and_check_return(fwatcher && fwatcher->semaphore && fwatcher->events_queue);

    // get events
    size_t i;
    tb_bool_t has_events = tb_false;
    for (i = 0; i < events_count; ++i)
    {
        // get event
#if defined(HAVE_MACOS_GE_10_13)
        CFDictionaryRef path_info_dict = (CFDictionaryRef)CFArrayGetValueAtIndex((CFArrayRef)event_paths, i);
        CFStringRef path = (CFStringRef)CFDictionaryGetValue(path_info_dict, kFSEventStreamEventExtendedDataPathKey);
        CFNumberRef cf_inode = (CFNumberRef)CFDictionaryGetValue(path_info_dict, kFSEventStreamEventExtendedFileIDKey);
        unsigned long inode;
        CFNumberGetValue(cf_inode, kCFNumberLongType, &inode);
        tb_char_t const* filepath = CFStringGetCStringPtr(path, kCFStringEncodingUTF8);
#else
        tb_char_t const* filepath = ((tb_char_t const**)event_paths)[i];
#endif

        // get file path
        FSEventStreamEventFlags flags = event_flags[i];
        tb_fwatcher_event_t event;
        if (filepath) tb_strlcpy(event.filepath, filepath, TB_PATH_MAXN);
        else event.filepath[0] = '\0';

        // we need filter file path if not recursion
        tb_bool_t matched = tb_false;
        if (watchitem->recursion)
            matched = tb_true;
        else
        {
            tb_char_t const* p = tb_strrchr(event.filepath, '/');
            if (p && p - event.filepath == tb_strlen(watchitem->watchdir))
                matched = tb_true;
        }
        tb_check_continue(matched);

        // filter need events
        if (flags & kFSEventStreamEventFlagItemCreated)
            event.event = TB_FWATCHER_EVENT_CREATE;
        else if (flags & kFSEventStreamEventFlagItemRemoved)
            event.event = TB_FWATCHER_EVENT_DELETE;
        else if (flags & kFSEventStreamEventFlagItemModified)
            event.event = TB_FWATCHER_EVENT_MODIFY;
        else if (flags & kFSEventStreamEventFlagItemRenamed)
        {
            // the parent directory is changed
            event.event = TB_FWATCHER_EVENT_MODIFY;
            tb_path_directory(filepath, event.filepath, TB_PATH_MAXN);
        }

        // add event to queue
        if (event.event)
        {
            tb_spinlock_enter(&fwatcher->lock);
            if (!tb_queue_full(fwatcher->events_queue))
                tb_queue_put(fwatcher->events_queue, &event);
            tb_spinlock_leave(&fwatcher->lock);
            has_events = tb_true;
        }
    }

    // notify events
    if (has_events) tb_semaphore_post(fwatcher->semaphore, 1);
}

static tb_bool_t tb_fwatcher_item_init(tb_fwatcher_t* fwatcher, tb_char_t const* watchdir, tb_fwatcher_item_t* watchitem)
{
    // check
    tb_assert_and_check_return_val(fwatcher && watchdir && watchitem, tb_false);

    // get path array
    CFStringRef pathstr = CFStringCreateWithCString(tb_null, watchdir, kCFStringEncodingUTF8);
    CFArrayRef path_array = CFArrayCreate(tb_null, (tb_cpointer_t*)&pathstr, 1, &kCFTypeArrayCallBacks);

    // init context
    FSEventStreamContext* context = &watchitem->context;
    context->version = 0;
    context->info = watchitem;
    context->retain = tb_null;
    context->release = tb_null;
    context->copyDescription = tb_null;

    // attach watchdir
    watchitem->fwatcher = (tb_fwatcher_ref_t)fwatcher;
    watchitem->watchdir = watchdir;

    // create fsevent stream
    FSEventStreamCreateFlags flags = kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagNoDefer;
#if defined (HAVE_MACOS_GE_10_13)
    flags |= kFSEventStreamCreateFlagUseExtendedData;
    flags |= kFSEventStreamCreateFlagUseCFTypes;
#endif
    watchitem->stream = FSEventStreamCreate(tb_null, tb_fwatcher_item_callback, context,
        path_array, kFSEventStreamEventIdSinceNow, 0, flags);

    // creating dispatch queue
    watchitem->fsevents_queue = dispatch_queue_create("fswatch_event_queue", tb_null);
    FSEventStreamSetDispatchQueue(watchitem->stream, watchitem->fsevents_queue);

    // start stream
    FSEventStreamStart(watchitem->stream);

    // free path array
    CFRelease(pathstr);
    CFRelease(path_array);
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

        // init watch items
        fwatcher->watchitems = tb_hash_map_init(0, tb_element_str(tb_true), tb_element_mem(sizeof(tb_fwatcher_item_t), tb_fwatcher_item_free, tb_null));
        tb_assert_and_check_break(fwatcher->watchitems);

        // init events queue
        fwatcher->events_queue = tb_queue_init(0, tb_element_mem(sizeof(tb_fwatcher_event_t), tb_null, tb_null));
        tb_assert_and_check_break(fwatcher->events_queue);

        // init semaphore
        fwatcher->semaphore = tb_semaphore_init(0);
        tb_assert_and_check_break(fwatcher->semaphore);

        // init lock
        tb_spinlock_init(&fwatcher->lock);

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
        // exit watchitems
        if (fwatcher->watchitems) tb_hash_map_exit(fwatcher->watchitems);
        fwatcher->watchitems = tb_null;

        // exit semaphore
        if (fwatcher->semaphore) tb_semaphore_exit(fwatcher->semaphore);
        fwatcher->semaphore = tb_null;

        // exit events queue
        if (fwatcher->events_queue) tb_queue_exit(fwatcher->events_queue);
        fwatcher->events_queue = tb_null;

        // exit lock
        tb_spinlock_exit(&fwatcher->lock);

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

    // get real path, we need match file path from event callback
    tb_char_t data[PATH_MAX];
    tb_char_t const* watchdir_real = realpath(watchdir, data);
    if (!watchdir_real) watchdir_real = watchdir;

    // this path has been added?
    tb_size_t itor = tb_hash_map_find(fwatcher->watchitems, watchdir_real);
    if (itor != tb_iterator_tail(fwatcher->watchitems))
        return tb_true;

    // save watch item
    tb_fwatcher_item_t watchitem;
    tb_memset(&watchitem, 0, sizeof(tb_fwatcher_item_t));
    watchitem.recursion = recursion;
    return tb_hash_map_insert(fwatcher->watchitems, watchdir_real, &watchitem) != tb_iterator_tail(fwatcher->watchitems);
}

tb_bool_t tb_fwatcher_remove(tb_fwatcher_ref_t self, tb_char_t const* watchdir)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->watchitems && watchdir, tb_false);

    // get real path, we need match file path from event callback
    tb_char_t data[PATH_MAX];
    tb_char_t const* watchdir_real = realpath(watchdir, data);
    if (!watchdir_real) watchdir_real = watchdir;

    // remove the watchitem
    tb_hash_map_remove(fwatcher->watchitems, watchdir_real);
    return tb_true;
}

tb_void_t tb_fwatcher_spak(tb_fwatcher_ref_t self)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return(fwatcher && fwatcher->semaphore);

    tb_semaphore_post(fwatcher->semaphore, 1);
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* event, tb_long_t timeout)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && fwatcher->semaphore && fwatcher->events_queue && event, -1);

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
    tb_spinlock_enter(&fwatcher->lock);
    if (!tb_queue_null(fwatcher->events_queue))
    {
        tb_fwatcher_event_t* e = (tb_fwatcher_event_t*)tb_queue_get(fwatcher->events_queue);
        if (e)
        {
            *event = *e;
            tb_queue_pop(fwatcher->events_queue);
            has_events = tb_true;
        }
    }
    tb_spinlock_leave(&fwatcher->lock);
    tb_check_return_val(!has_events, 1);

    // init watch items
    tb_for_all(tb_hash_map_item_ref_t, item, fwatcher->watchitems)
    {
        // get watch item and path
        tb_char_t const* watchdir = (tb_char_t const*)item->name;
        tb_fwatcher_item_t* watchitem = (tb_fwatcher_item_t*)item->data;
        tb_assert_and_check_return_val(watchitem && watchdir, -1);

        // init watch item first
        if (!watchitem->stream && !tb_fwatcher_item_init(fwatcher, watchdir, watchitem))
        {
            tb_trace_d("watch %s failed", watchdir);
            return -1;
        }
    }

    // wait events
    tb_long_t wait = tb_semaphore_wait(fwatcher->semaphore, timeout);
    tb_assert_and_check_return_val(wait >= 0, -1);
    tb_check_return_val(wait > 0, 0);

    // get event
    tb_spinlock_enter(&fwatcher->lock);
    if (!tb_queue_null(fwatcher->events_queue))
    {
        tb_fwatcher_event_t* e = (tb_fwatcher_event_t*)tb_queue_get(fwatcher->events_queue);
        if (e)
        {
            *event = *e;
            tb_queue_pop(fwatcher->events_queue);
            has_events = tb_true;
        }
    }
    tb_spinlock_leave(&fwatcher->lock);
    return has_events? 1 : 0;
}
