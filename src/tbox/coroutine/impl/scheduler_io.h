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
 * macros
 */
    
// get self io scheduler
#define tb_scheduler_io_self()      ((tb_scheduler_io_ref_t)tb_thread_local_get(&s_scheduler_io_self))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the io scheduler type
typedef struct __tb_scheduler_io_t
{
    // the maximum concurrent number
    tb_size_t           maxn;

    // is stopped?
    tb_bool_t           stop;

    // the scheduler 
    tb_scheduler_t*     scheduler;

    // the poller
    tb_poller_ref_t     poller;

}tb_scheduler_io_t, *tb_scheduler_io_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the self io scheduler local 
extern tb_thread_local_t s_scheduler_io_self;

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
