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
 * @tlocal      thread_local.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include <pthread.h>
#include "../thread.h"
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

// the once function
static tb_bool_t tb_thread_local_once(tb_cpointer_t priv)
{
    // check
    tb_value_ref_t tuple = (tb_value_ref_t)priv;
    tb_check_return_val(tuple, tb_false);

    // the thread local
    tb_thread_local_ref_t local = (tb_thread_local_ref_t)tuple[0].ptr;
    tb_check_return_val(local, tb_false);

    // save the free function
    local->free = (tb_thread_local_free_t)tuple[1].ptr;

    // check the pthread key space size
    tb_assert_static(sizeof(pthread_key_t) * 2 <= sizeof(local->priv));

    // create the pthread key for data
    tb_bool_t ok = pthread_key_create(&((pthread_key_t*)local->priv)[0], tb_null) == 0;
    if (ok)
    {
        // create the pthread key for mark
        ok = pthread_key_create(&((pthread_key_t*)local->priv)[1], tb_null) == 0;

        // failed? remove the data key
        if (!ok) pthread_key_delete(((pthread_key_t*)local->priv)[0]);
    }

    // save this thread local to list if ok
    if (ok)
    {
        tb_spinlock_enter(&g_thread_local_lock);
        tb_single_list_entry_insert_tail(&g_thread_local_list, &local->entry);
        tb_spinlock_leave(&g_thread_local_lock);
    }

    // ok?
    return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_thread_local_init(tb_thread_local_ref_t local, tb_thread_local_free_t func)
{
    // check
    tb_assert_and_check_return_val(local, tb_false);

    // run the once function
    tb_value_t tuple[2];
    tuple[0].ptr = (tb_pointer_t)local;
    tuple[1].ptr = (tb_pointer_t)func;
    return tb_thread_once(&local->once, tb_thread_local_once, tuple);
}
tb_void_t tb_thread_local_exit(tb_thread_local_ref_t local)
{
    // check
    tb_assert(local);

    // exit it
    pthread_key_delete(((pthread_key_t*)local->priv)[0]);
    pthread_key_delete(((pthread_key_t*)local->priv)[1]);
}
tb_bool_t tb_thread_local_has(tb_thread_local_ref_t local)
{
    // check
    tb_assert(local);

    // get it
    return pthread_getspecific(((pthread_key_t*)local->priv)[1]) != tb_null;
}
tb_pointer_t tb_thread_local_get(tb_thread_local_ref_t local)
{
    // check
    tb_assert(local);

    // get it
    return pthread_getspecific(((pthread_key_t*)local->priv)[0]);
}
tb_bool_t tb_thread_local_set(tb_thread_local_ref_t local, tb_cpointer_t priv)
{
    // check
    tb_assert(local);

    // free the previous data first
    if (local->free && tb_thread_local_has(local))
        local->free(tb_thread_local_get(local));

    // set it
    tb_bool_t ok = pthread_setspecific(((pthread_key_t*)local->priv)[0], priv) == 0;
    if (ok)
    {
        // mark exists
        ok = pthread_setspecific(((pthread_key_t*)local->priv)[1], (tb_pointer_t)tb_true) == 0;
    }

    // ok?
    return ok;
}

