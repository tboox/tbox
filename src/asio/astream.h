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
 * @file		astream.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_STREAM_H
#define TB_ASIO_STREAM_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the aicp file func type
typedef tb_void_t 	(*tb_aicp_astream_func_t)(tb_handle_t hfile, tb_ipv4_t const* file, tb_pointer_t data);

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the astream
 *
 * @param aicp 		the aicp
 *
 * @return 			the astream handle
 */
tb_handle_t 		tb_aicp_astream_init(tb_aicp_t* aicp);

/*! kill the file
 *
 * @param handle 	the astream handle
 */
tb_void_t 			tb_aicp_astream_kill(tb_handle_t handle);

/*! exit the file
 *
 * @param handle 	the astream handle
 */
tb_void_t 			tb_aicp_astream_exit(tb_handle_t handle);

#endif
