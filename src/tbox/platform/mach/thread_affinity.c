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
 * @file        thread_affinity.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../thread.h"
#include <pthread.h>
#include <mach/mach.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_thread_setaffinity(tb_thread_ref_t thread, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // get mach thread
    pthread_t pthread = thread? *((pthread_t*)thread) : pthread_self();
    thread_port_t mach_thread = pthread_mach_thread_np(pthread);
    tb_assert_and_check_return_val(mach_thread != MACH_PORT_NULL, tb_false);

    // get the first cpu core index
    tb_int_t core = TB_CPUSET_FFS(cpuset);

    // set thread affinity
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = core;
    return thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT) == KERN_SUCCESS;
}
tb_bool_t tb_thread_getaffinity(tb_thread_ref_t thread, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // get mach thread
    pthread_t pthread = thread? *((pthread_t*)thread) : pthread_self();
    thread_port_t mach_thread = pthread_mach_thread_np(pthread);
    tb_assert_and_check_return_val(mach_thread != MACH_PORT_NULL, tb_false);

    // get thread affinity
    boolean_t get_default = tb_false;
    mach_msg_type_number_t count = THREAD_AFFINITY_POLICY_COUNT;
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = 0;
    if (thread_policy_get(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, &count, &get_default) != KERN_SUCCESS)
        return tb_false;

    // save to cpuset
    TB_CPUSET_ZERO(cpuset);
    TB_CPUSET_SET(policy.affinity_tag, cpuset);
    return tb_true;
}
