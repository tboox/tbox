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
 * @ingroup 	aio
 *
 */
#ifndef TB_AIO_CALL_POOL_H
#define TB_AIO_CALL_POOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "aioo.h"
#include "../network/ipv4.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

struct __tb_aico_t;
struct __tb_aice_t;
struct __tb_aicp_t;
/// the callback type
typedef tb_bool_t (*tb_aicb_t)(struct __tb_aicp_t* aicp, struct __tb_aico_t const* aico, struct __tb_aice_t const* aice);

/// the aioo call object type
typedef struct __tb_aico_t
{
	/// the aioo
	tb_aioo_t 				aioo;

	/// the aicb
	tb_aicb_t 				aicb;

}tb_aico_t;

/// the aico event code enum
typedef enum __tb_aice_code_e
{
 	TB_AICE_CODE_NULL 		= 0 	//!< for null
,	TB_AICE_CODE_SYNC 		= 1		//!< for file
, 	TB_AICE_CODE_ACPT 		= 2 	//!< for sock
, 	TB_AICE_CODE_CONN 		= 3 	//!< for sock
, 	TB_AICE_CODE_CLOS 		= 4 	//!< for file or sock
,	TB_AICE_CODE_READ 		= 5		//!< for file or sock
,	TB_AICE_CODE_WRIT 		= 6		//!< for file or sock
,	TB_AICE_CODE_ERRO 		= 7		//!< for file or sock

}tb_aice_code_e;

/// the aico error code enum
typedef enum __tb_aice_error_e
{
 	TB_AICE_ERROR_UNKNOWN 	= 0

}tb_aice_error_e;

/// the aico sync event type
typedef struct __tb_aice_sync_t
{
	/// the ok
	tb_bool_t 				ok;

}tb_aice_sync_t;

/// the aico acpt event type
typedef struct __tb_aice_acpt_t
{
	/// the client socket 
	tb_handle_t 			sock;

}tb_aice_acpt_t;

/// the aico erro event type
typedef struct __tb_aice_erro_t
{
	/// the code
	tb_size_t 				code;

}tb_aice_erro_t;

/// the aico conn event type
typedef struct __tb_aice_conn_t
{
	/// the ok
	tb_long_t 				ok;

	/// the port
	tb_size_t 				port;

	/// the host
	tb_ipv4_t 				host;

}tb_aice_conn_t;

/// the aico clos event type
typedef struct __tb_aice_clos_t
{
	/// the ok
	tb_bool_t 				ok;

}tb_aice_clos_t;

/// the aico read event type
typedef struct __tb_aice_read_t
{
	/// the read data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_long_t 				real;

}tb_aice_read_t;

/// the aico writ event type
typedef struct __tb_aice_writ_t
{
	/// the writ data
	tb_byte_t* 				data;

	/// the data size
	tb_size_t 				size;

	/// the data real
	tb_long_t 				real;

}tb_aice_writ_t;

/// the aico event type
typedef struct __tb_aice_t
{
	/// the code
	tb_size_t 				code;

	/// the aico
	tb_aico_t* 				aico;

	/// the uion
	union
	{
		tb_aice_sync_t 		sync;
		tb_aice_acpt_t 		acpt;
		tb_aice_conn_t 		conn;
		tb_aice_clos_t 		clos;
		tb_aice_erro_t 		erro;
		tb_aice_read_t 		read;
		tb_aice_writ_t 		writ;
	} u;

}tb_aice_t;

struct __tb_aicp_t;
/// the aico pool reactor type
typedef struct __tb_aicp_reactor_t
{
	/// aicp
	struct __tb_aicp_t* 	aicp;

	/// exit
	tb_void_t 				(*exit)(struct __tb_aicp_reactor_t* reactor);

	/// addo
	tb_bool_t 				(*addo)(struct __tb_aicp_reactor_t* reactor, tb_aico_t const* aico);

	/// delo
	tb_bool_t 				(*delo)(struct __tb_aicp_reactor_t* reactor, tb_aico_t const* aico);

	/// spak
	tb_long_t 				(*spak)(struct __tb_aicp_reactor_t* reactor, tb_aice_t* resp, tb_long_t timeout);

}tb_aicp_reactor_t;

/// the aicp mutex type
typedef struct __tb_aicp_mutx_t
{
	/// the pool mutx
	tb_handle_t 			pool;

	/// the post mutx
	tb_handle_t 			post;

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
	/// the object type
	tb_size_t 				type;

	/// the object maxn
	tb_size_t 				maxn;

	/// is killed?
	tb_size_t 				kill;

	/// the aicp mutx
	tb_aicp_mutx_t 			mutx;

	/// the aico pool
	tb_handle_t 			pool;

	/// the aice post
	tb_queue_t* 			post;

	/// the reactor
	tb_aicp_reactor_t* 		rtor;

}tb_aicp_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the aicp
 *
 * @param type 		the aico type
 * @param maxn 		the aico maxn
 *
 * @return 			the aicp
 */
tb_aicp_t* 			tb_aicp_init(tb_size_t type, tb_size_t maxn);

/*! exit the aicp
 *
 * @note 			not multi-thread safe
 *
 * @param aicp 		the aicp
 */ 	
tb_void_t 			tb_aicp_exit(tb_aicp_t* aicp);

/*! the aico maxn
 *
 * @param aicp 		the aicp
 *
 * @return 			the aico maxn
 */
tb_size_t 			tb_aicp_maxn(tb_aicp_t* aicp);

/*! the aico size
 *
 * @param aicp 		the aicp
 *
 * @return 			the aico size
 */
tb_size_t 			tb_aicp_size(tb_aicp_t* aicp);

/*! add the aico
 *
 * @param aicp 		the aicp
 * @param handle 	the aico native handle: file or sock
 * @param aicb 		the aico callback, @note: cannot done weight work
 * @param odata 	the aico data for callback
 *
 * @return 			the aico
 */
tb_aico_t const* 	tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata);

/*! del the aico
 *
 * @param aicp 		the aicp
 * @param aico 		the aico
 */
tb_void_t 			tb_aicp_delo(tb_aicp_t* aicp, tb_aico_t const* aico);

/*! post the sync event
 *
 * @param aicp 		the aicp
 * @param aice 		the aico event
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_sync(tb_aicp_t* aicp, tb_aico_t const* aico);

/*! post the acpt event
 *
 * @param aicp 		the aicp
 * @param aice 		the aico event *
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_acpt(tb_aicp_t* aicp, tb_aico_t const* aico);

/*! post the clos event
 *
 * @param aicp 		the aicp
 * @param aice 		the aico event *
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_clos(tb_aicp_t* aicp, tb_aico_t const* aico);

/*! post the conn event
 *
 * @param aicp 		the aicp
 * @param aice 		the aico event
 * @param host 		the host
 * @param port 		the port
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_conn(tb_aicp_t* aicp, tb_aico_t const* aico, tb_char_t const* host, tb_size_t port);

/*! post the read event
 *
 * @param aicp 		the aicp
 * @param aice 		the aico event
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_read(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size);

/*! post the writ event
 *
 * @param aicp 		the aicp
 * @param aice 		the aico event
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_writ(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t const* data, tb_size_t size);

/*! spak aicp
 *
 * @code
 * tb_pointer_t spak_worker_thread(tb_pointer_t)
 * {
 * 		// wait will be called, so need not sleep
 * 		while (tb_aicp_spak(aicp, -1) >= 0) ;
 * }
 * @endcode
 *
 * @param aicp 		the aicp
 * @param timeout 	the timeout
 *
 * @return 			ok: the aice count, timeout: 0, fail: < 0
 */
tb_long_t 			tb_aicp_spak(tb_aicp_t* aicp, tb_long_t timeout);

/*! kill the spak
 *
 * @param aicp 		the aicp
 */
tb_void_t 			tb_aicp_kill(tb_aicp_t* aicp);

#endif
