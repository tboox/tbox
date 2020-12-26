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
#include "prefix.h"
#include "../thread.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_thread_setaffinity(tb_thread_ref_t thread, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // get thread
    HANDLE hthread = thread? (HANDLE)thread : GetCurrentThread();

    // set cpu affinity
    return (tb_bool_t)SetThreadAffinityMask(hthread, cpuset->_cpuset) != 0;
}
tb_bool_t tb_thread_getaffinity(tb_thread_ref_t thread, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // get thread
    HANDLE hthread = thread? (HANDLE)thread : GetCurrentThread();

    // get cpu affinity
    DWORD_PTR vThreadMask = SetThreadAffinityMask(hthread, 1);
    if (!vThreadMask) return tb_false;

    // restore cpu affinity
    if (vThreadMask != 1)
        SetThreadAffinityMask(hthread, vThreadMask);

    // save cpuset
    cpuset->_cpuset = (tb_size_t)vThreadMask;
    return tb_true;
}
