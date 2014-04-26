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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		cache.c
 * @ingroup 	network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"dns_cache"
#define TB_TRACE_MODULE_DEBUG 		(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "cache.h"
#include "../../platform/platform.h"
#include "../../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the cache maxn
#ifdef __tb_small__
# 	define TB_DNS_CACHE_MAXN 		(64)
#else
# 	define TB_DNS_CACHE_MAXN 		(256)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the lock
static tb_spinlock_t 		g_lock = TB_SPINLOCK_INIT;

// the cache
static tb_dns_cache_t 		g_cache = {0};

/* //////////////////////////////////////////////////////////////////////////////////////
 * helper
 */
static __tb_inline__ tb_size_t tb_dns_cache_now()
{
	return (tb_size_t)(tb_ctime_spak() / 1000);
}
static tb_bool_t tb_dns_cache_cler(tb_handle_t cache, tb_hash_item_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(cache && bdel, tb_false);

	if (item)
	{
		// the dns cache address
		tb_dns_cache_addr_t const* caddr = item->data;
		tb_assert_and_check_return_val(caddr, tb_false);

		// is expired?
		if (caddr->time < g_cache.expired)
		{
			// remove it
			*bdel = tb_true;

			// trace
			tb_trace_d("del: %s => %u.%u.%u.%u, time: %u, size: %u", (tb_char_t const*)item->name
																	, 	caddr->ipv4.u8[0]
																	, 	caddr->ipv4.u8[1]
																	, 	caddr->ipv4.u8[2]
																	, 	caddr->ipv4.u8[3]
																	, 	caddr->time
																	, 	tb_hash_size(g_cache.hash));

			// update times
			tb_assert_and_check_return_val(g_cache.times >= caddr->time, tb_false);
			g_cache.times -= caddr->time;
		}
	}

	// ok
	return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
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
		if (!g_cache.pool) g_cache.pool = tb_block_pool_init(TB_BLOCK_POOL_GROW_DEFAULT, 0);
		tb_assert_and_check_break(g_cache.pool);

		// init hash
		if (!g_cache.hash) g_cache.hash = tb_hash_init(tb_align8(tb_isqrti(TB_DNS_CACHE_MAXN) + 1), tb_item_func_str(tb_false, g_cache.pool), tb_item_func_mem(sizeof(tb_dns_cache_addr_t), tb_null, tb_null));
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
	if (g_cache.pool) tb_block_pool_exit(g_cache.pool);
	g_cache.pool = tb_null;

	// exit times
	g_cache.times = 0;

	// exit expired 
	g_cache.expired = 0;

	// leave
	tb_spinlock_leave(&g_lock);
}
tb_bool_t tb_dns_cache_get(tb_char_t const* name, tb_ipv4_t* addr)
{
	// check
	tb_assert_and_check_return_val(name && addr, tb_false);

	// trace
	tb_trace_d("get: %s", name);

	// is ipv4?
	tb_check_return_val(!tb_ipv4_set(addr, name), tb_true);

	// is localhost?
	if (!tb_stricmp(name, "localhost"))
	{
		tb_ipv4_set(addr, "127.0.0.1");
		return tb_true;
	}

	// clear addr
	tb_ipv4_clr(addr);

	// enter
	tb_spinlock_enter(&g_lock);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// check
		tb_assert_and_check_break(g_cache.hash);

		// get the host address
		tb_dns_cache_addr_t* caddr = tb_hash_get(g_cache.hash, name);
		tb_check_break(caddr);

		// trace
		tb_trace_d("get: %s => %u.%u.%u.%u, time: %u => %u, size: %u",	name
																	, 	caddr->ipv4.u8[0]
																	, 	caddr->ipv4.u8[1]
																	, 	caddr->ipv4.u8[2]
																	, 	caddr->ipv4.u8[3]
																	, 	caddr->time
																	, 	tb_dns_cache_now()
																	, 	tb_hash_size(g_cache.hash));

		// update time
		tb_assert_and_check_break(g_cache.times >= caddr->time);
		g_cache.times -= caddr->time;
		caddr->time = tb_dns_cache_now();
		g_cache.times += caddr->time;

		// save addr
		*addr = caddr->ipv4;

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
	tb_assert_and_check_return(name && addr && addr->u32);

	// trace
	tb_trace_d("set: %s => %u.%u.%u.%u", name, addr->u8[0], addr->u8[1], addr->u8[2], addr->u8[3]);

	// init addr
	tb_dns_cache_addr_t caddr;
	caddr.ipv4 = *addr;
	caddr.time = tb_dns_cache_now();

	// enter
	tb_spinlock_enter(&g_lock);

	// done
	do
	{
		// check
		tb_assert_and_check_break(g_cache.hash);

		// remove the expired items if full
		if (tb_hash_size(g_cache.hash) >= TB_DNS_CACHE_MAXN)
		{
			// the expired time
			g_cache.expired = ((tb_size_t)(g_cache.times / tb_hash_size(g_cache.hash)) + 1);

			// check
			tb_assert_and_check_break(g_cache.expired);

			// trace
			tb_trace_d("expired: %lu", g_cache.expired);

			// remove the expired times
			tb_hash_walk(g_cache.hash, tb_dns_cache_cler, tb_null);
		}

		// check
		tb_assert_and_check_break(tb_hash_size(g_cache.hash) < TB_DNS_CACHE_MAXN);

		// save addr
		tb_hash_set(g_cache.hash, name, &caddr);

		// update times
		g_cache.times += caddr.time;

		// trace
		tb_trace_d("set: %s => %u.%u.%u.%u, time: %u, size: %u", name
																, 	caddr.ipv4.u8[0]
																, 	caddr.ipv4.u8[1]
																, 	caddr.ipv4.u8[2]
																, 	caddr.ipv4.u8[3]
																, 	caddr.time
																, 	tb_hash_size(g_cache.hash));

	} while (0);

	// leave
	tb_spinlock_leave(&g_lock);
}
