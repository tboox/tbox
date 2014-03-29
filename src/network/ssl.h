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
 * @file		ssl.h
 * @ingroup 	network
 *
 */
#ifndef TB_NETWORK_SSL_H
#define TB_NETWORK_SSL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*! the ssl read func type 
 *
 * @param priv 		the priv data for context
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size, no data: 0, failed: -1
 */
typedef tb_long_t 	(*tb_ssl_func_read_t)(tb_pointer_t priv, tb_byte_t* data, tb_size_t size);

/*! the ssl writ func type 
 *
 * @param priv 		the priv data for context
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size, no data: 0, failed: -1
 */
typedef tb_long_t 	(*tb_ssl_func_writ_t)(tb_pointer_t priv, tb_byte_t const* data, tb_size_t size);

/*! the ssl wait func type 
 *
 * @param priv 		the priv data for context
 * @param code 		the aioe code
 * @param timeout 	the timeout
 *
 * @return 			the real code, no event: 0, failed or closed: -1
 */
typedef tb_long_t 	(*tb_ssl_func_wait_t)(tb_pointer_t priv, tb_size_t code, tb_long_t timeout);

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init ssl
 *
 * @param bserver 	is server endpoint?
 *
 * @return 			the ssl handle 
 */
tb_handle_t 		tb_ssl_init(tb_bool_t bserver);

/*! exit ssl
 *
 * @param ssl 		the ssl handle
 */
tb_void_t 			tb_ssl_exit(tb_handle_t ssl);

/*! set ssl bio sock
 *
 * @param ssl 		the ssl handle
 * @param sock 		the sock handle, @note need block this socket
 */
tb_void_t 			tb_ssl_set_bio_sock(tb_handle_t ssl, tb_handle_t sock);

/*! set ssl bio read and writ func 
 *
 * @param ssl 		the ssl handle
 * @param read 		the read func
 * @param writ 		the writ func
 * #param wait 		the wait func, optional
 * @param priv 		the priv data
 */
tb_void_t 			tb_ssl_set_bio_func(tb_handle_t ssl, tb_ssl_func_read_t read, tb_ssl_func_writ_t writ, tb_ssl_func_wait_t wait, tb_pointer_t priv);

/*! open ssl
 *
 * @param ssl 		the ssl handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_ssl_open(tb_handle_t ssl);

/*! clos ssl
 *
 * @param ssl 		the ssl handle
 */
tb_void_t 			tb_ssl_clos(tb_handle_t ssl);

/*! read ssl data
 *
 * @param ssl 		the ssl handle
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size, no data: 0 and see state for waiting, failed: -1
 */
tb_long_t 			tb_ssl_read(tb_handle_t ssl, tb_byte_t* data, tb_size_t size);

/*! writ ssl data
 *
 * @param ssl 		the ssl handle
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size, no data: 0 and see state for waiting, failed: -1
 */
tb_long_t 			tb_ssl_writ(tb_handle_t ssl, tb_byte_t const* data, tb_size_t size);

/*! the ssl state see the stream ssl state
 *
 * @param ssl 		the ssl handle
 *
 * @return 			the ssl state
 */
tb_size_t 			tb_ssl_state(tb_handle_t ssl);

#endif
