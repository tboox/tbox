/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
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
#include "../../memory/fixed_pool.h"
#include "../../platform/poller.h"
#include "../../platform/impl/pollerdata.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the io poller data type
typedef struct __tb_co_pollerdata_io_t
{
    // the suspended coroutine for waiting poller/recv
    tb_coroutine_t*     co_recv;

    // the suspended coroutine for waiting poller/send
    tb_coroutine_t*     co_send;

    // the waited events for poller
    tb_uint16_t         poller_events_wait;

    // the saved events for poller (triggered)
    tb_uint16_t         poller_events_save;

}tb_co_pollerdata_io_t, *tb_co_pollerdata_io_ref_t;

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

    // the poller data
    tb_pollerdata_t     pollerdata;

    // the poller data pool
    tb_fixed_pool_ref_t pollerdata_pool;

}tb_co_scheduler_io_t, *tb_co_scheduler_io_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init io scheduler
 *
 * @param scheduler         the scheduler
 *
 * @return                  the io scheduler
 */
tb_co_scheduler_io_ref_t    tb_co_scheduler_io_init(tb_co_scheduler_t* scheduler);

/*! exit io scheduler
 *
 * @param scheduler_io      the io scheduler
 */
tb_void_t                   tb_co_scheduler_io_exit(tb_co_scheduler_io_ref_t scheduler_io);

/* need io scheduler
 *
 * ensure the io scheduler has been initialized
 *
 * @param scheduler         the scheduler, get self scheduler if be null
 *
 * @return                  the io scheduler
 */
tb_co_scheduler_io_ref_t    tb_co_scheduler_io_need(tb_co_scheduler_t* scheduler);

/* kill the current io scheduler
 *
 * @param scheduler_io      the io scheduler
 */
tb_void_t                   tb_co_scheduler_io_kill(tb_co_scheduler_io_ref_t scheduler_io);

/* sleep the current coroutine
 *
 * @param scheduler_io      the io scheduler
 * @param interval          the interval (ms), infinity: -1
 *
 * @return                  the user private data from resume(priv)
 */
tb_pointer_t                tb_co_scheduler_io_sleep(tb_co_scheduler_io_ref_t scheduler_io, tb_long_t interval);

/*! wait io events
 *
 * @param scheduler_io      the io scheduler
 * @param object            the poller object, socket or pipe
 * @param events            the waited events
 * @param timeout           the timeout, infinity: -1
 *
 * @return                  > 0: the events, 0: timeout, -1: failed
 */
tb_long_t                   tb_co_scheduler_io_wait(tb_co_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object, tb_size_t events, tb_long_t timeout);

/*! wait process status
 *
 * @param scheduler_io      the io scheduler
 * @param object            the process poller object
 * @param pstatus           the process exited status pointer, maybe null
 * @param timeout           the timeout, infinity: -1
 *
 * @return                  > 0: process exited, 0: timeout, -1: failed
 */
tb_long_t                   tb_co_scheduler_io_wait_proc(tb_co_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object, tb_long_t* pstatus, tb_long_t timeout);

/*! wait fwatcher event
 *
 * @param scheduler_io      the io scheduler
 * @param object            the process poller object
 * @param pevent            the fwatcher event pointer
 * @param timeout           the timeout, infinity: -1
 *
 * @return                  > 0: has event, 0: timeout, -1: failed
 */
tb_long_t                   tb_co_scheduler_io_wait_fwatcher(tb_co_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object, tb_fwatcher_event_t* pevent, tb_long_t timeout);

/*! cancel io events for the given poller object
 *
 * @param scheduler_io      the io scheduler
 * @param object            the poller object
 *
 * @return                  tb_true or tb_false
 */
tb_bool_t                   tb_co_scheduler_io_cancel(tb_co_scheduler_io_ref_t scheduler_io, tb_poller_object_ref_t object);

/* get the current io scheduler
 *
 * @return                  the io scheduler
 */
tb_co_scheduler_io_ref_t    tb_co_scheduler_io_self(tb_noarg_t);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
