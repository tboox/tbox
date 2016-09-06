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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        prefix.h
 *
 */
#ifndef TB_STREAM_IMPL_FILTER_PREFIX_H
#define TB_STREAM_IMPL_FILTER_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../filter.h"
#include "../../static_stream.h"
#include "../../../memory/memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the filter impl type
typedef struct __tb_stream_filter_impl_t
{
    // the type
    tb_size_t           type;

    // the input is eof?
    tb_bool_t           beof;

    // is opened?
    tb_bool_t           bopened;

    // the input limit size 
    tb_hong_t           limit;
    
    // the input offset 
    tb_hize_t           offset;

    // the input data
    tb_buffer_t         idata;

    // the output data 
    tb_queue_buffer_t   odata;

    // the open
    tb_bool_t           (*open)(struct __tb_stream_filter_impl_t* impl);

    // the clos
    tb_void_t           (*clos)(struct __tb_stream_filter_impl_t* impl);

    // the spak
    tb_long_t           (*spak)(struct __tb_stream_filter_impl_t* impl, tb_static_stream_ref_t istream, tb_static_stream_ref_t ostream, tb_long_t sync);

    // the ctrl
    tb_bool_t           (*ctrl)(struct __tb_stream_filter_impl_t* impl, tb_size_t ctrl, tb_va_list_t args);

    // the exit
    tb_void_t           (*exit)(struct __tb_stream_filter_impl_t* impl);

}tb_stream_filter_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
static __tb_inline__ tb_bool_t tb_stream_filter_impl_init(tb_stream_filter_impl_t* impl, tb_size_t type)
{
    // check
    tb_assert_and_check_return_val(impl, tb_false);
    
    // init type
    impl->type = type;

    // init the input eof
    impl->beof = tb_false;

    // init input limit size
    impl->limit = -1;

    // init the input offset
    impl->offset = 0;

    // init idata
    if (!tb_buffer_init(&impl->idata)) return tb_false;

    // init odata
    if (!tb_queue_buffer_init(&impl->odata, 8192)) return tb_false;

    // ok
    return tb_true;
}

#endif
