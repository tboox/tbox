/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		socket.h
 *
 */
#ifndef TB_PLATFORM_SOCKET_H
#define TB_PLATFORM_SOCKET_H

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
// socket proto
typedef enum __tb_socket_type_t
{
	TB_SOCKET_TYPE_TCP
, 	TB_SOCKET_TYPE_UDP
, 	TB_SOCKET_TYPE_UNKNOWN = -1

}tb_socket_type_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & unint
tb_bool_t 	tb_socket_init();
void 			tb_socket_uninit();

// client open socket
tb_handle_t 	tb_socket_client_open(tb_char_t const* host, tb_uint16_t port, tb_int_t type, tb_bool_t is_block);

// server open socket
tb_handle_t 	tb_socket_server_open(tb_uint16_t port, tb_int_t type, tb_bool_t is_block);

// server accept client socket
tb_handle_t 	tb_socket_server_accept(tb_handle_t hserver);

// close socket
void 			tb_socket_close(tb_handle_t hsocket);

/* recv data
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tb_int_t 	tb_socket_recv(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size);

/* send data
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tb_int_t 	tb_socket_send(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size);

/* recv data from host
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tb_int_t 	tb_socket_recvfrom(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size);

/* send data to host
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tb_int_t 	tb_socket_sendto(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size);
	
// c plus plus
#ifdef __cplusplus
}
#endif

#endif
