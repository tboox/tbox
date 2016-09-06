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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        mktime.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "time.h"
#ifdef TB_CONFIG_LIBC_HAVE_MKTIME
#   include <time.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */

tb_time_t tb_mktime(tb_tm_t const* tm)
{
    // check
    tb_assert_and_check_return_val(tm, -1);

#ifdef TB_CONFIG_LIBC_HAVE_MKTIME
    // init
    struct tm t = {0};
    t.tm_sec    = (tb_int_t)tm->second;
    t.tm_min    = (tb_int_t)tm->minute;
    t.tm_hour   = (tb_int_t)tm->hour;
    t.tm_mday   = (tb_int_t)tm->mday;
    t.tm_mon    = (tb_int_t)tm->month - 1;
    t.tm_year   = (tb_int_t)(tm->year > 1900? tm->year - 1900 : tm->year);
    t.tm_wday   = (tb_int_t)tm->week;
    t.tm_yday   = (tb_int_t)tm->yday;
    t.tm_isdst  = (tb_int_t)tm->isdst;
    
    // mktime
    return (tb_time_t)mktime(&t);
#else
    // GMT+8 for beijing.china.
    tb_time_t time = tb_gmmktime(tm);
    return time >= 8 * 3600? time - 8 * 3600: -1;
#endif
}

