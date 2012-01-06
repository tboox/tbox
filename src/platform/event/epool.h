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
#ifndef TB_PLATFORM_EVENT_POOL_H
#define TB_PLATFORM_EVENT_POOL_H

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "eobject.h"

/* /////////////////////////////////////////////////////////
 * types
 */
// the event pool reactor type
typedef struct __tb_epool_t;
typedef struct __tb_epool_reactor_t
{
	// the reference to the event pool
	struct __tb_epool_t* 	epool;

	// exit
	tb_void_t 				(*exit)(struct __tb_epool_reactor_t* reactor);

	// addo
	tb_bool_t 				(*addo)(struct __tb_epool_reactor_t* reactor, tb_handle_t handle, tb_size_t etype);

	// seto
	tb_bool_t 				(*seto)(struct __tb_epool_reactor_t* reactor, tb_handle_t handle, tb_size_t etype);

	// delo
	tb_bool_t 				(*delo)(struct __tb_epool_reactor_t* reactor, tb_handle_t handle);

	// wait
	tb_long_t 				(*wait)(struct __tb_epool_reactor_t* reactor, tb_long_t timeout);

	// sync
	tb_void_t 				(*sync)(struct __tb_epool_reactor_t* reactor, tb_size_t evtn);

}tb_epool_reactor_t;

// the event pool type
typedef struct __tb_epool_t
{
	// the object type
	tb_size_t 				type;

	// the object maxn
	tb_size_t 				maxn;

	// the object size
	tb_size_t 				size;

	// the objects
	tb_eobject_t* 			objs;
	tb_size_t 				objn;

	// the event pool reactor
	tb_epool_reactor_t* 	reactor;

}tb_epool_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

/*!init the event pool
 *
 * @param 	type 	the object type
 * @param 	maxn 	the maximum number of concurrent objects
 *
 * @return 	the event pool
 */
tb_epool_t* 	tb_epool_init(tb_size_t type, tb_size_t maxn);

/// exit the event pool
tb_void_t 		tb_epool_exit(tb_epool_t* epool);

/// the object maximum number of the event pool
tb_size_t 		tb_epool_maxn(tb_epool_t* epool);

/// the object number of the event pool
tb_size_t 		tb_epool_size(tb_epool_t* epool);

/*!add the event object
 *
 * @param 	epool 	the event pool
 * @param 	handle 	the handle of the event object
 * @param 	etype 	the event type
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_epool_addo(tb_epool_t* epool, tb_handle_t handle, tb_size_t etype);

/*!set the event object
 *
 * @param 	epool 	the event pool
 * @param 	handle 	the handle of the event object
 * @param 	etype 	the event type
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_epool_seto(tb_epool_t* epool, tb_handle_t handle, tb_size_t etype);

/*!del the event object
 *
 * @param 	epool 	the event pool
 * @param 	handle 	the handle of the event object
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_epool_delo(tb_epool_t* epool, tb_handle_t handle);

/*!wait the event objects in the pool
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param 	epool 	the event pool
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event number, return 0 if timeout, return -1 if error
 */
tb_long_t 		tb_epool_wait(tb_epool_t* epool, tb_long_t timeout);

/// the event objects
tb_eobject_t* 	tb_epool_objs(tb_epool_t* epool);

#endif
