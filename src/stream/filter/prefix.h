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
 * @author      ruki
 * @file        prefix.h
 *
 */
#ifndef TB_STREAM_FILTER_PREFIX_H
#define TB_STREAM_FILTER_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../static_stream.h"
#include "../../memory/memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the stream filter ctrl
#define TB_STREAM_FILTER_CTRL(type, ctrl)               (((type) << 16) | (ctrl))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the filter type enum
typedef enum __tb_stream_filter_type_e
{
    TB_STREAM_FILTER_TYPE_NONE      = 0
,   TB_STREAM_FILTER_TYPE_ZIP       = 1
,   TB_STREAM_FILTER_TYPE_CACHE     = 2
,   TB_STREAM_FILTER_TYPE_CHARSET   = 3
,   TB_STREAM_FILTER_TYPE_CHUNKED   = 4

}tb_stream_filter_type_e;

/// the filter ctrl enum
typedef enum __tb_stream_filter_ctrl_e
{
    TB_STREAM_FILTER_CTRL_NONE                  = 0
,   TB_STREAM_FILTER_CTRL_ZIP_GET_ALGO          = TB_STREAM_FILTER_CTRL(TB_STREAM_FILTER_TYPE_ZIP, 1)
,   TB_STREAM_FILTER_CTRL_ZIP_GET_ACTION        = TB_STREAM_FILTER_CTRL(TB_STREAM_FILTER_TYPE_ZIP, 2)
,   TB_STREAM_FILTER_CTRL_ZIP_SET_ALGO          = TB_STREAM_FILTER_CTRL(TB_STREAM_FILTER_TYPE_ZIP, 3)
,   TB_STREAM_FILTER_CTRL_ZIP_SET_ACTION        = TB_STREAM_FILTER_CTRL(TB_STREAM_FILTER_TYPE_ZIP, 4)

}tb_stream_filter_ctrl_e;

/// the filter type
typedef struct __tb_stream_filter_t
{
    /// the type
    tb_size_t           type;

    /// the input is eof?
    tb_bool_t           beof;

    /// is opened?
    tb_bool_t           bopened;

    /// the input limit size 
    tb_hong_t           limit;
    
    /// the input offset 
    tb_hize_t           offset;

    /// the input data
    tb_scoped_buffer_t  idata;

    /// the output data 
    tb_queue_buffer_t   odata;

    /// the open
    tb_bool_t           (*open)(struct __tb_stream_filter_t* filter);

    /// the clos
    tb_void_t           (*clos)(struct __tb_stream_filter_t* filter);

    /// the spak
    tb_long_t           (*spak)(struct __tb_stream_filter_t* filter, tb_static_stream_t* istream, tb_static_stream_t* ostream, tb_long_t sync);

    /// the ctrl
    tb_bool_t           (*ctrl)(struct __tb_stream_filter_t* filter, tb_size_t ctrl, tb_va_list_t args);

    /// the exit
    tb_void_t           (*exit)(struct __tb_stream_filter_t* filter);

}tb_stream_filter_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
static __tb_inline__ tb_bool_t tb_stream_filter_init(tb_stream_filter_t* filter, tb_size_t type)
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
