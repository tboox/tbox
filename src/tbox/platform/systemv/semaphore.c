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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        semaphore.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_semaphore_ref_t tb_semaphore_init(tb_size_t init)
{
    // init semaphore
    tb_long_t h = semget((key_t)IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666);
    tb_assert_and_check_return_val(h >= 0 || errno == EEXIST, tb_null);

    // exists?
    if (errno == EEXIST)
    {
        h = semget((key_t)IPC_PRIVATE, 1, 0);
        tb_assert_and_check_return_val(h >= 0, tb_null);
    }

    // init value
#if 0
    union semun opts;
    opts.val = init;
#else
    union semun_u 
    {
        tb_int_t            val;
        struct semid_ds*    buf;
        tb_uint16_t*        array;
        struct seminfo*     __buf;
        tb_pointer_t        __pad;

    }opts;
    opts.val = init;
#endif
    if (semctl(h, 0, SETVAL, opts) < 0)
    {
        tb_semaphore_exit((tb_semaphore_ref_t)(h + 1));
        return tb_null;
    }

    // ok
    return (tb_semaphore_ref_t)(h + 1);
}
tb_void_t tb_semaphore_exit(tb_semaphore_ref_t semaphore)
{
    // check
    tb_long_t h = (tb_long_t)semaphore - 1;
    tb_assert_and_check_return(semaphore);

    // remove semaphore
    tb_long_t r = semctl(h, 0, IPC_RMID);
    tb_assert(r != -1);
}
tb_bool_t tb_semaphore_post(tb_semaphore_ref_t semaphore, tb_size_t post)
{
    // check
    tb_long_t h = (tb_long_t)semaphore - 1;
    tb_assert_and_check_return_val(semaphore && post, tb_false);

    // post
    while (post--)
    {
        // init
        struct sembuf sb;
        sb.sem_num = 0;
        sb.sem_op = 1;
        sb.sem_flg = SEM_UNDO;

        // post it
        if (semop(h, &sb, 1) < 0) return tb_false;
    }

    // ok
    return tb_true;
}
tb_long_t tb_semaphore_value(tb_semaphore_ref_t semaphore)
{
    // check
    tb_long_t h = (tb_long_t)semaphore - 1;
    tb_assert_and_check_return_val(semaphore, -1);

    // get value
    return semctl(h, 0, GETVAL, 0);
}
tb_long_t tb_semaphore_wait(tb_semaphore_ref_t semaphore, tb_long_t timeout)
{
    // check
    tb_long_t h = (tb_long_t)semaphore - 1;
    tb_assert_and_check_return_val(semaphore, -1);

    // init time
    struct timeval t = {0};
    if (timeout > 0)
    {
        t.tv_sec = timeout / 1000;
        t.tv_usec = (timeout % 1000) * 1000;
    }

    // init
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    // wait semaphore
    tb_long_t r = semtimedop(h, &sb, 1, timeout >= 0? &t : tb_null);

    // ok?
    tb_check_return_val(r, 1);

    // timeout?
    tb_check_return_val(errno != EAGAIN, 0);

    // error
    return -1;
}
