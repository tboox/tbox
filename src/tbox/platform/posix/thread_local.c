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
#include "../../libc/libc.h"
 
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
    tb_assert_static(sizeof(pthread_key_t) <= sizeof(local->priv));

    // create the pthread key
    return pthread_key_create((pthread_key_t*)local->priv, (tb_void_t(*)(tb_pointer_t))local->free) == 0;
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
    pthread_key_delete(*((pthread_key_t*)local->priv));

    // reset it
    tb_memset(local, 0, sizeof(tb_thread_local_t));
}
tb_pointer_t tb_thread_local_get(tb_thread_local_ref_t local)
{
    // check
    tb_assert(local);

    // get it
    return pthread_getspecific(*((pthread_key_t*)local->priv));
}
tb_bool_t tb_thread_local_set(tb_thread_local_ref_t local, tb_cpointer_t priv)
{
    // check
    tb_assert(local);

    // set it
    return pthread_setspecific(*((pthread_key_t*)local->priv), priv) == 0;
}

