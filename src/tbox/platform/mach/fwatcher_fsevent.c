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
#include "../semaphore.h"
#include "../../libc/libc.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#include <CoreServices/CoreServices.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the watch item type
typedef struct __tb_fwatcher_item_t
{
    tb_size_t           events;

}tb_fwatcher_item_t;


// the fwatcher type
typedef struct __tb_fwatcher_t
{
    FSEventStreamContext    context;
    FSEventStreamRef        stream;
    dispatch_queue_t        fsevents_queue;
    tb_hash_map_ref_t       watchitems;
    tb_semaphore_ref_t      semaphore;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_fwatcher_fsevent_stream_callback(ConstFSEventStreamRef stream, tb_pointer_t priv,
    size_t events_count, tb_pointer_t event_paths, const FSEventStreamEventFlags event_flags[], FSEventStreamEventId const* event_id)
{
    // check
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)priv;
    tb_assert_and_check_return(fwatcher && fwatcher->semaphore);

    // get events
    size_t i;
    for (i = 0; i < events_count; ++i)
    {
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
        tb_trace_i("tb_fwatcher_fsevent_stream_callback: %s, %x", filepath, event_flags[i]);
    }

    // notify events
    if (events_count) tb_semaphore_post(fwatcher->semaphore, 1);
}

static tb_bool_t tb_fwatcher_fsevent_stream_init(tb_fwatcher_t* fwatcher)
{
    // check
    tb_assert_and_check_return_val(fwatcher && fwatcher->watchitems, tb_false);

    // get items count
    tb_size_t itemcount = tb_hash_map_size(fwatcher->watchitems);
    tb_assert_and_check_return_val(itemcount, tb_false);

    // get path array
    CFStringRef* pathstrs = tb_nalloc_type(itemcount, CFStringRef);
    tb_assert_and_check_return_val(pathstrs, tb_false);

    tb_size_t i = 0;
    tb_for_all (tb_hash_map_item_ref_t, item, fwatcher->watchitems)
    {
        pathstrs[i++] = CFStringCreateWithCString(tb_null, item->name, kCFStringEncodingUTF8);
    }
    CFArrayRef path_array = CFArrayCreate(tb_null, (tb_cpointer_t*)pathstrs, itemcount, &kCFTypeArrayCallBacks);

    // init context
    FSEventStreamContext* context = &fwatcher->context;
    context->version = 0;
    context->info = fwatcher;
    context->retain = tb_null;
    context->release = tb_null;
    context->copyDescription = tb_null;

    // create fsevent stream
    FSEventStreamCreateFlags flags = kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagNoDefer;
#if defined (HAVE_MACOS_GE_10_13)
    flags |= kFSEventStreamCreateFlagUseExtendedData;
    flags |= kFSEventStreamCreateFlagUseCFTypes;
#endif
    fwatcher->stream = FSEventStreamCreate(tb_null, tb_fwatcher_fsevent_stream_callback, context,
        path_array, kFSEventStreamEventIdSinceNow, 0, flags);

    // creating dispatch queue
    fwatcher->fsevents_queue = dispatch_queue_create("fswatch_event_queue", tb_null);
    FSEventStreamSetDispatchQueue(fwatcher->stream, fwatcher->fsevents_queue);

    // start stream
    FSEventStreamStart(fwatcher->stream);

    // free path array
    for (i = 0; i < itemcount; i++)
    {
        if (pathstrs[i])
            CFRelease(pathstrs[i]);
        pathstrs[i] = tb_null;
    }
    tb_free(pathstrs);
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
        fwatcher->watchitems = tb_hash_map_init(0, tb_element_str(tb_true), tb_element_mem(sizeof(tb_fwatcher_item_t), tb_null, tb_null));
        tb_assert_and_check_break(fwatcher->watchitems);

        // init semaphore
        fwatcher->semaphore = tb_semaphore_init(0);
        tb_assert_and_check_break(fwatcher->semaphore);

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
        // exit stream
        if (fwatcher->stream)
        {
            FSEventStreamStop(fwatcher->stream);
            FSEventStreamInvalidate(fwatcher->stream);
            FSEventStreamRelease(fwatcher->stream);
            fwatcher->stream = tb_null;
        }

        // exit dispatch queue
        if (fwatcher->fsevents_queue)
            dispatch_release(fwatcher->fsevents_queue);
        fwatcher->fsevents_queue = tb_null;

        // exit watchitems
        if (fwatcher->watchitems) tb_hash_map_exit(fwatcher->watchitems);
        fwatcher->watchitems = tb_null;

        // exit stream
        if (fwatcher->stream) CFRelease(fwatcher->stream);
        fwatcher->stream = tb_null;

        // exit semaphore
        if (fwatcher->semaphore) tb_semaphore_exit(fwatcher->semaphore);
        fwatcher->semaphore = tb_null;

        // wait watcher
        tb_free(fwatcher);
        fwatcher = tb_null;
    }
}

tb_bool_t tb_fwatcher_add(tb_fwatcher_ref_t self, tb_char_t const* filepath, tb_size_t events)
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
    watchitem.events = events;
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
    tb_assert_and_check_return(fwatcher && fwatcher->semaphore);

    tb_semaphore_post(fwatcher->semaphore, 1);
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* events, tb_size_t events_maxn, tb_long_t timeout)
{
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    tb_assert_and_check_return_val(fwatcher && events && events_maxn, -1);

    // we need init fsevent stream first
    if (!fwatcher->stream && !tb_fwatcher_fsevent_stream_init(fwatcher))
        return -1;

    // wait events
    tb_long_t wait = tb_semaphore_wait(fwatcher->semaphore, timeout);
    tb_assert_and_check_return_val(wait >= 0, -1);
    tb_check_return_val(wait > 0, 0);

    return 0;
}
