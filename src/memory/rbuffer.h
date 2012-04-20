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
 * @file		rbuffer.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_RBUFFER_H
#define TB_MEMORY_RBUFFER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "pbuffer.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the reference buffer data type
typedef struct __tb_rbuffer_data_t
{
	// the buffer refn
	tb_size_t 				refn;

	// the buffer data
	tb_pbuffer_t 			pbuf;

}tb_rbuffer_data_t;

/* the shared buffer type
 *
 * rbuffer => shared pointer => shared data
 *      (no safed)
 */
typedef struct __tb_rbuffer_t
{
	// the shared data
	tb_rbuffer_data_t** 	data;

}tb_rbuffer_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t			tb_rbuffer_init(tb_rbuffer_t* buffer);
tb_void_t 			tb_rbuffer_exit(tb_rbuffer_t* buffer);

// accessors
tb_byte_t* 			tb_rbuffer_data(tb_rbuffer_t const* buffer);
tb_size_t 			tb_rbuffer_size(tb_rbuffer_t const* buffer);
tb_size_t 			tb_rbuffer_maxn(tb_rbuffer_t const* buffer);
tb_size_t 			tb_rbuffer_refn(tb_rbuffer_t const* buffer);

// modifiors
tb_void_t 			tb_rbuffer_clear(tb_rbuffer_t* buffer);
tb_byte_t* 			tb_rbuffer_resize(tb_rbuffer_t* buffer, tb_size_t n);
tb_size_t 			tb_rbuffer_incr(tb_rbuffer_t* buffer);
tb_size_t 			tb_rbuffer_decr(tb_rbuffer_t* buffer);

// memset: b => 0 ... e
tb_byte_t* 			tb_rbuffer_memset(tb_rbuffer_t* buffer, tb_byte_t b);
// memset: b => p ... e
tb_byte_t* 			tb_rbuffer_memsetp(tb_rbuffer_t* buffer, tb_size_t p, tb_byte_t b);
// memset: b => 0 ... n
tb_byte_t* 			tb_rbuffer_memnset(tb_rbuffer_t* buffer, tb_byte_t b, tb_size_t n);
// memset: b => p ... n
tb_byte_t* 			tb_rbuffer_memnsetp(tb_rbuffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n);

// memcpy: b => 0 ... 
tb_byte_t* 			tb_rbuffer_memcpy(tb_rbuffer_t* buffer, tb_rbuffer_t const* b);
// memcpy: b => p ... 
tb_byte_t* 			tb_rbuffer_memcpyp(tb_rbuffer_t* buffer, tb_size_t p, tb_rbuffer_t const* b);
// memcpy: b ... n => 0 ... 
tb_byte_t* 			tb_rbuffer_memncpy(tb_rbuffer_t* buffer, tb_byte_t const* b, tb_size_t n);
// memcpy: b ... n => p ... 
tb_byte_t* 			tb_rbuffer_memncpyp(tb_rbuffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n);

// memmov: b ... e => 0 ... 
tb_byte_t* 			tb_rbuffer_memmov(tb_rbuffer_t* buffer, tb_size_t b);
// memmov: b ... e => p ... 
tb_byte_t* 			tb_rbuffer_memmovp(tb_rbuffer_t* buffer, tb_size_t p, tb_size_t b);
// memmov: b ... n => 0 ... 
tb_byte_t* 			tb_rbuffer_memnmov(tb_rbuffer_t* buffer, tb_size_t b, tb_size_t n);
// memmov: b ... n => p ... 
tb_byte_t* 			tb_rbuffer_memnmovp(tb_rbuffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n);

// memcat: b +=> e ... 
tb_byte_t* 			tb_rbuffer_memcat(tb_rbuffer_t* buffer, tb_rbuffer_t const* b);
// memcat: b ... n +=> e ... 
tb_byte_t* 			tb_rbuffer_memncat(tb_rbuffer_t* buffer, tb_byte_t const* b, tb_size_t n);


#endif

