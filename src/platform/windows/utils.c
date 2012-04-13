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
 * \author		ruki
 * \file		utils.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../libc/libc.h"
#include "../../math/math.h"
#include <windows.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

// usleep
tb_void_t tb_usleep(tb_size_t us)
{
	Sleep(1);
}

// msleep
tb_void_t tb_msleep(tb_size_t ms)
{
	Sleep(ms);
}

// sleep
tb_void_t tb_sleep(tb_size_t s)
{
	Sleep(s * 1000);
}

// printf
tb_void_t tb_printf(tb_char_t const* fmt, ...)
{
	tb_int_t ret = 0;
	tb_char_t msg[4096];
	tb_va_format(msg, 4096, fmt, &ret);
	if (ret >= 0) msg[ret] = '\0';

	printf("%s", msg);
}

// mclock
tb_hong_t tb_mclock()
{
	return (tb_hong_t)GetTickCount();
}

// uclock
tb_hong_t tb_uclock()
{
	LARGE_INTEGER f = {0};
    if (!QueryPerformanceFrequency(&f)) return 0;
	tb_assert_and_check_return_val(f.QuadPart, 0);

	LARGE_INTEGER t = {0};
	if (!QueryPerformanceCounter(&t)) return 0;
	tb_assert_and_check_return_val(t.QuadPart, 0);
	
	return (t.QuadPart * 1000000) / f.QuadPart;
}
tb_hong_t tb_time()
{
	tb_trace_noimpl();
	return 0;
}


