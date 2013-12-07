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
 * @file		aicp.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AICP_H
#define TB_ASIO_AICP_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "aice.h"
#include "aico.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/// the aicp post maxn
#ifdef __tb_small__
# 	define TB_AICP_POST_MAXN 				(256)
#else
# 	define TB_AICP_POST_MAXN 				(1024)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

struct __tb_aicp_t;
/// the aico pool proactor type
typedef struct __tb_aicp_proactor_t
{
	/// aicp
	struct __tb_aicp_t* 	aicp;

	/// step
	tb_size_t 				step;

	/// kill
	tb_void_t 				(*kill)(struct __tb_aicp_proactor_t* proactor);

	/// exit
	tb_void_t 				(*exit)(struct __tb_aicp_proactor_t* proactor);

	/// addo
	tb_bool_t 				(*addo)(struct __tb_aicp_proactor_t* proactor, tb_aico_t* aico);

	/// delo
	tb_bool_t 				(*delo)(struct __tb_aicp_proactor_t* proactor, tb_aico_t* aico);

	/// post
	tb_bool_t 				(*post)(struct __tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size);

	/// spak
	tb_long_t 				(*spak)(struct __tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout);

}tb_aicp_proactor_t;

/*! the aico pool type
 *
 * <pre>
 *
 * init:                      ...
 *                             |
 *       |------------------------------------------------|
 * addo: | aico0   aico1   aico2   aico3      ...         |
 *       '------------------------------------------------'
 *                             | 
 *                            ...
 *                             |
 * post: |------------------------------------------------|
 * aice: | aice0   aice1   aice2   aice3      ...         | <-----------------------------
 *       '------------------------------------------------'                               |
 *                             |                                                          |
 *                             |                                                          |
 * queue:      [lock]          |         <= input aices                                   |
 *                             |                                                          |
 *                             |------------------------------------------                |
 *                             |                                  |       |               |
 * aiop: |------|-------|-------|-------|---- ... --|-----|    |-----|    |               |
 * aico: | aico0  aico1   aico2   aico3       ...         |    |  |  |    |               |
 * wait: |------|-------|-------|-------|---- ... --|-----|    |aice4|    |               |
 *       |   |              |                             |    |  |  |    |               |
 *       | aice0           aice2                          |    |aice5|    |               |
 *       |   |              |                             |    |  |  |    |               |
 *       | aice1           ...                            |    |aice6|    |               |
 *       |   |                                            |    |  |  |    |               |
 *       | aice3                                          |    |aice7| <= the other aices queue which is not support for aiop or iocp
 *       |   |                                            |    |  |  | <= spank it first if non-empty and no waiting
 *       |  ...                                           |    | ... |    |               |
 *       '------------------------------------------------'    |     |    |               |
 *                             or                              |     |    |               |
 * iocp: |------------------------------------------------|    |     |    |               |
 *       |                 wait port                      |    |     |    |               |
 *       '------------------------------------------------'    '-----'    |               |
 *                 |           |           |                      |       |               |
 *                 |           |           |                      |       |               |
 *                 |           |           |                      |    --->---            |
 *                 |           |           |                      |   | spank |           |
 *                 |           |           |                      |    ---<---            |
 *                 |           |           |                      |       |               |
 * resp: |---------------------|----------------------------------'-------'               |
 *                             |                                                          |
 *                             |         <= output aices                                  |
 *                             |                                                          |
 *         |---------------------------------------------|                                |
 * worker: |  worker0    |   worker1    |    ...         | <= aico is readonly for worker |
 *         '---------------------------------------------'                                |
 *                |             |               |                                         |
 * aices:  |---------------------------------------------|                                |
 *         |    aice0    |    aice2     |     ...        |                                |
 *         |    aice1    |    aice3     |     ...        |                                |
 *         |     ...     |    aice4     |     ...        |                                |
 *         |     ...     |     ...      |     ...        |                                |
 *         '---------------------------------------------'                                |
 *                |              |              |                                         |  *                             |                                                          |
 * done:   |---------------------------------------------|                                |
 *         |   caller0   |   caller2    |     ...        |                                |
 *         |   caller1   |     ...      |     ...        |                                |
 *         |     ...     |   caller2    |     ...        |                                |
 *         |     ...     |     ...      |     ...        |                                |
 *         '---------------------------------------------'                                |
 *                |              |                                                        | 
 *               ...            ...                                                       |
 *           post evet          end ----                                                  |
 *                |                     |                                                 |
 *                '---------------------|------------------------------------------------>'
 *                                      |
 * kill:               ...              |
 *                      |               |
 * exit:               ...    <---------'
 *
 * </pre>
 *
 */
typedef struct __tb_aicp_t
{
	/// the object maxn
	tb_size_t 				maxn;

	/// is killed?
	tb_size_t 				kill;

	/// the proactor
	tb_aicp_proactor_t* 	ptor;

	/// the worker size
	tb_atomic_t 			work;

	/// the aico pool
	tb_handle_t 			pool;

	/// the pool lock
	tb_handle_t 			lock;

}tb_aicp_t;

/* ///////////////////////////////////////////////////////////////////////
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
 *
 * @return 			the aico
 */
tb_handle_t 		tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type);

/*! del the aico
 *
 * @param aicp 		the aicp
 * @param aico 		the aico
 */
tb_void_t 			tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t aico);

/*! post the aice list
 *
 * @param aicp 		the aicp
 * @param list 		the aice list
 * @param size 		the aice size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_post(tb_aicp_t* aicp, tb_aice_t const* list, tb_size_t size);

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

/*! kill the spak
 *
 * @param aicp 		the aicp
 */
tb_void_t 			tb_aicp_kill(tb_aicp_t* aicp);

#endif
