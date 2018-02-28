/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        singleton.h
 * @ingroup     utils
 *
 */
#ifndef TB_UTILS_SINGLETON_H
#define TB_UTILS_SINGLETON_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the singleton type enum
typedef enum __tb_singleton_type_e
{
    /// the default allocator
    TB_SINGLETON_TYPE_DEFAULT_ALLOCATOR     = 0

    /// the static allocator
,   TB_SINGLETON_TYPE_STATIC_ALLOCATOR      = 1

#ifdef TB_CONFIG_MICRO_ENABLE

    /// the user defined type
,   TB_SINGLETON_TYPE_USER                  = 2

#else

    /// the lock profiler type
,   TB_SINGLETON_TYPE_LOCK_PROFILER         = 2

    /// the ifaddrs type
,   TB_SINGLETON_TYPE_IFADDRS               = 3

    /// the timer type
,   TB_SINGLETON_TYPE_TIMER                 = 4

    /// the ltimer type
,   TB_SINGLETON_TYPE_LTIMER                = 5

    /// the aicp type, TODO deprecated
,   TB_SINGLETON_TYPE_AICP                  = 6

    /// the openssl library type
,   TB_SINGLETON_TYPE_LIBRARY_OPENSSL       = 7

    /// the mysql library type
,   TB_SINGLETON_TYPE_LIBRARY_MYSQL         = 8

    /// the sqlite3 library type
,   TB_SINGLETON_TYPE_LIBRARY_SQLITE3       = 9

    /// the thread pool type
,   TB_SINGLETON_TYPE_THREAD_POOL           = 10

    /// the transfer pool type
,   TB_SINGLETON_TYPE_TRANSFER_POOL         = 11

    /// the cookies type
,   TB_SINGLETON_TYPE_COOKIES               = 12

    /// the user defined type
,   TB_SINGLETON_TYPE_USER                  = 13

#endif

    /// the max count of the singleton type
#if defined(TB_CONFIG_MICRO_ENABLE)
,   TB_SINGLETON_TYPE_MAXN                  = TB_SINGLETON_TYPE_USER + 2
#elif defined(__tb_small__)
,   TB_SINGLETON_TYPE_MAXN                  = TB_SINGLETON_TYPE_USER + 8
#else
,   TB_SINGLETON_TYPE_MAXN                  = TB_SINGLETON_TYPE_USER + 64
#endif

}tb_singleton_type_e;

/// the singleton init func type
typedef tb_handle_t (*tb_singleton_init_func_t)(tb_cpointer_t* ppriv);

/// the singleton exit func type
typedef tb_void_t   (*tb_singleton_exit_func_t)(tb_handle_t instance, tb_cpointer_t priv);

/// the singleton kill func type
typedef tb_void_t   (*tb_singleton_kill_func_t)(tb_handle_t instance, tb_cpointer_t priv);

/// the singleton static init func type
typedef tb_bool_t   (*tb_singleton_static_init_func_t)(tb_handle_t instance, tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init singleton
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_singleton_init(tb_noarg_t);

/*! kill singleton
 */
tb_void_t           tb_singleton_kill(tb_noarg_t);

/*! exit singleton
 */
tb_void_t           tb_singleton_exit(tb_noarg_t);

/*! the singleton instance
 *
 * @param type      the singleton type
 * @param init      the singleton init func 
 * @param exit      the singleton exit func 
 * @param kill      the singleton kill func 
 * @param priv      the private data
 *
 * @return          the singleton instance handle
 */
tb_handle_t         tb_singleton_instance(tb_size_t type, tb_singleton_init_func_t init, tb_singleton_exit_func_t exit, tb_singleton_kill_func_t kill, tb_cpointer_t priv);

/*! the singleton static instance
 *
 * @code
    static tb_bool_t tb_xxxx_instance_init(tb_handle_t instance, tb_cpointer_t priv)
    {
        // init 
        // ...

        // ok
        return tb_true;
    }
    tb_xxxx_t* tb_xxxx()
    {
        // init
        static tb_atomic_t      s_binited = 0;
        static tb_xxxx_t        s_xxxx = {0};

        // init the static instance
        tb_bool_t ok = tb_singleton_static_init(&s_binited, &s_xxxx, tb_xxxx_instance_init, tb_null);
        tb_assert(ok);

        // ok
        return &s_xxxx;
    }
 * @endcode
 *
 * @param binited   the singleton static instance is inited?
 * @param instance  the singleton static instance
 * @param init      the singleton static init func 
 * @param priv      the private data
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_singleton_static_init(tb_atomic_t* binited, tb_handle_t instance, tb_singleton_static_init_func_t init, tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

