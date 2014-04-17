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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		aiop.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AIOP_H
#define TB_ASIO_AIOP_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "aioo.h"
#include "aioe.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the aiop reactor type
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
	tb_bool_t 				(*addo)(struct __tb_aiop_reactor_t* reactor, tb_aioo_t const* aioo);

	/// delo
	tb_bool_t 				(*delo)(struct __tb_aiop_reactor_t* reactor, tb_aioo_t const* aioo);

	/// post
	tb_bool_t 				(*post)(struct __tb_aiop_reactor_t* reactor, tb_aioe_t const* aioe);

	/// wait
	tb_long_t 				(*wait)(struct __tb_aiop_reactor_t* reactor, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout);

}tb_aiop_reactor_t;

/*! the asio poll pool type 
 *
 * @note only for sock and using level triggered mode 
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
	/// the aioo maxn
	tb_size_t 				maxn;

	/// the aioo pool
	tb_handle_t 			pool;

	/// the pool lock
	tb_spinlock_t 			lock;

	/// the reactor
	tb_aiop_reactor_t* 		rtor;

	// the spak
	tb_handle_t 			spak[2];
	
}tb_aiop_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the aiop
 *
 * @param maxn 		the maximum number of concurrent objects
 *
 * @return 			the aiop
 */
tb_aiop_t* 			tb_aiop_init(tb_size_t maxn);

/*! exit the aiop
 *
 * @param aiop 		the aiop
 */
tb_void_t 			tb_aiop_exit(tb_aiop_t* aiop);

/*! cler the aiop
 *
 * @param aiop 		the aiop
 */
tb_void_t 			tb_aiop_cler(tb_aiop_t* aiop);

/*! kill the aiop
 *
 * @param aiop 		the aiop
 */
tb_void_t 			tb_aiop_kill(tb_aiop_t* aiop);

/*! spak the aiop, break the wait
 *
 * @param aiop 		the aiop
 */
tb_void_t 			tb_aiop_spak(tb_aiop_t* aiop);

/*! addo the aioo
 *
 * @param aiop 		the aiop
 * @param handle 	the handle
 * @param code 		the code
 * @param data 		the data
 *
 * @return 			the aioo
 */
tb_handle_t 		tb_aiop_addo(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t code, tb_pointer_t data);

/*! delo the aioo
 *
 * @param aiop 		the aiop
 * @param aioo 		the aioo
 *
 */
tb_void_t 			tb_aiop_delo(tb_aiop_t* aiop, tb_handle_t aioo);

/*! post the aioe list
 *
 * @param aiop 		the aiop
 * @param aioe 		the aioe
 *
 * @return 	 		tb_true or tb_false
 */
tb_bool_t 			tb_aiop_post(tb_aiop_t* aiop, tb_aioe_t const* aioe);

/*! set the asie
 *
 * @param aiop 		the aiop
 * @param aioo 		the aioo
 * @param code 		the code
 * @param data 		the data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aiop_sete(tb_aiop_t* aiop, tb_handle_t aioo, tb_size_t code, tb_pointer_t data);

/*! wait the asio objects in the pool
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param aiop 		the aiop
 * @param list 		the aioe list
 * @param maxn 		the aioe maxn
 * @param timeout 	the timeout, infinity: -1
 *
 * @return 			> 0: the aioe list size, 0: timeout, -1: failed
 */
tb_long_t 			tb_aiop_wait(tb_aiop_t* aiop, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout);

#endif
