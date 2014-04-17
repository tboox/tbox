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
 * @author		ruki
 * @file		ipv4.h
 * @ingroup 	network
 *
 */
#ifndef TB_NETWORK_IPV4_H
#define TB_NETWORK_IPV4_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// %u.%u.%u.%u for print argument
#define tb_ipv4_u8x4(ipv4) 	(ipv4).u8[0], (ipv4).u8[1], (ipv4).u8[2], (ipv4).u8[3]

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the ipv4
typedef union __tb_ipv4_t
{
	// u32 for bigendian
	tb_uint32_t u32;

	// bytes
	tb_uint8_t 	u8[4];

}tb_ipv4_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! clear the ipv4
 *
 * @param ipv4 		the ipv4
 */
tb_void_t 			tb_ipv4_clr(tb_ipv4_t* ipv4);

/*! set the ipv4 from the ip address
 *
 * @param ipv4 		the ipv4
 * @param ip 		the ip address
 *
 * @return 			the ipv4 value
 */
tb_uint32_t 		tb_ipv4_set(tb_ipv4_t* ipv4, tb_char_t const* ip);

/*! get the ipv4 address
 *
 * @param ipv4 		the ipv4
 * @param data 		the ipv4 data
 * @param maxn 		the data maxn
 *
 * @return 			the ipv4 address
 */
tb_char_t const* 	tb_ipv4_get(tb_ipv4_t const* ipv4, tb_char_t* data, tb_size_t maxn);

#endif
