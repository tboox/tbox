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
#include "time.h"
#include "../math/math.h"
#include <time.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

// time => local
tb_bool_t tb_time_to_local(tb_hong_t time, tb_time_t* local)
{
#if 0
	time_t t = (time_t)(time / 1000); 
	struct tm* date = localtime(&t);
	if (date)
	{
		local->year = date->tm_year + 1900;
		local->month = date->tm_mon + 1;
		local->day = date->tm_mday;
		local->hours = date->tm_hour;
		local->minutes = date->tm_min;
		local->seconds = date->tm_sec;
		local->wday = date->tm_wday;
		local->yday = date->tm_yday;
		local->isdst = date->tm_isdst;
		local->milliseconds = 0;

		// check
		if (local->year < 2000 || local->year > 3000) return TB_FALSE;
		if (!local->month || local->month > 12) return TB_FALSE;
		if (!local->day || local->day > 31) return TB_FALSE;
		if (local->hours > 23) return TB_FALSE;
		if (local->minutes > 59) return TB_FALSE;
		if (local->seconds > 59) return TB_FALSE;
		if (local->milliseconds > 999) return TB_FALSE;
		if (local->wday > 6) return TB_FALSE;
		if (local->yday > 365) return TB_FALSE;

		return TB_TRUE;
	}
	return TB_FALSE;
#else
	// for beigin: gmt+8
	time += 8 * 3600 * 1000;
	return tb_time_to_utc(time, local);
#endif
}

// time => utc
tb_bool_t tb_time_to_utc(tb_hong_t time, tb_time_t* utc)
{
	time_t t = (time_t)(time / 1000); 
	struct tm* date = gmtime(&t);
	if (date)
	{
		utc->year = date->tm_year + 1900;
		utc->month = date->tm_mon + 1;
		utc->day = date->tm_mday;
		utc->hours = date->tm_hour;
		utc->minutes = date->tm_min;
		utc->seconds = date->tm_sec;
		utc->wday = date->tm_wday;
		utc->yday = date->tm_yday;
		utc->isdst = date->tm_isdst;
		utc->milliseconds = 0;

		// check
		if (utc->year < 2000 || utc->year > 3000) return TB_FALSE;
		if (!utc->month || utc->month > 12) return TB_FALSE;
		if (!utc->day || utc->day > 31) return TB_FALSE;
		if (utc->hours > 23) return TB_FALSE;
		if (utc->minutes > 59) return TB_FALSE;
		if (utc->seconds > 59) return TB_FALSE;
		if (utc->milliseconds > 999) return TB_FALSE;
		if (utc->wday > 6) return TB_FALSE;
		if (utc->yday > 365) return TB_FALSE;

		return TB_TRUE;
	}
	return TB_FALSE;
}

// local => time
tb_bool_t tb_time_from_local(tb_hong_t* time, tb_time_t const* local)
{
	if (time && tb_time_from_utc(time, local))
	{
		// for beigin: gmt+8
		*time += 8 * 3600 * 1000;
		return TB_TRUE;
	}
	return TB_FALSE;
}

// utc => time
tb_bool_t tb_time_from_utc(tb_hong_t* time, tb_time_t const* utc)
{
	// check
	if (utc->year < 2000 || utc->year > 3000) return TB_FALSE;
	if (!utc->month || utc->month > 12) return TB_FALSE;
	if (!utc->day || utc->day > 31) return TB_FALSE;
	if (utc->hours > 23) return TB_FALSE;
	if (utc->minutes > 59) return TB_FALSE;
	if (utc->seconds > 59) return TB_FALSE;
	if (utc->milliseconds > 999) return TB_FALSE;
	if (utc->wday > 6) return TB_FALSE;
	if (utc->yday > 365) return TB_FALSE;

	if (time) 
	{
		tb_int_t y = utc->year;
		tb_int_t m = utc->month;
		tb_int_t d = utc->day;

		if (m < 3) 
		{
			m += 12;
			y--;
		}

		tb_hong_t t = 86400 * (d + (153 * m - 457) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 719469);
		t += 3600 * utc->hours;
		t += 60 * utc->minutes;
		t += utc->seconds;
		t *= 1000;
		t += utc->milliseconds;
		*time = t;

		return TB_TRUE;
	}

	return TB_FALSE;
}

tb_char_t const* tb_time_week(tb_time_t const* time)
{
	static tb_char_t const* weeks[] = 
	{
		"sun"
	, 	"mon"
	, 	"tue"
	, 	"wed"
	, 	"thu"
	, 	"fri"
	, 	"sat"
	};

	return ((time && time->wday >= 0 && time->wday < 7)? weeks[time->wday] : "unk");
}

