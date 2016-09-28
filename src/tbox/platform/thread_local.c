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
 * trace
 */
#define TB_TRACE_MODULE_NAME            "thread_local"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "thread_local.h"
#include "spinlock.h"
#include "impl/thread_local.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the thread local list
static tb_single_list_entry_head_t  g_thread_local_list;

// the thread local list lock
static tb_spinlock_t                g_thread_local_lock = TB_SPINLOCK_INIT;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
tb_bool_t tb_thread_local_init_env()
{
    // init lock
    if (!tb_spinlock_init(&g_thread_local_lock)) return tb_false;

    // init the thread local list
    tb_single_list_entry_init(&g_thread_local_list, tb_thread_local_t, entry, tb_null);

    // ok
    return tb_true;
}
tb_void_t tb_thread_local_exit_env()
{
    // enter lock
    tb_spinlock_enter(&g_thread_local_lock);

    // exit all thread locals
    tb_for_all_if (tb_thread_local_ref_t, local, tb_single_list_entry_itor(&g_thread_local_list), local)
    {
        // exit it
        tb_thread_local_exit(local);
    }

    // exit the thread local list
    tb_single_list_entry_exit(&g_thread_local_list);

    // leave lock
    tb_spinlock_leave(&g_thread_local_lock);

    // exit lock
    tb_spinlock_exit(&g_thread_local_lock);
}
tb_void_t tb_thread_local_walk(tb_walk_func_t func, tb_cpointer_t priv)
{
    // enter lock
    tb_spinlock_enter(&g_thread_local_lock);
    
    // walk all thread locals
    tb_walk_all(tb_single_list_entry_itor(&g_thread_local_list), func, priv);

    // leave lock
    tb_spinlock_leave(&g_thread_local_lock);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_POSIX_HAVE_PTHREAD_SETSPECIFIC) && \
    defined(TB_CONFIG_POSIX_HAVE_PTHREAD_GETSPECIFIC) && \
    defined(TB_CONFIG_POSIX_HAVE_PTHREAD_KEY_CREATE) && \
    defined(TB_CONFIG_POSIX_HAVE_PTHREAD_KEY_DELETE)
#   include "posix/thread_local.c"
#elif defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/thread_local.c"
#else
tb_bool_t tb_thread_local_init(tb_thread_local_ref_t local, tb_thread_local_free_t func)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_void_t tb_thread_local_exit(tb_thread_local_ref_t local)
{
    tb_trace_noimpl();
}
tb_bool_t tb_thread_local_has(tb_thread_local_ref_t local)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_pointer_t tb_thread_local_get(tb_thread_local_ref_t local)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_bool_t tb_thread_local_set(tb_thread_local_ref_t local, tb_cpointer_t priv)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif

