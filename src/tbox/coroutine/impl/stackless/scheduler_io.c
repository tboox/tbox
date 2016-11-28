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
#include "../../stackless/coroutine.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_lo_scheduler_io_events(tb_poller_ref_t poller, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
}
static tb_void_t tb_lo_scheduler_io_loop(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_lo_scheduler_io_ref_t scheduler_io = (tb_lo_scheduler_io_ref_t)priv;
    tb_assert(scheduler_io && scheduler_io->poller);

    // the scheduler
    tb_lo_scheduler_t* scheduler = scheduler_io->scheduler;
    tb_assert(scheduler);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine);

    // loop
    while (!scheduler->stopped)
    {
        // finish all other ready coroutines first
        while (tb_lo_scheduler_ready_count(scheduler) > 1)
            tb_lo_coroutine_yield();

        // no more suspended coroutines? loop end
        tb_check_break(tb_lo_scheduler_suspend_count(scheduler));

        // trace
        tb_trace_d("loop: wait 1000 ms ..");

        // no more ready coroutines? wait io events and timers (TODO)
        if (tb_poller_wait(scheduler_io->poller, tb_lo_scheduler_io_events, 1000) < 0) break;
    }

    // leave coroutine
    tb_lo_coroutine_leave();
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_lo_scheduler_io_ref_t tb_lo_scheduler_io_init(tb_lo_scheduler_t* scheduler)
{
    // done
    tb_bool_t                   ok = tb_false;
    tb_lo_scheduler_io_ref_t    scheduler_io = tb_null;
    do
    {
        // init io scheduler
        scheduler_io = tb_malloc0_type(tb_lo_scheduler_io_t);
        tb_assert_and_check_break(scheduler_io);

        // save scheduler
        scheduler_io->scheduler = (tb_lo_scheduler_t*)scheduler;

        // init poller
        scheduler_io->poller = tb_poller_init(tb_null);
        tb_assert_and_check_break(scheduler_io->poller);

        // start the io loop coroutine
        if (!tb_lo_coroutine_start((tb_lo_scheduler_ref_t)scheduler, tb_lo_scheduler_io_loop, scheduler_io)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit io scheduler
        if (scheduler_io) tb_lo_scheduler_io_exit(scheduler_io);
        scheduler_io = tb_null;
    }

    // ok?
    return scheduler_io;
}
tb_void_t tb_lo_scheduler_io_exit(tb_lo_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert_and_check_return(scheduler_io);

    // exit poller
    if (scheduler_io->poller) tb_poller_exit(scheduler_io->poller);
    scheduler_io->poller = tb_null;

    // clear scheduler
    scheduler_io->scheduler = tb_null;

    // exit it
    tb_free(scheduler_io);
}
tb_void_t tb_lo_scheduler_io_kill(tb_lo_scheduler_io_ref_t scheduler_io)
{
    // check
    tb_assert_and_check_return(scheduler_io);

    // trace
    tb_trace_d("kill: ..");

    // kill poller
    if (scheduler_io->poller) tb_poller_kill(scheduler_io->poller);
}
tb_void_t tb_lo_scheduler_io_sleep(tb_lo_scheduler_io_ref_t scheduler_io, tb_long_t interval)
{
    // TODO
    tb_trace_noimpl();
}
#if 0
tb_long_t tb_lo_scheduler_io_wait(tb_lo_scheduler_io_ref_t scheduler_io, tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(scheduler_io && sock && scheduler_io->poller && scheduler_io->scheduler, -1);

    // get the current coroutine
    tb_lo_coroutine_t* coroutine = tb_lo_scheduler_running(scheduler_io->scheduler);
    tb_assert(coroutine);

    // trace
    tb_trace_d("coroutine(%p): wait events(%lu) with %ld ms for socket(%p) ..", coroutine, events, timeout, sock);

    // no events? remove the this socket from poller
    tb_socket_ref_t sock_prev = coroutine->rs.waiting.sock;
    if (!events && sock_prev == sock)
    {
        // remove the previous socket first if exists
        if (!tb_poller_remove(scheduler_io->poller, sock))
        {
            // trace
            tb_trace_e("failed to remove sock(%p) to poller on coroutine(%p)!", sock, coroutine);

            // failed
            return -1;
        }

        // remove ok
        return 0;
    }

    // enable edge-trigger mode if be supported
    if (tb_poller_support(scheduler_io->poller, TB_POLLER_EVENT_CLEAR))
        events |= TB_POLLER_EVENT_CLEAR;

    // exists this socket? only modify events 
    if (sock_prev == sock)
    {
        // return the cached events directly if the waiting events exists cache
        tb_size_t events_prev   = coroutine->rs.waiting.events;
        tb_size_t events_cache  = coroutine->rs.waiting.events_cache;
        if (events_cache && (events_prev & events))
        {
            // clear cache events
            coroutine->rs.waiting.events_cache &= ~events;

            // return the cached events
            return events_cache & events;
        }

        // modify socket from poller for waiting events if the waiting events has been changed 
        if (events_prev != events && !tb_poller_modify(scheduler_io->poller, sock, events, coroutine))
        {
            // trace
            tb_trace_e("failed to modify sock(%p) to poller on coroutine(%p)!", sock, coroutine);

            // failed
            return -1;
        }
    }
    else
    {
        // remove the previous socket first if exists
        if (sock_prev && !tb_poller_remove(scheduler_io->poller, sock_prev))
        {
            // trace
            tb_trace_e("failed to remove sock(%p) to poller on coroutine(%p)!", sock_prev, coroutine);

            // failed
            return -1;
        }

        // insert socket to poller for waiting events
        if (!tb_poller_insert(scheduler_io->poller, sock, events, coroutine))
        {
            // trace
            tb_trace_e("failed to insert sock(%p) to poller on coroutine(%p)!", sock, coroutine);

            // failed
            return -1;
        }
    }

    // save the socket to coroutine for the timer function
    coroutine->rs.waiting.sock = sock;

    // save waiting events to coroutine
    coroutine->rs.waiting.events        = (tb_uint16_t)events;
    coroutine->rs.waiting.events_cache  = 0;

    // mark as waiting state
    coroutine->rs.waiting.waiting       = 1;

    // suspend the current coroutine and return the waited result
    return (tb_long_t)tb_lo_scheduler_suspend(scheduler_io->scheduler, tb_null);
}
#endif
