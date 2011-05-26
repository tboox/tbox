/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		utils.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

// msleep
void tb_msleep(tb_uint32_t ms)
{
	usleep(ms * 1000);
}

// printf
void tb_printf(tb_char_t const* fmt, ...)
{
	va_list argp;
	tb_char_t msg[4096];

    va_start(argp, fmt);
    if (vsnprintf(msg, 4096, fmt, argp) < 0) msg[4096 - 1] = '\0';
    va_end(argp);

	printf("%s", msg);
}

// clock
tb_int64_t tb_clock()
{
	//printf("tb_clock\n");

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tb_int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

// uclock
tb_int64_t tb_uclock()
{
	//printf("tb_uclock\n");

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tb_int64_t)tv.tv_sec * 1000000 + tv.tv_usec);
}
tb_int64_t tb_time()
{
#if 0
	return ((tb_int64_t)time(0) * 1000);
#else
	return tb_clock();
#endif
}
