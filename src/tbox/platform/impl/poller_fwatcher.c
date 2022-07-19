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
 * @file        poller_fwatcher.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../fwatcher.h"
#include "../thread.h"
#include "../atomic.h"
#include "../semaphore.h"
#include "../spinlock.h"
#include "../../algorithm/algorithm.h"
#include "../../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the poller fwatcher type
typedef struct __tb_poller_fwatcher_t
{
    // the main poller
    tb_poller_t*            main_poller;

    // the fwatcher poller thread
    tb_thread_ref_t         thread;

}tb_poller_fwatcher_t;


/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_poller_fwatcher_kill(tb_poller_fwatcher_ref_t self)
{
}
static tb_void_t tb_poller_fwatcher_exit(tb_poller_fwatcher_ref_t self)
{
}
static tb_poller_fwatcher_ref_t tb_poller_fwatcher_init(tb_poller_t* main_poller)
{
    tb_bool_t             ok = tb_false;
    tb_poller_fwatcher_t* poller = tb_null;
    do
    {
        // @note only support one fwatcher poller instance
        static tb_size_t s_poller_fwatcher_num = 0;
        if (s_poller_fwatcher_num++)
        {
            tb_trace_e("only support one fwatcher poller!");
            break;
        }

        // make the fwatcher poller
        poller = tb_malloc0_type(tb_poller_fwatcher_t);
        tb_assert_and_check_break(poller);

        // save the main poller
        poller->main_poller = main_poller;

        // ok
        ok = tb_true;

    } while (0);

    // failed? exit the poller
    if (!ok)
    {
        if (poller) tb_poller_fwatcher_exit((tb_poller_fwatcher_ref_t)poller);
        poller = tb_null;
    }
    return (tb_poller_fwatcher_ref_t)poller;
}
static tb_void_t tb_poller_fwatcher_spak(tb_poller_fwatcher_ref_t self)
{
}
static tb_bool_t tb_poller_fwatcher_insert(tb_poller_fwatcher_ref_t self, tb_fwatcher_ref_t fwatcher, tb_cpointer_t priv)
{
    return tb_true;
}
static tb_bool_t tb_poller_fwatcher_modify(tb_poller_fwatcher_ref_t self, tb_fwatcher_ref_t fwatcher, tb_cpointer_t priv)
{
    return tb_true;
}
static tb_bool_t tb_poller_fwatcher_remove(tb_poller_fwatcher_ref_t self, tb_fwatcher_ref_t fwatcher)
{
    return tb_true;
}
static tb_bool_t tb_poller_fwatcher_wait_prepare(tb_poller_fwatcher_ref_t self)
{
    return 0;
}
static tb_long_t tb_poller_fwatcher_wait_poll(tb_poller_fwatcher_ref_t self, tb_poller_event_func_t func)
{
    return -1;
}
