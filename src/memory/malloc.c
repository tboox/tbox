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
 * @file		malloc.c
 * @ingroup 	memory
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "malloc.h"
#include "gpool.h"
#include "scache.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_handle_t g_gpool = tb_null;
static tb_handle_t g_mutex = tb_null; 

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_memory_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	tb_assert_and_check_return_val(data && size, tb_false);

	// init mutex
	if (!g_mutex) g_mutex = tb_mutex_init(tb_null);
	tb_assert_and_check_return_val(g_mutex, tb_false);

	// init gpool
	if (!tb_mutex_enter(g_mutex)) return tb_false;
	if (!g_gpool) g_gpool = tb_gpool_init(data, size, align);
	if (!tb_mutex_leave(g_mutex)) return tb_false;

	// ok?
	return g_gpool? tb_scache_init(align) : tb_false;
}
tb_void_t tb_memory_exit()
{
	// exit scache
	tb_scache_exit();

	// dump gpool
#ifdef __tb_debug__
	tb_memory_dump();
#endif

	// exit gpool
	if (g_mutex && tb_mutex_enter(g_mutex))
	{
		if (g_gpool)
		{
			tb_gpool_exit(g_gpool);
			g_gpool = tb_null;
		}
		if (g_mutex) tb_mutex_leave(g_mutex);
	}

	// exit mutex
	if (g_mutex)	
	{
		tb_mutex_exit(g_mutex);
		g_mutex = tb_null;
	}
}

#ifndef __tb_debug__
tb_pointer_t tb_memory_malloc_impl(tb_size_t size)
#else
tb_pointer_t tb_memory_malloc_impl(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, tb_null);

	// enter
	if (!tb_mutex_enter(g_mutex)) return tb_null;

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_malloc_impl(g_gpool, size);
#else
	tb_byte_t* p = tb_gpool_malloc_impl(g_gpool, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_memory_malloc0_impl(tb_size_t size)
#else
tb_pointer_t tb_memory_malloc0_impl(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, tb_null);

	// enter
	if (!tb_mutex_enter(g_mutex)) return tb_null;

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_malloc0_impl(g_gpool, size);
#else
	tb_byte_t* p = tb_gpool_malloc0_impl(g_gpool, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_memory_nalloc_impl(tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_memory_nalloc_impl(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, tb_null);

	// enter
	if (!tb_mutex_enter(g_mutex)) return tb_null;

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_nalloc_impl(g_gpool, item, size);
#else
	tb_byte_t* p = tb_gpool_nalloc_impl(g_gpool, item, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_memory_nalloc0_impl(tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_memory_nalloc0_impl(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, tb_null);

	// enter
	if (!tb_mutex_enter(g_mutex)) return tb_null;

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_nalloc0_impl(g_gpool, item, size);
#else
	tb_byte_t* p = tb_gpool_nalloc0_impl(g_gpool, item, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_memory_ralloc_impl(tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_memory_ralloc_impl(tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, tb_null);

	// enter
	if (!tb_mutex_enter(g_mutex)) return tb_null;

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_ralloc_impl(g_gpool, data, size);
#else
	tb_byte_t* p = tb_gpool_ralloc_impl(g_gpool, data, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_mutex_leave(g_mutex);
	return p;
}

#ifndef __tb_debug__
tb_bool_t tb_memory_free_impl(tb_pointer_t data)
#else
tb_bool_t tb_memory_free_impl(tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_gpool && g_mutex, tb_false);

	// enter
	if (!tb_mutex_enter(g_mutex)) return tb_false;

#ifndef __tb_debug__
	tb_bool_t r = tb_gpool_free_impl(g_gpool, data);
#else
	tb_bool_t r = tb_gpool_free_impl(g_gpool, data, func, line, file);
#endif

	// check
	tb_assert(r);

	// leave
	tb_mutex_leave(g_mutex);

	// ok
	return r;
}

#ifdef __tb_debug__
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
