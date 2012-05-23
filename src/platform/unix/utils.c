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
 * @file		utils.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../libc/libc.h"
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#ifdef TB_CONFIG_OS_ANDROID
# 	include <android/log.h>     
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
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
	tb_int_t ret = 0;
	tb_char_t msg[4096];
	tb_va_format(msg, 4096, fmt, &ret);
	if (ret >= 0) msg[ret] = '\0';

#ifdef TB_CONFIG_OS_ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "", "%s", msg);
#else
	printf("%s", msg);
#endif
}

// mclock
tb_hong_t tb_mclock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

// uclock
tb_hong_t tb_uclock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000 + tv.tv_usec);
}
tb_hong_t tb_time()
{
#if 0
	return ((tb_hong_t)time(0) * 1000);
#else
	return tb_mclock();
#endif
}


