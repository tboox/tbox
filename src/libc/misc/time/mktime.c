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
 * @file		mktime.c
 * @ingroup 	libc
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "time.h"
#ifdef TB_CONFIG_LIBC_HAVE_GMTIME
# 	include <time.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces 
 */

tb_time_t tb_mktime(tb_tm_t const* tm)
{
	// check
	tb_assert_and_check_return_val(tm, -1);

#ifdef TB_CONFIG_LIBC_HAVE_GMTIME
	// init
	struct tm t = {0};
	t.tm_sec = tm->second;
	t.tm_min = tm->minute;
	t.tm_hour = tm->hour;
	t.tm_mday = tm->mday;
	t.tm_mon = tm->month - 1;
	t.tm_year = tm->year > 1900? tm->year - 1900 : tm->year;
	t.tm_wday = tm->week;
	t.tm_yday = tm->yday;
	t.tm_isdst = tm->isdst;
	
	// mktime
	return (tb_time_t)mktime(&t);
#else
	// GMT+8 for beijing.china.
	tb_time_t time = tb_gmmktime(tm);
	return time >= 8 * 3600? time - 8 * 3600: -1;
#endif
}

