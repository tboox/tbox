/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
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
#include "../impl/mutex.h"
#include "../../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
tb_mutex_ref_t tb_mutex_init_impl(tb_mutex_t* mutex)
{
    // init mutex, @note we cannot use asset/trace because them will use mutex
    tb_check_return_val(mutex, tb_null);
    tb_assert_static(sizeof(HANDLE) == sizeof(tb_mutex_t));

    *mutex = CreateMutex(tb_null, FALSE, tb_null);
    return ((*mutex != INVALID_HANDLE_VALUE)? (tb_mutex_ref_t)*mutex : tb_null);
}
tb_void_t tb_mutex_exit_impl(tb_mutex_t* mutex)
{
    // exit it
    if (mutex && *mutex) CloseHandle(*mutex);
}
tb_bool_t tb_mutex_enter_without_profiler(tb_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    tb_check_return_val(mutex, tb_false);

    // enter
    return mutex && WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)mutex, INFINITE);
}
tb_bool_t tb_mutex_entry_try_without_profiler(tb_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    tb_check_return_val(mutex, tb_false);

    // try to enter
    return mutex && WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)mutex, 0);
}

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
    if (mutex) CloseHandle((HANDLE)mutex);
}
tb_bool_t tb_mutex_enter(tb_mutex_ref_t mutex)
{
    // try to enter for profiler
#ifdef TB_LOCK_PROFILER_ENABLE
    if (tb_mutex_enter_try(mutex)) return tb_true;
#endif
    return tb_mutex_enter_without_profiler(mutex);
}
tb_bool_t tb_mutex_enter_try(tb_mutex_ref_t mutex)
{
    // try to enter
    if (!tb_mutex_entry_try_without_profiler(mutex))
    {
        // occupied
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_occupied(tb_lock_profiler(), (tb_handle_t)mutex);
#endif
        return tb_false;
    }
    return tb_true;
}
tb_bool_t tb_mutex_leave(tb_mutex_ref_t mutex)
{
    if (mutex) return ReleaseMutex((HANDLE)mutex)? tb_true : tb_false;
    return tb_false;
}
