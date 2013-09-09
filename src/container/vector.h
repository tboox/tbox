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
 * @file		vector.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_VECTOR_H
#define TB_CONTAINER_VECTOR_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*!the vector type
 *
 * <pre>
 * vector: |-----|--------------------------------------------------------|------|
 *       head                                                           last    tail
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
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
 * </pre>
 *
 * @note the itor of the same item is mutable
 *
 */
typedef struct __tb_vector_t
{
	// the itor
	tb_iterator_t 			itor;

	// the data
	tb_byte_t* 				data;
	tb_size_t 				size;
	tb_size_t 				grow;
	tb_size_t 				maxn;

	// the func
	tb_item_func_t 			func;

}tb_vector_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init vector
 *
 * @param grow 		the item grow
 * @param func 		the item func
 *
 * @return 			the vector
 */
tb_vector_t* 		tb_vector_init(tb_size_t grow, tb_item_func_t func);

/*! exist vector
 *
 * @param vector 	the vector
 */
tb_void_t 			tb_vector_exit(tb_vector_t* vector);

/*! the vector data
 *
 * @param vector 	the vector
 *
 * @return 			the vector data
 */
tb_pointer_t 		tb_vector_data(tb_vector_t* vector);

/*! the vector head item
 *
 * @param vector 	the vector
 *
 * @return 			the vector head item
 */
tb_pointer_t 		tb_vector_head(tb_vector_t* vector);

/*! the vector last item
 *
 * @param vector 	the vector
 *
 * @return 			the vector last item
 */
tb_pointer_t 		tb_vector_last(tb_vector_t* vector);

/*! resize the vector
 *
 * @param vector 	the vector
 * @param size 		the vector size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_vector_resize(tb_vector_t* vector, tb_size_t size);

/*! clear the vector
 *
 * @param vector 	the vector
 */
tb_void_t 			tb_vector_clear(tb_vector_t* vector);

/*! copy the vector
 *
 * @param vector 	the vector
 * @param copy 		the copied vector
 */
tb_void_t 			tb_vector_copy(tb_vector_t* vector, tb_vector_t* copy);

/*! insert the vector item
 *
 * @param vector 	the vector
 * @param itor 		the item itor
 * @param data 		the item data
 */
tb_void_t 	 		tb_vector_insert(tb_vector_t* vector, tb_size_t itor, tb_cpointer_t data);
tb_void_t 	 		tb_vector_insert_head(tb_vector_t* vector, tb_cpointer_t data);
tb_void_t 	 		tb_vector_insert_tail(tb_vector_t* vector, tb_cpointer_t data);

tb_void_t 	 		tb_vector_ninsert(tb_vector_t* vector, tb_size_t itor, tb_cpointer_t data, tb_size_t size);
tb_void_t 	 		tb_vector_ninsert_head(tb_vector_t* vector, tb_cpointer_t data, tb_size_t size);
tb_void_t 	 		tb_vector_ninsert_tail(tb_vector_t* vector, tb_cpointer_t data, tb_size_t size);

tb_void_t 	 		tb_vector_replace(tb_vector_t* vector, tb_size_t itor, tb_cpointer_t data);
tb_void_t 			tb_vector_replace_head(tb_vector_t* vector, tb_cpointer_t data);
tb_void_t 	 		tb_vector_replace_last(tb_vector_t* vector, tb_cpointer_t data);

tb_void_t 	 		tb_vector_nreplace(tb_vector_t* vector, tb_size_t itor, tb_cpointer_t data, tb_size_t size);
tb_void_t 	 		tb_vector_nreplace_head(tb_vector_t* vector, tb_cpointer_t data, tb_size_t size);
tb_void_t 	 		tb_vector_nreplace_last(tb_vector_t* vector, tb_cpointer_t data, tb_size_t size);

tb_void_t 			tb_vector_remove(tb_vector_t* vector, tb_size_t itor);
tb_void_t 	 		tb_vector_remove_head(tb_vector_t* vector);
tb_void_t 	 		tb_vector_remove_last(tb_vector_t* vector);

tb_void_t 	 		tb_vector_nremove(tb_vector_t* vector, tb_size_t itor, tb_size_t size);
tb_void_t 	 		tb_vector_nremove_head(tb_vector_t* vector, tb_size_t size);
tb_void_t 	 		tb_vector_nremove_last(tb_vector_t* vector, tb_size_t size);

/*! the vector size
 *
 * @param vector 	the vector
 *
 * @return 			the vector size
 */
tb_size_t 			tb_vector_size(tb_vector_t const* vector);

/*! the vector maxn
 *
 * @param vector 	the vector
 *
 * @return 			the vector size
 */
tb_size_t 			tb_vector_maxn(tb_vector_t const* vector);

/*!walk
 *
 * be faster than the iterator mode, optimizate to remove items for walking
 *
 * @code
 * tb_bool_t tb_vector_item_func(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
 * {
 * 		tb_assert_and_check_return_val(vector && bdel, tb_false);
 *
 * 		// is tail?
 * 		if (!item) ;
 *
 * 		// delete it?
 * 		// *bdel = tb_true;
 *
 * 		// ok
 * 		return tb_true;
 *
 * fail:
 * 		// break
 * 		return tb_false;
 * }
 * @endcode
 *
 */
tb_void_t 			tb_vector_walk(tb_vector_t* vector, tb_bool_t (*func)(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data);



#endif

