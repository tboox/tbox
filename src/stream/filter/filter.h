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
 * @file		filter.h
 * @defgroup 	stream
 *
 */
#ifndef TB_STREAM_FILTER_H
#define TB_STREAM_FILTER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init filter from zip
 *
 * @param algo 		the zip algorithm
 * @param action 	the zip action
 *
 * @return 			the filter
 */
tb_stream_filter_t*	tb_stream_filter_init_from_zip(tb_size_t algo, tb_size_t action);

/*! init filter from charset
 *
 * @param fr 		the from charset
 * @param to 		the to charset
 *
 * @return 			the filter
 */
tb_stream_filter_t*	tb_stream_filter_init_from_charset(tb_size_t fr, tb_size_t to);

/*! init filter from chunked
 *
 * @param dechunked decode the chunked data?
 *
 * @return 			the filter
 */
tb_stream_filter_t*	tb_stream_filter_init_from_chunked(tb_bool_t dechunked);

/*! init filter from cache
 *
 * @param size 		the initial cache size, using the default size if be zero
 *
 * @return 			the filter
 */
tb_stream_filter_t*	tb_stream_filter_init_from_cache(tb_size_t size);

/*! exit filter
 *
 * @param filter 	the filter
 */
tb_void_t 			tb_stream_filter_exit(tb_stream_filter_t* filter);

/*! clear filter
 *
 * @param filter 	the filter
 */
tb_void_t 			tb_stream_filter_cler(tb_stream_filter_t* filter);

/*! is eof for the filter input data, but the output maybe exists the left data and need flush it
 *
 * @param filter 	the filter
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_stream_filter_beof(tb_stream_filter_t* filter);

/*! limit the input size for filter
 *
 * @param filter 	the filter
 * @param limit 	the input limit size
 */
tb_void_t 			tb_stream_filter_limit(tb_stream_filter_t* filter, tb_hong_t limit);

/*! push data to the filter input data
 *
 * @param filter 	the filter
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_stream_filter_push(tb_stream_filter_t* filter, tb_byte_t const* data, tb_size_t size);

/*! spak filter
 *
 * @param filter 	the filter
 * @param data 		the input data, maybe null
 * @param size 		the input size, maybe zero
 * @param pdata 	the output data
 * @param need 		the need output size, using the default size if zero
 * @param sync 		sync? 1: sync, 0: no sync, -1: end
 *
 * @return 			> 0: the output size, 0: continue, -1: end
 */
tb_long_t 			tb_stream_filter_spak(tb_stream_filter_t* filter, tb_byte_t const* data, tb_size_t size, tb_byte_t const** pdata, tb_size_t need, tb_long_t sync);


#endif
