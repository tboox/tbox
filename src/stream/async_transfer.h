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

/*! the async transfer done func type
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
typedef tb_bool_t 	(*tb_async_transfer_done_func_t)(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init async transfer
 *
 * @param aicp      the aicp, using the default aicp if be null
 *
 * @return 			the async transfer 
 */
tb_handle_t 		tb_async_transfer_init(tb_aicp_t* aicp);

/*! init istream
 *
 * @param transfer 	the async transfer
 * @param stream    the stream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 		    tb_async_transfer_init_istream(tb_handle_t transfer, tb_async_stream_t* stream);

/*! init istream from url
 *
 * @param transfer 	the async transfer
 * @param url       the url
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 		    tb_async_transfer_init_istream_from_url(tb_handle_t transfer, tb_char_t const* url);

/*! init istream from data
 *
 * @param transfer 	the async transfer
 * @param data      the data
 * @param size      the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 		    tb_async_transfer_init_istream_from_data(tb_handle_t transfer, tb_byte_t* data, tb_size_t size);

/*! init ostream
 *
 * @param transfer 	the async transfer
 * @param stream    the stream
 *
 * @return 			the async transfer 
 */
tb_bool_t 		    tb_async_transfer_init_ostream(tb_handle_t transfer, tb_async_stream_t* stream);

/*! init ostream from url
 *
 * @param transfer 	the async transfer
 * @param url       the url
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 		    tb_async_transfer_init_ostream_from_url(tb_handle_t transfer, tb_char_t const* url);

/*! init ostream from data
 *
 * @param transfer 	the async transfer
 * @param data      the data
 * @param size      the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 		    tb_async_transfer_init_ostream_from_data(tb_handle_t transfer, tb_byte_t* data, tb_size_t size);

/*! ctrl istream
 *
 * @param transfer 	the async transfer
 * @param ctrl 		the ctrl code
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_ctrl_istream(tb_handle_t transfer, tb_size_t ctrl, ...);

/*! ctrl ostream
 *
 * @param transfer 	the async transfer
 * @param ctrl 		the ctrl code
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_ctrl_ostream(tb_handle_t transfer, tb_size_t ctrl, ...);

/*! open transfer
 *
 * @param transfer 	the async transfer
 * @param offset    the start offset
 * @param func 		the open func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_open(tb_handle_t transfer, tb_hize_t offset, tb_async_transfer_open_func_t func, tb_cpointer_t priv);

/*! done transfer and will close it automaticly
 *
 * @param transfer 	the async transfer
 * @param func 		the save func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_done(tb_handle_t transfer, tb_async_transfer_done_func_t func, tb_cpointer_t priv);

/*! open and done transfer and will close it automaticly
 *
 * @param transfer 	the async transfer
 * @param offset    the start offset
 * @param func 		the save func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_async_transfer_open_done(tb_handle_t transfer, tb_hize_t offset, tb_async_transfer_done_func_t func, tb_cpointer_t priv);

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
