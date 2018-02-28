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
 * @file        thread_local.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_THREAD_LOCAL_H
#define TB_PLATFORM_THREAD_LOCAL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../container/single_list_entry.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the thread local initial value
#define TB_THREAD_LOCAL_INIT    {{0}, 0}

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the thread local free function type
 *
 * @param priv      the thread local private data
 */
typedef tb_void_t   (*tb_thread_local_free_t)(tb_cpointer_t priv);

/// the thread local type
typedef struct __tb_thread_local_t
{
    // the list entry
    tb_single_list_entry_t  entry;

    // have been initialized?
    tb_bool_t               inited;

    // the atomice lock of once function
    tb_atomic_t             once;

    // the free function
    tb_thread_local_free_t  free;

    /* the private data space for implementation
     *
     * - sizeof(pthread_key_t) * 2 for pthread key
     * - sizeof(DWORD) * 2 for windows key
     */
    tb_byte_t               priv[16];

}tb_thread_local_t, *tb_thread_local_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init a thread local 
 *
 * @note support to be called repeatly, thread safely and only init it once
 *
 * @code
 
    // the thread local free function
    static tb_void_t tb_thread_local_free_func(tb_cpointer_t priv)
    {
        if (priv) tb_free(priv);
    }

    // the thread function
    static tb_cpointer_t th_thread_func(tb_cpointer_t priv)
    {
        // init the thread local, only once
        static tb_thread_local_t s_local = TB_THREAD_LOCAL_INIT;
        if (!tb_thread_local_init(&s_local, tb_thread_local_free_func)) return tb_null;

        // get the thread local data 
        tb_char_t const* data = tb_null;
        if (!(data = tb_thread_local_get(&s_local)))
        {
            tb_char_t const* cstr = tb_strdup("hello");
            if (tb_thread_local_set(&s_local, cstr))
                data = cstr;
            else tb_free(cstr);
        }

        // trace
        tb_trace_i("data: %s", data);

        ...
    }

 * @endcode
 *
 * @param local         the global or static thread local (need be initialized as TB_THREAD_LOCAL_INIT)
 * @param func          the thread local free function
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_thread_local_init(tb_thread_local_ref_t local, tb_thread_local_free_t func);

/*! exit the thread local (optional)
 *
 * @note this local will be exited automaticlly after tb_exit() was called 
 *       and we call also manually call this function to exit the thread local in earlier time
 *
 * @param local         the thread local reference
 */
tb_void_t               tb_thread_local_exit(tb_thread_local_ref_t local);

/*! has thead local data on the current thread?
 *
 * @param local         the thread local reference
 *
 * @return              the thread local private data
 */
tb_bool_t               tb_thread_local_has(tb_thread_local_ref_t local);

/*! get thead local data from the current thread
 *
 * @param local         the thread local reference
 *
 * @return              the thread local private data
 */
tb_pointer_t            tb_thread_local_get(tb_thread_local_ref_t local);

/*! set thead local data to the current thread
 *
 * @note this data will be freed automaticlly after it's thread was returned 
 *
 * @param local         the thread local reference
 * @param priv          the thread local private data
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_thread_local_set(tb_thread_local_ref_t local, tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
