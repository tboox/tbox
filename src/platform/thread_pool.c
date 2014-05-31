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
 * @file        thread_pool.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "thread_pool"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "platform.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the worker maxn
#ifdef __tb_small__
#   define TB_THREAD_POOL_WORKER_MAXN           (32)
#else
#   define TB_THREAD_POOL_WORKER_MAXN           (64)
#endif

// the jobs grow
#ifdef __tb_small__
#   define TB_THREAD_POOL_JOBS_POOL_GROW        (256)
#else
#   define TB_THREAD_POOL_JOBS_POOL_GROW        (512)
#endif

// the urgent jobs grow
#ifdef __tb_small__
#   define TB_THREAD_POOL_JOBS_URGENT_GROW      (32)
#else
#   define TB_THREAD_POOL_JOBS_URGENT_GROW      (64)
#endif

// the waiting jobs grow
#ifdef __tb_small__
#   define TB_THREAD_POOL_JOBS_WAITING_GROW     (128)
#else
#   define TB_THREAD_POOL_JOBS_WAITING_GROW     (256)
#endif

// the pending jobs grow
#ifdef __tb_small__
#   define TB_THREAD_POOL_JOBS_PENDING_GROW     (128)
#else
#   define TB_THREAD_POOL_JOBS_PENDING_GROW     (256)
#endif

// the working jobs grow
#ifdef __tb_small__
#   define TB_THREAD_POOL_JOBS_WORKING_GROW     (32)
#else
#   define TB_THREAD_POOL_JOBS_WORKING_GROW     (64)
#endif

// the jobs waiting maxn
#ifdef __tb_small__
#   define TB_THREAD_POOL_JOBS_WAITING_MAXN     (1 << 16)
#else
#   define TB_THREAD_POOL_JOBS_WAITING_MAXN     (1 << 20)
#endif

// the pull jobs time maxn
#ifdef __tb_small__
#   define TB_THREAD_POOL_JOBS_PULL_TIME_MAXN   (10000)
#else
#   define TB_THREAD_POOL_JOBS_PULL_TIME_MAXN   (20000)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the thread pool job type
typedef struct __tb_thread_pool_job_t
{
    // the task
    tb_thread_pool_task_t   task;

    // the reference count, must be <= 2
    tb_atomic_t             refn;

    /* the state
     *
     * TB_STATE_KILLED
     * TB_STATE_WAITING
     * TB_STATE_WORKING
     * TB_STATE_KILLING
     * TB_STATE_FINISHED
     */
    tb_atomic_t             state;

}tb_thread_pool_job_t;

// the thread pool job stats type
typedef struct __tb_thread_pool_job_stats_t
{
    // the done count
    tb_size_t               done_count;

    // the done total time
    tb_hize_t               total_time;

}tb_thread_pool_job_stats_t;

// the thread pool worker type
typedef struct __tb_thread_pool_worker_t
{
    // the worker id
    tb_size_t               id;

    // the thread pool handle
    tb_handle_t             pool;

    // the loop
    tb_handle_t             loop;

    // the jobs
    tb_vector_t*            jobs;

    // the pull time
    tb_size_t               pull;

    // the stats
    tb_hash_t*              stats;

    // is stoped?
    tb_atomic_t             bstoped;

}tb_thread_pool_worker_t;

// the thread pool type
typedef struct __tb_thread_pool_t
{
    // the thread stack size
    tb_size_t               stack;

    // the worker maxn
    tb_size_t               worker_maxn;

    // the lock
    tb_spinlock_t           lock;

    // the jobs pool
    tb_handle_t             jobs_pool;

    // the urgent jobs
    tb_single_list_t*       jobs_urgent;
    
    // the waiting jobs
    tb_single_list_t*       jobs_waiting;
    
    // the pending jobs
    tb_list_t*              jobs_pending;

    // is stoped
    tb_bool_t               bstoped;

    // the semaphore
    tb_handle_t             semaphore;
    
    // the worker size
    tb_size_t               worker_size;

    // the worker list
    tb_thread_pool_worker_t worker_list[TB_THREAD_POOL_WORKER_MAXN];

}tb_thread_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_thread_pool_instance_init(tb_cpointer_t* ppriv)
{
    // init it
    return tb_thread_pool_init(0, 0);
}
static tb_void_t tb_thread_pool_instance_exit(tb_handle_t handle, tb_cpointer_t priv)
{
    // exit it
    tb_thread_pool_exit(handle);
}
static tb_void_t tb_thread_pool_instance_kill(tb_handle_t handle, tb_cpointer_t priv)
{
    // dump it
#ifdef __tb_debug__
    tb_thread_pool_dump(handle);
#endif

    // kill it
    tb_thread_pool_kill(handle);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * worker implementation
 */
static tb_bool_t tb_thread_pool_worker_walk_pull(tb_single_list_t* jobs, tb_pointer_t item, tb_bool_t* bdel, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(jobs && bdel, tb_false);

    // the worker pull
    tb_thread_pool_worker_t* worker = (tb_thread_pool_worker_t*)priv;
    tb_assert_and_check_return_val(worker && worker->jobs && worker->stats, tb_false);

    // full?
    tb_check_return_val(worker->pull < TB_THREAD_POOL_JOBS_PULL_TIME_MAXN, tb_false);

    // the job
    tb_thread_pool_job_t* job = (tb_thread_pool_job_t*)item;
    tb_assert_and_check_return_val(job, tb_false);

    // the pool
    tb_thread_pool_t* pool = (tb_thread_pool_t*)worker->pool;
    tb_assert_and_check_return_val(pool && pool->jobs_pending, tb_false);

    // append the job to the pending jobs
    tb_size_t itor = tb_list_insert_tail(pool->jobs_pending, job);  
    tb_assert_and_check_return_val(itor != tb_iterator_tail(pool->jobs_pending), tb_false);

    // append the job to the working jobs
    tb_vector_insert_tail(worker->jobs, job);   

    // computate the job average time 
    tb_size_t average_time = 200;
    if (tb_hash_size(worker->stats))
    {
        tb_thread_pool_job_stats_t* stats = tb_hash_get(worker->stats, job->task.done);
        if (stats && stats->done_count) average_time = (tb_size_t)(stats->total_time / stats->done_count);
    }

    // update the pull time
    worker->pull += average_time;

    // remove the job from the waiting or urgent jobs
    *bdel = tb_true;
                
    // trace
    tb_trace_d("worker[%lu]: pull: task[%p:%s] from %s", worker->id, job->task.done, job->task.name, jobs == pool->jobs_waiting? "waiting" : "urgent");

    // ok
    return tb_true;
}
static tb_bool_t tb_thread_pool_worker_walk_pull_and_clean(tb_list_t* jobs, tb_pointer_t item, tb_bool_t* bdel, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(jobs && bdel, tb_false);

    // the worker pull
    tb_thread_pool_worker_t* worker = (tb_thread_pool_worker_t*)priv;
    tb_assert_and_check_return_val(worker && worker->jobs && worker->stats, tb_false);

    // the job
    tb_thread_pool_job_t* job = (tb_thread_pool_job_t*)item;
    tb_assert_and_check_return_val(job, tb_false);

    // the job state
    tb_size_t state = tb_atomic_get(&job->state);

    // waiting and non-full? pull it
    if (state == TB_STATE_WAITING && worker->pull < TB_THREAD_POOL_JOBS_PULL_TIME_MAXN)
    {
        // append the job to the working jobs
        tb_vector_insert_tail(worker->jobs, job);   

        // computate the job average time 
        tb_size_t average_time = 200;
        if (tb_hash_size(worker->stats))
        {
            tb_thread_pool_job_stats_t* stats = tb_hash_get(worker->stats, job->task.done);
            if (stats && stats->done_count) average_time = (tb_size_t)(stats->total_time / stats->done_count);
        }

        // update the pull time
        worker->pull += average_time;

        // trace
        tb_trace_d("worker[%lu]: pull: task[%p:%s] from pending", worker->id, job->task.done, job->task.name);
    }
    // finished or killed? remove it
    else if (state == TB_STATE_FINISHED || state == TB_STATE_KILLED)
    {
        // trace
        tb_trace_d("worker[%lu]: remove: task[%p:%s] from pending", worker->id, job->task.done, job->task.name);

        // exit the job
        if (job->task.exit) job->task.exit(job->task.priv);

        // remove it from the waiting or urgent jobs
        *bdel = tb_true;

        // refn--
        if (job->refn > 1) job->refn--;
        // remove it from pool directly
        else 
        {
            // the pool
            tb_thread_pool_t* pool = (tb_thread_pool_t*)worker->pool;
            tb_assert_and_check_return_val(pool, tb_false);

            // remove it from the jobs pool
            tb_fixed_pool_free(pool->jobs_pool, job);
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_thread_pool_worker_walk_clean(tb_list_t* jobs, tb_pointer_t item, tb_bool_t* bdel, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(jobs && bdel, tb_false);

    // the worker pull
    tb_thread_pool_worker_t* worker = (tb_thread_pool_worker_t*)priv;
    tb_assert_and_check_return_val(worker && worker->jobs, tb_false);

    // the job
    tb_thread_pool_job_t* job = (tb_thread_pool_job_t*)item;
    tb_assert_and_check_return_val(job, tb_false);

    // the job state
    tb_size_t state = tb_atomic_get(&job->state);

    // finished or killed? remove it
    if (state == TB_STATE_FINISHED || state == TB_STATE_KILLED)
    {
        // trace
        tb_trace_d("worker[%lu]: remove: task[%p:%s] from pending", worker->id, job->task.done, job->task.name);

        // exit the job
        if (job->task.exit) job->task.exit(job->task.priv);

        // remove it from the pending jobs
        *bdel = tb_true;

        // refn--
        if (job->refn > 1) job->refn--;
        // remove it from pool directly
        else 
        {
            // the pool
            tb_thread_pool_t* pool = (tb_thread_pool_t*)worker->pool;
            tb_assert_and_check_return_val(pool, tb_false);

            // remove it from the jobs pool
            tb_fixed_pool_free(pool->jobs_pool, job);
        }
    }

    // ok
    return tb_true;
}
static tb_void_t tb_thread_pool_worker_post(tb_thread_pool_t* pool, tb_size_t post)
{
    // check
    tb_assert_and_check_return(pool && pool->semaphore);

    // the semaphore value
    tb_long_t value = tb_semaphore_value(pool->semaphore);

    // post wait
    if (value >= 0 && value < post) 
        tb_semaphore_post(pool->semaphore, post - value);
}
static tb_pointer_t tb_thread_pool_worker_loop(tb_cpointer_t priv)
{
    // the worker
    tb_thread_pool_worker_t* worker = (tb_thread_pool_worker_t*)priv;

    // trace
    tb_trace_d("worker[%lu]: init", worker? worker->id : -1);

    // done
    do
    {
        // check
        tb_assert_and_check_break(worker && !worker->jobs && !worker->stats);

        // the pool
        tb_thread_pool_t* pool = (tb_thread_pool_t*)worker->pool;
        tb_assert_and_check_break(pool && pool->semaphore);

        // wait some time for leaving the lock
        tb_msleep((worker->id + 1)* 20);

        // init jobs
        worker->jobs = tb_vector_init(TB_THREAD_POOL_JOBS_WORKING_GROW, tb_item_func_ptr(tb_null, tb_null));
        tb_assert_and_check_break(worker->jobs);

        // init stats
        worker->stats = tb_hash_init(TB_HASH_BULK_SIZE_MICRO, tb_item_func_ptr(tb_null, tb_null), tb_item_func_mem(sizeof(tb_thread_pool_job_stats_t), tb_null, tb_null));
        tb_assert_and_check_break(worker->stats);
        
        // loop
        while (1)
        {
            // pull jobs if be idle
            if (!tb_vector_size(worker->jobs))
            {
                // enter 
                tb_spinlock_enter(&pool->lock);

                // init the pull time
                worker->pull = 0;

                // pull from the urgent jobs
                tb_size_t jobs_urgent_size = 0;
                if (pool->jobs_urgent && tb_single_list_size(pool->jobs_urgent))
                {
                    // trace
                    tb_trace_d("worker[%lu]: try pulling from urgent: %lu", worker->id, tb_single_list_size(pool->jobs_urgent));

                    // pull it
                    tb_single_list_walk(pool->jobs_urgent, tb_thread_pool_worker_walk_pull, worker);

                    // update the jobs urgent size
                    jobs_urgent_size = tb_single_list_size(pool->jobs_urgent);
                }

                // pull from the waiting jobs
                tb_size_t jobs_waiting_size = 0;
                if (pool->jobs_waiting && tb_single_list_size(pool->jobs_waiting))
                {
                    // trace
                    tb_trace_d("worker[%lu]: try pulling from waiting: %lu", worker->id, tb_single_list_size(pool->jobs_waiting));

                    // pull it
                    tb_single_list_walk(pool->jobs_waiting, tb_thread_pool_worker_walk_pull, worker);

                    // update the jobs waiting size
                    jobs_waiting_size = tb_single_list_size(pool->jobs_waiting);
                }

                // pull from the pending jobs and clean some finished and killed jobs
                tb_size_t jobs_pending_size = 0;
                if (pool->jobs_pending && tb_list_size(pool->jobs_pending))
                {
                    // trace
                    tb_trace_d("worker[%lu]: try pulling from pending: %lu", worker->id, tb_list_size(pool->jobs_pending));

                    // no jobs? try to pull from the pending jobs
                    if (!tb_vector_size(worker->jobs))
                        tb_list_walk(pool->jobs_pending, tb_thread_pool_worker_walk_pull_and_clean, worker);
                    // clean some finished and killed jobs
                    else tb_list_walk(pool->jobs_pending, tb_thread_pool_worker_walk_clean, worker);

                    // update the jobs pending size
                    jobs_pending_size = tb_list_size(pool->jobs_pending);
                }

                // leave 
                tb_spinlock_leave(&pool->lock);

                // idle? wait it
                if (!tb_vector_size(worker->jobs))
                {
                    // killed?
                    tb_check_break(!tb_atomic_get(&worker->bstoped));

                    // trace
                    tb_trace_d("worker[%lu]: wait: ..", worker->id);

                    // wait some time
                    tb_long_t wait = tb_semaphore_wait(pool->semaphore, -1);
                    tb_assert_and_check_break(wait > 0);

                    // trace
                    tb_trace_d("worker[%lu]: wait: ok", worker->id);

                    // continue it
                    continue;
                }
                else
                {
                    // trace
                    tb_trace_d("worker[%lu]: pull: jobs: %lu, time: %lu ms, waiting: %lu, pending: %lu, urgent: %lu", worker->id, tb_vector_size(worker->jobs), worker->pull, jobs_waiting_size, jobs_pending_size, jobs_urgent_size);
                }
            }

            // done jobs
            tb_for_all (tb_thread_pool_job_t*, job, worker->jobs)
            {
                // check
                tb_assert_and_check_continue(job && job->task.done);

                // the job state
                tb_size_t state = tb_atomic_fetch_and_pset(&job->state, TB_STATE_WAITING, TB_STATE_WORKING);
                
                // the job is waiting? work it
                if (state == TB_STATE_WAITING)
                {
                    // trace
                    tb_trace_d("worker[%lu]: done: task[%p:%s]: ..", worker->id, job->task.done, job->task.name);

                    // init the time
                    tb_hong_t time = tb_cache_time_spak();

                    // done the job
                    job->task.done(job->task.priv);

                    // computate the time
                    time = tb_cache_time_spak() - time;

                    // update the stats
                    tb_size_t done_count = 1;
                    tb_hize_t total_time = time;
                    {
                        // exists? update time and count
                        tb_size_t               itor;
                        tb_hash_item_t const*   item = tb_null;
                        if (    ((itor = tb_hash_itor(worker->stats, job->task.done)) != tb_iterator_tail(worker->stats))
                            &&  (item = tb_iterator_item(worker->stats, itor)))
                        {
                            // the stats
                            tb_thread_pool_job_stats_t* stats = (tb_thread_pool_job_stats_t*)item->data;
                            tb_assert_and_check_break(stats);

                            // update the done count
                            stats->done_count++;

                            // update the total time 
                            stats->total_time += time;

                            // save them
                            done_count = stats->done_count;
                            total_time = stats->total_time;
                        }
                        
                        // no item? add it
                        if (!item) 
                        {
                            // init stats
                            tb_thread_pool_job_stats_t stats = {0};
                            stats.done_count = 1;
                            stats.total_time = time;

                            // add stats
                            tb_hash_set(worker->stats, job->task.done, &stats);
                        }
                    }

                    // trace
                    tb_trace_d("worker[%lu]: done: task[%p:%s]: time: %lld ms, average: %lld ms, count: %lu", worker->id, job->task.done, job->task.name, time, (total_time / (tb_hize_t)done_count), done_count);

                    // update the job state
                    tb_atomic_set(&job->state, TB_STATE_FINISHED);
                }
                // the job is killing? work it
                else if (state == TB_STATE_KILLING)
                {
                    // update the job state
                    tb_atomic_set(&job->state, TB_STATE_KILLED);
                }
            }

            // clear jobs
            tb_vector_clear(worker->jobs);
        }

    } while (0);

    // exit worker
    if (worker)
    {
        // trace
        tb_trace_d("worker[%lu]: exit", worker->id);

        // stoped
        tb_atomic_set(&worker->bstoped, 1);

        // exit stats
        if (worker->stats) tb_hash_exit(worker->stats);
        worker->stats = tb_null;

        // exit jobs
        if (worker->jobs) tb_vector_exit(worker->jobs);
        worker->jobs = tb_null;
    }

    // exit
    tb_thread_return(tb_null);
    return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * jobs implementation
 */
static tb_bool_t tb_thread_pool_jobs_walk_kill_all(tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_thread_pool_job_t* job = (tb_thread_pool_job_t*)item;
    tb_assert_and_check_return_val(job, tb_false);

    // trace
    tb_trace_d("task[%p:%s]: kill: ..", job->task.done, job->task.name);

    // kill it if be waiting
    tb_atomic_pset(&job->state, TB_STATE_WAITING, TB_STATE_KILLING);

    // ok
    return tb_true;
}
#ifdef __tb_debug__
static tb_bool_t tb_thread_pool_jobs_walk_dump_all(tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_thread_pool_job_t* job = (tb_thread_pool_job_t*)item;
    tb_assert_and_check_return_val(job, tb_false);

    // the state string
    static tb_char_t const* s_state_cstr[] = 
    {
        "waiting"
    ,   "working"
    ,   "killed"
    ,   "finished"
    };
    tb_assert_and_check_return_val(job->state < tb_arrayn(s_state_cstr), tb_false);

    // trace
    tb_trace_d("    task[%p:%s]: refn: %lu, state: %s", job->task.done, job->task.name, job->refn, s_state_cstr[job->state]);

    // ok
    return tb_true;
}
#endif
static tb_thread_pool_job_t* tb_thread_pool_jobs_post_task(tb_thread_pool_t* pool, tb_thread_pool_task_t const* task, tb_size_t* post_size)
{
    // check
    tb_assert_and_check_return_val(pool && task && task->done && post_size, tb_null);
    tb_assert_and_check_return_val(pool->jobs_waiting && pool->jobs_urgent, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_thread_pool_job_t*   job = tb_null;
    do
    {
        // check
        tb_assert_and_check_break(tb_single_list_size(pool->jobs_waiting) + tb_single_list_size(pool->jobs_urgent) + 1 < TB_THREAD_POOL_JOBS_WAITING_MAXN);

        // make job
        job = (tb_thread_pool_job_t*)tb_fixed_pool_malloc0(pool->jobs_pool);
        tb_assert_and_check_break(job);

        // init job
        job->refn   = 1;
        job->state  = TB_STATE_WAITING;
        job->task   = *task;

        // non-urgent job? 
        if (!task->urgent)
        {
            // post to the waiting jobs
            tb_size_t itor = tb_single_list_insert_tail(pool->jobs_waiting, job);
            tb_assert_and_check_break(itor != tb_iterator_tail(pool->jobs_waiting));
        }
        else
        {
            // post to the urgent jobs
            tb_size_t itor = tb_single_list_insert_tail(pool->jobs_urgent, job);
            tb_assert_and_check_break(itor != tb_iterator_tail(pool->jobs_urgent));
        }

        // the waiting jobs count
        tb_size_t jobs_waiting_count = tb_single_list_size(pool->jobs_waiting) + tb_single_list_size(pool->jobs_urgent);
        tb_assert_and_check_break(jobs_waiting_count);

        // update the post size
        if (*post_size < pool->worker_size) (*post_size)++;

        // trace
        tb_trace_d("task[%p:%s]: post: %lu: ..", task->done, task->name, *post_size);

        // init them if the workers have been not inited
        if (pool->worker_size < jobs_waiting_count)
        {
            tb_size_t i = pool->worker_size;
            tb_size_t n = tb_min(jobs_waiting_count, pool->worker_maxn);
            for (; i < n; i++)
            {
                // the worker 
                tb_thread_pool_worker_t* worker = &pool->worker_list[i];

                // init worker
                worker->id          = i;
                worker->pool        = pool;
                worker->bstoped     = 0;
                worker->jobs        = tb_null;
                worker->pull        = 0;
                worker->stats       = tb_null;
                worker->loop        = tb_thread_init(__tb_lstring__("thread_pool"), tb_thread_pool_worker_loop, worker, pool->stack);
                tb_assert_and_check_continue(worker->loop);
            }

            // update the worker size
            pool->worker_size = i;
        }

        // ok
        ok = tb_true;
    
    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        tb_fixed_pool_free(pool->jobs_pool, job);
        job = tb_null;
    }

    // ok?
    return job;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_thread_pool()
{
    return tb_singleton_instance(TB_SINGLETON_TYPE_THREAD_POOL, tb_thread_pool_instance_init, tb_thread_pool_instance_exit, tb_thread_pool_instance_kill);
}
tb_handle_t tb_thread_pool_init(tb_size_t worker_maxn, tb_size_t stack)
{
    // done
    tb_thread_pool_t*   pool = tb_null;
    tb_bool_t           ok = tb_false;
    do
    {
        // make pool
        pool = tb_malloc0(sizeof(tb_thread_pool_t));
        tb_assert_and_check_break(pool);

        // init lock
        if (!tb_spinlock_init(&pool->lock)) break;

        // computate the worker maxn if be zero
        if (!worker_maxn)
        {
            // TODO: using cpu count * 4
#ifdef __tb_small__
            worker_maxn = 4;
#else
            worker_maxn = 8;
#endif
        }

        // init thread stack
        pool->stack         = stack;

        // init workers
        pool->worker_size   = 0;
        pool->worker_maxn   = worker_maxn;

        // init jobs pool
        pool->jobs_pool     = tb_fixed_pool_init(TB_THREAD_POOL_JOBS_POOL_GROW, sizeof(tb_thread_pool_job_t), 0);
        tb_assert_and_check_break(pool->jobs_pool);

        // init jobs urgent
        pool->jobs_urgent   = tb_single_list_init(TB_THREAD_POOL_JOBS_URGENT_GROW, tb_item_func_ptr(tb_null, tb_null));
        tb_assert_and_check_break(pool->jobs_urgent);

        // init jobs waiting
        pool->jobs_waiting  = tb_single_list_init(TB_THREAD_POOL_JOBS_WAITING_GROW, tb_item_func_ptr(tb_null, tb_null));
        tb_assert_and_check_break(pool->jobs_waiting);

        // init jobs pending
        pool->jobs_pending  = tb_list_init(TB_THREAD_POOL_JOBS_PENDING_GROW, tb_item_func_ptr(tb_null, tb_null));
        tb_assert_and_check_break(pool->jobs_pending);

        // init semaphore
        pool->semaphore = tb_semaphore_init(0);
        tb_assert_and_check_break(pool->semaphore);

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&pool->lock, TB_TRACE_MODULE_NAME);
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (pool) tb_thread_pool_exit((tb_handle_t)pool);
        pool = tb_null;
    }

    // ok?
    return (tb_handle_t)pool;
}
tb_bool_t tb_thread_pool_exit(tb_handle_t handle)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return_val(pool, tb_false);

    // trace
    tb_trace_d("exit: ..");

    // kill it first
    tb_thread_pool_kill(handle);

    // wait all
    if (tb_thread_pool_task_wait_all(handle, 5000) <= 0)
    {
        // trace
        tb_trace_e("exit: wait failed!");
        return tb_false;
    }

    /* exit all workers
     * need not lock it because the worker size will not be increase d
     */
    tb_size_t i = 0;
    tb_size_t n = pool->worker_size;
    for (i = 0; i < n; i++) 
    {
        // the worker
        tb_thread_pool_worker_t* worker = &pool->worker_list[i];

        // exit loop
        if (worker->loop)
        {
            // wait it
            tb_long_t wait = 0;
            if ((wait = tb_thread_wait(worker->loop, 5000)) <= 0)
            {
                // trace
                tb_trace_e("worker[%lu]: wait failed: %ld!", i, wait);
            }

            // exit it
            tb_thread_exit(worker->loop);
            worker->loop = tb_null;
        }
    }
    pool->worker_size = 0;

    // enter
    tb_spinlock_enter(&pool->lock);

    // exit pending jobs
    if (pool->jobs_pending) tb_list_exit(pool->jobs_pending);
    pool->jobs_pending = tb_null;

    // exit waiting jobs
    if (pool->jobs_waiting) tb_single_list_exit(pool->jobs_waiting);
    pool->jobs_waiting = tb_null;

    // exit urgent jobs
    if (pool->jobs_urgent) tb_single_list_exit(pool->jobs_urgent);
    pool->jobs_urgent = tb_null;

    // exit jobs pool
    if (pool->jobs_pool) tb_fixed_pool_exit(pool->jobs_pool);
    pool->jobs_pool = tb_null;

    // leave
    tb_spinlock_leave(&pool->lock);

    // exit lock
    tb_spinlock_exit(&pool->lock);

    // exit semaphore
    if (pool->semaphore) tb_semaphore_exit(pool->semaphore);
    pool->semaphore = tb_null;

    // exit it
    tb_free(pool);

    // trace
    tb_trace_d("exit: ok");

    // ok
    return tb_true;
}
tb_void_t tb_thread_pool_kill(tb_handle_t handle)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return(pool);

    // enter
    tb_spinlock_enter(&pool->lock);

    // kill it
    tb_size_t post = 0;
    if (!pool->bstoped)
    {
        // trace
        tb_trace_d("kill: ..");

        // stoped
        pool->bstoped = tb_true;
        
        // kill all workers
        tb_size_t i = 0;
        tb_size_t n = pool->worker_size;
        for (i = 0; i < n; i++) tb_atomic_set(&pool->worker_list[i].bstoped, 1);

        // kill all jobs
        if (pool->jobs_pool) tb_fixed_pool_walk(pool->jobs_pool, tb_thread_pool_jobs_walk_kill_all, tb_null);

        // post it
        post = pool->worker_size;
    }

    // leave
    tb_spinlock_leave(&pool->lock);

    // post the workers
    if (post) tb_thread_pool_worker_post(pool, post);
}
tb_size_t tb_thread_pool_worker_size(tb_handle_t handle)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return_val(pool, 0);

    // enter
    tb_spinlock_enter(&pool->lock);

    // the worker size
    tb_size_t worker_size = pool->worker_size;

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok?
    return worker_size;
}
tb_size_t tb_thread_pool_task_size(tb_handle_t handle)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return_val(pool, 0);

    // enter
    tb_spinlock_enter(&pool->lock);

    // the task size
    tb_size_t task_size = pool->jobs_pool? tb_fixed_pool_size(pool->jobs_pool) : 0;

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok?
    return task_size;
}
tb_bool_t tb_thread_pool_task_post(tb_handle_t handle, tb_char_t const* name, tb_thread_pool_task_done_func_t done, tb_thread_pool_task_exit_func_t exit, tb_cpointer_t priv, tb_bool_t urgent)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return_val(pool && done, tb_false);

    // init the post size
    tb_size_t post_size = 0;

    // enter
    tb_spinlock_enter(&pool->lock);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // stoped?
        tb_check_break(!pool->bstoped);

        // init task
        tb_thread_pool_task_t task = {0};
        task.name       = name;
        task.done       = done;
        task.exit       = exit;
        task.priv       = priv;
        task.urgent     = urgent;

        // post task
        tb_thread_pool_job_t* job = tb_thread_pool_jobs_post_task(pool, &task, &post_size);
        tb_assert_and_check_break(job);

        // ok
        ok = tb_true;

    } while (0);

    // leave
    tb_spinlock_leave(&pool->lock);

    // post the workers
    if (ok && post_size) tb_thread_pool_worker_post(pool, post_size);

    // ok?
    return ok;
}
tb_size_t tb_thread_pool_task_post_list(tb_handle_t handle, tb_thread_pool_task_t const* list, tb_size_t size)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return_val(pool && list, 0);

    // init the post size
    tb_size_t post_size = 0;

    // enter
    tb_spinlock_enter(&pool->lock);

    // done
    tb_size_t ok = 0;
    if (!pool->bstoped)
    {
        for (ok = 0; ok < size; ok++)
        {
            // post task
            tb_thread_pool_job_t* job = tb_thread_pool_jobs_post_task(pool, &list[ok], &post_size);
            tb_assert_and_check_break(job);
        }
    }

    // leave
    tb_spinlock_leave(&pool->lock);

    // post the workers
    if (ok && post_size) tb_thread_pool_worker_post(pool, post_size);

    // ok?
    return ok;
}
tb_handle_t tb_thread_pool_task_init(tb_handle_t handle, tb_char_t const* name, tb_thread_pool_task_done_func_t done, tb_thread_pool_task_exit_func_t exit, tb_cpointer_t priv, tb_bool_t urgent)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return_val(pool && done, tb_null);

    // init the post size
    tb_size_t post_size = 0;

    // enter
    tb_spinlock_enter(&pool->lock);

    // done
    tb_bool_t               ok = tb_false;
    tb_thread_pool_job_t*   job = tb_null;
    do
    {
        // stoped?
        tb_check_break(!pool->bstoped);

        // init task
        tb_thread_pool_task_t task = {0};
        task.name       = name;
        task.done       = done;
        task.exit       = exit;
        task.priv       = priv;
        task.urgent     = urgent;

        // post task
        job = tb_thread_pool_jobs_post_task(pool, &task, &post_size);
        tb_assert_and_check_break(job);

        // refn++
        job->refn++;

        // ok
        ok = tb_true;

    } while (0);

    // leave
    tb_spinlock_leave(&pool->lock);

    // post the workers
    if (ok && post_size) tb_thread_pool_worker_post(pool, post_size);
    // failed?
    else if (!ok) job = tb_null;

    // ok?
    return (tb_handle_t)job;
}
tb_void_t tb_thread_pool_task_kill(tb_handle_t handle, tb_handle_t task)
{
    // check
    tb_thread_pool_job_t* job = (tb_thread_pool_job_t*)task;
    tb_assert_and_check_return(handle && job);

    // trace
    tb_trace_d("task[%p:%s]: kill: state: %s: ..", job->task.done, job->task.name, tb_state_cstr(tb_atomic_get(&job->state)));

    // kill it if be waiting
    tb_atomic_pset(&job->state, TB_STATE_WAITING, TB_STATE_KILLING);
}
tb_void_t tb_thread_pool_task_kill_all(tb_handle_t handle)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return(pool);

    // enter
    tb_spinlock_enter(&pool->lock);

    // kill all jobs
    if (!pool->bstoped && pool->jobs_pool) 
        tb_fixed_pool_walk(pool->jobs_pool, tb_thread_pool_jobs_walk_kill_all, tb_null);

    // leave
    tb_spinlock_leave(&pool->lock);
}
tb_long_t tb_thread_pool_task_wait(tb_handle_t handle, tb_handle_t task, tb_long_t timeout)
{
    // check
    tb_thread_pool_job_t* job = (tb_thread_pool_job_t*)task;
    tb_assert_and_check_return_val(handle && job, -1);

    // wait it
    tb_hong_t time = tb_cache_time_spak();
    tb_size_t state = TB_STATE_WAITING;
    while ( ((state = tb_atomic_get(&job->state)) != TB_STATE_FINISHED) 
        &&  state != TB_STATE_KILLED
        &&  (timeout < 0 || tb_cache_time_spak() < time + timeout))
    {
        // trace
        tb_trace_d("task[%p:%s]: wait: state: %s: ..", job->task.done, job->task.name, tb_state_cstr(state));

        // wait some time
        tb_msleep(200);
    }

    // ok?
    return (state == TB_STATE_FINISHED || state == TB_STATE_KILLED)? 1 : 0;
}
tb_long_t tb_thread_pool_task_wait_all(tb_handle_t handle, tb_long_t timeout)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return_val(pool, -1);

    // wait it
    tb_size_t size = 0;
    tb_hong_t time = tb_cache_time_spak();
    while ((timeout < 0 || tb_cache_time_spak() < time + timeout))
    {
        // enter
        tb_spinlock_enter(&pool->lock);

        // the jobs count
        size = pool->jobs_pool? tb_fixed_pool_size(pool->jobs_pool) : 0;

        // trace
        tb_trace_d("wait: jobs: %lu, waiting: %lu, pending: %lu, urgent: %lu: .."
                    , size
                    , pool->jobs_waiting? tb_single_list_size(pool->jobs_waiting) : 0
                    , pool->jobs_pending? tb_list_size(pool->jobs_pending) : 0
                    , pool->jobs_urgent? tb_single_list_size(pool->jobs_urgent) : 0);

        // leave
        tb_spinlock_leave(&pool->lock);

        // ok?
        tb_check_break(size);

        // wait some time
        tb_msleep(200);
    }

    // ok?
    return !size? 1 : 0;
}
tb_void_t tb_thread_pool_task_exit(tb_handle_t handle, tb_handle_t task)
{
    // check
    tb_thread_pool_t*       pool = (tb_thread_pool_t*)handle;
    tb_thread_pool_job_t*   job = (tb_thread_pool_job_t*)task;
    tb_assert_and_check_return(pool && job);

    // kill it first
    tb_thread_pool_task_kill(handle, task);

    // enter
    tb_spinlock_enter(&pool->lock);

    // refn--
    if (job->refn > 1) job->refn--;
    // remove it from pool directly
    else tb_fixed_pool_free(pool->jobs_pool, job);

    // leave
    tb_spinlock_leave(&pool->lock);
}
#ifdef __tb_debug__
tb_void_t tb_thread_pool_dump(tb_handle_t handle)
{
    // check
    tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
    tb_assert_and_check_return(pool);

    // enter
    tb_spinlock_enter(&pool->lock);

    // dump workers
    if (pool->worker_size)
    {
        // trace
        tb_trace_i("======================================================================");
        tb_trace_i("workers: size: %lu, maxn: %lu", pool->worker_size, pool->worker_maxn);

        // walk
        tb_size_t i = 0;
        for (i = 0; i < pool->worker_size; i++)
        {
            // the worker
            tb_thread_pool_worker_t* worker = &pool->worker_list[i];

            // dump worker
            tb_trace_i("    worker: id: %lu, stoped: %ld", worker->id, (tb_long_t)tb_atomic_get(&worker->bstoped));
        }

        // dump all jobs
        if (pool->jobs_pool) 
        {
            // trace
            tb_trace_i("jobs: size: %lu", tb_fixed_pool_size(pool->jobs_pool));

            // dump jobs
            tb_fixed_pool_walk(pool->jobs_pool, tb_thread_pool_jobs_walk_dump_all, tb_null);
        }
        tb_trace_i("======================================================================");
    }

    // leave
    tb_spinlock_leave(&pool->lock);
}
#endif
