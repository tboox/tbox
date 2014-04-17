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
 * @file		aioo.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AIOO_H
#define TB_ASIO_AIOO_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the aioo type
typedef struct __tb_aioo_t
{
	// the code
	tb_size_t 				code;

	// the data
	tb_pointer_t 			data;

	// the handle 
	tb_handle_t 			handle;

}tb_aioo_t;


/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the aioo type
 *
 * @param aioo 		the aioo
 *
 * @return 			the aioo data
 */
tb_pointer_t 		tb_aioo_data(tb_handle_t aioo);

/*! the aioo handle
 *
 * @param aioo 		the aioo
 *
 * @return 			the aioo handle
 */
tb_handle_t 		tb_aioo_handle(tb_handle_t aioo);

/*! wait the aioo
 *
 * blocking wait the single event aioo, so need not aiop 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param handle 	the handle 
 * @param code 		the aioe code
 * @param timeout 	the timeout, infinity: -1
 *
 * @return 			> 0: the aioe code, 0: timeout, -1: failed
 */
tb_long_t 			tb_aioo_wait(tb_handle_t handle, tb_size_t code, tb_long_t timeout);


#endif
