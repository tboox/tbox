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
 * @file		dns.h
 * @ingroup 	network
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

// the host list
tb_bool_t 			tb_dns_list_init();
tb_void_t 			tb_dns_list_adds(tb_char_t const* host);
tb_void_t 			tb_dns_list_dels(tb_char_t const* host);
tb_void_t 			tb_dns_list_exit();
tb_void_t 			tb_dns_list_dump();

// look ipv4 for async
tb_handle_t 		tb_dns_look_init(tb_char_t const* name);
tb_long_t 			tb_dns_look_spak(tb_handle_t handle, tb_ipv4_t* ipv4);
tb_long_t 			tb_dns_look_wait(tb_handle_t handle, tb_long_t timeout);
tb_void_t 			tb_dns_look_exit(tb_handle_t handle);

// look ipv4 for block
tb_bool_t 			tb_dns_look_done(tb_char_t const* name, tb_ipv4_t* ipv4);

// look ipv4 for cache
tb_bool_t 			tb_dns_look_try4(tb_char_t const* name, tb_ipv4_t* ipv4);

#endif
