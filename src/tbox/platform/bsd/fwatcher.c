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
 * includes
 */
#include "../fwatcher.h"
#include "../socket.h"
#include <stdlib.h>
#include <errno.h>
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
    && !defined(TB_CONFIG_MICRO_ENABLE)
#   include "../../coroutine/coroutine.h"
#   include "../../coroutine/impl/impl.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// fd to fwatcher entry
#define tb_fd2entry(fd)              ((fd) >= 0? (tb_fwatcher_entry_ref_t)((tb_long_t)(fd) + 1) : tb_null)

// fwatcher entry to fd
#define tb_entry2fd(file)            (tb_int_t)((file)? (((tb_long_t)(file)) - 1) : -1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the fwatcher type
typedef struct __tb_fwatcher_t
{
    tb_int_t    fd;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c_enter__
tb_long_t tb_socket_wait_impl(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout);
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_fwatcher_ref_t tb_fwatcher_init()
{
    tb_bool_t ok = tb_false;
    tb_fwatcher_t* fwatcher = tb_null;
    do
    {
        fwatcher = tb_malloc0_type(tb_fwatcher_t);
        tb_assert_and_check_break(fwatcher);

        ok = tb_true;
    } while (0);

    if (!ok && fwatcher)
    {
        tb_fwatcher_exit((tb_fwatcher_ref_t)fwatcher);
        fwatcher = tb_null;
    }

    return (tb_fwatcher_ref_t)fwatcher;
}

tb_bool_t tb_fwatcher_exit(tb_fwatcher_ref_t self)
{
    tb_bool_t ok = tb_false;
    tb_fwatcher_t* fwatcher = (tb_fwatcher_t*)self;
    if (fwatcher)
    {
        tb_free(fwatcher);
        ok = tb_true;
    }
    return ok;
}

tb_bool_t tb_fwatcher_register(tb_fwatcher_ref_t self, tb_char_t const* dir, tb_size_t events)
{
    tb_trace_noimpl();
    return tb_false;
}

tb_void_t tb_fwatcher_spak(tb_fwatcher_ref_t self)
{
    tb_trace_noimpl();
}

tb_long_t tb_fwatcher_wait(tb_fwatcher_ref_t self, tb_fwatcher_event_t* events, tb_size_t events_maxn, tb_long_t timeout)
{
    tb_trace_noimpl();
    return -1;
}
