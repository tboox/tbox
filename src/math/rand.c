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
 * \file		rand.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rand.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_RAND_LINEAR_SEED 		(1)

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_rand_t* 		g_rand = TB_NULL;
static tb_handle_t 		g_mutex = TB_NULL;

/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static __tb_inline__ tb_uint32_t tb_rand_linear_next_uint32(tb_rand_linear_t* rand)
{
	rand->data = (rand->data * 10807 + 1) & 0xffffffff;
	return rand->data;
}

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

// the global rand
tb_bool_t tb_rand_init()
{
	if (!g_mutex) g_mutex = tb_mutex_init(TB_NULL);
	tb_assert_and_check_return_val(g_mutex, TB_FALSE);

	if (!tb_mutex_enter(g_mutex)) return TB_FALSE;
	if (!g_rand) g_rand = tb_rand_linear_init(TB_RAND_LINEAR_SEED);
	if (!tb_mutex_leave(g_mutex)) return TB_FALSE;

	return g_rand? TB_TRUE : TB_FALSE;
}
tb_void_t tb_rand_exit()
{
	if (tb_mutex_enter(g_mutex))
	{
		if (g_rand)
		{
			tb_rand_linear_exit(g_rand);
			g_rand = TB_NULL;
		}
		tb_mutex_leave(g_mutex);
	}

	if (g_mutex)	
	{
		tb_mutex_exit(g_mutex);
		g_mutex = TB_NULL;
	}
}

tb_uint32_t tb_rand_uint32(tb_uint32_t b, tb_uint32_t e)
{
	// check 
	tb_uint32_t rand = b;
	tb_assert_and_check_return_val(g_rand && g_mutex, rand);

	// lock
	if (tb_mutex_enter(g_mutex))
	{
		// rand
		rand = tb_rand_linear_uint32(g_rand, b, e);

		// unlock
		tb_mutex_leave(g_mutex);
	}

	return rand;
}
tb_sint32_t tb_rand_sint32(tb_sint32_t b, tb_sint32_t e)
{
	// check 
	tb_sint32_t rand = b;
	tb_assert_and_check_return_val(g_rand && g_mutex, rand);

	// lock
	if (tb_mutex_enter(g_mutex))
	{
		// rand
		rand = tb_rand_linear_sint32(g_rand, b, e);

		// unlock
		tb_mutex_leave(g_mutex);
	}

	return rand;
}

#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_rand_float(tb_float_t b, tb_float_t e)
{
	// check 
	tb_float_t rand = b;
	tb_assert_and_check_return_val(g_rand && g_mutex, rand);

	// lock
	if (tb_mutex_enter(g_mutex))
	{
		// rand
		rand = tb_rand_linear_float(g_rand, b, e);

		// unlock
		tb_mutex_leave(g_mutex);
	}

	return rand;
}
#endif

// the linear rand
tb_rand_linear_t* tb_rand_linear_init(tb_size_t seed)
{
	// alloc rand
	tb_rand_linear_t* rand = tb_calloc(1, sizeof(tb_rand_linear_t));
	tb_assert_and_check_return_val(rand, TB_NULL);

	// init rand
	rand->seed = seed;
	rand->data = seed;

	return rand;
}
tb_void_t tb_rand_linear_exit(tb_rand_linear_t* rand)
{
	if (rand) tb_free(rand);
}
tb_uint32_t tb_rand_linear_uint32(tb_rand_linear_t* rand, tb_uint32_t b, tb_uint32_t e)
{
	tb_assert_and_check_return_val(e > b, 0);
	return (b + (tb_rand_linear_next_uint32(rand) % (e - b)));
}
tb_sint32_t tb_rand_linear_sint32(tb_rand_linear_t* rand, tb_sint32_t b, tb_sint32_t e)
{
	tb_assert_and_check_return_val(e > b, 0);
	return (b + (tb_sint32_t)(tb_rand_linear_next_uint32(rand) % (e - b)));
}

#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_rand_linear_float(tb_rand_linear_t* rand, tb_float_t b, tb_float_t e)
{
	tb_assert_and_check_return_val(e > b, 0);
	return (b + (((tb_float_t)tb_rand_linear_next_uint32(rand) * (e - b)) / TB_MAXU32));
}
#endif

