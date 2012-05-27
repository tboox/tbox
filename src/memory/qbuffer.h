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
 * @file		qbuffer.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_QBUFFER_H
#define TB_MEMORY_QBUFFER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the queue buffer type
typedef struct __tb_qbuffer_t
{
	// the buffer data
	tb_byte_t* 		data;

	// the buffer head
	tb_byte_t* 		head;

	// the buffer size
	tb_size_t 		size;

	// the buffer maxn
	tb_size_t 		maxn;

}tb_qbuffer_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_qbuffer_init(tb_qbuffer_t* buffer, tb_size_t maxn);
tb_void_t 			tb_qbuffer_exit(tb_qbuffer_t* buffer);

// accessors
tb_byte_t* 			tb_qbuffer_data(tb_qbuffer_t const* buffer);
tb_byte_t* 			tb_qbuffer_head(tb_qbuffer_t const* buffer);
tb_byte_t* 			tb_qbuffer_tail(tb_qbuffer_t const* buffer);
tb_size_t 			tb_qbuffer_maxn(tb_qbuffer_t const* buffer);
tb_size_t 			tb_qbuffer_size(tb_qbuffer_t const* buffer);
tb_size_t 			tb_qbuffer_left(tb_qbuffer_t const* buffer);
tb_bool_t 			tb_qbuffer_full(tb_qbuffer_t const* buffer);
tb_bool_t 			tb_qbuffer_null(tb_qbuffer_t const* buffer);

// modifiors
tb_void_t 			tb_qbuffer_clear(tb_qbuffer_t* buffer);
tb_byte_t* 			tb_qbuffer_resize(tb_qbuffer_t* buffer, tb_size_t maxn);
tb_long_t 			tb_qbuffer_skip(tb_qbuffer_t* buffer, tb_size_t size);

// read & writ
tb_long_t 			tb_qbuffer_read(tb_qbuffer_t* buffer, tb_byte_t* data, tb_size_t size);
tb_long_t 			tb_qbuffer_writ(tb_qbuffer_t* buffer, tb_byte_t* data, tb_size_t size);

// pull
tb_byte_t* 			tb_qbuffer_pull_init(tb_qbuffer_t* buffer, tb_size_t* size);
tb_void_t 			tb_qbuffer_pull_done(tb_qbuffer_t* buffer, tb_size_t size);

// push
tb_byte_t* 			tb_qbuffer_push_init(tb_qbuffer_t* buffer, tb_size_t* size);
tb_void_t 			tb_qbuffer_push_done(tb_qbuffer_t* buffer, tb_size_t size);


#endif

