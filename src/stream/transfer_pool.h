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
 * @file		transfer_pool_pool.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_TRANSFER_POOL_H
#define TB_STREAM_TRANSFER_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "transfer.h"
#include "../asio/asio.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the transfer pool instance
 *
 * @return 				the transfer pool instance
 */
tb_handle_t 			tb_transfer_pool(tb_noarg_t);

/*! init transfer pool
 *
 * @param aicp 			the aicp, will make it if be null
 * @param conc 			the concurrent transfer count, no limit if be zero
 * @param timeout 		the timeout, using the default timeout if be zero 
 *
 * @return 				the transfer pool 
 */
tb_handle_t 			tb_transfer_pool_init(tb_aicp_t* aicp, tb_size_t conc, tb_long_t timeout);

/*! exit transfer pool
 *
 * @param pool 			the transfer pool 
 */
tb_void_t 				tb_transfer_pool_exit(tb_handle_t pool);

/*! kill transfer pool
 *
 * @param pool 			the transfer pool 
 */
tb_void_t 				tb_transfer_pool_kill(tb_handle_t pool);

/*! the transfer pool size
 *
 * @param pool 			the transfer pool 
 */
tb_size_t 				tb_transfer_pool_size(tb_handle_t pool);

/*! done transfer from iurl to ourl
 *
 * @param pool 			the transfer pool 
 * @param iurl 			the input url
 * @param ourl 			the output url
 * @param offset 		the offset
 * @param func 			the save func 
 * @param priv 			the func private data
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_transfer_pool_done(tb_handle_t pool, tb_char_t const* iurl, tb_char_t const* ourl, tb_hize_t offset, tb_transfer_save_func_t func, tb_pointer_t priv);

#endif
