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
 * \file		spool.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "spool.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the spool item type
typedef struct __tb_spool_item_t
{
	// the string refn
	tb_size_t 			refn;

	// the string data
	tb_char_t const* 	data;

	// the string size
	tb_size_t 			size;

}tb_spool_item_t;

// the chunk type of the string pool
typedef struct __tb_spool_chunk_t
{
	tb_handle_t 	pool;
	tb_byte_t* 		data;
	tb_size_t 		size;

}tb_spool_chunk_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

