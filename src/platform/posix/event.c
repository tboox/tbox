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
 * @file        event.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_event_init()
{
    return tb_semaphore_init(0);
}
tb_void_t tb_event_exit(tb_handle_t handle)
{
    if (handle) tb_semaphore_exit(handle);
}
tb_bool_t tb_event_post(tb_handle_t handle)
{
    // check
    tb_assert_and_check_return_val(handle, tb_false);

    // post
    return tb_semaphore_post(handle, 1);
}
tb_long_t tb_event_wait(tb_handle_t handle, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(handle, -1);

    // wait
    return tb_semaphore_wait(handle, timeout);
}


