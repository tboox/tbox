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
 * @file        ipv6.h
 * @ingroup     network
 *
 */
#ifndef TB_NETWORK_IPV6_H
#define TB_NETWORK_IPV6_H

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

/// the ipv6 string data maxn
#define TB_IPV6_CSTR_MAXN           (40)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the ipv6 type
 *
 * <pre>
 * xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
 * xxxx::xxxx:xxxx
 * ::ffff:xxx.xxx.xxx.xxx
 * </pre>
 */
typedef union __tb_ipv6_t
{
    /// u32, little-endian
    tb_uint32_t     u32[4];

    /// u16, little-endian
    tb_uint16_t     u16[8];

    /// u8
    tb_uint8_t      u8[16];

}tb_ipv6_t, *tb_ipv6_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! clear the ipv6
 *
 * @param ipv6      the ipv6
 */
tb_void_t           tb_ipv6_clear(tb_ipv6_ref_t ipv6);

/*! get the ipv6 string
 *
 * @param ipv6      the ipv6
 * @param data      the ipv6 string data
 * @param maxn      the ipv6 string data maxn
 *
 * @return          the ipv6 string
 */
tb_char_t const*    tb_ipv6_cstr(tb_ipv6_ref_t ipv6, tb_char_t* data, tb_size_t maxn);

/*! set the ipv6 from string
 *
 * @param ipv6      the ipv6
 * @param cstr      the ipv6 string
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_ipv6_set_cstr(tb_ipv6_ref_t ipv6, tb_char_t const* cstr);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
