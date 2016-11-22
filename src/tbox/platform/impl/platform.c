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
 * @file        platform.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "platform.h"
#include "impl.h"
#include "../exception.h"
#include "../cache_time.h"
#include "../../network/network.h"
#include "../deprecated/deprecated.h"
#ifdef TB_CONFIG_OS_ANDROID
#   include "../android/android.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_bool_t tb_platform_init_env(tb_handle_t priv)
{
    // init android envirnoment
#ifdef TB_CONFIG_OS_ANDROID
    if (!tb_android_init_env(priv)) return tb_false;
#endif

    // init socket envirnoment
    if (!tb_socket_init_env()) return tb_false;

    // init dns envirnoment
#ifndef TB_CONFIG_MICRO_ENABLE
    if (!tb_dns_init_env()) return tb_false;
#endif

    // init thread local envirnoment
#ifndef TB_CONFIG_MICRO_ENABLE
    if (!tb_thread_local_init_env()) return tb_false;
#endif

    // init exception envirnoment
#ifdef TB_CONFIG_EXCEPTION_ENABLE
    if (!tb_exception_init_env()) return tb_false;
#endif

#ifdef TB_CONFIG_API_HAVE_DEPRECATED
    // init thread store envirnoment
    if (!tb_thread_store_init_env()) return tb_false;
#endif

    // ok
    return tb_true;
}
tb_void_t tb_platform_exit_env()
{
#ifdef TB_CONFIG_API_HAVE_DEPRECATED
    // exit thread store envirnoment
    tb_thread_store_exit_env();
#endif

    // exit exception envirnoment
#ifdef TB_CONFIG_EXCEPTION_ENABLE
    tb_exception_exit_env();
#endif

    // exit thread local envirnoment
#ifndef TB_CONFIG_MICRO_ENABLE
    tb_thread_local_exit_env();
#endif

    // exit dns envirnoment
#ifndef TB_CONFIG_MICRO_ENABLE
    tb_dns_exit_env();
#endif

    // exit socket envirnoment
    tb_socket_exit_env();

    // exit android envirnoment
#ifdef TB_CONFIG_OS_ANDROID
    tb_android_exit_env();
#endif
}

