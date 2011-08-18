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
 * \file		memops.c
 *
 */
#ifndef TB_MEMORY_MEMOPS_H
#define TB_MEMORY_MEMOPS_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_void_t 	tb_memcpy(tb_void_t* dst, tb_void_t const* src, tb_size_t size);
tb_void_t 	tb_memmov(tb_void_t* dst, tb_void_t const* src, tb_size_t size);
tb_void_t 	tb_memset(tb_void_t* dst, tb_size_t src, tb_size_t size);

tb_void_t 	tb_memset_u8(tb_byte_t* dst, tb_uint8_t src, tb_size_t size);
tb_void_t 	tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size);
tb_void_t 	tb_memset_u24(tb_byte_t* dst, tb_uint32_t src, tb_size_t size);
tb_void_t 	tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

