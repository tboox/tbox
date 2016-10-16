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
 * @file        scheduler_io.h
 * @ingroup     coroutine
 *
 */
#ifndef TB_COROUTINE_IMPL_SCHEDULER_IO_H
#define TB_COROUTINE_IMPL_SCHEDULER_IO_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scheduler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the io scheduler type
typedef struct __tb_co_scheduler_io_t
{
    // is stopped?
    tb_bool_t           stop;

    // the scheduler 
    tb_co_scheduler_t*  scheduler;

    // the poller
    tb_poller_ref_t     poller;

    // the timer
    tb_timer_ref_t      timer;

    // the low-precision timer (faster)
    tb_ltimer_ref_t     ltimer;

}tb_co_scheduler_io_t, *tb_co_scheduler_io_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init io scheduler 
 *
 * @return                  the io scheduler 
 */
tb_co_scheduler_io_ref_t       tb_co_scheduler_io_init(tb_co_scheduler_t* scheduler);

/*! exit io scheduler 
 *
 * @param scheduler_io      the io scheduler
 */
tb_void_t                   tb_co_scheduler_io_exit(tb_co_scheduler_io_ref_t scheduler_io);

/*! kill the current io scheduler 
 *
 * @param scheduler_io      the io scheduler
 */
tb_void_t                   tb_co_scheduler_io_kill(tb_co_scheduler_io_ref_t scheduler_io);

/* sleep the current coroutine
 *
 * @param scheduler_io      the io scheduler
 * @param interval          the interval (ms)
 *
 * @return                  the user private data from resume(priv)
 */
tb_cpointer_t               tb_co_scheduler_io_sleep(tb_co_scheduler_io_ref_t scheduler_io, tb_size_t interval);

/*! wait io events 
 *
 * @param scheduler_io      the io scheduler
 * @param sock              the socket
 * @param events            the waited events
 * @param timeout           the timeout, infinity: -1
 *
 * @return                  > 0: the events, 0: timeout, -1: failed
 */
tb_long_t                   tb_co_scheduler_io_wait(tb_co_scheduler_io_ref_t scheduler_io, tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
