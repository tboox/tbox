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
 * macros
 */
 
// the default stream cache maxn
#define TB_GSTREAM_MCACHE_DEFAULT 					(8192)

// the default stream timeout
#define TB_GSTREAM_TIMEOUT_DEFAULT 					(10000)

/* ///////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_gstream_init(tb_gstream_t* gstream, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(gstream, tb_false);

	// init type
	gstream->type = type;

	// init timeout
	gstream->timeout = TB_GSTREAM_TIMEOUT_DEFAULT;

	// init url
	if (!tb_url_init(&gstream->url)) return tb_false;

	// init cache
	if (!tb_qbuffer_init(&gstream->cache, TB_GSTREAM_MCACHE_DEFAULT)) goto fail;
	gstream->bcached = 1;

	// ok
	return tb_true;

fail:
	tb_qbuffer_exit(&gstream->cache);
	return tb_false;
}

#endif
