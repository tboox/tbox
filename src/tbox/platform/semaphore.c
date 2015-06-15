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
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "semaphore.h"
#include "time.h"
#include "cache_time.h"
#include "atomic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/semaphore.c"
#elif defined(TB_CONFIG_OS_MACOSX) || defined(TB_CONFIG_OS_IOS)
#   include "mach/semaphore.c"
#elif defined(TB_CONFIG_API_HAVE_POSIX)
#   include "posix/semaphore.c"
#elif defined(TB_CONFIG_API_HAVE_SYSTEMV)
#   include "systemv/semaphore.c"
#else 
tb_semaphore_ref_t tb_semaphore_init(tb_size_t init)
{
    // make
    tb_atomic_t* semaphore = tb_malloc0_type(tb_atomic_t);
    tb_assert_and_check_return_val(semaphore, tb_null);

    // init
    *semaphore = init;

    // ok
    return (tb_semaphore_ref_t)semaphore;
}
tb_void_t tb_semaphore_exit(tb_semaphore_ref_t semaphore)
{
    // check
    tb_atomic_t* semaphore = (tb_atomic_t*)semaphore;
    tb_assert_and_check_return(semaphore);

    // free it
    tb_free(semaphore);
}
tb_bool_t tb_semaphore_post(tb_semaphore_ref_t semaphore, tb_size_t post)
{
    // check
    tb_atomic_t* semaphore = (tb_atomic_t*)semaphore;
    tb_assert_and_check_return_val(semaphore && post, tb_false);

    // post it
    tb_long_t value = tb_atomic_fetch_and_add(semaphore, post);

    // ok
    return value >= 0? tb_true : tb_false;
}
tb_long_t tb_semaphore_value(tb_semaphore_ref_t semaphore)
{
    // check
    tb_atomic_t* semaphore = (tb_atomic_t*)semaphore;
    tb_assert_and_check_return_val(semaphore, tb_false);

    // get value
    return (tb_long_t)tb_atomic_get(semaphore);
}
tb_long_t tb_semaphore_wait(tb_semaphore_ref_t semaphore, tb_long_t timeout)
{
    // check
    tb_atomic_t* semaphore = (tb_atomic_t*)semaphore;
    tb_assert_and_check_return_val(semaphore, -1);

    // init
    tb_long_t   r = 0;
    tb_hong_t   base = tb_cache_time_spak();

    // wait 
    while (1)
    {
        // get post
        tb_long_t post = (tb_long_t)tb_atomic_get(semaphore);

        // has signal?
        if (post > 0) 
        {
            // semaphore--
            tb_atomic_fetch_and_dec(semaphore);

            // ok
            r = post;
            break;
        }
        // no signal?
        else if (!post)
        {
            // timeout?
            if (timeout >= 0 && tb_cache_time_spak() - base >= timeout) break;
            else tb_msleep(200);
        }
        // error
        else
        {
            r = -1;
            break;
        }
    }

    return r;
}
#endif

