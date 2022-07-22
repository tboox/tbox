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
 * @file        poller_process.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../process.h"
#include "../thread.h"
#include "../atomic.h"
#include "../semaphore.h"
#include "../spinlock.h"
#include "../../algorithm/algorithm.h"
#include "../../container/container.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the processes data type
typedef struct __tb_poller_processes_data_t
{
    // the process reference
    tb_process_ref_t        process;

    // the user private data
    tb_cpointer_t           priv;

}tb_poller_processes_data_t;

// the processes status type
typedef struct __tb_poller_processes_status_t
{
    // the process id
    tb_int_t                pid;

    // the process status
    tb_int_t                status;

}tb_poller_processes_status_t;

// the poller process type
typedef struct __tb_poller_process_t
{
    // the main poller
    tb_poller_t*            main_poller;

    // the process poller thread
    tb_thread_ref_t         thread;

    // the waited processes data, pid => process and user private data
    tb_hash_map_ref_t       processes_data;

    // the processes status
    tb_vector_ref_t         processes_status;

    // the copied processes status
    tb_vector_ref_t         processes_status_copied;

    // the semaphore
    tb_semaphore_ref_t      semaphore;

    // is stopped?
    tb_atomic32_t           is_stopped;

    // the lock
    tb_spinlock_t           lock;

}tb_poller_process_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c_enter__
tb_int_t tb_process_pid(tb_process_ref_t self);
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the global process poller
static tb_poller_process_t* g_process_poller = tb_null;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_int_t tb_poller_process_loop(tb_cpointer_t priv)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)priv;
    tb_assert_and_check_return_val(poller && poller->semaphore, -1);

    while (!tb_atomic32_get(&poller->is_stopped))
    {
        // trace
        tb_trace_d("process: waitpid ..");

        // wait semaphore
        tb_long_t wait = tb_semaphore_wait(poller->semaphore, -1);
        tb_assert_and_check_break(wait >= 0);

        // interrupted? continue to wait
        tb_check_continue(wait != 0);

        tb_int_t  result = -1;
        tb_bool_t has_exited = tb_false;
        do
        {
            // poll processes
            tb_int_t status = -1;
            result = waitpid(-1, &status, WNOHANG | WUNTRACED);

            // trace
            tb_trace_d("process: finished: %d, status: %d", result, status);

            // has exited process?
            if (result != 0 && result != -1)
            {
                /* get status, only get 8bits retval
                 *
                 * tt's limited to 8-bits, which means 1 byte,
                 * which means the int from WEXITSTATUS can only range from 0-255.
                 *
                 * in fact, any unix program will only ever return a max of 255.
                 */
                tb_poller_processes_status_t object_event;
                object_event.status = WIFEXITED(status)? WEXITSTATUS(status) : -1;
                object_event.pid = result;

                // save the process status
                tb_spinlock_enter(&poller->lock);
                tb_vector_insert_tail(poller->processes_status, &object_event);
                has_exited = tb_true;
                tb_spinlock_leave(&poller->lock);
            }

        } while (result != 0 && result != -1);

        // has exited child processes? notify the main poller to poll them
        if (has_exited)
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
static tb_void_t tb_poller_process_signal_handler(tb_int_t signo)
{
    // post semaphore to wait processes
    if (g_process_poller && g_process_poller->semaphore)
        tb_semaphore_post(g_process_poller->semaphore, 1);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_poller_process_kill(tb_poller_process_ref_t self)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return(poller && poller->semaphore);

    // trace
    tb_trace_d("process: kill ..");

    // stop thread and post it
    if (!tb_atomic32_fetch_and_set(&poller->is_stopped, 1))
        tb_semaphore_post(poller->semaphore, 1);
}
static tb_void_t tb_poller_process_exit(tb_poller_process_ref_t self)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return(poller);

    // kill the process poller first
    tb_poller_process_kill(self);

    // exit the process poller thread
    if (poller->thread)
    {
        // wait it
        tb_long_t wait = 0;
        if ((wait = tb_thread_wait(poller->thread, 5000, tb_null)) <= 0)
        {
            // trace
            tb_trace_e("wait process poller thread failed: %ld!", wait);
        }

        // exit it
        tb_thread_exit(poller->thread);
        poller->thread = tb_null;
    }

    // clear signal
    signal(SIGCHLD, SIG_DFL);
    g_process_poller = tb_null;

    // exit the processes data
    if (poller->processes_data) tb_hash_map_exit(poller->processes_data);
    poller->processes_data = tb_null;

    // exit the processes status
    if (poller->processes_status) tb_vector_exit(poller->processes_status);
    poller->processes_status = tb_null;

    if (poller->processes_status_copied) tb_vector_exit(poller->processes_status_copied);
    poller->processes_status_copied = tb_null;

    // exit semaphore
    if (poller->semaphore) tb_semaphore_exit(poller->semaphore);
    poller->semaphore = tb_null;

    // exit lock
    tb_spinlock_exit(&poller->lock);

    // exit poller
    tb_free(poller);
}
static tb_poller_process_ref_t tb_poller_process_init(tb_poller_t* main_poller)
{
    tb_bool_t            ok = tb_false;
    tb_poller_process_t* poller = tb_null;
    do
    {
        // @note only support one process poller instance
        static tb_size_t s_poller_process_num = 0;
        if (s_poller_process_num++)
        {
            tb_trace_e("only support one process poller!");
            break;
        }

        // make the process poller
        poller = tb_malloc0_type(tb_poller_process_t);
        tb_assert_and_check_break(poller);

        // save the main poller
        poller->main_poller = main_poller;

        // init the processes data first
        poller->processes_data = tb_hash_map_init(TB_HASH_MAP_BUCKET_SIZE_MICRO, tb_element_uint32(), tb_element_mem(sizeof(tb_poller_processes_data_t), tb_null, tb_null));
        tb_assert_and_check_break(poller->processes_data);

        // init semaphore
        poller->semaphore = tb_semaphore_init(0);
        tb_assert_and_check_break(poller->semaphore);

        // init lock
        tb_spinlock_init(&poller->lock);

        // init the processes status
        poller->processes_status = tb_vector_init(0, tb_element_mem(sizeof(tb_poller_processes_status_t), tb_null, tb_null));
        tb_assert_and_check_break(poller->processes_status);

        // init the copied processes status
        poller->processes_status_copied = tb_vector_init(0, tb_element_mem(sizeof(tb_poller_processes_status_t), tb_null, tb_null));
        tb_assert_and_check_break(poller->processes_status_copied);

        // start the poller thread for processes first
        poller->thread = tb_thread_init(tb_null, tb_poller_process_loop, poller, 0);
        tb_assert_and_check_break(poller->thread);

        // register signal
        signal(SIGCHLD, tb_poller_process_signal_handler);
        g_process_poller = poller;

        // ok
        ok = tb_true;

    } while (0);

    // failed? exit the poller
    if (!ok)
    {
        if (poller) tb_poller_process_exit((tb_poller_process_ref_t)poller);
        poller = tb_null;
    }
    return (tb_poller_process_ref_t)poller;
}
static tb_void_t tb_poller_process_spak(tb_poller_process_ref_t self)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return(poller && poller->semaphore);

    // trace
    tb_trace_d("process: spak ..");

    // post it
    tb_semaphore_post(poller->semaphore, 1);
}
static tb_bool_t tb_poller_process_insert(tb_poller_process_ref_t self, tb_process_ref_t process, tb_cpointer_t priv)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->processes_data && process, tb_false);

    // get pid
    tb_int_t pid = tb_process_pid(process);
    tb_assert_and_check_return_val(pid != -1, tb_false);

    // trace
    tb_trace_d("process: insert pid: %d with priv: %p", pid, priv);

    // insert this process and the user private data
    tb_poller_processes_data_t procdata;
    procdata.process = process;
    procdata.priv    = priv;
    tb_hash_map_insert(poller->processes_data, tb_i2p(pid), &procdata);
    return tb_true;
}
static tb_bool_t tb_poller_process_modify(tb_poller_process_ref_t self, tb_process_ref_t process, tb_cpointer_t priv)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->processes_data && process, tb_false);

    // get pid
    tb_int_t pid = tb_process_pid(process);
    tb_assert_and_check_return_val(pid != -1, tb_false);

    // trace
    tb_trace_d("process: modify pid: %d with priv: %p", pid, priv);

    // modify the user private data of this process
    if (tb_hash_map_find(poller->processes_data, tb_i2p(pid)) != tb_iterator_tail(poller->processes_data))
    {
        tb_poller_processes_data_t procdata;
        procdata.process = process;
        procdata.priv    = priv;
        tb_hash_map_insert(poller->processes_data, tb_i2p(pid), &procdata);
    }
    return tb_true;
}
static tb_bool_t tb_poller_process_remove(tb_poller_process_ref_t self, tb_process_ref_t process)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->processes_data && process, tb_false);

    // get pid
    tb_int_t pid = tb_process_pid(process);
    tb_assert_and_check_return_val(pid != -1, tb_false);

    // trace
    tb_trace_d("process: remove pid: %d", pid);

    // remove this process and the user private data
    tb_hash_map_remove(poller->processes_data, tb_i2p(pid));
    return tb_true;
}
static tb_bool_t tb_poller_process_wait_prepare(tb_poller_process_ref_t self)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->processes_data && poller->semaphore, tb_false);

    // trace
    tb_trace_d("process: prepare %lu", tb_hash_map_size(poller->processes_data));

    // some processes maybe have been exited before starting the loop thread, so we need notify to waitpid
    if (tb_hash_map_size(poller->processes_data) && !tb_semaphore_post(poller->semaphore, 1))
        return tb_false;

    // is stopped?
    return !tb_atomic32_get(&poller->is_stopped);
}
static tb_long_t tb_poller_process_wait_poll(tb_poller_process_ref_t self, tb_poller_event_func_t func)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->processes_data && func, -1);
    tb_assert_and_check_return_val(poller->processes_status && poller->processes_status_copied, -1);

    // get all processes status
    tb_vector_clear(poller->processes_status_copied);
    tb_spinlock_enter(&poller->lock);
    if (tb_vector_size(poller->processes_status))
    {
        tb_vector_copy(poller->processes_status_copied, poller->processes_status);
        tb_vector_clear(poller->processes_status);
    }
    tb_spinlock_leave(&poller->lock);

    // trace
    tb_trace_d("process: poll %lu", tb_vector_size(poller->processes_status_copied));

    // poll all waited processes status
    tb_long_t wait = 0;
    tb_poller_object_t object;
    object.type = TB_POLLER_OBJECT_PROC;
    tb_for_all_if (tb_poller_processes_status_t*, object_event, poller->processes_status_copied, object_event)
    {
        // trace
        tb_trace_d("process: pid: %d", object_event->pid);

        tb_poller_processes_data_t* proc_data = tb_hash_map_get(poller->processes_data, tb_i2p(object_event->pid));
        if (proc_data)
        {
            object.ref.proc = proc_data->process;
            func((tb_poller_ref_t)poller->main_poller, &object, object_event->status, proc_data->priv);
            wait++;
        }
    }

    // trace
    tb_trace_d("process: poll wait %ld", wait);
    return wait;
}
