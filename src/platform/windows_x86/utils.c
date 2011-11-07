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
#include "../../libc/libc.h"
#include "../../math/math.h"
#include <windows.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

// usleep
tb_void_t tb_usleep(tb_size_t us)
{
	tb_trace_noimpl();
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
	TB_VA_FMT(msg, 4096, fmt, &ret);
	if (ret >= 0) msg[ret] = '\0';

	printf("%s", msg);
}

// mclock
tb_int64_t tb_mclock()
{
	DWORD ms = GetTickCount();
	return tb_int32_to_int64(ms);
}

// uclock
tb_int64_t tb_uclock()
{
	tb_trace_noimpl();
	return TB_INT64_ZERO;
}
tb_int64_t tb_time()
{
	tb_trace_noimpl();
	return TB_INT64_ZERO;
}


