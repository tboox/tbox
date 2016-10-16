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
 * @file        mutex.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "mutex.h"
#include "spinlock.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/mutex.c"
#elif defined(TB_CONFIG_POSIX_HAVE_PTHREAD_MUTEX_INIT)
#   include "posix/mutex.c"
#else
tb_mutex_ref_t tb_mutex_init()
{
    // done
    tb_bool_t           ok = tb_false;
    tb_spinlock_ref_t   lock = tb_null;
    do
    {
        // make lock
        lock = tb_malloc0_type(tb_spinlock_t);
        tb_assert_and_check_break(lock);

        // init lock
        if (!tb_spinlock_init(lock)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        tb_free(lock);
        lock = tb_null;
    }

    // ok?
    return (tb_mutex_ref_t)lock;
}
tb_void_t tb_mutex_exit(tb_mutex_ref_t mutex)
{
    // check
    tb_assert_and_check_return(mutex);

    // exit it
    tb_spinlock_ref_t lock = (tb_spinlock_ref_t)mutex;
    if (lock)
    {
        // exit lock
        tb_spinlock_exit(lock);

        // free it
        tb_free(lock);
    }
}
tb_bool_t tb_mutex_enter(tb_mutex_ref_t mutex)
{
    // check
    tb_assert_and_check_return_val(mutex, tb_false);

    // enter
    tb_spinlock_enter((tb_spinlock_ref_t)mutex);

    // ok
    return tb_true;
}
tb_bool_t tb_mutex_enter_try(tb_mutex_ref_t mutex)
{
    // check
    tb_assert_and_check_return_val(mutex, tb_false);

    // try to enter
    return tb_spinlock_enter_try((tb_spinlock_ref_t)mutex);
}
tb_bool_t tb_mutex_leave(tb_mutex_ref_t mutex)
{
    // check
    tb_assert_and_check_return_val(mutex, tb_false);

    // leave
    tb_spinlock_leave((tb_spinlock_ref_t)mutex);

    // ok
    return tb_true;
}
#endif
