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
 * \file		gpool.h
 *
 */
#ifndef TB_CONTAINER_GPOOL_H
#define TB_CONTAINER_GPOOL_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#ifndef TB_DEBUG
#define tb_gpool_get(gpool, item) 					((gpool)->data + ((item) - 1) * (gpool)->step)
#endif

// prediction
#define TB_GPOOL_PRED_ENABLE

#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_GPOOL_PRED_MAX 					(128)
#else
# 	define TB_GPOOL_PRED_MAX 					(256)
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// the item func
typedef tb_void_t 	(*tb_gpool_item_free_func_t)(tb_void_t* item, tb_void_t* priv);	

// the gpool item func type
typedef struct __tb_gpool_item_func_t
{
	// the item func
	tb_gpool_item_free_func_t 	free;

	// the priv data
	tb_void_t* 					priv;

}tb_gpool_item_func_t;

// the generic pool type, valid index > 0, 0: is end for list
typedef struct __tb_gpool_t
{
	tb_byte_t* 				data;
	tb_byte_t* 				info;
	tb_size_t 				size;
	tb_size_t 				grow;
	tb_size_t 				maxn;
	tb_size_t 				step;

	// predict the next free block
#ifdef TB_GPOOL_PRED_ENABLE
	tb_size_t 				pred[TB_GPOOL_PRED_MAX];
	tb_size_t 				pred_n;

# 	ifdef TB_DEBUG
	tb_size_t 				pred_failed;
	tb_size_t 				alloc_total;
# 	endif

#endif

	// func
	tb_gpool_item_func_t 	func;

}tb_gpool_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_gpool_t* 	tb_gpool_init(tb_size_t step, tb_size_t size, tb_size_t grow, tb_gpool_item_func_t const* func);
tb_void_t 		tb_gpool_exit(tb_gpool_t* gpool);

// alloc & free
tb_size_t 		tb_gpool_alloc(tb_gpool_t* gpool);
tb_void_t 		tb_gpool_free(tb_gpool_t* gpool, tb_size_t item);

// clear
tb_void_t 		tb_gpool_clear(tb_gpool_t* gpool);

#ifdef TB_DEBUG
tb_byte_t* 		tb_gpool_get(tb_gpool_t* gpool, tb_size_t item);
tb_void_t 		tb_gpool_dump(tb_gpool_t* gpool);
#endif


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

