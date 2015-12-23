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
#include "../event.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_event_ref_t tb_event_init()
{
    // create event
    HANDLE event = CreateEventA(tb_null, FALSE, FALSE, tb_null);

    // ok?
    return ((event != INVALID_HANDLE_VALUE)? (tb_event_ref_t)event : tb_null);
}
tb_void_t tb_event_exit(tb_event_ref_t event)
{
    if (event) CloseHandle((HANDLE)event);
}
tb_bool_t tb_event_post(tb_event_ref_t event)
{
    // check
    tb_assert_and_check_return_val(event, tb_false);
    
    // post
    return SetEvent((HANDLE)event)? tb_true : tb_false;
}
tb_long_t tb_event_wait(tb_event_ref_t event, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(event, -1);

    // wait
    tb_long_t r = WaitForSingleObject((HANDLE)event, (DWORD)(timeout >= 0? timeout : INFINITE));
    tb_assert_and_check_return_val(r != WAIT_FAILED, -1);

    // timeout?
    tb_check_return_val(r != WAIT_TIMEOUT, 0);

    // error?
    tb_check_return_val(r >= WAIT_OBJECT_0, -1);

    // ok
    return 1;
}


