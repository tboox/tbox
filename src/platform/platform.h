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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		platform.h
 * @defgroup 	platform
 *
 */
#ifndef TB_PLATFORM_H
#define TB_PLATFORM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "dns.h"
#include "path.h"
#include "file.h"
#include "time.h"
#include "mutex.h"
#include "event.h"
#include "ctime.h"
#include "timer.h"
#include "print.h"
#include "ltimer.h"
#include "socket.h"
#include "thread.h"
#include "atomic.h"
#include "memory.h"
#include "barrier.h"
#include "dynamic.h"
#include "process.h"
#include "spinlock.h"
#include "atomic64.h"
#include "hostname.h"
#include "semaphore.h"
#include "backtrace.h"
#include "directory.h"
#include "exception.h"
#include "thread_pool.h"
#include "thread_store.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*!init the platform
 *
 * @return tb_true or tb_false
 */
tb_bool_t 	tb_platform_init(tb_noarg_t);

/// exit the platform 
tb_void_t 	tb_platform_exit(tb_noarg_t);

#endif
