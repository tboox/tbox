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
 * @file        mutex.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../mutex.h"
#include "../../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_mutex_ref_t tb_mutex_init()
{
    HANDLE mutex = CreateMutex(tb_null, FALSE, tb_null);
    return ((mutex != INVALID_HANDLE_VALUE)? (tb_mutex_ref_t)mutex : tb_null);
}
tb_void_t tb_mutex_exit(tb_mutex_ref_t mutex)
{
    if (mutex) CloseHandle(mutex);
}
tb_bool_t tb_mutex_enter(tb_mutex_ref_t mutex)
{
    // try to enter for profiler
#ifdef TB_LOCK_PROFILER_ENABLE
    if (tb_mutex_enter_try(mutex)) return tb_true;
#endif
    
    // enter
    if (mutex && WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)mutex, INFINITE)) return tb_true;

    // failed
    return tb_false;
}
tb_bool_t tb_mutex_enter_try(tb_mutex_ref_t mutex)
{
    // try to enter
    if (mutex && WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)mutex, 0)) return tb_true;
    
    // occupied
#ifdef TB_LOCK_PROFILER_ENABLE
    tb_lock_profiler_occupied(tb_lock_profiler(), (tb_handle_t)mutex);
#endif

    // failed
    return tb_false;
}
tb_bool_t tb_mutex_leave(tb_mutex_ref_t mutex)
{
    if (mutex) return ReleaseMutex((HANDLE)mutex)? tb_true : tb_false;
    return tb_false;
}
