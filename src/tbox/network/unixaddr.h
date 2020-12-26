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
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      CodeHz
 * @file        unixaddr.h
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

/// the unixaddr string data maxn
#define TB_UNIXADDR_CSTR_MAXN           (108)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the unix type
 *
 * /path/to.sock
 */
typedef struct __tb_unixaddr_t
{
    /// is abstract
    tb_bool_t      is_abstract;
    /// path
    tb_char_t      path[TB_UNIXADDR_CSTR_MAXN];

}tb_unixaddr_t, *tb_unixaddr_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! clear the unixaddr
 *
 * @param unixaddr  the unixaddr
 */
tb_void_t           tb_unixaddr_clear(tb_unixaddr_ref_t unixaddr);

/*! is abstract?
 *
 * @param unixaddr  the unixaddr
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_unixaddr_is_abstract(tb_unixaddr_ref_t unixaddr);

/*! is equal?
 *
 * @param unixaddr  the unixaddr
 * @param other     the other unixaddr
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_unixaddr_is_equal(tb_unixaddr_ref_t unixaddr, tb_unixaddr_ref_t other);

/*! get the unixaddr string
 *
 * @param unixaddr  the unixaddr
 * @param data      the unixaddr data
 * @param maxn      the data maxn
 *
 * @return          the unixaddr path
 */
tb_char_t const*    tb_unixaddr_cstr(tb_unixaddr_ref_t unixaddr, tb_char_t* data, tb_size_t maxn);

/*! set the unixaddr from string
 *
 * @param unixaddr  the unixaddr
 * @param cstr      the unixaddr path
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_unixaddr_cstr_set(tb_unixaddr_ref_t unixaddr, tb_char_t const* cstr, tb_bool_t is_abstract);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
