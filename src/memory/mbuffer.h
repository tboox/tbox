/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		mbuffer.h
 *
 */
#ifndef TB_MEMORY_MBUFFER_H
#define TB_MEMORY_MBUFFER_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "rbuffer.h"

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

/* the multi-thread reference buffer type
 *
 * mbuffer => rbuffer
 *      mutex
 *     (atomic)
 */
typedef struct __tb_mbuffer_t
{
	// the shared buffer
	tb_rbuffer_t 			rbuf;

	// the shared mutex
	tb_handle_t* 			mutx;

}tb_mbuffer_t;

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_mbuffer_init(tb_mbuffer_t* buffer);
tb_void_t 			tb_mbuffer_exit(tb_mbuffer_t* buffer);

// accessors
tb_byte_t* 			tb_mbuffer_data(tb_mbuffer_t const* buffer);
tb_size_t 			tb_mbuffer_size(tb_mbuffer_t const* buffer);
tb_size_t 			tb_mbuffer_maxn(tb_mbuffer_t const* buffer);
tb_size_t 			tb_mbuffer_refn(tb_mbuffer_t const* buffer);

// modifiors
tb_void_t 			tb_mbuffer_clear(tb_mbuffer_t* buffer);
tb_byte_t* 			tb_mbuffer_resize(tb_mbuffer_t* buffer, tb_size_t n);
tb_size_t 			tb_mbuffer_incr(tb_mbuffer_t* buffer);
tb_size_t 			tb_mbuffer_decr(tb_mbuffer_t* buffer);

// enter & leave
tb_byte_t* 			tb_mbuffer_enter(tb_mbuffer_t const* buffer);
tb_void_t 			tb_mbuffer_leave(tb_mbuffer_t const* buffer);

// memset: b => 0 ... e
tb_byte_t* 			tb_mbuffer_memset(tb_mbuffer_t* buffer, tb_byte_t b);
// memset: b => p ... e
tb_byte_t* 			tb_mbuffer_memsetp(tb_mbuffer_t* buffer, tb_size_t p, tb_byte_t b);
// memset: b => 0 ... n
tb_byte_t* 			tb_mbuffer_memnset(tb_mbuffer_t* buffer, tb_byte_t b, tb_size_t n);
// memset: b => p ... n
tb_byte_t* 			tb_mbuffer_memnsetp(tb_mbuffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n);

// memcpy: b => 0 ... 
tb_byte_t* 			tb_mbuffer_memcpy(tb_mbuffer_t* buffer, tb_mbuffer_t const* b);
// memcpy: b => p ... 
tb_byte_t* 			tb_mbuffer_memcpyp(tb_mbuffer_t* buffer, tb_size_t p, tb_mbuffer_t const* b);
// memcpy: b ... n => 0 ... 
tb_byte_t* 			tb_mbuffer_memncpy(tb_mbuffer_t* buffer, tb_byte_t const* b, tb_size_t n);
// memcpy: b ... n => p ... 
tb_byte_t* 			tb_mbuffer_memncpyp(tb_mbuffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n);

// memmov: b ... e => 0 ... 
tb_byte_t* 			tb_mbuffer_memmov(tb_mbuffer_t* buffer, tb_size_t b);
// memmov: b ... e => p ... 
tb_byte_t* 			tb_mbuffer_memmovp(tb_mbuffer_t* buffer, tb_size_t p, tb_size_t b);
// memmov: b ... n => 0 ... 
tb_byte_t* 			tb_mbuffer_memnmov(tb_mbuffer_t* buffer, tb_size_t b, tb_size_t n);
// memmov: b ... n => p ... 
tb_byte_t* 			tb_mbuffer_memnmovp(tb_mbuffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n);

// memcat: b +=> e ... 
tb_byte_t* 			tb_mbuffer_memcat(tb_mbuffer_t* buffer, tb_mbuffer_t const* b);
// memcat: b ... n +=> e ... 
tb_byte_t* 			tb_mbuffer_memncat(tb_mbuffer_t* buffer, tb_byte_t const* b, tb_size_t n);


#endif

