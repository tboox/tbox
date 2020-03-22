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
 * @file        process_group.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "interface/interface.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_process_group_ref_t tb_process_group_init(tb_char_t const* name)
{
    HANDLE job = tb_null;
    do
    {
        // get unicode name
        tb_wchar_t name_w[MAX_PATH];
        if (name)
        {
            tb_size_t name_n = tb_atow(name_w, name, MAX_PATH);
            tb_assert_and_check_break(name_n != (tb_size_t)-1);
        }

        // create process job 
        job = tb_kernel32()->CreateJobObjectW(tb_null, name? name_w : tb_null);
        tb_assert_and_check_break(job && job != INVALID_HANDLE_VALUE);

        // set job limits, kill all processes on job when the job is destroyed.
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_limits;
        memset(&job_limits, 0, sizeof(job_limits));
        job_limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        tb_kernel32()->SetInformationJobObject(job, JobObjectExtendedLimitInformation, &job_limits, sizeof(job_limits));

    } while (0);
    return (tb_process_group_ref_t)job;
}
tb_void_t tb_process_group_exit(tb_process_group_ref_t group)
{
}
tb_void_t tb_process_group_kill(tb_process_group_ref_t group)
{
    // check
    tb_assert_and_check_return(group);

    // kill all processes on job
    tb_kernel32()->TerminateJobObject((HANDLE)group, 0);
}
