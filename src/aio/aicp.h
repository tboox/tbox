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
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aio call object type
typedef struct __tb_aico_t
{


}tb_aico_t;

// the aio call event type
typedef struct __tb_aice_t
{


}tb_aice_t;

// the aio call pool type
typedef struct __tb_aicp_t
{


}tb_aicp_t;

// the callback type
typedef tb_bool_t (*tb_aicb_t)(tb_aicp_t* aicp, tb_aico_t const* aico, tb_aice_t const* aice);

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
tb_aicp_t* 		tb_aicp_init(tb_size_t type, tb_size_t maxn);

/// exit the aio pool
tb_void_t 		tb_aicp_exit(tb_aicp_t* aicp);

/// the object maximum number of the aio pool
tb_size_t 		tb_aicp_maxn(tb_aicp_t* aicp);

/// the object number of the aio pool
tb_size_t 		tb_aicp_size(tb_aicp_t* aicp);

/*!add the aio object
 *
 * @param 	aicp 	the aio pool
 * @param 	handle 	the handle of the aio object
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata);

/*!del the aio object
 *
 * @param 	aicp 	the aio pool
 * @param 	handle 	the handle of the aio object
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata);

/*!add the aio event
 *
 * @param 	aicp 	the aio pool
 * @param 	handle 	the handle of the aio object
 * @param 	etype 	the event type
 *
 */
tb_void_t 		tb_aicp_adde(tb_aicp_t* aicp, tb_handle_t handle, tb_aice_t const* aice);

/*!del the aio event
 *
 * @param 	aicp 	the aio pool
 * @param 	handle 	the handle of the aio object
 *
 */
tb_void_t 		tb_aicp_dele(tb_aicp_t* aicp, tb_handle_t handle, tb_aice_t const* aice);

/*!set the aio odata
 *
 * @param 	aicp 	the aio pool
 * @param 	handle 	the handle of the aio object
 * @param 	odata 	the object data
 *
 */
tb_void_t 		tb_aicp_setp(tb_aicp_t* aicp, tb_handle_t handle, tb_pointer_t odata);

/// post read data
tb_bool_t 		tb_aicp_read(tb_aicp_t* aicp, tb_handle_t handle, tb_byte_t* data, tb_size_t size);

/// post writ data
tb_bool_t 		tb_aicp_writ(tb_aicp_t* aicp, tb_handle_t handle, tb_byte_t* data, tb_size_t size);

/// post sync
tb_bool_t 		tb_aicp_sync(tb_aicp_t* aicp, tb_handle_t handle);

/// post seek
tb_bool_t 		tb_aicp_seek(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t offset);

/// post skip
tb_bool_t 		tb_aicp_skip(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t size);

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
tb_long_t 		tb_aicp_wait(tb_aicp_t* aicp, tb_long_t timeout);

#endif
