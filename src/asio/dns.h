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
 * @file		dns.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_DNS_H
#define TB_ASIO_DNS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"
#include "../network/ipv4.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the aicp dns func type
typedef tb_void_t 	(*tb_aicp_dns_func_t)(tb_handle_t handle, tb_char_t const* host, tb_ipv4_t const* dns, tb_pointer_t data);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the dns 
 *
 * @param aicp 		the aicp
 * @param timeout 	the timeout, ms
 * @param func 		the func
 * @param data 		the func data
 *
 * @return 			the dns handle
 */
tb_handle_t 		tb_aicp_dns_init(tb_aicp_t* aicp, tb_long_t timeout, tb_aicp_dns_func_t func, tb_pointer_t data);

/*! kill the dns
 *
 * @param handle 	the dns handle
 */
tb_void_t 			tb_aicp_dns_kill(tb_handle_t handle);

/*! exit the dns
 *
 * @param handle 	the dns handle
 * @param bcalling 	exit it at the self callback?
 */
tb_void_t 			tb_aicp_dns_exit(tb_handle_t handle, tb_bool_t bcalling);

/*! done the dns
 *
 * @param handle 	the dns handle
 * @param host 		the host
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_dns_done(tb_handle_t handle, tb_char_t const* host);

/*! the dns aicp
 *
 * @param handle 	the dns handle
 *
 * @return 			the aicp
 */
tb_aicp_t* 			tb_aicp_dns_aicp(tb_handle_t handle);

#endif
