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
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scheduler_io.h"
#include "coroutine.h"

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

// the poller object data grow
#ifdef __tb_small__
#   define TB_SCHEDULER_IO_POLLERDATA_GROW    (64)
#else
#   define TB_SCHEDULER_IO_POLLERDATA_GROW    (4096)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_co_scheduler_io_resume(tb_co_scheduler_t* scheduler, tb_coroutine_t* coroutine, tb_size_t events)
{
    // exists the timer task? remove it
    tb_cpointer_t task = coroutine->rs.wait.task;
    if (task)
    {
        // get io scheduler
        tb_co_scheduler_io_ref_t scheduler_io = tb_co_scheduler_io(scheduler);
        tb_assert(scheduler_io && scheduler_io->poller);

        // remove the timer task
        if (coroutine->rs.wait.is_ltimer) tb_ltimer_task_exit(scheduler_io->ltimer, (tb_ltimer_task_ref_t)task);
        else tb_timer_task_exit(scheduler_io->timer, (tb_timer_task_ref_t)task);
        coroutine->rs.wait.task = tb_null;
    }

    // resume the coroutine
    tb_co_scheduler_resume(scheduler, coroutine,  (tb_cpointer_t)((events & TB_POLLER_EVENT_ERROR)? -1 : events));
}
static tb_void_t tb_co_scheduler_io_timeout(tb_bool_t killed, tb_cpointer_t priv)
{
    // check
    tb_coroutine_t* coroutine = (tb_coroutine_t*)priv;
    tb_assert(coroutine);

    // get scheduler
    tb_co_scheduler_t* scheduler = (tb_co_scheduler_t*)tb_coroutine_scheduler(coroutine);
    tb_assert(scheduler);

    // trace
    tb_trace_d("coroutine(%p): timer(%s) %s", coroutine, coroutine->rs.wait.object.type? "poller object" : "sleep", killed? "killed" : "timeout");

    // resume the waited coroutine if timer task has been not canceled
    if (!killed)
    {
        // reset the waited coroutines in the poller object data
        tb_size_t object_type = coroutine->rs.wait.object.type;
        if (object_type == TB_POLLER_OBJECT_PROC || object_type == TB_POLLER_OBJECT_FWATCHER)
            coroutine->rs.wait.object_waiting = 0;
        else if (object_type)
        {
            tb_co_scheduler_io_ref_t   scheduler_io = tb_co_scheduler_io(scheduler);
            tb_co_pollerdata_io_ref_t  pollerdata = (tb_co_pollerdata_io_ref_t)(scheduler_io? tb_pollerdata_get(&scheduler_io->pollerdata, &coroutine->rs.wait.object) : tb_null);
            if (pollerdata)
            {
                if (coroutine == pollerdata->co_recv)
                    pollerdata->co_recv = tb_null;
                if (coroutine == pollerdata->co_send)
                    pollerdata->co_send = tb_null;
            }
        }

        // resume the coroutine
        tb_co_scheduler_io_resume(scheduler, coroutine, TB_POLLER_EVENT_NONE);
    }
}
static tb_void_t tb_co_scheduler_io_events(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    // check
    tb_co_scheduler_io_ref_t scheduler_io = (tb_co_scheduler_io_ref_t)tb_poller_priv(poller);
    tb_assert(scheduler_io && scheduler_io->scheduler && object);

    // is process/fwatcher object?
    if (object->type == TB_POLLER_OBJECT_PROC || object->type == TB_POLLER_OBJECT_FWATCHER)
    {
        // resume coroutine and return the process exit status
        tb_coroutine_t* coroutine = (tb_coroutine_t*)priv;
        tb_assert(coroutine);
        coroutine->rs.wait.object_event = events;

        // waiting process? resume this coroutine
        if (coroutine->rs.wait.object_waiting)
        {
            coroutine->rs.wait.object_waiting = 0;
            tb_co_scheduler_io_resume(scheduler_io->scheduler, coroutine, 1);
        }
        else coroutine->rs.wait.object_pending = 1;
        return ;
    }

    // get pollerdata data
    tb_co_pollerdata_io_ref_t pollerdata = (tb_co_pollerdata_io_ref_t)tb_pollerdata_get(&scheduler_io->pollerdata, object);
    tb_assert(pollerdata);

    // get poller object events
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
    tb_coroutine_t* co_recv = (events & TB_POLLER_EVENT_RECV)? pollerdata->co_recv : tb_null;
    tb_coroutine_t* co_send = (events & TB_POLLER_EVENT_SEND)? pollerdata->co_send : tb_null;

    // trace
    tb_trace_d("object: %p, trigger events %lu, co_recv(%p), co_send(%p)", object->ref.ptr, events, co_recv, co_send);

    // return the events result for the waiting coroutines
    if (co_recv && co_recv == co_send)
    {
        pollerdata->co_recv = tb_null;
        pollerdata->co_send = tb_null;
        tb_co_scheduler_io_resume(scheduler_io->scheduler, co_recv, events);
    }
    else
    {
        if (co_recv)
        {
            pollerdata->co_recv = tb_null;
            tb_co_scheduler_io_resume(scheduler_io->scheduler, co_recv, events & ~TB_POLLER_EVENT_SEND);
            events &= ~TB_POLLER_EVENT_RECV;
        }
        if (co_send)
        {
            pollerdata->co_send = tb_null;
            tb_co_scheduler_io_resume(scheduler_io->scheduler, co_send, events & ~TB_POLLER_EVENT_RECV);
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
static tb_bool_t tb_co_scheduler_io_timer_spak(tb_co_scheduler_io_ref_t scheduler_io)
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
static tb_void_t tb_co_scheduler_io_loop(tb_cpointer_t priv)
{
    // check
    tb_co_scheduler_io_ref_t scheduler_io = (tb_co_scheduler_io_ref_t)priv;
    tb_assert_and_check_return(scheduler_io && scheduler_io->timer && scheduler_io->ltimer);

    // the scheduler
    tb_co_scheduler_t* scheduler = scheduler_io->scheduler;
    tb_assert_and_check_return(scheduler);

    // the poller
    tb_poller_ref_t poller = scheduler_io->poller;
    tb_assert_and_check_return(poller);

    // loop
    while (!scheduler->stopped)
    {
        // finish all other ready coroutines first
        while (tb_co_scheduler_yield(scheduler))
        {
            // spak timer
            if (!tb_co_scheduler_io_timer_spak(scheduler_io)) break;
        }

        // no more suspended coroutines? loop end
        tb_check_break(tb_co_scheduler_suspend_count(scheduler));

        // the delay
        tb_size_t delay = tb_timer_delay(scheduler_io->timer);

        // the ldelay
        tb_size_t ldelay = tb_ltimer_delay(scheduler_io->ltimer);

        // trace
        tb_trace_d("loop: wait %lu ms, %lu pending coroutines ..", tb_min(delay, ldelay), tb_co_scheduler_suspend_count(scheduler));

        // no more ready coroutines? wait io events and timers
        if (tb_poller_wait(poller, tb_co_scheduler_io_events, tb_min(delay, ldelay)) < 0)
        {
            tb_trace_e("loop: wait poller failed!");
            break;
        }

        // trace
        tb_trace_d("loop: wait ok, left %lu pending coroutines ..", tb_co_scheduler_suspend_count(scheduler));

        // spak timer
        if (!tb_co_scheduler_io_timer_spak(scheduler_io)) break;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_co_scheduler_io_ref_t tb_co_scheduler_io_init(tb_co_scheduler_t* scheduler)
{
    // done
    tb_bool_t                   ok = tb_false;
    tb_co_scheduler_io_ref_t    scheduler_io = tb_null;
    do
    {
        // init io scheduler
        scheduler_io = tb_malloc0_type(tb_co_scheduler_io_t);
        tb_assert_and_check_break(scheduler_io);

        // save scheduler
        scheduler_io->scheduler = (tb_co_scheduler_t*)scheduler;

        // init timer and using cache time
        scheduler_io->timer = tb_timer_init(TB_SCHEDULER_IO_TIMER_GROW, tb_true);
        tb_assert_and_check_break(scheduler_io->timer);

        // init ltimer and using cache time
        scheduler_io->ltimer = tb_ltimer_init(TB_SCHEDULER_IO_LTIMER_GROW, TB_LTIMER_TICK_S, tb_true);
        tb_assert_and_check_break(scheduler_io->ltimer);

        // init poller
        scheduler_io->poller = tb_poller_init(scheduler_io);
        tb_assert_and_check_break(scheduler_io->poller);

        // attach poller
        tb_poller_attach(scheduler_io->poller);

        // init poller object data pool
        scheduler_io->pollerdata_pool = tb_fixed_pool_init(tb_null, TB_SCHEDULER_IO_POLLERDATA_GROW, sizeof(tb_co_pollerdata_io_t), tb_null, tb_null, tb_null);
        tb_assert_and_check_break(scheduler_io->pollerdata_pool);

        // init poller object data
        tb_pollerdata_init(&scheduler_io->pollerdata);

        // start the io loop coroutine
        if (!tb_co_scheduler_start(scheduler_io->scheduler, tb_co_scheduler_io_loop, scheduler_io, 0)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit io scheduler
        if (scheduler_io) tb_co_scheduler_io_exit(scheduler_io);
        scheduler_io = tb_null;
    }

    // ok?
    return scheduler_io;
}
tb_void_t tb_co_scheduler_io_exit(tb_co_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert_and_check_return(scheduler_io);

    // exit poller object data
    tb_pollerdata_exit(&scheduler_io->pollerdata);

    // exit poller object data pool
    if (scheduler_io->pollerdata_pool) tb_fixed_pool_exit(scheduler_io->pollerdata_pool);
    scheduler_io->pollerdata_pool = tb_null;

    // exit poller
    if (scheduler_io->poller) tb_poller_exit(scheduler_io->poller);
    scheduler_io->poller = tb_null;

    // exit timer
    if (scheduler_io->timer) tb_timer_exit(scheduler_io->timer);
    scheduler_io->timer = tb_null;

    // exit ltimer
    if (scheduler_io->ltimer) tb_ltimer_exit(scheduler_io->ltimer);
    scheduler_io->ltimer = tb_null;

    // clear scheduler
    scheduler_io->scheduler = tb_null;

    // exit it
    tb_free(scheduler_io);
}
tb_co_scheduler_io_ref_t tb_co_scheduler_io_need(tb_co_scheduler_t* scheduler)
{
    // get the current scheduler
    if (!scheduler) scheduler = (tb_co_scheduler_t*)tb_co_scheduler_self();
    if (scheduler)
    {
        // init io scheduler first
        if (!scheduler->scheduler_io) scheduler->scheduler_io = tb_co_scheduler_io_init(scheduler);
        tb_assert(scheduler->scheduler_io);

        // get the current io scheduler
        return (tb_co_scheduler_io_ref_t)scheduler->scheduler_io;
    }
    return tb_null;
}
tb_void_t tb_co_scheduler_io_kill(tb_co_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert_and_check_return(scheduler_io);

    // trace
    tb_trace_d("kill: ..");

    // kill timer
    if (scheduler_io->timer) tb_timer_kill(scheduler_io->timer);

    // kill ltimer
    if (scheduler_io->ltimer) tb_ltimer_kill(scheduler_io->ltimer);

    // kill poller
    if (scheduler_io->poller) tb_poller_kill(scheduler_io->poller);
}
tb_pointer_t tb_co_scheduler_io_sleep(tb_co_scheduler_io_ref_t scheduler_io, tb_long_t interval)
{
    // check
    tb_assert_and_check_return_val(scheduler_io && scheduler_io->poller && scheduler_io->scheduler, tb_null);

    // no sleep?
    tb_check_return_val(interval, tb_null);

    // get the current coroutine
    tb_coroutine_t* coroutine = tb_co_scheduler_running(scheduler_io->scheduler);
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
            tb_timer_task_post(scheduler_io->timer, interval, tb_false, tb_co_scheduler_io_timeout, coroutine);
        }
        // low-precision interval?
        else
        {
            // post task to ltimer (faster)
            tb_ltimer_task_post(scheduler_io->ltimer, interval, tb_false, tb_co_scheduler_io_timeout, coroutine);
        }
    }

    // suspend it
    return tb_co_scheduler_suspend(scheduler_io->scheduler, tb_null);
}
tb_long_t tb_co_scheduler_io_wait(tb_co_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_assert(scheduler_io && object && scheduler_io->poller && scheduler_io->scheduler && events);

    // get the current coroutine
    tb_coroutine_t* coroutine = tb_co_scheduler_running(scheduler_io->scheduler);
    tb_assert(coroutine);

    // get the poller
    tb_poller_ref_t poller = scheduler_io->poller;
    tb_assert(poller);

    // trace
    tb_trace_d("coroutine(%p): wait events(%lu) with %ld ms for object(%p) ..", coroutine, events, timeout, object->ref.ptr);

    // get and allocate a poller object data
    tb_co_pollerdata_io_ref_t pollerdata = (tb_co_pollerdata_io_ref_t)tb_pollerdata_get(&scheduler_io->pollerdata, object);
    if (!pollerdata)
    {
        tb_assert(scheduler_io->pollerdata_pool);
        pollerdata = (tb_co_pollerdata_io_ref_t)tb_fixed_pool_malloc0(scheduler_io->pollerdata_pool);
        tb_pollerdata_set(&scheduler_io->pollerdata, object, pollerdata);
    }
    tb_assert_and_check_return_val(pollerdata, -1);

    // enable edge-trigger mode if be supported
    if (tb_poller_support(poller, TB_POLLER_EVENT_CLEAR))
        events |= TB_POLLER_EVENT_CLEAR;

    // get the previous poller object events
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
                return -1;
            }

            // clear cache events
            pollerdata->poller_events_save = (tb_uint16_t)(events_prev_save & ~events);

            // return the cached events
            return events_prev_save & events;
        }

        // modify the wait events and reserve the pending events in other coroutine
        events_wait = events_prev_wait;
        if ((events_wait & TB_POLLER_EVENT_RECV) && !pollerdata->co_recv) events_wait &= ~TB_POLLER_EVENT_RECV;
        if ((events_wait & TB_POLLER_EVENT_SEND) && !pollerdata->co_send) events_wait &= ~TB_POLLER_EVENT_SEND;
        events_wait |= events;

        // modify poller object from poller for waiting events if the waiting events has been changed
        if ((events_prev_wait & events_wait) != events_wait)
        {
            // trace
            tb_trace_d("modify poller object: %p events: %lx", object->ref.ptr, events_wait);

            // may be wait recv/send at same time
            if (!tb_poller_modify(poller, object, events_wait | TB_POLLER_EVENT_NOEXTRA, tb_null))
            {
                // trace
                tb_trace_e("failed to modify object(%p) to poller on coroutine(%p)!", object->ref.ptr, coroutine);
                return -1;
            }
        }
    }
    else
    {
        // trace
        tb_trace_d("insert poller object: %p events: %lx", object->ref.ptr, events_wait);

        // insert poller object to poller for waiting events
        if (!tb_poller_insert(poller, object, events_wait | TB_POLLER_EVENT_NOEXTRA, tb_null))
        {
            // trace
            tb_trace_e("failed to insert object(%p) to poller on coroutine(%p)!", object->ref.ptr, coroutine);
            return -1;
        }
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
            task = tb_timer_task_init(scheduler_io->timer, timeout, tb_false, tb_co_scheduler_io_timeout, coroutine);
            tb_assert_and_check_return_val(task, tb_false);
        }
        // low-precision interval?
        else
        {
            // init task for ltimer (faster)
            task = tb_ltimer_task_init(scheduler_io->ltimer, timeout, tb_false, tb_co_scheduler_io_timeout, coroutine);
            tb_assert_and_check_return_val(task, tb_false);

            // mark as low-precision timer
            is_ltimer = tb_true;
        }
    }

    // save the timer task to coroutine
    coroutine->rs.wait.task         = task;
    coroutine->rs.wait.object       = *object;
    coroutine->rs.wait.is_ltimer    = is_ltimer;

    // save waiting events
    pollerdata->poller_events_wait = (tb_uint16_t)events_wait;
    pollerdata->poller_events_save = 0;

    // save the current coroutine
    if (events & TB_POLLER_EVENT_RECV) pollerdata->co_recv = coroutine;
    if (events & TB_POLLER_EVENT_SEND) pollerdata->co_send = coroutine;

    // suspend the current coroutine and return the waited result
    return (tb_long_t)tb_co_scheduler_suspend(scheduler_io->scheduler, tb_null);
}
tb_long_t tb_co_scheduler_io_wait_proc(tb_co_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object, tb_long_t* pstatus, tb_long_t timeout)
{
    // check
    tb_assert(scheduler_io && scheduler_io->poller && scheduler_io->scheduler);
    tb_assert(object && object->type == TB_POLLER_OBJECT_PROC && object->ref.proc);

    // get the current coroutine
    tb_coroutine_t* coroutine = tb_co_scheduler_running(scheduler_io->scheduler);
    tb_assert(coroutine);

    // get the poller
    tb_poller_ref_t poller = scheduler_io->poller;
    tb_assert(poller);

    // trace
    tb_trace_d("coroutine(%p): wait process object(%p) with %ld ms ..", coroutine, object->ref.proc, timeout);

    // has pending process status?
    if (coroutine->rs.wait.object_pending)
    {
        if (pstatus) *pstatus = coroutine->rs.wait.object_event;
        coroutine->rs.wait.object_pending = 0;
        return 1;
    }

    // insert poller object to poller for waiting process
    if (!tb_poller_insert(poller, object, 0, coroutine))
    {
        // trace
        tb_trace_e("failed to insert process object(%p) to poller on coroutine(%p)!", object->ref.proc, coroutine);
        return -1;
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
            task = tb_timer_task_init(scheduler_io->timer, timeout, tb_false, tb_co_scheduler_io_timeout, coroutine);
            tb_assert_and_check_return_val(task, tb_false);
        }
        // low-precision interval?
        else
        {
            // init task for ltimer (faster)
            task = tb_ltimer_task_init(scheduler_io->ltimer, timeout, tb_false, tb_co_scheduler_io_timeout, coroutine);
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

    // suspend the current coroutine and return the waited result
    tb_long_t ok = (tb_long_t)tb_co_scheduler_suspend(scheduler_io->scheduler, tb_null);
    if (ok > 0 && pstatus) *pstatus = coroutine->rs.wait.object_event;
    return ok;
}
tb_long_t tb_co_scheduler_io_wait_fwatcher(tb_co_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object, tb_fwatcher_event_t* pevent, tb_long_t timeout)
{
    // check
    tb_assert(scheduler_io && scheduler_io->poller && scheduler_io->scheduler);
    tb_assert(object && object->type == TB_POLLER_OBJECT_FWATCHER && object->ref.fwatcher);

    // get the current coroutine
    tb_coroutine_t* coroutine = tb_co_scheduler_running(scheduler_io->scheduler);
    tb_assert(coroutine);

    // get the poller
    tb_poller_ref_t poller = scheduler_io->poller;
    tb_assert(poller);

    // trace
    tb_trace_d("coroutine(%p): wait fwatcher object(%p) with %ld ms ..", coroutine, object->ref.fwatcher, timeout);

    // has pending fwatcher event?
    if (coroutine->rs.wait.object_pending)
    {
        if (pevent) *pevent = *((tb_fwatcher_event_t*)coroutine->rs.wait.object_event);
        coroutine->rs.wait.object_pending = 0;
        return 1;
    }

    // insert poller object to poller for waiting fwatcher
    if (!tb_poller_insert(poller, object, 0, coroutine))
    {
        // trace
        tb_trace_e("failed to insert fwatcher object(%p) to poller on coroutine(%p)!", object->ref.fwatcher, coroutine);
        return -1;
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
            task = tb_timer_task_init(scheduler_io->timer, timeout, tb_false, tb_co_scheduler_io_timeout, coroutine);
            tb_assert_and_check_return_val(task, tb_false);
        }
        // low-precision interval?
        else
        {
            // init task for ltimer (faster)
            task = tb_ltimer_task_init(scheduler_io->ltimer, timeout, tb_false, tb_co_scheduler_io_timeout, coroutine);
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

    // suspend the current coroutine and return the waited result
    tb_long_t ok = (tb_long_t)tb_co_scheduler_suspend(scheduler_io->scheduler, tb_null);
    if (ok > 0 && pevent) *pevent = *((tb_fwatcher_event_t*)coroutine->rs.wait.object_event);
    return ok;
}
tb_bool_t tb_co_scheduler_io_cancel(tb_co_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object)
{
    // check
    tb_assert(scheduler_io && object && scheduler_io->poller && scheduler_io->scheduler);

    // get the current coroutine
    tb_coroutine_t* coroutine = tb_co_scheduler_running(scheduler_io->scheduler);
    tb_check_return_val(coroutine, tb_false);

    // trace
    tb_trace_d("coroutine(%p): cancel poller object(%p) ..", coroutine, object->ref.ptr);

    // cancel process object
    if (object->type == TB_POLLER_OBJECT_PROC || object->type == TB_POLLER_OBJECT_FWATCHER)
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

    // reset the pollerdata data
    tb_co_pollerdata_io_ref_t pollerdata = (tb_co_pollerdata_io_ref_t)tb_pollerdata_get(&scheduler_io->pollerdata, object);
    if (pollerdata)
    {
        // clear the waiting coroutines
        pollerdata->co_recv = tb_null;
        pollerdata->co_send = tb_null;

        // remove the this poller object from poller
        if (pollerdata->poller_events_wait)
        {
            // remove the previous poller object first if exists
            if (!tb_poller_remove(scheduler_io->poller, object))
            {
                // trace
                tb_trace_e("failed to remove object(%p) to poller on coroutine(%p)!", object->ref.ptr, coroutine);
                return tb_false;
            }

            // remove the poller object events
            pollerdata->poller_events_wait = 0;
            pollerdata->poller_events_save = 0;
            return tb_true;
        }
    }

    // no this poller object
    return tb_false;
}
tb_co_scheduler_io_ref_t tb_co_scheduler_io_self()
{
    // get the current scheduler
    tb_co_scheduler_t* scheduler = (tb_co_scheduler_t*)tb_co_scheduler_self();

    // get the current io scheduler
    return scheduler? (tb_co_scheduler_io_ref_t)scheduler->scheduler_io : tb_null;
}

