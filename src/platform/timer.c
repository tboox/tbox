/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        timer.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "timer"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "platform.h"
#include "../memory/memory.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the timer task impl type
typedef struct __tb_timer_task_impl_t
{
    // the func
    tb_timer_task_func_t        func;

    // the priv
    tb_cpointer_t               priv;

    // the when
    tb_hong_t                   when;

    // the period
    tb_uint32_t                 period  : 28;

    // is repeat?
    tb_uint32_t                 repeat  : 1;

    // is killed?
    tb_uint32_t                 killed  : 1;

    // the refn, <= 2
    tb_uint32_t                 refn    : 2;

}tb_timer_task_impl_t;

/// the timer impl type
typedef struct __tb_timer_impl_t
{
    // the maxn
    tb_size_t                   maxn;

    // is stoped?
    tb_atomic_t                 stop;

    // is worked?
    tb_atomic_t                 work;

    // cache time?
    tb_bool_t                   ctime;

    // the lock
    tb_spinlock_t               lock;

    // the pool
    tb_fixed_pool_ref_t         pool;

    // the heap
    tb_heap_ref_t               heap;

    // the event
    tb_event_ref_t              event;

}tb_timer_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_hong_t tb_timer_now(tb_timer_impl_t* impl)
{
    // using the real time?
    if (!impl->ctime)
    {
        // get the time
        tb_timeval_t tv = {0};
        if (tb_gettimeofday(&tv, tb_null)) return ((tb_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
    }

    // using cached time
    return tb_cache_time_mclock();
}
static tb_long_t tb_timer_comp_by_when(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    // check
    tb_timer_task_impl_t const* ltask = (tb_timer_task_impl_t const*)ldata;
    tb_timer_task_impl_t const* rtask = (tb_timer_task_impl_t const*)rdata;
    tb_assert_and_check_return_val(ltask && rtask, -1);

    // comp
    return (ltask->when > rtask->when? 1 : (ltask->when < rtask->when? -1 : 0));
}
static tb_long_t tb_timer_comp_by_task(tb_iterator_ref_t iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    return ((tb_long_t)ltem > (tb_long_t)rtem? 1 : ((tb_long_t)ltem < (tb_long_t)rtem? -1 : 0));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_timer_ref_t tb_timer_init(tb_size_t maxn, tb_bool_t ctime)
{
    // done
    tb_bool_t           ok = tb_false;
    tb_timer_impl_t*    impl = tb_null;
    do
    {
        // make timer
        impl = tb_malloc0_type(tb_timer_impl_t);
        tb_assert_and_check_break(impl);

        // init func
        tb_item_func_t func = tb_item_func_ptr(tb_null, tb_null); func.comp = tb_timer_comp_by_when;

        // init timer
        impl->maxn         = tb_max(maxn, 16);
        impl->ctime        = ctime;

        // init lock
        if (!tb_spinlock_init(&impl->lock)) break;

        // init pool
        impl->pool         = tb_fixed_pool_init(tb_null, (maxn >> 2) + 16, sizeof(tb_timer_task_impl_t), tb_null, tb_null, tb_null);
        tb_assert_and_check_break(impl->pool);
        
        // init heap
        impl->heap         = tb_heap_init((maxn >> 2) + 16, func);
        tb_assert_and_check_break(impl->heap);

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&impl->lock, TB_TRACE_MODULE_NAME);
#endif
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_timer_exit((tb_timer_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_timer_ref_t)impl;
}
tb_void_t tb_timer_exit(tb_timer_ref_t timer)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return(impl);

    // stop it
    tb_atomic_set(&impl->stop, 1);

    // wait loop exit
    tb_size_t tryn = 10;
    while (tb_atomic_get(&impl->work) && tryn--) tb_msleep(500);

    // warning
    if (!tryn && tb_atomic_get(&impl->work)) tb_trace_w("[timer]: the loop has been not exited now!");

    // post event
    tb_spinlock_enter(&impl->lock);
    tb_event_ref_t event = impl->event;
    tb_spinlock_leave(&impl->lock);
    if (event) tb_event_post(event);

    // enter
    tb_spinlock_enter(&impl->lock);

    // exit heap
    if (impl->heap) tb_heap_exit(impl->heap);
    impl->heap = tb_null;

    // exit pool
    if (impl->pool) tb_fixed_pool_exit(impl->pool);
    impl->pool = tb_null;

    // exit event
    if (impl->event) tb_event_exit(impl->event);
    impl->event = tb_null;

    // leave
    tb_spinlock_leave(&impl->lock);

    // exit lock
    tb_spinlock_exit(&impl->lock);

    // exit it
    tb_free(impl);
}
tb_void_t tb_timer_clear(tb_timer_ref_t timer)
{
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    if (impl)
    {
        // enter
        tb_spinlock_enter(&impl->lock);

        // clear heap
        if (impl->heap) tb_heap_clear(impl->heap);

        // clear pool
        if (impl->pool) tb_fixed_pool_clear(impl->pool);

        // leave
        tb_spinlock_leave(&impl->lock);
    }
}
tb_hize_t tb_timer_top(tb_timer_ref_t timer)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && impl->heap, -1);

    // stoped?
    tb_assert_and_check_return_val(!tb_atomic_get(&impl->stop), -1);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    tb_hize_t when = -1; 
    if (tb_heap_size(impl->heap))
    {
        // the task
        tb_timer_task_impl_t const* task_impl = (tb_timer_task_impl_t const*)tb_heap_top(impl->heap);
        if (task_impl) when = task_impl->when;
    }

    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return when;
}
tb_size_t tb_timer_delay(tb_timer_ref_t timer)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && impl->heap, -1);

    // stoped?
    tb_assert_and_check_return_val(!tb_atomic_get(&impl->stop), -1);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    tb_size_t delay = -1; 
    if (tb_heap_size(impl->heap))
    {
        // the task
        tb_timer_task_impl_t const* task_impl = (tb_timer_task_impl_t const*)tb_heap_top(impl->heap);
        if (task_impl)
        {
            // the now
            tb_hong_t now = tb_timer_now(impl);

            // the delay
            delay = task_impl->when > now? (tb_size_t)(task_impl->when - now) : 0;
        }
    }

    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return delay;
}
tb_bool_t tb_timer_spak(tb_timer_ref_t timer)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && impl->pool && impl->heap, tb_false);

    // stoped?
    tb_check_return_val(!tb_atomic_get(&impl->stop), tb_false);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    tb_bool_t               ok = tb_false;
    tb_timer_task_func_t    func = tb_null;
    tb_cpointer_t           priv = tb_null;
    tb_bool_t               killed = tb_false;
    do
    {
        // empty? 
        if (!tb_heap_size(impl->heap))
        {
            ok = tb_true;
            break;
        }

        // the top task
        tb_timer_task_impl_t* task_impl = (tb_timer_task_impl_t*)tb_heap_top(impl->heap);
        tb_assert_and_check_break(task_impl);

        // check refn
        tb_assert(task_impl->refn);

        // the now
        tb_hong_t now = tb_timer_now(impl);

        // timeout?
        if (task_impl->when <= now)
        {
            // pop it
            tb_heap_pop(impl->heap);

            // save func and data for calling it later
            func = task_impl->func;
            priv = task_impl->priv;

            // killed?
            killed = task_impl->killed? tb_true : tb_false;

            // repeat?
            if (task_impl->repeat)
            {
                // update when
                task_impl->when = now + task_impl->period;

                // continue task_impl
                tb_heap_put(impl->heap, task_impl);
            }
            else 
            {
                // refn--
                if (task_impl->refn > 1) task_impl->refn--;
                // remove it from pool directly
                else tb_fixed_pool_free(impl->pool, task_impl);
            }
        }

        // ok
        ok = tb_true;

    } while (0);

    // leave
    tb_spinlock_leave(&impl->lock);

    // done func
    if (func) func(killed, priv);

    // ok?
    return ok;
}
tb_void_t tb_timer_loop(tb_timer_ref_t timer)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return(impl);

    // work++
    tb_atomic_fetch_and_inc(&impl->work);

    // init event 
    tb_spinlock_enter(&impl->lock);
    if (!impl->event) impl->event = tb_event_init();
    tb_spinlock_leave(&impl->lock);

    // loop
    while (!tb_atomic_get(&impl->stop))
    {
        // the delay
        tb_size_t delay = tb_timer_delay(timer);
        if (delay)
        {
            // the event
            tb_spinlock_enter(&impl->lock);
            tb_event_ref_t event = impl->event;
            tb_spinlock_leave(&impl->lock);
            tb_check_break(event);

            // wait some time
            if (tb_event_wait(event, delay) < 0) break;
        }

        // spak ctime
        if (impl->ctime) tb_cache_time_spak();

        // spak it
        if (!tb_timer_spak(timer)) break;
    }

    // work--
    tb_atomic_fetch_and_dec(&impl->work);
}
tb_timer_task_ref_t tb_timer_task_init(tb_timer_ref_t timer, tb_size_t delay, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && func, tb_null);

    // add task_impl
    return tb_timer_task_init_at(timer, tb_timer_now(impl) + delay, delay, repeat, func, priv);
}
tb_timer_task_ref_t tb_timer_task_init_at(tb_timer_ref_t timer, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && impl->pool && impl->heap && func, tb_null);

    // stoped?
    tb_assert_and_check_return_val(!tb_atomic_get(&impl->stop), tb_null);

    // enter
    tb_spinlock_enter(&impl->lock);

    // make task
    tb_event_ref_t          event = tb_null;
    tb_hize_t               when_top = -1;
    tb_timer_task_impl_t*   task_impl = (tb_timer_task_impl_t*)tb_fixed_pool_malloc0(impl->pool);
    if (task_impl)
    {
        // the top when 
        if (tb_heap_size(impl->heap))
        {
            tb_timer_task_impl_t* task_impl = (tb_timer_task_impl_t*)tb_heap_top(impl->heap);
            if (task_impl) when_top = task_impl->when;
        }

        // init task
        task_impl->refn      = 2;
        task_impl->func      = func;
        task_impl->priv      = priv;
        task_impl->when      = when;
        task_impl->period    = period;
        task_impl->repeat    = repeat? 1 : 0;

        // add task
        tb_heap_put(impl->heap, task_impl);

        // the event
        event = impl->event;
    }

    // leave
    tb_spinlock_leave(&impl->lock);

    // post event if the top task is changed
    if (event && task_impl && when < when_top)
        tb_event_post(event);

    // ok?
    return (tb_timer_task_ref_t)task_impl;
}
tb_timer_task_ref_t tb_timer_task_init_after(tb_timer_ref_t timer, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && func, tb_null);

    // add task
    return tb_timer_task_init_at(timer, tb_timer_now(impl) + after, period, repeat, func, priv);
}
tb_void_t tb_timer_task_post(tb_timer_ref_t timer, tb_size_t delay, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return(impl && func);

    // run task
    tb_timer_task_post_at(timer, tb_timer_now(impl) + delay, delay, repeat, func, priv);
}
tb_void_t tb_timer_task_post_at(tb_timer_ref_t timer, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return(impl && impl->pool && impl->heap && func);

    // stoped?
    tb_assert_and_check_return(!tb_atomic_get(&impl->stop));

    // enter
    tb_spinlock_enter(&impl->lock);

    // make task
    tb_event_ref_t          event = tb_null;
    tb_hize_t               when_top = -1;
    tb_timer_task_impl_t*   task_impl = (tb_timer_task_impl_t*)tb_fixed_pool_malloc0(impl->pool);
    if (task_impl)
    {
        // the top when 
        if (tb_heap_size(impl->heap))
        {
            tb_timer_task_impl_t* task_impl = (tb_timer_task_impl_t*)tb_heap_top(impl->heap);
            if (task_impl) when_top = task_impl->when;
        }

        // init task
        task_impl->refn      = 1;
        task_impl->func      = func;
        task_impl->priv      = priv;
        task_impl->when      = when;
        task_impl->period    = period;
        task_impl->repeat    = repeat? 1 : 0;

        // add task
        tb_heap_put(impl->heap, task_impl);

        // the event
        event = impl->event;
    }

    // leave
    tb_spinlock_leave(&impl->lock);

    // post event if the top task is changed
    if (event && task_impl && when < when_top)
        tb_event_post(event);
}
tb_void_t tb_timer_task_post_after(tb_timer_ref_t timer, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_timer_impl_t* impl = (tb_timer_impl_t*)timer;
    tb_assert_and_check_return(impl && func);

    // run task 
    tb_timer_task_post_at(timer, tb_timer_now(impl) + after, period, repeat, func, priv);
}
tb_void_t tb_timer_task_exit(tb_timer_ref_t timer, tb_timer_task_ref_t task)
{
    // check
    tb_timer_impl_t*         impl = (tb_timer_impl_t*)timer;
    tb_timer_task_impl_t*    task_impl = (tb_timer_task_impl_t*)task;
    tb_assert_and_check_return(impl && impl->pool && task_impl);

    // trace
    tb_trace_d("exit: when: %lld, period: %u, refn: %u", task_impl->when, task_impl->period, task_impl->refn);

    // enter
    tb_spinlock_enter(&impl->lock);

    // remove it?
    if (task_impl->refn > 1)
    {
        // refn--
        task_impl->refn--;

        // cancel task 
        task_impl->func      = tb_null;
        task_impl->priv      = tb_null;
        task_impl->repeat    = 0;
    }
    // remove it from pool directly if the task_impl have been expired 
    else tb_fixed_pool_free(impl->pool, task_impl);

    // leave
    tb_spinlock_leave(&impl->lock);
}
tb_void_t tb_timer_task_kill(tb_timer_ref_t timer, tb_timer_task_ref_t task)
{
    // check
    tb_timer_impl_t*        impl = (tb_timer_impl_t*)timer;
    tb_timer_task_impl_t*   task_impl = (tb_timer_task_impl_t*)task;
    tb_assert_and_check_return(impl && impl->pool && task_impl);

    // trace
    tb_trace_d("kill: when: %lld, period: %u, refn: %u", task_impl->when, task_impl->period, task_impl->refn);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    do
    {
        // expired or removed?
        tb_check_break(task_impl->refn == 2);

        // find it
        tb_size_t itor = tb_find_all(impl->heap, task_impl, tb_timer_comp_by_task);
        tb_assert_and_check_break(itor != tb_iterator_tail(impl->heap));

        // del this task_impl
        tb_heap_del(impl->heap, itor);

        // killed
        task_impl->killed = 1;

        // no repeat
        task_impl->repeat = 0;
                
        // modify when => now
        task_impl->when = tb_timer_now(impl);

        // re-add task_impl
        tb_heap_put(impl->heap, task_impl);

    } while (0);

    // leave
    tb_spinlock_leave(&impl->lock);
}
