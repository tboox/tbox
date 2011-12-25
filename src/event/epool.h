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
 * \file		epool.h
 *
 */
#ifndef TB_EVENT_POOL_H
#define TB_EVENT_POOL_H


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "eobject.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define tb_epool_object_max 			()
#else
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// the event pool type
typedef struct __tb_epool_t
{
	// the maximum number of concurrent connections
	tb_size_t 		maxc;

	// the object hash: handle => object
	tb_hash_t* 		objs;

}tb_epool_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */


/*!init the event pool
 *
 * @param 	maxn 	the maximum number of concurrent connections
 *
 * @return 	the event pool
 */
tb_epool_t* 	tb_epool_init(tb_size_t maxc);

/// exit the event pool
tb_void_t 		tb_epool_exit(tb_epool_t* pool);

/*!add the event object
 *
 * @param 	pool 	the event pool
 * @param 	handle 	the handle of the event object
 * @param 	otype 	the type of the event object
 * @param 	etype 	the event type
 *
 * @return 	the number of the objects
 */
tb_size_t 		tb_epool_addo(tb_epool_t* pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype);

/*!del the event object
 *
 * @param 	pool 	the event pool
 * @param 	handle 	the handle of the event object
 *
 * @return 	the number of the objects
 */
tb_size_t 		tb_epool_delo(tb_epool_t* pool, tb_handle_t handle);

/*!set the event type
 *
 * @param 	pool 	the event pool
 * @param 	handle 	the handle of the event object
 * @param 	etype 	the event type
 *
 * @return 	the new event type
 */
tb_size_t 		tb_epool_sete(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype);

/*!add the event type
 *
 * add the event type by 'or' before waiting it
 *
 * @param 	pool 	the event pool
 * @param 	handle 	the handle of the event object
 * @param 	etype 	the event type
 *
 * @return 	the new event type
 */
tb_size_t 		tb_epool_adde(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype);

/*!del the event type
 *
 * delete the event type by 'and' before waiting it
 *
 * @param 	pool 	the event pool
 * @param 	handle 	the handle of the event object
 * @param 	etype 	the event type
 *
 * @return 	the new event type
 */
tb_size_t 		tb_epool_dele(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype);

/*!wait the event objects in the epool
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param 	pool 	the event pool
 * @param 	objs 	the returned event objects array
 * @param 	maxn 	the maximum event objects number
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event number, return 0 if timeout
 */
tb_size_t 		tb_epool_wait(tb_epool_t* pool, tb_eobject_t* objs, tb_size_t maxn, tb_long_t timeout);

#endif
