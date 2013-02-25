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
 * @file		time.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../time.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_usleep(tb_size_t us)
{
	usleep(us);
}

tb_void_t tb_msleep(tb_size_t ms)
{
	tb_usleep(ms * 1000);
}

tb_void_t tb_sleep(tb_size_t s)
{
	tb_msleep(s * 1000);
}

tb_hong_t tb_mclock()
{
	tb_timeval_t tv = {0};
    if (!tb_gettimeofday(&tv, tb_null)) return -1;
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

tb_hong_t tb_uclock()
{
	tb_timeval_t tv = {0};
    if (!tb_gettimeofday(&tv, tb_null)) return -1;
	return (tv.tv_sec * 1000000 + tv.tv_usec);
}

tb_bool_t tb_gettimeofday(tb_timeval_t* tv, tb_timezone_t* tz)
{
	// check
	tb_assert_static(sizeof(tb_time_t) == sizeof(time_t));
	tb_assert_static(sizeof(tb_suseconds_t) == sizeof(suseconds_t));
	tb_assert_static(sizeof(tb_timeval_t) == sizeof(struct timeval));
	tb_assert_static(sizeof(tb_timezone_t) == sizeof(struct timezone));

	// ok?
	return !gettimeofday(tv, tz)? tb_true : tb_false;
}
