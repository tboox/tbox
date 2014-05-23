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
 * @file		async_transfer.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_ASYNC_TRANSFER_H
#define TB_STREAM_ASYNC_TRANSFER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "async_stream.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the async transfer ctrl func type
 *
 * @param istream 	the istream 
 * @param ostream 	the ostream 
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok, tb_false: break it
 */
typedef tb_bool_t 	(*tb_async_transfer_ctrl_func_t)(tb_stream_t* istream, tb_stream_t* ostream, tb_cpointer_t priv);

/*! the async transfer open func type
 *
 * @param state 	the stream state 
 * @param offset 	the istream offset
 * @param size 		the istream size, no size: -1
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok, tb_false: break it
 */
typedef tb_bool_t 	(*tb_async_transfer_open_func_t)(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_cpointer_t priv);

/*! the async transfer save func type
 *
 * @param state 	the stream state 
 * @param offset 	the istream offset
 * @param size 		the istream size, no size: -1
 * @param save 		the saved size
 * @param rate 		the current rate, bytes/s
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok and continue it if need, tb_false: break it
 */
typedef tb_bool_t 	(*tb_async_transfer_save_func_t)(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init transfer from astream to astream, async transfer
 *
 * @param istream 	the istream
 * @param ostream 	the ostream
 * @param offset 	the offset
 *
 * @return 			the async transfer 
 */
tb_handle_t 		tb_async_transfer_init_aa(tb_async_stream_t* istream, tb_async_stream_t* ostream, tb_hize_t offset);

/*! init transfer from astream to url, async transfer
 *
 * @param istream 	the istream
 * @param ourl 		the output url
 * @param offset 	the offset
 *
 * @return 			the async transfer 
 */
tb_handle_t 		tb_async_transfer_init_au(tb_async_stream_t* istream, tb_char_t const* ourl, tb_hize_t offset);

/*! init transfer from url to url, async transfer
 *
 * @param aicp 		the aicp
 * @param iurl 		the input url
 * @param ourl 		the output url
 * @param offset 	the offset
 *
 * @return 			the async transfer 
 */
tb_handle_t 		tb_async_transfer_init_uu(tb_aicp_t* aicp, tb_char_t const* iurl, tb_char_t const* ourl, tb_hize_t offset);

/*! init transfer from url to astream, async transfer
 *
 * @param iurl 		the input url
 * @param ostream 	the ostream
 * @param offset 	the offset
 *
 * @return 			the async transfer 
 */
tb_handle_t 		tb_async_transfer_init_ua(tb_char_t const* iurl, tb_async_stream_t* ostream, tb_hize_t offset);

/*! init transfer from data to url, async transfer
 *
 * @param aicp 		the aicp
 * @param idata 	the input data
 * @param isize 	the input size
 * @param ourl 		the output url
 * @param offset 	the offset
 *
 * @return 			the async transfer 
 */
tb_handle_t 		tb_async_transfer_init_du(tb_aicp_t* aicp, tb_byte_t const* idata, tb_size_t isize, tb_char_t const* ourl, tb_hize_t offset);

/*! init transfer from data to astream, async transfer
 *
 * @param idata 	the input data
 * @param isize 	the input size
 * @param ostream 	the ostream
 * @param offset 	the offset
 *
 * @return 			the async transfer 
 */
tb_handle_t 		tb_async_transfer_init_da(tb_byte_t const* idata, tb_size_t isize, tb_async_stream_t* ostream, tb_hize_t offset);

/*! ctrl transfer, will call the given ctrl func before opening transfer
 *
 * @param transfer 	the async transfer
 * @param func 		the ctrl func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_ctrl(tb_handle_t transfer, tb_async_transfer_ctrl_func_t func, tb_cpointer_t priv);

/*! open transfer
 *
 * @param transfer 	the async transfer
 * @param func 		the open func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_open(tb_handle_t transfer, tb_async_transfer_open_func_t func, tb_cpointer_t priv);

/*! save transfer
 *
 * @param transfer 	the async transfer
 * @param func 		the save func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_save(tb_handle_t transfer, tb_async_transfer_save_func_t func, tb_cpointer_t priv);

/*! open and save transfer
 *
 * @param transfer 	the async transfer
 * @param func 		the save func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_open_save(tb_handle_t transfer, tb_async_transfer_save_func_t func, tb_cpointer_t priv);

/*! kill transfer 
 *
 * @param transfer 	the async transfer
 */
tb_void_t 			tb_async_transfer_kill(tb_handle_t transfer);

/*! exit transfer 
 *
 * @note will wait transfer closed and cannot be called in the callback func
 *
 * @param transfer 	the async transfer
 *
 * @return          tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_exit(tb_handle_t transfer);

/*! pause transfer 
 *
 * the save func state will return TB_STATE_PAUSED 
 *
 * @param transfer 	the async transfer
 */
tb_void_t 			tb_async_transfer_pause(tb_handle_t transfer);

/*! resume transfer 
 *
 * @param transfer 	the async transfer
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_resume(tb_handle_t transfer);

/*! limit transfer rate  
 *
 * @param transfer 	the async transfer
 * @param rate 		the trasfer rate and no limit if 0, bytes/s
 */
tb_void_t 			tb_async_transfer_limitrate(tb_handle_t transfer, tb_size_t rate);

/*! set transfer timeout 
 *
 * @param transfer 	the async transfer
 * @param timeout 	the timeout, using the default timeout if be zero 
 */
tb_void_t 			tb_async_transfer_timeout_set(tb_handle_t transfer, tb_long_t timeout);

#endif
