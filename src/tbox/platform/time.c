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
 * @author      ruki
 * @file        time.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "time.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/time.c"
#elif defined(TB_CONFIG_API_HAVE_POSIX)
#   include "posix/time.c"
#else
tb_void_t tb_usleep(tb_size_t us)
{
    tb_trace_noimpl();
}
tb_void_t tb_msleep(tb_size_t ms)
{
    tb_trace_noimpl();
}
tb_void_t tb_sleep(tb_size_t s)
{
    tb_trace_noimpl();
}
tb_hong_t tb_mclock()
{
    tb_trace_noimpl();
    return 0;
}
tb_hong_t tb_uclock()
{
    tb_trace_noimpl();
    return 0;
}
tb_bool_t tb_gettimeofday(tb_timeval_t* tv, tb_timezone_t* tz)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif
