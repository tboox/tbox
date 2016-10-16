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
 * @file        lock.h
 * @ingroup     coroutine
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "lock"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "lock.h"
#include "coroutine.h"
#include "scheduler.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the coroutine lock type
typedef struct __tb_co_lock_t
{
}tb_co_lock_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_co_lock_ref_t tb_co_lock_init()
{
    // done
    tb_bool_t       ok = tb_false;
    tb_co_lock_t*   lock = tb_null;
    do
    {
        // make lock
        lock = tb_malloc0_type(tb_co_lock_t);
        tb_assert_and_check_break(lock);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (lock) tb_co_lock_exit((tb_co_lock_ref_t)lock);
        lock = tb_null;
    }

    // ok?
    return (tb_co_lock_ref_t)lock;
}
tb_void_t tb_co_lock_exit(tb_co_lock_ref_t self)
{
    // check
    tb_co_lock_t* lock = (tb_co_lock_t*)self;
    tb_assert_and_check_return(lock);

    // exit the lock
    tb_free(lock);
}
tb_void_t tb_co_lock_enter(tb_co_lock_ref_t lock)
{
}
tb_bool_t tb_co_lock_enter_try(tb_co_lock_ref_t lock)
{
    return tb_false;
}
tb_void_t tb_co_lock_leave(tb_co_lock_ref_t lock)
{
}
