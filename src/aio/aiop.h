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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		aiop.h
 *
 */
#ifndef TB_AIO_POOL_H
#define TB_AIO_POOL_H

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "aioo.h"
#include "../container/container.h"

/* /////////////////////////////////////////////////////////
 * types
 */
// the aio pool reactor type
typedef struct __tb_aiop_t;
typedef struct __tb_aiop_reactor_t
{
	// the reference to the aio pool
	struct __tb_aiop_t* 	aiop;

	// exit
	tb_void_t 				(*exit)(struct __tb_aiop_reactor_t* reactor);

	// addo
	tb_bool_t 				(*addo)(struct __tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype);

	// seto
	tb_bool_t 				(*seto)(struct __tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype, tb_aioo_t const* obj);

	// delo
	tb_bool_t 				(*delo)(struct __tb_aiop_reactor_t* reactor, tb_handle_t handle);

	// wait
	tb_long_t 				(*wait)(struct __tb_aiop_reactor_t* reactor, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout);

}tb_aiop_reactor_t;

// the aio pool type
typedef struct __tb_aiop_t
{
	// the object type
	tb_size_t 				type;

	// the object maxn
	tb_size_t 				maxn;

	// the objects hash
	tb_hash_t* 				hash;

	// the reactor
	tb_aiop_reactor_t* 		rtor;

}tb_aiop_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

/*!init the aio pool
 *
 * @param 	type 	the object type
 * @param 	maxn 	the maximum number of concurrent objects
 *
 * @return 	the aio pool
 */
tb_aiop_t* 		tb_aiop_init(tb_size_t type, tb_size_t maxn);

/// exit the aio pool
tb_void_t 		tb_aiop_exit(tb_aiop_t* aiop);

/// the object maximum number of the aio pool
tb_size_t 		tb_aiop_maxn(tb_aiop_t* aiop);

/// the object number of the aio pool
tb_size_t 		tb_aiop_size(tb_aiop_t* aiop);

/*!add the aio object
 *
 * @param 	aiop 	the aio pool
 * @param 	handle 	the handle of the aio object
 * @param 	etype 	the event type
 * @param 	odata 	the object data
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_aiop_addo(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype, tb_pointer_t odata);

/*!del the aio object
 *
 * @param 	aiop 	the aio pool
 * @param 	handle 	the handle of the aio object
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_aiop_delo(tb_aiop_t* aiop, tb_handle_t handle);

/*!set the aio event
 *
 * @param 	aiop 	the aio pool
 * @param 	handle 	the handle of the aio object
 * @param 	etype 	the event type
 *
 */
tb_size_t 		tb_aiop_gete(tb_aiop_t* aiop, tb_handle_t handle);

/*!set the aio event
 *
 * @param 	aiop 	the aio pool
 * @param 	handle 	the handle of the aio object
 * @param 	etype 	the event type
 *
 */
tb_void_t 		tb_aiop_sete(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype);

/*!add the aio event
 *
 * @param 	aiop 	the aio pool
 * @param 	handle 	the handle of the aio object
 * @param 	etype 	the event type
 *
 */
tb_void_t 		tb_aiop_adde(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype);

/*!del the aio event
 *
 * @param 	aiop 	the aio pool
 * @param 	handle 	the handle of the aio object
 *
 */
tb_void_t 		tb_aiop_dele(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype);

/*!set the aio odata
 *
 * @param 	aiop 	the aio pool
 * @param 	handle 	the handle of the aio object
 * @param 	odata 	the object data
 *
 */
tb_void_t 		tb_aiop_setp(tb_aiop_t* aiop, tb_handle_t handle, tb_pointer_t odata);

/*!wait the aio objects in the pool
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param 	aiop 	the aio pool
 * @param 	objs 	the aio objects
 * @param 	objm 	the maximum size of the aio objects
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event number, return 0 if timeout, return -1 if error
 */
tb_long_t 		tb_aiop_wait(tb_aiop_t* aiop, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout);

#endif
