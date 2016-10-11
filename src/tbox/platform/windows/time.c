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
 * @file        utils.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../time.h"
#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
#   include "../../coroutine/coroutine.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_usleep(tb_size_t us)
{
    Sleep(1);
}
tb_void_t tb_msleep(tb_size_t ms)
{
#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
    // attempt to sleep in coroutine
    if (tb_coroutine_self())
    {
        // sleep it
        tb_coroutine_sleep(ms);
        return ;
    }
#endif

    // sleep it
    Sleep((DWORD)ms);
}
tb_void_t tb_sleep(tb_size_t s)
{
    tb_msleep(s * 1000);
}
tb_hong_t tb_mclock()
{
    return (tb_hong_t)GetTickCount();
}
tb_hong_t tb_uclock()
{
    LARGE_INTEGER f = {{0}};
    if (!QueryPerformanceFrequency(&f)) return 0;
    tb_assert_and_check_return_val(f.QuadPart, 0);

    LARGE_INTEGER t = {{0}};
    if (!QueryPerformanceCounter(&t)) return 0;
    tb_assert_and_check_return_val(t.QuadPart, 0);
    
    return (t.QuadPart * 1000000) / f.QuadPart;
}
tb_bool_t tb_gettimeofday(tb_timeval_t* tv, tb_timezone_t* tz)
{
    union 
    {
        tb_uint64_t ns100; //< time since 1 Jan 1601 in 100ns units
        FILETIME    ft;

    }now;

    if (tv)
    {
        GetSystemTimeAsFileTime(&now.ft);
        tv->tv_sec  = (tb_time_t)((now.ns100 - 116444736000000000ULL) / 10000000ULL);
        tv->tv_usec = (tb_suseconds_t)((now.ns100 / 10ULL) % 1000000ULL);
    }

    // tz is not implementated now.
    tb_assert_and_check_return_val(!tz, tb_false);

    // ok
    return tb_true;
}
