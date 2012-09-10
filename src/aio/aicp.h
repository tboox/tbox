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

/// the aio call object type
typedef struct __tb_aico_t
{
	/// the aioo
	tb_aioo_t 				aioo;

	/// the aicb
	tb_aicb_t 				aicb;

}tb_aico_t;

/// the aio event code type
typedef enum __tb_aice_code_t
{
 	TB_AICE_CODE_NULL 		= 0 	//!< for null
,	TB_AICE_CODE_SYNC 		= 1		//!< for file
, 	TB_AICE_CODE_ACPT 		= 2 	//!< for socket
, 	TB_AICE_CODE_CONN 		= 3 	//!< for socket
,	TB_AICE_CODE_READ 		= 4		//!< for all i/o object
,	TB_AICE_CODE_WRIT 		= 5		//!< for all i/o object

}tb_aice_code_t;

/// the aio sync event type
typedef struct __tb_aice_sync_t
{
	/// the ok
	tb_bool_t 				ok;

}tb_aice_sync_t;

/// the aio acpt event type
typedef struct __tb_aice_acpt_t
{
	/// the client socket 
	tb_handle_t 			sock;

}tb_aice_acpt_t;

/// the aio conn event type
typedef struct __tb_aice_conn_t
{
	/// the ok
	tb_size_t 				ok 		: 1;

	/// the port
	tb_size_t 				port 	: 31;

	/// the host
	tb_ipv4_t 				host;

}tb_aice_conn_t;

/// the aio read event type
typedef struct __tb_aice_read_t
{
	/// the read data
	tb_byte_t* 				data;

	/// the data maxn
	tb_size_t 				maxn;

	/// the data size
	tb_long_t 				size;

}tb_aice_read_t;

/// the aio writ event type
typedef struct __tb_aice_writ_t
{
	/// the writ data
	tb_byte_t* 				data;

	/// the data maxn
	tb_size_t 				maxn;

	/// the data size
	tb_long_t 				size;

}tb_aice_writ_t;

/// the aio call event type
typedef struct __tb_aice_t
{
	/// the code
	tb_size_t 				code;

	/// the aico
	tb_pointer_t 			aico;

	/// the uion
	union
	{
		tb_aice_sync_t 		sync;
		tb_aice_acpt_t 		acpt;
		tb_aice_conn_t 		conn;
		tb_aice_read_t 		read;
		tb_aice_writ_t 		writ;
	} u;

}tb_aice_t;

struct __tb_aicp_t;
/// the aio poll pool reactor type
typedef struct __tb_aicp_reactor_t
{
	/// the reference to the aio pool
	struct __tb_aicp_t* 	aicp;

	/// exit
	tb_void_t 				(*exit)(struct __tb_aicp_reactor_t* reactor);

	/// addo
	tb_bool_t 				(*addo)(struct __tb_aicp_reactor_t* reactor, tb_aico_t const* aico);

	/// delo
	tb_bool_t 				(*delo)(struct __tb_aicp_reactor_t* reactor, tb_aico_t const* aico);

	/// spak
	tb_long_t 				(*spak)(struct __tb_aicp_reactor_t* reactor, tb_aice_t const* post, tb_aice_t const** resp);

}tb_aicp_reactor_t;

/// the aicp mutex type
typedef struct __tb_aicp_mutx_t
{
	/// the pool mutx
	tb_handle_t 			pool;

	/// the post mutx
	tb_handle_t 			post;

	/// the resp mutx
	tb_handle_t 			resp;

}tb_aicp_mutx_t;

/*!the aio call pool type
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
 * queue:      [lock]          |         <= input aice to queue                           |
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
 * queue:      [lock]          |         <= output aice to queue                          |
 *                             |                                                          |
 *                             |                                                          |
 *                             |                                                          |
 *         |---------------------------------------------|                                |
 * worker: |  worker0    |   worker1    |    ...         | <= aico is readonly for worker |
 *         '---------------------------------------------'                                |
 *                |             |               |                                         |
 *                ------------------------------                                          |
 *                       [lock] |        <= the aices of the same aico must be in the same worker queue
 *                 -----------------------------                                          |
 *                |             |               |                                         |
 * aices:  |---------------------------------------------|                                |
 *         |    aice0    |    aice2     |     ...        |                                |
 *         |    aice1    |    aice3     |     ...        |                                |
 *         |     ...     |    aice4     |     ...        |                                |
 *         |     ...     |     ...      |     ...        |                                |
 *         '---------------------------------------------'                                |
 *                |              |              |                                         | 
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

	/// the aice resp
	tb_queue_t* 			resp;

	/// the reactor
	tb_aicp_reactor_t* 		rtor;

}tb_aicp_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*!init the aio call pool
 *
 * @param 	type 	the object type
 * @param 	maxn 	the maximum number of concurrent objects
 *
 * @return 	the aio call pool
 */
tb_aicp_t* 			tb_aicp_init(tb_size_t type, tb_size_t maxn);

/// exit the aio call pool
tb_void_t 			tb_aicp_exit(tb_aicp_t* aicp);

/// the object maximum number of the aio pool
tb_size_t 			tb_aicp_maxn(tb_aicp_t* aicp);

/// the object number of the aio pool
tb_size_t 			tb_aicp_size(tb_aicp_t* aicp);

/*!add the aio call object
 *
 * @param 	aicp 	the aio call pool
 * @param 	handle 	the file or sock handle
 * @param 	aicb 	the aio callback
 * @param 	odata 	the aio callback data
 *
 * @return 	the aico call object
 */
tb_aico_t const* 	tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata);

/*!del the aio object
 *
 * @param 	aicp 	the aio call pool
 * @param 	aico 	the aio call object
 */
tb_void_t 			tb_aicp_delo(tb_aicp_t* aicp, tb_aico_t const* aico);

/*!post the aio event
 *
 * @param 	aicp 	the aio call pool
 * @param 	aice 	the aio call event
 *
 */
tb_bool_t 			tb_aicp_post(tb_aicp_t* aicp, tb_aice_t const* aice);

/// post sync
tb_bool_t 			tb_aicp_sync(tb_aicp_t* aicp, tb_aico_t const* aico);

/// post acpt
tb_bool_t 			tb_aicp_acpt(tb_aicp_t* aicp, tb_aico_t const* aico);

/// post conn 
tb_bool_t 			tb_aicp_conn(tb_aicp_t* aicp, tb_aico_t const* aico, tb_char_t const* host, tb_size_t port);

/// post read
tb_bool_t 			tb_aicp_read(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size);

/// post writ
tb_bool_t 			tb_aicp_writ(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size);

/*!spak aices in the spak worker thread
 *
 * @note only one thread
 *
 * @code
 * tb_pointer_t spak_worker_thread(tb_pointer_t)
 * {
 * 		// wait will be called, so need not sleep
 * 		while (tb_aicp_spak(aicp)) ;
 * }
 * @endcode
 *
 */
tb_bool_t 			tb_aicp_spak(tb_aicp_t* aicp);

/*!spdoneak aices in the call worker thread
 *
 * @note one or some thread
 *
 * @code
 * tb_pointer_t call_worker_thread(tb_pointer_t)
 * {
 * 		while (tb_aicp_done(aicp)) tb_msleep(200);
 * }
 * @endcode
 *
 */
tb_bool_t 			tb_aicp_done(tb_aicp_t* aicp);

/// kill
tb_void_t 			tb_aicp_kill(tb_aicp_t* aicp);

#endif
