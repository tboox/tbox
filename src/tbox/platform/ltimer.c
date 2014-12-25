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
 * @file        ltimer.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "ltimer"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "platform.h"
#include "../memory/memory.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the timer wheel maxn
#ifdef __tb_small__
#   define TB_LTIMER_WHEEL_MAXN             (4096)
#else
#   define TB_LTIMER_WHEEL_MAXN             (8192)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the timer task impl type
typedef struct __tb_ltimer_task_impl_t
{
    // the func
    tb_ltimer_task_func_t        func;

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

    // the wheel index
    tb_uint32_t                 windx;

}tb_ltimer_task_impl_t;

/*! the timer impl type
 *
 * <pre>
 *
 * tick: 1s
 *
 *           1s    1s   ..
 * wheel: |-----|-----|-----|-----|-----|-----|---- ... -----|  <= circle queue
 *                             timers
 *                 btime ==================> now
 *                       |     |     |
 *                       |     |     |
 *                       |     |     | => the same timeout task_impl list (vector)
 *                                   |
 *
 * </pre>
 */
typedef struct __tb_ltimer_impl_t
{
    // the maxn
    tb_size_t                   maxn;

    // is stoped?
    tb_atomic_t                 stop;

    // is worked?
    tb_atomic_t                 work;

    // the base time
    tb_hong_t                   btime;

    // cache time?
    tb_bool_t                   ctime;

    // the tick
    tb_size_t                   tick;

    // the lock
    tb_spinlock_t               lock;

    // the pool
    tb_fixed_pool_ref_t         pool;

    // the expired tasks
    tb_vector_ref_t             expired;

    // the wheel
    tb_vector_ref_t             wheel[TB_LTIMER_WHEEL_MAXN];

    // the wheel base
    tb_size_t                   wbase;

}tb_ltimer_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_hong_t tb_ltimer_now(tb_ltimer_impl_t* impl)
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
static tb_bool_t tb_ltimer_add_task(tb_ltimer_impl_t* impl, tb_ltimer_task_impl_t* task_impl)
{
    // check
    tb_assert_and_check_return_val(impl && impl->pool && impl->tick, tb_false);
    tb_assert_and_check_return_val(task_impl && task_impl->func && task_impl->refn && task_impl->when, tb_false);

    // trace
    tb_trace_d("add: when: %lld, period: %u, refn: %u", task_impl->when, task_impl->period, task_impl->refn);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // empty? move to the wheel head
        if (!tb_fixed_pool_size(impl->pool)) 
        {
            impl->btime = tb_ltimer_now(impl);
            impl->wbase = 0;
        }
        tb_trace_d("add: btime: %lld, wbase: %lu", impl->btime, impl->wbase);

        // the timer difference
        tb_hong_t tdiff = task_impl->when - impl->btime;
        tb_assert_and_check_break(tdiff >= 0);
        tb_trace_d("add: tdiff: %lld", tdiff);

        // the wheel difference
        tb_size_t wdiff = (tb_size_t)(tdiff / impl->tick);
        tb_assert_and_check_break(wdiff < TB_LTIMER_WHEEL_MAXN);
        tb_trace_d("add: wdiff: %lu", wdiff);

        // the wheel index
        tb_size_t windx = (impl->wbase + wdiff) & (TB_LTIMER_WHEEL_MAXN - 1);
        tb_trace_d("add: windx: %lu", windx);

        // the wheel list
        tb_vector_ref_t wlist = impl->wheel[windx];
        if (!wlist) wlist = impl->wheel[windx] = tb_vector_init((impl->maxn / TB_LTIMER_WHEEL_MAXN) + 8, tb_item_func_ptr(tb_null, tb_null));
        tb_assert_and_check_break(wlist);

        // save the wheel index
        task_impl->windx = windx;

        // add task to the wheel list
        tb_vector_insert_tail(wlist, task_impl);

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
static tb_bool_t tb_ltimer_del_task(tb_ltimer_impl_t* impl, tb_ltimer_task_impl_t* task_impl)
{
    // check
    tb_assert_and_check_return_val(impl && impl->pool && impl->tick, tb_false);
    tb_assert_and_check_return_val(task_impl && task_impl->func && task_impl->refn && task_impl->when, tb_false);

    // trace
    tb_trace_d("del: when: %lld, period: %u, refn: %u", task_impl->when, task_impl->period, task_impl->refn);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // check
        tb_assert_and_check_break(task_impl->windx != -1 && task_impl->windx < tb_arrayn(impl->wheel));

        // the wheel list
        tb_vector_ref_t wlist = impl->wheel[task_impl->windx];
        tb_assert_and_check_break(wlist);

        // find the task from the wheel list
        tb_size_t itor = tb_find_all(wlist, task_impl);
        tb_assert_and_check_break(itor != tb_iterator_tail(wlist));

        // del the task from the wheel list
        tb_vector_remove(wlist, itor);

        // clear the wheel index
        task_impl->windx = -1;

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
static tb_bool_t tb_ltimer_expired_init(tb_iterator_ref_t iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)priv;
    tb_assert_and_check_return_val(impl && impl->expired, tb_false);

    // the task
    tb_ltimer_task_impl_t const* task_impl = (tb_ltimer_task_impl_t const*)item;
    if (task_impl)
    {
        // check refn
        tb_assert(task_impl->refn);

        // expired 
        tb_vector_insert_tail(impl->expired, task_impl);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_ltimer_expired_done(tb_iterator_ref_t iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    // the task
    tb_ltimer_task_impl_t const* task_impl = (tb_ltimer_task_impl_t const*)item;
    
    // done func
    if (task_impl && task_impl->func) 
    { 
        // trace
        tb_trace_d("done: expired: when: %lld, period: %u, refn: %u, killed: %u", task_impl->when, task_impl->period, task_impl->refn, task_impl->killed);

        // done
        task_impl->func(task_impl->killed? tb_true : tb_false, task_impl->priv);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_ltimer_expired_exit(tb_iterator_ref_t iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_ltimer_impl_t*   impl = priv? (tb_ltimer_impl_t*)(((tb_pointer_t*)priv)[0]) : tb_null;
    tb_hong_t*          now = priv? (tb_hong_t*)(((tb_pointer_t*)priv)[1]) : tb_null;
    tb_assert_and_check_return_val(impl && now, tb_false);

    // the task
    tb_ltimer_task_impl_t* task_impl = (tb_ltimer_task_impl_t*)item;
    if (task_impl)
    {
        // repeat?
        if (task_impl->repeat)
        {
            // update when
            task_impl->when = *now + task_impl->period;

            // continue the task
            if (!tb_ltimer_add_task(impl, task_impl))
            {
                // trace
                tb_trace_e("continue to add task_impl failed");
            }
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
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_ltimer_ref_t tb_ltimer_init(tb_size_t maxn, tb_size_t tick, tb_bool_t ctime)
{
    // check
    tb_assert_and_check_return_val(tick >= TB_LTIMER_TICK_100MS, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_ltimer_impl_t*   impl = tb_null;
    do
    {
        // make timer
        impl = tb_malloc0_type(tb_ltimer_impl_t);
        tb_assert_and_check_break(impl);

        // init timer
        impl->maxn         = tb_max(maxn, 16);
        impl->ctime        = ctime;
        impl->tick         = tick;
        impl->btime        = tb_ltimer_now(impl);

        // init lock
        if (!tb_spinlock_init(&impl->lock)) break;

        // init pool
        impl->pool         = tb_fixed_pool_init(tb_null, (maxn >> 4) + 16, sizeof(tb_ltimer_task_impl_t), tb_null, tb_null, tb_null);
        tb_assert_and_check_break(impl->pool);

        // init the expired tasks
        impl->expired      = tb_vector_init((maxn / TB_LTIMER_WHEEL_MAXN) + 8, tb_item_func_ptr(tb_null, tb_null));
        tb_assert_and_check_break(impl->expired);

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
        if (impl) tb_ltimer_exit((tb_ltimer_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_ltimer_ref_t)impl;
}
tb_void_t tb_ltimer_exit(tb_ltimer_ref_t timer)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return(timer);

    // stop it
    tb_atomic_set(&impl->stop, 1);

    // wait loop exit
    tb_size_t tryn = 10;
    while (tb_atomic_get(&impl->work) && tryn--) tb_msleep(500);

    // warning
    if (!tryn && tb_atomic_get(&impl->work))
    {
        tb_trace_w("[ltimer]: the loop has been not exited now!");
    }

    // enter
    tb_spinlock_enter(&impl->lock);

    // exit wheel
    {
        tb_size_t i = 0;
        for (i = 0; i < TB_LTIMER_WHEEL_MAXN; i++)
        {
            if (impl->wheel[i]) tb_vector_exit(impl->wheel[i]);
            impl->wheel[i] = tb_null;
        }
    }

    // exit pool
    if (impl->pool) tb_fixed_pool_exit(impl->pool);
    impl->pool = tb_null;

    // leave
    tb_spinlock_leave(&impl->lock);

    // exit the expired tasks
    if (impl->expired) tb_vector_exit(impl->expired);
    impl->expired = tb_null;

    // exit lock
    tb_spinlock_exit(&impl->lock);

    // exit it
    tb_free(impl);
}
tb_void_t tb_ltimer_clear(tb_ltimer_ref_t timer)
{
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    if (impl)
    {
        // enter
        tb_spinlock_enter(&impl->lock);

        // move to the wheel head
        impl->btime = tb_ltimer_now(impl);
        impl->wbase = 0;

        // clear wheel
        {
            tb_size_t i = 0;
            for (i = 0; i < TB_LTIMER_WHEEL_MAXN; i++)
            {
                if (impl->wheel[i]) tb_vector_clear(impl->wheel[i]);
            }
        }

        // clear pool
        if (impl->pool) tb_fixed_pool_clear(impl->pool);

        // leave
        tb_spinlock_leave(&impl->lock);
    }
}
tb_size_t tb_ltimer_limit(tb_ltimer_ref_t timer)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return_val(impl, 0);

    // the timer limit
    return (TB_LTIMER_WHEEL_MAXN * impl->tick);
}
tb_size_t tb_ltimer_delay(tb_ltimer_ref_t timer)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return_val(impl, -1);

    // ok?
    return impl->tick;
}
tb_bool_t tb_ltimer_spak(tb_ltimer_ref_t timer)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && impl->pool && impl->tick && impl->expired, tb_false);

    // stoped?
    tb_check_return_val(!tb_atomic_get(&impl->stop), tb_false);

    // the now time
    tb_hong_t now = tb_ltimer_now(impl);

    // clear expired
    tb_vector_clear(impl->expired);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // empty? move to the wheel head
        if (!tb_fixed_pool_size(impl->pool))
        {
            impl->btime = now;
            impl->wbase = 0;
            ok = tb_true;
            break;
        }

        // the diff
        tb_size_t diff = (tb_size_t)((now - impl->btime) / impl->tick);

        // trace
        tb_trace_d("spak: btime: %lld, wbase: %lu, now: %lld, diff: %lu", impl->btime, impl->wbase, now, diff);

        // walk the expired lists
        tb_size_t i = 0;
        tb_size_t n = tb_min(diff, TB_LTIMER_WHEEL_MAXN - 1);
        for (i = 0; i <= n; i++)
        {
            // the wheel index
            tb_size_t indx = (impl->wbase + i) & (TB_LTIMER_WHEEL_MAXN - 1);

            // the wheel list
            tb_vector_ref_t list = impl->wheel[indx];
            tb_check_continue(list && tb_vector_size(list));

            // init the expired task 
            tb_walk_all(list, tb_ltimer_expired_init, timer);

            // clear the wheel list
            tb_vector_clear(list);
        }

        // update the base time
        impl->btime = now;

        // update the wheel base
        impl->wbase = (impl->wbase + diff) & (TB_LTIMER_WHEEL_MAXN - 1);
        
        // ok
        ok = tb_true;

    } while (0);

    // leave
    tb_spinlock_leave(&impl->lock);

    // ok? and exists expired task_impl?
    if (ok && tb_vector_size(impl->expired))
    {
        // done the expired task 
        tb_walk_all(impl->expired, tb_ltimer_expired_done, tb_null);

        // enter
        tb_spinlock_enter(&impl->lock);

        // exit the expired task
        tb_pointer_t data[2]; data[0] = timer; data[1] = &now;
        tb_walk_all(impl->expired, tb_ltimer_expired_exit, data);

        // leave
        tb_spinlock_leave(&impl->lock);
    }

    // ok?
    return ok;
}
tb_void_t tb_ltimer_loop(tb_ltimer_ref_t timer)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return(impl);

    // work++
    tb_atomic_fetch_and_inc(&impl->work);

    // loop
    while (!tb_atomic_get(&impl->stop))
    {
        // the delay
        tb_size_t delay = tb_ltimer_delay(timer);
        tb_assert_and_check_break(delay != -1);

        // wait some time
        if (delay) tb_msleep(delay);

        // spak ctime
        if (impl->ctime) tb_cache_time_spak();

        // spak it
        if (!tb_ltimer_spak(timer)) break;
    }

    // work--
    tb_atomic_fetch_and_dec(&impl->work);
}
tb_ltimer_task_ref_t tb_ltimer_task_init(tb_ltimer_ref_t timer, tb_size_t delay, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && func, tb_null);

    // add task
    return tb_ltimer_task_init_at(timer, tb_ltimer_now(impl) + delay, delay, repeat, func, priv);
}
tb_ltimer_task_ref_t tb_ltimer_task_init_at(tb_ltimer_ref_t timer, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && impl->pool && func, tb_null);

    // stoped?
    tb_assert_and_check_return_val(!tb_atomic_get(&impl->stop), tb_null);

    // enter
    tb_spinlock_enter(&impl->lock);

    // make task
    tb_ltimer_task_impl_t* task_impl = (tb_ltimer_task_impl_t*)tb_fixed_pool_malloc0(impl->pool);
    if (task_impl)
    {
        // init task
        task_impl->refn      = 2;
        task_impl->func      = func;
        task_impl->priv      = priv;
        task_impl->when      = when;
        task_impl->period    = period;
        task_impl->repeat    = repeat? 1 : 0;
        task_impl->killed    = 0;
        task_impl->windx     = -1;

        // add task_impl
        if (!tb_ltimer_add_task(impl, task_impl))
        {
            tb_fixed_pool_free(impl->pool, task_impl);
            task_impl = tb_null;
        }
    }

    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return (tb_ltimer_task_ref_t)task_impl;
}
tb_ltimer_task_ref_t tb_ltimer_task_init_after(tb_ltimer_ref_t timer, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return_val(impl && func, tb_null);

    // add task
    return tb_ltimer_task_init_at(timer, tb_ltimer_now(impl) + after, period, repeat, func, priv);
}
tb_void_t tb_ltimer_task_post(tb_ltimer_ref_t timer, tb_size_t delay, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return(impl && func);

    // run task
    tb_ltimer_task_post_at(timer, tb_ltimer_now(impl) + delay, delay, repeat, func, priv);
}
tb_void_t tb_ltimer_task_post_at(tb_ltimer_ref_t timer, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return(impl && impl->pool && func);

    // stoped?
    tb_assert_and_check_return(!tb_atomic_get(&impl->stop));

    // enter
    tb_spinlock_enter(&impl->lock);

    // make task
    tb_ltimer_task_impl_t* task_impl = (tb_ltimer_task_impl_t*)tb_fixed_pool_malloc0(impl->pool);
    if (task_impl)
    {
        // init task
        task_impl->refn      = 1;
        task_impl->func      = func;
        task_impl->priv      = priv;
        task_impl->when      = when;
        task_impl->period    = period;
        task_impl->repeat    = repeat? 1 : 0;
        task_impl->killed    = 0;
        task_impl->windx     = -1;

        // add task
        if (!tb_ltimer_add_task(impl, task_impl))
            tb_fixed_pool_free(impl->pool, task_impl);
    }

    // leave
    tb_spinlock_leave(&impl->lock);
}
tb_void_t tb_ltimer_task_post_after(tb_ltimer_ref_t timer, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_ltimer_impl_t* impl = (tb_ltimer_impl_t*)timer;
    tb_assert_and_check_return(impl && func);

    // run task
    tb_ltimer_task_post_at(timer, tb_ltimer_now(impl) + after, period, repeat, func, priv);
}
tb_void_t tb_ltimer_task_exit(tb_ltimer_ref_t timer, tb_ltimer_task_ref_t task)
{
    // check
    tb_ltimer_impl_t*        impl = (tb_ltimer_impl_t*)timer;
    tb_ltimer_task_impl_t*   task_impl = (tb_ltimer_task_impl_t*)task;
    tb_assert_and_check_return(impl && impl->pool && task_impl);

    // trace
    tb_trace_d("exit: when: %lld, period: %u, refn: %u", task_impl->when, task_impl->period, task_impl->refn);

    // enter
    tb_spinlock_enter(&impl->lock);

    if (task_impl->refn > 1)
    {
        // refn--
        task_impl->refn--;

        // cancel task
        task_impl->func      = tb_null;
        task_impl->priv      = tb_null;
        task_impl->repeat    = 0;
    }
    // remove it from pool directly if the task have been expired 
    else tb_fixed_pool_free(impl->pool, task_impl);

    // leave
    tb_spinlock_leave(&impl->lock);
}
tb_void_t tb_ltimer_task_kill(tb_ltimer_ref_t timer, tb_ltimer_task_ref_t task)
{
    // check
    tb_ltimer_impl_t*        impl = (tb_ltimer_impl_t*)timer;
    tb_ltimer_task_impl_t*   task_impl = (tb_ltimer_task_impl_t*)task;
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

        // del the task first
        if (!tb_ltimer_del_task(impl, task_impl))
        {
            // trace
            tb_trace_e("del task_impl failed");
            break;
        }

        // killed
        task_impl->killed = 1;

        // no repeat
        task_impl->repeat = 0;
                
        // modify when => now
        task_impl->when = tb_ltimer_now(impl);

        // re-add task
        if (!tb_ltimer_add_task(impl, task_impl))
        {
            // trace
            tb_trace_e("re-add task_impl failed");
            break;
        }

    } while (0);

    // leave
    tb_spinlock_leave(&impl->lock);
}

