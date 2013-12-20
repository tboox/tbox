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
 * @file		cache.c
 * @ingroup 	network
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "cache.h"
#include "../../platform/platform.h"
#include "../../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the cache maxn
#ifdef __tb_small__
# 	define TB_DNS_CACHE_MAXN 		(64)
#else
# 	define TB_DNS_CACHE_MAXN 		(256)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the dns cache type
typedef struct __tb_dns_cache_t
{
	// the pool
	tb_handle_t 			pool;

	// the hash
	tb_hash_t* 				hash;

	// the times
	tb_hize_t 				times;

	// the expired
	tb_size_t 				expired;

}tb_dns_cache_t;

// the dns cache addr type
typedef struct __tb_dns_cache_addr_t
{
	// the ipv4
	tb_ipv4_t 				ipv4;

	// the time
	tb_size_t 				time;

}tb_dns_cache_addr_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the lock
static tb_spinlock_t 		g_lock = TB_SPINLOCK_INIT;

// the cache
static tb_dns_cache_t 		g_cache = {0};

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_dns_cache_init()
{
	// enter
	tb_spinlock_enter(&g_lock);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// init pool
		if (!g_cache.pool) g_cache.pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
		tb_assert_and_check_break(g_cache.pool);

		// init hash
		if (!g_cache.hash) g_cache.hash = tb_hash_init(tb_align8(tb_isqrti(TB_DNS_CACHE_MAXN) + 1), tb_item_func_str(tb_false, g_cache.pool), tb_item_func_ifm(sizeof(tb_dns_cache_addr_t), tb_null, tb_null));
		tb_assert_and_check_break(g_cache.hash);

		// ok
		ok = tb_true;

	} while (0);

	// leave
	tb_spinlock_leave(&g_lock);

	// failed? exit it
	if (!ok) tb_dns_cache_exit();

	// ok?
	return ok;
}
tb_void_t tb_dns_cache_exit()
{
	// enter
	tb_spinlock_enter(&g_lock);

	// exit hash
	if (g_cache.hash) tb_hash_exit(g_cache.hash);
	g_cache.hash = tb_null;

	// exit pool
	if (g_cache.pool) tb_hash_exit(g_cache.pool);
	g_cache.pool = tb_null;

	// exit times
	g_cache.times = 0;

	// exit expired 
	g_cache.expired = 0;

	// leave
	tb_spinlock_leave(&g_lock);
}
tb_void_t tb_dns_cache_dump()
{
}
tb_bool_t tb_dns_cache_get(tb_char_t const* name, tb_ipv4_t* addr)
{
	// check
	tb_assert_and_check_return_val(name && addr, tb_false);

	// enter
	tb_spinlock_enter(&g_lock);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// ok
		ok = tb_true;

	} while (0);

	// leave
	tb_spinlock_leave(&g_lock);

	// ok?
	return ok;
}
tb_void_t tb_dns_cache_set(tb_char_t const* name, tb_ipv4_t const* addr)
{
	// check
	tb_assert_and_check_return(name && addr);

	// enter
	tb_spinlock_enter(&g_lock);

	// done
	do
	{
	} while (0);

	// leave
	tb_spinlock_leave(&g_lock);
}
