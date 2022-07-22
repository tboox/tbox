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
 * @file        scheduler_io.c
 * @ingroup     coroutine
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "scheduler_io"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scheduler_io.h"
#include "coroutine.h"
#include "../../stackless/coroutine.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the ltimer grow
#ifdef __tb_small__
#   define TB_SCHEDULER_IO_LTIMER_GROW      (64)
#else
#   define TB_SCHEDULER_IO_LTIMER_GROW      (4096)
#endif

// the timer grow
#define TB_SCHEDULER_IO_TIMER_GROW          (TB_SCHEDULER_IO_LTIMER_GROW >> 4)

// the poller data grow
#ifdef __tb_small__
#   define TB_SCHEDULER_IO_POLLERDATA_GROW    (64)
#else
#   define TB_SCHEDULER_IO_POLLERDATA_GROW    (4096)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_lo_scheduler_io_resume(tb_lo_scheduler_t* scheduler, tb_lo_coroutine_t* coroutine, tb_size_t events)
{
#ifndef TB_CONFIG_MICRO_ENABLE
    // exists the timer task? remove it
    tb_cpointer_t task = coroutine->rs.wait.task;
    if (task)
    {
        // get io scheduler
        tb_lo_scheduler_io_ref_t scheduler_io = tb_lo_scheduler_io(scheduler);
        tb_assert(scheduler_io && scheduler_io->poller);

        // remove the timer task
        if (coroutine->rs.wait.is_ltimer) tb_ltimer_task_exit(scheduler_io->ltimer, (tb_ltimer_task_ref_t)task);
        else tb_timer_task_exit(scheduler_io->timer, (tb_timer_task_ref_t)task);
        coroutine->rs.wait.task = tb_null;
    }
#endif

    // return events
    coroutine->rs.wait.result = (tb_sint32_t)((events & TB_POLLER_EVENT_ERROR)? -1 : events);

    // resume the coroutine
    tb_lo_scheduler_resume(scheduler, coroutine);
}
#ifndef TB_CONFIG_MICRO_ENABLE
static tb_void_t tb_lo_scheduler_io_timeout(tb_bool_t killed, tb_cpointer_t priv)
{
    // check
    tb_lo_coroutine_t* coroutine = (tb_lo_coroutine_t*)priv;
    tb_assert(coroutine);

    // get scheduler
    tb_lo_scheduler_t* scheduler = (tb_lo_scheduler_t*)coroutine->scheduler;
    tb_assert(scheduler);

    // trace
    tb_trace_d("coroutine(%p): timer(%s) %s", coroutine, coroutine->rs.wait.object.type? "poller" : "sleep", killed? "killed" : "timeout");

    // resume the waited coroutine if timer task has been not canceled
    if (!killed)
    {
        // reset the waited coroutines in the poller data
        tb_size_t object_type = coroutine->rs.wait.object.type;
#ifndef TB_CONFIG_MICRO_ENABLE
        if (object_type == TB_POLLER_OBJECT_PROC)
            coroutine->rs.wait.object_waiting = 0;
        else
#endif
        if (object_type)
        {
            tb_lo_scheduler_io_ref_t scheduler_io = tb_lo_scheduler_io(scheduler);
            tb_lo_pollerdata_io_ref_t  pollerdata = (tb_lo_pollerdata_io_ref_t)(scheduler_io? tb_pollerdata_get(&scheduler_io->pollerdata, &coroutine->rs.wait.object) : tb_null);
            if (pollerdata)
            {
                if (coroutine == pollerdata->lo_recv)
                    pollerdata->lo_recv = tb_null;
                if (coroutine == pollerdata->lo_send)
                    pollerdata->lo_send = tb_null;
            }
        }

        // resume the coroutine
        tb_lo_scheduler_io_resume(scheduler, coroutine, TB_POLLER_EVENT_NONE);
    }
}
#endif
static tb_void_t tb_lo_scheduler_io_events(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    // check
    tb_lo_scheduler_io_ref_t scheduler_io = (tb_lo_scheduler_io_ref_t)tb_poller_priv(poller);
    tb_assert(scheduler_io && scheduler_io->scheduler && object);

#ifndef TB_CONFIG_MICRO_ENABLE
    // is process object?
    if (object->type == TB_POLLER_OBJECT_PROC)
    {
        // resume coroutine and return the process exit status
        tb_lo_coroutine_t* coroutine = (tb_lo_coroutine_t*)priv;
        tb_assert(coroutine);
        coroutine->rs.wait.object_event = events;

        // waiting process? resume this coroutine
        if (coroutine->rs.wait.object_waiting)
        {
            coroutine->rs.wait.object_waiting = 0;
            tb_lo_scheduler_io_resume(scheduler_io->scheduler, coroutine, 1);
        }
        else coroutine->rs.wait.object_pending = 1;
        return ;
    }
#endif

    // get pollerdata data
    tb_lo_pollerdata_io_ref_t pollerdata = (tb_lo_pollerdata_io_ref_t)tb_pollerdata_get(&scheduler_io->pollerdata, object);
    tb_assert(pollerdata);

    // get poller events
    tb_size_t events_prev_wait = pollerdata->poller_events_wait;
    tb_size_t events_prev_save = pollerdata->poller_events_save;

    // eof for edge trigger?
    if (events & TB_POLLER_EVENT_EOF)
    {
        // cache this eof as next recv/send event
        events &= ~TB_POLLER_EVENT_EOF;
        events_prev_save |= events_prev_wait;
        pollerdata->poller_events_save = (tb_uint16_t)events_prev_save;
    }

    // get the waiting coroutines
    tb_lo_coroutine_t* lo_recv = (events & TB_POLLER_EVENT_RECV)? pollerdata->lo_recv : tb_null;
    tb_lo_coroutine_t* lo_send = (events & TB_POLLER_EVENT_SEND)? pollerdata->lo_send : tb_null;

    // trace
    tb_trace_d("object: %p, trigger events %lu, lo_recv(%p), lo_send(%p)", object->ref.ptr, events, lo_recv, lo_send);

    // return the events result for the waiting coroutines
    if (lo_recv && lo_recv == lo_send)
    {
        pollerdata->lo_recv = tb_null;
        pollerdata->lo_send = tb_null;
        tb_lo_scheduler_io_resume(scheduler_io->scheduler, lo_recv, events);
    }
    else
    {
        if (lo_recv)
        {
            pollerdata->lo_recv = tb_null;
            tb_lo_scheduler_io_resume(scheduler_io->scheduler, lo_recv, events & ~TB_POLLER_EVENT_SEND);
            events &= ~TB_POLLER_EVENT_RECV;
        }
        if (lo_send)
        {
            pollerdata->lo_send = tb_null;
            tb_lo_scheduler_io_resume(scheduler_io->scheduler, lo_send, events & ~TB_POLLER_EVENT_RECV);
            events &= ~TB_POLLER_EVENT_SEND;
        }

        // no coroutines are waiting? cache this events
        if ((events & TB_POLLER_EVENT_RECV) || (events & TB_POLLER_EVENT_SEND))
        {
            // trace
            tb_trace_d("object: %p, cache events %lu", object->ref.ptr, events);

            // cache this events
            events_prev_save |= events;
            pollerdata->poller_events_save = (tb_uint16_t)events_prev_save;
        }
    }
}
#ifndef TB_CONFIG_MICRO_ENABLE
static tb_bool_t tb_lo_scheduler_io_timer_spak(tb_lo_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert(scheduler_io && scheduler_io->timer && scheduler_io->ltimer);

    // spak ctime
    tb_cache_time_spak();

    // spak timer
    if (!tb_timer_spak(scheduler_io->timer)) return tb_false;

    // spak ltimer
    if (!tb_ltimer_spak(scheduler_io->ltimer)) return tb_false;

    // pk
    return tb_true;
}
static tb_long_t tb_lo_scheduler_io_timer_delay(tb_lo_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert(scheduler_io && scheduler_io->timer && scheduler_io->ltimer);

    // the delay
    tb_size_t delay = tb_timer_delay(scheduler_io->timer);

    // the ldelay
    tb_size_t ldelay = tb_ltimer_delay(scheduler_io->ltimer);

    // return the timer delay
    return tb_min(delay, ldelay);
}
#else
static __tb_inline__ tb_long_t tb_lo_scheduler_io_timer_delay(tb_lo_scheduler_io_ref_t scheduler_io)
{
    return 1000;
}
#endif
static tb_void_t tb_lo_scheduler_io_loop(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_lo_scheduler_io_ref_t scheduler_io = (tb_lo_scheduler_io_ref_t)priv;
    tb_assert(scheduler_io && scheduler_io->poller);

    // the scheduler
    tb_lo_scheduler_t* scheduler = scheduler_io->scheduler;
    tb_assert(scheduler);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // loop
        while (!scheduler->stopped)
        {
            // finish all other ready coroutines first
            while (tb_lo_scheduler_ready_count(scheduler) > 1)
            {
                // yield it
                tb_lo_coroutine_yield();

#ifndef TB_CONFIG_MICRO_ENABLE
                // spak timer
                if (!tb_lo_scheduler_io_timer_spak(scheduler_io)) break;
#endif
            }

            // no more suspended coroutines? loop end
            tb_check_break(tb_lo_scheduler_suspend_count(scheduler));

            // trace
            tb_trace_d("loop: wait %ld ms ..", tb_lo_scheduler_io_timer_delay(scheduler_io));

            // no more ready coroutines? wait io events and timers (TODO)
            if (tb_poller_wait(scheduler_io->poller, tb_lo_scheduler_io_events, tb_lo_scheduler_io_timer_delay(scheduler_io)) < 0) break;

#ifndef TB_CONFIG_MICRO_ENABLE
            // spak timer
            if (!tb_lo_scheduler_io_timer_spak(scheduler_io)) break;
#endif
        }
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_lo_scheduler_io_ref_t tb_lo_scheduler_io_init(tb_lo_scheduler_t* scheduler)
{
    // done
    tb_bool_t                   ok = tb_false;
    tb_lo_scheduler_io_ref_t    scheduler_io = tb_null;
    do
    {
        // init io scheduler
        scheduler_io = tb_malloc0_type(tb_lo_scheduler_io_t);
        tb_assert_and_check_break(scheduler_io);

        // save scheduler
        scheduler_io->scheduler = (tb_lo_scheduler_t*)scheduler;

        // init poller
        scheduler_io->poller = tb_poller_init(scheduler_io);
        tb_assert_and_check_break(scheduler_io->poller);

        // attach poller
        tb_poller_attach(scheduler_io->poller);

#ifndef TB_CONFIG_MICRO_ENABLE
        // init timer and using cache time
        scheduler_io->timer = tb_timer_init(TB_SCHEDULER_IO_TIMER_GROW, tb_true);
        tb_assert_and_check_break(scheduler_io->timer);

        // init ltimer and using cache time
        scheduler_io->ltimer = tb_ltimer_init(TB_SCHEDULER_IO_LTIMER_GROW, TB_LTIMER_TICK_S, tb_true);
        tb_assert_and_check_break(scheduler_io->ltimer);
#endif

        // init poller data pool
        scheduler_io->pollerdata_pool = tb_fixed_pool_init(tb_null, TB_SCHEDULER_IO_POLLERDATA_GROW, sizeof(tb_lo_pollerdata_io_t), tb_null, tb_null, tb_null);
        tb_assert_and_check_break(scheduler_io->pollerdata_pool);

        // init poller data
        tb_pollerdata_init(&scheduler_io->pollerdata);

        // start the io loop coroutine
        if (!tb_lo_coroutine_start((tb_lo_scheduler_ref_t)scheduler, tb_lo_scheduler_io_loop, scheduler_io, tb_null)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit io scheduler
        if (scheduler_io) tb_lo_scheduler_io_exit(scheduler_io);
        scheduler_io = tb_null;
    }

    // ok?
    return scheduler_io;
}
tb_void_t tb_lo_scheduler_io_exit(tb_lo_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert_and_check_return(scheduler_io);

    // exit poller data
    tb_pollerdata_exit(&scheduler_io->pollerdata);

    // exit poller data pool
    if (scheduler_io->pollerdata_pool) tb_fixed_pool_exit(scheduler_io->pollerdata_pool);
    scheduler_io->pollerdata_pool = tb_null;

    // exit poller
    if (scheduler_io->poller) tb_poller_exit(scheduler_io->poller);
    scheduler_io->poller = tb_null;

#ifndef TB_CONFIG_MICRO_ENABLE
    // exit timer
    if (scheduler_io->timer) tb_timer_exit(scheduler_io->timer);
    scheduler_io->timer = tb_null;

    // exit ltimer
    if (scheduler_io->ltimer) tb_ltimer_exit(scheduler_io->ltimer);
    scheduler_io->ltimer = tb_null;
#endif

    // clear scheduler
    scheduler_io->scheduler = tb_null;

    // exit it
    tb_free(scheduler_io);
}
tb_lo_scheduler_io_ref_t tb_lo_scheduler_io_need(tb_lo_scheduler_t* scheduler)
{
    // get the current scheduler
    if (!scheduler) scheduler = (tb_lo_scheduler_t*)tb_lo_scheduler_self_();
    if (scheduler)
    {
        // init io scheduler first
        if (!scheduler->scheduler_io) scheduler->scheduler_io = tb_lo_scheduler_io_init(scheduler);
        tb_assert(scheduler->scheduler_io);

        // get the current io scheduler
        return (tb_lo_scheduler_io_ref_t)scheduler->scheduler_io;
    }
    return tb_null;
}
tb_void_t tb_lo_scheduler_io_kill(tb_lo_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert_and_check_return(scheduler_io);

    // trace
    tb_trace_d("kill: ..");

#ifndef TB_CONFIG_MICRO_ENABLE
    // kill timer
    if (scheduler_io->timer) tb_timer_kill(scheduler_io->timer);

    // kill ltimer
    if (scheduler_io->ltimer) tb_ltimer_kill(scheduler_io->ltimer);
#endif

    // kill poller
    if (scheduler_io->poller) tb_poller_kill(scheduler_io->poller);
}
tb_void_t tb_lo_scheduler_io_sleep(tb_lo_scheduler_io_ref_t scheduler_io, tb_long_t interval)
{
#ifndef TB_CONFIG_MICRO_ENABLE
    // check
    tb_assert_and_check_return(scheduler_io && scheduler_io->poller && scheduler_io->scheduler);

    // get the current coroutine
    tb_lo_coroutine_t* coroutine = tb_lo_scheduler_running(scheduler_io->scheduler);
    tb_assert(coroutine);

    // trace
    tb_trace_d("coroutine(%p): sleep %ld ms ..", coroutine, interval);

    // clear waiting task first
    coroutine->rs.wait.task = tb_null;
    coroutine->rs.wait.object.type = TB_POLLER_OBJECT_NONE;

    // infinity?
    if (interval > 0)
    {
        // high-precision interval?
        if (interval % 1000)
        {
            // post task to timer
            tb_timer_task_post(scheduler_io->timer, interval, tb_false, tb_lo_scheduler_io_timeout, coroutine);
        }
        // low-precision interval?
        else
        {
            // post task to ltimer (faster)
            tb_ltimer_task_post(scheduler_io->ltimer, interval, tb_false, tb_lo_scheduler_io_timeout, coroutine);
        }
    }
#else
    // not impl
    tb_trace_noimpl();
#endif
}
tb_bool_t tb_lo_scheduler_io_wait(tb_lo_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_assert(scheduler_io && object && scheduler_io->poller && scheduler_io->scheduler && events);

    // get the current coroutine
    tb_lo_coroutine_t* coroutine = tb_lo_scheduler_running(scheduler_io->scheduler);
    tb_assert(coroutine);

    // get the poller
    tb_poller_ref_t poller = scheduler_io->poller;
    tb_assert(poller);

    // trace
    tb_trace_d("coroutine(%p): wait events(%lu) with %ld ms for poller(%p) ..", coroutine, events, timeout, object->ref.ptr);

    // get and allocate a poller data
    tb_lo_pollerdata_io_ref_t pollerdata = (tb_lo_pollerdata_io_ref_t)tb_pollerdata_get(&scheduler_io->pollerdata, object);
    if (!pollerdata)
    {
        tb_assert(scheduler_io->pollerdata_pool);
        pollerdata = (tb_lo_pollerdata_io_ref_t)tb_fixed_pool_malloc0(scheduler_io->pollerdata_pool);
        tb_pollerdata_set(&scheduler_io->pollerdata, object, pollerdata);
    }
    tb_assert_and_check_return_val(pollerdata, -1);

    // enable edge-trigger mode if be supported
    if (tb_poller_support(poller, TB_POLLER_EVENT_CLEAR))
        events |= TB_POLLER_EVENT_CLEAR;

    // get the previous poller events
    tb_size_t events_wait = events;
    if (pollerdata->poller_events_wait)
    {
        // return the cached events directly if the waiting events exists cache
        tb_size_t events_prev_wait = pollerdata->poller_events_wait;
        tb_size_t events_prev_save = pollerdata->poller_events_save;
        if (events_prev_save && (events_prev_wait & events))
        {
            // check error?
            if (events_prev_save & TB_POLLER_EVENT_ERROR)
            {
                pollerdata->poller_events_save = 0;
                coroutine->rs.wait.result = -1;
                return tb_false;
            }

            // clear cache events
            pollerdata->poller_events_save = (tb_uint16_t)(events_prev_save & ~events);

            // return the cached events
            coroutine->rs.wait.result = events_prev_save & events;
            return tb_false;
        }

        // modify the wait events and reserve the pending events in other coroutine
        events_wait = events_prev_wait;
        if ((events_wait & TB_POLLER_EVENT_RECV) && !pollerdata->lo_recv) events_wait &= ~TB_POLLER_EVENT_RECV;
        if ((events_wait & TB_POLLER_EVENT_SEND) && !pollerdata->lo_send) events_wait &= ~TB_POLLER_EVENT_SEND;
        events_wait |= events;

        // modify poller from poller for waiting events if the waiting events has been changed
        if ((events_prev_wait & events_wait) != events_wait)
        {
            // trace
            tb_trace_d("modify poller object: %p events: %lx", object, events_wait);

            // may be wait recv/send at same time
            if (!tb_poller_modify(poller, object, events_wait | TB_POLLER_EVENT_NOEXTRA, tb_null))
            {
                // trace
                tb_trace_e("failed to modify object(%p) to poller on coroutine(%p)!", object->ref.ptr, coroutine);
                coroutine->rs.wait.result = -1;
                return tb_false;
            }
        }
    }
    else
    {
        // trace
        tb_trace_d("insert poller object: %p events: %lx", object->ref.ptr, events_wait);

        // insert poller to poller for waiting events
        if (!tb_poller_insert(poller, object, events_wait | TB_POLLER_EVENT_NOEXTRA, tb_null))
        {
            // trace
            tb_trace_e("failed to insert object(%p) to poller on coroutine(%p)!", object->ref.ptr, coroutine);
            coroutine->rs.wait.result = -1;
            return tb_false;
        }
    }

#ifndef TB_CONFIG_MICRO_ENABLE
    // exists timeout?
    tb_cpointer_t   task = tb_null;
    tb_bool_t       is_ltimer = tb_false;
    if (timeout >= 0)
    {
        // high-precision interval?
        if (timeout % 1000)
        {
            // init task for timer
            task = tb_timer_task_init(scheduler_io->timer, timeout, tb_false, tb_lo_scheduler_io_timeout, coroutine);
            tb_assert_and_check_return_val(task, tb_false);
        }
        // low-precision interval?
        else
        {
            // init task for ltimer (faster)
            task = tb_ltimer_task_init(scheduler_io->ltimer, timeout, tb_false, tb_lo_scheduler_io_timeout, coroutine);
            tb_assert_and_check_return_val(task, tb_false);

            // mark as low-precision timer
            is_ltimer = tb_true;
        }
    }

    // save the timer task to coroutine
    coroutine->rs.wait.task = task;
    coroutine->rs.wait.is_ltimer = is_ltimer;
#endif
    coroutine->rs.wait.object = *object;
    coroutine->rs.wait.result = 0;

    // save waiting events
    pollerdata->poller_events_wait = (tb_uint16_t)events_wait;
    pollerdata->poller_events_save = 0;

    // save the current coroutine
    if (events & TB_POLLER_EVENT_RECV) pollerdata->lo_recv = coroutine;
    if (events & TB_POLLER_EVENT_SEND) pollerdata->lo_send = coroutine;

    // suspend the current coroutine
    return tb_true;
}
#ifndef TB_CONFIG_MICRO_ENABLE
tb_bool_t tb_lo_scheduler_io_wait_proc(tb_lo_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object, tb_long_t timeout)
{
    // check
    tb_assert(scheduler_io && scheduler_io->poller && scheduler_io->scheduler);
    tb_assert(object && object->type == TB_POLLER_OBJECT_PROC && object->ref.proc);

    // get the current coroutine
    tb_lo_coroutine_t* coroutine = tb_lo_scheduler_running(scheduler_io->scheduler);
    tb_assert(coroutine);

    // get the poller
    tb_poller_ref_t poller = scheduler_io->poller;
    tb_assert(poller);

    // trace
    tb_trace_d("coroutine(%p): wait process object(%p) with %ld ms ..", coroutine, object->ref.proc, timeout);

    // has pending process status?
    if (coroutine->rs.wait.object_pending)
    {
        coroutine->rs.wait.object_pending = 0;
        coroutine->rs.wait.result = 1;
        return tb_false;
    }

    // insert poller object to poller for waiting process
    if (!tb_poller_insert(poller, object, 0, coroutine))
    {
        // trace
        tb_trace_e("failed to insert process object(%p) to poller on coroutine(%p)!", object->ref.proc, coroutine);
        coroutine->rs.wait.result = -1;
        return tb_false;
    }

    // exists timeout?
    tb_cpointer_t   task = tb_null;
    tb_bool_t       is_ltimer = tb_false;
    if (timeout >= 0)
    {
        // high-precision interval?
        if (timeout % 1000)
        {
            // init task for timer
            task = tb_timer_task_init(scheduler_io->timer, timeout, tb_false, tb_lo_scheduler_io_timeout, coroutine);
            tb_assert_and_check_return_val(task, tb_false);
        }
        // low-precision interval?
        else
        {
            // init task for ltimer (faster)
            task = tb_ltimer_task_init(scheduler_io->ltimer, timeout, tb_false, tb_lo_scheduler_io_timeout, coroutine);
            tb_assert_and_check_return_val(task, tb_false);

            // mark as low-precision timer
            is_ltimer = tb_true;
        }
    }

    // save the timer task to coroutine
    coroutine->rs.wait.task           = task;
    coroutine->rs.wait.object         = *object;
    coroutine->rs.wait.is_ltimer      = is_ltimer;
    coroutine->rs.wait.object_event   = 0;
    coroutine->rs.wait.object_pending = 0;
    coroutine->rs.wait.object_waiting = 1;

    // suspend the current coroutine
    return tb_true;
}
#endif
tb_bool_t tb_lo_scheduler_io_cancel(tb_lo_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object)
{
    // check
    tb_assert(scheduler_io && object && scheduler_io->poller && scheduler_io->scheduler);

    // get the current coroutine
    tb_lo_coroutine_t* coroutine = tb_lo_scheduler_running(scheduler_io->scheduler);
    tb_check_return_val(coroutine, tb_false);

    // trace
    tb_trace_d("coroutine(%p): cancel poller object(%p) ..", coroutine, object->ref.ptr);

#ifndef TB_CONFIG_MICRO_ENABLE
    // cancel process object
    if (object->type == TB_POLLER_OBJECT_PROC)
    {
        // clear process status
        coroutine->rs.wait.object_event = 0;
        coroutine->rs.wait.object_pending = 0;
        coroutine->rs.wait.object_waiting = 0;

        // remove the previous poller object first if exists
        if (!tb_poller_remove(scheduler_io->poller, object))
        {
            // trace
            tb_trace_e("failed to remove object(%p) to poller on coroutine(%p)!", object->ref.ptr, coroutine);
            return tb_false;
        }
        return tb_true;
    }
#endif

    // reset the pollerdata data
    tb_lo_pollerdata_io_ref_t pollerdata = (tb_lo_pollerdata_io_ref_t)tb_pollerdata_get(&scheduler_io->pollerdata, object);
    if (pollerdata)
    {
        // clear the waiting coroutines
        pollerdata->lo_recv = tb_null;
        pollerdata->lo_send = tb_null;

        // remove the this poller from poller
        if (pollerdata->poller_events_wait)
        {
            // remove the previous poller first if exists
            if (!tb_poller_remove(scheduler_io->poller, object))
            {
                // trace
                tb_trace_e("failed to remove object(%p) to poller on coroutine(%p)!", object->ref.ptr, coroutine);

                // failed
                coroutine->rs.wait.result = -1;
                return tb_false;
            }

            // remove the poller events
            coroutine->rs.wait.result = 0;
            pollerdata->poller_events_wait = 0;
            pollerdata->poller_events_save = 0;
            return tb_true;
        }
    }

    // no this poller
    return tb_false;
}
tb_lo_scheduler_io_ref_t tb_lo_scheduler_io_self()
{
    // get the current scheduler
    tb_lo_scheduler_t* scheduler = (tb_lo_scheduler_t*)tb_lo_scheduler_self_();

    // get the current io scheduler
    return scheduler? (tb_lo_scheduler_io_ref_t)scheduler->scheduler_io : tb_null;
}

