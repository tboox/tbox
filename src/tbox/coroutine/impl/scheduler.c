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
 * @file        scheduler.c
 * @ingroup     coroutine
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scheduler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_scheduler_start(tb_scheduler_t* scheduler, tb_coroutine_func_t func, tb_cpointer_t priv, tb_size_t stacksize)
{
    // check
    tb_assert(scheduler && scheduler->start && func);

    // start the coroutine function
    return scheduler->start(scheduler, func, priv, stacksize);
}
tb_void_t tb_scheduler_yield(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine)
{
    // check
    tb_assert(scheduler && scheduler->yield && coroutine);

    // yield the coroutine
    scheduler->yield(scheduler, coroutine);
}
tb_void_t tb_scheduler_sleep(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine, tb_size_t interval)
{
    // check
    tb_assert(scheduler && scheduler->sleep && coroutine);

    // sleep the coroutine
    scheduler->sleep(scheduler, coroutine, interval);
}

