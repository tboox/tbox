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
#include "../semaphore.h"
#include "../spinlock.h"
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

    // the semaphore
    tb_semaphore_ref_t      semaphore;

    // is stopped?
    tb_atomic32_t           is_stopped;

    // the lock
    tb_spinlock_t           lock;

    // the fwatcher
    tb_fwatcher_ref_t       fwatcher;

    // the userdata
    tb_cpointer_t           udata;

}tb_poller_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_int_t tb_poller_fwatcher_loop(tb_cpointer_t priv)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)priv;
    tb_assert_and_check_return_val(poller && poller->semaphore, -1);

    while (!tb_atomic32_get(&poller->is_stopped))
    {
        // TODO
        tb_fwatcher_event_t events[64];
        tb_fwatcher_wait(poller->fwatcher, events, tb_arrayn(events), -1);

        // has waited events? notify the main poller to poll them
        tb_bool_t has_events = tb_false;
        if (has_events)
        {
            tb_poller_t* main_poller = poller->main_poller;
            if (main_poller && main_poller->spak)
                main_poller->spak(main_poller);
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
        tb_semaphore_post(poller->semaphore, 1);
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

        // init semaphore
        poller->semaphore = tb_semaphore_init(0);
        tb_assert_and_check_break(poller->semaphore);

        // init lock
        tb_spinlock_init(&poller->lock);

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

    // trace
    tb_trace_d("fwatcher: spak ..");

    // post it
    tb_semaphore_post(poller->semaphore, 1);
}
static tb_bool_t tb_poller_fwatcher_insert(tb_poller_fwatcher_ref_t self, tb_fwatcher_ref_t fwatcher, tb_cpointer_t priv)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return_val(poller && fwatcher, tb_false);

    // attach fwatcher
    if (!poller->fwatcher)
    {
        poller->fwatcher = fwatcher;
        poller->udata = priv;
        return tb_true;
    }
    else if (poller->fwatcher == fwatcher)
    {
        poller->udata = priv;
        return tb_true;
    }
    // we can insert only one fwatcher
    return tb_false;
}
static tb_bool_t tb_poller_fwatcher_modify(tb_poller_fwatcher_ref_t self, tb_fwatcher_ref_t fwatcher, tb_cpointer_t priv)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return_val(poller && fwatcher, tb_false);

    poller->fwatcher = fwatcher;
    poller->udata = priv;
    return tb_true;
}
static tb_bool_t tb_poller_fwatcher_remove(tb_poller_fwatcher_ref_t self, tb_fwatcher_ref_t fwatcher)
{
    // check
    tb_poller_fwatcher_t* poller = (tb_poller_fwatcher_t*)self;
    tb_assert_and_check_return_val(poller && fwatcher, tb_false);

    if (poller->fwatcher == fwatcher)
    {
        poller->fwatcher = tb_null;
        poller->udata = tb_null;
        return tb_true;
    }
    return tb_false;
}
static tb_bool_t tb_poller_fwatcher_wait_prepare(tb_poller_fwatcher_ref_t self)
{
    return 0;
}
static tb_long_t tb_poller_fwatcher_wait_poll(tb_poller_fwatcher_ref_t self, tb_poller_event_func_t func)
{
    return -1;
}
