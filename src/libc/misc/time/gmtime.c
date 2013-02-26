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
 * @file		gmtime.c
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

tb_bool_t tb_gmtime(tb_time_t time, tb_tm_t* tm)
{
#ifdef TB_CONFIG_LIBC_HAVE_GMTIME
	// gmtime
	time_t t = (time_t)time;
	struct tm* ptm = gmtime(&t);
	if (ptm && tm)
	{
		tm->second = ptm->tm_sec;
		tm->minute = ptm->tm_min;
		tm->hour = ptm->tm_hour;
		tm->mday = ptm->tm_mday;
		tm->month = ptm->tm_mon + 1;
		tm->year = ptm->tm_year + 1900;
		tm->week = ptm->tm_wday;
		tm->yday = ptm->tm_yday;
		tm->isdst = ptm->tm_isdst;
	}

	// ok?
	return ptm? tb_true : tb_false;
#else
	tb_trace_noimpl();
	return tb_false;
#endif
}

