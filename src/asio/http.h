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
 * @file		http.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_HTTP_H
#define TB_ASIO_HTTP_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"
#include "../network/http.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/// open
#define tb_aicp_http_open(handle, func, priv) 								tb_aicp_http_open_impl(handle, func, priv __tb_debug_vals__)

/// read
#define tb_aicp_http_read(handle, maxn, func, priv) 						tb_aicp_http_read_impl(handle, maxn, func, priv __tb_debug_vals__)

/// writ
#define tb_aicp_http_writ(handle, data, size, func, priv) 					tb_aicp_http_writ_impl(handle, data, size, func, priv __tb_debug_vals__)

/// seek
#define tb_aicp_http_seek(handle, offset, func, priv) 						tb_aicp_http_seek_impl(handle, offset, func, priv __tb_debug_vals__)

/// sync
#define tb_aicp_http_sync(handle, bclosing, func, priv) 					tb_aicp_http_sync_impl(handle, bclosing, func, priv __tb_debug_vals__)

/// task
#define tb_aicp_http_task(handle, delay, func, priv) 						tb_aicp_http_task_impl(handle, delay, func, priv __tb_debug_vals__)

/// open and read
#define tb_aicp_http_oread(handle, maxn, func, priv) 						tb_aicp_http_oread_impl(handle, maxn, func, priv __tb_debug_vals__)

/// open and writ
#define tb_aicp_http_owrit(handle, data, size, func, priv) 					tb_aicp_http_owrit_impl(handle, data, size, func, priv __tb_debug_vals__)

/// open and seek
#define tb_aicp_http_oseek(handle, offset, func, priv) 						tb_aicp_http_oseek_impl(handle, offset, func, priv __tb_debug_vals__)

/// read after delay
#define tb_aicp_http_read_after(handle, delay, maxn, func, priv) 			tb_aicp_http_read_after_impl(handle, delay, maxn, func, priv __tb_debug_vals__)

/// writ after delay
#define tb_aicp_http_writ_after(handle, delay, data, size, func, priv) 		tb_aicp_http_writ_after_impl(handle, delay, data, size, func, priv __tb_debug_vals__)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*! the aicp http open func type
 *
 * @param handle 	the http handle
 * @param state 	the astream state
 * @param status 	the http status
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_open_func_t)(tb_handle_t handle, tb_size_t state, tb_http_status_t const* status, tb_pointer_t priv);

/*! the aicp http read func type
 *
 * @param handle 	the http handle
 * @param state 	the astream state
 * @param data 		the readed data
 * @param real 		the real size, maybe zero
 * @param size 		the need size
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_read_func_t)(tb_handle_t handle, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv);

/*! the aicp http writ func type
 *
 * @param handle 	the http handle
 * @param state 	the astream state
 * @param data 		the writed data
 * @param real 		the real size, maybe zero
 * @param size 		the need size
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_writ_func_t)(tb_handle_t handle, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv);

/*! the aicp http seek func type
 *
 * @param handle 	the http handle
 * @param state 	the astream state
 * @param offset 	the real offset
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_seek_func_t)(tb_handle_t handle, tb_size_t state, tb_hize_t offset, tb_pointer_t priv);

/*! the aicp http sync func type
 *
 * @param handle 	the http handle
 * @param state 	the astream state
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_sync_func_t)(tb_handle_t handle, tb_size_t state, tb_pointer_t priv);

/*! the aicp http task func type
 *
 * @param handle 	the http handle
 * @param state 	the astream state
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok and continue it if need, tb_false: break it, but not break aicp
 */
typedef tb_bool_t 	(*tb_aicp_http_task_func_t)(tb_handle_t handle, tb_size_t state, tb_pointer_t priv);

/* ///////////////////////////////////////////////////////////////////////
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
tb_bool_t 			tb_aicp_http_open_impl(tb_handle_t handle, tb_aicp_http_open_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! read the http 
 *
 * @param handle 	the http handle
 * @param maxn 		the read maxn, using the default maxn if be zero
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_read_impl(tb_handle_t handle, tb_size_t maxn, tb_aicp_http_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! writ the http 
 *
 * @param handle 	the http handle
 * @param data 		the data
 * @param size 		the size
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_writ_impl(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_http_writ_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! seek the http
 *
 * @param handle 	the http handle
 * @param offset 	the offset
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_seek_impl(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! sync the http
 *
 * @param handle 	the http handle
 * @param bclosing 	sync the tail data for closing
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_sync_impl(tb_handle_t handle, tb_bool_t bclosing, tb_aicp_http_sync_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! task the http
 *
 * @param handle 	the http handle
 * @param delay 	the delay time, ms
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_task_impl(tb_handle_t handle, tb_size_t delay, tb_aicp_http_task_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and read the http, open it first if not opened 
 *
 * @param handle 	the http handle
 * @param maxn 		the read maxn, using the default maxn if be zero
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_oread_impl(tb_handle_t handle, tb_size_t maxn, tb_aicp_http_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and writ the http, open it first if not opened 
 *
 * @param handle 	the http handle
 * @param data 		the data
 * @param size 		the size
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_owrit_impl(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_http_writ_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! open and seek the http, open it first if not opened 
 *
 * @param handle 	the http handle
 * @param offset 	the offset
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_oseek_impl(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! read the http after the delay time
 *
 * @param handle 	the http handle
 * @param delay 	the delay time, ms
 * @param maxn 		the read maxn, using the default maxn if be zero
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_read_after_impl(tb_handle_t handle, tb_size_t delay, tb_size_t maxn, tb_aicp_http_read_func_t func, tb_pointer_t priv __tb_debug_decl__);

/*! writ the http after the delay time
 *
 * @param handle 	the http handle
 * @param delay 	the delay time, ms
 * @param data 		the data
 * @param size 		the size
 * @param func 		the func
 * @param priv 		the func data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_http_writ_after_impl(tb_handle_t handle, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_aicp_http_writ_func_t func, tb_pointer_t priv __tb_debug_decl__);

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
