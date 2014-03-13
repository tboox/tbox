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
#ifndef TB_STREAM_GSTREAM_PREFIX_H
#define TB_STREAM_GSTREAM_PREFIX_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../gstream.h"

/* ///////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_gstream_init(tb_gstream_t* gstream, tb_size_t type, tb_size_t cache)
{
	// check
	tb_assert_and_check_return_val(gstream, tb_false);

	// init mode
	gstream->base.mode = TB_STREAM_MODE_AIOO;

	// init type
	gstream->base.type = type;

	// init timeout, 10s
	gstream->base.timeout = 10000;

	// init stoped?
	gstream->base.bstoped = 1;

	// init url
	if (!tb_url_init(&gstream->base.url)) return tb_false;

	// init cache
	if (!tb_qbuffer_init(&gstream->base.cache, cache)) goto fail;

	// ok
	return tb_true;

fail:
	tb_qbuffer_exit(&gstream->base.cache);
	return tb_false;
}

#endif
