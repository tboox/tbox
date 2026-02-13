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
#ifdef __NetBSD__
#include <sched.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_sched_setaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

#ifdef __NetBSD__
    // NetBSD uses cpuset_t API for process affinity
    cpuset_t *cpu_set = cpuset_create();
    if (!cpu_set)
        return tb_false;

    tb_int_t i;
    for (i = 0; i < TB_CPUSET_SIZE; i++)
    {
        if (TB_CPUSET_ISSET(i, cpuset))
            cpuset_set(i, cpu_set);
    }
    tb_bool_t ok = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, (pid_t)pid, cpuset_size(cpu_set), cpu_set) == 0;
    cpuset_destroy(cpu_set);
    return ok;
#elif defined(TB_CONFIG_OS_HAIKU)
    tb_int_t i;
    cpuset_t cpu_set;
    CPUSET_ZERO(&cpu_set);
    for (i = 0; i < TB_CPUSET_SIZE; i++)
    {
        if (TB_CPUSET_ISSET(i, cpuset))
            CPUSET_SET(i, &cpu_set);
    }
    return sched_setaffinity((pid_t)pid, sizeof(cpuset_t), &cpu_set) == 0;
#else
    // Linux uses cpu_set_t API
    tb_int_t i;
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    for (i = 0; i < TB_CPUSET_SIZE; i++)
    {
        if (TB_CPUSET_ISSET(i, cpuset) && i < CPU_SETSIZE)
            CPU_SET(i, &cpu_set);
    }
    return sched_setaffinity((pid_t)pid, sizeof(cpu_set_t), &cpu_set) == 0;
#endif
}
tb_bool_t tb_sched_getaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset)
{
    // check
    tb_assert_and_check_return_val(cpuset, tb_false);

#ifdef __NetBSD__
    // NetBSD uses cpuset_t API for process affinity
    cpuset_t *cpu_set = cpuset_create();
    if (!cpu_set)
        return tb_false;

    if (cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, (pid_t)pid, cpuset_size(cpu_set), cpu_set) != 0)
    {
        cpuset_destroy(cpu_set);
        return tb_false;
    }

    // save cpuset
    tb_int_t i;
    TB_CPUSET_ZERO(cpuset);
    for (i = 0; i < TB_CPUSET_SIZE; i++)
    {
        if (cpuset_isset(i, cpu_set))
            TB_CPUSET_SET(i, cpuset);
    }
    cpuset_destroy(cpu_set);
    return tb_true;
#elif defined(TB_CONFIG_OS_HAIKU)
    cpuset_t cpu_set;
    CPUSET_ZERO(&cpu_set);
    if (sched_getaffinity((pid_t)pid, sizeof(cpuset_t), &cpu_set) != 0)
        return tb_false;

    // save cpuset
    tb_int_t i;
    TB_CPUSET_ZERO(cpuset);
    for (i = 0; i < TB_CPUSET_SIZE; i++)
    {
        if (CPUSET_ISSET(i, &cpu_set))
            TB_CPUSET_SET(i, cpuset);
    }
    return tb_true;
#else
    // Linux uses cpu_set_t API
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
#endif
}
