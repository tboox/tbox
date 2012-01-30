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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		spool.h
 *
 */
#ifndef TB_CONTAINER_SPOOL_H
#define TB_CONTAINER_SPOOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "slist.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_SPOOL_SIZE_MICRO 				(8096)
#define TB_SPOOL_SIZE_SMALL 				(65536)
#define TB_SPOOL_SIZE_LARGE 				(256 * 1024)

#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_SPOOL_SIZE_DEFAULT 			TB_SPOOL_SIZE_SMALL
#else
# 	define TB_SPOOL_SIZE_DEFAULT 			TB_SPOOL_SIZE_LARGE
#endif


/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/* the string or small pool type for reducing memory fragmentation
 * 
 * small data:   |-------|---|-----|    |--|------|----|-|
 * chunk list:   |-----------------| => |----------------| => 0
 * local spool:  |-----------------------------------------|                      |---------------------|
 * global gpool: |----------------------------------------------------------------------------------------------|
 *                                 tb_malloc ...
 */
typedef struct __tb_spool_t
{
	// the chunk list
	tb_slist_t* 		list;

	// the chunk pred
	tb_size_t 			pred;

	// the chunk size
	tb_size_t 			size;

}tb_spool_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_spool_t* 		tb_spool_init(tb_size_t size);
tb_void_t 			tb_spool_exit(tb_spool_t* spool);

// modifiors
tb_void_t 			tb_spool_clear(tb_spool_t* spool);

// malloc
tb_pointer_t 		tb_spool_malloc(tb_spool_t* spool, tb_size_t size);
tb_pointer_t 		tb_spool_calloc(tb_spool_t* spool, tb_size_t item, tb_size_t size);
tb_pointer_t 		tb_spool_realloc(tb_spool_t* spool, tb_pointer_t data, tb_size_t size);
tb_void_t 			tb_spool_free(tb_spool_t* spool, tb_pointer_t data);

// string
tb_char_t* 			tb_spool_strdup(tb_spool_t* spool, tb_char_t const* s);
tb_char_t* 			tb_spool_strndup(tb_spool_t* spool, tb_char_t const* s, tb_size_t n);



#endif

