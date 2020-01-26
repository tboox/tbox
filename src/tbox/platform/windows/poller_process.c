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
 * Copyright (C) 2009-2020, TBOOX Open Source Group.
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
    // the process reference
    tb_process_ref_t        process;

    // the user private data
    tb_cpointer_t           priv;

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

    // the waited processes data
    tb_vector_ref_t         processes_data;

    // the processes status 
    tb_vector_ref_t         processes_status;

    // the copied processes status 
    tb_vector_ref_t         processes_status_copied;

    // is stopped?
    tb_atomic32_t           is_stopped;

    // semaphore
    HANDLE                  semaphore;

    // the lock
    tb_spinlock_t           lock;

}tb_poller_process_t; 

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c_enter__
HANDLE      tb_process_handle(tb_process_ref_t self);
tb_void_t   tb_process_handle_close(tb_process_ref_t self);
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_int_t tb_poller_process_loop(tb_cpointer_t priv)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)priv;
    tb_assert_and_check_return_val(poller && poller->semaphore, -1);

    tb_size_t                   procsize = 0;
    HANDLE                      proclist[512] = {0};
    tb_poller_processes_data_t  procdata[512] = {0};
    while (!tb_atomic32_get(&poller->is_stopped))
    {
        // add semaphore first
        proclist[0] = poller->semaphore;
        tb_spinlock_enter(&poller->lock);
        {
            // append all previous pending processes
            if (procsize > 1)
            {
                tb_size_t i;
                for (i = 1; i < procsize; i++)
                {
                    if (procdata[i].process)
                        tb_vector_insert_tail(poller->processes_data, &procdata[i]);
                }
            }

            // generate waited processes list
            procsize = 1;
            if (tb_vector_size(poller->processes_data))
            {
                tb_for_all_if (tb_poller_processes_data_t*, proc_data, poller->processes_data, proc_data)
                {
                    tb_assert_and_check_continue(procsize < tb_arrayn(proclist));
                    procdata[procsize]   = *proc_data;
                    proclist[procsize++] = tb_process_handle(proc_data->process);
                }
                tb_vector_clear(poller->processes_data);
            }
        }
        tb_spinlock_leave(&poller->lock);

        // trace
        tb_trace_d("process: wait %lu ..", procsize - 1);

        // wait processes
        DWORD exitcode = 0;
        DWORD result = tb_kernel32()->WaitForMultipleObjects((DWORD)procsize, proclist, FALSE, -1);
        tb_assert_and_check_break(result != WAIT_FAILED);

        // trace
        tb_trace_d("process: wait ok, result: %d", result);

        // has exited processes?
        tb_bool_t has_exited = tb_false;
        tb_poller_processes_status_t proc_status;
        if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + procsize)
        {
            // the process index
            DWORD index = result - WAIT_OBJECT_0;

            // the process handle
            HANDLE proc_handle = proclist[index];
            tb_assert_and_check_break(proc_handle && proc_handle != INVALID_HANDLE_VALUE);

            // get process status
            if (proc_handle != poller->semaphore)
            {
                exitcode = 0;
                proc_status.status = tb_kernel32()->GetExitCodeProcess(proc_handle, &exitcode)? (tb_int_t)exitcode : -1;  
                proc_status.process = procdata[index].process;
                proc_status.priv    = procdata[index].priv;
                tb_assert(proc_status.process);

                // mark as finished
                procdata[index].process = tb_null;

                // trace
                tb_trace_d("process: finished: %p, status: %d", proc_status.process, proc_status.status);

                // close process handles first
                tb_process_handle_close(proc_status.process);

                // save the process status
                tb_spinlock_enter(&poller->lock);
                tb_vector_insert_tail(poller->processes_status, &proc_status);
                tb_spinlock_leave(&poller->lock);
                has_exited = tb_true;
            }

            // next index
            index++;
            while (index < procsize)
            {
                // attempt to wait next process
                result = tb_kernel32()->WaitForMultipleObjects((DWORD)(procsize - index), proclist + index, FALSE, 0);
                tb_assert_and_check_break(result != WAIT_FAILED);

                // has exited processes?
                if (result >= WAIT_OBJECT_0 && index + result < WAIT_OBJECT_0 + procsize)
                {
                    // the process index
                    index += result - WAIT_OBJECT_0;

                    // the process handle
                    proc_handle = proclist[index];
                    tb_assert_and_check_break(proc_handle && proc_handle != INVALID_HANDLE_VALUE);

                    // get process status
                    exitcode = 0;
                    proc_status.status = tb_kernel32()->GetExitCodeProcess(proc_handle, &exitcode)? (tb_int_t)exitcode : -1;  
                    proc_status.process = procdata[index].process;
                    proc_status.priv    = procdata[index].priv;
                    tb_assert(proc_status.process);

                    // mark as finished
                    procdata[index].process = tb_null;

                    // trace
                    tb_trace_d("process: finished: %p, status: %d", proc_status.process, proc_status.status);

                    // close process handles first
                    tb_process_handle_close(proc_status.process);

                    // save the process status
                    tb_spinlock_enter(&poller->lock);
                    tb_vector_insert_tail(poller->processes_status, &proc_status);
                    tb_spinlock_leave(&poller->lock);
                    has_exited = tb_true;

                    // next index
                    index++;
                }
                else break;
            }
        }

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
static tb_bool_t tb_poller_process_data_pred(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t name)
{
    // check
    tb_assert(item);

    // is equal?
    return ((tb_poller_processes_data_t*)item)->process == (tb_process_ref_t)name;
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
        ReleaseSemaphore(poller->semaphore, 1, tb_null);
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

    // exit the processes data
    if (poller->processes_data) tb_vector_exit(poller->processes_data);
    poller->processes_data = tb_null;

    // exit the processes status
    if (poller->processes_status) tb_vector_exit(poller->processes_status);
    poller->processes_status = tb_null;

    if (poller->processes_status_copied) tb_vector_exit(poller->processes_status_copied);
    poller->processes_status_copied = tb_null;

    // exit lock
    tb_spinlock_exit(&poller->lock);

    // exit semaphore
    if (poller->semaphore) CloseHandle(poller->semaphore);
    poller->semaphore = tb_null;

    // exit poller
    tb_free(poller);
}
static tb_poller_process_ref_t tb_poller_process_init(tb_poller_t* main_poller)
{
    tb_bool_t            ok = tb_false;
    tb_poller_process_t* poller = tb_null;
    do
    {
        // make the process poller
        poller = tb_malloc0_type(tb_poller_process_t);
        tb_assert_and_check_break(poller);

        // save the main poller
        poller->main_poller = main_poller;

        // init semaphore
        poller->semaphore = CreateSemaphoreA(tb_null, 0, 128, tb_null);
        tb_assert_and_check_break(poller->semaphore && poller->semaphore != INVALID_HANDLE_VALUE);

        // init lock
        tb_spinlock_init(&poller->lock);

        // init the processes data
        poller->processes_data = tb_vector_init(0, tb_element_mem(sizeof(tb_poller_processes_data_t), tb_null, tb_null));
        tb_assert_and_check_break(poller->processes_data);

        // init the processes status
        poller->processes_status = tb_vector_init(0, tb_element_mem(sizeof(tb_poller_processes_status_t), tb_null, tb_null));
        tb_assert_and_check_break(poller->processes_status);

        // init the copied processes status
        poller->processes_status_copied = tb_vector_init(0, tb_element_mem(sizeof(tb_poller_processes_status_t), tb_null, tb_null));
        tb_assert_and_check_break(poller->processes_status_copied);

        // start the poller thread for processes first
        poller->thread = tb_thread_init(tb_null, tb_poller_process_loop, poller, 0);
        tb_assert_and_check_break(poller->thread);

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
    ReleaseSemaphore(poller->semaphore, 1, tb_null);
}
static tb_bool_t tb_poller_process_insert(tb_poller_process_ref_t self, tb_process_ref_t process, tb_cpointer_t priv)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->processes_data && process, tb_false);

    // trace
    tb_trace_d("process: insert: %p with priv: %p", process, priv);

    // insert this process and the user private data
    tb_spinlock_enter(&poller->lock);
    {
        tb_poller_processes_data_t procdata;
        procdata.process  = process;
        procdata.priv     = priv;
        tb_vector_insert_tail(poller->processes_data, &procdata);
    }
    tb_spinlock_leave(&poller->lock);
    return tb_true;
}

static tb_bool_t tb_poller_process_modify(tb_poller_process_ref_t self, tb_process_ref_t process, tb_cpointer_t priv)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->processes_data && process, tb_false);

    // trace
    tb_trace_d("process: modify: %p with priv: %p", process, priv);

    /* modify the user private data of this process
     * @note may be slower, but it will be cleared after triggering the wait, so this list will not be too large
     */
    tb_spinlock_enter(&poller->lock);
    {
        tb_size_t itor = tb_find_all_if(poller->processes_data, tb_poller_process_data_pred, process);
        if (itor != tb_iterator_tail(poller->processes_data))
        {
            tb_poller_processes_data_t* procdata = (tb_poller_processes_data_t*)tb_iterator_item(poller->processes_data, itor);
            if (procdata) procdata->priv = priv;
        }
    }
    tb_spinlock_leave(&poller->lock);
    return tb_true;
}
static tb_bool_t tb_poller_process_remove(tb_poller_process_ref_t self, tb_process_ref_t process)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->processes_data && process, tb_false);

    // trace
    tb_trace_d("process: remove: %p", process);

    /* remove the user private data of this process
     * @note may be slower, but it will be cleared after triggering the wait, so this list will not be too large
     */
    tb_spinlock_enter(&poller->lock);
    {
        tb_size_t itor = tb_find_all_if(poller->processes_data, tb_poller_process_data_pred, process);
        if (itor != tb_iterator_tail(poller->processes_data))
            tb_vector_remove(poller->processes_data, itor);
    }
    tb_spinlock_leave(&poller->lock);
    return tb_true;
}
static tb_bool_t tb_poller_process_wait_prepare(tb_poller_process_ref_t self)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->processes_data, tb_false);

    // is stopped?
    tb_check_return_val(!tb_atomic32_get(&poller->is_stopped), tb_false);

    // get the current waited processes size
    tb_spinlock_enter(&poller->lock);
    tb_size_t size = tb_vector_size(poller->processes_data);
    tb_spinlock_leave(&poller->lock);

    // trace
    tb_trace_d("process: prepare %lu", size);

    // notify to wait processes
    if (size) ReleaseSemaphore(poller->semaphore, 1, tb_null);
    return tb_true;
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
    tb_long_t     wait = 0;
    tb_cpointer_t priv = tb_null;
    tb_poller_object_t object;
    object.type = TB_POLLER_OBJECT_PROC;
    tb_for_all_if (tb_poller_processes_status_t*, proc_status, poller->processes_status_copied, proc_status)
    {
        // trace
        tb_trace_d("process: %p, status: %ld", proc_status->process, proc_status->status);

        // do callback
        object.ref.proc = proc_status->process;
        func((tb_poller_ref_t)poller->main_poller, &object, proc_status->status, proc_status->priv);
        wait++;
    }

    // trace
    tb_trace_d("process: poll wait %ld", wait);
    return wait;
}
