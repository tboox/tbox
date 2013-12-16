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
#include "../platform/platform.h"


/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the pool
static tb_handle_t 		g_pool = tb_null;

// the lock
static tb_spinlock_t 	g_lock = TB_SPINLOCK_INIT; 

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_malloc_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	// check
	tb_assert_and_check_return_val(data && size, tb_false);

	// init pool
	tb_bool_t ok = tb_false;
	tb_spinlock_enter(&g_lock);
	if (!g_pool) g_pool = tb_gpool_init(data, size, align);
	if (g_pool) ok = tb_true;
	tb_spinlock_leave(&g_lock);

	// ok?
	return ok;
}
tb_void_t tb_malloc_exit()
{
	// exit pool
	tb_spinlock_enter(&g_lock);
	if (g_pool) tb_gpool_exit(g_pool);
	g_pool = tb_null;
	tb_spinlock_leave(&g_lock);

	// exit lock
	g_lock = 0;
}
#ifdef __tb_debug__
tb_size_t tb_malloc_data_size(tb_cpointer_t data)
{
	// check 
	tb_check_return_val(g_pool, 0);

	// try to enter, ensure outside the pool
	tb_size_t size = 0;
	if (tb_spinlock_enter_try(&g_lock))
	{
		// size
		size = tb_gpool_data_size(g_pool, data);

		// leave
		tb_spinlock_leave(&g_lock);
	}

	// ok?
	return size;
}
tb_void_t tb_malloc_data_dump(tb_cpointer_t data, tb_char_t const* prefix)
{
	// check 
	tb_check_return(g_pool);

	// try to enter, ensure outside the pool
	if (tb_spinlock_enter_try(&g_lock))
	{
		// dump
		tb_gpool_data_dump(g_pool, data, prefix);

		// leave
		tb_spinlock_leave(&g_lock);
	}
}
tb_void_t tb_malloc_dump()
{
	// check 
	tb_assert_and_check_return(g_pool);

	// enter
	tb_spinlock_enter(&g_lock);
	
	// dump
	tb_gpool_dump(g_pool);

	// leave
	tb_spinlock_leave(&g_lock);
}
#endif
#ifndef __tb_debug__
tb_pointer_t tb_malloc_malloc_impl(tb_size_t size)
#else
tb_pointer_t tb_malloc_malloc_impl(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_pool, tb_null);

	// enter
	tb_spinlock_enter(&g_lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_malloc_impl(g_pool, size);
#else
	tb_byte_t* p = tb_gpool_malloc_impl(g_pool, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&g_lock);
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_malloc_malloc0_impl(tb_size_t size)
#else
tb_pointer_t tb_malloc_malloc0_impl(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_pool, tb_null);

	// enter
	tb_spinlock_enter(&g_lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_malloc0_impl(g_pool, size);
#else
	tb_byte_t* p = tb_gpool_malloc0_impl(g_pool, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&g_lock);
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_malloc_nalloc_impl(tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_malloc_nalloc_impl(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_pool, tb_null);

	// enter
	tb_spinlock_enter(&g_lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_nalloc_impl(g_pool, item, size);
#else
	tb_byte_t* p = tb_gpool_nalloc_impl(g_pool, item, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&g_lock);
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_malloc_nalloc0_impl(tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_malloc_nalloc0_impl(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_pool, tb_null);

	// enter
	tb_spinlock_enter(&g_lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_nalloc0_impl(g_pool, item, size);
#else
	tb_byte_t* p = tb_gpool_nalloc0_impl(g_pool, item, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&g_lock);
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_malloc_ralloc_impl(tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_malloc_ralloc_impl(tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_pool, tb_null);

	// enter
	tb_spinlock_enter(&g_lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_gpool_ralloc_impl(g_pool, data, size);
#else
	tb_byte_t* p = tb_gpool_ralloc_impl(g_pool, data, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&g_lock);
	return p;
}

#ifndef __tb_debug__
tb_bool_t tb_malloc_free_impl(tb_pointer_t data)
#else
tb_bool_t tb_malloc_free_impl(tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(g_pool, tb_false);

	// enter
	tb_spinlock_enter(&g_lock);

#ifndef __tb_debug__
	tb_bool_t r = tb_gpool_free_impl(g_pool, data);
#else
	tb_bool_t r = tb_gpool_free_impl(g_pool, data, func, line, file);
#endif

	// check
	tb_assert(r);

	// leave
	tb_spinlock_leave(&g_lock);

	// ok
	return r;
}

