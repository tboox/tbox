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
 * @file		opool.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__
# 	define TB_OPOOL_GROW 		(256 * 1024)
#else
# 	define TB_OPOOL_GROW 		(512 * 1024)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the pool
static tb_handle_t 				g_pool = tb_null;

// the lock
static tb_spinlock_t 			g_lock = TB_SPINLOCK_INIT;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_opool_init()
{
	// init pool
	if (!g_pool) g_pool = tb_spool_init(TB_OPOOL_GROW, 0);
	tb_assert_and_check_return_val(g_pool, tb_false);

	// ok
	return tb_true;
}
tb_void_t tb_opool_exit()
{
	// dump
#ifdef __tb_debug__
	tb_opool_dump();
#endif

	// enter
	tb_spinlock_enter(&g_lock);

	// exit pool
	if (g_pool) tb_spool_exit(g_pool);
	g_pool = tb_null;

	// leave
	tb_spinlock_leave(&g_lock);
}
tb_void_t tb_opool_clear()
{
	// enter
	tb_spinlock_enter(&g_lock);

	// clear pool
	if (g_pool) tb_spool_clear(g_pool);

	// leave
	tb_spinlock_leave(&g_lock);
}
tb_object_t* tb_opool_get_impl(tb_size_t size, tb_size_t flag, tb_size_t type __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return_val(size && type, tb_null);

	// enter
	tb_spinlock_enter(&g_lock);

	// make object
	tb_object_t* object = g_pool? tb_spool_malloc0_impl(g_pool, size __tb_debug_args__) : tb_null;

	// init object
	if (object) 
	{
		if (!tb_object_init(object, flag, type)) 
		{
			tb_spool_free_impl(g_pool, object __tb_debug_args__);
			object = tb_null;
		}
	}

	// leave
	tb_spinlock_leave(&g_lock);

	// ok?
	return object;
}
tb_void_t tb_opool_del_impl(tb_object_t* object __tb_debug_decl__)
{
	// check
	tb_assert_and_check_return(object);

	// enter
	tb_spinlock_enter(&g_lock);

	// exit object
	if (g_pool) tb_spool_free_impl(g_pool, object __tb_debug_args__);

	// leave
	tb_spinlock_leave(&g_lock);
}
#ifdef __tb_debug__
tb_void_t tb_opool_dump()
{
	// enter
	tb_spinlock_enter(&g_lock);

	// dump
	if (g_pool) tb_spool_dump(g_pool, "opool");

	// leave
	tb_spinlock_leave(&g_lock);
}
#endif
