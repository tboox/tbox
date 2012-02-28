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

// the aio completion object type
typedef struct __tb_aico_t
{


}tb_aico_t;

// the aio completion pool type
typedef struct __tb_aicp_t
{


}tb_aicp_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*!init the aio iocp
 *
 * @param 	type 	the object type
 * @param 	maxn 	the maximum number of concurrent objects
 *
 * @return 	the aio iocp
 */
tb_aicp_t* 		tb_aicp_init(tb_size_t type, tb_size_t maxn);

/// exit the aio iocp
tb_void_t 		tb_aicp_exit(tb_aicp_t* aicp);

/*!wait the aio objects in the iocp
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param 	aicp 	the aio iocp
 * @param 	objs 	the aio objects
 * @param 	objm 	the maximum size of the aio objects
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event number, return 0 if timeout, return -1 if error
 */
tb_long_t 		tb_aicp_wait(tb_aicp_t* aicp, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout);

#endif
