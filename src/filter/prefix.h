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
 * @file		prefix.h
 *
 */
#ifndef TB_FILTER_PREFIX_H
#define TB_FILTER_PREFIX_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../stream/bstream.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the filter type enum
typedef enum __tb_filter_type_e
{
	TB_FILTER_TYPE_NONE 	= 0
,	TB_FILTER_TYPE_ZIP 		= 1
,	TB_FILTER_TYPE_CHARSET 	= 2
,	TB_FILTER_TYPE_CHUNKED 	= 3

}tb_filter_type_e;

/// the filter type
typedef struct __tb_filter_t
{
	/// the type
	tb_size_t 		type;

	/// the input data
	tb_pbuffer_t 	idata;

	/// the output data 
	tb_qbuffer_t 	odata;

	/// the spak
	tb_long_t 		(*spak)(struct __tb_filter_t* filter, tb_bstream_t* istream, tb_bstream_t* ostream, tb_long_t sync);

	/// the cler
	tb_void_t 		(*cler)(struct __tb_filter_t* filter);

	/// the exit
	tb_void_t 		(*exit)(struct __tb_filter_t* filter);

}tb_filter_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
static __tb_inline__ tb_bool_t tb_filter_init(tb_filter_t* filter, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(filter, tb_false);
	
	// init type
	filter->type = type;

	// init idata
	if (!tb_pbuffer_init(&filter->idata)) return tb_false;

	// init odata
	if (!tb_qbuffer_init(&filter->odata, 8192)) return tb_false;

	// ok
	return tb_true;
}

#endif
