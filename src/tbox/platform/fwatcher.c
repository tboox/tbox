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
 * @file        fwatcher.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "fwatcher"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fwatcher.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/fwatcher_iocp.c"
#elif defined(TB_CONFIG_LINUX_HAVE_INOTIFY_INIT)
#   include "linux/fwatcher_inotify.c"
#elif defined(TB_CONFIG_OS_MACOSX)
#   include "mach/fwatcher_fsevent.c"
#elif defined(TB_CONFIG_OS_BSD)
#   include "bsd/fwatcher_kqueue.c"
#else
tb_fwatcher_ref_t tb_fwatcher_init()
{
    tb_trace_noimpl();
    return tb_null;
}

tb_void_t tb_fwatcher_exit(tb_fwatcher_ref_t self)
{
    tb_trace_noimpl();
}

tb_bool_t tb_fwatcher_add(tb_fwatcher_ref_t self, tb_char_t const* watchdir, tb_bool_t recursion)
{
    tb_trace_noimpl();
    return tb_false;
}

tb_bool_t tb_fwatcher_remove(tb_fwatcher_ref_t self, tb_char_t const* watchdir)
{
    tb_trace_noimpl();
    return tb_false;
}

tb_void_t tb_fwatcher_spak(tb_fwatcher_ref_t self)
{
    tb_trace_noimpl();
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* event, tb_long_t timeout)
{
    tb_trace_noimpl();
    return -1;
}

#endif
