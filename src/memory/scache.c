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
 * @file		scache.c
 * @ingroup 	memory
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scache.h"
#include "spool.h"
#include "../libc/libc.h"
#include "../platform/platform.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the string pool
static tb_handle_t 		g_pool = tb_null;

// the string hash
static tb_handle_t 		g_hash = tb_null;

// the string lock
static tb_handle_t 		g_lock = tb_null;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_scache_init(tb_size_t align)
{
	// init lock
	if (!g_lock) g_lock = tb_spinlock_init();
	tb_assert_and_check_return_val(g_lock, tb_false);

	// init pool
	if (!g_pool) g_pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, align);
	tb_assert_and_check_return_val(g_pool, tb_false);

	// init hash
	if (!g_hash) g_hash = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_str(tb_true, g_pool), tb_item_func_size());
	tb_assert_and_check_return_val(g_hash, tb_false);

	// ok
	return tb_true;
}
tb_void_t tb_scache_exit()
{
	// dump
#ifdef __tb_debug__
	tb_scache_dump();
#endif

	// enter
	if (g_lock) tb_spinlock_enter(g_lock);

	// exit hash
	if (g_hash) tb_hash_exit(g_hash);
	g_hash = tb_null;

	// exit pool
	if (g_pool) tb_spool_exit(g_pool);
	g_pool = tb_null;

	// leave
	if (g_lock) tb_spinlock_leave(g_lock);

	// exit lock
	if (g_lock) tb_spinlock_exit(g_lock);
	g_lock = tb_null;
}
tb_void_t tb_scache_clear()
{
	// enter
	if (g_lock) tb_spinlock_enter(g_lock);

	// clear hash
	if (g_hash) tb_hash_clear(g_hash);

	// clear pool
	if (g_pool) tb_spool_clear(g_pool);

	// leave
	if (g_lock) tb_spinlock_leave(g_lock);
}
tb_char_t const* tb_scache_put(tb_char_t const* data)
{
	// check
	tb_assert_and_check_return_val(data, tb_null);

	// enter
	if (g_lock) tb_spinlock_enter(g_lock);

	// done
	tb_char_t const* cstr = tb_null;
	if (g_hash)
	{
		// exists?
		tb_size_t 				itor = 0;
		tb_hash_item_t const* 	item = tb_null;
		if ((itor = tb_hash_itor(g_hash, data)) && (item = tb_iterator_item(g_hash, itor)))
		{
			// refn
			tb_size_t refn = (tb_size_t)item->data;

			// refn++
			if (refn) tb_iterator_copy(g_hash, itor, refn + 1);
			// no refn? remove it
			else
			{
				// assert
				tb_assert(0);

				// del it
				tb_iterator_delt(g_hash, itor);
				item = tb_null;
			}
		}
		
		// no item? add it
		if (!item)
		{
			// add it
			if (itor = tb_hash_set(g_hash, data, 1))
				item = tb_iterator_item(g_hash, itor);
		}

		// save the cstr
		if (item) cstr = item->name;
	}

	// leave
	if (g_lock) tb_spinlock_leave(g_lock);

	// ok?
	return cstr;
}
tb_void_t tb_scache_del(tb_char_t const* data)
{
	// check
	tb_assert_and_check_return(data);

	// enter
	if (g_lock) tb_spinlock_enter(g_lock);

	// done
	tb_hash_item_t const* item = tb_null;
	if (g_hash)
	{
		// exists?
		tb_size_t itor = 0;
		if ((itor = tb_hash_itor(g_hash, data)) && (item = tb_iterator_item(g_hash, itor)))
		{
			// refn
			tb_size_t refn = (tb_size_t)item->data;

			// refn--
			if (refn > 1) tb_iterator_copy(g_hash, itor, refn - 1);
			// del it
			else tb_iterator_delt(g_hash, itor);
		}
	}

	// leave
	if (g_lock) tb_spinlock_leave(g_lock);
}
#ifdef __tb_debug__
tb_void_t tb_scache_dump()
{
	// enter
	if (g_lock) tb_spinlock_enter(g_lock);

	// dump hash
	if (g_hash && tb_hash_size(g_hash))
	{
		// trace
		tb_print("======================================================================");
		tb_print("scache: size: %lu", tb_hash_size(g_hash));

		// walk
		tb_size_t itor = tb_iterator_head(g_hash);
		tb_size_t tail = tb_iterator_tail(g_hash);
		for (; itor != tail; itor = tb_iterator_next(g_hash, itor))
		{
			tb_hash_item_t const* item = tb_iterator_item(g_hash, itor);
			if (item) tb_print("scache: item: refn: %lu, cstr: %s", (tb_size_t)item->data, item->name);
		}
	}

	// leave
	if (g_lock) tb_spinlock_leave(g_lock);
}
#endif
