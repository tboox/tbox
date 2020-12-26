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
 * @file        sched_affinity.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../sched.h"
#include <mach/mach.h>
#include <mach/mach_vm.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_sched_setaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // get the first cpu core index
    tb_int_t core = TB_CPUSET_FFS(cpuset);

    // trace
    tb_trace_d("setaffinity: core(%d) to pid(%zu)", core, pid);

    // get port
    mach_port_name_t port;
    if (pid)
    {
        if (task_for_pid(mach_task_self(), (tb_int_t)pid, &port) != KERN_SUCCESS)
        {
            tb_trace_e("cannot get port for the gived pid(%zu)!", pid);
            return tb_false;
        }
    }
    else port = mach_task_self();

    // get all threads
    thread_act_t*           threads = tb_null;
    mach_msg_type_number_t  thread_count = 0;
    if (task_threads(port, &threads, &thread_count) != KERN_SUCCESS)
    {
        tb_trace_e("cannot get task threads for the gived pid(%lu)!", pid);
        return tb_false;
    }
    tb_assert_and_check_return_val(threads && thread_count > 0, tb_false);

    // suspend threads first
    tb_size_t i;
    thread_t thread_self = mach_thread_self();
    mach_port_deallocate(mach_task_self(), thread_self);
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL && threads[i] != thread_self)
            thread_suspend(threads[i]);
    }

    // set affinity for all threads
    tb_size_t ok = tb_true;
    thread_affinity_policy_data_t policy;
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL)
        {
            // TODO only set the first thread successfully now.
            policy.affinity_tag = core;
            if (thread_policy_set(threads[i], THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT) != KERN_SUCCESS)
                ok = tb_false;
        }
    }

    // resume threads
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL && threads[i] != thread_self)
            thread_suspend(threads[i]);
    }

    // free threads
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL)
            mach_port_deallocate(port, threads[i]);
    }
    mach_vm_deallocate(port, (mach_vm_address_t)threads, thread_count * sizeof(*threads));
    return ok;
}
tb_bool_t tb_sched_getaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // get port
    mach_port_name_t port;
    if (pid)
    {
        if (task_for_pid(mach_task_self(), (tb_int_t)pid, &port) != KERN_SUCCESS)
        {
            tb_trace_e("cannot get port for the gived pid(%lu)!", pid);
            return tb_false;
        }
    }
    else port = mach_task_self();

    // get all threads
    thread_act_t*           threads = tb_null;
    mach_msg_type_number_t  thread_count = 0;
    if (task_threads(port, &threads, &thread_count) != KERN_SUCCESS)
    {
        tb_trace_e("cannot get task threads for the gived pid(%lu)!", pid);
        return tb_false;
    }
    tb_assert_and_check_return_val(threads && thread_count > 0, tb_false);

    // suspend threads first
    tb_size_t i;
    thread_t thread_self = mach_thread_self();
    mach_port_deallocate(mach_task_self(), thread_self);
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL && threads[i] != thread_self)
            thread_suspend(threads[i]);
    }

    // set affinity for all threads
    tb_size_t ok = tb_true;
    TB_CPUSET_ZERO(cpuset);
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL)
        {
            boolean_t get_default = tb_false;
            mach_msg_type_number_t count = THREAD_AFFINITY_POLICY_COUNT;
            thread_affinity_policy_data_t policy;
            policy.affinity_tag = 0;
            if (thread_policy_get(threads[i], THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, &count, &get_default) == KERN_SUCCESS)
            {
                TB_CPUSET_SET(policy.affinity_tag, cpuset);
            }
            else ok = tb_false;
        }
    }

    // resume threads
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL && threads[i] != thread_self)
            thread_suspend(threads[i]);
    }

    // free threads
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL)
            mach_port_deallocate(port, threads[i]);
    }
    mach_vm_deallocate(port, (mach_vm_address_t)threads, thread_count * sizeof(*threads));
    return ok;
}
