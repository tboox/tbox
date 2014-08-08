/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_semaphore_ref_t tb_semaphore_init(tb_size_t init)
{
    // done
    tb_bool_t           ok = tb_false;
    sem_t*              semaphore = tb_null;
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

    // init time
    struct timespec t = {0};
    t.tv_sec = time(tb_null);
    if (timeout > 0)
    {
        t.tv_sec += timeout / 1000;
        t.tv_nsec += (timeout % 1000) * 1000000;
    }
    else if (timeout < 0) t.tv_sec += 12 * 30 * 24 * 3600; // infinity: one year

    // wait semaphore
    tb_long_t r = sem_timedwait(h, &t);

    // ok?
    tb_check_return_val(r, 1);

    // timeout?
    tb_check_return_val(errno != EAGAIN && errno != ETIMEDOUT, 0);

    // error
    return -1;
}

