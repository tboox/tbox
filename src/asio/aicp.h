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
 * @file		aicp.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AICP_H
#define TB_ASIO_AICP_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "aice.h"
#include "aico.h"
#include "../platform/timer.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// post
#define tb_aicp_post(aicp, aice) 				tb_aicp_post_(aicp, aice __tb_debug_vals__)
#define tb_aicp_post_after(aicp, delay, aice) 	tb_aicp_post_after_(aicp, delay, aice __tb_debug_vals__)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

struct __tb_aicp_t;
/// the aico pool proactor type
typedef struct __tb_aicp_proactor_t
{
	/// aicp
	struct __tb_aicp_t* 	aicp;

	/// the aico step
	tb_size_t 				step;

	/// kill
	tb_void_t 				(*kill)(struct __tb_aicp_proactor_t* proactor);

	/// exit
	tb_void_t 				(*exit)(struct __tb_aicp_proactor_t* proactor);

	/// addo
	tb_bool_t 				(*addo)(struct __tb_aicp_proactor_t* proactor, tb_aico_t* aico);

	/// delo
	tb_bool_t 				(*delo)(struct __tb_aicp_proactor_t* proactor, tb_aico_t* aico);
	
	/// kilo
	tb_void_t 				(*kilo)(struct __tb_aicp_proactor_t* proactor, tb_aico_t* aico);
	
	/// post
	tb_bool_t 				(*post)(struct __tb_aicp_proactor_t* proactor, tb_aice_t const* aice);

	/// loop: init
	tb_handle_t 			(*loop_init)(struct __tb_aicp_proactor_t* proactor);

	/// loop: exit
	tb_void_t 				(*loop_exit)(struct __tb_aicp_proactor_t* proactor, tb_handle_t loop);

	/// loop: spak
	tb_long_t 				(*loop_spak)(struct __tb_aicp_proactor_t* proactor, tb_handle_t loop, tb_aice_t* resp, tb_long_t timeout);

}tb_aicp_proactor_t;

/*! the aico pool type
 *
 * <pre>
 *       |------------------------------------------------|
 *       |                   astream                      |
 *       |------------------------------------------------|
 *       |  addr  | http | file | sock |      ..          | 
 *       '------------------------------------------------'
 *                             |
 * init:                    [aicp]
 *                             |
 *       |------------------------------------------------|
 * addo: | aico0   aico1   aico2   aico3      ...         | <= sock, file, and task aico
 *       '------------------------------------------------'
 *                             | 
 *                          [aicp]
 *                             |
 * post: |------------------------------------------------| <= only post one aice for the same aico util the aice is finished
 * aice: | aice0   aice1   aice2   aice3      ...         | <---------------------------------------------------------------------------------
 *       '------------------------------------------------'                                                                                  |
 *                             |                                                                                                             |
 *                          [aicp]                                                                                                           |
 *                             |         <= input aices                                                                                      |
 *                             |                                                                                                             |
 *                             '--------------------------------------------------------------                                               | 
 *                                                                |                          |                                               |
 *       |--------------------------------------------------------------------------------------------------|                                |
 *       |                         unix proactor                  |              |     windows proactor     |                                |
 *       |-----------------------------------------------------------------------|--------------------------|                                |
 *       |                                                        |              |           |              |                                |
 *       |                           continue to spak aice        |              |           |-----         |                                |
 *       |                      ------------------------------->  |              |           |     |        |                                |
 *       |                     |                                 \/    [lock]    |          \/     |        |                                |
 * aiop: |------|-------|-------|-------|---- ... --|-----|    |-----|           |         done  post       |                                |
 * aico: | aico0  aico1   aico2   aico3       ...         |    |  |  |           |          |      |        |                                |
 * wait: |------|-------|-------|-------|---- ... --|-----|    |aice4|           |    |----------------|    |                                |
 *       |   |              |                             |    |  |  |           |    |                |    |                                |
 *       | aice0           aice2                          |    |aice5|           |    |                |    |                                |
 *       |   |              |                             |    |  |  |           |    |                |    |                                |
 *       | aice1           ...                            |    |aice6|           |    |      iocp      |    |                                |
 *       |   |                                            |    |  |  |           |    |                |    |                                |
 *       | aice3                                          |    |aice7|           |    |                |    |                                |
 *       |   |                                            |    |  |  |           |    |                |    |                                |
 *       |  ...                                           |    | ... |           |    |                |    |                                |
 *       |   |                                            |    |  |  |           |    | wait0 wait1 .. |    |                                |
 *       |                                                |    |     |           |     ----------------     |                                |
 *       |                 wait poll                      |    |queue|           |      |         |         |                                |
 *       '------------------------------------------------'    '-----'-----------'--------------------------'                                |
 *                             /\                                 |    [lock]           |         |                                          |
 *                             |                                  |                     |         |                                          |              
 *                             |     no data? wait aice        --------------------------->-----------------                                 |
 *                             |<-----------------------------|    worker0   |   worker1    |    ...        | <= done loop for workers       |
 *                                                             -------------------<-------------------------                                 |
 *                                                                   |             |              |                                          |
 *                                                            |---------------------------------------------|                                |
 *                                                            |    aice0    |    aice2     |     ...        |                                |
 *                                                            |    aice1    |    aice3     |     ...        | <= output aices                |
 *                                                            |     ...     |    aice4     |     ...        |                                |
 *                                                            |     ...     |     ...      |     ...        |                                |
 *                                                            '---------------------------------------------'                                |
 *                                                                   |              |              |                                         |         
 *                                                            |---------------------------------------------|                                |
 *                                                            |   caller0   |   caller2    |     ...        |                                |
 *                                                            |   caller1   |     ...      |     ...        | <= done callers                |
 *                                                            |     ...     |   caller3    |     ...        |                                |
 *                                                            |     ...     |     ...      |     ...        |                                |
 *                                                            '---------------------------------------------'                                |
 *                                                                   |              |                                                        | 
 *                                                                  ...            ...                                                       |
 *                                                              post aice          end ----                                                  |
 *                                                                   |                     |                                                 |
 *                                                                   '---------------------|------------------------------------------------>'
 *                                                                                         |
 * kill:                                                                  ...              |
 *                                                                         |               |
 * exit:                                                                  ...    <---------'
 *
 * </pre>
 *
 */
typedef struct __tb_aicp_t
{
	/// the object maxn
	tb_size_t 				maxn;

	/// is killed?
	tb_atomic_t 			kill;

	/// the proactor
	tb_aicp_proactor_t* 	ptor;

	/// the worker size
	tb_atomic_t 			work;

	/// the pool
	tb_handle_t 			pool;

	/// the pool lock
	tb_spinlock_t 			lock;

}tb_aicp_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the aicp
 *
 * @param maxn 		the aico maxn
 *
 * @return 			the aicp
 */
tb_aicp_t* 			tb_aicp_init(tb_size_t maxn);

/*! exit the aicp
 *
 * @note 			not multi-thread safe
 *
 * @param aicp 		the aicp
 */ 	
tb_void_t 			tb_aicp_exit(tb_aicp_t* aicp);

/*! add the aico
 *
 * @param aicp 		the aicp
 * @param handle 	the handle
 * @param type 		the aico type
 * @param exit 		the exit func
 * @param priv 		the private data for exit func
 *
 * @return 			the aico
 */
tb_handle_t 		tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type);

/*! del the aico
 *
 * @param aicp 		the aicp
 * @param aico 		the aico
 * @param bcalling 	delo it at the self callback?
 */
tb_void_t 			tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t aico, tb_bool_t bcalling);

/*! kil the aico
 *
 * @param aicp 		the aicp
 * @param aico 		the aico
 */
tb_void_t 			tb_aicp_kilo(tb_aicp_t* aicp, tb_handle_t aico);

/*! post the aice 
 *
 * @param aicp 		the aicp
 * @param aice 		the aice 
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_post_(tb_aicp_t* aicp, tb_aice_t const* aice __tb_debug_decl__);

/*! post the aice 
 *
 * @param aicp 		the aicp
 * @param delay 	the delay time, ms
 * @param aice 		the aice 
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_post_after_(tb_aicp_t* aicp, tb_size_t delay, tb_aice_t const* aice __tb_debug_decl__);

/*! loop aicp for the external thread
 *
 * @code
 * tb_pointer_t tb_aicp_worker_thread(tb_pointer_t)
 * {
 * 		tb_aicp_loop(aicp);
 * }
 * @endcode
 *
 * @param aicp 		the aicp
 */
tb_void_t 			tb_aicp_loop(tb_aicp_t* aicp);

/*! loop aicp util ... for the external thread
 *
 * @code
 * tb_bool_t tb_aicp_stop_func(tb_pointer_t)
 * {
 *     if (...) return tb_true;
 *     return tb_false;
 * }
 * tb_pointer_t tb_aicp_worker_thread(tb_pointer_t)
 * {
 * 		tb_aicp_loop_util(aicp, stop_func, tb_null);
 * }
 * @endcode
 *
 * @param aicp 		the aicp
 */
tb_void_t 			tb_aicp_loop_util(tb_aicp_t* aicp, tb_bool_t (*stop)(tb_pointer_t priv), tb_pointer_t priv);

/*! kill the spak 
 *
 * @param aicp 		the aicp
 */
tb_void_t 			tb_aicp_kill(tb_aicp_t* aicp);

/*! the spak time
 *
 * @param aicp 		the aicp
 *
 * @return 			the time
 */
tb_hong_t 			tb_aicp_time(tb_aicp_t* aicp);

#endif
