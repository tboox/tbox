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
 * \file		fpool.h
 *
 */
#ifndef TB_CONTAINER_FPOOL_H
#define TB_CONTAINER_FPOOL_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// prediction
#define TB_FPOOL_PRED_ENABLE

#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_FPOOL_PRED_MAX 					(128)
#else
# 	define TB_FPOOL_PRED_MAX 					(256)
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// the fixed pool type, valid index > 0, 0: is end for list
typedef struct __tb_fpool_t
{
	tb_byte_t* 				data;
	tb_byte_t* 				info;
	tb_size_t 				size;
	tb_size_t 				grow;
	tb_size_t 				maxn;

	// predict the next free block
#ifdef TB_FPOOL_PRED_ENABLE
	tb_size_t 				pred[TB_FPOOL_PRED_MAX];
	tb_size_t 				pred_n;

# 	ifdef TB_DEBUG
	tb_size_t 				pred_failed;
	tb_size_t 				alloc_total;
# 	endif

#endif

	// func
	tb_item_func_t 			func;

}tb_fpool_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_fpool_t* 		tb_fpool_init(tb_size_t size, tb_size_t grow, tb_item_func_t func);
tb_void_t 			tb_fpool_exit(tb_fpool_t* fpool);

// modifiors
tb_void_t 			tb_fpool_clear(tb_fpool_t* fpool);

tb_size_t 			tb_fpool_put(tb_fpool_t* fpool, tb_cpointer_t data);
tb_void_t 			tb_fpool_set(tb_fpool_t* fpool, tb_size_t itor, tb_cpointer_t data);
tb_void_t 			tb_fpool_del(tb_fpool_t* fpool, tb_size_t itor);
tb_void_t 			tb_fpool_clr(tb_fpool_t* fpool, tb_size_t itor);
tb_pointer_t 		tb_fpool_get(tb_fpool_t* fpool, tb_size_t itor);

/* iterator
 * 
 * tb_size_t itor = tb_fpool_itor_head(fpool);
 * tb_size_t tail = tb_fpool_itor_tail(fpool);
 * for (; itor != tail; itor = tb_fpool_itor_next(fpool, itor))
 * {
 * 		tb_cpointer_t item = tb_fpool_itor_const_at(fpool, itor);
 * 		if (item)
 * 		{
 * 			// ...
 * 		}
 * }
 *
 */
tb_pointer_t 		tb_fpool_itor_at(tb_fpool_t* fpool, tb_size_t itor);
tb_cpointer_t 		tb_fpool_itor_const_at(tb_fpool_t const* fpool, tb_size_t itor);

tb_size_t 			tb_fpool_itor_head(tb_fpool_t const* fpool);
tb_size_t 			tb_fpool_itor_tail(tb_fpool_t const* fpool);
tb_size_t 			tb_fpool_itor_next(tb_fpool_t const* fpool, tb_size_t itor);

// attributes
tb_size_t 			tb_fpool_size(tb_fpool_t const* fpool);
tb_size_t 			tb_fpool_maxn(tb_fpool_t const* fpool);
tb_size_t 			tb_fpool_step(tb_fpool_t const* fpool);

// debug
tb_void_t 			tb_fpool_dump(tb_fpool_t* fpool);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

