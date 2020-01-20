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
 * Copyright (C) 2009-2020, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        poller_process.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../process.h"
#include "../thread.h"
#include "../../container/hash_map.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#ifdef TB_CONFIG_LIBC_HAVE_KILL
#   include <signal.h>
#   include <sys/types.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the poller process type
typedef struct __tb_poller_process_t
{
    // the process poller thread
    tb_thread_ref_t         thread;

    // the waited processes data, process => user private data
    tb_hash_map_ref_t       process_data;

}tb_poller_process_t; 

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_int_t tb_poller_process_thread(tb_cpointer_t priv)
{
    // check
    tb_poller_t* poller = (tb_poller_t*)priv;
    tb_assert_and_check_return_val(poller, -1);

    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_poller_process_exit(tb_poller_process_ref_t self)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return(poller);

    // exit the process poller thread
    if (poller->thread)
    {
        // wait it
        tb_long_t wait = 0;
        if ((wait = tb_thread_wait(poller->thread, 5000, tb_null)) <= 0)
            tb_trace_e("wait process poller thread failed: %ld!", wait);

        // exit it
        tb_thread_exit(poller->thread);
        poller->thread = tb_null;
    }

    // exit the process data
    if (!poller->process_data) tb_hash_map_exit(poller->process_data);
    poller->process_data = tb_null;

    // exit poller
    tb_free(poller);
}
static tb_poller_process_ref_t tb_poller_process_init()
{
    tb_bool_t            ok = tb_false;
    tb_poller_process_t* poller = tb_null;
    do
    {
        // make the process poller
        poller = tb_malloc0_type(tb_poller_process_t);
        tb_assert_and_check_break(poller);

        // start the poller thread for processes first
        poller->thread = tb_thread_init(tb_null, tb_poller_process_thread, poller, 0);
        tb_assert_and_check_break(poller->thread);

        // init the process data first
        poller->process_data = tb_hash_map_init(TB_HASH_MAP_BUCKET_SIZE_MICRO, tb_element_ptr(tb_null, tb_null), tb_element_ptr(tb_null, tb_null));
        tb_assert_and_check_break(poller->process_data);

        // TODO

        // ok
        ok = tb_true;

    } while (0);

    // failed? exit the poller
    if (!ok)
    {
        if (poller) tb_poller_process_exit((tb_poller_process_ref_t)poller);
        poller = tb_null;
    }
    return (tb_poller_process_ref_t)poller;
}
static tb_void_t tb_poller_process_kill(tb_poller_process_ref_t self)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return(poller);
    
    // TODO
}
static tb_void_t tb_poller_process_spak(tb_poller_process_ref_t self)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return(poller);
    
    // TODO
}
static tb_bool_t tb_poller_process_insert(tb_poller_process_ref_t self, tb_process_ref_t process, tb_cpointer_t priv)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->process_data && process, tb_false);

    // insert this process and the user private data
    tb_hash_map_insert(poller->process_data, process, priv);
    return tb_true;
}
static tb_bool_t tb_poller_process_modify(tb_poller_process_ref_t self, tb_process_ref_t process, tb_cpointer_t priv)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->process_data && process, tb_false);

    // modify the user private data of this process
    if (tb_hash_map_find(poller->process_data, process) != tb_iterator_tail(poller->process_data))
        tb_hash_map_insert(poller->process_data, process, priv);
    return tb_true;
}
static tb_bool_t tb_poller_process_remove(tb_poller_process_ref_t self, tb_process_ref_t process)
{
    // check
    tb_poller_process_t* poller = (tb_poller_process_t*)self;
    tb_assert_and_check_return_val(poller && poller->process_data && process, tb_false);

    // remove this process and the user private data
    tb_hash_map_remove(poller->process_data, process);
    return tb_true;
}
