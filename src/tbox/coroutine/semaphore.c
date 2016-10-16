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
 * @file        semaphore.h
 * @ingroup     coroutine
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "semaphore"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "semaphore.h"
#include "coroutine.h"
#include "scheduler.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the coroutine semaphore type
typedef struct __tb_co_semaphore_t
{
}tb_co_semaphore_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_co_semaphore_ref_t tb_co_semaphore_init(tb_size_t value)
{
    // done
    tb_bool_t           ok = tb_false;
    tb_co_semaphore_t*  semaphore = tb_null;
    do
    {
        // make semaphore
        semaphore = tb_malloc0_type(tb_co_semaphore_t);
        tb_assert_and_check_break(semaphore);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (semaphore) tb_co_semaphore_exit((tb_co_semaphore_ref_t)semaphore);
        semaphore = tb_null;
    }

    // ok?
    return (tb_co_semaphore_ref_t)semaphore;
}
tb_void_t tb_co_semaphore_exit(tb_co_semaphore_ref_t self)
{
    // check
    tb_co_semaphore_t* semaphore = (tb_co_semaphore_t*)self;
    tb_assert_and_check_return(semaphore);

    // exit the semaphore
    tb_free(semaphore);
}
tb_bool_t tb_co_semaphore_post(tb_co_semaphore_ref_t self, tb_size_t post)
{
    return tb_false;
}
tb_long_t tb_co_semaphore_value(tb_co_semaphore_ref_t self)
{
    return 0;
}
tb_long_t tb_co_semaphore_wait(tb_co_semaphore_ref_t self, tb_long_t timeout)
{
    return 0;
}
