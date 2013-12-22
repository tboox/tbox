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
 * @file		addr.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_ADDR_H
#define TB_ASIO_ADDR_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"
#include "../network/network.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aicp addr impl type
typedef struct __tb_aicp_addr_impl_t
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

}tb_aicp_addr_impl_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! post the addr aice
 *
 * @param aicp 		the aicp
 * @param aice 		the aice 
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_post_addr_impl(tb_aicp_t* aicp, tb_aice_t const* aice);

#endif
