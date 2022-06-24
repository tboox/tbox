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
#include <string.h>
#include <sched.h>
#include <sys/types.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_sched_setaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // set cpu affinity
    tb_int_t i;
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    for (i = 0; i < TB_CPUSET_SIZE; i++)
    {
        if (TB_CPUSET_ISSET(i, cpuset) && i < CPU_SETSIZE)
            CPU_SET(i, &cpu_set);
    }
    return sched_setaffinity((pid_t)pid, sizeof(cpu_set_t), &cpu_set) == 0;
}
tb_bool_t tb_sched_getaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

    // get cpu affinity
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    if (sched_getaffinity((pid_t)pid, sizeof(cpu_set_t), &cpu_set) != 0)
        return tb_false;

    // save cpuset
    tb_int_t i;
    TB_CPUSET_ZERO(cpuset);
    for (i = 0; i < CPU_SETSIZE; i++)
    {
        if (CPU_ISSET(i, &cpu_set) && i < TB_CPUSET_SIZE)
            TB_CPUSET_SET(i, cpuset);
    }
    return tb_true;
}
