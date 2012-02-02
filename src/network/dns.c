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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		dns.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dns.h"
#include "ipv4.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../platform/platform.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the dns item type
typedef struct __tb_dns_item_t
{
	// the host
	tb_ipv4_t 		host;

	// the rate
	tb_size_t 		rate;

}tb_dns_item_t;

// the dns list type
typedef struct __tb_dns_list_t
{
	// the list
	tb_vector_t* 	list;

	// the fast
	tb_ipv4_t 		fast;

	// the mutx
	tb_handle_t 	mutx;

}tb_dns_list_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the dns list
static tb_dns_list_t* 	g_dns_list = TB_NULL;

/* ///////////////////////////////////////////////////////////////////////
 * list
 */
tb_bool_t tb_dns_list_init()
{
	// no list?
	if (!g_dns_list)
	{
		// alloc list
		g_dns_list = tb_calloc(1, sizeof(tb_dns_list_t));
		tb_assert_and_check_return_val(g_dns_list, TB_FALSE);

		// init mutx
		g_dns_list->mutx = tb_mutex_init(TB_NULL);
		tb_assert_and_check_goto(g_dns_list->mutx, fail);
			
		// init list
		g_dns_list->list = tb_vector_init(8, tb_item_func_ifm(sizeof(tb_dns_item_t), TB_NULL, TB_NULL));
		tb_assert_and_check_goto(g_dns_list->list, fail);
	}

	// init local
	tb_dns_local_init();

	// add the hosts
	tb_dns_list_adds("8.8.8.8");
	tb_dns_list_adds("8.8.4.4");

	// ok
	return TB_TRUE;

fail:
	tb_dns_list_exit();
	return TB_FALSE;
}
tb_void_t tb_dns_list_adds(tb_char_t const* host)
{
	tb_assert_and_check_return(g_dns_list && host);

	// init item
	tb_dns_item_t item;
	if (tb_ipv4_set(&item.host, host))
	{
		item.rate = 0;

		// enter
		if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

		// add
		tb_vector_insert_tail(g_dns_list->list, &item);

		// leave
		if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);

		// ok
		return ;
	}

	// trace
	tb_trace("[dns]: host: add %s failed", host);
}
tb_void_t tb_dns_list_dels(tb_char_t const* host)
{
	tb_assert_and_check_return(g_dns_list && host);

	// enter
	if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

	// list
	tb_vector_t* list = g_dns_list->list;
	tb_assert_and_check_goto(list, end);

	// ipv4
	tb_uint32_t ipv4 = tb_ipv4_set(TB_NULL, host);

	// find it
	tb_size_t itor = tb_vector_itor_head(list);
	tb_size_t tail = tb_vector_itor_tail(list);
	for (; itor != tail; itor = tb_vector_itor_next(list, itor))
	{
		tb_dns_item_t const* item = tb_vector_itor_const_at(list, itor);
		if (item && item->host.u32 == ipv4) break;
	}

	// remove it
	if (itor != tail) tb_vector_remove(list, itor);

end:
	// leave
	if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);
}
tb_ipv4_t tb_dns_list_fast()
{
	// init
	tb_ipv4_t fast = {0};
	tb_assert_and_check_return_val(g_dns_list, fast);

	// enter
	if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

	// fast
	fast = g_dns_list->fast;

	// leave
	if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);

	// ok
	return fast;
}
tb_void_t tb_dns_list_exit()
{
	// exit local
	tb_dns_local_exit();

	// exit list
	tb_handle_t mutx = TB_NULL;
	if (g_dns_list)
	{
		// enter
		if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

		// exists?
		if (g_dns_list)
		{
			// save mutx
			mutx = g_dns_list->mutx;
			g_dns_list->mutx = TB_NULL;

			// free list
			if (g_dns_list->list) tb_vector_exit(g_dns_list->list);
			g_dns_list->list = TB_NULL;

			// free it
			tb_free(g_dns_list);
			g_dns_list = TB_NULL;

			// free mutx
			if (mutx) 
			{
				// leave
				tb_mutex_leave(mutx);

				// exit mutx
				tb_mutex_exit(mutx);
			}
		}
	}
}
#ifdef TB_DEBUG
tb_void_t tb_dns_list_dump()
{	
	tb_assert_and_check_return(g_dns_list);

	// enter
	if (g_dns_list->mutx) tb_mutex_enter(g_dns_list->mutx);

	// list
	tb_vector_t* list = g_dns_list->list;
	tb_assert_and_check_return(list);

	// find it
	tb_print("============================================================");
	tb_print("[dns]: list: %u items", tb_vector_size(list));
	tb_print("[dns]: fast: %u.%u.%u.%u", g_dns_list->fast.u8[0], g_dns_list->fast.u8[1], g_dns_list->fast.u8[2], g_dns_list->fast.u8[3]);
	tb_size_t itor = tb_vector_itor_head(list);
	tb_size_t tail = tb_vector_itor_tail(list);
	for (; itor != tail; itor = tb_vector_itor_next(list, itor))
	{
		tb_dns_item_t const* item = tb_vector_itor_const_at(list, itor);
		if (item) 
		{
			tb_print("[dns]: host: %u.%u.%u.%u, rate: %u"
				, item->host.u8[0]
				, item->host.u8[1]
				, item->host.u8[2]
				, item->host.u8[3]
				, item->rate);
		}
	}

	// leave
	if (g_dns_list->mutx) tb_mutex_leave(g_dns_list->mutx);
}
#endif

/* ///////////////////////////////////////////////////////////////////////
 * host
 */
tb_handle_t tb_dns_host_init(tb_char_t const* host)
{
	tb_trace_noimpl();
	return TB_NULL;
}
tb_long_t tb_dns_host_spak(tb_handle_t hdns, tb_char_t* data, tb_size_t maxn)
{
	tb_trace_noimpl();
	return 0;
}
tb_long_t tb_dns_host_wait(tb_handle_t hdns, tb_size_t timeout)
{
	tb_trace_noimpl();
	return 0;
}
tb_void_t tb_dns_host_exit(tb_handle_t hdns)
{
	tb_trace_noimpl();
}
tb_char_t const* tb_dns_host_done(tb_char_t const* host, tb_char_t* data, tb_size_t maxn)
{
	tb_trace_noimpl();
	return TB_NULL;
}

