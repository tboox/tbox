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
 * @file		aioo.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AIOO_H
#define TB_ASIO_AIOO_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the asio type, only for sock
typedef struct __tb_aioo_t
{
	// the aioe
	tb_size_t 		aioe;

	// the data
	tb_pointer_t 	data;

	// the handle
	tb_handle_t 	handle;

}tb_aioo_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! set the aioo
 *
 * @param aioo 		the aioo
 * @param handle 	the aioo native handle
 * @param aioe 		the aioe
 * @param data 		the aioo data
 */
tb_void_t 			tb_aioo_seto(tb_aioo_t* aioo, tb_handle_t handle, tb_size_t aioe, tb_pointer_t data);

/*! get the aioe 
 *
 * @param aioo 		the aioo
 *
 * @return 			the aioe
 */
tb_size_t 			tb_aioo_gete(tb_aioo_t* aioo);

/*! set the aioe
 *
 * @param aioo 		the aioo
 * @param aioe 		the aioe
 */
tb_void_t 			tb_aioo_sete(tb_aioo_t* aioo, tb_size_t aioe);

/*! add the aioe
 *
 * add the event type by 'or' before waiting it
 *
 * @param aioo 		the aioo
 * @param type 		the aioe
 *
 * @return 			the new aioe
 */
tb_size_t 			tb_aioo_adde(tb_aioo_t* aioo, tb_size_t aioe);

/*! del the aioe
 *
 * del the event type by 'and' before waiting it
 *
 * @param aioo 		the aioo
 * @param type 		the aioe
 *
 * @return 			the new aioe
 */
tb_size_t 			tb_aioo_dele(tb_aioo_t* aioo, tb_size_t aioe);

/*! wait the asio aioo
 *
 * blocking wait the single event aioo, so need not aiop 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param aioo 		the aioo
 * @param timeout 	the timeout value, return immediately if 0, infinity if -1
 *
 * @return 			the aioe or timeout: 0 or error: -1
 */
tb_long_t 			tb_aioo_wait(tb_aioo_t* aioo, tb_long_t timeout);


#endif
