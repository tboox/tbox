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
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		array.h
 *
 */
#ifndef TB_CONTAINER_ARRAY_H
#define TB_CONTAINER_ARRAY_H

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
#define TB_ARRAY_PUSH(array, type) 						((type*)tb_array_push((array)))
#define TB_ARRAY_FIRST(array, type) 					((type*)tb_array_first((array)))
#define TB_ARRAY_LAST(array, type) 						((type*)tb_array_last((array)))
#define TB_ARRAY_GET(array, index, type) 				((type*)tb_array_get((array), (index)))
#define TB_ARRAY_PUT(array, index, type) 				((type*)tb_array_put((array), (index)))

#define TB_ARRAY_MAX_SIZE 								(1 << 30)


/* /////////////////////////////////////////////////////////
 * types
 */

// the array type
typedef struct __tb_array_t
{
	tb_byte_t* 		data;
	tb_size_t 		size;
	tb_size_t 		grow;
	tb_size_t 		maxn;
	tb_size_t 		step;

	// free item
	void 			(*free)(void* priv, void* data);
	void* 			priv;

}tb_array_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_array_t* 	tb_array_create(tb_size_t step, tb_size_t size, tb_size_t grow);
void 			tb_array_destroy(tb_array_t* array);
tb_array_t* 	tb_array_duplicate(tb_array_t* array);

tb_byte_t* 		tb_array_put(tb_array_t* array, tb_int_t index);
tb_byte_t* 		tb_array_get(tb_array_t* array, tb_int_t index);

tb_byte_t* 		tb_array_push(tb_array_t* array);
void 			tb_array_pop(tb_array_t* array);
void 			tb_array_clear(tb_array_t* array);

tb_byte_t* 		tb_array_first(tb_array_t* array);
tb_byte_t* 		tb_array_last(tb_array_t* array);

tb_size_t 		tb_array_size(tb_array_t const* array);
tb_bool_t 		tb_array_resize(tb_array_t* array, tb_size_t size);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

