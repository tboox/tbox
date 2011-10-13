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
 * \file		hash.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "hash.h"
#include "../libc/libc.h"
#include "../utils/utils.h"


/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_hash_t* tb_hash_init(tb_size_t step, tb_size_t grow, tb_void_t (*free)(tb_void_t* , tb_void_t* ), tb_void_t* priv)
{
	return tb_vector_init(step, grow, free, priv);
}

tb_void_t tb_hash_exit(tb_hash_t* hash)
{
	tb_vector_exit(hash);
}
tb_void_t tb_hash_clear(tb_hash_t* hash)
{
	tb_vector_clear(hash);
}
tb_void_t tb_hash_put(tb_hash_t* hash, tb_byte_t const* item)
{
	tb_vector_insert_tail(hash, item);
}
tb_void_t tb_hash_pop(tb_hash_t* hash, tb_byte_t* item)
{
	TB_ASSERT_RETURN(hash);
	if (item) 
	{
		tb_byte_t const* last = tb_vector_const_at_last(hash);
		if (last) tb_memcpy(item, last, hash->step);
	}
	tb_vector_remove_last(hash);
}
tb_byte_t* tb_hash_at(tb_hash_t* hash, tb_size_t index)
{
	return tb_vector_at(hash, index);
}
tb_byte_t* tb_hash_at_head(tb_hash_t* hash)
{
	return tb_vector_at_head(hash);
}
tb_byte_t* tb_hash_at_last(tb_hash_t* hash)
{
	return tb_vector_at_last(hash);
}
tb_byte_t const* tb_hash_const_at(tb_hash_t const* hash, tb_size_t index)
{
	return tb_vector_const_at(hash, index);
}
tb_byte_t const* tb_hash_const_at_head(tb_hash_t const* hash)
{
	return tb_vector_const_at_head(hash);
}
tb_byte_t const* tb_hash_const_at_last(tb_hash_t const* hash)
{
	return tb_vector_const_at_last(hash);
}
tb_size_t tb_hash_head(tb_hash_t const* hash)
{
	return tb_vector_head(hash);
}
tb_size_t tb_hash_last(tb_hash_t const* hash)
{
	return tb_vector_last(hash);
}
tb_size_t tb_hash_tail(tb_hash_t const* hash)
{
	return tb_vector_tail(hash);
}
tb_size_t tb_hash_next(tb_hash_t const* hash, tb_size_t index)
{
	return tb_vector_next(hash, index);
}
tb_size_t tb_hash_prev(tb_hash_t const* hash, tb_size_t index)
{
	return tb_vector_prev(hash, index);
}
tb_size_t tb_hash_size(tb_hash_t const* hash)
{
	return tb_vector_size(hash);
}
tb_size_t tb_hash_maxn(tb_hash_t const* hash)
{
	return tb_vector_maxn(hash);

}

