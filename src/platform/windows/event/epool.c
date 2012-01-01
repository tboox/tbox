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
 * \file		epool.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../event.h"
#include "../../../math/math.h"
#include "../../../memory/memory.h"
#include "../../../container/container.h"
#include "winsock2.h"
#include "windows.h"

/* /////////////////////////////////////////////////////////
 * implemention
 */
#ifdef TB_CONFIG_EVENT_HAVE_WAITO
# 	include "epool/waito.c"
#endif

#ifdef TB_CONFIG_EVENT_HAVE_SELECT
# 	include "epool/select.c"
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// the event pool mode
typedef enum __tb_epool_mode_t
{
	TB_EPOOL_MODE_SLECT 	= 0
,	TB_EPOOL_MODE_WAITO 	= 1

}tb_epool_mode_t;

// the event pool type
typedef struct __tb_epool_t
{
	// the maxn
	tb_size_t 			maxn;

	// the pool
	tb_handle_t 		pool[2];

}tb_epool_t;

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_handle_t tb_epool_init(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// init pool
	tb_epool_t* ep = tb_calloc(1, sizeof(tb_epool_t));
	tb_assert_and_check_return_val(ep, TB_NULL);

	// init maxn
	ep->maxn = maxn;

	// ok
	return (tb_handle_t)ep;
}

tb_void_t tb_epool_exit(tb_handle_t pool)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	if (ep)
	{
#ifdef TB_CONFIG_EVENT_HAVE_SELECT
		if (ep->pool[TB_EPOOL_MODE_SLECT]) tb_epool_slect_exit(ep->pool[TB_EPOOL_MODE_SLECT]);
#endif

#ifdef TB_CONFIG_EVENT_HAVE_WAITO
		if (ep->pool[TB_EPOOL_MODE_WAITO]) tb_epool_waito_exit(ep->pool[TB_EPOOL_MODE_WAITO]);
#endif
		tb_free(ep);
	}
}
tb_size_t tb_epool_addo(tb_handle_t pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && handle, 0);

	// addo
	switch (otype)
	{
#ifdef TB_CONFIG_EVENT_HAVE_SELECT
	case TB_EOTYPE_SOCK:
		{
			// only for slect pool
			tb_assert_and_check_return_val(!ep->pool[TB_EPOOL_MODE_WAITO], 0);

			// init pool
			if (!ep->pool[TB_EPOOL_MODE_SLECT]) ep->pool[TB_EPOOL_MODE_SLECT] = tb_epool_slect_init(ep->maxn);
			tb_assert_and_check_return_val(ep->pool[TB_EPOOL_MODE_SLECT], 0);

			// add object
			return tb_epool_slect_addo(ep->pool[TB_EPOOL_MODE_SLECT], handle, otype, etype);
		}
		break;
#endif

#ifdef TB_CONFIG_EVENT_HAVE_WAITO
	case TB_EOTYPE_FILE:
	case TB_EOTYPE_EVET:
		{
			// only for waito pool
			tb_assert_and_check_return_val(!ep->pool[TB_EPOOL_MODE_SLECT], 0);

			// init pool
			if (!ep->pool[TB_EPOOL_MODE_WAITO]) ep->pool[TB_EPOOL_MODE_WAITO] = tb_epool_waito_init(ep->maxn);
			tb_assert_and_check_return_val(ep->pool[TB_EPOOL_MODE_WAITO], 0);

			// add object
			return tb_epool_waito_addo(ep->pool[TB_EPOOL_MODE_WAITO], handle, otype, etype);
		}
#endif
	default:
		break;
	}
	
	tb_trace_noimpl();
	return 0;
}
tb_size_t tb_epool_seto(tb_handle_t pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && handle, 0);

	// addo
	switch (otype)
	{
#ifdef TB_CONFIG_EVENT_HAVE_SELECT
	case TB_EOTYPE_SOCK:
		{
			// only for slect pool
			tb_assert_and_check_return_val(!ep->pool[TB_EPOOL_MODE_WAITO], 0);

			// check pool
			tb_assert_and_check_return_val(ep->pool[TB_EPOOL_MODE_SLECT], 0);

			// add object
			return tb_epool_slect_seto(ep->pool[TB_EPOOL_MODE_SLECT], handle, otype, etype);
		}
		break;
#endif

#ifdef TB_CONFIG_EVENT_HAVE_WAITO
	case TB_EOTYPE_FILE:
	case TB_EOTYPE_EVET:
		{
			// only for waito pool
			tb_assert_and_check_return_val(!ep->pool[TB_EPOOL_MODE_SLECT], 0);

			// check pool
			tb_assert_and_check_return_val(ep->pool[TB_EPOOL_MODE_WAITO], 0);

			// add object
			return tb_epool_waito_seto(ep->pool[TB_EPOOL_MODE_WAITO], handle, otype, etype);
		}
#endif
	default:
		break;
	}
	
	tb_trace_noimpl();
	return 0;
}
tb_size_t tb_epool_delo(tb_handle_t pool, tb_handle_t handle)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && handle, 0);

	// delo
#ifdef TB_CONFIG_EVENT_HAVE_SELECT
	if (ep->pool[TB_EPOOL_MODE_SLECT]) return tb_epool_slect_delo(ep->pool[TB_EPOOL_MODE_SLECT], handle);
#endif

#ifdef TB_CONFIG_EVENT_HAVE_WAITO
	if (ep->pool[TB_EPOOL_MODE_WAITO]) return tb_epool_waito_delo(ep->pool[TB_EPOOL_MODE_WAITO], handle);
#endif

	tb_trace_noimpl();
	return 0;

}
tb_long_t tb_epool_wait(tb_handle_t pool, tb_eobject_t** objs, tb_long_t timeout)
{	
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && objs, -1);

	// wait
#ifdef TB_CONFIG_EVENT_HAVE_SELECT
	if (ep->pool[TB_EPOOL_MODE_SLECT]) return tb_epool_slect_wait(ep->pool[TB_EPOOL_MODE_SLECT], objs, timeout);
#endif

#ifdef TB_CONFIG_EVENT_HAVE_WAITO
	if (ep->pool[TB_EPOOL_MODE_WAITO]) return tb_epool_waito_wait(ep->pool[TB_EPOOL_MODE_WAITO], objs, timeout);
#endif

	tb_trace_noimpl();
	return -1;
}

