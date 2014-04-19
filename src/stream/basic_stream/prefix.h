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
#ifndef TB_STREAM_BASIC_STREAM_PREFIX_H
#define TB_STREAM_BASIC_STREAM_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../basic_stream.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_basic_stream_init(tb_basic_stream_t* bstream, tb_size_t type, tb_size_t cache)
{
	// check
	tb_assert_and_check_return_val(bstream, tb_false);

	// init mode
	bstream->base.mode = TB_STREAM_MODE_AIOO;

	// init type
	bstream->base.type = type;

	// init timeout, 10s
	bstream->base.timeout = 10000;

	// init stoped?
	bstream->base.bstoped = 1;

	// init url
	if (!tb_url_init(&bstream->base.url)) return tb_false;

	// init cache
	if (!tb_queue_buffer_init(&bstream->cache, cache)) goto fail;

	// ok
	return tb_true;

fail:
	tb_queue_buffer_exit(&bstream->cache);
	return tb_false;
}

#endif
