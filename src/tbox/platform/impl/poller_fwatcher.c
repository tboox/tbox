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
 * @file        poller_fwatcher.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../fwatcher.h"
#include "../thread.h"
#include "../atomic.h"
#include "../spinlock.h"
#include "../semaphore.h"
#include "../../algorithm/algorithm.h"
#include "../../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the poller fwatcher type
typedef struct __tb_poller_fwatcher_t
{
    // the main poller
    tb_poller_t*            main_poller;

    // the fwatcher poller thread
    tb_thread_ref_t         thread;

    // is stopped?
    tb_atomic32_t           is_stopped;

    // the lock
    tb_spinlock_t           lock;

    // the fwatcher
    tb_fwatcher_ref_t       fwatcher;

    // the user data
    tb_cpointer_t           udata;

    // the semaphore
    tb_semaphore_ref_t      semaphore;

    // the waiting event
    tb_fwatcher_event_t     waiting_event;

    // the waited events
    tb_vector_ref_t         waited_events;
    tb_vector_ref_t         waited_events_copied;

}tb_poller_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_int_t tb_poller_fwatcher_loop(tb_cpointer_t priv)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)priv;
    tb_assert_and_check_return_val(poller && poller->semaphore && poller->waited_events, -1);

    // do loop
    tb_fwatcher_ref_t fwatcher = tb_null;
    while (!tb_atomic32_get(&poller->is_stopped))
    {
        // get fwatcher
        tb_spinlock_enter(&poller->lock);
        fwatcher = poller->fwatcher;
        tb_spinlock_leave(&poller->lock);

        if (fwatcher)
        {
            // wait events
            tb_long_t wait = tb_fwatcher_wait(fwatcher, &poller->waiting_event, -1);
            tb_assert_and_check_break(wait >= 0);
            tb_check_continue(wait > 0);

            // save waited event
            tb_spinlock_enter(&poller->lock);
            tb_vector_insert_tail(poller->waited_events, &poller->waiting_event);
            tb_spinlock_leave(&poller->lock);

            // notify the main poller to poll them
            tb_poller_t* main_poller = poller->main_poller;
            if (main_poller && main_poller->spak)
                main_poller->spak(main_poller);
        }
        else
        {
            // wait semaphore
            tb_long_t wait = tb_semaphore_wait(poller->semaphore, -1);
            tb_assert_and_check_break(wait >= 0);

            // interrupted? continue to wait
            tb_check_continue(wait != 0);
        }
    }

    // mark this thread is stopped
    tb_atomic32_set(&poller->is_stopped, 1);
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_poller_fwatcher_kill(tb_poller_fwatcher_ref_t self)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return(poller && poller->semaphore);

    // trace
    tb_trace_d("fwatcher: kill ..");

    // stop thread and post it
    if (!tb_atomic32_fetch_and_set(&poller->is_stopped, 1))
    {
        if (poller->fwatcher)
            tb_fwatcher_spak(poller->fwatcher);
        tb_semaphore_post(poller->semaphore, 1);
    }
}
static tb_void_t tb_poller_fwatcher_exit(tb_poller_fwatcher_ref_t self)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return(poller);

    // kill the fwatcher poller first
    tb_poller_fwatcher_kill(self);

    // exit the fwatcher poller thread
    if (poller->thread)
    {
        // wait it
        tb_long_t wait = 0;
        if ((wait = tb_thread_wait(poller->thread, 5000, tb_null)) <= 0)
        {
            // trace
            tb_trace_e("wait fwatcher poller thread failed: %ld!", wait);
        }

        // exit it
        tb_thread_exit(poller->thread);
        poller->thread = tb_null;
    }

    // exit waited events
    if (poller->waited_events) tb_vector_exit(poller->waited_events);
    if (poller->waited_events_copied) tb_vector_exit(poller->waited_events_copied);
    poller->waited_events = tb_null;
    poller->waited_events_copied = tb_null;

    // exit semaphore
    if (poller->semaphore) tb_semaphore_exit(poller->semaphore);
    poller->semaphore = tb_null;

    // exit lock
    tb_spinlock_exit(&poller->lock);

    // reset fwatcher
    poller->fwatcher = tb_null;
    poller->udata = tb_null;

    // exit poller
    tb_free(poller);
}
static tb_poller_fwatcher_ref_t tb_poller_fwatcher_init(tb_poller_t* main_poller)
{
    tb_bool_t             ok = tb_false;
    tb_poller_fwatcher_t* poller = tb_null;
    do
    {
        // @note only support one fwatcher poller instance
        static tb_size_t s_poller_fwatcher_num = 0;
        if (s_poller_fwatcher_num++)
        {
            tb_trace_e("only support one fwatcher poller!");
            break;
        }

        // make the fwatcher poller
        poller = tb_malloc0_type(tb_poller_fwatcher_t);
        tb_assert_and_check_break(poller);

        // save the main poller
        poller->main_poller = main_poller;

        // init lock
        tb_spinlock_init(&poller->lock);

        // init semaphore
        poller->semaphore = tb_semaphore_init(0);
        tb_assert_and_check_break(poller->semaphore);

        // init waited events
        poller->waited_events = tb_vector_init(0, tb_element_mem(sizeof(tb_fwatcher_event_t), tb_null, tb_null));
        poller->waited_events_copied = tb_vector_init(0, tb_element_mem(sizeof(tb_fwatcher_event_t), tb_null, tb_null));
        tb_assert_and_check_break(poller->waited_events && poller->waited_events_copied);

        // start the poller thread for fwatchers first
        poller->thread = tb_thread_init(tb_null, tb_poller_fwatcher_loop, poller, 0);
        tb_assert_and_check_break(poller->thread);

        // ok
        ok = tb_true;

    } while (0);

    // failed? exit the poller
    if (!ok)
    {
        if (poller) tb_poller_fwatcher_exit((tb_poller_fwatcher_ref_t)poller);
        poller = tb_null;
    }
    return (tb_poller_fwatcher_ref_t)poller;
}
static tb_void_t tb_poller_fwatcher_spak(tb_poller_fwatcher_ref_t self)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return(poller && poller->semaphore);

    if (poller->fwatcher) tb_fwatcher_spak(poller->fwatcher);
    tb_semaphore_post(poller->semaphore, 1);
}
static tb_bool_t tb_poller_fwatcher_insert(tb_poller_fwatcher_ref_t self, tb_fwatcher_ref_t fwatcher, tb_cpointer_t priv)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return_val(poller && poller->semaphore && fwatcher, tb_false);

    // attach fwatcher
    tb_bool_t ok = tb_false;
    tb_bool_t notify = tb_false;
    tb_spinlock_enter(&poller->lock);
    tb_fwatcher_ref_t fwatcher_old = poller->fwatcher;
    if (!poller->fwatcher)
    {
        poller->fwatcher = fwatcher;
        poller->udata = priv;
        ok = tb_true;
        notify = tb_true;
    }
    else if (poller->fwatcher == fwatcher)
    {
        poller->udata = priv;
        ok = tb_true;
    }
    tb_spinlock_leave(&poller->lock);

    // notify thread to update fwatcher
    if (notify)
    {
        if (fwatcher_old) tb_fwatcher_spak(fwatcher_old);
        tb_semaphore_post(poller->semaphore, 1);
    }

    // we can insert only one fwatcher
    return ok;
}
static tb_bool_t tb_poller_fwatcher_modify(tb_poller_fwatcher_ref_t self, tb_fwatcher_ref_t fwatcher, tb_cpointer_t priv)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return_val(poller && poller->semaphore && fwatcher, tb_false);

    // update fwatcher
    tb_bool_t notify = tb_false;
    tb_spinlock_enter(&poller->lock);
    tb_fwatcher_ref_t fwatcher_old = poller->fwatcher;
    if (poller->fwatcher != fwatcher)
    {
        poller->fwatcher = fwatcher;
        notify = tb_true;
    }
    poller->udata = priv;
    tb_spinlock_leave(&poller->lock);

    // notify thread to update fwatcher
    if (notify)
    {
        if (fwatcher_old) tb_fwatcher_spak(fwatcher_old);
        tb_semaphore_post(poller->semaphore, 1);
    }
    return tb_true;
}
static tb_bool_t tb_poller_fwatcher_remove(tb_poller_fwatcher_ref_t self, tb_fwatcher_ref_t fwatcher)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return_val(poller && poller->semaphore && fwatcher, tb_false);

    // remove fwatcher
    tb_bool_t ok = tb_false;
    tb_bool_t notify = tb_false;
    tb_spinlock_enter(&poller->lock);
    tb_fwatcher_ref_t fwatcher_old = poller->fwatcher;
    if (poller->fwatcher == fwatcher)
    {
        poller->fwatcher = tb_null;
        poller->udata = tb_null;
        ok = tb_true;
        notify = tb_true;
    }
    tb_spinlock_leave(&poller->lock);

    // notify thread to update fwatcher
    if (notify)
    {
        if (fwatcher_old) tb_fwatcher_spak(fwatcher_old);
        tb_semaphore_post(poller->semaphore, 1);
    }
    return ok;
}
static tb_bool_t tb_poller_fwatcher_wait_prepare(tb_poller_fwatcher_ref_t self)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return_val(poller, tb_false);

    // is stopped?
    return !tb_atomic32_get(&poller->is_stopped) && poller->fwatcher;
}
static tb_long_t tb_poller_fwatcher_wait_poll(tb_poller_fwatcher_ref_t self, tb_poller_event_func_t func)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return_val(poller && func, -1);
    tb_assert_and_check_return_val(poller->waited_events && poller->waited_events_copied, -1);

    // get all waited events
    tb_vector_clear(poller->waited_events_copied);
    tb_spinlock_enter(&poller->lock);
    if (tb_vector_size(poller->waited_events))
    {
        tb_vector_copy(poller->waited_events_copied, poller->waited_events);
        tb_vector_clear(poller->waited_events);
    }
    tb_spinlock_leave(&poller->lock);

    // trace
    tb_trace_d("fwatcher: poll %lu", tb_vector_size(poller->waited_events_copied));

    // poll all waited events
    tb_long_t wait = 0;
    tb_poller_object_t object;
    object.type = TB_POLLER_OBJECT_FWATCHER;
    object.ref.fwatcher = poller->fwatcher;
    tb_for_all_if (tb_fwatcher_event_t*, event, poller->waited_events_copied, event)
    {
        func((tb_poller_ref_t)poller->main_poller, &object, (tb_long_t)event, poller->udata);
        wait++;
    }

    // trace
    tb_trace_d("fwatcher: poll wait %ld", wait);
    return wait;
}
