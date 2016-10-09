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
 * @file        coroutine_io.c
 * @ingroup     coroutine
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "coroutine_io"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "coroutine.h"
#include "impl/impl.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_coroutine_io_wait(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    // get the current io scheduler
    tb_scheduler_io_ref_t scheduler_io = tb_scheduler_io_self();
    tb_assert_and_check_return_val(scheduler_io && scheduler_io->poller, -1);

    // get the current scheduler
    tb_scheduler_t* scheduler = scheduler_io->scheduler;
    tb_assert(scheduler);

    // get the current coroutine
    tb_coroutine_t* coroutine = tb_scheduler_running(scheduler);
    tb_assert(coroutine);

    // TODO timeout and oneshot, clear ...

    // insert socket to poller for waiting events
    if (!tb_poller_insert(scheduler_io->poller, sock, events, coroutine))
    {
        // trace
        tb_trace_e("failed to insert sock(%p) to poller on coroutine(%p)!", sock, coroutine);

        // failed
        return tb_false;
    }

    // suspend the current coroutine and return the waited result
    return (tb_long_t)tb_scheduler_suspend(scheduler);
}
