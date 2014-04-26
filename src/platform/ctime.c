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
 * @file		ctime.c
 * @ingroup 	platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ctime.h"
#include "time.h"
#include "atomic64.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the cached time
static tb_atomic64_t 	g_time = 0;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_hong_t tb_ctime_spak(tb_noarg_t)
{
	// get the time
	tb_timeval_t tv = {0};
    if (!tb_gettimeofday(&tv, tb_null)) return -1;

	// the time value
	tb_hong_t val = ((tb_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);

	// save it
	tb_atomic64_set(&g_time, val);

	// ok
	return val;
}
tb_hong_t tb_ctime_time(tb_noarg_t)
{
	return (tb_hong_t)tb_atomic64_get(&g_time);
}

