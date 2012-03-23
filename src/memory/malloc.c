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
 * \file		malloc.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "malloc.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_handle_t g_gpool = TB_NULL;
static tb_handle_t g_mutex = TB_NULL; 

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_bool_t tb_memory_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	tb_assert_and_check_return_val(data && size, TB_FALSE);

	// init mutex
	if (!g_mutex) g_mutex = tb_mutex_init(TB_NULL);
	tb_assert_and_check_return_val(g_mutex, TB_FALSE);

	// init gpool
	if (!tb_mutex_enter(g_mutex)) return TB_FALSE;
	if (!g_gpool) g_gpool = tb_gpool_init(data, size, align);
	if (!tb_mutex_leave(g_mutex)) return TB_FALSE;

	// ok?
	return g_gpool? TB_TRUE : TB_FALSE;
}
tb_void_t tb_memory_exit()
{
	// exit gpool
	if (g_mutex && tb_mutex_enter(g_mutex))
	{
		if (g_gpool)
		{
			tb_gpool_exit(g_gpool);
			g_gpool = TB_NULL;
		}
		if (g_mutex) tb_mutex_leave(g_mutex);
	}

	// exit mutex
	if (g_mutex)	
	{
		tb_mutex_exit(g_mutex);
		g_mutex = TB_NULL;
	}
}

#ifndef TB_DEBUG
tb_pointer_t tb_memory_malloc_impl(tb_size_t size)
#else
tb_pointer_t tb_memory_malloc_impl(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, TB_NULL);

	// enter
	if (!tb_mutex_enter(g_mutex)) return TB_NULL;

#ifndef TB_DEBUG
	tb_byte_t* p = tb_gpool_malloc_impl(g_gpool, size);
#else
	tb_byte_t* p = tb_gpool_malloc_impl(g_gpool, size, func, line, file);
#endif

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef TB_DEBUG
tb_pointer_t tb_memory_malloc0_impl(tb_size_t size)
#else
tb_pointer_t tb_memory_malloc0_impl(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, TB_NULL);

	// enter
	if (!tb_mutex_enter(g_mutex)) return TB_NULL;

#ifndef TB_DEBUG
	tb_byte_t* p = tb_gpool_malloc0_impl(g_gpool, size);
#else
	tb_byte_t* p = tb_gpool_malloc0_impl(g_gpool, size, func, line, file);
#endif

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef TB_DEBUG
tb_pointer_t tb_memory_nalloc_impl(tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_memory_nalloc_impl(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, TB_NULL);

	// enter
	if (!tb_mutex_enter(g_mutex)) return TB_NULL;

#ifndef TB_DEBUG
	tb_byte_t* p = tb_gpool_nalloc_impl(g_gpool, item, size);
#else
	tb_byte_t* p = tb_gpool_nalloc_impl(g_gpool, item, size, func, line, file);
#endif

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef TB_DEBUG
tb_pointer_t tb_memory_nalloc0_impl(tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_memory_nalloc0_impl(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, TB_NULL);

	// enter
	if (!tb_mutex_enter(g_mutex)) return TB_NULL;

#ifndef TB_DEBUG
	tb_byte_t* p = tb_gpool_nalloc0_impl(g_gpool, item, size);
#else
	tb_byte_t* p = tb_gpool_nalloc0_impl(g_gpool, item, size, func, line, file);
#endif

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef TB_DEBUG
tb_pointer_t tb_memory_ralloc_impl(tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_memory_ralloc_impl(tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, TB_NULL);

	// enter
	if (!tb_mutex_enter(g_mutex)) return TB_NULL;

#ifndef TB_DEBUG
	tb_byte_t* p = tb_gpool_ralloc_impl(g_gpool, data, size);
#else
	tb_byte_t* p = tb_gpool_ralloc_impl(g_gpool, data, size, func, line, file);
#endif

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef TB_DEBUG
tb_size_t tb_memory_free_impl(tb_pointer_t data)
#else
tb_size_t tb_memory_free_impl(tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, 0);

	// enter
	if (!tb_mutex_enter(g_mutex)) return 0;

#ifndef TB_DEBUG
	tb_size_t size = tb_gpool_free_impl(g_gpool, data);
#else
	tb_size_t size = tb_gpool_free_impl(g_gpool, data, func, line, file);
#endif

	// leave
	tb_mutex_leave(g_mutex);

	// ok
	return size;
}

#ifdef TB_DEBUG
tb_void_t tb_memory_dump()
{
	// check 
	tb_assert_and_check_return(g_gpool && g_mutex);

	// enter
	if (tb_mutex_enter(g_mutex)) 
	{
		// dump
		tb_gpool_dump(g_gpool);

		// leave
		tb_mutex_leave(g_mutex);
	}
}
#endif
