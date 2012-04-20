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
 * @file		sbuffer.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_SBUFFER_H
#define TB_MEMORY_SBUFFER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the static buffer type
typedef struct __tb_sbuffer_t
{
	// the buffer data
	tb_char_t* 		data;

	// the buffer size
	tb_size_t 		size;

	// the buffer maxn
	tb_size_t 		maxn;

}tb_sbuffer_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_sbuffer_init(tb_sbuffer_t* buffer, tb_byte_t* data, tb_size_t maxn);
tb_void_t 			tb_sbuffer_exit(tb_sbuffer_t* buffer);

// accessors
tb_byte_t* 			tb_sbuffer_data(tb_sbuffer_t const* buffer);
tb_size_t 			tb_sbuffer_size(tb_sbuffer_t const* buffer);
tb_size_t 			tb_sbuffer_maxn(tb_sbuffer_t const* buffer);

// modifiors
tb_void_t 			tb_sbuffer_clear(tb_sbuffer_t* buffer);
tb_byte_t* 			tb_sbuffer_resize(tb_sbuffer_t* buffer, tb_size_t n);

// memset: b => 0 ... e
tb_byte_t* 			tb_sbuffer_memset(tb_sbuffer_t* buffer, tb_byte_t b);
// memset: b => p ... e
tb_byte_t* 			tb_sbuffer_memsetp(tb_sbuffer_t* buffer, tb_size_t p, tb_byte_t b);
// memset: b => 0 ... n
tb_byte_t* 			tb_sbuffer_memnset(tb_sbuffer_t* buffer, tb_byte_t b, tb_size_t n);
// memset: b => p ... n
tb_byte_t* 			tb_sbuffer_memnsetp(tb_sbuffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n);

// memcpy: b => 0 ... 
tb_byte_t* 			tb_sbuffer_memcpy(tb_sbuffer_t* buffer, tb_sbuffer_t const* b);
// memcpy: b => p ... 
tb_byte_t* 			tb_sbuffer_memcpyp(tb_sbuffer_t* buffer, tb_size_t p, tb_sbuffer_t const* b);
// memcpy: b ... n => 0 ... 
tb_byte_t* 			tb_sbuffer_memncpy(tb_sbuffer_t* buffer, tb_byte_t const* b, tb_size_t n);
// memcpy: b ... n => p ... 
tb_byte_t* 			tb_sbuffer_memncpyp(tb_sbuffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n);

// memmov: b ... e => 0 ... 
tb_byte_t* 			tb_sbuffer_memmov(tb_sbuffer_t* buffer, tb_size_t b);
// memmov: b ... e => p ... 
tb_byte_t* 			tb_sbuffer_memmovp(tb_sbuffer_t* buffer, tb_size_t p, tb_size_t b);
// memmov: b ... n => 0 ... 
tb_byte_t* 			tb_sbuffer_memnmov(tb_sbuffer_t* buffer, tb_size_t b, tb_size_t n);
// memmov: b ... n => p ... 
tb_byte_t* 			tb_sbuffer_memnmovp(tb_sbuffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n);

// memcat: b +=> e ... 
tb_byte_t* 			tb_sbuffer_memcat(tb_sbuffer_t* buffer, tb_sbuffer_t const* b);
// memcat: b ... n +=> e ... 
tb_byte_t* 			tb_sbuffer_memncat(tb_sbuffer_t* buffer, tb_byte_t const* b, tb_size_t n);


#endif

