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
 * @file		gmmktime.c
 * @ingroup 	libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "time.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */

tb_time_t tb_gmmktime(tb_tm_t const* tm)
{
	// check
	tb_assert_and_check_return_val(tm, -1);

	tb_long_t y = tm->year;
	tb_long_t m = tm->month;
	tb_long_t d = tm->mday;

	if (m < 3) 
	{
		m += 12;
		y--;
	}

	tb_time_t time = 86400 * (d + (153 * m - 457) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 719469);
	time += 3600 * tm->hour;
	time += 60 * tm->minute;
	time += tm->second;

	// time
	return time;
}

