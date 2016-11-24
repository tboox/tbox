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
 * @file        addrinfo.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "addrinfo"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "addrinfo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_POSIX_HAVE_GETADDRINFO) || \
    defined(TB_CONFIG_POSIX_HAVE_GETHOSTBYNAME)
#   define TB_ADDRINFO_ADDR_IMPL
#   include "posix/addrinfo.c"
#   undef TB_ADDRINFO_ADDR_IMPL
#else
tb_bool_t tb_addrinfo_addr(tb_char_t const* name, tb_ipaddr_ref_t addr)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif

#if defined(TB_CONFIG_POSIX_HAVE_GETNAMEINFO) || \
    defined(TB_CONFIG_POSIX_HAVE_GETHOSTBYADDR)
#   define TB_ADDRINFO_NAME_IMPL
#   include "posix/addrinfo.c"
#   undef TB_ADDRINFO_NAME_IMPL
#else
tb_char_t const* tb_addrinfo_name(tb_ipaddr_ref_t addr, tb_char_t* name, tb_size_t maxn)
{
    tb_trace_noimpl();
    return tb_null;
}
#endif

