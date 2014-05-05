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

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_async_stream_init(tb_async_stream_t* astream, tb_aicp_t* aicp, tb_size_t type, tb_size_t rcache, tb_size_t wcache)
{
	// check
	tb_assert_and_check_return_val(astream && aicp, tb_false);

	// done
	tb_bool_t ok = tb_false;
	tb_bool_t ok_url = tb_false;
	tb_bool_t ok_rcache = tb_false;
	do
	{
		// init
		astream->base.mode 		= TB_STREAM_MODE_AICO;
		astream->base.type 		= type;
		astream->base.timeout 	= -1;
		astream->base.bopened 	= 0;
		astream->base.bstoped 	= 1;
		astream->aicp 			= aicp;

		// init url
		if (!tb_url_init(&astream->base.url)) break;
		ok_url = tb_true;

		// init rcache
		if (!tb_scoped_buffer_init(&astream->rcache_data)) break;
		astream->rcache_maxn = rcache;
		ok_rcache = tb_true;

		// init wcache
		if (!tb_scoped_buffer_init(&astream->wcache_data)) break;
		astream->wcache_maxn = wcache;

		// ok
		ok = tb_true;

	} while (0);

	// failed? 
	if (!ok)
	{
		// exit rcache
		if (ok_rcache) tb_scoped_buffer_exit(&astream->rcache_data);

		// exit url
		if (ok_url) tb_url_exit(&astream->base.url);
	}

	// ok?
	return ok;
}

#endif
