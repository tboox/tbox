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
 * \file		utils.h
 *
 */
#ifndef TPLAT_UTILS_H
#define TPLAT_UTILS_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// sleep
void 			tplat_msleep(tplat_uint32_t ms);

// printf
void 			tplat_printf(tplat_char_t const* fmt, ...);

// clock, ms
tplat_int64_t 	tplat_clock();

// clock, us
tplat_int64_t 	tplat_uclock();

// the milliseconds from 1970-01-01 00:00:00:000
tplat_int64_t 	tplat_time();

	
// c plus plus
#ifdef __cplusplus
}
#endif

#endif
