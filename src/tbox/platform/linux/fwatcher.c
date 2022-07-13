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
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_FWATCHER_EVENT_SIZE      (sizeof(struct inotify_event))
#define TB_FWATCHER_BUFFER_SIZE     (1024 * (TB_FWATCHER_EVENT_SIZE + 16))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the fwatcher type
typedef struct __tb_fwatcher_t
{
    tb_int_t dummy;

}tb_fwatcher_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_fwatcher_ref_t tb_fwatcher_init()
{
    tb_trace_noimpl();
    return tb_null;
}

tb_bool_t tb_fwatcher_exit(tb_fwatcher_ref_t self)
{
    tb_trace_noimpl();
    return tb_false;
}

tb_fwatcher_entry_ref_t tb_fwatcher_entry_add(tb_fwatcher_ref_t self, tb_char_t const* dir, tb_size_t events)
{
    tb_trace_noimpl();
    return tb_null;
}

tb_bool_t tb_fwatcher_entry_remove(tb_fwatcher_ref_t self, tb_fwatcher_entry_ref_t entry)
{
    tb_trace_noimpl();
    return tb_false;
}

tb_long_t tb_fwatcher_entry_wait(tb_fwatcher_ref_t self, tb_fwatcher_entry_ref_t entry, tb_long_t timeout)
{
    tb_trace_noimpl();
    return -1;
}

tb_long_t tb_fwatcher_entry_read(tb_fwatcher_ref_t self, tb_fwatcher_entry_ref_t entry, tb_char_t const* pfile)
{
    tb_trace_noimpl();
    return -1;
}

