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

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_SPOOL_CHUNK_GROW 			(8)
#else
# 	define TB_SPOOL_CHUNK_GROW 			(16)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the small or string pool
typedef struct __tb_spool_t
{
	// the pools align
	tb_size_t 			align;

	// the chunk pools
	tb_handle_t* 		pools;
	tb_size_t 			pooln;
	tb_size_t 			poolm;

	// the chunk size
	tb_size_t 			size;

	// the chunk pred
	tb_size_t 			pred;

}tb_spool_t;


/* ///////////////////////////////////////////////////////////////////////
 * the implemention
 */
tb_handle_t tb_spool_init(tb_size_t size, tb_size_t align)
{
	// the chunk size cannot be too small for the memory pool
	tb_assert_and_check_return_val(size, TB_NULL);

	// init spool
	tb_spool_t* spool = (tb_spool_t*)tb_malloc0(sizeof(tb_spool_t));
	tb_assert_and_check_return_val(spool, TB_NULL);

	// init pools align
	spool->align = align;

	// init chunk size
	spool->size = size;

	// init chunk pools
	spool->pooln = 0;
	spool->poolm = TB_SPOOL_CHUNK_GROW;
	spool->pools = (tb_handle_t*)tb_nalloc0(TB_SPOOL_CHUNK_GROW, sizeof(tb_handle_t));
	tb_assert_and_check_goto(spool->pools, fail);

	// init chunk pred
	spool->pred = 0;

	// ok
	return (tb_handle_t)spool;

fail:
	if (spool) tb_spool_exit(spool);
	return TB_NULL;
}

tb_void_t tb_spool_exit(tb_handle_t handle)
{
	// check 
	tb_spool_t* spool = (tb_spool_t*)handle;
	tb_assert_and_check_return(spool);

	// clear
	tb_spool_clear(handle);

	// free pools
	if (spool->pools) tb_free(spool->pools);

	// free spool
	tb_free(spool);
}
tb_void_t tb_spool_clear(tb_handle_t handle)
{
	// check 
	tb_spool_t* spool = (tb_spool_t*)handle;
	tb_assert_and_check_return(spool && spool->pools);

	// clear pools
	tb_size_t i = 0;
	tb_size_t n = spool->pooln;
	for (i = 0; i < n; i++)
	{
		tb_handle_t vpool = spool->pools[i];
		if (vpool) tb_vpool_clear(vpool);
	}
	
	// reinit size
	spool->size = 0;

	// reinit pred
	spool->pred = 0;
}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_malloc_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_spool_malloc_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{

}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_malloc0_impl(tb_handle_t handle, tb_size_t size)
#else
tb_pointer_t tb_spool_malloc0_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{

}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_spool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{

}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_spool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{

}

#ifndef TB_DEBUG
tb_pointer_t tb_spool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_spool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{

}

#ifndef TB_DEBUG
tb_char_t* tb_spool_strdup_impl(tb_handle_t handle, tb_char_t const* data)
#else
tb_char_t* tb_spool_strdup_impl(tb_handle_t handle, tb_char_t const* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	tb_assert_and_check_return_val(handle && data, TB_NULL);

	__tb_register__ tb_size_t 	n = tb_strlen(data);
#ifndef TB_DEBUG
	__tb_register__ tb_char_t* 	p = tb_spool_malloc_impl(handle, n + 1);
#else
	__tb_register__ tb_char_t* 	p = tb_spool_malloc_impl(handle, n + 1, func, line, file);
#endif
	if (p)
	{
		tb_memcpy(p, data, n);
		p[n] = '\0';
	}

	return p;}

#ifndef TB_DEBUG
tb_char_t* tb_spool_strndup_impl(tb_handle_t handle, tb_char_t const* data, tb_size_t size)
#else
tb_char_t* tb_spool_strndup_impl(tb_handle_t handle, tb_char_t const* data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	tb_assert_and_check_return_val(handle && data, TB_NULL);

	size = tb_strnlen(data, size);
#ifndef TB_DEBUG
	__tb_register__ tb_char_t* 	p = tb_spool_malloc_impl(handle, n + 1);
#else
	__tb_register__ tb_char_t* 	p = tb_spool_malloc_impl(handle, n + 1, func, line, file);
#endif
	if (p)
	{
		tb_memcpy(p, data, n);
		p[n] = '\0';
	}

	return p;
}

#ifndef TB_DEBUG
tb_bool_t tb_spool_free_impl(tb_handle_t handle, tb_pointer_t data)
#else
tb_bool_t tb_spool_free_impl(tb_handle_t handle, tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{

}

#ifdef TB_DEBUG
tb_void_t tb_spool_dump(tb_handle_t handle)
{

}
#endif
