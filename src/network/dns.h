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

/*! init the host list
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_dns_list_init(tb_noarg_t);

/*! add the dns server the host list
 *
 * @param host 		the server host 
 */
tb_void_t 			tb_dns_list_adds(tb_char_t const* host);

/*! del the dns server the host list
 *
 * @param host 		the server host 
 */
tb_void_t 			tb_dns_list_dels(tb_char_t const* host);

/// exit the host list
tb_void_t 			tb_dns_list_exit(tb_noarg_t);

/// dump the host list
tb_void_t 			tb_dns_list_dump(tb_noarg_t);

/*! init for looking ipv4 from the host name, non-block
 *
 * @param name 		the host name
 *
 * @return 			the looker handle
 */
tb_handle_t 		tb_dns_look_init(tb_char_t const* name);

/*! spak the looker
 *
 * @param handle 	the looker handle
 * @param ipv4 		the ipv4
 *
 * @return 			1: ok, 0: continue: -1: failed
 */
tb_long_t 			tb_dns_look_spak(tb_handle_t handle, tb_ipv4_t* ipv4);

/*! wait the looker
 *
 * @param handle 	the looker handle
 * @param timeout 	the timeout
 *
 * @return 			1: ok, 0: continue: -1: failed
 */
tb_long_t 			tb_dns_look_wait(tb_handle_t handle, tb_long_t timeout);

/*! exit the looker
 *
 * @param handle 	the looker handle
 */
tb_void_t 			tb_dns_look_exit(tb_handle_t handle);

/*! look ipv4 from the host name, block
 *
 * @param name 		the host name
 * @param ipv4 		the ipv4
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_dns_look_done(tb_char_t const* name, tb_ipv4_t* ipv4);

/*! add ipv4 to cache
 *
 * @param name 		the host name
 * @param ipv4 		the ipv4
 */
tb_void_t 			tb_dns_look_add4(tb_char_t const* name, tb_ipv4_t const* ipv4);

/*! try to look ipv4 from cache
 *
 * @param name 		the host name
 * @param ipv4 		the ipv4
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_dns_look_try4(tb_char_t const* name, tb_ipv4_t* ipv4);

#endif
