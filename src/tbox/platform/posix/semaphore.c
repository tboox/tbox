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
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include "../time.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_semaphore_ref_t tb_semaphore_init(tb_size_t init)
{
    // done
    tb_bool_t   ok = tb_false;
    sem_t*      semaphore = tb_null;
    do
    {
        // make semaphore
        semaphore = tb_malloc0_type(sem_t);
        tb_assert_and_check_break(semaphore);

        // init
        if (sem_init(semaphore, 0, init) < 0) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (semaphore) tb_free(semaphore);
        semaphore = tb_null;
    }

    // ok?
    return (tb_semaphore_ref_t)semaphore;
}
tb_void_t tb_semaphore_exit(tb_semaphore_ref_t semaphore)
{
    // check
    sem_t* h = (sem_t*)semaphore;
    tb_assert_and_check_return(h);

    // exit it
    sem_destroy(h);

    // free it
    tb_free(h);
}
tb_bool_t tb_semaphore_post(tb_semaphore_ref_t semaphore, tb_size_t post)
{
    // check
    sem_t* h = (sem_t*)semaphore;
    tb_assert_and_check_return_val(h && post, tb_false);

    // post
    while (post--)
    {
        if (sem_post(h) < 0) return tb_false;
    }

    // ok
    return tb_true;
}
tb_long_t tb_semaphore_value(tb_semaphore_ref_t semaphore)
{
    // check
    sem_t* h = (sem_t*)semaphore;
    tb_assert_and_check_return_val(h, -1);

    // get value
    tb_int_t value = 0;
    return (!sem_getvalue(h, &value))? (tb_long_t)value : -1;
}
tb_long_t tb_semaphore_wait(tb_semaphore_ref_t semaphore, tb_long_t timeout)
{
    // check
    sem_t* h = (sem_t*)semaphore;
    tb_assert_and_check_return_val(h, -1);

    // non-blocking?
    if (!timeout)
    {
        while (sem_trywait(h))
        {
            if (errno == EINTR) continue;
            return (errno == EAGAIN)? 0 : -1;
        }
        return 1;
    }

    // infinite wait? use very large timeout and retry on timeout/interrupt
    if (timeout < 0)
    {
        while (tb_true)
        {
            struct timespec ts;
            if (clock_gettime(CLOCK_REALTIME, &ts))
                return -1;

            ts.tv_sec += (time_t)(30 * 24 * 3600); // one month ahead
            // ensure nsec stays normalized
            if (ts.tv_nsec >= 1000000000L)
            {
                ts.tv_sec += ts.tv_nsec / 1000000000L;
                ts.tv_nsec %= 1000000000L;
            }

            if (!sem_timedwait(h, &ts))
                return 1;

            if (errno == EINTR)
                continue;

            if (errno == ETIMEDOUT || errno == EAGAIN)
                continue; // we treat as infinite wait

            return -1;
        }
    }

    // finite timeout, loop until deadline or success
    tb_hong_t deadline = tb_mclock() + timeout;
    while (tb_true)
    {
        tb_hong_t remain = deadline - tb_mclock();
        if (remain <= 0) return 0;

        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts))
            return -1;

        ts.tv_sec += (time_t)(remain / 1000);
        ts.tv_nsec += (long)((remain % 1000) * 1000000);
        if (ts.tv_nsec >= 1000000000L)
        {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000L;
        }

        if (!sem_timedwait(h, &ts))
            return 1;

        if (errno == EINTR)
            continue;

        if (errno == ETIMEDOUT || errno == EAGAIN)
            return 0;

        return -1;
    }
}

