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
 * @file		scoped_buffer.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_SCOPED_BUFFER_H
#define TB_MEMORY_SCOPED_BUFFER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the scoped buffer type
typedef struct __tb_scoped_buffer_t
{
	// the buffer data
	tb_char_t* 		data;

	// the buffer size
	tb_size_t 		size;

	// the buffer maxn
	tb_size_t 		maxn;

}tb_scoped_buffer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_scoped_buffer_init(tb_scoped_buffer_t* buffer);
tb_void_t 			tb_scoped_buffer_exit(tb_scoped_buffer_t* buffer);

// accessors
tb_byte_t* 			tb_scoped_buffer_data(tb_scoped_buffer_t const* buffer);
tb_size_t 			tb_scoped_buffer_size(tb_scoped_buffer_t const* buffer);
tb_size_t 			tb_scoped_buffer_maxn(tb_scoped_buffer_t const* buffer);

// modifiors
tb_void_t 			tb_scoped_buffer_clear(tb_scoped_buffer_t* buffer);
tb_byte_t* 			tb_scoped_buffer_resize(tb_scoped_buffer_t* buffer, tb_size_t n);

// memset: b => 0 ... e
tb_byte_t* 			tb_scoped_buffer_memset(tb_scoped_buffer_t* buffer, tb_byte_t b);
// memset: b => p ... e
tb_byte_t* 			tb_scoped_buffer_memsetp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_byte_t b);
// memset: b => 0 ... n
tb_byte_t* 			tb_scoped_buffer_memnset(tb_scoped_buffer_t* buffer, tb_byte_t b, tb_size_t n);
// memset: b => p ... n
tb_byte_t* 			tb_scoped_buffer_memnsetp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n);

// memcpy: b => 0 ... 
tb_byte_t* 			tb_scoped_buffer_memcpy(tb_scoped_buffer_t* buffer, tb_scoped_buffer_t const* b);
// memcpy: b => p ... 
tb_byte_t* 			tb_scoped_buffer_memcpyp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_scoped_buffer_t const* b);
// memcpy: b ... n => 0 ... 
tb_byte_t* 			tb_scoped_buffer_memncpy(tb_scoped_buffer_t* buffer, tb_byte_t const* b, tb_size_t n);
// memcpy: b ... n => p ... 
tb_byte_t* 			tb_scoped_buffer_memncpyp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n);

// memmov: b ... e => 0 ... 
tb_byte_t* 			tb_scoped_buffer_memmov(tb_scoped_buffer_t* buffer, tb_size_t b);
// memmov: b ... e => p ... 
tb_byte_t* 			tb_scoped_buffer_memmovp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_size_t b);
// memmov: b ... n => 0 ... 
tb_byte_t* 			tb_scoped_buffer_memnmov(tb_scoped_buffer_t* buffer, tb_size_t b, tb_size_t n);
// memmov: b ... n => p ... 
tb_byte_t* 			tb_scoped_buffer_memnmovp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n);

// memcat: b +=> e ... 
tb_byte_t* 			tb_scoped_buffer_memcat(tb_scoped_buffer_t* buffer, tb_scoped_buffer_t const* b);
// memcat: b ... n +=> e ... 
tb_byte_t* 			tb_scoped_buffer_memncat(tb_scoped_buffer_t* buffer, tb_byte_t const* b, tb_size_t n);


#endif

