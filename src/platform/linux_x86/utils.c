/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		utils.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../math/math.h"
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

// usleep
tb_void_t tb_usleep(tb_size_t us)
{
	usleep(us);
}

// msleep
tb_void_t tb_msleep(tb_size_t ms)
{
	tb_usleep(ms * 1000);
}

// sleep
tb_void_t tb_sleep(tb_size_t s)
{
	tb_msleep(s * 1000);
}

// printf
tb_void_t tb_printf(tb_char_t const* fmt, ...)
{
	va_list argp;
	tb_char_t msg[4096];

    va_start(argp, fmt);
    if (vsnprintf(msg, 4096, fmt, argp) < 0) msg[4096 - 1] = '\0';
    va_end(argp);

	printf("%s", msg);
}

// mclock
tb_int64_t tb_mclock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
	tb_int64_t ms = tb_int64_mul_int32(tb_int32_to_int64(tv.tv_sec), 1000);
	return tb_int64_add_int32(ms, tv.tv_usec / 1000);
}

// uclock
tb_int64_t tb_uclock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
	tb_int64_t us = tb_int64_mul_int32(tb_int32_to_int64(tv.tv_sec), 1000000);
	return tb_int64_add_int32(us, tv.tv_usec);
}
tb_int64_t tb_time()
{
#if 0
	return ((tb_int64_t)time(0) * 1000);
#else
	return tb_mclock();
#endif
}


