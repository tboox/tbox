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
#ifndef TB_STREAM_ASTREAM_PREFIX_H
#define TB_STREAM_ASTREAM_PREFIX_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../astream.h"

/* ///////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_astream_init(tb_astream_t* astream, tb_aicp_t* aicp, tb_size_t type, tb_size_t cache)
{
	// check
	tb_assert_and_check_return_val(astream && aicp, tb_false);

	// init
	astream->base.mode 		= TB_STREAM_MODE_AICO;
	astream->base.type 		= type;
	astream->base.timeout 	= -1;
	astream->base.bopened 	= 0;
	astream->base.bstoped 	= 1;
	astream->aicp 			= aicp;

	// init url
	if (!tb_url_init(&astream->base.url)) return tb_false;

	// init cache
	if (!tb_qbuffer_init(&astream->base.cache, cache)) goto fail;

	// ok
	return tb_true;

fail:
	tb_qbuffer_exit(&astream->base.cache);
	return tb_false;
}

#endif
