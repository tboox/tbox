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
 * @file		epool.h
 * @ingroup 	platform
 *
 */
#ifndef TB_EPOOL_H
#define TB_EPOOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the epool object type
typedef struct __tb_eobject_t
{
	// the event
	tb_handle_t 	evet;

	// the data
	tb_pointer_t 	data;

}tb_eobject_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/// init the event pool
tb_handle_t 	tb_epool_init(tb_size_t maxn);

/// exit the event pool
tb_void_t 		tb_epool_exit(tb_handle_t epool);

/// the object maximum number of the event pool
tb_size_t 		tb_epool_maxn(tb_handle_t epool);

/// the object number of the event pool
tb_size_t 		tb_epool_size(tb_handle_t epool);

/*!add the event object
 *
 * @param 	epool 	the event pool
 * @param 	edata 	the event data
 *
 * @return 	the number of the objects, return 0 if failed
 */
tb_handle_t 	tb_epool_adde(tb_handle_t epool, tb_pointer_t edata);

/*!del the event object
 *
 * @param 	epool 	the event pool
 * @param 	event 	the event object
 *
 */
tb_void_t 		tb_epool_dele(tb_handle_t epool, tb_handle_t event);

/*!post the event 
 *
 * @param 	epool 	the event pool
 * @param 	event 	the event object
 *
 */
tb_void_t 		tb_epool_post(tb_handle_t epool, tb_handle_t event);

/*!post the all events signal
 *
 * @param 	epool 	the event pool
 * @param 	event 	the event object
 *
 */
tb_void_t 		tb_epool_kill(tb_handle_t epool);

/*!wait the event objects in the pool
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param 	epool 	the event pool
 * @param 	objs 	the returned event objects
 * @param 	maxn 	the maximum number of the returned event objects
 * @param 	epool 	the event pool
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event number, return 0 if timeout, return -1 if error
 */
tb_long_t 		tb_epool_wait(tb_handle_t epool, tb_eobject_t* objs, tb_size_t maxn, tb_long_t timeout);


#endif
