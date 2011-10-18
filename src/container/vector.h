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
 * \file		vector.h
 *
 */
#ifndef TB_CONTAINER_VECTOR_H
#define TB_CONTAINER_VECTOR_H

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

/* the vector type
 *
 * performance: 
 *
 * insert:
 * insert midd: slow
 * insert head: slow
 * insert tail: fast
 *
 * ninsert:
 * ninsert midd: fast
 * ninsert head: fast
 * ninsert tail: fast
 *
 * remove:
 * remove midd: slow
 * remove head: slow
 * remove last: fast
 *
 * nremove:
 * nremove midd: fast
 * nremove head: fast
 * nremove last: fast
 *
 * iterator:
 * next: fast
 * prev: fast
 *
 * \note the index of the same item is mutable
 *
 */
typedef struct __tb_vector_t
{
	// the data
	tb_byte_t* 		data;

	// the info
	tb_size_t 		size;
	tb_size_t 		step;
	tb_size_t 		grow;
	tb_size_t 		maxn;

	// free
	tb_void_t 		(*free)(tb_void_t* data, tb_void_t* priv);
	tb_void_t* 		priv;

}tb_vector_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_vector_t* 		tb_vector_init(tb_size_t step, tb_size_t grow, tb_void_t (*free)(tb_void_t* , tb_void_t* ), tb_void_t* priv);
tb_void_t 			tb_vector_exit(tb_vector_t* vector);

// accessors
tb_byte_t* 			tb_vector_at(tb_vector_t* vector, tb_size_t index);
tb_byte_t* 			tb_vector_at_head(tb_vector_t* vector);
tb_byte_t* 			tb_vector_at_last(tb_vector_t* vector);

tb_byte_t const* 	tb_vector_const_at(tb_vector_t const* vector, tb_size_t index);
tb_byte_t const* 	tb_vector_const_at_head(tb_vector_t const* vector);
tb_byte_t const* 	tb_vector_const_at_last(tb_vector_t const* vector);

// modifiors
tb_bool_t 			tb_vector_resize(tb_vector_t* vector, tb_size_t size);
tb_void_t 			tb_vector_clear(tb_vector_t* vector);

tb_void_t 	 		tb_vector_insert(tb_vector_t* vector, tb_size_t index, tb_byte_t const* item);
tb_void_t 	 		tb_vector_insert_head(tb_vector_t* vector, tb_byte_t const* item);
tb_void_t 	 		tb_vector_insert_tail(tb_vector_t* vector, tb_byte_t const* item);

tb_void_t 	 		tb_vector_ninsert(tb_vector_t* vector, tb_size_t index, tb_byte_t const* item, tb_size_t size);
tb_void_t 	 		tb_vector_ninsert_head(tb_vector_t* vector, tb_byte_t const* item, tb_size_t size);
tb_void_t 	 		tb_vector_ninsert_tail(tb_vector_t* vector, tb_byte_t const* item, tb_size_t size);

tb_void_t 	 		tb_vector_replace(tb_vector_t* vector, tb_size_t index, tb_byte_t const* item);
tb_void_t 			tb_vector_replace_head(tb_vector_t* vector, tb_byte_t const* item);
tb_void_t 	 		tb_vector_replace_last(tb_vector_t* vector, tb_byte_t const* item);

tb_void_t 	 		tb_vector_nreplace(tb_vector_t* vector, tb_size_t index, tb_byte_t const* item, tb_size_t size);
tb_void_t 	 		tb_vector_nreplace_head(tb_vector_t* vector, tb_byte_t const* item, tb_size_t size);
tb_void_t 	 		tb_vector_nreplace_last(tb_vector_t* vector, tb_byte_t const* item, tb_size_t size);

tb_void_t 			tb_vector_remove(tb_vector_t* vector, tb_size_t index);
tb_void_t 	 		tb_vector_remove_head(tb_vector_t* vector);
tb_void_t 	 		tb_vector_remove_last(tb_vector_t* vector);

tb_void_t 	 		tb_vector_nremove(tb_vector_t* vector, tb_size_t index, tb_size_t size);
tb_void_t 	 		tb_vector_nremove_head(tb_vector_t* vector, tb_size_t size);
tb_void_t 	 		tb_vector_nremove_last(tb_vector_t* vector, tb_size_t size);

/* iterator
 * 
 * tb_size_t itor = tb_vector_head(vector);
 * tb_size_t tail = tb_vector_tail(vector);
 * for (; itor != tail; itor = tb_vector_next(vector, itor))
 * {
 * 		tb_byte_t const* item = tb_vector_const_at(vector, itor);
 * 		if (item)
 * 		{
 * 			// ...
 * 		}
 * }
 *
 * tb_size_t itor = 0;
 * tb_size_t size = tb_vector_size(vector);
 * for (itor = 0; itor < size; itor++)
 * {
 * 		// ...
 * }
 *
 * vector: |-----|--------------------------------------------------------|------|
 *       head                                                           last    tail
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
 */
tb_size_t 			tb_vector_head(tb_vector_t const* vector);
tb_size_t 			tb_vector_tail(tb_vector_t const* vector);
tb_size_t 			tb_vector_last(tb_vector_t const* vector);
tb_size_t 			tb_vector_size(tb_vector_t const* vector);
tb_size_t 			tb_vector_next(tb_vector_t const* vector, tb_size_t index);
tb_size_t 			tb_vector_prev(tb_vector_t const* vector, tb_size_t index);

// attributes
tb_size_t 			tb_vector_maxn(tb_vector_t const* vector);



// c plus plus
#ifdef __cplusplus
}
#endif

#endif

