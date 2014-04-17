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
 * @file		http.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_HTTP_H
#define TB_ASIO_HTTP_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"
#include "../network/http.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the aicp http open func type
 *
 * @param http 		the http handle
 * @param state 	the state
 * @param status 	the http status
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_open_func_t)(tb_handle_t http, tb_size_t state, tb_http_status_t const* status, tb_pointer_t priv);

/*! the aicp http read func type
 *
 * @param http 		the http handle
 * @param state 	the state
 * @param data 		the readed data
 * @param real 		the real size, maybe zero
 * @param size 		the need size
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_read_func_t)(tb_handle_t http, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv);

/*! the aicp http seek func type
 *
 * @param http 		the http handle
 * @param state 	the state
 * @param offset 	the real offset
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_seek_func_t)(tb_handle_t http, tb_size_t state, tb_hize_t offset, tb_pointer_t priv);

/*! the aicp http task func type
 *
 * @param http 		the http handle
 * @param state 	the state
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok, tb_false: error, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_task_func_t)(tb_handle_t http, tb_size_t state, tb_pointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the http 
 *
 * @param aicp 		the aicp
 *
 * @return 			the http handle
 */
tb_handle_t 		tb_aicp_http_init(tb_aicp_t* aicp);

/*! kill the http
 *
 * @param handle 	the http handle
 */
tb_void_t 			tb_aicp_http_kill(tb_handle_t handle);

/*! close the http
 *
 * @param handle 	the http handle
 * @param bcalling 	close it at the self callback?
 */
tb_void_t 			tb_aicp_http_clos(tb_handle_t handle, tb_bool_t bcalling);

/*! exit the http
 *
 * @param handle 	the http handle
 * @param bcalling 	exit it at the self callback?
 */
tb_void_t 			tb_aicp_http_exit(tb_handle_t handle, tb_bool_t bcalling);

/*! open the http 
 *
 * @param handle 	the http handle
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_open(tb_handle_t handle, tb_aicp_http_open_func_t func, tb_pointer_t priv);

/*! read the http
 *
 * @param handle 	the http handle
 * @param size 		the read size, using the default size if be zero
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_read(tb_handle_t handle, tb_size_t size, tb_aicp_http_read_func_t func, tb_pointer_t priv);

/*! read the http after the delay time
 *
 * @param handle 	the http handle
 * @param delay 	the delay time, ms
 * @param size 		the read size, using the default size if be zero
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_read_after(tb_handle_t handle, tb_size_t delay, tb_size_t size, tb_aicp_http_read_func_t func, tb_pointer_t priv);

/*! seek the http
 *
 * @param handle 	the http handle
 * @param offset 	the offset
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_seek(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv);

/*! task the http
 *
 * @param handle 	the http handle
 * @param delay 	the delay time, ms
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_task(tb_handle_t handle, tb_size_t delay, tb_aicp_http_task_func_t func, tb_pointer_t priv);

/*! open and read the http, open it first if not opened 
 *
 * @param handle 	the http handle
 * @param size 		the read size, using the default size if be zero
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_oread(tb_handle_t handle, tb_size_t size, tb_aicp_http_read_func_t func, tb_pointer_t priv);

/*! open and seek the http, open it first if not opened 
 *
 * @param handle 	the http handle
 * @param offset 	the offset
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_oseek(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv);

/*! the http aicp
 *
 * @param handle 	the http handle
 *
 * @return 			the aicp
 */
tb_aicp_t* 			tb_aicp_http_aicp(tb_handle_t handle);

/*! the http option
 *
 * @param handle 	the http handle
 * @param option 	the http option
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_option(tb_handle_t handle, tb_size_t option, ...);


#endif
