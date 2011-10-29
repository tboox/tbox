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
 * \file		spool.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "spool.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
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

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_void_t tb_spool_item_free(tb_void_t* item, tb_void_t* priv)
{
	tb_spool_chunk_t* chunk = item;
	if (chunk)
	{
		if (chunk->pool) tb_mpool_exit(chunk->pool);
		if (chunk->data) tb_free(chunk->data);
	}
}

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_spool_t* tb_spool_init(tb_size_t size)
{
	// the chunk size cannot be too small for the memory pool
	TB_ASSERT_RETURN_VAL(size >= TB_MPOOL_SIZE_MIN, TB_NULL);

	// alloc spool
	tb_spool_t* spool = (tb_spool_t*)tb_calloc(1, sizeof(tb_spool_t));
	TB_ASSERT_RETURN_VAL(spool, TB_NULL);

	// init chunk list
	tb_slist_item_func_t func = {tb_spool_item_free, TB_NULL};
	spool->list = tb_slist_init(sizeof(tb_spool_chunk_t), 8, &func);
	TB_ASSERT_GOTO(spool->list, fail);

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
	TB_ASSERT_RETURN(spool && spool->list);

	// clear list
	tb_slist_clear(spool->list);

	// reset prediction
	spool->pred = 0;
}
tb_void_t* tb_spool_malloc(tb_spool_t* spool, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(spool && spool->list, TB_NULL);
	TB_IF_FAIL_RETURN_VAL(size, TB_NULL);

	// allocate it from the predicted pool first
	if (spool->pred)
	{
		tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, spool->pred);
		if (chunk) 
		{
			// check the chunk
			TB_ASSERT_RETURN_VAL(chunk->pool && chunk->data && chunk->size, TB_NULL);

			// try allocating it
			tb_void_t* p = tb_mpool_malloc(chunk->pool, size);

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
				TB_ASSERT_RETURN_VAL(chunk->pool && chunk->data && chunk->size, TB_NULL);

				// try allocating it
				tb_void_t* p = tb_mpool_malloc(chunk->pool, size);
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
				TB_ASSERT_BREAK(chunk->data);

				// init chunk pool
				chunk->pool = tb_mpool_init(chunk->data, chunk->size);
				TB_ASSERT_BREAK(chunk->pool);

				// try allocating it
				tb_void_t* p = tb_mpool_malloc(chunk->pool, size);
				if (p) 
				{
					// update predicted info
					spool->pred = itor;

					// return it
					return p;
				}
				
				TB_ASSERTM(0, "the chunk size may be too small: %u < %u", chunk->size, size);

			} while (0);

			// remove it if failed
			tb_slist_remove(spool->list, itor);
		}
	}
	TB_ASSERTM(0, "cannot alloc size: %u", size);
	return TB_NULL;
}
tb_void_t* tb_spool_calloc(tb_spool_t* spool, tb_size_t item, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(spool, TB_NULL);
	TB_IF_FAIL_RETURN_VAL(item && size, TB_NULL);

	tb_void_t* p = tb_spool_malloc(spool, item * size);
	if (p) tb_memset(p, 0, item * size);
	return p;
}
tb_void_t* tb_spool_realloc(tb_spool_t* spool, tb_void_t* data, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(spool && spool->list, TB_NULL);
	TB_IF_FAIL_RETURN_VAL(data && size, TB_NULL);

	// reallocate it from the predicted pool first
	if (spool->pred)
	{
		tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, spool->pred);
		if (chunk) 
		{
			// check the chunk
			TB_ASSERT_RETURN_VAL(chunk->pool && chunk->data && chunk->size, TB_NULL);

			// try allocating it
			tb_void_t* p = tb_mpool_realloc(chunk->pool, data, size);

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
				TB_ASSERT_RETURN_VAL(chunk->pool && chunk->data && chunk->size, TB_NULL);

				// try allocating it
				tb_void_t* p = tb_mpool_realloc(chunk->pool, data, size);
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
				TB_ASSERT_BREAK(chunk->data);

				// init chunk pool
				chunk->pool = tb_mpool_init(chunk->data, chunk->size);
				TB_ASSERT_BREAK(chunk->pool);

				// try allocating it
				tb_void_t* p = tb_mpool_realloc(chunk->pool, data, size);
				if (p) 
				{
					// update predicted info
					spool->pred = itor;

					// return it
					return p;
				}
				
				TB_ASSERTM(0, "the chunk size may be too small: %u < %u", chunk->size, size);

			} while (0);

			// remove it if failed
			tb_slist_remove(spool->list, itor);
		}
	}
	TB_ASSERTM(0, "invalid realloc data address:%x %u", data, size);
	return TB_NULL;
}
tb_void_t tb_spool_free(tb_spool_t* spool, tb_void_t* data)
{
	TB_ASSERT_RETURN(spool && spool->list);
	TB_IF_FAIL_RETURN(data);

	// free it from the predicted pool first
	if (spool->pred)
	{
		tb_spool_chunk_t* chunk = tb_slist_itor_at(spool->list, spool->pred);
		if (chunk) 
		{
			// check the chunk
			TB_ASSERT_RETURN(chunk->pool && chunk->data && chunk->size);

			// try freeing it
			if (tb_mpool_free(chunk->pool, data)) return ;
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
				TB_ASSERT_RETURN(chunk->pool && chunk->data && chunk->size);

				// try free it
				if (tb_mpool_free(chunk->pool, data))
				{
					// update predicted info
					spool->pred = itor;

					// return it
					return ;
				}
			}
		}
	}

	TB_ASSERTM(0, "invalid free data address:%x", data);
}
tb_char_t* tb_spool_strdup(tb_spool_t* spool, tb_char_t const* s)
{
	TB_ASSERT_RETURN_VAL(spool && s, TB_NULL);

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
	TB_ASSERT_RETURN_VAL(spool && s, TB_NULL);

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


