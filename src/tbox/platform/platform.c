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
 * @file        platform.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "platform.h"
#include "../network/network.h"
#ifdef TB_CONFIG_OS_ANDROID
#   include "linux/android/android.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c_enter__

// init socket context
tb_bool_t   tb_socket_context_init(tb_noarg_t);
// exit socket context
tb_void_t   tb_socket_context_exit(tb_noarg_t);

__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_bool_t tb_platform_init(tb_handle_t priv)
{
    // init android
#ifdef TB_CONFIG_OS_ANDROID
    if (!tb_android_init(priv)) return tb_false;
#endif

    // init thread store
    if (!tb_thread_store_init()) return tb_false;

    // init socket context
    if (!tb_socket_context_init()) return tb_false;

    // init dns
    if (!tb_dns_init()) return tb_false;

    // spak ctime
    tb_cache_time_spak();

    // ok
    return tb_true;
}
tb_void_t tb_platform_exit()
{
    // exit dns
    tb_dns_exit();

    // exit socket context
    tb_socket_context_exit();

    // exit thread store
    tb_thread_store_exit();

    // exit android
#ifdef TB_CONFIG_OS_ANDROID
    tb_android_exit();
#endif
}

