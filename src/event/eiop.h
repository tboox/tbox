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
 * \file		eiop.h
 *
 */
#ifndef TB_EVENT_IO_POOL_H
#define TB_EVENT_IO_POOL_H

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "eio.h"

/* /////////////////////////////////////////////////////////
 * types
 */
// the event pool reactor type
typedef struct __tb_eiop_t;
typedef struct __tb_eiop_reactor_t
{
	// the reference to the event pool
	struct __tb_eiop_t* 	eiop;

	// exit
	tb_void_t 				(*exit)(struct __tb_eiop_reactor_t* reactor);

	// addo
	tb_bool_t 				(*addo)(struct __tb_eiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype);

	// seto
	tb_bool_t 				(*seto)(struct __tb_eiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype);

	// delo
	tb_bool_t 				(*delo)(struct __tb_eiop_reactor_t* reactor, tb_handle_t handle);

	// wait
	tb_long_t 				(*wait)(struct __tb_eiop_reactor_t* reactor, tb_long_t timeout);

	// sync
	tb_void_t 				(*sync)(struct __tb_eiop_reactor_t* reactor, tb_size_t evtn);

}tb_eiop_reactor_t;

// the event pool type
typedef struct __tb_eiop_t
{
	// the object type
	tb_size_t 				type;

	// the object maxn
	tb_size_t 				maxn;

	// the object size
	tb_size_t 				size;

	// the objects
	tb_eio_t* 			objs;
	tb_size_t 				objn;

	// the event pool reactor
	tb_eiop_reactor_t* 	reactor;

}tb_eiop_t;

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
tb_eiop_t* 	tb_eiop_init(tb_size_t type, tb_size_t maxn);

/// exit the event pool
tb_void_t 		tb_eiop_exit(tb_eiop_t* eiop);

/// the object maximum number of the event pool
tb_size_t 		tb_eiop_maxn(tb_eiop_t* eiop);

/// the object number of the event pool
tb_size_t 		tb_eiop_size(tb_eiop_t* eiop);

/*!add the event object
 *
 * @param 	eiop 	the event pool
 * @param 	handle 	the handle of the event object
 * @param 	etype 	the event type
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_eiop_addo(tb_eiop_t* eiop, tb_handle_t handle, tb_size_t etype);

/*!set the event object
 *
 * @param 	eiop 	the event pool
 * @param 	handle 	the handle of the event object
 * @param 	etype 	the event type
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_eiop_seto(tb_eiop_t* eiop, tb_handle_t handle, tb_size_t etype);

/*!del the event object
 *
 * @param 	eiop 	the event pool
 * @param 	handle 	the handle of the event object
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_size_t 		tb_eiop_delo(tb_eiop_t* eiop, tb_handle_t handle);

/*!wait the event objects in the pool
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param 	eiop 	the event pool
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event number, return 0 if timeout, return -1 if error
 */
tb_long_t 		tb_eiop_wait(tb_eiop_t* eiop, tb_long_t timeout);

/// the event objects
tb_eio_t* 	tb_eiop_objs(tb_eiop_t* eiop);

#endif
