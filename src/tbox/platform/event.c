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
 * @file        event.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "event.h"
#include "time.h"
#include "cache_time.h"
#include "atomic.h"
#include "semaphore.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/event.c"
#else 
tb_event_ref_t tb_event_init()
{
    return (tb_event_ref_t)tb_semaphore_init(0);
}
tb_void_t tb_event_exit(tb_event_ref_t event)
{
    if (event) tb_semaphore_exit((tb_semaphore_ref_t)event);
}
tb_bool_t tb_event_post(tb_event_ref_t event)
{
    // check
    tb_assert_and_check_return_val(event, tb_false);

    // post
    return tb_semaphore_post((tb_semaphore_ref_t)event, 1);
}
tb_long_t tb_event_wait(tb_event_ref_t event, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(event, -1);

    // wait
    return tb_semaphore_wait((tb_semaphore_ref_t)event, timeout);
}

#endif

