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
 * Copyright (C) 2009 - 2019, TBOOX Open Source Group.
 *
 * @author      CodeHz
 * @file        unix.h
 * @ingroup     network
 *
 */
#ifndef TB_NETWORK_UNIX_H
#define TB_NETWORK_UNIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the ipv4 string data maxn
#define TB_UNIX_CSTR_MAXN           (108)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the ipv4 type
 *
 * xxx.xxx.xxx.xxx
 */
typedef struct __tb_unix_t
{
    /// str
    tb_char_t      str[TB_UNIX_CSTR_MAXN];

}tb_unix_t, *tb_unix_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! clear the unix
 *
 * @param unix      the unix
 */
tb_void_t           tb_unix_clear(tb_unix_ref_t _unix);

/*! is equal?
 *
 * @param unix      the unix
 * @param other     the other unix
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_unix_is_equal(tb_unix_ref_t _unix, tb_unix_ref_t other);

/*! get the unix string
 *
 * @param unix      the unix
 * @param data      the unix data
 * @param maxn      the data maxn
 *
 * @return          the unix address
 */
tb_char_t const*    tb_unix_cstr(tb_unix_ref_t _unix, tb_char_t* data, tb_size_t maxn);

/*! set the unix from string
 *
 * @param unix      the unix
 * @param cstr      the unix string 
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_unix_cstr_set(tb_unix_ref_t _unix, tb_char_t const* cstr);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
