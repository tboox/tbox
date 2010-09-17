/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		pool.h
 *
 */
#ifndef TB_POOL_H
#define TB_POOL_H

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
#define TB_POOL_GET(pool, item, type) 				((type*)tb_pool_get((pool), (item)))
#define TB_POOL_GET_NEXT(pool, item, type) 			(((type*)tb_pool_get((pool), (item)))->next)
#define TB_POOL_GET_PREV(pool, item, type) 			(((type*)tb_pool_get((pool), (item)))->prev)

#define TB_POOL_SET(pool, item, type, value) 		do { type* __p = (type*)tb_pool_get((pool), (item)); TF_ASSERT(__p); if (__p) *__p = (value); } while(0)
#define TB_POOL_SET_NEXT(pool, item, type, value) 	(((type*)tb_pool_get((pool), (item)))->next = (value))
#define TB_POOL_SET_PREV(pool, item, type, value) 	(((type*)tb_pool_get((pool), (item)))->prev = (value))

#define TB_POOL_MAX_SIZE 							(1 << 30)

// trace pool
//#define tb_pool_create(step, size) tb_pool_create_with_trace(step, size, __tplat_func__, __tplat_line__, __tplat_file__)

/* /////////////////////////////////////////////////////////
 * types
 */

// the pool type, valid index > 0, 0: is end for list
typedef struct __tb_pool_t
{
	tb_byte_t* 		data;
	tb_byte_t* 		info;
	tb_size_t 		size;
	tb_size_t 		grow;
	tb_size_t 		maxn;
	tb_size_t 		step;

	// predict the next free block
#ifdef TB_MEMORY_POOL_PREDICTION_ENABLE
	tb_size_t 		pred;
#endif

}tb_pool_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_pool_t* 		tb_pool_create(tb_size_t step, tb_size_t size, tb_size_t grow);
void 			tb_pool_destroy(tb_pool_t* pool);

tb_size_t 		tb_pool_alloc(tb_pool_t* pool);
void 			tb_pool_free(tb_pool_t* pool, tb_size_t item);
tb_byte_t* 		tb_pool_get(tb_pool_t* pool, tb_size_t item);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

