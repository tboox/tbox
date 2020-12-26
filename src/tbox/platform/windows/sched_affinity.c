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
#include "prefix.h"
#include "../sched.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_sched_setaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // get the current pid
    if (!pid) pid = (tb_size_t)GetCurrentProcessId();

    // set affinity
    tb_bool_t ok = tb_false;
    HANDLE    process = tb_null;
    do
    {
        // open process
        process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION, FALSE, (DWORD)pid);
        tb_assert_and_check_break(process);

        // get process affinity
        DWORD_PTR vProcessMask;
        DWORD_PTR vSystemMask;
        if (!GetProcessAffinityMask(process, &vProcessMask, &vSystemMask)) break;

        // result is the intersection of available CPUs and the mask.
        DWORD_PTR newMask = vSystemMask & cpuset->_cpuset;
        if (newMask)
        {
            if (SetProcessAffinityMask(process, newMask) == 0)
                break;
        }

        // ok
        ok = tb_true;

    } while (0);

    // close process
    if (process) CloseHandle(process);
    process = tb_null;

    // ok?
    return ok;
}
tb_bool_t tb_sched_getaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // get the current pid
    if (!pid) pid = (tb_size_t)GetCurrentProcessId();

    // set affinity
    tb_bool_t ok = tb_false;
    HANDLE    process = tb_null;
    do
    {
        // open process
        process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION, FALSE, (DWORD)pid);
        tb_assert_and_check_break(process);

        // get process affinity
        DWORD_PTR vProcessMask;
        DWORD_PTR vSystemMask;
        if (!GetProcessAffinityMask(process, &vProcessMask, &vSystemMask)) break;

        // save result
        cpuset->_cpuset = vProcessMask;

        // ok
        ok = tb_true;

    } while (0);

    // close process
    if (process) CloseHandle(process);
    process = tb_null;

    // ok?
    return ok;
}
