/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
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

