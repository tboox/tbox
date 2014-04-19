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
 * @file		transfer_stream.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_TRANSFER_STREAM_H
#define TB_STREAM_TRANSFER_STREAM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "basic_stream.h"
#include "async_stream.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the tstream open func type
 *
 * @param state 	the stream state 
 * @param offset 	the istream offset
 * @param size 		the istream size, no size: -1
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok, tb_false: break it
 */
typedef tb_bool_t 	(*tb_transfer_stream_open_func_t)(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_pointer_t priv);

/*! the tstream save func type
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
typedef tb_bool_t 	(*tb_transfer_stream_save_func_t)(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! save bstream to bstream, block transfer
 *
 * @param istream 	the istream
 * @param ostream 	the ostream
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_transfer_stream_save_gg(tb_basic_stream_t* istream, tb_basic_stream_t* ostream, tb_size_t lrate, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! save bstream to url, block transfer
 *
 * @param istream 	the istream
 * @param ourl 		the output url
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_transfer_stream_save_gu(tb_basic_stream_t* istream, tb_char_t const* ourl, tb_size_t lrate, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! save bstream to data, block transfer
 *
 * @param istream 	the istream
 * @param odata 	the output data
 * @param osize 	the output size
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_transfer_stream_save_gd(tb_basic_stream_t* istream, tb_byte_t* odata, tb_size_t osize, tb_size_t lrate, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! save url to url, block transfer
 *
 * @param iurl 		the input url
 * @param ourl 		the output url
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_transfer_stream_save_uu(tb_char_t const* iurl, tb_char_t const* ourl, tb_size_t lrate, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! save url to bstream, block transfer
 *
 * @param iurl 		the input url
 * @param ostream 	the ostream
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_transfer_stream_save_ug(tb_char_t const* iurl, tb_basic_stream_t* ostream, tb_size_t lrate, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! save url to data, block transfer
 *
 * @param iurl 		the input url
 * @param odata 	the output data
 * @param osize 	the output size
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_transfer_stream_save_ud(tb_char_t const* iurl, tb_byte_t* odata, tb_size_t osize, tb_size_t lrate, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! save data to url, block transfer
 *
 * @param idata 	the input data
 * @param isize 	the input size
 * @param ourl 		the output url
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_transfer_stream_save_du(tb_byte_t const* idata, tb_size_t isize, tb_char_t const* ourl, tb_size_t lrate, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! save data to bstream, block transfer
 *
 * @param idata 	the input data
 * @param isize 	the input size
 * @param ostream 	the ostream
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_transfer_stream_save_dg(tb_byte_t const* idata, tb_size_t isize, tb_basic_stream_t* ostream, tb_size_t lrate, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! init tstream from astream to astream, async transfer
 *
 * @param istream 	the istream
 * @param ostream 	the ostream
 * @param offset 	the offset
 *
 * @return 			the tstream 
 */
tb_handle_t 		tb_transfer_stream_init_aa(tb_async_stream_t* istream, tb_async_stream_t* ostream, tb_hize_t offset);

/*! init tstream from astream to bstream, async transfer
 *
 * @param istream 	the istream
 * @param ostream 	the ostream
 * @param offset 	the offset
 *
 * @return 			the tstream 
 */
tb_handle_t 		tb_transfer_stream_init_ag(tb_async_stream_t* istream, tb_basic_stream_t* ostream, tb_hize_t offset);

/*! init tstream from astream to url, async transfer
 *
 * @param istream 	the istream
 * @param ourl 		the output url
 * @param offset 	the offset
 *
 * @return 			the tstream 
 */
tb_handle_t 		tb_transfer_stream_init_au(tb_async_stream_t* istream, tb_char_t const* ourl, tb_hize_t offset);

/*! init tstream from url to url, async transfer
 *
 * @param aicp 		the aicp
 * @param iurl 		the input url
 * @param ourl 		the output url
 * @param offset 	the offset
 *
 * @return 			the tstream 
 */
tb_handle_t 		tb_transfer_stream_init_uu(tb_aicp_t* aicp, tb_char_t const* iurl, tb_char_t const* ourl, tb_hize_t offset);

/*! init tstream from url to astream, async transfer
 *
 * @param iurl 		the input url
 * @param ostream 	the ostream
 * @param offset 	the offset
 *
 * @return 			the tstream 
 */
tb_handle_t 		tb_transfer_stream_init_ua(tb_char_t const* iurl, tb_async_stream_t* ostream, tb_hize_t offset);

/*! init tstream from data to url, async transfer
 *
 * @param aicp 		the aicp
 * @param idata 	the input data
 * @param isize 	the input size
 * @param ourl 		the output url
 * @param offset 	the offset
 *
 * @return 			the tstream 
 */
tb_handle_t 		tb_transfer_stream_init_du(tb_aicp_t* aicp, tb_byte_t const* idata, tb_size_t isize, tb_char_t const* ourl, tb_hize_t offset);

/*! init tstream from data to astream, async transfer
 *
 * @param idata 	the input data
 * @param isize 	the input size
 * @param ostream 	the ostream
 * @param offset 	the offset
 *
 * @return 			the tstream 
 */
tb_handle_t 		tb_transfer_stream_init_da(tb_byte_t const* idata, tb_size_t isize, tb_async_stream_t* ostream, tb_hize_t offset);

/*! open tstream
 *
 * @param tstream 	the tstream
 * @param func 		the open func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_transfer_stream_open(tb_handle_t tstream, tb_transfer_stream_open_func_t func, tb_pointer_t priv);

/*! save tstream
 *
 * @param tstream 	the tstream
 * @param func 		the save func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_transfer_stream_save(tb_handle_t tstream, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! open and save tstream
 *
 * @param tstream 	the tstream
 * @param func 		the save func 
 * @param priv 		the func private data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_transfer_stream_osave(tb_handle_t tstream, tb_transfer_stream_save_func_t func, tb_pointer_t priv);

/*! kill tstream 
 *
 * @param tstream 	the tstream
 */
tb_void_t 			tb_transfer_stream_kill(tb_handle_t tstream);

/*! exit tstream 
 *
 * @param tstream 	the tstream
 * @param bcalling 	exit it at the self callback?
 */
tb_void_t 			tb_transfer_stream_exit(tb_handle_t tstream, tb_bool_t bcalling);

/*! pause tstream 
 *
 * the save func state will return TB_STATE_PAUSED 
 *
 * @param tstream 	the tstream
 */
tb_void_t 			tb_transfer_stream_pause(tb_handle_t tstream);

/*! resume tstream 
 *
 * @param tstream 	the tstream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_transfer_stream_resume(tb_handle_t tstream);

/*! limit tstream rate  
 *
 * @param tstream 	the tstream
 * @param rate 		the trasfer rate and no limit if 0, bytes/s
 */
tb_void_t 			tb_transfer_stream_limitrate(tb_handle_t tstream, tb_size_t rate);

/*! set tstream timeout 
 *
 * @param tstream 	the tstream
 * @param timeout 	the timeout, using the default timeout if be zero 
 */
tb_void_t 			tb_transfer_stream_timeout_set(tb_handle_t tstream, tb_long_t timeout);

#endif
