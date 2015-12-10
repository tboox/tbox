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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        singleton.c
 * @ingroup     utils
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "singleton"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "singleton.h"
#include "../libc/libc.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the singleton type
typedef struct __tb_singleton_t
{
    // the exit func
    tb_singleton_exit_func_t        exit;

    // the kill func
    tb_singleton_kill_func_t        kill;

    // the priv data
    tb_cpointer_t                   priv;

    // the instance
    tb_atomic_t                     instance;

}tb_singleton_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the singletons
static tb_singleton_t g_singletons[TB_SINGLETON_TYPE_MAXN] = {{0}};

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_singleton_init()
{
    /* init it
     *
     * @note
     * this is thread safe, because tb_singleton_init() only will be called in/before the tb_init()
     */
    static tb_bool_t binited = tb_false;
    if (!binited)
    {
        // init it
        tb_memset(&g_singletons, 0, sizeof(g_singletons));

        // ok
        binited = tb_true;
    }

    // ok
    return tb_true;
}
tb_void_t tb_singleton_kill()
{
    tb_size_t i = TB_SINGLETON_TYPE_MAXN;
    while (i--)
    {
        if (g_singletons[i].kill) 
        {
            // the instance
            tb_handle_t instance = (tb_handle_t)tb_atomic_get(&g_singletons[i].instance);
            if (instance && instance != (tb_handle_t)1) 
            {   
                // trace
                tb_trace_d("instance: kill: %lu: ..", i);

                // kill it
                g_singletons[i].kill(instance, g_singletons[i].priv);
            }
        }
    }
}
tb_void_t tb_singleton_exit()
{
    // done
    tb_size_t i = TB_SINGLETON_TYPE_MAXN;
    while (i--)
    {
        if (g_singletons[i].exit) 
        {
            // the instance
            tb_handle_t instance = (tb_handle_t)tb_atomic_fetch_and_set0(&g_singletons[i].instance);
            if (instance && instance != (tb_handle_t)1) 
            {
                // trace
                tb_trace_d("instance: exit: %lu: ..", i);

                // exit it
                g_singletons[i].exit(instance, g_singletons[i].priv);
            }
        }
    }
 
    // clear it
    tb_memset(&g_singletons, 0, sizeof(g_singletons));
}
tb_handle_t tb_singleton_instance(tb_size_t type, tb_singleton_init_func_t init, tb_singleton_exit_func_t exit, tb_singleton_kill_func_t kill, tb_cpointer_t priv)
{
    // check, @note cannot use trace, assert and memory
    tb_check_return_val(type < TB_SINGLETON_TYPE_MAXN, tb_null);
    
    // the instance
    tb_handle_t instance = (tb_handle_t)tb_atomic_fetch_and_pset(&g_singletons[type].instance, 0, 1);

    // ok?
    if (instance && instance != (tb_handle_t)1) return instance;
    // null? init it
    else if (!instance)
    {
        // check
        tb_check_return_val(init && exit, tb_null);

        // init priv
        g_singletons[type].priv = priv;

        // init it
        instance = init(&g_singletons[type].priv);
        tb_check_return_val(instance, tb_null);

        // init func
        g_singletons[type].exit = exit;
        g_singletons[type].kill = kill;

        // register instance 
        tb_atomic_set(&g_singletons[type].instance, (tb_long_t)instance);
    }
    // initing? wait it
    else
    {
        // try getting it
        tb_size_t tryn = 50;
        while ((instance = (tb_handle_t)tb_atomic_get(&g_singletons[type].instance)) && (instance != (tb_handle_t)1) && tryn--)
        {
            // wait some time
            tb_msleep(100);
        }

        // failed?
        if (instance == (tb_handle_t)1 || !instance)
            return tb_null;
    }

    // ok?
    return instance;
}
tb_bool_t tb_singleton_static_init(tb_atomic_t* binited, tb_handle_t instance, tb_singleton_static_init_func_t init, tb_cpointer_t priv)
{
    // check
    tb_check_return_val(binited && instance, tb_false);

    // inited?
    tb_atomic_t inited = tb_atomic_fetch_and_pset(binited, 0, 1);

    // ok?
    if (inited && inited != 1) return tb_true;
    // null? init it
    else if (!inited)
    {
        // check
        tb_check_return_val(init, tb_false);

        // init it
        if (!init(instance, priv)) return tb_false;

        // init ok
        tb_atomic_set(binited, 2);
    }
    // initing? wait it
    else
    {
        // try getting it
        tb_size_t tryn = 50;
        while ((1 != tb_atomic_get(binited)) && tryn--)
        {
            // wait some time
            tb_msleep(100);
        }

        // failed?
        if (tb_atomic_get(binited) == 1 || !instance)
            return tb_false;
    }

    // ok?
    return tb_true;
}

