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
#ifndef TB_COROUTINE_SCHEDULER_IO_H
#define TB_COROUTINE_SCHEDULER_IO_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scheduler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init io scheduler 
 *
 * @param maxn          the maximum concurrent number  
 *
 * @return              the scheduler 
 */
tb_scheduler_ref_t      tb_scheduler_io_init(tb_size_t maxn);

/*! stop the current io scheduler 
 */
tb_void_t               tb_scheduler_io_stop(tb_noarg_t);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
