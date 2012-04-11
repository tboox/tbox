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
 * \author		ruki
 * \file		aicp.h
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

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the callback type
struct __tb_aico_t;
struct __tb_aice_t;
struct __tb_aicp_t;
typedef tb_bool_t (*tb_aicb_t)(struct __tb_aicp_t* aicp, struct __tb_aico_t const* aico, struct __tb_aice_t const* aice);

// the aio call object type
typedef struct __tb_aico_t
{
	// the aioo
	tb_aioo_t 				aioo;

	// the aicb
	tb_aicb_t 				aicb;

}tb_aico_t;

// the aio event code type
typedef enum __tb_aice_code_t
{
 	TB_AICE_CODE_NULL 		= 0
,	TB_AICE_CODE_SYNC 		= 1		//!< for file
, 	TB_AICE_CODE_CONN 		= 2 	//!< for socket
,	TB_AICE_CODE_READ 		= 3		//!< for all i/o object
,	TB_AICE_CODE_WRIT 		= 4		//!< for all i/o object

}tb_aice_code_t;

// the aio conn event type
typedef struct __tb_aice_conn_t
{
	// the host
	tb_ipv4_t 				host;

	// the port
	tb_size_t 				port;

}tb_aice_conn_t;

// the aio read event type
typedef struct __tb_aice_read_t
{
	// the read data
	tb_byte_t* 				data;

	// the data size
	tb_size_t 				size;

}tb_aice_read_t;

// the aio writ event type
typedef struct __tb_aice_writ_t
{
	// the writ data
	tb_byte_t* 				data;

	// the data size
	tb_size_t 				size;

}tb_aice_writ_t;

// the aio call event type
typedef struct __tb_aice_t
{
	// the ok
	tb_long_t 				ok;

	// the code
	tb_size_t 				code;

	// the aico
	tb_aico_t const* 		aico;

	// the uion
	union
	{
		tb_aice_conn_t 		conn;
		tb_aice_read_t 		read;
		tb_aice_writ_t 		writ;
	} u;

}tb_aice_t;

// the aio poll pool reactor type
typedef struct __tb_aicp_t;
typedef struct __tb_aicp_reactor_t
{
	// the reference to the aio pool
	struct __tb_aicp_t* 	aicp;

	// exit
	tb_void_t 				(*exit)(struct __tb_aicp_reactor_t* reactor);

	// addo
	tb_bool_t 				(*addo)(struct __tb_aicp_reactor_t* reactor, tb_aico_t const* aico);

	// delo
	tb_bool_t 				(*delo)(struct __tb_aicp_reactor_t* reactor, tb_aico_t const* aico);

	// post
	tb_bool_t 				(*post)(struct __tb_aicp_reactor_t* reactor, tb_aice_t const* aice);

	// wait
	tb_long_t 				(*wait)(struct __tb_aicp_reactor_t* reactor, tb_long_t timeout);

}tb_aicp_reactor_t;

// the aio call pool type
typedef struct __tb_aicp_t
{
	// the object type
	tb_size_t 				type;

	// the object maxn
	tb_size_t 				maxn;

	// the aicp mutex
	tb_handle_t 			mutx;

	// the aico pool
	tb_handle_t 			pool;

	// the aice post
	tb_handle_t 			post;

	// the reactor
	tb_aicp_reactor_t* 		rtor;

}tb_aicp_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*!init the aio pool
 *
 * @param 	type 	the object type
 * @param 	maxn 	the maximum number of concurrent objects
 *
 * @return 	the aio pool
 */
tb_aicp_t* 			tb_aicp_init(tb_size_t type, tb_size_t maxn);

/// exit the aio pool
tb_void_t 			tb_aicp_exit(tb_aicp_t* aicp);

/// the object maximum number of the aio pool
tb_size_t 			tb_aicp_maxn(tb_aicp_t* aicp);

/// the object number of the aio pool
tb_size_t 			tb_aicp_size(tb_aicp_t* aicp);

/*!add the aio object
 *
 * @param 	aicp 	the aio pool
 * @param 	handle 	the file or sock handle
 *
 * @return 	the aico data
 */
tb_aico_t const* 	tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata);

/*!del the aio object
 *
 * @param 	aicp 	the aio pool
 * @param 	handle 	the aico handle
 */
tb_void_t 			tb_aicp_delo(tb_aicp_t* aicp, tb_aico_t const* aico);

/*!post the aio event
 *
 * @param 	aicp 	the aio pool
 * @param 	aice 	the aice data
 *
 * @return 	return TB_TRUE if ok
 *
 */
tb_bool_t 			tb_aicp_post(tb_aicp_t* aicp, tb_aice_t const* aice);

/// post sync
tb_bool_t 			tb_aicp_sync(tb_aicp_t* aicp, tb_aico_t const* aico);

/// post conn 
tb_bool_t 			tb_aicp_conn(tb_aicp_t* aicp, tb_aico_t const* aico, tb_char_t const* host, tb_size_t port);

/// post read
tb_bool_t 			tb_aicp_read(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size);

/// post writ
tb_bool_t 			tb_aicp_writ(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size);

/*!wait the aio objects in the pool
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param 	aicp 	the aio pool
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	return 1 if ok, return 0 if timeout, return -1 if error
 */
tb_long_t 			tb_aicp_wait(tb_aicp_t* aicp, tb_long_t timeout);

// spak aice
tb_long_t 			tb_aicp_spak(tb_aicp_t* aicp);

#endif
