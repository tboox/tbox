/*!The Treasure Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
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

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "eobject.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the event ctrl type
typedef enum __tb_ectrl_t
{
 	TB_ECTRL_NULL 	= 0

}tb_ectrl_t;

// the event pool type
typedef struct __tb_epool_t
{
	// the object handles
	tb_vector_t* 	handles;

	// the object hash: handle => object
	tb_hash_t* 		hash;

}tb_epool_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_epool_t* 	tb_epool_init();
tb_void_t 		tb_epool_exit(tb_epool_t* pool);

// ctrl
tb_bool_t 		tb_epool_ctrl(tb_epool_t* pool, tb_size_t ectrl, tb_pointer_t arg0, ...);

/*!wait the event objects in the epool
 *
 * blocking wait the multiple event objects
 * return the event number if ok, otherwise return 0 for timeout
 *
 * @param 	pool 	the event pool
 * @param 	objs 	the returned event objects array
 * @param 	maxn 	the maximum event objects number
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 * @return 	the event number, return 0 if timeout
 */
tb_size_t 		tb_epool_wait(tb_epool_t* pool, tb_eobject_t* objs, tb_size_t maxn, tb_long_t timeout);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
