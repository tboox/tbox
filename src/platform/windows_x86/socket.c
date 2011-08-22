/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		socket.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * implemention
 */

tb_bool_t tb_socket_init()
{
	TB_NOT_IMPLEMENT();
	return TB_FALSE;
}
tb_void_t tb_socket_uninit()
{
	TB_NOT_IMPLEMENT();
}
tb_handle_t tb_socket_client_open(tb_char_t const* host, tb_uint16_t port, tb_int_t type, tb_bool_t is_block)
{
	TB_NOT_IMPLEMENT();
	return TB_NULL;
}
tb_void_t tb_socket_close(tb_handle_t hsocket)
{
	TB_NOT_IMPLEMENT();
}

tb_int_t tb_socket_recv(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size)
{
	TB_NOT_IMPLEMENT();
	return -1;
}

tb_int_t tb_socket_send(tb_handle_t hsocket, tb_byte_t* data, tb_size_t size)
{
	TB_NOT_IMPLEMENT();
	return -1;
}
tb_int_t tb_socket_recvfrom(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size)
{
	TB_NOT_IMPLEMENT();
	return -1;
}
tb_int_t tb_socket_sendto(tb_handle_t hsocket, tb_char_t const* host, tb_uint16_t port, tb_byte_t* data, tb_size_t size)
{
	TB_NOT_IMPLEMENT();
	return -1;
}
tb_handle_t tb_socket_server_open(tb_uint16_t port, tb_int_t type, tb_bool_t is_block)
{
	TB_NOT_IMPLEMENT();
	return TB_NULL;
}
tb_handle_t tb_socket_server_accept(tb_handle_t hserver)
{
	TB_NOT_IMPLEMENT();
	return TB_NULL;
}

