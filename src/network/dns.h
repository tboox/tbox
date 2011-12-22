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


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * the interfaces
 */

// manage dns server
tb_void_t 				tb_dns_server_add(tb_char_t const* ip);
tb_void_t 				tb_dns_server_del(tb_char_t const* ip);
tb_void_t 				tb_dns_server_dump();

// lookup host ip from name
tb_char_t const* 	tb_dns_lookup(tb_char_t const* host, tb_char_t* ip);


#endif
