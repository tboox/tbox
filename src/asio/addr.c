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
#define TB_TRACE_IMPL_TAG 				"addr"

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
	// refn
	tb_atomic_t 			refn;

	// the itor
	tb_size_t 				itor;

	// the server maxn
	tb_size_t 				maxn;

	// the func
	tb_aicp_addr_func_t 	func;

	// the func data
	tb_pointer_t 			priv;

	// the sock
	tb_handle_t 			sock;

	// the aico
	tb_handle_t 			aico;

	// the server list
	tb_ipv4_t 				list[2];

	// the data
	tb_byte_t 				data[TB_DNS_RPKT_MAXN];

	// the host
	tb_char_t 				host[256];

}tb_aicp_addr_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_aicp_addr_init(tb_aicp_t* aicp, tb_char_t const* host, tb_aicp_addr_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(aicp && host && func, tb_null);

	// done
	tb_bool_t 		ok = tb_false;
	tb_aicp_addr_t* addr = tb_null;
	tb_handle_t 	sock = tb_null;
	tb_handle_t 	aico = tb_null;
	do
	{
		// init sock
		sock = tb_socket_open(TB_SOCKET_TYPE_UDP);
		tb_assert_and_check_break(sock);

		// init aico
		aico = tb_aico_init_sock(aicp, sock);
		tb_assert_and_check_break(aico);

		// make addr
		addr = (tb_aicp_addr_t*)tb_aico_pool_malloc0(aico, sizeof(tb_aicp_addr_t));
		tb_assert_and_check_break(addr);

		// init addr
		addr->refn = 2;
		addr->itor = 1;
		addr->sock = sock;
		addr->aico = aico;
		addr->func = func;
		addr->priv = data;
		addr->maxn = tb_dns_server_get(addr->list);
		tb_strlcpy(addr->host, host, sizeof(addr->host) - 1);
		sock = tb_null;
		aico = tb_null;

		// check
		tb_assert_and_check_break(addr->maxn && addr->maxn <= tb_arrayn(addr->list));

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit it
		if (addr) tb_aicp_addr_exit(addr);
		addr = tb_null;

		// exit aico
		if (aico) tb_aico_exit(aico);
		aico = tb_null;

		// exit sock
		if (sock) tb_socket_close(sock);
		sock = tb_null;
	}

	// ok?
	return addr;
}
tb_void_t tb_aicp_addr_exit(tb_handle_t haddr)
{
	tb_aicp_addr_t* addr = (tb_aicp_addr_t*)haddr;
	if (addr)
	{
//		tb_size_t refn = tb_atomic_fetch_and_dec(&addr->refn);
	
	}
}
tb_void_t tb_aicp_addr_done(tb_handle_t haddr)
{
}
tb_char_t const* tb_aicp_addr_host(tb_handle_t haddr)
{
	// check
	tb_aicp_addr_t* addr = (tb_aicp_addr_t*)haddr;
	tb_assert_and_check_return_val(addr, tb_null);
	
	// the host
	return addr->host;
}
tb_aicp_t* tb_aicp_addr_aicp(tb_handle_t haddr)
{
	// check
	tb_aicp_addr_t* addr = (tb_aicp_addr_t*)haddr;
	tb_assert_and_check_return_val(addr && addr->aico, tb_null);
	
	// the aicp
	return tb_aico_aicp(addr->aico);
}
