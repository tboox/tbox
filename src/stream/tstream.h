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
 * @file		tstream.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_TSTREAM_H
#define TB_STREAM_TSTREAM_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "gstream.h"
#include "astream.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*! the tstream open func type
 *
 * @param istream 	the istream 
 * @param ostream 	the ostream 
 * @param state 	the gstream/astream state
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok, tb_false: break it
 */
typedef tb_bool_t 	(*tb_tstream_open_func_t)(tb_handle_t istream, tb_handle_t ostream, tb_size_t state, tb_pointer_t priv);

/*! the tstream save func type
 *
 * @param istream 	the istream 
 * @param ostream 	the ostream 
 * @param state 	the gstream/astream state
 * @param save 		the saved size
 * @param rate 		the current rate, bytes/s
 * @param priv 		the func private data
 *
 * @return 			tb_true: ok and continue it if need, tb_false: break it
 */
typedef tb_bool_t 	(*tb_tstream_save_func_t)(tb_handle_t istream, tb_handle_t ostream, tb_size_t state, tb_hize_t save, tb_size_t rate, tb_pointer_t priv);

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! save gstream to gstream, block transfer
 *
 * @param istream 	the istream
 * @param ostream 	the ostream
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_tstream_save_gg(tb_gstream_t* istream, tb_gstream_t* ostream, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv);

/*! save gstream to url, block transfer
 *
 * @param istream 	the istream
 * @param ourl 		the output url
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_tstream_save_gu(tb_gstream_t* istream, tb_char_t const* ourl, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv);

/*! save gstream to data, block transfer
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
tb_hong_t 			tb_tstream_save_gd(tb_gstream_t* istream, tb_byte_t* odata, tb_size_t osize, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv);

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
tb_hong_t 			tb_tstream_save_uu(tb_char_t const* iurl, tb_char_t const* ourl, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv);

/*! save url to gstream, block transfer
 *
 * @param iurl 		the input url
 * @param ostream 	the ostream
 * @param lrate 	the limit rate and no limit if 0, bytes/s
 * @param func 		the save func and be optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_hong_t 			tb_tstream_save_ug(tb_char_t const* iurl, tb_gstream_t* ostream, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv);

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
tb_hong_t 			tb_tstream_save_ud(tb_char_t const* iurl, tb_byte_t* odata, tb_size_t osize, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv);

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
tb_hong_t 			tb_tstream_save_du(tb_byte_t const* idata, tb_size_t isize, tb_char_t const* ourl, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv);

/*! save data to gstream, block transfer
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
tb_hong_t 			tb_tstream_save_dg(tb_byte_t const* idata, tb_size_t isize, tb_gstream_t* ostream, tb_size_t lrate, tb_tstream_save_func_t func, tb_pointer_t priv);

/*! init transfer stream from astrean to astream, async transfer
 *
 * @param istream 	the istream
 * @param ostream 	the ostream
 * @param seek 		the seek offset, seek the given offset if offset >= 0
 * @param open 		the open func and be optional
 * @param save 		the save func and be not optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_handle_t 		tb_tstream_init_aa(tb_astream_t* istream, tb_astream_t* ostream, tb_hong_t seek, tb_tstream_open_func_t open, tb_tstream_save_func_t save, tb_pointer_t priv);

/*! init transfer stream from astream to gstream, async transfer
 *
 * @param istream 	the istream
 * @param ostream 	the ostream
 * @param seek 		the seek offset, seek the given offset if offset >= 0
 * @param open 		the open func and be optional
 * @param save 		the save func and be not optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_handle_t 		tb_tstream_init_ag(tb_astream_t* istream, tb_gstream_t* ostream, tb_hong_t seek, tb_tstream_open_func_t open, tb_tstream_save_func_t save, tb_pointer_t priv);

/*! init transfer stream from astream to url, async transfer
 *
 * @param istream 	the istream
 * @param ourl 		the output url
 * @param seek 		the seek offset, seek the given offset if offset >= 0
 * @param open 		the open func and be optional
 * @param save 		the save func and be not optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_handle_t 		tb_tstream_init_au(tb_astream_t* istream, tb_char_t const* ourl, tb_hong_t seek, tb_tstream_open_func_t open, tb_tstream_save_func_t save, tb_pointer_t priv);

/*! init transfer stream from url to url, async transfer
 *
 * @param aicp 		the aicp
 * @param iurl 		the input url
 * @param ourl 		the output url
 * @param seek 		the seek offset, seek the given offset if offset >= 0
 * @param open 		the open func and be optional
 * @param save 		the save func and be not optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_handle_t 		tb_tstream_init_uu(tb_aicp_t* aicp, tb_char_t const* iurl, tb_char_t const* ourl, tb_hong_t seek, tb_tstream_open_func_t open, tb_tstream_save_func_t save, tb_pointer_t priv);

/*! init transfer stream from url to astream, async transfer
 *
 * @param iurl 		the input url
 * @param ostream 	the ostream
 * @param seek 		the seek offset, seek the given offset if offset >= 0
 * @param open 		the open func and be optional
 * @param save 		the save func and be not optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_handle_t 		tb_tstream_init_ua(tb_char_t const* iurl, tb_astream_t* ostream, tb_hong_t seek, tb_tstream_open_func_t open, tb_tstream_save_func_t save, tb_pointer_t priv);

/*! init transfer stream from data to url, async transfer
 *
 * @param aicp 		the aicp
 * @param idata 	the input data
 * @param isize 	the input size
 * @param ourl 		the output url
 * @param seek 		the seek offset, seek the given offset if offset >= 0
 * @param open 		the open func and be optional
 * @param save 		the save func and be not optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_handle_t 		tb_tstream_init_du(tb_aicp_t* aicp, tb_byte_t const* idata, tb_size_t isize, tb_char_t const* ourl, tb_hong_t seek, tb_tstream_open_func_t open, tb_tstream_save_func_t save, tb_pointer_t priv);

/*! init transfer stream from data to astream, async transfer
 *
 * @param aicp 		the aicp
 * @param idata 	the input data
 * @param isize 	the input size
 * @param ostream 	the ostream
 * @param seek 		the seek offset, seek the given offset if offset >= 0
 * @param open 		the open func and be optional
 * @param save 		the save func and be not optional
 * @param priv 		the func private data
 *
 * @return 			the saved size, failed: -1
 */
tb_handle_t 		tb_tstream_init_da(tb_byte_t const* idata, tb_size_t isize, tb_astream_t* ostream, tb_hong_t seek, tb_tstream_open_func_t open, tb_tstream_save_func_t save, tb_pointer_t priv);

/*! open transfer stream, optional 
 *
 * @param tstream 	the tstream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_tstream_open(tb_handle_t tstream);

/*! open and start transfer stream 
 *
 * @param tstream 	the tstream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_tstream_start(tb_handle_t tstream);

/*! pause transfer stream 
 *
 * @param tstream 	the tstream
 */
tb_void_t 			tb_tstream_pause(tb_handle_t tstream);

/*! resume transfer stream 
 *
 * @param tstream 	the tstream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_tstream_resume(tb_handle_t tstream);

/*! limit transfer rate  
 *
 * @param tstream 	the tstream
 * @param rate 		the trasfer rate and no limit if 0, bytes/s
 */
tb_void_t 			tb_tstream_limit(tb_handle_t tstream, tb_size_t rate);

/*! stop transfer stream 
 *
 * @param tstream 	the tstream
 */
tb_void_t 			tb_tstream_stop(tb_handle_t tstream);

/*! exit transfer stream 
 *
 * @param tstream 	the tstream
 * @param bcalling 	exit it at the self callback?
 */
tb_void_t 			tb_tstream_exit(tb_handle_t tstream, tb_bool_t bcalling);

#endif
