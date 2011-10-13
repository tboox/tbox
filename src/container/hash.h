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
 * \file		hash.h
 *
 */
#ifndef TB_CONTAINER_HASH_H
#define TB_CONTAINER_HASH_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "vector.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the hash item type
typedef struct __tb_hash_item_t
{
	// the item name
	tb_void_t* 			name;

	// the item hash value
	tb_size_t 			hash;

	// the prev & next item
	tb_size_t 			prev;
	tb_size_t 			next;

}tb_hash_item_t;

// the hash bucket type
typedef struct __tb_hash_bucket_t
{
	// the bucket size
	tb_size_t 			size;

	// the prev & next item
	tb_size_t 			prev;
	tb_size_t 			next;

}tb_hash_bucket_t;

/* the hash type
 *
 * \note the index of the same item is mutable
 */
typedef struct __tb_hash_t
{
	// the pool
	tb_pool_t* 			pool;

	// the bucket data & size
	tb_hash_bucket_t* 	data;
	tb_size_t 			size;

	// the item step
	tb_size_t 			step;

	// free
	tb_void_t 			(*free)(tb_void_t* data, tb_void_t* priv);
	tb_void_t* 			priv;
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_hash_t* 			tb_hash_init(tb_size_t step, tb_size_t size, tb_void_t (*free)(tb_void_t* , tb_void_t* ), tb_void_t* priv);
tb_void_t 			tb_hash_exit(tb_hash_t* hash);

// accessors
tb_byte_t* 			tb_hash_at(tb_hash_t* hash, tb_void_t* name);
tb_byte_t* 			tb_hash_at_head(tb_hash_t* hash);
tb_byte_t* 			tb_hash_at_last(tb_hash_t* hash);

tb_byte_t const* 	tb_hash_const_at(tb_hash_t const* hash, tb_void_t* name);
tb_byte_t const* 	tb_hash_const_at_head(tb_hash_t const* hash);
tb_byte_t const* 	tb_hash_const_at_last(tb_hash_t const* hash);

// modifiors
tb_void_t 			tb_hash_clear(tb_hash_t* hash);

tb_void_t 	 		tb_hash_put(tb_hash_t* hash, tb_byte_t const* item);
tb_void_t 	 		tb_hash_pop(tb_hash_t* hash, tb_byte_t* item);

/* iterator
 * 
 * tb_size_t itor = tb_hash_head(hash);
 * tb_size_t tail = tb_hash_tail(hash);
 * for (; itor != tail; itor = tb_hash_next(hash, itor))
 * {
 * 		tb_byte_t const* item = tb_hash_const_at(hash, itor);
 * 		if (item)
 * 		{
 * 			// ...
 * 		}
 * }
 *
 * tb_size_t itor = 0;
 * tb_size_t size = tb_hash_size(hash);
 * for (itor = 0; itor < size; itor++)
 * {
 * 		// ...
 * }
 *
 * hash: |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||------|
 *       head                                                           last    tail
 *
 * hash: |||||||||||||||||||||||||||||||||||||||||------|
 *       head                                   last    tail
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
 */
tb_size_t 			tb_hash_head(tb_hash_t const* hash);
tb_size_t 			tb_hash_tail(tb_hash_t const* hash);
tb_size_t 			tb_hash_last(tb_hash_t const* hash);
tb_size_t 			tb_hash_size(tb_hash_t const* hash);
tb_size_t 			tb_hash_next(tb_hash_t const* hash, tb_size_t index);
tb_size_t 			tb_hash_prev(tb_hash_t const* hash, tb_size_t index);

// attributes
tb_size_t 			tb_hash_maxn(tb_hash_t const* hash);



// c plus plus
#ifdef __cplusplus
}
#endif

#endif

