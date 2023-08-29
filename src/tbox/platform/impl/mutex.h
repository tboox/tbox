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
 * @file        mutex.h
 *
 */
#ifndef TB_PLATFORM_IMPL_MUTEX_H
#define TB_PLATFORM_IMPL_MUTEX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#ifdef TB_CONFIG_OS_WINDOWS
#   include "../windows/prefix.h"
#elif defined(TB_CONFIG_POSIX_HAVE_PTHREAD_MUTEX_INIT)
#   include <pthread.h>
#else
#   include "../spinlock.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
#if defined(TB_CONFIG_OS_WINDOWS)
typedef HANDLE              tb_mutex_t;
#elif defined(TB_CONFIG_POSIX_HAVE_PTHREAD_MUTEX_INIT)
typedef pthread_mutex_t     tb_mutex_t;
#else
typedef tb_spinlock_t       tb_mutex_t;
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
/* init mutex at the given mutex data
 *
 * @param       the mutex data
 *
 * @return      the mutex reference
 */
tb_mutex_ref_t  tb_mutex_init_impl(tb_mutex_t* mutex);

/* exit mutex
 *
 * @param       the mutex data
 */
tb_void_t       tb_mutex_exit_impl(tb_mutex_t* mutex);

/* enter mutex without profiler
 *
 * @param mutex the mutex
 *
 * @return      tb_true or tb_false
 */
tb_bool_t       tb_mutex_enter_without_profiler(tb_mutex_ref_t mutex);

/* try to enter mutex without profiler
 *
 * @param mutex the mutex
 *
 * @return      tb_true or tb_false
 */
tb_bool_t       tb_mutex_entry_try_without_profiler(tb_mutex_ref_t mutex);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
