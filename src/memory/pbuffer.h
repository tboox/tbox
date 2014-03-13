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
 * @file		pbuffer.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_PBUFFER_H
#define TB_MEMORY_PBUFFER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the scoped buffer type
typedef struct __tb_pbuffer_t
{
	// the buffer data
	tb_char_t* 		data;

	// the buffer size
	tb_size_t 		size;

	// the buffer maxn
	tb_size_t 		maxn;

}tb_pbuffer_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_pbuffer_init(tb_pbuffer_t* buffer);
tb_void_t 			tb_pbuffer_exit(tb_pbuffer_t* buffer);

// accessors
tb_byte_t* 			tb_pbuffer_data(tb_pbuffer_t const* buffer);
tb_size_t 			tb_pbuffer_size(tb_pbuffer_t const* buffer);
tb_size_t 			tb_pbuffer_maxn(tb_pbuffer_t const* buffer);

// modifiors
tb_void_t 			tb_pbuffer_clear(tb_pbuffer_t* buffer);
tb_byte_t* 			tb_pbuffer_resize(tb_pbuffer_t* buffer, tb_size_t n);

// memset: b => 0 ... e
tb_byte_t* 			tb_pbuffer_memset(tb_pbuffer_t* buffer, tb_byte_t b);
// memset: b => p ... e
tb_byte_t* 			tb_pbuffer_memsetp(tb_pbuffer_t* buffer, tb_size_t p, tb_byte_t b);
// memset: b => 0 ... n
tb_byte_t* 			tb_pbuffer_memnset(tb_pbuffer_t* buffer, tb_byte_t b, tb_size_t n);
// memset: b => p ... n
tb_byte_t* 			tb_pbuffer_memnsetp(tb_pbuffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n);

// memcpy: b => 0 ... 
tb_byte_t* 			tb_pbuffer_memcpy(tb_pbuffer_t* buffer, tb_pbuffer_t const* b);
// memcpy: b => p ... 
tb_byte_t* 			tb_pbuffer_memcpyp(tb_pbuffer_t* buffer, tb_size_t p, tb_pbuffer_t const* b);
// memcpy: b ... n => 0 ... 
tb_byte_t* 			tb_pbuffer_memncpy(tb_pbuffer_t* buffer, tb_byte_t const* b, tb_size_t n);
// memcpy: b ... n => p ... 
tb_byte_t* 			tb_pbuffer_memncpyp(tb_pbuffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n);

// memmov: b ... e => 0 ... 
tb_byte_t* 			tb_pbuffer_memmov(tb_pbuffer_t* buffer, tb_size_t b);
// memmov: b ... e => p ... 
tb_byte_t* 			tb_pbuffer_memmovp(tb_pbuffer_t* buffer, tb_size_t p, tb_size_t b);
// memmov: b ... n => 0 ... 
tb_byte_t* 			tb_pbuffer_memnmov(tb_pbuffer_t* buffer, tb_size_t b, tb_size_t n);
// memmov: b ... n => p ... 
tb_byte_t* 			tb_pbuffer_memnmovp(tb_pbuffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n);

// memcat: b +=> e ... 
tb_byte_t* 			tb_pbuffer_memcat(tb_pbuffer_t* buffer, tb_pbuffer_t const* b);
// memcat: b ... n +=> e ... 
tb_byte_t* 			tb_pbuffer_memncat(tb_pbuffer_t* buffer, tb_byte_t const* b, tb_size_t n);


#endif

