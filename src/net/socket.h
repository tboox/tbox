/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		socket.h
 *
 */
#ifndef TB_NET_SOCKET_H
#define TB_NET_SOCKET_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* /////////////////////////////////////////////////////////
 * types
 */

// the socket protocal type
typedef enum __tb_socket_type_t
{
	TB_SOCKET_TYPE_TCP 		= TPLAT_SOCKET_TYPE_TCP
, 	TB_SOCKET_TYPE_UDP 		= TPLAT_SOCKET_TYPE_UDP
, 	TB_SOCKET_TYPE_UNKNOWN 	= TPLAT_SOCKET_TYPE_UNKNOWN

}tb_socket_type_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// open & close
tb_handle_t tb_socket_client_open(tb_char_t const* host, tb_uint16_t port, tb_int_t type, tb_bool_t is_block);
tb_handle_t	tb_socket_server_open(tb_uint16_t port, tb_int_t type, tb_bool_t is_block);
tb_handle_t	tb_socket_server_accept(tb_handle_t hserver);
void 		tb_socket_close(tb_handle_t hsocket);

// for tcp
tb_int_t 	tb_socket_recv(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size);
tb_int_t 	tb_socket_send(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size);

// for udp
tb_int_t 	tb_socket_recvfrom(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size);
tb_int_t 	tb_socket_sendto(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size);
	
// c plus plus
#ifdef __cplusplus
}
#endif

#endif
