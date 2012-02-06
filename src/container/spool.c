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
 * \author		ruki
 * \file		spool.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "spool.h"
#include "item.h"
#include "slist.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the chunk type of the string pool
typedef struct __tb_spool_chunk_t
{
	// the memory pool
	tb_handle_t 	pool;

	// the chunk data
	tb_byte_t* 		data;
	tb_size_t 		size;

}tb_spool_chunk_t;

/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static tb_void_t tb_spool_item_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_spool_chunk_t* chunk = item;
	if (chunk)
	{
		if (chunk->pool) tb_gpool_exit(chunk->pool);
		if (chunk->data) tb_free(chunk->data);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_spool_t* tb_spool_init(tb_size_t size)
{
	// the chunk size cannot be too small for the memory pool
	tb_assert_and_check_return_val(size >= TB_GPOOL_SIZE_MIN, TB_NULL);

	// alloc spool
	tb_spool_t* spool = (tb_spool_t*)tb_calloc(1, sizeof(tb_spool_t));
	tb_assert_and_check_return_val(spool, TB_NULL);

	// init chunk list
	spool->list = tb_slist_init(8, tb_item_func_ifm(sizeof(tb_spool_chunk_t), tb_spool_item_free, TB_NULL));
	tb_assert_and_check_goto(spool->list, fail);

	// init chunk size
	spool->size = size;

	return spool;
fail:
	if (spool) tb_spool_exit(spool);
	return TB_NULL;
}
tb_void_t tb_spool_exit(tb_spool_t* spool)
{
	if (spool)
	{
		// clear it
		tb_spool_clear(spool);

		// free list
		if (spool->list) tb_slist_exit(spool->list);

		// free it
		tb_free(spool);
	}

}
tb_void_t tb_spool_clear(tb_spool_t* spool)
{
	tb_assert_and_check_return(spool && spool->list);

	// clear list
	tb_slist_clear(spool->list);

	// reset prediction
	spool->pred = 0;
}
tb_pointer_t tb_spool_malloc(tb_spool_t* spool, tb_size_t size)
{
	tb_assert_and_check_return_val(spool && spool->list, TB_NULL);
	tb_check_return_VAL(size, TB_NULL);

	// allocate it from the predicted pool first
	if (spool->pred)
	{
		tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, spool->pred);
		if (chunk) 
		{
			// check the chunk
			tb_assert_and_check_return_val(chunk->pool && chunk->data && chunk->size, TB_NULL);

			// try allocating it
			tb_pointer_t p = tb_gpool_malloc(chunk->pool, size);

			// return it
			if (p) return p;
		}
	}

	// allocate it from the existing pool
	if (tb_slist_size(spool->list))
	{
		tb_size_t itor = tb_slist_itor_head(spool->list);
		tb_size_t tail = tb_slist_itor_tail(spool->list);
		for (; itor != tail; itor = tb_slist_itor_next(spool->list, itor))
		{
			tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, itor);
			if (chunk) 
			{
				// check the chunk
				tb_assert_and_check_return_val(chunk->pool && chunk->data && chunk->size, TB_NULL);

				// try allocating it
				tb_pointer_t p = tb_gpool_malloc(chunk->pool, size);
				if (p) 
				{
					// update predicted info
					spool->pred = itor;

					// return it
					return p;
				}
			}
		}
	}

	// append a new chunk to the list head for the faster allocation
	tb_size_t itor = tb_slist_insert_head(spool->list, TB_NULL);
	if (itor)
	{
		tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, itor);
		if (chunk) 
		{
			// clear the chunk
			tb_memset(chunk, 0, sizeof(tb_spool_chunk_t));

			// allocate it from the new chunk
			do 
			{
				// alloc chunk data
				chunk->size = spool->size;
				chunk->data = tb_malloc(chunk->size);
				tb_assert_and_check_break(chunk->data);

				// init chunk pool
				chunk->pool = tb_gpool_init(chunk->data, chunk->size);
				tb_assert_and_check_break(chunk->pool);

				// try allocating it
				tb_pointer_t p = tb_gpool_malloc(chunk->pool, size);
				if (p) 
				{
					// update predicted info
					spool->pred = itor;

					// return it
					return p;
				}
				
				tb_assert_message(0, "the chunk size may be too small: %u < %u", chunk->size, size);

			} while (0);

			// remove it if failed
			tb_slist_remove(spool->list, itor);
		}
	}
	tb_assert_message(0, "cannot alloc size: %u", size);
	return TB_NULL;
}
tb_pointer_t tb_spool_calloc(tb_spool_t* spool, tb_size_t item, tb_size_t size)
{
	tb_assert_and_check_return_val(spool, TB_NULL);
	tb_check_return_VAL(item && size, TB_NULL);

	tb_pointer_t p = tb_spool_malloc(spool, item * size);
	if (p) tb_memset(p, 0, item * size);
	return p;
}
tb_pointer_t tb_spool_realloc(tb_spool_t* spool, tb_pointer_t data, tb_size_t size)
{
	tb_assert_and_check_return_val(spool && spool->list, TB_NULL);
	tb_check_return_VAL(data && size, TB_NULL);

	// reallocate it from the predicted pool first
	if (spool->pred)
	{
		tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, spool->pred);
		if (chunk) 
		{
			// check the chunk
			tb_assert_and_check_return_val(chunk->pool && chunk->data && chunk->size, TB_NULL);

			// try allocating it
			tb_pointer_t p = tb_gpool_realloc(chunk->pool, data, size);

			// return it
			if (p) return p;
		}
	}

	// reallocate it from the existing pool
	if (tb_slist_size(spool->list))
	{
		tb_size_t itor = tb_slist_itor_head(spool->list);
		tb_size_t tail = tb_slist_itor_tail(spool->list);
		for (; itor != tail; itor = tb_slist_itor_next(spool->list, itor))
		{
			tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, itor);
			if (chunk) 
			{
				// check the chunk
				tb_assert_and_check_return_val(chunk->pool && chunk->data && chunk->size, TB_NULL);

				// try allocating it
				tb_pointer_t p = tb_gpool_realloc(chunk->pool, data, size);
				if (p) 
				{
					// update predicted info
					spool->pred = itor;

					// return it
					return p;
				}
			}
		}
	}

	// append a new chunk to the list head for the faster allocation
	tb_size_t itor = tb_slist_insert_head(spool->list, TB_NULL);
	if (itor)
	{
		tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, itor);
		if (chunk) 
		{
			// clear the chunk
			tb_memset(chunk, 0, sizeof(tb_spool_chunk_t));

			// reallocate it from the new chunk
			do 
			{
				// alloc chunk data
				chunk->size = spool->size;
				chunk->data = tb_malloc(chunk->size);
				tb_assert_and_check_break(chunk->data);

				// init chunk pool
				chunk->pool = tb_gpool_init(chunk->data, chunk->size);
				tb_assert_and_check_break(chunk->pool);

				// try allocating it
				tb_pointer_t p = tb_gpool_realloc(chunk->pool, data, size);
				if (p) 
				{
					// update predicted info
					spool->pred = itor;

					// return it
					return p;
				}
				
				tb_assert_message(0, "the chunk size may be too small: %u < %u", chunk->size, size);

			} while (0);

			// remove it if failed
			tb_slist_remove(spool->list, itor);
		}
	}
	tb_assert_message(0, "invalid realloc data address:%x %u", data, size);
	return TB_NULL;
}
tb_void_t tb_spool_free(tb_spool_t* spool, tb_pointer_t data)
{
	tb_assert_and_check_return(spool && spool->list);
	tb_check_return(data);

	// free it from the predicted pool first
	if (spool->pred)
	{
		tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, spool->pred);
		if (chunk) 
		{
			// check the chunk
			tb_assert_and_check_return(chunk->pool && chunk->data && chunk->size);

			// try freeing it
			if (tb_gpool_free(chunk->pool, data)) return ;
		}
	}

	// free it from the existing pool
	if (tb_slist_size(spool->list))
	{
		tb_size_t itor = tb_slist_itor_head(spool->list);
		tb_size_t tail = tb_slist_itor_tail(spool->list);
		for (; itor != tail; itor = tb_slist_itor_next(spool->list, itor))
		{
			tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, itor);
			if (chunk) 
			{
				// check the chunk
				tb_assert_and_check_return(chunk->pool && chunk->data && chunk->size);

				// try free it
				if (tb_gpool_free(chunk->pool, data))
				{
					// update predicted info
					spool->pred = itor;

					// return it
					return ;
				}
			}
		}
	}

	tb_assert_message(0, "invalid free data address:%x", data);
}
tb_char_t* tb_spool_strdup(tb_spool_t* spool, tb_char_t const* s)
{
	tb_assert_and_check_return_val(spool && s, TB_NULL);

	__tb_register__ tb_size_t 	n = tb_strlen(s);
	__tb_register__ tb_char_t* 	p = tb_spool_malloc(spool, n + 1);
	if (p)
	{
		tb_memcpy(p, s, n);
		p[n] = '\0';
	}

	return p;
}
tb_char_t* tb_spool_strndup(tb_spool_t* spool, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(spool && s, TB_NULL);

	__tb_register__ tb_char_t* p;

	n = tb_strnlen(s, n);
	p = tb_spool_malloc(spool, n + 1);
	if (p)
	{
		tb_memcpy(p, s, n);
		p[n] = '\0';
	}

	return p;
}


