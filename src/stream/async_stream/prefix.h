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
#ifndef TB_STREAM_ASYNC_STREAM_PREFIX_H
#define TB_STREAM_ASYNC_STREAM_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../stream.h"
#include "../async_stream.h"
#include "../../platform/atomic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_void_t tb_async_stream_clear(tb_async_stream_t* stream)
{
    // check
    tb_assert_and_check_return(stream);

	// clear rcache
	tb_scoped_buffer_clear(&stream->rcache_data);

	// clear wcache
	tb_scoped_buffer_clear(&stream->wcache_data);

    // clear opened
	tb_atomic_set0(&stream->base.bopened);

    // clear stoped
	tb_atomic_set(&stream->base.bstoped, 1);
}

#endif
