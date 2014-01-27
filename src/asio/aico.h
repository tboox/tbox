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
 * @file		aico.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AICO_H
#define TB_ASIO_AICO_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../network/ipv4.h"
#include "../platform/prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_aico_acpt(aico, aicb_func, aicb_data) 											tb_aico_acpt_impl(aico, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_conn(aico, addr, port, aicb_func, aicb_data) 								tb_aico_conn_impl(aico, addr, port, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_recv(aico, data, size, aicb_func, aicb_data) 								tb_aico_recv_impl(aico, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_send(aico, data, size, aicb_func, aicb_data) 								tb_aico_send_impl(aico, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_urecv(aico, addr, port, data, size, aicb_func, aicb_data) 					tb_aico_urecv_impl(aico, addr, port, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_usend(aico, addr, port, data, size, aicb_func, aicb_data) 					tb_aico_usend_impl(aico, addr, port, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_recvv(aico, list, size, aicb_func, aicb_data) 								tb_aico_recvv_impl(aico, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_sendv(aico, list, size, aicb_func, aicb_data) 								tb_aico_sendv_impl(aico, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_urecvv(aico, addr, port, list, size, aicb_func, aicb_data) 					tb_aico_urecvv_impl(aico, addr, port, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_usendv(aico, addr, port, list, size, aicb_func, aicb_data) 					tb_aico_usendv_impl(aico, addr, port, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_sendfile(aico, file, seek, size, aicb_func, aicb_data) 						tb_aico_sendfile_impl(aico, file, seek, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_read(aico, seek, data, size, aicb_func, aicb_data) 							tb_aico_read_impl(aico, seek, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_writ(aico, seek, data, size, aicb_func, aicb_data) 							tb_aico_writ_impl(aico, seek, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_readv(aico, seek, list, size, aicb_func, aicb_data) 						tb_aico_readv_impl(aico, seek, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_writv(aico, seek, list, size, aicb_func, aicb_data) 						tb_aico_writv_impl(aico, seek, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_fsync(aico, aicb_func, aicb_data) 											tb_aico_fsync_impl(aico, aicb_func, aicb_data __tb_debug_vals__)

#define tb_aico_acpt_after(aico, delay, aicb_func, aicb_data) 								tb_aico_acpt_after_impl(aico, delay, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_conn_after(aico, delay, addr, port, aicb_func, aicb_data) 					tb_aico_conn_after_impl(aico, delay, addr, port, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_recv_after(aico, delay, data, size, aicb_func, aicb_data) 					tb_aico_recv_after_impl(aico, delay, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_send_after(aico, delay, data, size, aicb_func, aicb_data) 					tb_aico_send_after_impl(aico, delay, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_urecv_after(aico, delay, addr, port, data, size, aicb_func, aicb_data) 		tb_aico_urecv_after_impl(aico, delay, addr, port, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_usend_after(aico, delay, addr, port, data, size, aicb_func, aicb_data) 		tb_aico_usend_after_impl(aico, delay, addr, port, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_recvv_after(aico, delay, list, size, aicb_func, aicb_data) 					tb_aico_recvv_after_impl(aico, delay, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_sendv_after(aico, delay, list, size, aicb_func, aicb_data) 					tb_aico_sendv_after_impl(aico, delay, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_urecvv_after(aico, delay, addr, port, list, size, aicb_func, aicb_data) 	tb_aico_urecvv_after_impl(aico, delay, addr, port, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_usendv_after(aico, delay, addr, port, list, size, aicb_func, aicb_data) 	tb_aico_usendv_after_impl(aico, delay, addr, port, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_sendfile_after(aico, delay, file, seek, size, aicb_func, aicb_data) 		tb_aico_sendfile_after_impl(aico, delay, file, seek, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_read_after(aico, delay, seek, data, size, aicb_func, aicb_data) 			tb_aico_read_after_impl(aico, delay, seek, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_writ_after(aico, delay, seek, data, size, aicb_func, aicb_data) 			tb_aico_writ_after_impl(aico, delay, seek, data, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_readv_after(aico, delay, seek, list, size, aicb_func, aicb_data) 			tb_aico_readv_after_impl(aico, delay, seek, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_writv_after(aico, delay, seek, list, size, aicb_func, aicb_data) 			tb_aico_writv_after_impl(aico, delay, seek, list, size, aicb_func, aicb_data __tb_debug_vals__)
#define tb_aico_fsync_after(aico, delay, aicb_func, aicb_data) 								tb_aico_fsync_after_impl(aico, delay, aicb_func, aicb_data __tb_debug_vals__)

#define tb_aico_task_run(aico, delay, aicb_func, aicb_data) 								tb_aico_task_run_impl(aico, delay, aicb_func, aicb_data __tb_debug_vals__)


/* ///////////////////////////////////////////////////////////////////////
 * types
 */

struct __tb_aice_t;
/// the aico callback type
typedef tb_bool_t (*tb_aicb_t)(struct __tb_aice_t const* aice);

/// the aico type enum
typedef enum __tb_aico_type_e
{
 	TB_AICO_TYPE_NONE 		= 0 	//!< null
,	TB_AICO_TYPE_SOCK 		= 1 	//!< sock
, 	TB_AICO_TYPE_FILE 		= 2 	//!< file
, 	TB_AICO_TYPE_TASK 		= 3 	//!< task
, 	TB_AICO_TYPE_MAXN 		= 4

}tb_aico_type_e;

/// the aico timeout enum, only for sock
typedef enum __tb_aico_timeout_e
{
	TB_AICO_TIMEOUT_ACPT 	= 0
,	TB_AICO_TIMEOUT_CONN 	= 1
, 	TB_AICO_TIMEOUT_RECV 	= 2
, 	TB_AICO_TIMEOUT_SEND 	= 3
, 	TB_AICO_TIMEOUT_MAXN 	= 4

}tb_aico_timeout_e;

/// the aico type
typedef struct __tb_aico_t
{
	/// the aicp
	tb_handle_t 		aicp;

	/// the type
	tb_size_t 			type;

	/// the pool
	tb_handle_t 		pool;

	/// the handle
	tb_handle_t 		handle;

	/// is killed?
	tb_atomic_t 		killed;

	/// is pending? must be zero or one
	tb_atomic_t 		pending;

	/// is calling?
	tb_atomic_t 		calling;

	/// the timeout for aice
	tb_atomic_t 		timeout[TB_AICO_TIMEOUT_MAXN];

	/// the exit func
	tb_void_t 			(*exit)(tb_pointer_t priv);

	/// the private data for the exit func
	tb_pointer_t 		priv;

#ifdef __tb_debug__
	/// the func
	tb_char_t const* 	func;

	/// the file
	tb_char_t const* 	file;

	/// the line
	tb_size_t 			line;
#endif

}tb_aico_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the sock aico
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param exit 		the exit func
 * @param priv 		the private data for exit func
 *
 * @return 			the aico
 */
tb_handle_t 		tb_aico_init_sock(tb_handle_t aicp, tb_handle_t handle, tb_void_t (*exit)(tb_pointer_t), tb_pointer_t priv);

/*! init the file aico
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param exit 		the exit func
 * @param priv 		the private data for exit func
 *
 * @return 			the aico
 */
tb_handle_t 		tb_aico_init_file(tb_handle_t aicp, tb_handle_t handle, tb_void_t (*exit)(tb_pointer_t ), tb_pointer_t priv);

/*! init the task aico
 *
 * @param aicp 		the aicp
 * @param bltimer 	is the lower precision timer
 * @param exit 		the exit func
 * @param priv 		the private data for exit func
 *
 * @return 			the aico
 */
tb_handle_t 		tb_aico_init_task(tb_handle_t aicp, tb_bool_t bltimer, tb_void_t (*exit)(tb_pointer_t ), tb_pointer_t priv);

/*! kill the aico
 *
 * @param aico 		the aico
 */
tb_void_t 			tb_aico_kill(tb_handle_t aico);

/*! exit the aico
 *
 * @param aico 		the aico
 */
tb_void_t 			tb_aico_exit(tb_handle_t aico);

/*! the aico aicp
 *
 * @param aico 		the aico
 *
 * @return 			the aico aicp
 */
tb_handle_t 		tb_aico_aicp(tb_handle_t aico);

/*! the aico type
 *
 * @param aico 		the aico
 *
 * @return 			the aico type
 */
tb_size_t 			tb_aico_type(tb_handle_t aico);

/*! the aico pool
 *
 * @param aico 		the aico
 *
 * @return 			the pool handle
 */
tb_handle_t 		tb_aico_pool(tb_handle_t aico);

/*! the aico handle
 *
 * @param aico 		the aico
 *
 * @return 			the aico handle
 */
tb_handle_t 		tb_aico_handle(tb_handle_t aico);

/*! the aico is pending?
 *
 * @param aico 		the aico
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_pending(tb_handle_t aico);

#ifdef __tb_debug__
/*! the aico func name from post for debug
 *
 * @param aico 		the aico
 *
 * @return 			the aico func name
 */
tb_char_t const* 	tb_aico_func(tb_handle_t aico);

/*! the aico file name from post for debug
 *
 * @param aico 		the aico
 *
 * @return 			the aico file name
 */
tb_char_t const* 	tb_aico_file(tb_handle_t aico);

/*! the aico line number from post for debug
 *
 * @param aico 		the aico
 *
 * @return 			the aico line number
 */
tb_size_t 			tb_aico_line(tb_handle_t aico);
#endif

/*! the aico timeout
 *
 * @param aico 		the aico
 * @param type 		the timeout type
 *
 * @return 			the timeout
 */
tb_long_t 			tb_aico_timeout(tb_handle_t aico, tb_size_t type);

/*! set the aico timeout
 *
 * @param aico 		the aico
 * @param type 		the timeout type
 * @param timeout 	the timeout
 */
tb_void_t 			tb_aico_timeout_set(tb_handle_t aico, tb_size_t type, tb_long_t timeout);

/*! post the acpt
 *
 * @param aico 		the aico
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_acpt_impl(tb_handle_t aico, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the conn
 *
 * @param aico 		the aico
 * @param addr 		the addr
 * @param port 		the port
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_conn_impl(tb_handle_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the recv for sock
 *
 * @param aico 		the aico
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_recv_impl(tb_handle_t aico, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the send for sock
 *
 * @param aico 		the aico
 * @param data 		the data
 * @param size 		the size, send the left file data if size == 0
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_send_impl(tb_handle_t aico, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the urecv for sock
 *
 * @param aico 		the aico
 * @param addr 		the addr
 * @param port 		the port
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_urecv_impl(tb_handle_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the usend for sock
 *
 * @param aico 		the aico
 * @param addr 		the addr
 * @param port 		the port
 * @param data 		the data
 * @param size 		the size, send the left file data if size == 0
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_usend_impl(tb_handle_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the recvv for sock
 *
 * @param aico 		the aico
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_recvv_impl(tb_handle_t aico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the sendv for sock
 *
 * @param aico 		the aico
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_sendv_impl(tb_handle_t aico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the urecvv for sock
 *
 * @param aico 		the aico
 * @param addr 		the addr
 * @param port 		the port
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_urecvv_impl(tb_handle_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the usendv for sock
 *
 * @param aico 		the aico
 * @param addr 		the addr
 * @param port 		the port
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_usendv_impl(tb_handle_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the sendfile for sock
 *
 * @param aico 		the aico
 * @param file 		the file handle
 * @param seek 		the seek
 * @param size 		the size, send the left data if size == 0
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_sendfile_impl(tb_handle_t aico, tb_handle_t file, tb_hize_t seek, tb_hize_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the read for file
 *
 * @param aico 		the aico
 * @param seek 		the seek
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_read_impl(tb_handle_t aico, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the writ for file
 *
 * @param aico 		the aico
 * @param seek 		the seek
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_writ_impl(tb_handle_t aico, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the readv for file
 *
 * @param aico 		the aico
 * @param seek 		the seek
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_readv_impl(tb_handle_t aico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the writv for file
 *
 * @param aico 		the aico
 * @param seek 		the seek
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_writv_impl(tb_handle_t aico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the fsync for file
 *
 * @param aico 		the aico
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_fsync_impl(tb_handle_t aico, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the acpt after the delay time
 *
 * @param aico 		the aico
 * @param delay		the delay time, ms
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_acpt_after_impl(tb_handle_t aico, tb_size_t delay, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the conn after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param addr 		the addr
 * @param port 		the port
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_conn_after_impl(tb_handle_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the recv for sock after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_recv_after_impl(tb_handle_t aico, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the send for sock after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param data 		the data
 * @param size 		the size, send the left file data if size == 0
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_send_after_impl(tb_handle_t aico, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the urecv for sock after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param addr 		the addr
 * @param port 		the port
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_urecv_after_impl(tb_handle_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the usend for sock after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param addr 		the addr
 * @param port 		the port
 * @param data 		the data
 * @param size 		the size, send the left file data if size == 0
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_usend_after_impl(tb_handle_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the recvv for sock after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_recvv_after_impl(tb_handle_t aico, tb_size_t delay, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the sendv for sock after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_sendv_after_impl(tb_handle_t aico, tb_size_t delay, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the urecvv for sock after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param addr 		the addr
 * @param port 		the port
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_urecvv_after_impl(tb_handle_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the usendv for sock after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param addr 		the addr
 * @param port 		the port
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_usendv_after_impl(tb_handle_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the sendfile for sock after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param file 		the file handle
 * @param seek 		the seek
 * @param size 		the size, send the left data if size == 0
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_sendfile_after_impl(tb_handle_t aico, tb_size_t delay, tb_handle_t file, tb_hize_t seek, tb_hize_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the read for file after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param seek 		the seek
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_read_after_impl(tb_handle_t aico, tb_size_t delay, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the writ for file after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param seek 		the seek
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_writ_after_impl(tb_handle_t aico, tb_size_t delay, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the readv for file after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param seek 		the seek
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_readv_after_impl(tb_handle_t aico, tb_size_t delay, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the writv for file after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param seek 		the seek
 * @param list 		the list
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_writv_after_impl(tb_handle_t aico, tb_size_t delay, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! post the fsync for file after the delay time
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_fsync_after_impl(tb_handle_t aico, tb_size_t delay, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/*! run aico task after timeout and will be auto-remove it after be expired
 *
 * only once, need continue to call it again if want to repeat task
 *
 * @param aico 		the aico
 * @param delay 	the delay time, ms
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aico_task_run_impl(tb_handle_t aico, tb_size_t delay, tb_aicb_t aicb_func, tb_pointer_t aicb_data __tb_debug_decl__);

/* ///////////////////////////////////////////////////////////////////////
 * inline
 */
static __tb_inline__ tb_long_t tb_aico_timeout_from_code(tb_handle_t aico, tb_size_t code)
{
	// init the timeout type
	static tb_size_t type[] = 
	{
		-1

	, 	TB_AICO_TIMEOUT_ACPT
	, 	TB_AICO_TIMEOUT_CONN
	, 	TB_AICO_TIMEOUT_RECV
	, 	TB_AICO_TIMEOUT_SEND
	, 	TB_AICO_TIMEOUT_RECV
	, 	TB_AICO_TIMEOUT_SEND
	, 	TB_AICO_TIMEOUT_RECV
	, 	TB_AICO_TIMEOUT_SEND
	, 	TB_AICO_TIMEOUT_RECV
	, 	TB_AICO_TIMEOUT_SEND
	, 	TB_AICO_TIMEOUT_SEND

	, 	-1
	, 	-1
	, 	-1
	, 	-1
	, 	-1

	, 	-1
	};
	tb_assert_and_check_return_val(code < tb_arrayn(type) && type[code] != (tb_size_t)-1, -1);

	// timeout
	return tb_aico_timeout(aico, type[code]);
}


#endif
