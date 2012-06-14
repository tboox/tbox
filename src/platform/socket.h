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
 * @file		socket.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_SOCKET_H
#define TB_PLATFORM_SOCKET_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/* ///////////////////////////////////////////////////////////////////////
 * types
 */
// socket proto
typedef enum __tb_socket_type_t
{
 	TB_SOCKET_TYPE_NUL = 0
,	TB_SOCKET_TYPE_TCP = 1
, 	TB_SOCKET_TYPE_UDP = 2

}tb_socket_type_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t 		tb_socket_init();
tb_void_t 		tb_socket_exit();

// open
tb_handle_t 	tb_socket_open(tb_size_t type);

// connect
tb_long_t 		tb_socket_connect(tb_handle_t handle, tb_char_t const* ip, tb_size_t port);

// bind 
tb_bool_t 		tb_socket_bind(tb_handle_t handle, tb_size_t port);

// accept
tb_handle_t 	tb_socket_accept(tb_handle_t handle);

// close
tb_bool_t 		tb_socket_close(tb_handle_t handle);

// recv & send
tb_long_t 		tb_socket_recv(tb_handle_t handle, tb_byte_t* data, tb_size_t size);
tb_long_t 		tb_socket_send(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

// recv & send for udp
tb_long_t 		tb_socket_urecv(tb_handle_t handle, tb_char_t const* host, tb_size_t port, tb_byte_t* data, tb_size_t size);
tb_long_t 		tb_socket_usend(tb_handle_t handle, tb_char_t const* host, tb_size_t port, tb_byte_t* data, tb_size_t size);
	
#endif
