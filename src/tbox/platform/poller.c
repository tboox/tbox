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
 * @file        poller.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "poller"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "poller.h"
#include "time.h"
#include "impl/poller.h"
#include "impl/pollerdata.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   ifndef TB_CONFIG_MICRO_ENABLE
#       include "windows/poller_iocp.c"
#       define TB_POLLER_ENABLE_IOCP
#   else
#       include "posix/poller_select.c"
#       define TB_POLLER_ENABLE_SELECT
#   endif
#elif defined(TB_CONFIG_POSIX_HAVE_EPOLL_CREATE) \
    && defined(TB_CONFIG_POSIX_HAVE_EPOLL_WAIT)
#   include "linux/poller_epoll.c"
#   define TB_POLLER_ENABLE_EPOLL
#elif defined(TB_CONFIG_OS_MACOSX) || defined(TB_CONFIG_OS_BSD)
#   include "bsd/poller_kqueue.c"
#   define TB_POLLER_ENABLE_KQUEUE
#elif defined(TB_CONFIG_POSIX_HAVE_POLL) && !defined(TB_CONFIG_MICRO_ENABLE) /* TODO remove vector for supporting the micro mode */
#   include "posix/poller_poll.c"
#   define TB_POLLER_ENABLE_POLL
#elif defined(TB_CONFIG_POSIX_HAVE_SELECT)
#   include "posix/poller_select.c"
#   define TB_POLLER_ENABLE_SELECT
#endif

#ifndef TB_CONFIG_MICRO_ENABLE
#   if defined(TB_CONFIG_OS_WINDOWS)
#       include "windows/poller_process.c"
#       define TB_POLLER_ENABLE_PROCESS
#   elif defined(TB_CONFIG_POSIX_HAVE_WAITPID) && defined(TB_CONFIG_LIBC_HAVE_SIGNAL)
#       include "posix/poller_process.c"
#       define TB_POLLER_ENABLE_PROCESS
#   endif
#endif

#ifndef TB_CONFIG_MICRO_ENABLE
#   if defined(TB_CONFIG_OS_WINDOWS) || \
        defined(TB_CONFIG_LINUX_HAVE_INOTIFY_INIT) || \
        defined(TB_CONFIG_OS_MACOSX) || \
        defined(TB_CONFIG_OS_BSD)
#       include "impl/poller_fwatcher.c"
#       define TB_POLLER_ENABLE_FWATCHER
#   endif
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_poller_ref_t tb_poller_init(tb_cpointer_t priv)
{
    tb_bool_t       ok = tb_false;
    tb_poller_t*    poller = tb_null;
    do
    {
        // init poller
#if defined(TB_POLLER_ENABLE_EPOLL)
        poller = tb_poller_epoll_init();
#elif defined(TB_POLLER_ENABLE_KQUEUE)
        poller = tb_poller_kqueue_init();
#elif defined(TB_POLLER_ENABLE_IOCP)
        poller = tb_poller_iocp_init();
#elif defined(TB_POLLER_ENABLE_POLL)
        poller = tb_poller_poll_init();
#elif defined(TB_POLLER_ENABLE_SELECT)
        poller = tb_poller_select_init();
#endif
        tb_assert_and_check_break(poller);

        // save the user private data
        poller->priv = priv;

        // ok
        ok = tb_true;

    } while (0);

    // failed? exit the poller
    if (!ok)
    {
        if (poller) tb_poller_exit((tb_poller_ref_t)poller);
        poller = tb_null;
    }
    return (tb_poller_ref_t)poller;
}
tb_void_t tb_poller_exit(tb_poller_ref_t self)
{
    // check
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return(poller);

    // kill the poller first
    tb_poller_kill(self);

#ifdef TB_POLLER_ENABLE_PROCESS
    // exit the process poller
    if (poller->process_poller) tb_poller_process_exit(poller->process_poller);
    poller->process_poller = tb_null;
#endif

#ifdef TB_POLLER_ENABLE_FWATCHER
    // exit the fwatcher poller
    if (poller->fwatcher_poller) tb_poller_fwatcher_exit(poller->fwatcher_poller);
    poller->fwatcher_poller = tb_null;
#endif

    // exit poller
    if (poller->exit)
        poller->exit(poller);
}
tb_cpointer_t tb_poller_priv(tb_poller_ref_t self)
{
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return_val(poller, tb_null);

    return poller->priv;
}
tb_size_t tb_poller_type(tb_poller_ref_t self)
{
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return_val(poller, TB_POLLER_TYPE_NONE);

    return poller->type;
}
tb_void_t tb_poller_kill(tb_poller_ref_t self)
{
    // check
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return(poller);

#ifdef TB_POLLER_ENABLE_PROCESS
    // kill the process poller
    if (poller->process_poller) tb_poller_process_kill(poller->process_poller);
#endif

#ifdef TB_POLLER_ENABLE_FWATCHER
    // kill the fwatcher poller
    if (poller->fwatcher_poller) tb_poller_fwatcher_kill(poller->fwatcher_poller);
#endif

    // kill the poller
    if (poller->kill) poller->kill(poller);
}
tb_void_t tb_poller_spak(tb_poller_ref_t self)
{
    // check
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return(poller);

#ifdef TB_POLLER_ENABLE_PROCESS
    // spank the process poller
    if (poller->process_poller) tb_poller_process_spak(poller->process_poller);
#endif

#ifdef TB_POLLER_ENABLE_FWATCHER
    // spank the fwatcher poller
    if (poller->fwatcher_poller) tb_poller_fwatcher_spak(poller->fwatcher_poller);
#endif

    // spank the poller
    if (poller->spak) poller->spak(poller);
}
tb_bool_t tb_poller_support(tb_poller_ref_t self, tb_size_t events)
{
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return_val(poller, tb_false);

    return (poller->supported_events & events) == events;
}
tb_bool_t tb_poller_insert(tb_poller_ref_t self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return_val(poller && poller->insert && object, tb_false);

#ifdef TB_POLLER_ENABLE_PROCESS
    // is the process object?
    if (object->type == TB_POLLER_OBJECT_PROC)
    {
        // init the process poller first
        if (!poller->process_poller) poller->process_poller = tb_poller_process_init(poller);
        tb_assert_and_check_return_val(poller->process_poller, tb_false);

        // insert this process and the user private data
        return tb_poller_process_insert(poller->process_poller, object->ref.proc, priv);
    }
#else
    tb_assert_and_check_return_val(object->type != TB_POLLER_OBJECT_PROC, tb_false);
#endif

#ifdef TB_POLLER_ENABLE_FWATCHER
    // is the fwatcher object?
    if (object->type == TB_POLLER_OBJECT_FWATCHER)
    {
        // init the fwatcher poller first
        if (!poller->fwatcher_poller) poller->fwatcher_poller = tb_poller_fwatcher_init(poller);
        tb_assert_and_check_return_val(poller->fwatcher_poller, tb_false);

        // insert this fwatcher and the user private data
        return tb_poller_fwatcher_insert(poller->fwatcher_poller, object->ref.fwatcher, priv);
    }
#else
    tb_assert_and_check_return_val(object->type != TB_POLLER_OBJECT_FWATCHER, tb_false);
#endif

    // insert the poller object
    return poller->insert(poller, object, events, priv);
}
tb_bool_t tb_poller_remove(tb_poller_ref_t self, tb_poller_object_ref_t object)
{
    // check
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return_val(poller && poller->remove && object, tb_false);

#ifdef TB_POLLER_ENABLE_PROCESS
    // is the process object?
    if (object->type == TB_POLLER_OBJECT_PROC)
    {
        // remove this process and the user private data
        if (poller->process_poller)
            return tb_poller_process_remove(poller->process_poller, object->ref.proc);
        return tb_true;
    }
#else
    tb_assert_and_check_return_val(object->type != TB_POLLER_OBJECT_PROC, tb_false);
#endif

#ifdef TB_POLLER_ENABLE_FWATCHER
    // is the fwatcher object?
    if (object->type == TB_POLLER_OBJECT_FWATCHER)
    {
        // remove this fwatcher and the user private data
        if (poller->fwatcher_poller)
            return tb_poller_fwatcher_remove(poller->fwatcher_poller, object->ref.fwatcher);
        return tb_true;
    }
#else
    tb_assert_and_check_return_val(object->type != TB_POLLER_OBJECT_FWATCHER, tb_false);
#endif

    // remove the poller object
    return poller->remove(poller, object);
}
tb_bool_t tb_poller_modify(tb_poller_ref_t self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return_val(poller && poller->modify && object, tb_false);

#ifdef TB_POLLER_ENABLE_PROCESS
    // is the process object?
    if (object->type == TB_POLLER_OBJECT_PROC)
    {
        // modify the user private data of this process
        if (poller->process_poller)
            return tb_poller_process_modify(poller->process_poller, object->ref.proc, priv);
        return tb_true;
    }
#else
    tb_assert_and_check_return_val(object->type != TB_POLLER_OBJECT_PROC, tb_false);
#endif

#ifdef TB_POLLER_ENABLE_FWATCHER
    // is the fwatcher object?
    if (object->type == TB_POLLER_OBJECT_FWATCHER)
    {
        // modify the user private data of this fwatcher
        if (poller->fwatcher_poller)
            return tb_poller_fwatcher_modify(poller->fwatcher_poller, object->ref.fwatcher, priv);
        return tb_true;
    }
#else
    tb_assert_and_check_return_val(object->type != TB_POLLER_OBJECT_FWATCHER, tb_false);
#endif

    // modify the poller object
    return poller->modify(poller, object, events, priv);
}
tb_long_t tb_poller_wait(tb_poller_ref_t self, tb_poller_event_func_t func, tb_long_t timeout)
{
    // check
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return_val(poller && poller->wait && func, -1);

#ifdef TB_POLLER_ENABLE_PROCESS
    // prepare to wait the processes
    if (poller->process_poller)
    {
        // prepare to wait processes
        if (!tb_poller_process_wait_prepare(poller->process_poller))
            return -1;
    }
#endif

#ifdef TB_POLLER_ENABLE_FWATCHER
    // prepare to wait the fwatchers
    if (poller->fwatcher_poller)
    {
        // prepare to wait fwatchers
        if (!tb_poller_fwatcher_wait_prepare(poller->fwatcher_poller))
            return -1;
    }
#endif

    // wait the poller objects
    tb_long_t wait = poller->wait(poller, func, timeout);
    tb_check_return_val(wait >= 0, -1);

#ifdef TB_POLLER_ENABLE_PROCESS
    // poll all waited processes
    if (poller->process_poller)
    {
        tb_long_t proc_wait = tb_poller_process_wait_poll(poller->process_poller, func);
        tb_check_return_val(proc_wait >= 0, -1);
        wait += proc_wait;
    }
#endif

#ifdef TB_POLLER_ENABLE_FWATCHER
    // poll all waited fwatchers
    if (poller->fwatcher_poller)
    {
        tb_long_t fwatcher_wait = tb_poller_fwatcher_wait_poll(poller->fwatcher_poller, func);
        tb_check_return_val(fwatcher_wait >= 0, -1);
        wait += fwatcher_wait;
    }
#endif
    return wait;
}
tb_void_t tb_poller_attach(tb_poller_ref_t self)
{
    // check
    tb_poller_t* poller = (tb_poller_t*)self;
    tb_assert_and_check_return(poller);

    // attach the poller to the current thread (only for windows/iocp now)
    if (poller->attach) poller->attach(poller);
}
