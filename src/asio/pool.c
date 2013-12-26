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
 * @file		pool.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pool.h"
#include "aico.h"
#include "aicp.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifndef __tb_debug__
tb_pointer_t tb_aicp_pool_malloc_impl(tb_aicp_t* aicp, tb_size_t size)
#else
tb_pointer_t tb_aicp_pool_malloc_impl(tb_aicp_t* aicp, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(aicp && aicp->pool, tb_null);

	// enter
	tb_spinlock_enter(&aicp->lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_spool_malloc_impl(aicp->pool, size);
#else
	tb_byte_t* p = tb_spool_malloc_impl(aicp->pool, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&aicp->lock);

	// ok?
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_aicp_pool_malloc0_impl(tb_aicp_t* aicp, tb_size_t size)
#else
tb_pointer_t tb_aicp_pool_malloc0_impl(tb_aicp_t* aicp, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(aicp && aicp->pool, tb_null);

	// enter
	tb_spinlock_enter(&aicp->lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_spool_malloc0_impl(aicp->pool, size);
#else
	tb_byte_t* p = tb_spool_malloc0_impl(aicp->pool, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&aicp->lock);

	// ok?
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_aicp_pool_nalloc_impl(tb_aicp_t* aicp, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_aicp_pool_nalloc_impl(tb_aicp_t* aicp, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(aicp && aicp->pool, tb_null);

	// enter
	tb_spinlock_enter(&aicp->lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_spool_nalloc_impl(aicp->pool, item, size);
#else
	tb_byte_t* p = tb_spool_nalloc_impl(aicp->pool, item, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&aicp->lock);

	// ok?
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_aicp_pool_nalloc0_impl(tb_aicp_t* aicp, tb_size_t item, tb_size_t size)
#else
tb_pointer_t tb_aicp_pool_nalloc0_impl(tb_aicp_t* aicp, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(aicp && aicp->pool, tb_null);

	// enter
	tb_spinlock_enter(&aicp->lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_spool_nalloc0_impl(aicp->pool, item, size);
#else
	tb_byte_t* p = tb_spool_nalloc0_impl(aicp->pool, item, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&aicp->lock);

	// ok?
	return p;
}

#ifndef __tb_debug__
tb_pointer_t tb_aicp_pool_ralloc_impl(tb_aicp_t* aicp, tb_pointer_t data, tb_size_t size)
#else
tb_pointer_t tb_aicp_pool_ralloc_impl(tb_aicp_t* aicp, tb_pointer_t data, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(aicp && aicp->pool, tb_null);

	// enter
	tb_spinlock_enter(&aicp->lock);

#ifndef __tb_debug__
	tb_byte_t* p = tb_spool_ralloc_impl(aicp->pool, data, size);
#else
	tb_byte_t* p = tb_spool_ralloc_impl(aicp->pool, data, size, func, line, file);
#endif

	// check
	tb_assert(p);

	// leave
	tb_spinlock_leave(&aicp->lock);

	// ok?
	return p;
}

#ifndef __tb_debug__
tb_bool_t tb_aicp_pool_free_impl(tb_aicp_t* aicp, tb_pointer_t data)
#else
tb_bool_t tb_aicp_pool_free_impl(tb_aicp_t* aicp, tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file)
#endif
{
	// check 
	tb_assert_and_check_return_val(aicp && aicp->pool, tb_false);

	// enter
	tb_spinlock_enter(&aicp->lock);

#ifndef __tb_debug__
	tb_bool_t r = tb_spool_free_impl(aicp->pool, data);
#else
	tb_bool_t r = tb_spool_free_impl(aicp->pool, data, func, line, file);
#endif

	// check
	tb_assert(r);

	// leave
	tb_spinlock_leave(&aicp->lock);

	// ok?
	return r;
}

