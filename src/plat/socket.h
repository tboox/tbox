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
#ifndef TPLAT_SOCKET_H
#define TPLAT_SOCKET_H

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
typedef enum __tplat_socket_type_t
{
	TPLAT_SOCKET_TYPE_TCP
, 	TPLAT_SOCKET_TYPE_UDP
, 	TPLAT_SOCKET_TYPE_UNKNOWN = -1

}tplat_socket_type_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & unint
tplat_bool_t 	tplat_socket_init();
void 			tplat_socket_uninit();

// client open socket
tplat_handle_t 	tplat_socket_client_open(tplat_char_t const* host, tplat_uint16_t port, tplat_int_t type, tplat_bool_t is_block);

// server open socket
tplat_handle_t 	tplat_socket_server_open(tplat_uint16_t port, tplat_int_t type, tplat_bool_t is_block);

// server accept client socket
tplat_handle_t 	tplat_socket_server_accept(tplat_handle_t hserver);

// close socket
void 			tplat_socket_close(tplat_handle_t hsocket);

/* recv data
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tplat_int_t 	tplat_socket_recv(tplat_handle_t hsocket, tplat_byte_t* data, tplat_size_t size);

/* send data
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tplat_int_t 	tplat_socket_send(tplat_handle_t hsocket, tplat_byte_t* data, tplat_size_t size);

/* recv data from host
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tplat_int_t 	tplat_socket_recvfrom(tplat_handle_t hsocket, tplat_char_t const* host, tplat_uint16_t port, tplat_byte_t* data, tplat_size_t size);

/* send data to host
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tplat_int_t 	tplat_socket_sendto(tplat_handle_t hsocket, tplat_char_t const* host, tplat_uint16_t port, tplat_byte_t* data, tplat_size_t size);
	
// c plus plus
#ifdef __cplusplus
}
#endif

#endif
