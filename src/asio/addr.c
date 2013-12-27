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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		addr.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 				"addr"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "addr.h"
#include "aico.h"
#include "aicp.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aicp addr type
typedef struct __tb_aicp_addr_t
{
	// the itor
	tb_size_t 				itor;

	// the server maxn
	tb_size_t 				maxn;

	// the server list
	tb_ipv4_t 				list[2];

	// the data
	tb_byte_t 				data[TB_DNS_RPKT_MAXN];

	// the aice
	tb_aice_t 				aice;

}tb_aicp_addr_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_aicp_addr_init(tb_aicp_t* aicp, tb_char_t const* host, tb_aicp_addr_func_t func, tb_pointer_t data)
{
	return tb_null;
}
tb_void_t tb_aicp_addr_exit(tb_handle_t haddr)
{
}
tb_void_t tb_aicp_addr_done(tb_handle_t haddr)
{
}
tb_char_t const* tb_aicp_addr_host(tb_handle_t haddr)
{
	return tb_null;
}
tb_aicp_t* tb_aicp_addr_aicp(tb_handle_t haddr)
{
	return tb_null;
}
