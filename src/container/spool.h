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
 * \file		spool.h
 *
 */
#ifndef TB_CONTAINER_SPOOL_H
#define TB_CONTAINER_SPOOL_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the string pool type
typedef struct __tb_spool_t
{
	// the chunk list
	tb_slist_t* 				list;

	// the chunk pred
	tb_size_t 					pred;

}tb_spool_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_spool_t* 		tb_spool_init(tb_size_t size);
tb_void_t 			tb_spool_exit(tb_spool_t* spool);

// modifiors
tb_void_t 			tb_spool_clear(tb_spool_t* spool);

tb_char_t const* 	tb_spool_dup(tb_spool_t* spool, tb_char_t const* s);
tb_void_t 			tb_spool_del(tb_spool_t* spool, tb_char_t const* s);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

