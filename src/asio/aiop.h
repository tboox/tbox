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
 * @file		aiop.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AIOP_H
#define TB_ASIO_AIOP_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "aioo.h"
#include "aioe.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aiop reactor type
struct __tb_aiop_t;
typedef struct __tb_aiop_reactor_t
{
	/// aiop
	struct __tb_aiop_t* 	aiop;

	/// exit
	tb_void_t 				(*exit)(struct __tb_aiop_reactor_t* reactor);

	/// cler
	tb_void_t 				(*cler)(struct __tb_aiop_reactor_t* reactor);

	/// addo
	tb_bool_t 				(*addo)(struct __tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t aioe);

	/// seto
	tb_bool_t 				(*seto)(struct __tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t aioe, tb_aioo_t const* aioo);

	/// delo
	tb_bool_t 				(*delo)(struct __tb_aiop_reactor_t* reactor, tb_handle_t handle);

	/// wait
	tb_long_t 				(*wait)(struct __tb_aiop_reactor_t* reactor, tb_aioo_t* aioo, tb_size_t maxn, tb_long_t timeout);

}tb_aiop_reactor_t;

/*! the asio pool pool type
 *
 * @note only for sock
 *
 * <pre>
 * objs: |-----|------|------|--- ... ...---|-------|
 * wait:    |            |
 * evet:   read         writ ...
 * </pre>
 *
 */
typedef struct __tb_aiop_t
{
	/// the object maxn
	tb_size_t 				maxn;

	/// the objects hash
	tb_handle_t 			hash;

	/// the reactor
	tb_aiop_reactor_t* 		rtor;

}tb_aiop_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the aiop
 *
 * @param 	maxn 	the maximum number of concurrent objects
 *
 * @return 	the aiop
 */
tb_aiop_t* 			tb_aiop_init(tb_size_t maxn);

/*! exit the aiop
 *
 * @param 	aiop 	the aiop
 */
tb_void_t 			tb_aiop_exit(tb_aiop_t* aiop);

/*! cler the aiop
 *
 * @param 	aiop 	the aiop
 */
tb_void_t 			tb_aiop_cler(tb_aiop_t* aiop);

/*! add the asio object
 *
 * @param 	aiop 	the aiop
 * @param 	handle 	the handle
 * @param 	aioe 	the aioe
 * @param 	data 	the aioo data
 *
 * @return 	 		tb_true or tb_false
 */
tb_bool_t 			tb_aiop_addo(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t aioe, tb_pointer_t data);

/*! del the asio object
 *
 * @param 	aiop 	the aiop
 * @param 	handle 	the handle
 *
 * @return 	 		tb_true or tb_false
 */
tb_bool_t 			tb_aiop_delo(tb_aiop_t* aiop, tb_handle_t handle);

/*! set the asio event
 *
 * @param 	aiop 	the aiop
 * @param 	handle 	the handle
 * @param 	aioe 	the aioe
 *
 * @return 			the new aioe
 */
tb_size_t 			tb_aiop_gete(tb_aiop_t* aiop, tb_handle_t handle);

/*! set the asio event
 *
 * @param 	aiop 	the aiop
 * @param 	handle 	the handle
 * @param 	aioe 	the aioe
 *
 */
tb_void_t 			tb_aiop_sete(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t aioe);

/*! add the asio event
 *
 * @param 	aiop 	the aiop
 * @param 	handle 	the handle
 * @param 	aioe 	the aioe
 *
 */
tb_void_t 			tb_aiop_adde(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t aioe);

/*! del the asio event
 *
 * @param 	aiop 	the aiop
 * @param 	handle 	the handle
 *
 */
tb_void_t 			tb_aiop_dele(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t aioe);

/*! set the asio data
 *
 * @param 	aiop 	the aiop
 * @param 	handle 	the handle
 * @param 	data 	the aioo data
 *
 */
tb_void_t 			tb_aiop_setp(tb_aiop_t* aiop, tb_handle_t handle, tb_pointer_t data);

/*! get the asio data
 *
 * @param 	aiop 	the aiop
 * @param 	handle 	the handle
 *
 * @return 			the aioo data
 */
tb_pointer_t 		tb_aiop_getp(tb_aiop_t* aiop, tb_handle_t handle);

/*! wait the asio objects in the pool
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param 	aiop 	the aiop
 * @param 	aioo 	the asio objects
 * @param 	maxn 	the maximum size of the asio objects
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event number, return 0 if timeout, return -1 if error
 */
tb_long_t 			tb_aiop_wait(tb_aiop_t* aiop, tb_aioo_t* aioo, tb_size_t maxn, tb_long_t timeout);

#endif
