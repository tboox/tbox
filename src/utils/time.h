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
 * @file		time.h
 * @ingroup 	utils
 *
 */
#ifndef TB_UTILS_TIME_H
#define TB_UTILS_TIME_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the time type
typedef struct __tb_time_t
{
	tb_int_t 	year; 			// [1900, 2299]
	tb_int_t 	month; 			// [1, 12]
	tb_int_t 	day; 			// [1, 31]
	tb_int_t 	hours; 			// [0, 23]
	tb_int_t 	minutes; 		// [0, 59]
	tb_int_t 	seconds; 		// [0, 59]
	tb_int_t 	milliseconds; 	// [0, 999]
	tb_int_t 	wday; 			// days since sunday: [0, 6]
	tb_int_t 	yday; 			// days since january 1: [0, 365]
	tb_int_t 	isdst; 			// daylight savings time flag: 

}tb_time_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// time => local
tb_bool_t 			tb_time_to_local(tb_hong_t time, tb_time_t* local);

// time => utc/gmt
tb_bool_t 			tb_time_to_utc(tb_hong_t time, tb_time_t* utc);

// local => time
tb_bool_t 			tb_time_from_local(tb_hong_t* time, tb_time_t const* local);

// utc/gmt => time
tb_bool_t 			tb_time_from_utc(tb_hong_t* time, tb_time_t const* utc);

// week
tb_char_t const* 	tb_time_week(tb_time_t const* time);

	
#endif
