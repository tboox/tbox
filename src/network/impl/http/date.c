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
 * If not, see <a href="impl://www.gnu.org/licenses/"> impl://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        date.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "http_date"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "date.h"
#include "../../../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_time_t tb_http_date_from_cstr(tb_char_t const* cstr, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(cstr && size, 0);

    // done
    tb_tm_t             tm = {0};
    tb_time_t           date = 0;
    tb_char_t const*    p = cstr;
    tb_char_t const*    e = cstr + size;
    do
    {
        // skip space
        while (p < e && tb_isspace(*p)) p++;

        // ignore
#if 0
        // parse week
        if ((p + 6 < e && !tb_strnicmp(p, "Monday", 6)) || (p + 3 < e && !tb_strnicmp(p, "Mon", 3)))
            tm.week = 1;
        else if ((p + 7 < e && !tb_strnicmp(p, "Tuesday", 7)) || (p + 3 < e && !tb_strnicmp(p, "Tue", 3)))
            tm.week = 2;
        else if ((p + 9 < e && !tb_strnicmp(p, "Wednesday", 9)) || (p + 3 < e && !tb_strnicmp(p, "Wed", 3)))
            tm.week = 3;    
        else if ((p + 8 < e && !tb_strnicmp(p, "Thursday", 8)) || (p + 3 < e && !tb_strnicmp(p, "Thu", 3)))
            tm.week = 4;
        else if ((p + 6 < e && !tb_strnicmp(p, "Friday", 6)) || (p + 3 < e && !tb_strnicmp(p, "Fri", 3)))
            tm.week = 5;
        else if ((p + 8 < e && !tb_strnicmp(p, "Saturday", 8)) || (p + 3 < e && !tb_strnicmp(p, "Sat", 3)))
            tm.week = 6;
        else if ((p + 6 < e && !tb_strnicmp(p, "Sunday", 6)) || (p + 3 < e && !tb_strnicmp(p, "Sun", 3)))
            tm.week = 7;
#endif

        // skip week
        while (p < e && *p != ',' && !tb_isspace(*p)) p++; if (p < e && (*p == ',' || tb_isspace(*p))) p++;

        // skip space
        while (p < e && tb_isspace(*p)) p++;

        // is day?
        tb_bool_t year_suffix = tb_true;
        if (p < e && tb_isdigit(*p))
        {
            /* prefix year
             * 
             * .e.g 
             * year_suffix == false: Sun, 06-Nov-1994 08:49:37
             * year_suffix == true: Sun Nov 6 08:49:37 1994
             */
            year_suffix = tb_false;

            // parse day
            tm.mday = tb_s10tou32(p);

            // skip day
            while (p < e && *p != '-' && !tb_isspace(*p)) p++; if (p < e && (*p == '-' || tb_isspace(*p))) p++;
        }

        // parse month
        if (p + 3 < e && !tb_strnicmp(p, "Jan", 3))
            tm.month = 1;
        else if (p + 3 < e && !tb_strnicmp(p, "Feb", 3))
            tm.month = 2;
        else if (p + 3 < e && !tb_strnicmp(p, "Mar", 3))
            tm.month = 3;
        else if (p + 3 < e && !tb_strnicmp(p, "Apr", 3))
            tm.month = 4;
        else if (p + 3 < e && !tb_strnicmp(p, "May", 3))
            tm.month = 5;
        else if (p + 3 < e && !tb_strnicmp(p, "Jun", 3))
            tm.month = 6;
        else if (p + 3 < e && !tb_strnicmp(p, "Jul", 3))
            tm.month = 7;
        else if (p + 3 < e && !tb_strnicmp(p, "Aug", 3))
            tm.month = 8;
        else if (p + 3 < e && !tb_strnicmp(p, "Sep", 3))
            tm.month = 9;
        else if (p + 3 < e && !tb_strnicmp(p, "Oct", 3))
            tm.month = 10;
        else if (p + 3 < e && !tb_strnicmp(p, "Nov", 3))
            tm.month = 11;
        else if (p + 3 < e && !tb_strnicmp(p, "Dec", 3))
            tm.month = 12;

        // skip month
        while (p < e && *p != '-' && !tb_isspace(*p)) p++; if (p < e && (*p == '-' || tb_isspace(*p))) p++;

        // year suffix?
        if (year_suffix)
        {   
            // parse day
            tm.mday = tb_s10tou32(p);
        }
        else
        {
            // parse year
            tm.year = tb_s10tou32(p);
            if (tm.year < 100) tm.year += 2000;
        }

        // skip year or day
        while (p < e && !tb_isspace(*p)) p++; 
        while (p < e && tb_isspace(*p)) p++; 

        // parse hour
        tm.hour = tb_s10tou32(p);

        // skip hour
        while (p < e && *p != ':') p++; if (p < e && *p == ':') p++;

        // parse minute
        tm.minute = tb_s10tou32(p);

        // skip minute
        while (p < e && *p != ':') p++; if (p < e && *p == ':') p++;

        // parse second
        tm.second = tb_s10tou32(p);

        // year suffix?
        if (year_suffix)
        {
            // skip time
            while (p < e && !tb_isspace(*p)) p++; 
            while (p < e && tb_isspace(*p)) p++; 

            // parse year
            tm.year = tb_s10tou32(p);
            if (tm.year < 100) tm.year += 1900;
        }

        // make date
        date = tb_gmmktime(&tm);

    } while (0);

    // ok?
    return date;
}
