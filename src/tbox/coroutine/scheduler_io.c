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
#include "scheduler.h"
#include "coroutine.h"
#include "impl/impl.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the self io scheduler local 
tb_thread_local_t s_scheduler_io_self = TB_THREAD_LOCAL_INIT;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_scheduler_io_wait(tb_poller_ref_t poller, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_assert(poller && sock && priv);

    // get scheduler
    tb_scheduler_t* scheduler = (tb_scheduler_t*)tb_poller_priv(poller);
    tb_assert(scheduler);

    // remove this socket from poller
    tb_poller_remove(poller, sock);

    // resume the coroutine of this socket
    tb_scheduler_resume(scheduler, (tb_coroutine_t*)priv);
}
static tb_void_t tb_scheduler_io_exit(tb_scheduler_io_ref_t scheduler_io)
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

    // init io scheduler local
    if (!tb_thread_local_init(&s_scheduler_io_self, tb_null)) return ;
 
    // attach io scheduler to self
    tb_thread_local_set(&s_scheduler_io_self, scheduler_io);

    // loop
    while (1)
    {
        // finish all other ready coroutines first
        while (tb_scheduler_yield(scheduler)) {}

        // no more ready coroutines? wait io events and timers
        tb_poller_wait(poller, tb_scheduler_io_wait, -1);

        // stop?
        tb_check_break(!scheduler_io->stop);
    }
 
    // detach io scheduler to self
    tb_thread_local_set(&s_scheduler_io_self, tb_null);

    // exit this io scheduler
    tb_scheduler_io_exit(scheduler_io);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_scheduler_ref_t tb_scheduler_io_init(tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(maxn, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_scheduler_ref_t      scheduler = tb_null;
    tb_scheduler_io_ref_t   scheduler_io = tb_null;
    do
    {
        // init scheduler
        scheduler = tb_scheduler_init();
        tb_assert_and_check_break(scheduler);

        // init io scheduler
        scheduler_io = tb_malloc0_type(tb_scheduler_io_t);
        tb_assert_and_check_break(scheduler_io);

        // save maxn
        scheduler_io->maxn = maxn;

        // save scheduler
        scheduler_io->scheduler = (tb_scheduler_t*)scheduler;

        // init poller
        scheduler_io->poller = tb_poller_init(maxn, scheduler_io->scheduler);
        tb_assert_and_check_break(scheduler_io->poller);

        // start the io loop coroutine (must be the first running coroutine)
        if (!tb_coroutine_start(scheduler, tb_scheduler_io_loop, scheduler_io, 0)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit io scheduler
        if (scheduler_io) tb_scheduler_io_exit(scheduler_io);
        scheduler_io = tb_null;

        // exit scheduler
        if (scheduler) tb_scheduler_exit(scheduler);
        scheduler = tb_null;
    }

    // ok?
    return scheduler;
}
tb_void_t tb_scheduler_io_stop()
{
    // get self io scheduler
    tb_scheduler_io_ref_t scheduler_io = tb_scheduler_io_self();
    tb_assert_and_check_return(scheduler_io);

    // stop it
    scheduler_io->stop = tb_true;
}
