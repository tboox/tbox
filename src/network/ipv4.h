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
 * @file		ipv4.h
 * @ingroup 	network
 *
 */
#ifndef TB_NETWORK_IPV4_H
#define TB_NETWORK_IPV4_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

typedef union __tb_ipv4_t
{
	tb_uint32_t u32;
	tb_uint8_t 	u8[4];

}tb_ipv4_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_void_t 			tb_ipv4_clr(tb_ipv4_t* ipv4);
tb_uint32_t 		tb_ipv4_set(tb_ipv4_t* ipv4, tb_char_t const* ip);
tb_char_t const* 	tb_ipv4_get(tb_ipv4_t const* ipv4, tb_char_t* data, tb_size_t maxn);

#endif
