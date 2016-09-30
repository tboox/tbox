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
 * @file        scheduler.h
 * @ingroup     coroutine
 *
 */
#ifndef TB_COROUTINE_SCHEDULER_H
#define TB_COROUTINE_SCHEDULER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the scheduler ref type
typedef __tb_typeref__(scheduler);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init scheduler with fifo
 *
 * @return              the scheduler 
 */
tb_scheduler_ref_t      tb_scheduler_init_with_fifo(tb_noarg_t);

/*! init scheduler with poll (for io)
 *
 * @return              the scheduler 
 */
tb_scheduler_ref_t      tb_scheduler_init_with_poll(tb_noarg_t);

/*! exit scheduler
 *
 * @param scheduler     the scheduler
 */
tb_void_t               tb_scheduler_exit(tb_scheduler_ref_t scheduler);

/*! run the scheduler loop
 *
 * @param scheduler     the scheduler
 */
tb_void_t               tb_scheduler_loop(tb_scheduler_ref_t scheduler);

/*! get the scheduler type
 *
 * @param scheduler     the scheduler
 *
 * @return              the scheduler type
 */
tb_size_t               tb_scheduler_type(tb_scheduler_ref_t scheduler);

/*! get the scheduler of the current coroutine
 *
 * @return              the scheduler
 */
tb_size_t               tb_scheduler_self(tb_noarg_t);

/*! control the scheduler
 *
 * @param scheduler     the scheduler
 * @param ctrl          the ctrl code
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_scheduler_ctrl(tb_scheduler_ref_t scheduler, tb_size_t ctrl, ...);


/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
