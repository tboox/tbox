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
 * @file        semaphore.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../time.h"
#include <time.h>
#include <errno.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#include <mach/mach.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the semaphore type
typedef struct __tb_semaphore_impl_t
{
    // the semaphore
    semaphore_t         semaphore;

    // the value
    tb_atomic32_t       value;

}tb_semaphore_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_semaphore_ref_t tb_semaphore_init(tb_size_t init)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_semaphore_impl_t*    impl = tb_null;
    do
    {
        // make semaphore
        impl = tb_malloc0_type(tb_semaphore_impl_t);
        tb_assert_and_check_break(impl);

        // init semaphore
        if (KERN_SUCCESS != semaphore_create(mach_task_self(), &(impl->semaphore), SYNC_POLICY_FIFO, init)) break;

        // init value
        tb_atomic32_init(&impl->value, (tb_int32_t)init);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_semaphore_exit((tb_semaphore_ref_t)impl);
        impl = tb_null;
    }

    // ok
    return (tb_semaphore_ref_t)impl;
}
tb_void_t tb_semaphore_exit(tb_semaphore_ref_t semaphore)
{
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    if (semaphore)
    {
        // exit semaphore
        semaphore_destroy(mach_task_self(), impl->semaphore);

        // exit it
        tb_free(semaphore);
    }
}
tb_bool_t tb_semaphore_post(tb_semaphore_ref_t semaphore, tb_size_t post)
{
    // check
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    tb_assert_and_check_return_val(semaphore && post, tb_false);

    // post
    while (post--)
    {
        // +2 first
        tb_atomic32_fetch_and_add(&impl->value, 2);

        // signal
        if (KERN_SUCCESS != semaphore_signal(impl->semaphore))
        {
            // restore
            tb_atomic32_fetch_and_sub(&impl->value, 2);
            return tb_false;
        }

        // -1
        tb_atomic32_fetch_and_sub(&impl->value, 1);
    }

    // ok
    return tb_true;
}
tb_long_t tb_semaphore_value(tb_semaphore_ref_t semaphore)
{
    // check
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    tb_assert_and_check_return_val(semaphore, -1);

    // get value
    return (tb_long_t)tb_atomic32_get(&impl->value);
}
tb_long_t tb_semaphore_wait(tb_semaphore_ref_t semaphore, tb_long_t timeout)
{
    // check
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    tb_assert_and_check_return_val(semaphore, -1);

    // the deadline (milliseconds)
    tb_hong_t deadline = 0;
    if (timeout > 0)
        deadline = tb_mclock() + timeout;

    kern_return_t result = KERN_SUCCESS;

    while (tb_true)
    {
        if (timeout < 0)
        {
            // infinite wait
            result = semaphore_wait(impl->semaphore);
        }
        else
        {
            // compute remaining time (milliseconds)
            tb_hong_t remain = timeout;
            if (timeout > 0)
            {
                remain = deadline - tb_mclock();
                if (remain <= 0)
                    return 0;
            }

            if (remain < 0)
                remain = 0;

            mach_timespec_t spec;
            spec.tv_sec = (unsigned int)tb_min(remain / 1000, (tb_hong_t)TB_MAXU32);
            spec.tv_nsec = (unsigned int)(((remain % 1000) < 0? 0 : (remain % 1000)) * 1000000);

            result = semaphore_timedwait(impl->semaphore, spec);
        }

        if (result == KERN_SUCCESS)
            break;

        if (result == KERN_OPERATION_TIMED_OUT)
            return 0;

        if (result == KERN_ABORTED)
        {
            // The wait was interrupted (e.g. by thread cancellation). Retry until timeout expires.
            if (!timeout)
                return 0;
            continue;
        }

        return -1;
    }

    // check value
    tb_assert_and_check_return_val((tb_long_t)tb_atomic32_get(&impl->value) > 0, -1);

    tb_atomic32_fetch_and_sub(&impl->value, 1);
    return 1;
}


