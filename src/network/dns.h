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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		dns.h
 *
 */
#ifndef TB_NETWORK_DNS_H
#define TB_NETWORK_DNS_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "ipv4.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// list
tb_bool_t 			tb_dns_list_init();
tb_void_t 			tb_dns_list_adds(tb_char_t const* host);
tb_void_t 			tb_dns_list_dels(tb_char_t const* host);
tb_void_t 			tb_dns_list_exit();
tb_ipv4_t 			tb_dns_list_fast();
tb_void_t 			tb_dns_list_dump();

// host
tb_handle_t 		tb_dns_host_init(tb_char_t const* host);
tb_long_t 			tb_dns_host_spak(tb_handle_t hdns, tb_char_t* data, tb_size_t maxn);
tb_long_t 			tb_dns_host_wait(tb_handle_t hdns, tb_size_t timeout);
tb_void_t 			tb_dns_host_exit(tb_handle_t hdns);
tb_char_t const* 	tb_dns_host_done(tb_char_t const* host, tb_char_t* data, tb_size_t maxn);

#endif
