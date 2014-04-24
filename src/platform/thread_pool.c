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
 * @author		ruki
 * @file		thread_pool.c
 * @ingroup 	platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 				"thread_pool"
#define TB_TRACE_MODULE_DEBUG 				(0)

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
#ifdef __tb_small__
# 	define TB_THREAD_POOL_WORKER_MAXN 		(16)
#else
# 	define TB_THREAD_POOL_WORKER_MAXN 		(64)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the thread pool job state enum
typedef enum __tb_thread_pool_job_state_e
{
	TB_THREAD_POOL_JOB_STATE_WAITING 	= 0
,	TB_THREAD_POOL_JOB_STATE_PENDING 	= 1
,	TB_THREAD_POOL_JOB_STATE_WORKING 	= 2
,	TB_THREAD_POOL_JOB_STATE_KILLED 	= 3
,	TB_THREAD_POOL_JOB_STATE_FINISHED 	= 4

}tb_thread_pool_job_state_e;

// the thread pool job type
typedef struct __tb_thread_pool_job_t
{
	// the task
	tb_thread_pool_task_t 	task;

	// state
	tb_atomic_t 			state;

}tb_thread_pool_job_t;

// the thread pool worker type
typedef struct __tb_thread_pool_worker_t
{
	// the thread pool handle
	tb_handle_t 			pool;

	// the loop
	tb_handle_t 			loop;

	// the jobs
	tb_vector_t* 			jobs;

	// is stoped?
	tb_atomic_t 			bstoped;
	
}tb_thread_pool_worker_t;

// the thread pool type
typedef struct __tb_thread_pool_t
{
	// the thread stack size
	tb_size_t 				stack;

	// the worker maxn
	tb_size_t 				worker_maxn;

	// the lock
	tb_spinlock_t 			lock;

	// the jobs pool
	tb_handle_t 			jobs_pool;

	// the urgent jobs
	tb_slist_t* 			jobs_urgent;
	
	// the waiting jobs
	tb_slist_t* 			jobs_waiting;
	
	// the pending jobs
	tb_dlist_t* 			jobs_pending;

	// is stoped
	tb_bool_t 				bstoped;

	// the worker size
	tb_size_t 				worker_size;

	// the worker list
	tb_thread_pool_worker_t worker_list[TB_THREAD_POOL_WORKER_MAXN];

}tb_thread_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_thread_pool_instance_init()
{
	// init it
	return tb_thread_pool_init(0, 0);
}
static tb_void_t tb_thread_pool_instance_exit(tb_handle_t handle)
{
	// dump it
#ifdef __tb_debug__
	tb_thread_pool_dump(handle);
#endif

	// exit it
	tb_thread_pool_exit(handle);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * worker implementation
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * jobs implementation
 */
tb_bool_t tb_thread_pool_jobs_walk_kill_all(tb_pointer_t item, tb_pointer_t data)
{
	// check
	tb_thread_pool_job_t* job = (tb_thread_pool_job_t*)item;
	tb_assert_and_check_return_val(job, tb_false);

	// kill it
	tb_atomic_set(&job->state, TB_THREAD_POOL_JOB_STATE_KILLED);

	// ok
	return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_thread_pool_init(tb_size_t worker_maxn, tb_size_t stack)
{
	// done
	tb_thread_pool_t* 	pool = tb_null;
	tb_bool_t 			ok = tb_false;
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

		// init pool
		pool->stack 		= stack;
		pool->worker_maxn 	= worker_maxn;

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
tb_void_t tb_thread_pool_exit(tb_handle_t handle)
{
	// check
	tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
	tb_assert_and_check_return(pool);

	// kill it first
	tb_thread_pool_kill(handle);

	// enter
	tb_spinlock_enter(&pool->lock);

	// exit all workers
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

		// exit jobs
		if (worker->jobs) tb_vector_exit(worker->jobs);
		worker->jobs = tb_null;
	}
	pool->worker_size = 0;

	// exit pending jobs
	if (pool->jobs_pending) tb_dlist_exit(pool->jobs_pending);
	pool->jobs_pending = tb_null;

	// exit waiting jobs
	if (pool->jobs_waiting) tb_slist_exit(pool->jobs_waiting);
	pool->jobs_waiting = tb_null;

	// exit urgent jobs
	if (pool->jobs_urgent) tb_slist_exit(pool->jobs_urgent);
	pool->jobs_urgent = tb_null;

	// exit jobs pool
	if (pool->jobs_pool) tb_fixed_pool_exit(pool->jobs_pool);
	pool->jobs_pool = tb_null;

	// leave
	tb_spinlock_leave(&pool->lock);

	// exit lock
	tb_spinlock_exit(&pool->lock);

	// exit it
	tb_free(pool);
}
tb_void_t tb_thread_pool_kill(tb_handle_t handle)
{
	// check
	tb_thread_pool_t* pool = (tb_thread_pool_t*)handle;
	tb_assert_and_check_return(pool);

	// enter
	tb_spinlock_enter(&pool->lock);

	// kill it
	if (!pool->bstoped)
	{
		// stoped
		pool->bstoped = tb_true;
		
		// kill all workers
		tb_size_t i = 0;
		tb_size_t n = pool->worker_size;
		for (i = 0; i < n; i++) tb_atomic_set(&pool->worker_list[i].bstoped, 1);

		// kill all jobs
		if (pool->jobs_pool) tb_fixed_pool_walk(pool->jobs_pool, tb_thread_pool_jobs_walk_kill_all, tb_null);
	}

	// leave
	tb_spinlock_leave(&pool->lock);
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
tb_bool_t tb_thread_pool_task_post(tb_handle_t handle, tb_thread_pool_task_done_func_t done, tb_thread_pool_task_exit_func_t exit, tb_pointer_t priv, tb_bool_t urgent)
{
	return tb_false;
}
tb_bool_t tb_thread_pool_task_post_list(tb_handle_t handle, tb_thread_pool_task_t const* list, tb_size_t size)
{
	return tb_false;
}
tb_handle_t tb_thread_pool_task_init(tb_handle_t handle, tb_thread_pool_task_done_func_t done, tb_thread_pool_task_exit_func_t exit, tb_pointer_t priv, tb_bool_t urgent)
{
	return tb_null;
}
tb_void_t tb_thread_pool_task_kill(tb_handle_t handle, tb_handle_t task)
{
	// check
	tb_thread_pool_job_t* job = (tb_thread_pool_job_t*)task;
	tb_assert_and_check_return(job);

	// kill it
	tb_atomic_set(&job->state, TB_THREAD_POOL_JOB_STATE_KILLED);
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
tb_long_t tb_thread_pool_task_wait(tb_handle_t pool, tb_handle_t task, tb_long_t timeout)
{
	return -1;
}
tb_void_t tb_thread_pool_task_exit(tb_handle_t pool, tb_handle_t task)
{
}
tb_handle_t tb_thread_pool_instance()
{
	return tb_singleton_instance(TB_SINGLETON_TYPE_THREAD_POOL, tb_thread_pool_instance_init, tb_thread_pool_instance_exit, tb_thread_pool_kill);
}
#ifdef __tb_debug__
tb_void_t tb_thread_pool_dump(tb_handle_t handle)
{
}
#endif
