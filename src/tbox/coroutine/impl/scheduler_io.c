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
 * @file        scheduler_io.c
 * @ingroup     coroutine
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "scheduler_io"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scheduler_io.h"
#include "coroutine.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_scheduler_io_events(tb_poller_ref_t poller, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_assert(poller && sock && priv);

    // get scheduler
    tb_scheduler_t* scheduler = (tb_scheduler_t*)tb_poller_priv(poller);
    tb_assert(scheduler);

    // remove this socket from poller
    tb_poller_remove(poller, sock);

    // resume the coroutine of this socket and pass the events to suspend()
    tb_scheduler_resume(scheduler, (tb_coroutine_t*)priv, (tb_cpointer_t)events);
}
static tb_void_t tb_scheduler_io_loop(tb_cpointer_t priv)
{
    // check
    tb_scheduler_io_ref_t scheduler_io = (tb_scheduler_io_ref_t)priv;
    tb_assert_and_check_return(scheduler_io);

    // the scheduler
    tb_scheduler_t* scheduler = scheduler_io->scheduler;
    tb_assert_and_check_return(scheduler);

    // the poller
    tb_poller_ref_t poller = scheduler_io->poller;
    tb_assert_and_check_return(poller);

    // loop
    while (!scheduler_io->stop)
    {
        // finish all other ready coroutines first
        while (tb_scheduler_yield(scheduler)) {}

        // TODO timer


        // no more ready coroutines? wait io events and timers
        if (tb_poller_wait(poller, tb_scheduler_io_events, -1) < 0) break;

    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_scheduler_io_ref_t tb_scheduler_io_init(tb_scheduler_t* scheduler)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_scheduler_io_ref_t   scheduler_io = tb_null;
    do
    {
        // init io scheduler
        scheduler_io = tb_malloc0_type(tb_scheduler_io_t);
        tb_assert_and_check_break(scheduler_io);

        // save scheduler
        scheduler_io->scheduler = (tb_scheduler_t*)scheduler;

        // TODO maxfd
        // init poller
        scheduler_io->poller = tb_poller_init(0, scheduler_io->scheduler);
        tb_assert_and_check_break(scheduler_io->poller);

        // start the io loop coroutine
        if (!tb_scheduler_start(scheduler_io->scheduler, tb_scheduler_io_loop, scheduler_io, 0)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit io scheduler
        if (scheduler_io) tb_scheduler_io_exit(scheduler_io);
        scheduler_io = tb_null;
    }

    // ok?
    return scheduler_io;
}
tb_void_t tb_scheduler_io_exit(tb_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert_and_check_return(scheduler_io);

    // must be stopped
    tb_assert(scheduler_io->stop);

    // exit poller
    if (scheduler_io->poller) tb_poller_exit(scheduler_io->poller);
    scheduler_io->poller = tb_null;

    // clear scheduler
    scheduler_io->scheduler = tb_null;

    // exit it
    tb_free(scheduler_io);
}
tb_void_t tb_scheduler_io_stop(tb_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert_and_check_return(scheduler_io);

    // stop it
    scheduler_io->stop = tb_true;

    // kill poller
    if (scheduler_io->poller) tb_poller_kill(scheduler_io->poller);
}
tb_cpointer_t tb_scheduler_io_sleep(tb_scheduler_io_ref_t scheduler_io, tb_size_t interval)
{
    return tb_null;
}
tb_long_t tb_scheduler_io_wait(tb_scheduler_io_ref_t scheduler_io, tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    // check
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
