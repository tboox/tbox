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
#include "semaphore.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_co_lock_ref_t tb_co_lock_init()
{
    // init lock
    return (tb_co_lock_ref_t)tb_co_semaphore_init(1);
}
tb_void_t tb_co_lock_exit(tb_co_lock_ref_t self)
{
    // exit lock
    tb_co_semaphore_exit((tb_co_semaphore_ref_t)self);
}
tb_void_t tb_co_lock_enter(tb_co_lock_ref_t self)
{
    // enter lock
    tb_co_semaphore_wait((tb_co_semaphore_ref_t)self, -1);
}
tb_bool_t tb_co_lock_enter_try(tb_co_lock_ref_t self)
{
    // try to enter lock
    return tb_co_semaphore_wait((tb_co_semaphore_ref_t)self, 0) > 0;
}
tb_void_t tb_co_lock_leave(tb_co_lock_ref_t self)
{
    // leave lock
    tb_co_semaphore_post((tb_co_semaphore_ref_t)self, 1);
}
