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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		time.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_TIME_H
#define TB_PLATFORM_TIME_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// usleep
tb_void_t 		tb_usleep(tb_size_t us);

// msleep
tb_void_t 		tb_msleep(tb_size_t ms);

// sleep
tb_void_t 		tb_sleep(tb_size_t s);

// clock, ms
tb_hong_t 		tb_mclock();

// clock, us
tb_hong_t 		tb_uclock();

// get the time from 1970-01-01 00:00:00:000
tb_bool_t 		tb_gettimeofday(tb_timeval_t* tv, tb_timezone_t* tz);

#endif
