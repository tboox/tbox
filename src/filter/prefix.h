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
 * @file		prefix.h
 *
 */
#ifndef TB_FILTER_PREFIX_H
#define TB_FILTER_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../stream/static_stream.h"
#include "../memory/memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the filter type enum
typedef enum __tb_filter_type_e
{
	TB_FILTER_TYPE_NONE 	= 0
,	TB_FILTER_TYPE_ZIP 		= 1
,	TB_FILTER_TYPE_CACHE 	= 2
,	TB_FILTER_TYPE_CHARSET 	= 3
,	TB_FILTER_TYPE_CHUNKED 	= 4

}tb_filter_type_e;

/// the filter type
typedef struct __tb_filter_t
{
	/// the type
	tb_size_t 			type;

	/// the input is eof?
	tb_bool_t 			beof;

	/// the input limit size 
	tb_hong_t 			limit;
	
	/// the input offset 
	tb_hize_t 			offset;

	/// the input data
	tb_scoped_buffer_t 	idata;

	/// the output data 
	tb_queue_buffer_t 	odata;

	/// the spak
	tb_long_t 			(*spak)(struct __tb_filter_t* filter, tb_static_stream_t* istream, tb_static_stream_t* ostream, tb_long_t sync);

	/// the cler
	tb_void_t 			(*cler)(struct __tb_filter_t* filter);

	/// the exit
	tb_void_t 			(*exit)(struct __tb_filter_t* filter);

}tb_filter_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
static __tb_inline__ tb_bool_t tb_filter_init(tb_filter_t* filter, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(filter, tb_false);
	
	// init type
	filter->type = type;

	// init the input eof
	filter->beof = tb_false;

	// init input limit size
	filter->limit = -1;

	// init the input offset
	filter->offset = 0;

	// init idata
	if (!tb_scoped_buffer_init(&filter->idata)) return tb_false;

	// init odata
	if (!tb_queue_buffer_init(&filter->odata, 8192)) return tb_false;

	// ok
	return tb_true;
}

#endif
