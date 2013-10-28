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
 * @file		aicp.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_CALL_POOL_H
#define TB_ASIO_CALL_POOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "aioo.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

struct __tb_aice_t;
struct __tb_aicp_t;
/// the callback type
typedef tb_bool_t (*tb_aicb_t)(struct __tb_aicp_t* aicp, struct __tb_aice_t const* aice);

/// the aice code enum
typedef enum __tb_aice_code_e
{
 	TB_AICE_CODE_NULL 		= 0 	//!< for null
, 	TB_AICE_CODE_ACPT 		= 1 	//!< for sock
, 	TB_AICE_CODE_CONN 		= 2 	//!< for sock
,	TB_AICE_CODE_RECV 		= 3		//!< for sock
,	TB_AICE_CODE_SEND 		= 4		//!< for sock
,	TB_AICE_CODE_READ 		= 5		//!< for file
,	TB_AICE_CODE_WRIT 		= 6		//!< for file

}tb_aice_code_e;

/// the aice state code enum
typedef enum __tb_aice_state_e
{
 	TB_AICE_STATE_OK 					= 0
, 	TB_AICE_STATE_FAILED 				= 1
,	TB_AICE_STATE_CLOSED 				= 2

}tb_aice_state_e;

/// the acpt aice type
typedef struct __tb_aice_acpt_t
{
	/// the client socket 
	tb_handle_t 			sock;

}tb_aice_acpt_t;

/// the conn aice type
typedef struct __tb_aice_conn_t
{
	/// the port
	tb_size_t 				port;

	/// the host, @note: reference only
	tb_char_t const* 		host;

}tb_aice_conn_t;

/// the read aice type
typedef struct __tb_aice_read_t
{
	/// the read data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_size_t 				real;

	/// the file seek
	tb_hize_t 				seek;

}tb_aice_read_t;

/// the writ aice type
typedef struct __tb_aice_writ_t
{
	/// the writ data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_size_t 				real;

	/// the file seek
	tb_hize_t 				seek;

}tb_aice_writ_t;

/// the recv aice type
typedef struct __tb_aice_recv_t
{
	/// the recv data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_size_t 				real;

}tb_aice_recv_t;

/// the send aice type
typedef struct __tb_aice_send_t
{
	/// the send data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_size_t 				real;

}tb_aice_send_t;

/// the aice type
typedef struct __tb_aice_t
{
	/// the code
	tb_size_t 				code 	: 8;

	/// the state
	tb_size_t 				state 	: 8;

	/// the otype
	tb_size_t 				otype 	: 8;

	/// the handle
	tb_handle_t 			handle;

	/// the aicb
	tb_aicb_t 				aicb;

	/// the data
	tb_cpointer_t 			data;

	/// the uion
	union
	{
		tb_aice_acpt_t 		acpt;
		tb_aice_conn_t 		conn;
		tb_aice_read_t 		read;
		tb_aice_writ_t 		writ;
		tb_aice_recv_t 		recv;
		tb_aice_send_t 		send;

	} u;

}tb_aice_t;

struct __tb_aicp_t;
/// the aico pool reactor type
typedef struct __tb_aicp_reactor_t
{
	/// aicp
	struct __tb_aicp_t* 	aicp;

	/// kill
	tb_void_t 				(*kill)(struct __tb_aicp_reactor_t* reactor);

	/// exit
	tb_void_t 				(*exit)(struct __tb_aicp_reactor_t* reactor);

	/// addo
	tb_bool_t 				(*addo)(struct __tb_aicp_reactor_t* reactor, tb_handle_t handle, tb_size_t otype);

	/// delo
	tb_bool_t 				(*delo)(struct __tb_aicp_reactor_t* reactor, tb_handle_t handle);

	/// post
	tb_long_t 				(*post)(struct __tb_aicp_reactor_t* reactor, tb_aice_t* reqt);

	/// spak
	tb_long_t 				(*spak)(struct __tb_aicp_reactor_t* reactor, tb_aice_t* resp, tb_long_t timeout);

}tb_aicp_reactor_t;

/// the aicp mutex type
typedef struct __tb_aicp_mutx_t
{
	/// the pool mutx
	tb_handle_t 			pool;

}tb_aicp_mutx_t;

/*! the aico pool type
 *
 * <pre>
 *
 * init:                      ...
 *                             |
 *       |------------------------------------------------|
 * addo: | aico0   aico1   aico2   aico3      ...         |
 *       '------------------------------------------------'
 *                             | 
 *                            ...
 *                             |
 * post: |------------------------------------------------|
 * aice: | aice0   aice1   aice2   aice3      ...         | <-----------------------------
 *       '------------------------------------------------'                               |
 *                             |                                                          |
 *                             |                                                          |
 * queue:      [lock]          |         <= input aices                                   |
 *                             |                                                          |
 *                             |------------------------------------------                |
 *                             |                                  |       |               |
 * aipp: |------|-------|-------|-------|---- ... --|-----|    |-----|    |               |
 * aico: | aico0  aico1   aico2   aico3       ...         |    |  |  |    |               |
 * wait: |------|-------|-------|-------|---- ... --|-----|    |aice4|    |               |
 *       |   |              |                             |    |  |  |    |               |
 *       | aice0           aice2                          |    |aice5|    |               |
 *       |   |              |                             |    |  |  |    |               |
 *       | aice1           ...                            |    |aice6|    |               |
 *       |   |                                            |    |  |  |    |               |
 *       | aice3                                          |    |aice7| <= the other aices queue which is not support for aipp or iocp
 *       |   |                                            |    |  |  | <= spank it first if non-empty and no waiting
 *       |  ...                                           |    | ... |    |               |
 *       '------------------------------------------------'    |     |    |               |
 *                             or                              |     |    |               |
 * iocp: |------------------------------------------------|    |     |    |               |
 *       |                 wait port                      |    |     |    |               |
 *       '------------------------------------------------'    '-----'    |               |
 *                 |           |           |                      |       |               |
 *                 |           |           |                      |       |               |
 *                 |           |           |                      |    --->---            |
 *                 |           |           |                      |   | spank |           |
 *                 |           |           |                      |    ---<---            |
 *                 |           |           |                      |       |               |
 * resp: |---------------------|----------------------------------'-------'               |
 *                             |                                                          |
 *                             |         <= output aices                                  |
 *                             |                                                          |
 *         |---------------------------------------------|                                |
 * worker: |  worker0    |   worker1    |    ...         | <= aico is readonly for worker |
 *         '---------------------------------------------'                                |
 *                |             |               |                                         |
 * aices:  |---------------------------------------------|                                |
 *         |    aice0    |    aice2     |     ...        |                                |
 *         |    aice1    |    aice3     |     ...        |                                |
 *         |     ...     |    aice4     |     ...        |                                |
 *         |     ...     |     ...      |     ...        |                                |
 *         '---------------------------------------------'                                |
 *                |              |              |                                         |  *                             |                                                          |
 * done:   |---------------------------------------------|                                |
 *         |   caller0   |   caller2    |     ...        |                                |
 *         |   caller1   |     ...      |     ...        |                                |
 *         |     ...     |   caller2    |     ...        |                                |
 *         |     ...     |     ...      |     ...        |                                |
 *         '---------------------------------------------'                                |
 *                |              |                                                        | 
 *               ...            ...                                                       |
 *           post evet          end ----                                                  |
 *                |                     |                                                 |
 *                '---------------------|------------------------------------------------>'
 *                                      |
 * kill:               ...              |
 *                      |               |
 * exit:               ...    <---------'
 *
 * </pre>
 *
 */
typedef struct __tb_aicp_t
{
	/// the object maxn
	tb_size_t 				maxn;

	/// is killed?
	tb_size_t 				kill;

	/// the aicp mutx
	tb_aicp_mutx_t 			mutx;

	/// the data pool
	tb_handle_t 			pool;

	/// the reactor
	tb_aicp_reactor_t* 		rtor;

	// the worker size
	tb_atomic_t 			work;

}tb_aicp_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the aicp
 *
 * @param maxn 		the aico maxn
 *
 * @return 			the aicp
 */
tb_aicp_t* 			tb_aicp_init(tb_size_t maxn);

/*! exit the aicp
 *
 * @note 			not multi-thread safe
 *
 * @param aicp 		the aicp
 */ 	
tb_void_t 			tb_aicp_exit(tb_aicp_t* aicp);

/*! add the aico
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param otype 	the otype
 */
tb_bool_t 			tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t otype);

/*! del the aico
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 */
tb_void_t 			tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t handle);

/*! post the acpt event
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_acpt(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the conn event
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param host 		the host
 * @param port 		the port
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_conn(tb_aicp_t* aicp, tb_handle_t handle, tb_char_t const* host, tb_size_t port, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the read event for file
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param seek 		the seek
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_read(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t seek, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the writ event for file
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param seek 		the seek
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_writ(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the recv event for sock
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_recv(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! post the send event for sock
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param data 		the data
 * @param size 		the size
 * @param aicb_func the callback func
 * @param aicb_data the callback data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_send(tb_aicp_t* aicp, tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data);

/*! loop aicp
 *
 * @code
 * tb_pointer_t worker_thread(tb_pointer_t)
 * {
 * 		tb_aicp_loop(aicp, -1);
 * }
 * @endcode
 *
 * @param aicp 		the aicp
 * @param timeout 	the timeout 
 */
tb_void_t 			tb_aicp_loop(tb_aicp_t* aicp, tb_long_t timeout);

/*! kill the spak
 *
 * @param aicp 		the aicp
 */
tb_void_t 			tb_aicp_kill(tb_aicp_t* aicp);

#endif
